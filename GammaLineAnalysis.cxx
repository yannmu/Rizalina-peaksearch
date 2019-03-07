#include "GammaLineAnalysis.h"
#include "TFile.h"

namespace std {

    GammaLineAnalysis::GammaLineAnalysis( string log_ds,
                                          BCEngineMCMC::Precision precision ) :
            fLogDir(log_ds), fPrecision(precision), fDS(log_ds)
    {

        fSpectra = new map<TString,AnalysisSpectrum>;
        fLines   = new map<TString,GammaRegion>;
        fFitter  = new GammaLineFit();
    }

    GammaLineAnalysis::~GammaLineAnalysis()
    {
        delete fSpectra;
        delete fLines;
        delete fFitter;
    }

    void GammaLineAnalysis::PerformFits(TF1* resCurve, bool ifresolprior )
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
                 
                fFitter->Fit(Form("%s/%s-%s",fLogDir.Data(), spectr.first.Data(),line.first.Data()));
                std::cout << "fFitter->Fit();" << std::endl;                         
				fFitter->WriteToCommonLog(Form("%s-%s",spectr.first.Data(),line.first.Data()),
                                          Form("%s/gamma.log",fLogDir.Data()),
                                          spectr.second.spectrum->GetExposure());
						//std::cout << "Form("%s-%s",spectr.first.Data(),line.first.Data())" << std::endl;
                        //            std::cout << "Form(\"%s/gamma.log\",fLogDir.Data())" << std::endl;
                                          
            }
       }
    }

    void GammaLineAnalysis::RegisterStandardLines()
    {
        RegisterLine("K42_1525"  ,{1524.7},{1495.,1555.});
        RegisterLine("K40_1461"  ,{1460.8},{1430.,1490.});

        //Th chain
        RegisterLine("Ac228_911" ,{ 911.2},{ 890., 935.}); /*25.8%*/
        RegisterLine("Ac228_969" ,{ 969.0},{ 950., 990.}); /*15.8%*/
        RegisterLine("Tl208_2614",{2614.5},{2590.,2640.}); /*99.8%*/
        RegisterLine("Tl208-583" ,{ 583.2},{ 575., 600.}); /*85.0%*/
        RegisterLine("Tl208_861", { 860.6},{ 840., 880.}); /*12.5%*/

        //U chain
        RegisterLine("Bi214_609" ,{ 609.3},{ 590., 630.}); /*45.5%*/
        RegisterLine("Bi214_1764",{1764.5},{1740.,1785.}); /*15.3%*/
        RegisterLine("Bi214_1120",{1120.3},{1100.,1140.}); /*14.9%*/
        RegisterLine("Bi214_1238",{1238.1},{1215.,1260.}); /* 5.8%*/
        RegisterLine("Bi214_2204",{2204.1},{2180.,2225.}); /* 4.9%*/
        RegisterLine("Bi214_768" ,{ 768.4},{ 745., 790.}); /* 4.9%*/
        RegisterLine("Bi214_1378",{1377.7},{1355.,1400.}); /* 4.0%*/
        RegisterLine("Bi214_934" ,{ 934.1},{ 915., 955.}); /* 3.1%*/

        RegisterLine("Co60_1332" ,{1332.5},{1310.,1355.}); /*99.9%*/
        RegisterLine("Co60_1173" ,{1173.2},{1150.,1195.}); /*100.%*/
        RegisterLine("Pa234_1001",{1001.0},{ 980.,1025.});
        RegisterLine("Bi207_1063",{1063.7},{1040.,1085.});
        RegisterLine("Bi207_570" ,{ 569.7},{ 540., 580.});
        RegisterLine("Kr85_514",  {514.0,511.0},{ 490., 530.});

        //these guys need linear bkg -> to be implemented
        RegisterLine("Pb214_352",{ 351.9},{ 330., 370.});
        RegisterLine("Pb214_295",{ 295.2},{ 275., 315.});
        RegisterLine("Pb212_239",{ 238.6},{ 220., 255.});
        //Ag108m 722.91, 614.28 and 433.94
    }

} // namespace


