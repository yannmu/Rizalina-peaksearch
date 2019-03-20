#include "GammaLineAnalysis.h"
#include "TFile.h"

#include <fstream>

#include <sstream>
#include <string>

namespace std {

    GammaLineAnalysis::GammaLineAnalysis(std::string ds ,string larmode, string log_ds, string gfile, string sumfile, double b_w,
                                          BCEngineMCMC::Precision precision ) :
            fLar(larmode), fLogDir(log_ds), fPrecision(precision), fDS(ds), fgfile(gfile), fSumFile(sumfile), fSpectraBinning(b_w)
    {

        fSpectra = new map<TString,AnalysisSpectrum>;
        fLines   = new map<TString,GammaRegion>;
        fFitter  = new GammaLineFit();
        ReadLines();
    }

    GammaLineAnalysis::~GammaLineAnalysis()
    {
        delete fSpectra;
        delete fLines;
        delete fFitter;
    }


TH1D*  GammaLineAnalysis::GetHistFromTree(TFile *f){
      TH1D*  h = new TH1D("h", "h", 8000/fSpectraBinning, 0, 8000);
        TTree *tier4 = (TTree*)f->Get("tier4");
        std::vector<double>* energy = new std::vector<double>();
        std::vector<int>* dsID = new std::vector<int>();
        int  isMV ;
        int  mult;
        int  tp ;
        int  bl ;
        int  larvetod ;
	std::cout << "creating hist for the " << fDS << std::endl;
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
        for( int i=0; i<tier4->GetEntries(); i++ ){
            tier4->GetEntry(i);

            int idx_en = 0;
            if(mult>1 || tp == 1|| bl ==1 || isMV ==1 ) continue;
            if(fLar.find("LArAC") != std::string::npos and larvetod ==1) continue;
            else if (fLar.find("LArC") != std::string::npos and larvetod ==0)  continue;
            for (int j = 0; j<energy->size(); j++){
                if (energy->at(j)!=0) idx_en = j;
            }
            if(fDS.find("Coax") != std::string::npos && dsID->at(idx_en) == 5){
                h->Fill(energy->at(idx_en));
            }
            else if (fDS.find("BEGe") !=std::string::npos and dsID->at(idx_en) == 0){
                h->Fill(energy->at(idx_en));
            }
            else if(fDS.find("BEGe") == std::string::npos &&fDS.find("Coax") == std::string::npos  && dsID->at(idx_en) ==3){
                h->Fill(energy->at(idx_en));
            }
        }
    return h;
    }
    void GammaLineAnalysis::PerformFits(TF1* resCurve, bool ifresolprior, bool ifgammas, bool iftest )
    {

        fFitter->SetPrecision(fPrecision);
        if(!ifresolprior)
            fFitter->SwitchOffPriors();
		TH1D *h  = new TH1D();

		if(m_pseudo){

			char path[500];
			sprintf(path, "%s/data_fake_%d.root", m_pseudo_path.c_str(), m_pseudo_iter);

			TFile *f = new TFile(path);
			char hname[100];
			//sprintf(hname, "energy_%0.f_keV", m_pseudo_energy);
			if (fDS.find("Coax") != std::string::npos)
				h = (TH1D*)f->Get("M1_enrCoax");
			else if (fDS.find("BEGe") !=std::string::npos)
				h = (TH1D*)f->Get("M1_enrBEGe");
			else
				h = (TH1D*)f->Get("M1_natCoax");
			//fFitter->SetHist((TH1D*) spectr.second.spectrum);
			fFitter->SetHist(h);
        }
		else
		{
			TFile *f = new TFile(fSumFile.c_str());
    		if(! ifgammas)// or fSpectraBinning == 1)
			{
				h = new TH1D("h", "h", 8000/fSpectraBinning, 0, 8000);
				if (fDS.find("Coax") != std::string::npos)
					h = (TH1D*)f->Get("hRaw5");
				else if (fDS.find("BEGe") !=std::string::npos)
					h = (TH1D*)f->Get("hRaw0");
				else
					h = (TH1D*)f->Get("hRaw5");
			}
			else if(ifgammas && !iftest){
                h = GetHistFromTree(f);
				TH1D* hraw5 = (TH1D*)f->Get("hRaw0");

				hraw5->SetLineColor(kRed);
				TFile *ftest = new TFile("test_hist_b1.root", "recreate");
				h->Write();
				hraw5->Write();
				ftest->Close();
			}else if(iftest){
				std::cout << "TEST" << std::endl;
				//TH1D *h = new TH1D("h", "h", 8000/fSpectraBinning, 0, 8000);
				char hitsfilename[500];
				sprintf(hitsfilename,"/nfs/gerda2/users/rizalinko/gamma-analysis/%s_hist_b03.root", fDS.c_str() );
				TFile *f = new TFile(hitsfilename);
				h= (TH1D*)f->Get("h");
				TCanvas *c = new TCanvas();
				h->Draw();
				c->SaveAs("tests_hist.png");
				
            }
		}
		fFitter->SetHist(h);

		for(auto& line : *(fLines)) {     //loop over lines
            for(auto& spectr : *(fSpectra)) { //loop over spectra

				bool igl = false;				

				fFitter->SetRes(resCurve);
				// std:;cout << "fFitter->SetRes(resCurve);" << std::endl;
			for(int i =0;i<fglines.size(); i++){
				for(int j = 0; j<fglines[i].size(); j++){
					for (int k =0; k<line.second.peakPos.size(); k++)
						if(abs(line.second.peakPos[k] - fglines[i][j])<1) igl = true; 
				}
			}
			//std::cout<<"is ga line" << igl << std::endl;
			fFitter->SetPeakPos(line.second.peakPos, igl);
			fFitter->SetRange(line.second.range);
     

			if(fFitter->Fit(Form("%s/%s-%s",fLogDir.Data(), spectr.first.Data(),line.first.Data()), ifgammas)){
			//std::cout << "fFitter->Fit();" << std::endl;				  
							  
					  
			fFitter->WriteToCommonLog(Form("%s-%s",
										   spectr.first.Data(),line.first.Data()),
									  Form("%s/gamma.log",fLogDir.Data()),
									  spectr.second.spectrum->GetExposure(), ifgammas);
				}
						
            }
       }
    }	

    void GammaLineAnalysis::RegisterStandardLines(double woi, TF1* resCurve)
    {
		
		for(int i=0; i < fisotopes.size(); i++){
			//std::cout << fisotopes[i] << "\t";
		
			for(int j = 0; j<fglines[i].size(); j++){
				double woi_current = woi;
				char acname[500];
				sprintf(acname, "%s_%d",fisotopes[i].c_str(), int(fglines[i][j]) );
				//std::cout << acname << "\t";
				std::cout << " REAL FWHM HERE" << resCurve->Eval(fglines[i][j]) << std::endl;
				vector<double> peaks = GetCloseLinesToLine(fglines[i][j], woi_current, resCurve->Eval(fglines[i][j]));
				//gg->RegisterLine(line, peaks,{energy - woi,energy+woi});
				//std::cout <<  "RegisterLine(" << acname <<",{";
				for(int p=0; p<peaks.size(); p++)
					std::cout <<peaks[p]<< " ";
				//std::cout <<"},{ "<<int(fglines[i][j] -woi_current) <<"," << int(fglines[i][j] + woi_current) <<" })" << std::endl; 
				
				RegisterLine(acname ,peaks,{ int(fglines[i][j] -woi) ,int(fglines[i][j] + woi)}); 
				
			}
			//std::cout << std::endl;
		}
		
    }



 void GammaLineAnalysis::ReadLines(){
	
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
//				std::cout << "converting into double" << substr << std::endl;
				gamma_energies.push_back(std::stod(substr));
			}			
		}
		fglines.push_back(gamma_energies);

		}
	}



std::vector<double> GammaLineAnalysis::GetCloseLinesToLine(double energy, double & woi, double fwhm){
	std::vector<double> peaks_pos;
	
	peaks_pos.push_back(energy);   

    int add_size = ceil(fwhm);
fwhm = ceil(fwhm);
  //                              std::cout << "add to woi" << add_size << std::endl;
 
	for(int is = 0; is<fisotopes.size(); is++){
			for (int jen = 0; jen<fglines[is].size(); jen++){
				if(fglines[is][jen] >= energy - woi +2*fwhm && fglines[is][jen] <= energy + woi - 2*fwhm && fglines[is][jen]!=energy && fglines[is][jen]){
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
				}
				// g line is close to the right edge
				// 1 - right outside of the fit window
				else if(fglines[is][jen] - energy - woi < 2*fwhm && fglines[is][jen] - energy - woi >= 0) {
					std::cout << "adjsuting window; case 1 " <<fglines[is][jen] <<  std::endl;
					woi-=add_size;
				}
				// 2 - right before the fit window
				else if (abs(fglines[is][jen] - energy - woi) <= 2*fwhm && fglines[is][jen] < energy + woi){
					std::cout << "adjsuting window; case 2" << std::endl;
                                       
					peaks_pos.push_back(fglines[is][jen]);
					woi+=add_size;
				}
				// from the left side
				//right outside of the window
				else if(energy - woi - fglines[is][jen] <= 2*fwhm && energy - woi - fglines[is][jen] >=0) {

				      std::cout << "adjsuting window; case 3" << std::endl;
                              		
					woi-=add_size;
				}

				// right before the fit window range
				else if(fglines[is][jen] - (energy -woi) < 2* fwhm && fglines[is][jen] - (energy -woi) >0 ) {
					//std::cout << "woi was changed" << std::endl;
				      std::cout << "adjsuting window; case 4" << std::endl;
                                       
					peaks_pos.push_back(fglines[is][jen]);
					woi+=add_size;
				}
		}
	}
	std::cout << "woi" << woi << std::endl;
	return  peaks_pos;
}
} // namespace

