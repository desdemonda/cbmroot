/* $Id: CbmEcalAnalysisDS.cxx,v 1.9 2006/09/18 07:58:04 prokudin Exp $*/

/*
 * $Log: CbmEcalAnalysisDS.cxx,v $
 * Revision 1.9  2006/09/18 07:58:04  prokudin
 * First implementation of SCurve library
 *
 * Revision 1.3  2006/08/30 13:16:45  prokudin
 * Bug huntong
 *
 * Revision 1.2  2006/08/25 19:17:48  prokudin
 * Energy in cell added. Minor bug fixed.
 *
 * Revision 1.1  2006/08/17 18:16:35  prokudin
 * first version of UrqmdCalibrator
 *
 */

#include "CbmEcalAnalysisDS.h"

#include "CbmEcalInf.h"
#include "CbmEcalPoint.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalStructure.h"
#include "CbmEcalSCurveLib.h"
#include "CbmEcalCellMC.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

using std::cout;
using std::cerr;
using std::endl;
using std::list;


/** Loop procedure **/
void CbmEcalAnalysisDS::Exec(Option_t* option)
{
  list<CbmEcalPoint*>::const_iterator p;
  fEvent++;
  if (fVerbose>0)
    cout << "Processing event " << fEvent << "." << endl; 
  FillStructure();
  InitTree();
  FindPeaks();

}


/** Fills ECAL structure **/
void CbmEcalAnalysisDS::FillStructure()
{
  CbmEcalPointLite* pt;
  CbmMCTrack* track;
  CbmEcalCellMC* cell;
  Int_t ten;
  UInt_t n;
  Bool_t isPS;

  fStr->ResetModules();

  n=fLitePoints->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << "Number of input lite ECAL points " << n;
    cout << "." << endl;
  }
  for(UInt_t j=0; j<n; j++)
  {
    pt=(CbmEcalPointLite*)fLitePoints->At(j);
    cell=(CbmEcalCellMC*)fStr->GetCell(pt->GetDetectorID(), ten, isPS);
    if (pt->GetEnergyLoss()<0)
      cout << "Here" << pt->GetEnergyLoss() << endl;
    if (ten!=0) continue;
    if (isPS)
      ; // cell->AddPSEnergy(pt->GetEnergyLoss()); preshower removed
    else
      cell->AddEnergy(pt->GetEnergyLoss());

    track=(CbmMCTrack*)fMCTracks->At(pt->GetTrackID());
    if (track->GetPdgCode()!=fPDGType) continue;
    if (isPS)
      ; // cell->AddTrackPSEnergy(pt->GetTrackID(), pt->GetEnergyLoss()); preshower removed
    else
      cell->AddTrackEnergy(pt->GetTrackID(), pt->GetEnergyLoss());
  } 
}

/** Initializes tree **/
void CbmEcalAnalysisDS::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ECALcal","Ecal Calibration tree");
  fTree->Branch("ev",&fEvent,"ev/I");
  fTree->Branch("type",&fType,"type/I");
  fTree->Branch("dtype",&fDiffType,"dtype/I");
  fTree->Branch("mcode",&fMotherCode,"mcode/I");
  fTree->Branch("mcx",&fMCX,"mcx/F");
  fTree->Branch("mcy",&fMCY,"mcy/F");
  fTree->Branch("cellx",&fCellX,"cellx/F");
  fTree->Branch("celly",&fCellY,"celly/F");
  fTree->Branch("maxcellx",&fMaxCellX,"maxcellx/F");
  fTree->Branch("maxcelly",&fMaxCellY,"maxcelly/F");
  fTree->Branch("mce",&fMCE,"mce/F");
  fTree->Branch("mcpx",&fMCPX,"mcpx/F");
  fTree->Branch("mcpy",&fMCPY,"mcpy/F");
  fTree->Branch("mcpz",&fMCPZ,"mcpz/F");
  fTree->Branch("pse",&fPSE,"pse/F");
  fTree->Branch("e",&fE,"e/F");
  fTree->Branch("e2",&fE2x2,"e2/F");
  fTree->Branch("e3",&fE3x3,"e3/F");
  fTree->Branch("te",&fTotalTrackEnergy,"te/F");
  fTree->Branch("ereco",&fEReco,"ereco/F");
  fTree->Branch("ax",&fAX,"ax/F");
  fTree->Branch("ay",&fAY,"ay/F");
  fTree->Branch("xreco",&fXReco,"xreco/F");
  fTree->Branch("yreco",&fYReco,"yreco/F");
  fTree->Branch("nd",&fND,"nd/F");
  fTree->Branch("ncode",&fNCode,"ncode/I");
}

/** Populates fList with particles **/
void CbmEcalAnalysisDS::FindPeaks()
{
  list<CbmEcalCell*> all;
  list<CbmEcalCell*> neib;
  list<CbmEcalCell*>::const_iterator pt;
  list<CbmEcalCell*>::const_iterator pv;

  fStr->GetCells(all);

  for(pt=all.begin();pt!=all.end();++pt)
  {
    neib.clear();
    (*pt)->GetNeighborsList(0, neib);
    for(pv=neib.begin();pv!=neib.end();++pv)
      if ((*pv)->GetEnergy()>(*pt)->GetEnergy()) 
	break;
    if (pv!=neib.end())
      continue;
    FillEnergies(*pt);
  }
}

/** Returns incoming particle energy **/
Float_t CbmEcalAnalysisDS::GetEnergy(Float_t e2, CbmEcalCell* cell)
{
  Int_t type=cell->GetType();
  Float_t x=cell->GetCenterX();
  Float_t y=cell->GetCenterY();
  Float_t tantheta=TMath::Sqrt(x*x+y*y)/fInf->GetZPos();
  Float_t a=fP0a[type]+fP1a[type]*tantheta;
  Float_t b=fP0b[type]+fP1b[type]*tantheta;
  Float_t d=TMath::Sqrt(a*a+4.0*e2*b);
  Float_t res=(-a+d)/2.0/b;
  return res*res;
}

/** Returns mass of particle with pdgcode **/
Float_t __GetMass(Int_t pdgcode)
{
    Double_t mass=-1;
    TParticlePDG* fParticlePDG=TDatabasePDG::Instance()->GetParticle(pdgcode);

    if (fParticlePDG)
     mass=fParticlePDG->Mass();

    return mass;
}

Int_t __CalculateDiffType(CbmEcalCell* cell)
{
  Float_t dx=cell->GetX2()-cell->GetX1();
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;

  cells.clear();
  cell->GetNeighborsList(0, cells);
  for(p=cells.begin(); p!=cells.end(); ++p)
   if (TMath::Abs((*p)->GetX2()-(*p)->GetX1()-dx)>0.001)
     return 1;

  return 0;
}

/** Calculates ECAL response for cell **/
void CbmEcalAnalysisDS::FillEnergies(CbmEcalCell* cell)
{
  Float_t en;
  Float_t maxe;
  Int_t type;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;

  cells.clear();

  fType=cell->GetType();
  fDiffType=0;

  cell->GetNeighborsList(0,cells);
  fE=cell->GetEnergy();
  en=cell->GetEnergy();
  type=cell->GetType();
  fAX=0; fAY=0;
  for(p=cells.begin();p!=cells.end();++p) 
  {
    if (type!=(*p)->GetType())
    {
      fAX=-1111;
      fAY=-1111;
      type=-1111;
      fDiffType=1;
    } else
    {
      if (cell->GetCenterX()+0.001<(*p)->GetCenterX())
	fAX+=(*p)->GetEnergy();
      else
      if (cell->GetCenterX()-0.001>(*p)->GetCenterX())
	fAX-=(*p)->GetEnergy();
      if (cell->GetCenterY()+0.001<(*p)->GetCenterY())
	fAY+=(*p)->GetEnergy();
      else
      if (cell->GetCenterY()-0.001>(*p)->GetCenterY())
	fAY-=(*p)->GetEnergy();
    }
    en+=(*p)->GetEnergy();
  }
  fE3x3=en;

  if (fAX!=-1111)
  {
    fAX/=fE3x3;
    fAY/=fE3x3;
  }

  maxe=0;
  for(Int_t i=1;i<5;i++)
  {
    cell->GetNeighborsList(i,cells);
    en=cell->GetEnergy();
    for(p=cells.begin();p!=cells.end();++p) 
      en+=(*p)->GetEnergy();
    if (maxe<en)
      maxe=en;
  }
  fE2x2=maxe;

  fCellX=cell->GetCenterX();
  fCellY=cell->GetCenterY();
  
  fPSE=0;

  if (fE2x2<0.05) return; 
  fEReco=GetEnergy(fE2x2+fPSE, cell);
  if (fAX!=-1111)
    fXReco=fLib->GetX(fAX, fEReco, cell);
  else
    fXReco=-1111;
  if (fXReco!=-1111)
    fXReco+=cell->GetCenterX();
  
  if (fAY!=-1111)
    fYReco=fLib->GetY(fAY, fEReco, cell);
  else
    fYReco=-1111;
  
  if (fYReco!=-1111)
    fYReco+=cell->GetCenterY();
  if ((fYReco==-1111||fXReco==-1111)&&fXReco!=-1111)
  {
     cout << fXReco << "  " << fAX << "	" << fYReco << "	" << fAY << endl;
     cout << "->" << fMCE << " " << fEReco << endl;
  }
  FindND();
}

/** Calculate distance to nearest track **/
void CbmEcalAnalysisDS::FindND()
{
  CbmEcalPoint* pt;
  CbmEcalPoint* mp=NULL;
  CbmMCTrack* track;
  TVector3 mom;
  Float_t dx;
  Float_t dy;
  Float_t dist;
  Float_t ne;
 
  UInt_t n;

  ne=1111;
  fND=1111;
  n=fEcalPoints->GetEntriesFast();
  for(UInt_t i=0;i<n;i++)
  {
    pt=(CbmEcalPoint*)fEcalPoints->At(i);
    track=(CbmMCTrack*)fMCTracks->At(pt->GetTrackID());
    //Exclude gammas, neutrons and Kl
    if (track->GetPdgCode()!=22&&TMath::Abs(track->GetPdgCode())==11)
      continue;
    dx=pt->GetX()-fXReco;
    dy=pt->GetY()-fYReco;
    dist=TMath::Sqrt(dx*dx+dy*dy);
    if (dist>20)
      continue;
    track->GetMomentum(mom);
    if (TMath::Abs(mom.Mag()/fEReco-1)<ne)
    {
      ne=TMath::Abs(mom.Mag()/fEReco-1);
      fND=dist;
      fNCode=track->GetPdgCode();
      mp=pt;
    }
  }
  pt=mp;
  if (fND==1111)
  {
    fMCE=-1111;
    fMCX=-1111;
    fMCY=-1111;
    fMCPX=-1111;
    fMCPY=-1111;
    fMCPZ=-1111;
    fND=-1111;
  }
  else
  {
    track=(CbmMCTrack*)fMCTracks->At(mp->GetTrackID());
    track->GetMomentum(mom);
    fMCE=mom.Mag();
    fMCX=pt->GetX();
    fMCY=pt->GetY();
    fMCPX=mom.Px();
    fMCPY=mom.Py();
    fMCPZ=mom.Pz();
  }
  fTree->Fill();
}

/** Finds cell with more energy deposition than given **/
CbmEcalCell* CbmEcalAnalysisDS::FindNextCell(CbmEcalCell* cell)
{
  CbmEcalCell* ret=cell;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;

  cells.clear();

  cell->GetNeighborsList(0,cells);
  for(p=cells.begin();p!=cells.end();++p) 
  {
    if ((*p)->GetEnergy()>ret->GetEnergy())
      ret=(*p);
  }

  return ret;
}

CbmEcalAnalysisDS::CbmEcalAnalysisDS(const char* name, const Int_t iVerbose, const char* fileGeo)
  : FairTask(name, iVerbose), 
    fPDGType(22), 
    fMinEnergy(0.5), 
    fGeoFile(fileGeo),
    fTree(NULL),
    fEvent(-1),
    fMCX(0.),
    fMCY(0.),
    fCellX(0.),
    fCellY(0.),
    fP0a(),
    fP0b(),
    fP1a(),
    fP1b(),
    fMCE(0.),
    fMCPX(0.),
    fMCPY(0.),
    fMCPZ(0.),
    fPSE(0.),
    fE(0.),
    fE2x2(0.),
    fE3x3(0.),
    fTotalTrackEnergy(0.),
    fMaxCellX(0.),
    fMaxCellY(0.),
    fEReco(0.),
    fAX(0.),
    fAY(0.),
    fXReco(0.),
    fYReco(0.),
    fND(0.),
    fNCode(-1),
    fMotherCode(-1),
    fType(-1),
    fStr(NULL),
    fDiffType(-1),
    fInf(NULL),
    fList(),
    fLib(NULL),
    fLitePoints(NULL),
    fEcalPoints(NULL),
    fMCTracks(NULL)
{
  fTree=NULL;
  fInf=CbmEcalInf::GetInstance(fGeoFile);
  fLib=new CbmEcalSCurveLib(0);
  fLib->AddFile("3x3.root");
  fLib->AddFile("6x6.root");
  fLib->AddFile("12x12.root");

  fP0a[1]=0.002833;
  fP0a[2]=0.003345;
  fP0a[4]=0.003587;
  fP0b[1]=0.08788;
  fP0b[2]=0.08496;
  fP0b[4]=0.07886;

  fP1a[1]=0.003398;
  fP1a[2]=0.003108;
  fP1a[4]=0.001337;
  fP1b[1]=-0.005302;
  fP1b[2]=-0.005155;
  fP1b[4]=-0.006937;
}

/** Initing routine **/
InitStatus CbmEcalAnalysisDS::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();

  fLitePoints=(TClonesArray*)fManager->GetObject("EcalPointLite");
  if (fLitePoints==NULL)
  {
    cerr << "There are no EcalPointLite branch in the file!!!" << endl;
    return kFATAL; 
  }
  fEcalPoints=(TClonesArray*)fManager->GetObject("EcalPoint");
  if (fEcalPoints==NULL)
  {
    cerr << "There are no EcalPoint branch in the file!!!" << endl;
    return kFATAL; 
  }
  fMCTracks=(TClonesArray*)fManager->GetObject("MCTrack");
  if (fMCTracks==NULL)
  {
    cerr << "There are no MCTrack branch in the file!!!" << endl;
    return kFATAL;
  }

  fTree=NULL;

  fEvent=0;
  fStr=new CbmEcalStructure(fInf);
  fStr->Construct();

  return kSUCCESS;
}

/** Parameter container init **/
void CbmEcalAnalysisDS::SetParContainers()
{
  FairRunAna* ana=FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  rtdb->getContainer("CbmGeoEcalPar");

}

/** Finishing routine **/
void CbmEcalAnalysisDS::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisDS)
