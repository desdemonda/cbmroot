#include "CbmEcalAnalysisDst.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "CbmEcalCell.h"
#include "CbmEcalPoint.h"
#include "CbmEcalStructure.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalAnalysisDst::Exec(Option_t* option)
{
  Double_t z;
  Double_t r;
  CbmEcalPoint* pt;
  Double_t px;
  Double_t py;
  Double_t pz;
  Double_t e;
  Double_t max=-1111;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* cl=NULL;
  Int_t imax=-1111;
  Double_t e2=-1111;
  Double_t e2m;
  Int_t i;
  TVector2 v;
  Double_t phi;

  fEvent=0;

  InitTree();

  pt=(CbmEcalPoint*)fPts->At(0);
  if (pt==NULL) return;
  fX=pt->GetX();
  fY=pt->GetY();

  px=pt->GetPx(); px*=px;
  py=pt->GetPy(); py*=py;
  pz=pt->GetPz(); pz*=pz;
  fE=TMath::Sqrt(px+py+pz);

  fCaloE=0;
  fStr->GetCells(cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    if ((*p)==NULL) continue;
    e=(*p)->GetTotalEnergy();
    fCaloE+=e;
    if (e>max)
    {
      max=e;
      cl=(*p);
    }
  }
  for(i=1;i<5;i++)
  {
    cl->GetNeighborsList(i, cells);
    e2m=max;
    for(p=cells.begin();p!=cells.end();++p)
      e2m+=(*p)->GetTotalEnergy();
    if (e2m>e2)
    { 
      e2=e2m;
      imax=i;
    }
  }
  cl->GetNeighborsList(imax, cells);
  fCX=cl->GetCenterX()*cl->GetTotalEnergy(); 
  fCY=cl->GetCenterY()*cl->GetTotalEnergy();
  fMX=cl->GetCenterX();
  fMY=cl->GetCenterY();
  for(p=cells.begin();p!=cells.end();++p)
  {
    fCX+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
    fCY+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();
  }
  fCX/=e2; fCY/=e2;

  v.Set(fCX, fCY);
  phi=-(v.Phi());
  v.Set(fX-fCX, fY-fCY);
  v=v.Rotate(phi);
  fP=v.X(); fQ=v.Y();

  v.Set(fX, fY);
  phi=-(v.Phi());
  v.Set(fX-fMX, fY-fMY);
  v=v.Rotate(phi);
  fMP=v.X(); fMQ=v.Y();

  z=pt->GetZ();
  fPhi=TMath::ATan2(fY, fX);
  r=TMath::Sqrt(fX*fX+fY*fY);
  fTheta=TMath::ATan2(r, z);
  fCPhi=TMath::ATan2(fCY, fCX);
  r=TMath::Sqrt(fCX*fCX+fCY*fCY);
  fCTheta=TMath::ATan2(r, z);

  fTree->Fill();
}


/** Initializes tree **/
void CbmEcalAnalysisDst::InitTree()
{
  if (fTree) return;
  fTree=new TTree("dst","dst tree");
  fTree->Branch("ev",&fEvent,"ev/I");

  fTree->Branch("e",&fE,"e/D");
  fTree->Branch("x",&fX,"x/D");
  fTree->Branch("y",&fY,"y/D");
  fTree->Branch("caloe",&fCaloE,"caloe/D");
  fTree->Branch("cx",&fCX,"cx/D");
  fTree->Branch("cy",&fCY,"cy/D");
  fTree->Branch("p",&fP,"p/D");
  fTree->Branch("q",&fQ,"q/D");
  fTree->Branch("theta",&fTheta,"theta/D");
  fTree->Branch("phi",&fPhi,"phi/D");
  fTree->Branch("ctheta",&fCTheta,"ctheta/D");
  fTree->Branch("cphi",&fCPhi,"cphi/D");
  fTree->Branch("mx",&fMX,"mx/D");
  fTree->Branch("my",&fMY,"my/D");
  fTree->Branch("mp",&fMP,"mp/D");
  fTree->Branch("mq",&fMQ,"mq/D");
}

CbmEcalAnalysisDst::CbmEcalAnalysisDst()
  : FairTask(),
    fTree(NULL),
    fEntries(0),
    fEvent(0),
    fE(0.),
    fX(0.),
    fY(0.),
    fCaloE(0.),
    fCX(0.),
    fCY(0.),
    fP(0.),
    fQ(0.),
    fTheta(0.),
    fPhi(0.),
    fCTheta(0.),
    fCPhi(0.),
    fMX(0.),
    fMY(0.),
    fMP(0.),
    fMQ(0.),
    fPts(NULL),
    fStr(NULL)
{
}


CbmEcalAnalysisDst::CbmEcalAnalysisDst(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEntries(0),
    fEvent(0),
    fE(0.),
    fX(0.),
    fY(0.),
    fCaloE(0.),
    fCX(0.),
    fCY(0.),
    fP(0.),
    fQ(0.),
    fTheta(0.),
    fPhi(0.),
    fCTheta(0.),
    fCPhi(0.),
    fMX(0.),
    fMY(0.),
    fMP(0.),
    fMQ(0.),
    fPts(NULL),
    fStr(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisDst::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fPts=(TClonesArray*)fManager->GetObject("EcalPoint");
  if (!fPts)
  {
    Fatal("Init", "Can't find an array of calorimeter points.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fTree=NULL;
  fEvent=0;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisDst::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisDst)
