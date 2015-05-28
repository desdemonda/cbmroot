/** @file  CbmMCDataArray.cxx
 ** @author //Dr.Sys <mikhail.prokudin@cern.ch>
 **/


#include "CbmMCDataArray.h"

#include "TClonesArray.h"
#include "TChain.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include <iostream>

using namespace std;


// --- Standard constructor
CbmMCDataArray::CbmMCDataArray(const char* branchname,
		                           const std::vector<std::list<TString> >& fileList)
  : fLegacy(0), fLegacyArray(NULL), fBranchName(branchname), fSize(-1111), fChains(), fTArr(), fN(), fArrays()
{
  list<TString>::const_iterator p;
  Int_t i;
  Int_t s=fileList.size();

  fSize=s;
  fChains.resize(s);
  fTArr.resize(s);
  fN.resize(s);

  for(i=0;i<s;i++)
  {
    fTArr[i]=0;
    fChains[i]=new TChain("cbmsim");
    for(p=fileList[i].begin();p!=fileList[i].end();++p)
      fChains[i]->AddFile(*p);
    fChains[i]->SetBranchAddress(branchname, &(fTArr[i]));
    fN[i]=fChains[i]->GetEntries();
  }

  fArrays.resize(s);
  for(i=0;i<s;i++)
    fArrays[i].clear();
}


// --- Legacy constructor
CbmMCDataArray::CbmMCDataArray(const char* branchname)
  : fLegacy(1), fLegacyArray(NULL), fBranchName(branchname), fSize(-1111), fChains(), fTArr(), fN(), fArrays()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    LOG(FATAL) << "CbmMCDataArray():	Can't find a Root Manager." << endl;
    return;
  }
  fLegacyArray=(TClonesArray*)fManager->GetObject(branchname);
  if (!fLegacyArray)
  {
    LOG(FATAL) << "CbmMCDataArray(): Can't find " << fBranchName << " in the system.";
    return;
  }
}

// --- Legacy Get
TObject* CbmMCDataArray::LegacyGet(Int_t fileNumber, Int_t eventNumber, Int_t index)
{
  if (fileNumber>=0||eventNumber>=0)
    LOG(DEBUG1) << "LegacyGet:	Trying to get object with fileNum=" << fileNumber
                << ", entryNum=" << eventNumber << " in legacy mode."
                << FairLogger::endl;
  if (index<0) return 0;
  return fLegacyArray->At(index);
}


// --- Get an object
TObject* CbmMCDataArray::Get(Int_t fileNumber, Int_t eventNumber, Int_t index)
{
  if (fLegacy==1) return LegacyGet(fileNumber, eventNumber, index);
  if (fileNumber<0||fileNumber>=fSize) return NULL;
  if (eventNumber<0||eventNumber>=fN[fileNumber]) return NULL;
  if (index<0) return NULL;
  
  // --- Cached arrays
  map<Int_t, TClonesArray*> &arr=fArrays[fileNumber];

  // --- If the array for this event is already in the cache, use it.
  if (arr.find(eventNumber)!=arr.end()) return arr[eventNumber]->At(index);

  // --- If not, copy the array from the chain into the cache
  TChain* ch=fChains[fileNumber];
  ch->GetEntry(eventNumber);
  arr[eventNumber]=(TClonesArray*)(fTArr[fileNumber]->Clone());

  return arr[eventNumber]->At(index);
}


// --- At end of one event: clear the cache to free the memory
void CbmMCDataArray::FinishEvent()
{
  if (fLegacy==1) return;

  Int_t i;
  map<Int_t, TClonesArray*>::const_iterator p;

  for(i=0;i<fSize;i++)
  {
    for(p=fArrays[i].begin();p!=fArrays[i].end();++p)
      delete (p->second);
    fArrays[i].clear();
  }
}


// --- Clean up
void CbmMCDataArray::Done()
{
  if (fLegacy==1) return;
  Int_t i;

  FinishEvent();
  for(i=0;i<fSize;i++)
    delete fChains[i];
}

ClassImp(CbmMCDataArray)
