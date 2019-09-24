

#ifndef GammaLineAnalysis_H
#define GammaLineAnalysis_H

//root
#include "TH1D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"

// c++
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>

// BAT
#include <BAT/BCLog.h>
#include <BAT/BCAux.h>
#include <BAT/BCHistogramFitter.h>
#include <BAT/BCH1D.h>
#include <BAT/BCParameter.h>

#include "GammaLineFit.h"
#include "GammaSpectrum.h"

namespace std {

    class GammaLineAnalysis
    {
    public:
        struct AnalysisSpectrum{
           GammaSpectrum* spectrum;
            TF1* resolution;
        };
        struct GammaRegion{
            vector<double> peakPos;
            pair<double,double> range;
        };

        //! Constructor
        GammaLineAnalysis(std::string ds , std::string lar, std::string logDir = ".",std::string gfile = "", std::string sumfile = "", double b_w=1., 
                           BCEngineMCMC::Precision precision = BCEngineMCMC::kLow );

        //! Destructor
        virtual ~GammaLineAnalysis();

        //! Getters
        BCEngineMCMC::Precision  GetPrecision()               { return fPrecision;    };
        TString                  GetLogDir()                  { return fLogDir;       };
        GammaLineFit*            GetFitter()                  { return fFitter;       };

        //! Setters
        void  SetPrecision(BCEngineMCMC::Precision precision) { fPrecision=precision; };
        void  SetLogDir(TString logDir)                       { fLogDir=logDir;       };

        //! Methods
                //for (auto &kv : specs) gg->RegisterSpectrum(kv->GetName(), kv, resCurve);
        void RegisterSpectrum(TString name, GammaSpectrum* spectrum, TF1* resolution)
        { fSpectra->insert({name,{spectrum,resolution}});
        };
            
            
        void RegisterSpectrum(TString name, GammaSpectrum* spectrum, double constRes){
            TF1* resolution = new TF1("res","[0]",0,10000); //add in destructor!!!!
            resolution->SetParameter(0,constRes);
            fSpectra->insert({name,{spectrum,resolution}});
        }

        void RegisterLine(TString name, vector<double> peakPos, pair<double,double> range)
        { fLines->insert({name,{peakPos,range}});         };
        void RegisterStandardLines(double woi,  TF1* resCurve);
        TH1D* GetHistogramForFit(bool ifgammaanalysis);

        void PerformFits(TF1* resCurve, string priors_file, bool ifpriors=false, bool ifgammas = false);
        void RegisterPseudoData(int iter, string path, double energy, string pseudo_file=""){
            m_pseudo = true;
            m_pseudo_iter = iter;
            m_pseudo_path = path;
            m_pseudo_energy = energy;
            m_pseudo_file = pseudo_file;
        }
	        
	std::vector<double> GetCloseLinesToLine(double energy,  double & woi, double fwhm);
    private:
        string m_pseudo_file;
		std::vector<std::string> fisotopes;
		std::vector<std::vector<double>> fglines;
        TH1D* GetHistFromTree(TFile *f);
        map<TString,AnalysisSpectrum>* fSpectra;
        map<TString,GammaRegion>*      fLines;
        GammaLineFit*                  fFitter;
        BCEngineMCMC::Precision        fPrecision;
        TString                        fLogDir;
        std::string                     fDS;
	
	bool fPhase = false;
        bool m_pseudo = false;
        double m_pseudo_energy;
        int m_pseudo_iter;
        string m_pseudo_path = "";
		std::string fgfile;
		std::string fSumFile;
		void ReadLines();
		std::string fLar ="";
		double fSpectraBinning = 0.;

    };

} // namespace std

#endif


