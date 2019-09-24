#include "GammaLineAnalysis.h"
#include "TFile.h"

#include <fstream>

#include <sstream>
#include <string>


/*Class responsible for handling the input data and creating proper hists for the analysis;
 * estimated the size of the fit window depending on the presence of the bkg gamma lines in the vicinity of the analysed energy
 * */
namespace std {

	GammaLineAnalysis::GammaLineAnalysis( std::string ds ,string larmode, string log_ds, string gfile, string sumfile, double b_w,
										  BCEngineMCMC::Precision precision):
			fLar(larmode), fLogDir(log_ds), fPrecision(precision), fDS(ds), fgfile(gfile), fSumFile(sumfile), fSpectraBinning(b_w){
		fSpectra = new map<TString,AnalysisSpectrum>;
		fLines   = new map<TString,GammaRegion>;
		fFitter  = new GammaLineFit();
		ReadLines();
	}

	GammaLineAnalysis::~GammaLineAnalysis(){
		delete fSpectra;
		delete fLines;
		delete fFitter;
	}


	TH1D*  GammaLineAnalysis::GetHistFromTree(TFile *f){
		TH1D*  h = new TH1D("h", "h", 8000/fSpectraBinning, 0, 8000);
		TTree *tier4 = (TTree*)f->Get("tier4");

		//entries of the tier4 tree
		std::vector<double>* energy = new std::vector<double>();
		std::vector<int>* dsID = new std::vector<int>();
		int  isMV ;
		int  mult;
		int  tp ;
		int  bl ;
		int  larvetod ;

		tier4->SetBranchAddress( "energy", &energy );
		tier4->SetBranchAddress( "datasetID", &dsID );
		tier4->SetBranchAddress( "isMuVetoed", &isMV );
		tier4->SetBranchAddress( "multiplicity", &mult );
		tier4->SetBranchAddress( "isTP", &tp );
		tier4->SetBranchAddress( "isBL", &bl );

		if(fLar.find("LAr") != std::string::npos){
			std::cout << fLar << std::endl;
			tier4->SetBranchAddress( "isLArVetoed", &larvetod );
		}

		int coaxid = 5; // In the latest summary file, full coax dataset has id 5
		if(fPhase) coaxid = 1;

		std::cout << "creating hist for the " << fDS << std::endl;
		for( int i=0; i<tier4->GetEntries(); i++ ){
			tier4->GetEntry(i);


			if(mult>1 || tp == 1|| bl ==1 || isMV ==1 ) continue;
			if(fLar.find("LArAC") != std::string::npos and larvetod ==1) continue;
			else if (fLar.find("LArC") != std::string::npos and larvetod ==0)  continue;

			int idx_en = 0; // idx of the detector with non-zero energy entry
			for (int j = 0; j<energy->size(); j++){
				if (energy->at(j)!=0) {
					idx_en = j;
				}
			}
			if(fDS.find("Coax") != std::string::npos && (dsID->at(idx_en) == 5 || dsID->at(idx_en) == 1)){
				h->Fill(energy->at(idx_en));
			}
			else if (fDS.find("BEGe") !=std::string::npos and dsID->at(idx_en) == 0){
				h->Fill(energy->at(idx_en));
			}
			else if(fDS.find("BEGe") == std::string::npos &&fDS.find("Coax") == std::string::npos  && dsID->at(idx_en) ==3){
				h->Fill(energy->at(idx_en));
			}
		}
		std::ofstream of;
		char txt_file[500];

		//saving hist for plotting
		sprintf(txt_file, "hist_%s_basic",fDS.c_str() );
        of.open(txt_file, std::ios_base::app);
        of << "Bin center  " << endl;
		for(int i =0; i < h->GetNbinsX(); i++){
			of << h->GetBinCenter(i) << "\t";
		}
		of<<endl;
		of << "Bin content  " << endl;
		for(int i =0; i < h->GetNbinsX(); i++){
			of << h->GetBinContent(i) << "\t";
		}
		of<<endl;
		return h;
	}

TH1D* GammaLineAnalysis::GetHistogramForFit(bool ifgammas){
	TH1D *h = new TH1D();

	if (m_pseudo) { // if running tests on pseudo-data
		char path[500];
		if (m_pseudo_file == "") // not empty in case run on the data not produced by the standard gerda sw
			m_pseudo_file = "datafake-bkg-model-v1.0"; // unchangeble part of the pseudo-data file name,
		// as it is created by the fake data generator program

		sprintf(path, "%s/%s_%d.root", m_pseudo_path.c_str(), m_pseudo_file.c_str(),
				m_pseudo_iter); // total path of the pseudo data file
		TFile *f = new TFile(path);
		if (fDS.find("Coax") != std::string::npos)
			h = (TH1D *) f->Get("M1_enrCoax");
		else if (fDS.find("BEGe") != std::string::npos) {
			h = (TH1D *) f->Get("M1_enrBEGe");
		} else if (fDS.find("Nat") != std::string::npos)
			h = (TH1D *) f->Get("M1_natCoax");
		else
			h = (TH1D *) f->Get("h1f");
		TCanvas *c = new TCanvas();
		h->Draw();
		fFitter->SetHist(h);
	} else {
		TFile *f = new TFile(fSumFile.c_str()); // gerda summary file
		if (fSumFile.find("PhaseIIa") != std::string::npos || fSumFile.find("PhaseIIb") != std::string::npos)
			fPhase = true; // will be needed to correctly set dataset id for the coax in  GammaLineAnalysis::GetHistFromTree
		if (!ifgammas) // in case analysis is run for gammas diffenent settings of the hist
		{
			if (fDS.find("Coax") != std::string::npos) {
				h = (TH1D *) f->Get("h"); // in case of coax dataset the historgram is a combination of the two from the summary file -> therefore,
				// I create another sum file, just for the coax ds//
			} else if (fDS.find("BEGe") != std::string::npos)
				h = (TH1D *) f->Get("hRaw0");
			else {//natural datasset
				h = (TH1D *) f->Get("hRaw3");
			}
		} else {
			// to perform the gamma analysis I was creating custom made summary hist, which kept in a private dir
			char root_hist[5000];
			string ph = "PhaseIIc";
			if (fSumFile.find("PhaseIIa") != std::string::npos)
				ph = "PhaseIIa";
			else if (fSumFile.find("PhaseIIb") != std::string::npos)
				ph = "PhaseIIb";

			sprintf(root_hist, "/disk/data1/atp/rizalina/gammas_input/COaxPhaseII/%s%s%s.root", ph.c_str(),
					fDS.c_str(), fLar.c_str()); // path of the custom made summary histogram

			TFile *ftest = new TFile(root_hist);
			h = (TH1D *) ftest->Get("h");
			if (!h) {// if the custom made hist wasn't found - > create one, and save for future computations
				h = GetHistFromTree(f);
				ftest = new TFile(root_hist, "recreate");
				h->Write();
				ftest->Close();
			}
		}
	}
	return h;
}

	void GammaLineAnalysis::PerformFits(TF1* resCurve, std::string priors_file, bool ifpriors, bool ifgammas){
		cout << "prior file" << priors_file << endl; // just as a check printout of the file with the precomputed priors

		fFitter->SetPrecision(fPrecision);

		TH1D * h = GetHistogramForFit(ifgammas);
		fFitter->SetHist(h); // set hist for the BAT fit

		for(auto& line : *(fLines)) {     //loop over lines
			for(auto& spectr : *(fSpectra)) { //loop over spectra
				fFitter->SetRes(resCurve);
				fFitter->SetPeakPos(line.second.peakPos);
				fFitter->SetRange(line.second.range);

				fFitter->Fit(ifpriors, priors_file, fLogDir.Data(), fDS,  Form("%s/%s-%s",fLogDir.Data(), spectr.first.Data(),line.first.Data()), ifgammas);
				fFitter->WriteToCommonLog(Form("%s-%s",
											   spectr.first.Data(),line.first.Data()),
										  Form("%s/gamma.log",fLogDir.Data()),
										  spectr.second.spectrum->GetExposure(), ifgammas, fDS);

				if(!m_pseudo )
					//!Save bat output to the txt for future plotting w/Py
					fFitter->SaveFit( Form("%s/%s-%s",fLogDir.Data(), spectr.first.Data(),line.first.Data()));
			}
		}
		delete h;
	}

	void GammaLineAnalysis::RegisterStandardLines(double woi, TF1* resCurve){
		for(int i=0; i < fisotopes.size(); i++){
			for(int j = 0; j<fglines[i].size(); j++){
				double woi_current = woi;
				char acname[500];
				sprintf(acname, "%s_%d",fisotopes[i].c_str(), int(fglines[i][j]));
				vector<double> peaks = GetCloseLinesToLine(fglines[i][j], woi_current, resCurve->Eval(fglines[i][j]));
				for(int p=0; p<peaks.size(); p++)
					std::cout <<peaks[p]<< " ";
				RegisterLine(acname ,peaks,{ int(fglines[i][j] -woi) ,int(fglines[i][j] + woi)});
			}
		}
	}

	void GammaLineAnalysis::ReadLines(){ // Read list with gamma bkg lines
		std::ifstream infile(fgfile);
		std::string line;
		while (std::getline(infile, line)){
			if(line.size() == 0)continue;

			std::string delimiter = ":";
			std::string iso = line.substr(0, line.find(delimiter)); //
			std::string energies = line.substr(line.find(delimiter)+1); //
			fisotopes.push_back(iso);
			std::vector<double> gamma_energies;
			stringstream ss( energies);
			while( ss.good() )
			{
				string substr;
				getline( ss, substr, ',' );
				if(substr.size()){
					gamma_energies.push_back(std::stod(substr));
				}
			}
			fglines.push_back(gamma_energies);
		}
	}

std::vector<double> GammaLineAnalysis::GetCloseLinesToLine(double energy, double & woi, double fwhm){
		// a function to find the closest gamma bkg lines to the fitted energy, and modify the fit window if needed
		// (see my thesis for more info)
		// Perhaps, this function is far from optimum solution, just written on the back of the envelope
		std::vector<double> peaks_pos;
		peaks_pos.push_back(energy);

		int add_size = ceil(fwhm);

		for(int is = 0; is<fisotopes.size(); is++){
			for (int jen = 0; jen<fglines[is].size(); jen++){
				if(fglines[is][jen] >= energy - woi +2*fwhm &&
				   fglines[is][jen] <= energy + woi - 2*fwhm && fglines[is][jen]!=energy && fglines[is][jen]){
					std::cout << "the closest line " << fglines[is][jen] << std::endl;
					std::vector<double>::iterator it = std::find(peaks_pos.begin(), peaks_pos.end(), fglines[is][jen]);
					if (it == peaks_pos.end()){
						bool add = true;
						for(int p = 0; p<peaks_pos.size(); p++){
							if(abs(fglines[is][jen] - peaks_pos[p]) < 1)
								add =false;
						}
						if(add) peaks_pos.push_back(fglines[is][jen]);
					}
					else
						continue;
				}else if(fglines[is][jen] - (energy + woi) < 2*fwhm && fglines[is][jen] - energy - woi >= 0) {
					std::cout << "adjsuting window; case 1 " <<fglines[is][jen] <<  std::endl;
					cout << fwhm << endl;
					cout << energy-woi << "\t" << energy+woi  <<endl;
					woi-=add_size;
				}
				else if (abs(fglines[is][jen] - energy - woi) <= 2*fwhm && fglines[is][jen] < energy + woi || fglines[is][jen] - (energy -woi) < 2* fwhm && fglines[is][jen] - (energy -woi) >0 ){
					std::cout << "adjsuting window; case 2 and 4" << std::endl;
					std::vector<double>::iterator it = std::find(peaks_pos.begin(), peaks_pos.end(), fglines[is][jen]);
					if (it == peaks_pos.end()){
						bool add = true;
						for(int p = 0; p<peaks_pos.size(); p++){
							if(abs(fglines[is][jen] - peaks_pos[p]) < 1)
								add =false;
						}
						if(add) peaks_pos.push_back(fglines[is][jen]);
					}
					woi+=add_size;
				}
				else if(energy - woi - fglines[is][jen] <= 2*fwhm && energy - woi - fglines[is][jen] >=0) {
					std::cout << "adjsuting window; case 3" << fglines[is][jen] <<std::endl;
					woi-=add_size;
				}
			}
		}
		std::cout << "woi" << woi << std::endl;
		return  peaks_pos;
	}
} // namespace

