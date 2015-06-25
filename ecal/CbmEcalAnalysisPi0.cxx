#include "CbmEcalAnalysisPi0.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"

#include "CbmEcalRecParticle.h"
#include "CbmEcalCell.h"
#include "CbmEcalCluster.h"
#include "CbmEcalStructure.h"
#include "CbmEcalPoint.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalAnalysisPi0::Exec(Option_t* option)
{
  Int_t i;
  Int_t n=fMC->GetEntriesFast();
  CbmMCTrack* mtr;

  fEvent++;
  for(i=0;i<n;i++)
  {
    InitTree();
    mtr=(CbmMCTrack*)fMC->At(i);
    // Only eta and pi0
    if (mtr->GetPdgCode()!=111&&mtr->GetPdgCode()!=221) continue;
    fMotherID=mtr->GetPdgCode();
    fMX=mtr->GetStartX();
    fMY=mtr->GetStartY();
    fMZ=mtr->GetStartZ();
    fMPx=mtr->GetPx();
    fMPy=mtr->GetPy();
    fMPz=mtr->GetPz();
    fME=mtr->GetEnergy();
    fMPhi=TMath::ATan2(fMPy, fMPx)*TMath::RadToDeg();
    fMTheta=TMath::ATan2(TMath::Sqrt(fMPy*fMPy+fMPx*fMPx), fMPz)*TMath::RadToDeg();    
    FindDaughters(i);
    GetPoints();
    // Only two daughter tracks
    if (fN3==-1111)
      GetInfo();
     
    fEntries++;
    fTree->Fill();
  }
  if (fVerbose>0)
    Info("Exec", "%d found particles in event.", fEntries);
}

/** Get information about MC ecal points **/
void CbmEcalAnalysisPi0::GetPoints()
{
  Int_t i;
  Int_t n=fPoints->GetEntriesFast();
  CbmEcalPoint* pt;
  
  for(i=0;i<n;i++)
  {
    pt=(CbmEcalPoint*)fPoints->At(i);
    if (pt->GetTrackID()==fN1)
    {
      fMCX1=pt->GetX();
      fMCY1=pt->GetY();
    }
    if (pt->GetTrackID()==fN2)
    {
      fMCX2=pt->GetX();
      fMCY2=pt->GetY();
    }
  }
}
/** Finds daughters of MC track **/
void CbmEcalAnalysisPi0::FindDaughters(Int_t mother)
{
  Int_t i;
  Int_t n=fMC->GetEntriesFast();
  CbmMCTrack* mtr;

  for(i=0;i<n;i++)
  {
    mtr=(CbmMCTrack*)fMC->At(i);
    if (mtr->GetMotherId()==mother)
    {
      if (fN1==-1111) fN1=i; else
      if (fN2==-1111) fN2=i; else
      { 
	fN3=i;
	return;
      }
    }
  }
}

/** Get information for daughter tracks **/
void CbmEcalAnalysisPi0::GetInfo()
{
  CbmMCTrack* tr1=NULL;
  if (fN1>=0)
  {
    tr1=(CbmMCTrack*)fMC->At(fN1);
    fMCE1=tr1->GetEnergy(); fMCPx1=tr1->GetPx(); fMCPy1=tr1->GetPy(); fMCPz1=tr1->GetPz();
    fPdg1=tr1->GetPdgCode();
  }

  CbmMCTrack* tr2=NULL;
  if (fN2>=0)
  {
    tr2=(CbmMCTrack*)fMC->At(fN2);
    fPdg2=tr2->GetPdgCode();
    fMCE2=tr2->GetEnergy(); fMCPx2=tr2->GetPx(); fMCPy2=tr2->GetPy(); fMCPz2=tr2->GetPz();
  }
  TLorentzVector v1;
  TLorentzVector v2;
  TLorentzVector p;

  if (fN1>=0&&fN2>=0)
  {
    v1.SetPxPyPzE(fMCPx1, fMCPy1, fMCPz1, fMCE1);
    v2.SetPxPyPzE(fMCPx2, fMCPy2, fMCPz2, fMCE2);
    p=v1; p+=v2;
    fMCM=p.M();
  }

  CbmEcalRecParticle* p1=NULL;
  CbmEcalRecParticle* p2=NULL;
  CbmEcalRecParticle* rp=NULL;

  Int_t n=fReco->GetEntriesFast();
  Int_t i;
  CbmEcalCell* cell;
  CbmEcalCluster* cls;
 
  for(i=0;i<n;i++)
  {
    rp=(CbmEcalRecParticle*)fReco->At(i);

    if (rp->MCTrack()==fN1&&fN1>=0)
    {
      if (p1==NULL) p1=rp;
      else
      {
	Info("Exec", "Two reconstructed partiles belong to the same MC track %d.", p1->MCTrack());
	continue;
      }
    }
    if (rp->MCTrack()==fN2&&fN2>=0)
    {
      if (p2==NULL) p2=rp;
      else
      {
	Info("Exec", "Two reconstructed partiles belong to the same MC track %d.", p2->MCTrack());
	continue;
      }
    }
  }
  if (p1!=NULL)
  {
    fE1=p1->E(); fPx1=p1->Px(); fPy1=p1->Py(); fPz1=p1->Pz();
    cls=(CbmEcalCluster*)fClusters->At(p1->ClusterNum());
    fX1=p1->X(); fY1=p1->Y(); fChi21=cls->Chi2();
    cell=fStr->GetHitCell(p1->CellNum());
    fCellX1=cell->GetCenterX(); fCellY1=cell->GetCenterY();
    fCellType1=cell->GetType();
    v1.SetPxPyPzE(fPx1, fPy1, fPz1, fE1);
  }
  if (p2!=NULL)
  {
    fE2=p2->E(); fPx2=p2->Px(); fPy2=p2->Py(); fPz2=p2->Pz();
    cls=(CbmEcalCluster*)fClusters->At(p2->ClusterNum());
    fX2=p2->X(); fY2=p2->Y(); fChi22=cls->Chi2();
    cell=fStr->GetHitCell(p2->CellNum());
    fCellX2=cell->GetCenterX(); fCellY2=cell->GetCenterY();
    fCellType2=cell->GetType();
    v2.SetPxPyPzE(fPx2, fPy2, fPz2, fE2);
  }
  if (p1!=NULL&&p2!=NULL)
  {
    p=v1; p+=v2;
    fM=p.M();
  }
}

/** Initializes tree **/
void CbmEcalAnalysisPi0::InitTree()
{
  fE1=-1111; fPx1=-1111; fPy1=-1111; fPz1=-1111; fMCE1=-1111; fMCPx1=-1111; fMCPy1=-1111; fMCPz1=-1111;
  fX1=-1111; fY1=-1111;  fCellX1=-1111; fCellY1=-1111; fChi21=-1111; fCellType1=0; fPdg1=-1111;
  fR1=-1111; fMCX1=-1111; fMCY1=-1111;

  fE2=-1111; fPx2=-1111; fPy2=-1111; fPz2=-1111; fMCE2=-1111; fMCPx2=-1111; fMCPy2=-1111; fMCPz2=-1111;
  fX2=-1111; fY2=-1111;  fCellX2=-1111; fCellY2=-1111; fChi22=-1111; fCellType2=0; fPdg2=-1111;
  fR2=-1111; fMCX2=-1111; fMCY2=-1111;

  fN1=-1111; fN2=-1111; fN3=-1111;

  fM=-1111; fMCM=-1111;

  if (fTree) return;
  fTree=new TTree("Pi0Tree","Pi0 and eta tree");
  fTree->Branch("ev",&fEvent,"ev/I");
  fTree->Branch("mother",&fMotherID,"mother/I");
  fTree->Branch("m",&fM,"m/D");
  fTree->Branch("mtheta",&fMTheta,"mtheta/D");
  fTree->Branch("mphi",&fMPhi,"mphi/D");
  fTree->Branch("mx",&fMX,"mx/D");
  fTree->Branch("my",&fMY,"my/D");
  fTree->Branch("mz",&fMZ,"mz/D");
  fTree->Branch("mpx",&fMPx,"mpx/D");
  fTree->Branch("mpy",&fMPy,"mpy/D");
  fTree->Branch("mpz",&fMPz,"mpz/D");
  fTree->Branch("me",&fME,"me/D");
  fTree->Branch("mcm",&fMCM,"mcm/D");

  fTree->Branch("e1",&fE1,"e1/D");
  fTree->Branch("px1",&fPx1,"px1/D");
  fTree->Branch("py1",&fPy1,"py1/D");
  fTree->Branch("pz1",&fPz1,"pz1/D");
  fTree->Branch("mce1",&fMCE1,"mce1/D");
  fTree->Branch("mcpx1",&fMCPx1,"mcpx1/D");
  fTree->Branch("mcpy1",&fMCPy1,"mcpy1/D");
  fTree->Branch("mcpz1",&fMCPz1,"mcpz1/D");
  fTree->Branch("x1",&fX1,"x1/D");
  fTree->Branch("y1",&fY1,"y1/D");
  fTree->Branch("mcx1",&fMCX1,"mcx1/D");
  fTree->Branch("mcy1",&fMCY1,"mcy1/D");
  fTree->Branch("r1",&fR1,"r1/D");
  fTree->Branch("cx1",&fCellX1,"cx1/D");
  fTree->Branch("cy1",&fCellY1,"cy1/D");
  fTree->Branch("chi21",&fChi21,"chi21/D");
  fTree->Branch("ctype1",&fCellType1,"ctype1/I");
  fTree->Branch("pdg1",&fPdg1,"pdg1/I");

  fTree->Branch("e2",&fE2,"e2/D");
  fTree->Branch("px2",&fPx2,"px2/D");
  fTree->Branch("py2",&fPy2,"py2/D");
  fTree->Branch("pz2",&fPz2,"pz2/D");
  fTree->Branch("mce2",&fMCE2,"mce2/D");
  fTree->Branch("mcpx2",&fMCPx2,"mcpx2/D");
  fTree->Branch("mcpy2",&fMCPy2,"mcpy2/D");
  fTree->Branch("mcpz2",&fMCPz2,"mcpz2/D");
  fTree->Branch("x2",&fX2,"x2/D");
  fTree->Branch("y2",&fY2,"y2/D");
  fTree->Branch("mcx2",&fMCX2,"mcx2/D");
  fTree->Branch("mcy2",&fMCY2,"mcy2/D");
  fTree->Branch("r2",&fR2,"r2/D");
  fTree->Branch("cx2",&fCellX2,"cx2/D");
  fTree->Branch("cy2",&fCellY2,"cy2/D");
  fTree->Branch("chi22",&fChi22,"chi22/D");
  fTree->Branch("ctype2",&fCellType2,"ctype2/I");
  fTree->Branch("pdg2",&fPdg2,"pdg2/I");
}

CbmEcalAnalysisPi0::CbmEcalAnalysisPi0()
  : FairTask(),
    fTree(NULL),
    fEntries(0),
    fEvent(0),
    fE1(0.),
    fPx1(0.),
    fPy1(0.),
    fPz1(0.),
    fMCE1(0.),
    fMCPx1(0.),
    fMCPy1(0.),
    fMCPz1(0.),
    fMCX1(0.),
    fMCY1(0.),
    fX1(0.),
    fY1(0.),
    fCellX1(0.),
    fCellY1(0.),
    fR1(0.),
    fChi21(0.),
    fCellType1(0),
    fPdg1(0),
    fN1(0),
    fE2(0.),
    fPx2(0.),
    fPy2(0.),
    fPz2(0.),
    fMCE2(0.),
    fMCPx2(0.),
    fMCPy2(0.),
    fMCPz2(0.),
    fMCX2(0.),
    fMCY2(0.),
    fX2(0.),
    fY2(0.),
    fCellX2(0.),
    fCellY2(0.),
    fR2(0.),
    fChi22(0.),
    fCellType2(0),
    fPdg2(0),
    fPdg3(0),
    fN2(0),
    fN3(0),
    fM(0.),
    fMCM(0.),
    fMotherID(0),
    fMX(0.),
    fMY(0.),
    fMZ(0.),
    fMPx(0.),
    fMPy(0.),
    fMPz(0.),
    fME(0.),
    fMTheta(0.),
    fMPhi(0.),
    fReco(NULL),
    fMC(NULL),
    fTracks(NULL),
    fClusters(NULL),
    fStr(NULL),
    fPoints(NULL)
{
}

CbmEcalAnalysisPi0::CbmEcalAnalysisPi0(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEntries(0),
    fEvent(0),
    fE1(0.),
    fPx1(0.),
    fPy1(0.),
    fPz1(0.),
    fMCE1(0.),
    fMCPx1(0.),
    fMCPy1(0.),
    fMCPz1(0.),
    fMCX1(0.),
    fMCY1(0.),
    fX1(0.),
    fY1(0.),
    fCellX1(0.),
    fCellY1(0.),
    fR1(0.),
    fChi21(0.),
    fCellType1(0),
    fPdg1(0),
    fN1(0),
    fE2(0.),
    fPx2(0.),
    fPy2(0.),
    fPz2(0.),
    fMCE2(0.),
    fMCPx2(0.),
    fMCPy2(0.),
    fMCPz2(0.),
    fMCX2(0.),
    fMCY2(0.),
    fX2(0.),
    fY2(0.),
    fCellX2(0.),
    fCellY2(0.),
    fR2(0.),
    fChi22(0.),
    fCellType2(0),
    fPdg2(0),
    fPdg3(0),
    fN2(0),
    fN3(0),
    fM(0.),
    fMCM(0.),
    fMotherID(0),
    fMX(0.),
    fMY(0.),
    fMZ(0.),
    fMPx(0.),
    fMPy(0.),
    fMPz(0.),
    fME(0.),
    fMTheta(0.),
    fMPhi(0.),
    fReco(NULL),
    fMC(NULL),
    fTracks(NULL),
    fClusters(NULL),
    fStr(NULL),
    fPoints(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisPi0::Init()
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
  fTracks=(TClonesArray*)fManager->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find array of reconstructed tracks");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fClusters=(TClonesArray*)fManager->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find array of clusters");
    return kFATAL;
  }
  fPoints=(TClonesArray*)fManager->GetObject("EcalPoint");
  if (fPoints==NULL)
  {
    Fatal("Init", "Can't find array of ecal points.");
    return kFATAL; 
  }
  fTree=NULL;


  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisPi0::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisPi0)
