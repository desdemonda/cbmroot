#include "CbmEcalAnalysisMaterial.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRadLenPoint.h"


#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalAnalysisMaterial::Exec(Option_t* option)
{
  fEvent++;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEvent);
  InitTree();
  CbmMCTrack* tr;
  FairRadLenPoint* p;
  Int_t i;
  Int_t n;
  Double_t z=600;
  TVector3 newp;
  TVector3 oldp(0.0, 0.0, 0.0);
  TVector3 d;
  if (fMC->GetEntriesFast()!=1)
  {
    cout << "Size of MC points arrays differs from one" << endl;
  }
  tr=(CbmMCTrack*)fMC->At(0);
  fX=tr->GetPx();
  fY=tr->GetPy();
  fZ=tr->GetPz();
  fX*=z/fZ;
  fY*=z/fZ;
  fZ*=z/fZ;
//  if (TMath::Abs(fX)>594||TMath::Abs(fY)>474) return;
//  if (TMath::Abs(fX)<42&&TMath::Abs(fY)<42) return;
  n=fRadLen->GetEntriesFast();
  fX0=0; fIntL=0;
  for(i=0;i<n;i++)
  {
    p=(FairRadLenPoint*)fRadLen->At(i);
    newp=p->GetPositionOut();
//    cout << d.Mag() << "	" << p->GetRadLength() << "	" << newp.Z() << "	" << fX0 << endl;
    if (newp.Z()>600) break;
    d=newp-oldp;
//    cout << p->GetLength() << "	" << p->GetRadLength() << "	" << fEvent << "	" <<p->GetXOut() << "	" << p->GetYOut() << "	" << p->GetZOut() << endl;
    fX0+=d.Mag()/p->GetRadLength();
    oldp=newp;
  }
  fTree->Fill();
}


/** Initializes tree **/
void CbmEcalAnalysisMaterial::InitTree()
{
  if (fTree) return;
  fTree=new TTree("rad","Radiation lenght tree");
  fTree->Branch("ev", &fEvent, "ev/I");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "Y/D");
  fTree->Branch("z", &fZ, "z/D");
  fTree->Branch("x0", &fX0, "x0/D");
  fTree->Branch("intl", &fIntL, "intl/D");
}

CbmEcalAnalysisMaterial::CbmEcalAnalysisMaterial()
  : FairTask(),
    fTree(NULL),
    fEvent(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fX0(0.),
    fIntL(0.),
    fMC(NULL),
    fRadLen(NULL)
{
}

CbmEcalAnalysisMaterial::CbmEcalAnalysisMaterial(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEvent(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fX0(0.),
    fIntL(0.),
    fMC(NULL),
    fRadLen(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisMaterial::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fMC=(TClonesArray*)fManager->GetObject("MCTrack");
  if (!fMC)
  {
    Fatal("Init", "Can't find an array of MC tracks.");
    return kFATAL;
  }
  fRadLen=(TClonesArray*)fManager->GetObject("RadLen");
  if (!fRadLen)
  {
    Fatal("Init", "Can't find an array of radiation length points.");
    return kFATAL;
  }
  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisMaterial::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisMaterial)
