//handler for tier data
// - tier3/4 access
// - runconf

#ifndef GammaTierHandler_H
#define GammaTierHandler_H

//ROOT
#include "TChain.h"

//GELATIO
//#include "GETRunConfiguration.hh"
//#include "GERunConfigurationManager.hh"

// c++
#include <vector>

#include "GammaEvent.h"

namespace std {

class GammaTierHandler
{
   public:
      GammaTierHandler( TChain* master, bool ZAC = true );
      virtual ~GammaTierHandler();

      //! Getters
      GammaEvent*           GetEvent()   { return  fEvent;    };
     // GETRunConfiguration*  GetRunConf() { return  fRunConf;  };

      //! Methods
      int   GetEntries()    { return fMaster->GetEntries();   };
     // void  GetEntry(int i);
      void  Reset();

   protected:
      void Initialize();
      void SetBranchAddresses();

   private:
      bool                        fIsInitialized;
      TChain*                     fMaster;
     // GERunConfigurationManager*  fRunConfManager;
     // GETRunConfiguration*        fRunConf;
      GammaEvent*                 fEvent;
};

}
#endif
