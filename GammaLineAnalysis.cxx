#include "GammaLineAnalysis.h"
#include "TFile.h"

#include <fstream>

#include <sstream>
#include <string>

namespace std {

    GammaLineAnalysis::GammaLineAnalysis( string larmode, string log_ds, string gfile, string sumfile, double b_w,
                                          BCEngineMCMC::Precision precision ) :
            fLar(larmode), fLogDir(log_ds), fPrecision(precision), fDS(log_ds), fgfile(gfile), fSumFile(sumfile), fSpectraBinning(b_w)
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

    void GammaLineAnalysis::PerformFits(TF1* resCurve, bool ifresolprior, bool ifgammas )
    {
        fFitter->SetPrecision(fPrecision);
        if(!ifresolprior)
            fFitter->SwitchOffPriors();

		if(m_pseudo){

			char path[500];
			sprintf(path, "%s/data_fake_%d.root", m_pseudo_path.c_str(), m_pseudo_iter);

			TFile *f = new TFile(path);
			char hname[100];

			TH1D *h  = new TH1D();
			//sprintf(hname, "energy_%0.f_keV", m_pseudo_energy);
			if (fDS.find("Coax") != std::string::npos)
				h = (TH1D*)f->Get("M1_enrCoax");
			else if (fDS.find("BEGe") !=std::string::npos)
				h = (TH1D*)f->Get("M1_enrBEGe");
			else
				h = (TH1D*)f->Get("M1_natCoax");
			//fFitter->SetHist((TH1D*) spectr.second.spectrum);
			fFitter->SetHist(h);

			//delete f;
			//delete h;


		}
		else
		{
			TH1D *h = new TH1D("h", "h", 8000/fSpectraBinning, 0, 8000);
			TFile *f = new TFile(fSumFile.c_str());
			if(! ifgammas)
			{
				if (fDS.find("Coax") != std::string::npos)
					h = (TH1D*)f->Get("hRaw5");
				else if (fDS.find("BEGe") !=std::string::npos)
					h = (TH1D*)f->Get("hRaw0");
				else
					h = (TH1D*)f->Get("hRaw5");
			}
			if(ifgammas){
				std::cout << "taking hist from tree" << std::endl;
				TTree *tier4 = (TTree*)f->Get("tier4");
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
				//if(h->GetEntries )
				/*
                if (fDS.find("Coax") != std::string::npos)
                    tier4->Draw("energy>>h", "datasetID == 5 && !isBL && !isMuVetoed && multiplicity ==1 && !isTP");
                else if (fDS.find("BEGe") !=std::string::npos)
                    tier4->Draw("energy>>h", "datasetID == 0 && !isBL && !isMuVetoed && multiplicity ==1 && !isTP");
                else
                    tier4->Draw("energy>>h", "datasetID == 3 && !isBL && !isMuVetoed && multiplicity ==1 && !isTP");*/
			}
			//for(int i=50; i<150; i++) std::cout <<  h->GetBinCenter(i) << "\t" << h->GetBinContent(i) << "\t";
			TCanvas * c = new TCanvas();
			h->Draw();
			c->SaveAs("histfromtree.root");
			fFitter->SetHist(h);
			std::cout << "nbins in hist " << h->GetNbinsX() << std::endl;
			//delete f;
			//delete h;
		}
        for(auto& line : *(fLines)) {     //loop over lines
			
            for(auto& spectr : *(fSpectra)) { //loop over spectra

				
				
                fFitter->SetRes(resCurve);
                std:;cout << "fFitter->SetRes(resCurve);" << std::endl;
                fFitter->SetPeakPos(line.second.peakPos);
                fFitter->SetRange(line.second.range);
                
                std::cout << "fFitter->SetRange(line.second.range);" << std::endl;
                /*
                if( std::get<0>(line.second.range) < 400){
			         fFitter->SetLinBkg(true);
				}*/
                 
                if(fFitter->Fit(Form("%s/%s-%s",fLogDir.Data(), spectr.first.Data(),line.first.Data()), ifgammas)){
					std::cout << "fFitter->Fit();" << std::endl;
									  
									  
							  
					fFitter->WriteToCommonLog(Form("%s-%s",
												   spectr.first.Data(),line.first.Data()),
											  Form("%s/gamma.log",fLogDir.Data()),
											  spectr.second.spectrum->GetExposure(), ifgammas);
				}
						
            }
       }
    }	

    void GammaLineAnalysis::RegisterStandardLines(double woi)
    {
		
		for(int i=0; i < fisotopes.size(); i++){
			//std::cout << fisotopes[i] << "\t";
		
			for(int j = 0; j<fglines[i].size(); j++){
				double woi_current = woi;
				char acname[500];
				sprintf(acname, "%s_%d",fisotopes[i].c_str(), int(fglines[i][j]) );
				//std::cout << acname << "\t";
				
				vector<double> peaks = GetCloseLinesToLine(fglines[i][j], woi_current);
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
				std::cout << "converting into double" << substr << std::endl;
				gamma_energies.push_back(std::stod(substr));
			}			
		}
		fglines.push_back(gamma_energies);

		}
	}



std::vector<double> GammaLineAnalysis::GetCloseLinesToLine(double energy, double & woi){
	std::vector<double> peaks_pos;
	
	peaks_pos.push_back(energy);    
	for(int is = 0; is<fisotopes.size(); is++){
			for (int jen = 0; jen<fglines[is].size(); jen++){
				if(fglines[is][jen] > energy - woi +3 && fglines[is][jen] < energy + woi -3 && fglines[is][jen]!=energy && fglines[is][jen]){
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
				// right edge
				
				else if(fglines[is][jen] - energy - woi < 4 && fglines[is][jen] - energy - woi > 0) {
					//std::cout << "woi was changed" << std::endl;
					std::cout << energy << " is close to " << fglines[is][jen] << std::endl;
					woi-=2;
				}else if (fglines[is][jen] - energy - woi < 4 && fglines[is][jen] - energy - woi >= -3){
					//std::cout << "woi was changed" << std::endl;
					std::cout << energy << " is close to " << fglines[is][jen] << std::endl;
					peaks_pos.push_back(fglines[is][jen]);
					woi+=2;
				}
				else if(energy - woi - fglines[is][jen] < 4 && energy - woi - fglines[is][jen] >0) {
					//std::cout << "woi was changed" << std::endl;
					std::cout << energy << " is close to " << fglines[is][jen] << std::endl;
					woi-=2;
				}else if(energy - woi - fglines[is][jen] < 4 && energy - woi - fglines[is][jen] >=-3) {
					//std::cout << "woi was changed" << std::endl;
					std::cout << energy << " is close to " << fglines[is][jen] << std::endl;
					peaks_pos.push_back(fglines[is][jen]);
					woi+=2;
				}
		}
	}
	std::cout << "woi" << woi << std::endl;
	return  peaks_pos;
}
} // namespace

