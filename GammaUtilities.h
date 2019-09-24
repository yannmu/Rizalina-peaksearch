//load MasterChain (everything that needs the DataLoader)

#ifndef GammaUtilities_H
#define GammaUtilities_H

//#include <DataLoader.h>

#include "stdlib.h" // getenv

using namespace gada;

TChain* GetMasterFromMap ( FileMap* mymap )
{
   DataLoader loader;
   loader.AddFileMap(mymap);
   loader.BuildTier3();
   loader.BuildTier4();

   TChain* master = loader.GetUniqueMasterChain(false);

   return master;
}

FileMap* GetFileMap( TString rootDir, TString keyList )
{
   FileMap* mymap = new FileMap();
   if(rootDir!="") mymap->SetRootDir(rootDir.Data());
   mymap->BuildFromListOfKeys(keyList.Data());
   mymap->SortAndRemoveDublicates();

   return mymap;
}

TChain* GetMaster( TString rootDir, TString keyList )
{
   return GetMasterFromMap(GetFileMap(rootDir,keyList));
}

TChain* GetMaster ( TString keyList )
{
   return GetMasterFromMap(GetFileMap( "",keyList));
}

#endif
