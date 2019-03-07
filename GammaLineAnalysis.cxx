#include "GammaLineAnalysis.h"
#include "TFile.h"

#include <fstream>

#include <sstream>
#include <string>

namespace std {

    GammaLineAnalysis::GammaLineAnalysis( string log_ds,
                                          BCEngineMCMC::Precision precision ) :
            fLogDir(log_ds), fPrecision(precision), fDS(log_ds)
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
        for(auto& line : *(fLines)) {     //loop over lines
			
            for(auto& spectr : *(fSpectra)) { //loop over spectra
                if(m_pseudo){
                    
                    char path[500];
                    sprintf(path, "%s/datafake-bkg-model-v1.0_%d.root", m_pseudo_path.c_str(), m_pseudo_iter);
                    
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
					TH1D *h  = new TH1D();
					TFile *f = new TFile("/nfs/gerda2/PhaseIIsummary/PhaseII53-92_orig.root");
					if (fDS.find("Coax") != std::string::npos)
						h = (TH1D*)f->Get("hRaw5");
					else if (fDS.find("BEGe") !=std::string::npos)
						h = (TH1D*)f->Get("hRaw0"); 
					else
						h = (TH1D*)f->Get("hRaw5");
					fFitter->SetHist(h);
					std::cout << "nbins in hist " << h->GetNbinsX() << std::endl;
				//delete f;
				//delete h;
				}
				
				
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
			std::cout << fisotopes[i] << "\t";
		
			for(int j = 0; j<fglines[i].size(); j++){
				double woi_current = woi;
				char acname[500];
				sprintf(acname, "%s_%d",fisotopes[i].c_str(), int(fglines[i][j]) );
				std::cout << acname << "\t";
				
				vector<double> peaks = GetCloseLinesToLine(fglines[i][j], woi_current);
				//gg->RegisterLine(line, peaks,{energy - woi,energy+woi});
				std::cout <<  "RegisterLine(" << acname <<",{";
				for(int p=0; p<peaks.size(); p++)
					std::cout <<peaks[p]<< " ";
				std::cout <<"},{ "<<int(fglines[i][j] -woi_current) <<"," << int(fglines[i][j] + woi_current) <<" })" << std::endl; 
				
				RegisterLine(acname ,peaks,{ int(fglines[i][j] -woi) ,int(fglines[i][j] + woi)}); 
				
			}
			std::cout << std::endl;
		}
		
    }



 void GammaLineAnalysis::ReadLines(){
	
	std::ifstream infile("/nfs/gerda2/users/rizalinko/gamma-analysis/g_lines.txt");
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



std::vector<double> GammaLineAnalysis::GetCloseLinesToLine(double energy, double & woi){
	std::vector<double> peaks_pos;
	
	peaks_pos.push_back(energy);    
	for(int is = 0; is<fisotopes.size(); is++){
			for (int jen = 0; jen<fglines[is].size(); jen++){
				if(fglines[is][jen] > energy - woi && fglines[is][jen] < energy + woi && fglines[is][jen]!=energy){
					std::cout << "the closest line " << fglines[is][jen] << std::endl; 
					std::vector<double>::iterator it = std::find(peaks_pos.begin(), peaks_pos.end(), fglines[is][jen]);
					if (it == peaks_pos.end())
						peaks_pos.push_back(fglines[is][jen]);
					else 
						continue;
				}
				else if(fglines[is][jen] - energy - woi < 4 && fglines[is][jen] - energy - woi > 0 ) {
					std::cout << "woi was changed" << std::endl;
					std::cout << energy << " is close to " << fglines[is][jen] << std::endl;
					woi-=2;
				}
				else if(energy - woi - fglines[is][jen] < 4 && energy - woi - fglines[is][jen] >0) {
					std::cout << "woi was changed" << std::endl;
					std::cout << energy << " is close to " << fglines[is][jen] << std::endl;
					woi-=2;
				}
			
		}
	}
	std::cout << "woi" << woi << std::endl;
	return  peaks_pos;
}
} // namespace

