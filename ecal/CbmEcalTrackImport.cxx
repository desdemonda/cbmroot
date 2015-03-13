#include "CbmEcalTrackImport.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "TMatrixFSym.h"

#include <iostream>

using namespace std;

/** add a root file with tracks to chain **/
void CbmEcalTrackImport::AddFile(const char* name)
{
  fChain->Add(name);
}

/** Standard constructor **/
CbmEcalTrackImport::CbmEcalTrackImport(const char* name, const Int_t verbose)
  : FairTask(name, verbose),
    fChain(new TChain("ecaltracks")),
    fEventN(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fTx(0.),
    fTy(0.),
    fQp(0.),
    fN(0),
    fEntry(0),
    fEv(0),
    fTracks(NULL)
{
}


/** Task initialization **/
InitStatus CbmEcalTrackImport::Init()
{
  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fChain->SetBranchAddress("ev", &fEventN);
  fChain->SetBranchAddress("x", &fX);
  fChain->SetBranchAddress("y", &fY);
  fChain->SetBranchAddress("z", &fZ);
  fChain->SetBranchAddress("tx", &fTx);
  fChain->SetBranchAddress("ty", &fTy);
  fChain->SetBranchAddress("qp", &fQp);
  fN=fChain->GetEntries();
  if (fVerbose>0)
    Info("Init", "%d tracks in input files.", fN);
  if (fN==0)
    Error("Init", "No tracks in input files");
  fTracks=new TClonesArray("FairTrackParam", 1000);
  io->Register("EcalTrackParam", "ECAL", fTracks, kFALSE);
  fEv=0; fEntry=1;
  fChain->GetEntry(0);
  return kSUCCESS;
}

/** Exec for task **/
void CbmEcalTrackImport::Exec(Option_t* opt)
{
  fTracks->Delete();

  fEv++;
  Int_t nTr=0;
  TMatrixFSym mat(5);
  if (fEntry>=fN)
  {
    Warning("Exec", "No tracks found in file for event %d!", fEventN);
    return;
  }
  while(fEventN<fEv)
  {
    new ((*fTracks)[nTr++]) FairTrackParam(fX, fY, fZ, fTx, fTy, fQp, mat);
    if (fEntry<fN)
      fChain->GetEntry(fEntry++);
    else
      break;
  }
  if (fVerbose>0)
    Info("Exec", "%d tracks imported.", nTr);
}
  
/** Finish task **/
void CbmEcalTrackImport::Finish()
{
  ;
}

/** virtual destructor **/
CbmEcalTrackImport::~CbmEcalTrackImport()
{
  fTracks->Delete();
  delete fTracks;
}
  
/** Only to comply with framework **/
CbmEcalTrackImport::CbmEcalTrackImport()
  : FairTask(),
    fChain(),
    fEventN(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fTx(0.),
    fTy(0.),
    fQp(0.),
    fN(0),
    fEntry(0),
    fEv(0),
    fTracks(NULL)
{
}


ClassImp(CbmEcalTrackImport)
