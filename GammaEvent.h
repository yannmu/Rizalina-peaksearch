    //gamma event container
// - tier3/4 branches
// - datasetID
// - runconfig

#ifndef GammaEvent_H
#define GammaEvent_H

// c++
#include <vector>

//GADA
//#include <GADatasetManager.h>

//GELATIO
//#include "GETRunConfiguration.hh"

namespace std {

class GammaEvent
{
   public:
      //! constructor
      GammaEvent( bool ZAC = true ) : fZAC(ZAC) {
    //     fDatasetManager     = new gada::GADatasetManager();
         fEventChannelNumber = new int;
         fTimestamp          = new ULong64_t;
         fDecimalTimestamp   = new UInt_t;
         fIsMuVetoed         = new int;
         fMultiplicity       = new int;
         fFiredFlag          = new vector<int>;
         fEnergy             = new vector<double>;
         fIsTP               = new int;
         fIsBL               = new int;
         fPsdIsEval          = new vector<int>;
         fIsPSDVetoed        = new vector<int>;
         fIsLArVetoed        = new int;
         fEnergyZAC          = new vector<double>;
         fFailedFlag         = new vector<UInt_t>;
         fDatasetID          = new vector<int>;
      };
      //! destructor
      virtual ~GammaEvent() {
        // delete fDatasetManager;
         delete fEventChannelNumber;
         delete fTimestamp;
         delete fDecimalTimestamp;
         delete fIsMuVetoed;
         delete fMultiplicity;
         delete fFiredFlag;
         delete fEnergy;
         delete fIsTP;
         delete fIsBL;
         delete fPsdIsEval;
         delete fIsPSDVetoed;
         delete fIsLArVetoed;
         delete fEnergyZAC;
         delete fFailedFlag;
         delete fDatasetID;
      };

      //! getters
 //     gada::GADatasetManager* GetDatasetManager()        { return  fDatasetManager;     };
      int                     GetEventChannelNumber()    { return *fEventChannelNumber; };
      int*                    GetEventChannelNumberPtr() { return  fEventChannelNumber; };
      ULong64_t               GetTimestamp()             { return *fTimestamp;          };
      ULong64_t*              GetTimestampPtr()          { return  fTimestamp;          };
      UInt_t                  GetDecimalTimestamp()      { return *fDecimalTimestamp;   };
      UInt_t*                 GetDecimalTimestampPtr()   { return  fDecimalTimestamp;   };
      int                     GetIsMuVetoed()            { return *fIsMuVetoed;         };
      int*                    GetIsMuVetoedPtr()         { return  fIsMuVetoed;         };
      int                     GetMultiplicity()          { return *fMultiplicity;       };
      int*                    GetMultiplicityPtr()       { return  fMultiplicity;       };
      vector<int>*            GetFiredFlag()             { return  fFiredFlag;          };
      vector<int>**           GetFiredFlagPtr()          { return &fFiredFlag;          };
      vector<double>*         GetEnergy()                { return  fEnergy;             };
      vector<double>**        GetEnergyPtr()             { return &fEnergy;             };
      int                     GetIsTP()                  { return *fIsTP;               };
      int*                    GetIsTPPtr()               { return  fIsTP;               };
      int                     GetIsBL()                  { return *fIsBL;               };
      int*                    GetIsBLPtr()               { return  fIsBL;               };
      vector<int>*            GetPsdIsEval()             { return  fPsdIsEval;          };
      vector<int>**           GetPsdIsEvalPtr()          { return &fPsdIsEval;          };
      vector<int>*            GetIsPSDVetoed()           { return  fIsPSDVetoed;        };
      vector<int>**           GetIsPSDVetoedPtr()        { return &fIsPSDVetoed;        };
      int                     GetIsLArVetoed()           { return *fIsLArVetoed;        };
      int*                    GetIsLArVetoedPtr()        { return  fIsLArVetoed;        };
      vector<double>*         GetEnergyZAC()             { return  fEnergyZAC;          };
      vector<double>**        GetEnergyZACPtr()          { return &fEnergyZAC;          };
      vector<UInt_t>*         GetFailedFlag()            { return  fFailedFlag;         };
      vector<UInt_t>**        GetFailedFlagPtr()         { return &fFailedFlag;         };
      vector<int>*            GetDatasetID()             { return  fDatasetID;          };
      vector<int>**           GetDatasetIDPtr()          { return &fDatasetID;          };
      //GETRunConfiguration*    GetRunConfig()             { return  fRunConf;            };

      //! setters
    //  void SetRunConfig(GETRunConfiguration* runConf)    { fRunConf = runConf;          };

      //! methods

   private:
      //! tier4 fields
      int*             fEventChannelNumber;
      ULong64_t*       fTimestamp;
      UInt_t*          fDecimalTimestamp;
      int*             fIsMuVetoed;
      int*             fMultiplicity;
      vector<int>*     fFiredFlag;
      vector<double>*  fEnergy;
      int*             fIsTP;
      int*             fIsBL;
      //-> if other psd needed, add fields here
      vector<int>*     fPsdIsEval;
      vector<int>*     fIsPSDVetoed;
      int*             fIsLArVetoed;

      //! tier3 fields
      bool             fZAC;
      vector<double>*  fEnergyZAC;
      vector<UInt_t>*  fFailedFlag;

      //! extra fields
   //   gada::GADatasetManager* fDatasetManager;
      vector<int>*            fDatasetID;
//      GETRunConfiguration*    fRunConf;
};

}
#endif
