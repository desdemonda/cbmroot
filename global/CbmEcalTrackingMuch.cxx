#include "CbmEcalTrackingMuch.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"

#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalTrackingMuch::Exec(Option_t* option)
{
  Int_t n=fStsTracks->GetEntriesFast();
  Int_t i=0;
  CbmStsTrack* tr;
  FairTrackParam tpar;

  fTrackPar->Clear();
  for(;i<n;i++)
  {
    tr=(CbmStsTrack*)fStsTracks->At(i);
    if (!tr) continue;
    fFitter->Extrapolate(tr, fCaloZ, &tpar);
    new ((*fTrackPar)[i]) FairTrackParam(tpar);
  }
}


CbmEcalTrackingMuch::CbmEcalTrackingMuch(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fStsTracks(NULL),
    fStr(NULL),
    fInf(NULL),
    fCaloZ(0.),
    fFitter(NULL),
    fTrackPar(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalTrackingMuch::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fStsTracks=(TClonesArray*)fManager->GetObject("StsTrack");
  if (!fStsTracks)
  {
    Fatal("Init", "Can't find an array of STS tracks.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fTrackPar=new TClonesArray("FairTrackParam", 3000);
  fManager->Register("EcalTrackParam", "ECAL", fTrackPar, kTRUE);
  fFitter=new CbmStsKFTrackFitter(); 
  fInf=fStr->GetEcalInf();
  fCaloZ=fInf->GetZPos();

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalTrackingMuch::Finish()
{
  fTrackPar->Delete();
  delete fTrackPar;
  delete fFitter;
}

ClassImp(CbmEcalTrackingMuch)
