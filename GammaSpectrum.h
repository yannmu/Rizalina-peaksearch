//gamma GammaSpectrum class
// - produce spectra while looping over dataset
// - specific detector (on/off) settings
// - calculates exposure/efficiency

#ifndef GammaSpectrum_H
#define GammaSpectrum_H

#include "GammaTierHandler.h"
#include "GammaEvent.h"

#include "TH1D.h"

namespace std {

    class GammaSpectrum : public TH1D
    {
    public:
        GammaSpectrum( TH1D hist, double exposure=-1, double efficiency=1 ) :
                TH1D(hist), fExposure(exposure), fEfficiency(efficiency) {
              fBinning = GetBinWidth(0);
        };
        //! destructor
        virtual ~GammaSpectrum() {};

        //! getters
        double GetBinning()     { return fBinning;    };
        double GetExposure()    { return fExposure;   };
        double GetEfficiency()  { return fEfficiency; };

        //! setters
        void   SetExposure(double exposure)      { fExposure=exposure;     };
        void   SetEfficiency(double efficiency)  { fEfficiency=efficiency; };

    protected:
        double fBinning;
        double fExposure;
        double fEfficiency;
    };


    class GammaAutoSpectrum : public GammaSpectrum
    {
    public:
        enum LArMode { kLArNull = 0, kLArAC = 1, kLArC = 2 };
        enum PSDMode { kPSDNull = 0, kPSDAC = 1, kPSDC = 2 };

        //! constructors
        GammaAutoSpectrum( string name, double binning = 0.5,
                           pair<double,double> range = {0,1e4} );
        //! destructor
        virtual ~GammaAutoSpectrum() {};

        //! getters
        double           GetBinning()          { return fBinning;         };
        double           GetExposure()         { return fExposure;        };
        double           GetEfficiency()       { return fEfficiency;      };
        vector<string>  GetDatasets()         { return fDatasets;        };
        string          GetIncludedDets()     { return fIncludedDets;    };
        string          GetExcludedDets()     { return fExcludedDets;    };
        bool             GetACCut()            { return fACCut;           };
        LArMode          GetLArMode()          { return fLArMode;         };
        PSDMode          GetPSDMode()          { return fPSDMode;         };

        //! setters
        void  SetDatasets(vector<string> datasets) { fDatasets=datasets;           };
        void  SetIncludedDets(string includedDets) { fIncludedDets=includedDets;   };
        void  SetExcludedDets(string excludedDets) { fExcludedDets=excludedDets;   };
        void  SetLArMode(LArMode lArMode)           { fLArMode=lArMode;             };
        void  SetPSDMode(PSDMode pSDMode)           { fPSDMode=pSDMode;             };
        void  UseACCut(bool aCCut=true)             { fACCut=aCCut;                 };

        //! special setters and methods
        void  AddDataset(string dataset)           { fDatasets.push_back(dataset); };
        void  SetTierHandler(GammaTierHandler* tierhandler) {
              fTierHandler=tierhandler; fTierHandlerSet= true; };
       void  DumpSettings();

        void SetMultiplicity(const int mult){fMultiplicity = mult;}

    protected:
        void  Initialize(GammaEvent* event);
        bool  IsIncluded(GammaEvent* event, int ch);
        bool  IsInDataset(GammaEvent* event, int ch);

    private:
        double       fExposureCorr;
        bool         fIsInitialized;

        int fMultiplicity = 1;
        //! tierhandler interface
        bool              fTierHandlerSet;
        GammaTierHandler* fTierHandler;

        //! settings
        vector<string>   fDatasets;
        string           fIncludedDets;
        string           fExcludedDets;
        vector<int>       fChannels;
        bool              fACCut;
        LArMode           fLArMode;
        PSDMode           fPSDMode;
    };

} //namespace std

#endif
