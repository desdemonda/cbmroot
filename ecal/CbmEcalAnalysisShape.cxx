#include "CbmEcalAnalysisShape.h"

#include "FairRootManager.h"

#include "CbmMCTrack.h"

#include "CbmEcalInf.h"
#include "CbmEcalPoint.h"
#include "CbmEcalPointLite.h"
#include "CbmEcal.h"

#include "TTree.h"
#include "TMath.h"
#include "TClonesArray.h"

#include <iostream>
#include <fstream>

using namespace std;

Int_t CbmEcalAnalysisShape::GetNum(Double_t x, Double_t y) const
{
  Double_t dx=x; dx+=0.025; dx-=fDX; dx/=fStep;
  Double_t dy=y; dy+=0.025; dy-=fDY; dy/=fStep;
  Int_t ix=TMath::FloorNint(dx+0.00001);
  Int_t iy=TMath::FloorNint(dy+0.00001);
  if (fVerbose>99)
  {
    cout << "(x, y)=(" << x << ", " << y << "), (dx, dy)=(" << dx << ", " << dy << "), (ix, iy)=(" << ix << ", " << iy << ")" << endl; 
  }
  if (ix<0) return -1111;
  if (iy<0) return -1111;
  if (ix>=fXSize) return -1111;
  if (iy>=fYSize) return -1111;
  return ix+iy*fXSize;
}
/** Loop procedure **/
void CbmEcalAnalysisShape::Exec(Option_t* option)
{
  fEv++;
//  cout << fEv << endl;
  InitTree();
  InitE();

  Int_t i;
  Int_t n;
  Int_t m;
  CbmEcalPointLite* p;
  Bool_t isPS;
  Int_t ten;
  Float_t x;
  Float_t y;

  if (fWriteEach)
    for(i=0;i<fSize;i++)
      fShape[i]=0.0;
  n=fP->GetEntriesFast();
  for(i=0;i<n;i++)
  {
    p=(CbmEcalPointLite*)fP->At(i);
    if (!p) continue;
    isPS=CbmEcal::GetCellCoord(p->GetDetectorID(), x, y, ten);
    if (ten!=0) continue;
    m=GetNum(x-fStartX, y-fStartY);
    if (m<0)
    {
//      cout << m << endl;
      continue;
    }
    fShape[m]+=p->GetEnergyLoss();
  }
  if (fWriteEach)
  {
    for(i=0;i<fSize;i++)
      fShapeOut[i]=fShape[i];
    fTree->Fill();
  }
} 


CbmEcalAnalysisShape::CbmEcalAnalysisShape()
  : FairTask(),
    fName(),
    fE(0.),
    fPhi(0.),
    fTheta(0.),
    fShape(NULL),
    fShapeOut(NULL),
    fXSize(30*12),
    fYSize(30*12),
    fSize(0),
    fDX(-12.),
    fDY(-12.),
    fStep(0.1),
    fStartX(0.),
    fStartY(0.),
    fWriteEach(kFALSE),
    fEv(0),
    fTree(NULL),
    fInf(),
    fMC(NULL),
    fP(NULL),
    fMCTracks(NULL)
{
}


CbmEcalAnalysisShape::CbmEcalAnalysisShape(const char* name, const Int_t iVerbose, const char* fname)
  : FairTask(name, iVerbose),
    fName(fname),
    fE(0.),
    fPhi(0.),
    fTheta(0.),
    fShape(NULL),
    fShapeOut(NULL),
    fXSize(30*12),
    fYSize(30*12),
    fSize(0),
    fDX(-12.),
    fDY(-12.),
    fStep(0.1),
    fStartX(0.),
    fStartY(0.),
    fWriteEach(kFALSE),
    fEv(0),
    fTree(NULL),
    fInf(CbmEcalInf::GetInstance(fName)),
    fMC(NULL),
    fP(NULL),
    fMCTracks(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisShape::Init()
{
  Int_t i;
  FairRootManager* fManager = FairRootManager::Instance();

  fMC=(TClonesArray*)fManager->GetObject("EcalPoint");
  if (!fMC)
  {
    Fatal("Init()", "Can't find array of calorimeter points in the system.");
    return kFATAL;
  }
  fP=(TClonesArray*)fManager->GetObject("EcalPointLite");
  if (!fP)
  {
    Fatal("Init()", "Can't find array of lite points in the system.");
    return kFATAL;
  }
  fMCTracks=(TClonesArray*)fManager->GetObject("MCTrack");
  if (!fMCTracks)
  {
    Fatal("Init()", "Can't find array of MC tracks in the system.");
    return kFATAL;
  }
  fStep=fInf->GetCellSize();
  fXSize=30*12*(0.1+0.000001)/fStep;
  fYSize=30*12*(0.1+0.000001)/fStep;
  Info("Init()", "fXSize=%d, fYSize=%d", fXSize, fYSize);
  Info("Init", "Using %f cell size", fStep);
  fSize=fXSize*fYSize;
  fShape=new Double_t[fSize];
  fShapeOut=new Float_t[fSize];
  for(i=0;i<fSize;i++)
    fShape[i]=0.0;
  fE=-1111;
  fPhi=-1111;
  fTheta=-1111;

  return kSUCCESS;
}

void CbmEcalAnalysisShape::InitTree()
{
  if (fTree) return;
  fTree=new TTree("shape","shover shape");
  fTree->Branch("e", &fE, "e/D");
  fTree->Branch("phi", &fPhi, "phi/D");
  fTree->Branch("theta", &fTheta, "theta/D");
  fTree->Branch("dx", &fDX, "dx/D");
  fTree->Branch("dy", &fDY, "dy/D");
  fTree->Branch("step", &fStep, "step/D");
  fTree->Branch("xsize", &fXSize, "xsize/I");
  fTree->Branch("ysize", &fYSize, "ysize/I");
  TString nm; nm="shape["; nm+=fSize; nm+="]/F";
  fTree->Branch("shape", fShapeOut, nm);
}

void CbmEcalAnalysisShape::InitE()
{
  CbmEcalPoint* pt=(CbmEcalPoint*)fMC->At(0);
  CbmMCTrack* mc=(CbmMCTrack*)fMCTracks->At(pt->GetTrackID());
  TLorentzVector v;
  mc->Get4Momentum(v);
  fE=v.E();
  fPhi=v.Phi();
  fTheta=v.Theta();
  fStartX=pt->GetX();
  fStartY=pt->GetY();
 
  if (fVerbose>9)
    cout << "start: (" << fStartX << ", " << fStartY << ")" << endl;
}
/** Finishing routine **/
void CbmEcalAnalysisShape::Finish()
{
  Int_t i;

  if (fTree)
  {
    if (!fWriteEach)
    {
      for(i=0;i<fSize;i++)
      {
	fShape[i]/=fEv;
        fShapeOut[i]=fShape[i];
      }
      fTree->Fill();
    }
    fTree->Write();
  }
}

CbmEcalAnalysisShape::~CbmEcalAnalysisShape()
{
  delete fShape;
  delete fShapeOut;
}

ClassImp(CbmEcalAnalysisShape)
