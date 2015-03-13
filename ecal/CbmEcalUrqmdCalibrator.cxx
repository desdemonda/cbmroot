/* $Id: CbmEcalUrqmdCalibrator.cxx,v 1.3 2006/08/30 13:16:45 prokudin Exp $*/

/*
 * $Log: CbmEcalUrqmdCalibrator.cxx,v $
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

#include "CbmEcalUrqmdCalibrator.h"

#include "CbmEcalInf.h"
#include "CbmEcalPoint.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalStructure.h"

#include "CbmEcalCellMC.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalUrqmdCalibrator::Exec(Option_t* option)
{
  list<CbmEcalPoint*>::const_iterator p;
  fEvent++;
  if (fVerbose>0)
    cout << "Processing event " << fEvent << "." << endl; 
  FillStructure();
  InitTree();
  FillList();

  for(p=fList.begin(); p!=fList.end(); ++p)
  {
    ProcessHit(*p);
  }
}


/** Fills ECAL structure **/
void CbmEcalUrqmdCalibrator::FillStructure()
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
      ; //     cell->AddPSEnergy(pt->GetEnergyLoss()); // No preshower
    else
      cell->AddEnergy(pt->GetEnergyLoss());

    track=(CbmMCTrack*)fMCTracks->At(pt->GetTrackID());
    if (track->GetPdgCode()!=fPDGType) continue;
    if (isPS)
      ;	//     cell->AddTrackPSEnergy(pt->GetTrackID(), pt->GetEnergyLoss()); // No preshower
    else
      cell->AddTrackEnergy(pt->GetTrackID(), pt->GetEnergyLoss());
  } 
}

/** Initializes tree **/
void CbmEcalUrqmdCalibrator::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ECALcal","Ecal Calibration tree");
  fTree->Branch("ev",&fEvent,"ev/I");
  fTree->Branch("steps",&fSteps,"steps/I");
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
  fTree->Branch("e",&fE,"e/F");
  fTree->Branch("e2",&fE2x2,"e2/F");
  fTree->Branch("e3",&fE3x3,"e3/F");
  fTree->Branch("tracke2",&fTrackE2x2,"tracke2/F");
  fTree->Branch("tracke3",&fTrackE3x3,"tracke3/F");
  fTree->Branch("te",&fTotalTrackEnergy,"te/F");
}

/** Populates fList with particles **/
void CbmEcalUrqmdCalibrator::FillList()
{
  CbmEcalPoint* pt;
  CbmMCTrack* track;
  TVector3 mom;
  UInt_t n;

  fList.clear();
  
  n=fEcalPoints->GetEntriesFast();
  for(UInt_t i=0;i<n;i++)
  {
    pt=(CbmEcalPoint*)fEcalPoints->At(i);
    track=(CbmMCTrack*)fMCTracks->At(pt->GetTrackID());
    if (track->GetPdgCode()!=fPDGType)
      continue;
    track->GetMomentum(mom);
    if (mom.Mag2()<fMinEnergy*fMinEnergy)
      continue;
    fList.push_back(pt);
  }
}


/** Returns mass of particle with pdgcode **/
Float_t GetMass(Int_t pdgcode)
{
    Double_t mass=-1;
    TParticlePDG* fParticlePDG=TDatabasePDG::Instance()->GetParticle(pdgcode);

    if (fParticlePDG)
     mass=fParticlePDG->Mass();

    return mass;
}

Int_t CalculateDiffType(CbmEcalCell* cell)
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

/** Do everything for one ecal point**/
void CbmEcalUrqmdCalibrator::ProcessHit(CbmEcalPoint* point)
{
  TVector3 mom;
  TVector3 pos;
  CbmMCTrack* track=(CbmMCTrack*)fMCTracks->At(point->GetTrackID());
  CbmMCTrack* mother;
  CbmEcalCell* cell;
  CbmEcalCell* old_cell=NULL;
  CbmEcalModule* module;


  Float_t calo_x;
  Float_t calo_y;
  Float_t ps_x;
  Float_t ps_y;

  static Float_t sPSDeltaZ=fInf->GetPSLead()+fInf->GetPSScin()/2.0;
  static Float_t sDeltaZ=fInf->GetPSLead()+fInf->GetPSScin()+fInf->GetPSGap();

  point->Momentum(mom);
  point->Position(pos);

  calo_x=pos.X()+pos.X()/pos.Z()*sDeltaZ;
  calo_y=pos.Y()+pos.Y()/pos.Z()*sDeltaZ;
/*
  cout << pos.X() << " " << pos.Y() << endl;
  pos=track->GetMomentum();
  cout << pos.X() << " " << pos.Y() << " " << pos.Z() << endl;
  point->Position(pos);
*/
  ps_x=pos.X()+pos.X()/pos.Z()*sPSDeltaZ;
  ps_y=pos.Y()+pos.Y()/pos.Z()*sPSDeltaZ;

  if (TMath::Abs(calo_x*2.0)>fInf->GetEcalSize(0))
    return;
  if (TMath::Abs(calo_y*2.0)>fInf->GetEcalSize(1))
    return;

  cell=fStr->GetCell(calo_x, calo_y);
  if (cell==NULL)
  {
    cerr << "Can't find cell (" << calo_x << "," << calo_y;
    cerr << ")!" << endl;
    return;
  }

  if (track->GetMotherId()<0)
    fMotherCode=0;
  else
  {
    mother=(CbmMCTrack*)fMCTracks->At(track->GetMotherId());
    fMotherCode=mother->GetPdgCode();
  }
  fSteps=0;
  while(cell!=old_cell)
  {
    old_cell=cell;
    cell=FindNextCell(cell);
    fSteps++;
  }

  fMCX=pos.X();
  fMCY=pos.Y();

  fMCPX=mom.Px();
  fMCPY=mom.Py();
  fMCPZ=mom.Pz();
//  mom=track->GetMomentum();
  fMCE=GetMass(track->GetPdgCode());
  fMCE=TMath::Sqrt(fMCE*fMCE+mom.Mag2());

  fCellX=cell->GetCenterX();
  fCellY=cell->GetCenterY();

  module=fStr->GetModule(fCellX, fCellY);
  fType=module->GetType();

  fDiffType=CalculateDiffType(cell);

  FillEnergies(cell, point->GetTrackID());
  FillTotalEnergy(point->GetTrackID());

  fTree->Fill();
}

/** Calculates ECAL response for track **/
void CbmEcalUrqmdCalibrator::FillTotalEnergy(Int_t tracknum)
{
  static std::list<CbmEcalCell*> cells; 
  if (cells.size()<100)
  {
    cells.clear();
    fStr->GetCells(cells);
  }
  Float_t maxe=0;
  Float_t e;
  CbmEcalCell* cell=NULL;
  std::list<CbmEcalCell*>::const_iterator p;

  fTotalTrackEnergy=0;
  for(p=cells.begin(); p!=cells.end();++p)
  {
    e=((CbmEcalCellMC*)(*p))->GetTrackEnergy(tracknum);
    if (e>maxe)
    {
      cell=(*p);
      maxe=e;
    }
    fTotalTrackEnergy+=e;
  }
  if (cell)
  {
    fMaxCellX=cell->GetCenterX();
    fMaxCellY=cell->GetCenterY();
  } 
  else
  {
    fMaxCellX=0;
    fMaxCellY=0;
  }
}

/** Calculates ECAL response for cell **/
void CbmEcalUrqmdCalibrator::FillEnergies(CbmEcalCell* cell, Int_t trackid)
{
  Float_t en;
  Float_t tracken;
  Float_t maxe;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCellMC* cellmc=(CbmEcalCellMC*)cell;

  cells.clear();

  cell->GetNeighborsList(0,cells);
  fE=cell->GetEnergy();
  en=cell->GetEnergy();
  tracken=cellmc->GetTrackEnergy(trackid);;
  for(p=cells.begin();p!=cells.end();++p) 
  {
    en+=(*p)->GetEnergy();
    tracken+=((CbmEcalCellMC*)(*p))->GetTrackEnergy(trackid);
  }
  fE3x3=en;
  fTrackE3x3=tracken;

  maxe=0;
  for(Int_t i=1;i<5;i++)
  {
    cell->GetNeighborsList(i,cells);
    en=cell->GetEnergy();
    for(p=cells.begin();p!=cells.end();++p) 
      en+=(*p)->GetEnergy();
    if (maxe<en)
    {
      maxe=en;
      tracken=cellmc->GetTrackEnergy(trackid);
      for(p=cells.begin();p!=cells.end();++p) 
	tracken+=((CbmEcalCellMC*)(*p))->GetTrackEnergy(trackid);
     
    }
  }
  fE2x2=maxe;
  fTrackE2x2=tracken;
}

/** Finds cell with more energy deposition than given **/
CbmEcalCell* CbmEcalUrqmdCalibrator::FindNextCell(CbmEcalCell* cell)
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

CbmEcalUrqmdCalibrator::CbmEcalUrqmdCalibrator()
  : FairTask(),
    fPDGType(22),
    fMinEnergy(0.5),
    fGeoFile(""),
    fTree(NULL),
    fEvent(0),
    fSteps(0),
    fMCX(0.),
    fMCY(0.),
    fCellX(0.),
    fCellY(0.),
    fMCE(0.),
    fMCPX(0.),
    fMCPY(0.),
    fMCPZ(0.),
    fE(0.),
    fE2x2(0.),
    fE3x3(0.),
    fTrackE2x2(0.),
    fTrackE3x3(0.),
    fTotalTrackEnergy(0.),
    fMaxCellX(0.),
    fMaxCellY(0.),
    fMotherCode(0),
    fType(0),
    fStr(NULL),
    fDiffType(0),
    fInf(NULL),
    fList(),
    fLitePoints(NULL),
    fEcalPoints(NULL),
    fMCTracks(NULL)
{
}

CbmEcalUrqmdCalibrator::CbmEcalUrqmdCalibrator(const char* name, const Int_t iVerbose, const char* fileGeo)
  : FairTask(name, iVerbose), 
    fPDGType(22),
    fMinEnergy(0.5),
    fGeoFile(fileGeo),
    fTree(NULL),
    fEvent(0),
    fSteps(0),
    fMCX(0.),
    fMCY(0.),
    fCellX(0.),
    fCellY(0.),
    fMCE(0.),
    fMCPX(0.),
    fMCPY(0.),
    fMCPZ(0.),
    fE(0.),
    fE2x2(0.),
    fE3x3(0.),
    fTrackE2x2(0.),
    fTrackE3x3(0.),
    fTotalTrackEnergy(0.),
    fMaxCellX(0.),
    fMaxCellY(0.),
    fMotherCode(0),
    fType(0),
    fStr(NULL),
    fDiffType(0),
    fInf(CbmEcalInf::GetInstance(fGeoFile)),
  fList(),
    fLitePoints(NULL),
    fEcalPoints(NULL),
    fMCTracks(NULL)

{
}

/** Initing routine **/
InitStatus CbmEcalUrqmdCalibrator::Init()
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
void CbmEcalUrqmdCalibrator::SetParContainers()
{
  FairRunAna* ana=FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  rtdb->getContainer("CbmGeoEcalPar");

}

/** Finishing routine **/
void CbmEcalUrqmdCalibrator::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalUrqmdCalibrator)
