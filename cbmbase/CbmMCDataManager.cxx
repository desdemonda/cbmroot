#include "CbmMCDataManager.h"

#include "CbmMCDataArray.h"

#include "FairRootManager.h"

#include <iostream>

using namespace std;

// --- Default constructor
CbmMCDataManager::CbmMCDataManager()
  : FairTask("MCDataManager", 0), fLegacy(0), fFileList(), fActive()
{
  fFileList.clear();
  fActive.clear();
}


// --- Standard constructor
CbmMCDataManager::CbmMCDataManager(const char* name, Int_t legacy)
  : FairTask(name,0), fLegacy(legacy), fFileList(), fActive()
{
  fFileList.clear();
  fActive.clear();
}


// --- Destructor
CbmMCDataManager::~CbmMCDataManager()
{
  map<TString, CbmMCDataArray*>::const_iterator p;
  for(p=fActive.begin();p!=fActive.end();++p) delete p->second;
  fActive.clear();
}


// --- Add an additional input file
Int_t CbmMCDataManager::AddFile(const char* name)
{
  Int_t n=fFileList.size();
  fFileList.resize(n+1);
  fFileList[n].clear();
  fFileList[n].push_back(name);
  return n;
}


// --- Add a file to a given input chain
Int_t CbmMCDataManager::AddFileToChain(const char* name, Int_t number)
{
  Int_t i;
  Int_t n=fFileList.size();

  if (number<0) return -1111;
  if (number>=n)
  {
    fFileList.resize(number+1);
    for(i=n;i<=number;i++) fFileList[i].clear();
  }
  fFileList[number].push_back(name);

  return fFileList[i].size();
}


// --- End of event action
void CbmMCDataManager::FinishEvent()
{
  cout << "FinishEvent" << endl;
  map<TString, CbmMCDataArray*>::const_iterator p;

  for(p=fActive.begin();p!=fActive.end();++p)
    p->second->FinishEvent();
}


// --- Initialisation
InitStatus CbmMCDataManager::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager) return kFATAL;
  
  fManager->Register("MCDataManager", "Stack", (TNamed*)this, kFALSE);
  
  return kSUCCESS;
}


// --- Instantiate a data branch
CbmMCDataArray* CbmMCDataManager::InitBranch(const char* brname)
{
  CbmMCDataArray* arr;
  TString nm=brname;

  if (fActive.find(nm)!=fActive.end())
  {
    cout << "InitBranch: " << nm << " " << fActive[nm] << endl; 
    return fActive[nm];
  }  
  if (fLegacy==0)
    arr=new CbmMCDataArray(brname, fFileList);
  else
    arr=new CbmMCDataArray(brname);

  fActive[nm]=arr;
  cout << "InitBranch: " << nm << " " << arr << endl; 
  return arr;
}



ClassImp(CbmMCDataManager)
