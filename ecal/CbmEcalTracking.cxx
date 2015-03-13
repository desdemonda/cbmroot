#include "CbmEcalTracking.h"

#include "CbmEcalStructure.h"
#include "CbmEcalTrackExtrapolation.h"
#include "FairRootManager.h"

#include "FairTrackParam.h"

#include "TClonesArray.h"

#include <iostream>

using namespace std;

/** Default constructor **/
CbmEcalTracking::CbmEcalTracking()
  : FairTask(), 
    fVerbose(0), 
    fStru(NULL), 
    fTrE(NULL),
    fGTArray(NULL),
    fTrackPar(NULL)
{
}

/** Standerd constructor **/
CbmEcalTracking::CbmEcalTracking(const char* name, const Int_t verbose)
  : FairTask(name, verbose), 
    fVerbose(0), 
    fStru(NULL), 
    fTrE(NULL),
    fGTArray(NULL),
    fTrackPar(NULL)
{
}

/** Task initialization **/
InitStatus CbmEcalTracking::Init()
{
  if (!fTrE) return kFATAL; 
  
  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init()", "Can't find IOManager.");
    return kFATAL;
  }
  fGTArray=(TClonesArray*)io->GetObject("GlobalTrack");
  if (!fGTArray)
  {
    Fatal("Init()", "Can't find GlobalTrack.");
    return kFATAL;
  }
  fTrackPar=new TClonesArray("FairTrackParam", 1000);
  io->Register("EcalTrackParam", "ECAL", fTrackPar, kTRUE);
  fStru=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStru) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fTrE->SetStructure(fStru);
  
  fTrE->Init();
  return kSUCCESS;
}

/** Exec for task **/
void CbmEcalTracking::Exec(Option_t* opt)
{
  Int_t tn;
  
  fTrackPar->Clear();
  tn=fTrE->DoExtrapolate(fGTArray, fTrackPar);
  cout << "Total " << tn << " tracks before ECAL." << endl;
/*
  tn=fTrackPar->GetEntriesFast();
  for(i=0;i<tn;i++)
  {
    par=(FairTrackParam*)fTrackPar->At(i);
    if (!par) continue;
    cout << "Track " << i << ":(" << par->GetX() << ", " << par->GetY();
    cout << ", " << par->GetZ() << ")" << endl;
  }
*/
}
  
/** Finish task **/
void CbmEcalTracking::Finish()
{
  fTrE->Finish();
}

/** virtual destructor **/
CbmEcalTracking::~CbmEcalTracking()
{
  fTrackPar->Delete();
  delete fTrackPar;
}

ClassImp(CbmEcalTracking)
