//gamma line fit:
// - gaussian peak on (constant) background
// - gaussian prior on peakposition / resolution

#ifndef GammaLineFit_H
#define GammaLineFit_H

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

#include "GammaFitFunction.h"

namespace std {

class GammaLineFit
{
   public:
      //! constructors
      GammaLineFit() :
       fHistSet(false), fResSet(false), fRangeSet(false),
       fFitSet(false), fLinBkg(false), fPrecision(BCEngineMCMC::kLow),
       fNPeaks(0), fPeakPriorWidth(0.2), fFWHMPriorWidth(0.2) { };
      GammaLineFit(TH1D* hist, TF1* res, vector<double> peakPos,
       pair<double,double> range) : GammaLineFit() {
         SetHist(hist);
         SetRes(res);
         SetPeakPos(peakPos);
         SetRange(range); };
      GammaLineFit(TH1D* hist, double res, vector<double> peakPos,
       pair<double,double> range) : GammaLineFit() {
         SetHist(hist);
         SetRes(res);
         SetPeakPos(peakPos);
         SetRange(range); };
      //! destructor
      virtual ~GammaLineFit() {
         if(fHistSet) delete fHist;
         if(fResSet)  delete fRes;
         if(fFitSet) {
           delete fCanvas2;
           delete fCanvas1;
           delete fHistFitter;
           while(! fResPriors.empty()) {
              delete fResPriors.back();
              fResPriors.pop_back();
           }
           while(! fPeakPriors.empty()) {
             delete fPeakPriors.back();
             fPeakPriors.pop_back();
           }
           delete fFitHist;
           delete fFitFunction;
         }
      };

      //! getters
      TH1D*                    GetHist()                    { return fHist;           };
      TF1*                     GetRes()                     { return fRes;            };
      double                   GetPeakPos(int i=0)          { return fPeakPos.at(i);  };
      vector<double>           GetPeakPos()                 { return fPeakPos;        };
      pair<double,double>      GetRange()                   { return fRange;          };
      double                   GetPeakPriorWidth()          { return fPeakPriorWidth; };
      double                   GetFWHMPriorWidth()          { return fFWHMPriorWidth; };
      BCEngineMCMC::Precision  GetPrecision()               { return fPrecision;      };

      //! setters
      void  SetLinBkg(bool linBkg=true)                     { fLinBkg=linBkg;                 };
      void  SetPeakPriorWidth(double peakPriorWidth)        { fPeakPriorWidth=peakPriorWidth; };
      void  SetFWHMPriorWidth(double fWHMPriorWidth)        { fFWHMPriorWidth=fWHMPriorWidth; };
      void  SetPrecision(BCEngineMCMC::Precision precision) { fPrecision=precision;           };

      //! methods and special setters
      void  SetHist(TH1D* hist);
      void  SetRes(TF1* resCurve);
      void  SetRes(double constRes);
      void  SetPeakPos(vector<double> peakPos)  { fPeakPos=peakPos; fNPeaks=(int) peakPos.size(); };
      void  AddPeakPos(double peakPos)          { fPeakPos.push_back(peakPos); fNPeaks++;         };
      void  SetRange(pair<double,double> range) { fRange=range; fRangeSet=true;                   };
      bool  Fit(TString name = "fit", bool ifgammas = false);
      void  WriteToCommonLog(TString name, TString logFileName="gamma.log", double exposure=-1, bool ifgammas = false);

     void SwitchOffPriors(){fResolPrior = false;}
   protected:
      bool  ResetFit(TString, bool ifgammas = false );
      void  EvaluateFit(TString logDir);

   private:
      //! histo
      bool                     fHistSet;
      TH1D*                    fHist;

      //! resolution
      bool                     fResSet;
      TF1*                     fRes;

      //! fit settings
      bool                     fRangeSet;
      pair<double,double>      fRange;
      vector<double>           fPeakPos;
      int                      fNPeaks;
      bool                     fLinBkg;
      BCEngineMCMC::Precision  fPrecision;
      double                   fPeakPriorWidth;
      double                   fFWHMPriorWidth;

      //! Internals
      bool                     fFitSet;
      pair<int,int>            fRangeBins;
      int                      fNBins;
      TH1D*                    fFitHist;
      GammaFitFunction*        fFitFunction;

      BCHistogramFitter*       fHistFitter;
      TCanvas*                 fCanvas1;
      double                   kPeakPosRange = /*+-*/3;
      vector<TF1*>             fPeakPriors;
      double                   kFWHMRange = /*+-*/1;
      vector<TF1*>             fResPriors;

      TCanvas*                 fCanvas2;
	bool fResolPrior = true;	
};

} // namespace std

#endif
