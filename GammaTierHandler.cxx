#include "GammaTierHandler.h"
#include <stdlib.h> 
namespace std {

GammaTierHandler::GammaTierHandler( TChain* master, bool ZAC ) :
 fMaster(master), fIsInitialized(false)
{
   fMaster->SetBranchStatus("*",0); //disable all branches
   fEvent = new GammaEvent(ZAC);
}

GammaTierHandler::~GammaTierHandler()
{
   delete fEvent;
   if(fIsInitialized) delete fRunConfManager;
}

void GammaTierHandler::SetBranchAddresses()
{
   fMaster->SetBranchStatus("tier4_all.eventChannelNumber",    1 );
   fMaster->SetBranchStatus("tier4_all.timestamp",             1 );
   fMaster->SetBranchStatus("tier4_all.decimalTimestamp",      1 );
   fMaster->SetBranchStatus("tier4_all.isMuVetoed",            1 );
   fMaster->SetBranchStatus("tier4_all.multiplicity",          1 );
   fMaster->SetBranchStatus("tier4_all.firedFlag",             1 );
   fMaster->SetBranchStatus("tier4_all.energy",                1 );
   fMaster->SetBranchStatus("tier4_all.isTP",                  1 );
   fMaster->SetBranchStatus("tier4_all.isBL",                  1 );
   fMaster->SetBranchStatus("tier4_all.psdIsEval",             1 );
   fMaster->SetBranchStatus("tier4_all.isPSDVetoed",           1 );
   fMaster->SetBranchStatus("tier4_all.isLArVetoed",           1 );
   fMaster->SetBranchStatus("tier3_all.rawEnergyZAC",          1 );
   fMaster->SetBranchStatus("tier3_all.failedFlag_isPhysical", 1 );

   fMaster->SetBranchAddress("tier4_all.eventChannelNumber",    fEvent->GetEventChannelNumberPtr() );
   fMaster->SetBranchAddress("tier4_all.timestamp",             fEvent->GetTimestampPtr()          );
   fMaster->SetBranchAddress("tier4_all.decimalTimestamp",      fEvent->GetDecimalTimestampPtr()   );
   fMaster->SetBranchAddress("tier4_all.isMuVetoed",            fEvent->GetIsMuVetoedPtr()         );
   fMaster->SetBranchAddress("tier4_all.multiplicity",          fEvent->GetMultiplicityPtr()       );
   fMaster->SetBranchAddress("tier4_all.firedFlag",             fEvent->GetFiredFlagPtr()          );
   fMaster->SetBranchAddress("tier4_all.energy",                fEvent->GetEnergyPtr()             );
   fMaster->SetBranchAddress("tier4_all.isTP",                  fEvent->GetIsTPPtr()               );
   fMaster->SetBranchAddress("tier4_all.isBL",                  fEvent->GetIsBLPtr()               );
   fMaster->SetBranchAddress("tier4_all.psdIsEval",             fEvent->GetPsdIsEvalPtr()          );
   fMaster->SetBranchAddress("tier4_all.isPSDVetoed",           fEvent->GetIsPSDVetoedPtr()        );
   fMaster->SetBranchAddress("tier4_all.isLArVetoed",           fEvent->GetIsLArVetoedPtr()        );
   fMaster->SetBranchAddress("tier3_all.rawEnergyZAC",          fEvent->GetEnergyZACPtr()          );
   fMaster->SetBranchAddress("tier3_all.failedFlag_isPhysical", fEvent->GetFailedFlagPtr()         );
}

void GammaTierHandler::Reset()
{
   //get new runconfmanager, which is a one-way-street
   if(fIsInitialized) delete fRunConfManager;
   std::cout << "run config manager" << std::endl;
   //char mypath[]="export MU_CAL=/nfs/gerda5/gerda-data/blind/v03.02/meta/config/_aux/geruncfg/";
   //putenv( mypath );
   fRunConfManager = new GERunConfigurationManager();
   fRunConfManager->SetVerbosity(1);
   fRunConfManager->AllowRunConfigurationSwitch(true);
}

void GammaTierHandler::Initialize()
{
   if(fIsInitialized) return;
   SetBranchAddresses();
   Reset();

   fIsInitialized = true;
}


void GammaTierHandler::GetEntry(int i)
{
   if(!fIsInitialized) Initialize();
   fMaster->GetEntry(i);
   if(fEvent->GetTimestamp()){
      fRunConf = fRunConfManager->GetRunConfiguration(fEvent->GetTimestamp());
}
   fEvent->SetRunConfig(fRunConf);
   fEvent->SetFields();
}

} // namespace
