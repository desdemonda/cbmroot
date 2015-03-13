#include "CbmEcalAnalysisPair.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "CbmEcalRecParticle.h"
#include "CbmEcalCell.h"
#include "CbmEcalCluster.h"
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
void CbmEcalAnalysisPair::Exec(Option_t* option)
{
  Int_t i;
  Int_t n=fReco->GetEntriesFast();
  Int_t mcn=fMC->GetEntriesFast();
  CbmEcalRecParticle* p1;
  CbmEcalRecParticle* p2;
  CbmEcalCluster* cluster;
  CbmEcalCell* cell;
  CbmMCTrack* tr1;
  CbmMCTrack* tr2;
  CbmMCTrack* mtr;
  Double_t dx;
  Double_t dy;
  
  fEvent++;
  fEntries=0;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEvent);
  InitTree();

  fCellN=fClusters->GetEntriesFast();
  p1=(CbmEcalRecParticle*)fReco->At(0);
  if (p1)
  {
    fE1=p1->E();
    fX1=p1->X();
    fY1=p1->Y();
    fPx1=p1->Px();
    fPy1=p1->Py();
    fPz1=p1->Pz();
    fChi1=p1->Chi2();
    fCellType1=p1->CellType();
    cell=fStr->GetHitCell(p1->CellNum());
    fCellX1=cell->GetCenterX();
    fCellY1=cell->GetCenterY();
    cluster=(CbmEcalCluster*)fClusters->At(p1->ClusterNum());
    fCells=cluster->Size();
    fM=cluster->Moment();
    fMx=cluster->MomentX();
    fMy=cluster->MomentY();
    fEntries++;
  }
  fR=-1111;
  if (n>1)
    p2=(CbmEcalRecParticle*)fReco->At(1);
  else
    p2=NULL;

  fE2=-1111;
  if (p2)
  {
    fE2=p2->E();
    fX2=p2->X();
    fY2=p2->Y();
    fPx2=p2->Px();
    fPy2=p2->Py();
    fPz2=p2->Pz();
    fChi2=p2->Chi2();
    fCellType2=p2->CellType();
    cell=fStr->GetHitCell(p2->CellNum());
    fCellX2=cell->GetCenterX();
    fCellY2=cell->GetCenterY();
    fEntries++;
  }
  else
  {
    fE2=-1111;
    fX2=-1111;
    fY2=-1111;
    fPx2=-1111;
    fPy2=-1111;
    fPz2=-1111;
    fChi2=-1111;
    fCellType2=-1111;
    fCellX2=-1111;
    fCellY2=-1111;
  }
  if (mcn<2)
    Info("Exec", "Less than 2 tracks in input!");
  tr1=NULL;
  tr2=NULL;

  for(i=0;i<mcn;i++)
  {
    mtr=(CbmMCTrack*)fMC->At(i);
    if (mtr->GetMotherId()>=0) continue;
    if (tr1==NULL)
    {
      tr1=mtr;
      continue;
    }
    if (tr2==NULL)
    {
      tr2=mtr;
      continue;
    }
    Info("Exec", "More than 2 initial tracks in input!");
  }

  if (tr1!=NULL&&tr2!=NULL)
  {
    tr1=(CbmMCTrack*)fMC->At(0);
    tr2=(CbmMCTrack*)fMC->At(1);
    dx=tr1->GetStartX()-tr2->GetStartX();
    dy=tr1->GetStartY()-tr2->GetStartY();
    dx*=dx; dy*=dy; fR=TMath::Sqrt(dx+dy);
  }

  fTree->Fill();
  if (fVerbose>0)
    Info("Exec", "%d found particles in event.", fEntries);
}


/** Initializes tree **/
void CbmEcalAnalysisPair::InitTree()
{
  if (fTree) return;
  fTree=new TTree("pair","pair tree");
  fTree->Branch("ev",&fEvent,"ev/I");

  fTree->Branch("e1",&fE1,"e1/D");
  fTree->Branch("px1",&fPx1,"px1/D");
  fTree->Branch("py1",&fPy1,"py1/D");
  fTree->Branch("pz1",&fPz1,"pz1/D");
  fTree->Branch("x1",&fX1,"x1/D");
  fTree->Branch("y1",&fY1,"y1/D");
  fTree->Branch("cx1",&fCellX1,"cx1/D");
  fTree->Branch("cy1",&fCellY1,"cy1/D");
  fTree->Branch("chi1",&fChi1,"chi1/D");
  fTree->Branch("ctype1",&fCellType1,"ctype1/I");

  fTree->Branch("e2",&fE2,"e2/D");
  fTree->Branch("px2",&fPx2,"px2/D");
  fTree->Branch("py2",&fPy2,"py2/D");
  fTree->Branch("pz2",&fPz2,"pz2/D");
  fTree->Branch("x2",&fX2,"x2/D");
  fTree->Branch("y2",&fY2,"y2/D");
  fTree->Branch("cx2",&fCellX2,"cx2/D");
  fTree->Branch("cy2",&fCellY2,"cy2/D");
  fTree->Branch("chi2",&fChi2,"chi2/D");
  fTree->Branch("ctype2",&fCellType2,"ctype2/I");

  fTree->Branch("r", &fR, "r/D");
  fTree->Branch("cn", &fCellN, "cn/I");
  fTree->Branch("cs", &fCells, "cs/I");
  fTree->Branch("m", &fM, "m/D");
  fTree->Branch("mx", &fMx, "mx/D");
  fTree->Branch("my", &fMy, "my/D");
}

CbmEcalAnalysisPair::CbmEcalAnalysisPair()
  : FairTask(),
    fTree(NULL),
    fEntries(0),
    fEvent(0),
    fE1(0.),
    fPx1(0.),
    fPy1(0.),
    fPz1(0.),
    fX1(0.),
    fY1(0.),
    fCellX1(0.),
    fCellY1(0.),
    fCellType1(0),
    fChi1(0.),
    fE2(0.),
    fPx2(0.),
    fPy2(0.),
    fPz2(0.),
    fX2(0.),
    fY2(0.),
    fCellX2(0.),
    fCellY2(0.),
    fCellType2(0),
    fChi2(0.),
    fR(0.),
    fCellN(0),
    fCells(0),
    fM(0.),
    fMx(0.),
    fMy(0.),
    fReco(NULL),
    fMC(NULL),
    fClusters(NULL),
    fStr(NULL)
{
}

CbmEcalAnalysisPair::CbmEcalAnalysisPair(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEntries(0),
    fEvent(0),
    fE1(0.),
    fPx1(0.),
    fPy1(0.),
    fPz1(0.),
    fX1(0.),
    fY1(0.),
    fCellX1(0.),
    fCellY1(0.),
    fCellType1(0),
    fChi1(0.),
    fE2(0.),
    fPx2(0.),
    fPy2(0.),
    fPz2(0.),
    fX2(0.),
    fY2(0.),
    fCellX2(0.),
    fCellY2(0.),
    fCellType2(0),
    fChi2(0.),
    fR(0.),
    fCellN(0),
    fCells(0),
    fM(0.),
    fMx(0.),
    fMy(0.),
    fReco(NULL),
    fMC(NULL),
    fClusters(NULL),
    fStr(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisPair::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fReco=(TClonesArray*)fManager->GetObject("EcalReco");
  if (!fReco)
  {
    Fatal("Init", "Can't find an array of reco parcles.");
    return kFATAL;
  }
  fMC=(TClonesArray*)fManager->GetObject("MCTrack");
  if (!fMC)
  {
    Fatal("Init", "Can't find an array of MC tracks.");
    return kFATAL;
  }
  fClusters=(TClonesArray*)fManager->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find EcalClusters");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisPair::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisPair)
