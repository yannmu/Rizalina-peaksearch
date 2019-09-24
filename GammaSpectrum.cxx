#include "GammaSpectrum.h"
namespace std {

    GammaAutoSpectrum::GammaAutoSpectrum( std::string name,
                                          double binning, pair<double,double> range ) :
            GammaSpectrum(TH1D(name.c_str(),"",(int)(range.second-range.first)
                                               / binning,range.first,range.second), 0, 1),
            fIsInitialized(false), fTierHandlerSet(false)
    {
        fExposureCorr = fExposure*fEfficiency;

        fIncludedDets = "ALL";
        fExcludedDets = "";
        fACCut        = true;
        fLArMode      = kLArNull;
        fPSDMode      = kPSDNull;
    }

//Contains - > find()
    void GammaAutoSpectrum::DumpSettings()
    {
        cout << "- name:          " <<  GetName()                         << endl;
        cout << "- binning:       " <<  fBinning << " keV"                << endl;
        cout << "- dataset:       ";
      //  if(! fDatasets.size()) cout <<  "not specified"                   << endl;
        //else for(auto& kv : fDatasets) { cout << kv.c_str() << " "; } cout << endl;
        cout << "- included dets: " <<  fIncludedDets.c_str()              << endl;
        cout << "- exluded dets:  " <<  fExcludedDets.c_str()              << endl;
        cout << "- AC cut:        " << (fACCut       ? "ON" : "OFF")      << endl;
        cout << "- LAr veto cut:  " << (fLArMode == kLArNull ? "OFF"  : "ON ")
             << (fLArMode == kLArAC   ? "(AC)" : ""   )
             << (fLArMode == kLArC    ? "(C)"  : ""   ) << endl;
        cout << "- PSD cut:       " << (fPSDMode == kPSDNull ? "OFF"  : "ON ")
             << (fPSDMode == kPSDAC   ? "(AC)" : ""   )
             << (fPSDMode == kPSDC    ? "(C)"  : ""   ) << endl;
    }

} // namespace
