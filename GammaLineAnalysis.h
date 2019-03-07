

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
        GammaLineAnalysis( std::string logDir = ".",
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
            
            
        void RegisterSpectrum(TString name, GammaSpectrum* spectrum, double constRes)
        {  TF1* resolution = new TF1("res","[0]",0,10000); //add in destructor!!!!
            resolution->SetParameter(0,constRes);
            fSpectra->insert({name,{spectrum,resolution}}); }
        void RegisterLine(TString name, vector<double> peakPos, pair<double,double> range)
        { fLines->insert({name,{peakPos,range}});         };
        
        void RegisterStandardLines();
        void PerformFits(TF1* resCurve, bool ifresolpr = true );
        void RegisterPseudoData(int iter, string path, double energy){
            m_pseudo = true;
            m_pseudo_iter = iter;
            m_pseudo_path = path;
            m_pseudo_energy = energy;
        }

    private:
        map<TString,AnalysisSpectrum>* fSpectra;
        map<TString,GammaRegion>*      fLines;
        GammaLineFit*                  fFitter;
        BCEngineMCMC::Precision        fPrecision;
        TString                        fLogDir;
        std::string                     fDS;

        bool m_pseudo = false;
        double m_pseudo_energy;
        int m_pseudo_iter;
        string m_pseudo_path = "";
    };

} // namespace std

#endif


