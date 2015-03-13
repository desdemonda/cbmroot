#include "CbmEcalAnalysisNeutron.h"

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
void CbmEcalAnalysisNeutron::Exec(Option_t* option)
{
  Int_t i;
  Int_t n=fReco->GetEntriesFast();
  CbmEcalRecParticle* p;
  CbmEcalCluster* cl;
  CbmMCTrack* mtr;
  Double_t dx;
  Double_t dy;
  Double_t cx;
  Double_t cy;
  Double_t r;
  list<CbmEcalCell*>::const_iterator c;
  list<CbmEcalCell*> neib;
  CbmEcalCell* cell;
  
  fEvent++;
  fN=0;
  fEntries=0;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEvent);
  InitTree();

  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(0);
    if (p->MCTrack()<0) continue;
    cl=(CbmEcalCluster*)fClusters->At(p->ClusterNum());
    fMaxs=cl->Maxs(); 
    fM=cl->Moment();
    fMx=cl->MomentX();
    fMy=cl->MomentY();
    mtr=(CbmMCTrack*)fMC->At(p->MCTrack());
    fDiffType=0;
    if ((Int_t)(cl->Type())!=cl->Type()) fDiffType=1;
    fE=p->E();
    fX=p->X();
    fY=p->Y();
    fPx=p->Px();
    fPy=p->Py();
    fPz=p->Pz();
    fChi2=p->Chi2();
    fCellType=p->CellType();
    fPdg=mtr->GetPdgCode();

    cell=fStr->GetHitCell(p->CellNum());
    cx=cell->GetCenterX();
    cy=cell->GetCenterY();
    cell->GetNeighborsList(0, neib);
    fM3=0; fMx3=0; fMy3=0;
    for(c=neib.begin();c!=neib.end();++c)
    {
      dx=cx; dx-=(*c)->GetCenterX(); dx*=dx;
      dy=cy; dy-=(*c)->GetCenterY(); dy*=dy;
      r=dx+dy;
      fM3+=r*(*c)->GetTotalEnergy();
      fMx3+=dx*(*c)->GetTotalEnergy();
      fMy3+=dy*(*c)->GetTotalEnergy();
    }

    fN++;
    fTree->Fill();
  }
  if (fVerbose>0)
    Info("Exec", "%d found particles in event.", fEntries);
}


/** Initializes tree **/
void CbmEcalAnalysisNeutron::InitTree()
{
  if (fTree) return;
  fTree=new TTree("neutron","neutron tree");
  fTree->Branch("ev",&fEvent,"ev/I");
  fTree->Branch("n", &fN, "n/I");

  fTree->Branch("e",&fE,"e/D");
  fTree->Branch("px",&fPx,"px/D");
  fTree->Branch("py",&fPy,"py/D");
  fTree->Branch("pz",&fPz,"pz/D");
  fTree->Branch("x",&fX,"x/D");
  fTree->Branch("y",&fY,"y/D");
  fTree->Branch("chi2",&fChi2,"chi2/D");
  fTree->Branch("ctype",&fCellType,"ctype/I");
  fTree->Branch("cdiff",&fDiffType,"cdiff/I");
  fTree->Branch("pdg", &fPdg, "pdg/I");
  fTree->Branch("cmaxs", &fMaxs, "cmaxs/I");
  fTree->Branch("m3", &fM3, "m3/D");
  fTree->Branch("mx3", &fMx3, "mx3/D");
  fTree->Branch("my3", &fMy3, "my3/D");
  fTree->Branch("m", &fM, "m/D");
  fTree->Branch("mx", &fMx, "mx/D");
  fTree->Branch("my", &fMy, "my/D");
}

CbmEcalAnalysisNeutron::CbmEcalAnalysisNeutron()
  : FairTask(),
    fTree(NULL),
    fEntries(0),
    fN(0),
    fEvent(0),
    fE(0.),
    fPx(0.),
    fPy(0.),
    fPz(0.),
    fX(0.),
    fY(0.),
    fCellType(0),
    fDiffType(0),
    fM3(0.),
    fMx3(0.),
    fMy3(0.),
    fChi2(0.),
    fM(0.),
    fMx(0.),
    fMy(0.),
    fPdg(0),
    fMaxs(0),
    fReco(NULL),
    fMC(NULL),
    fClusters(NULL),
    fStr(NULL)
{
}

CbmEcalAnalysisNeutron::CbmEcalAnalysisNeutron(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEntries(0),
    fN(0),
    fEvent(0),
    fE(0.),
    fPx(0.),
    fPy(0.),
    fPz(0.),
    fX(0.),
    fY(0.),
    fCellType(0),
    fDiffType(0),
    fM3(0.),
    fMx3(0.),
    fMy3(0.),
    fChi2(0.),
    fM(0.),
    fMx(0.),
    fMy(0.),
    fPdg(0),
    fMaxs(0),
    fReco(NULL),
    fMC(NULL),
    fClusters(NULL),
    fStr(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisNeutron::Init()
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
void CbmEcalAnalysisNeutron::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisNeutron)
