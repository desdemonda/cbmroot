#include "CbmEcalTrackExport.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"
#include "TFile.h"

#include <iostream>

using namespace std;

/** Default constructor **/
CbmEcalTrackExport::CbmEcalTrackExport()
  : FairTask(),
    fVerbose(0), 
    fEventN(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fTx(0.),
    fTy(0.),
    fQp(0.),
    fTracksOut(NULL),
    fTrackPar(NULL)
{
}

/** Standerd constructor **/
CbmEcalTrackExport::CbmEcalTrackExport(const char* name, const Int_t verbose)
  : FairTask(name, verbose),
    fVerbose(verbose), 
    fEventN(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fTx(0.),
    fTy(0.),
    fQp(0.),
    fTracksOut(NULL),
    fTrackPar(NULL)
{

}

/** Task initialization **/
InitStatus CbmEcalTrackExport::Init()
{
  fTracksOut=NULL;
  fEventN=0;

  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fTrackPar=(TClonesArray*)io->GetObject("EcalTrackParam");
  if (!fTrackPar)
  {
    Fatal("Init", "Can't find EcalTrackParam.");
    return kFATAL;
  }
  return kSUCCESS;
}

/** Exec for task **/
void CbmEcalTrackExport::Exec(Option_t* opt)
{
  if (fTracksOut==NULL)
  {
    fTracksOut=new TTree("ecaltracks","Tracks before the calorimeter");
    fTracksOut->Branch("x", &fX, "x/F");
    fTracksOut->Branch("y", &fY, "y/F");
    fTracksOut->Branch("z", &fZ, "z/F");
    fTracksOut->Branch("tx", &fTx, "tx/F");
    fTracksOut->Branch("ty", &fTy, "ty/F");
    fTracksOut->Branch("qp", &fQp, "qp/F");
    fTracksOut->Branch("ev", &fEventN, "ev/I");
  }
  Int_t i;
  Int_t n=fTrackPar->GetEntriesFast();
  Int_t tr=0;
  FairTrackParam* par;

  for(i=0;i<n;i++)
  {
    par=(FairTrackParam*)fTrackPar->At(i);
    if (par==NULL) continue;
    fX=par->GetX();
    fY=par->GetY();
    fZ=par->GetZ();
    fTx=par->GetTx();
    fTy=par->GetTy();
    fQp=par->GetQp();
    fTracksOut->Fill();
    tr++;
  }
  if (fVerbose>0)
  {
    Info("Exec", "%d tracks writen to disk.", tr);
  }
  fEventN++;
  gFile->Write();
}
  
/** Finish task **/
void CbmEcalTrackExport::Finish()
{
  if (fTracksOut) fTracksOut->Write();
}

/** virtual destructor **/
CbmEcalTrackExport::~CbmEcalTrackExport()
{
  ;
}


ClassImp(CbmEcalTrackExport)
