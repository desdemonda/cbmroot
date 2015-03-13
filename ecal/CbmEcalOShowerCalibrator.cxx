#include "CbmEcalOShowerCalibrator.h"

#include "CbmGeoEcalPar.h"
#include "CbmEcalPoint.h"
#include "CbmEcalPointLite.h"
#include "CbmEcal.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TTree.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

using std::cout;
using std::cerr;
using std::endl;
using std::flush;

CbmEcalOShowerCalibrator::CbmEcalOShowerCalibrator()
	: FairTask(), 
	  fZPosition(-2.),
	  fPDGCode(22),
	  fLowEnergy(0.1),
	  fLitePoints(NULL),
	  fEcalPoints(NULL),
	  fMCTracks(NULL),
	  fTree(NULL),
	  fOutMCX(0.),
	  fOutMCY(0.),
	  fOutDX(0),
	  fOutDY(0),
	  fCellSize(0.),
	  fOutMCE(0.),
	  fOutMCPx(0.),
	  fOutMCPy(0.),
	  fOutMCPz(0.),
	  fEvent(0),
	  fLocalEnergy(NULL),
	  fLocalPSEnergy(NULL),
	  fLocalSize(0),
	  fLocalL(0),
	  fModuleSize(0.),
	  fLocalXSize(0),
	  fLocalYSize(0),
	  fLocalMCX(0.),
	  fLocalMCY(0.),
	  fMCX(0.),
	  fMCY(0.),
	  fCount(0),
	  fPar(NULL)
{
}
CbmEcalOShowerCalibrator::CbmEcalOShowerCalibrator(const char* name, const Int_t iVerbose)
	: FairTask(name, iVerbose), 
	  fZPosition(-2.),
	  fPDGCode(22),
	  fLowEnergy(0.1),
	  fLitePoints(NULL),
	  fEcalPoints(NULL),
	  fMCTracks(NULL),
	  fTree(NULL),
	  fOutMCX(0.),
	  fOutMCY(0.),
	  fOutDX(0),
	  fOutDY(0),
	  fCellSize(0.),
	  fOutMCE(0.),
	  fOutMCPx(0.),
	  fOutMCPy(0.),
	  fOutMCPz(0.),
	  fEvent(0),
	  fLocalEnergy(NULL),
	  fLocalPSEnergy(NULL),
	  fLocalSize(0),
	  fLocalL(0),
	  fModuleSize(0.),
	  fLocalXSize(0),
	  fLocalYSize(0),
	  fLocalMCX(0.),
	  fLocalMCY(0.),
	  fMCX(0.),
	  fMCY(0.),
	  fCount(0),
	  fPar(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalOShowerCalibrator::Init()
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

  fCellSize=fPar->GetVariableStrict("cellsize");
  fModuleSize=fPar->GetVariableStrict("modulesize");

  //Fixing local sizes according with shower size
  //and simulation cell size of 1mm
  fLocalXSize=120;
  fLocalYSize=120;

  cout << "Inited for transport cell size " << fCellSize;
  cout << ", module size " << fModuleSize << endl;
  cout << "Size of local array: x " << fLocalXSize;
  cout << ", y " << fLocalYSize << endl;

  fLocalL=3*fLocalXSize+1; fLocalSize=fLocalL; fLocalSize*=fLocalL;
  fLocalEnergy=new Float_t[fLocalSize];
  fLocalPSEnergy=new Float_t[fLocalSize];
  
  cout << "Local array size " << fLocalSize << endl;
  fEvent=0;
  return kSUCCESS;
}

/** Clear fLocalEnergy and fLocalPSEnergy **/
void CbmEcalOShowerCalibrator::ClearLocal()
{
  if (fVerbose>10)
    cout << "Clearing local arrays..." << flush;
  for(Int_t i=0;i<fLocalSize;i++)
  {
    fLocalEnergy[i]=0;
    fLocalPSEnergy[i]=0;
  }
  if (fVerbose>10)
    cout << " Done." << endl << flush;
}

/** Loop procedure **/
void CbmEcalOShowerCalibrator::Exec(Option_t* option)
{
  /** Just because now it is assured we are in
   ** output file **/
  if (fTree==NULL)
    InitTree();

  fEvent++;
  if (fVerbose>0)
    cout << "Processing event " << fEvent << "." << endl;
  ClearLocal();
  GetMCParameters();
  if (fCount!=1) 
  {
    cerr << "Event " << fEvent << "." << endl;
    cerr << "->Found " << fCount << " suitable events.";
    cerr << " Can't continue with this event. Skipping event." << endl;
    return;
  }
  FillLocal();

  if (fVerbose>10)
    cout << "Constructing output matrices... " << flush;
  fTree->Fill();
  if (fVerbose>10)
    cout << "Done." << flush;
}

/** Fill fLocalEnergy and fLocalPSEnergy **/
void CbmEcalOShowerCalibrator::FillLocal()
{
  CbmEcalPointLite* p;
  Int_t n;
  Bool_t isPS;
  Float_t x;
  Float_t y;
  Int_t ten;
  Int_t num;
 
  n=fLitePoints->GetEntriesFast();
  if (fVerbose>10)
    cout << "Filling local arrays... " << flush;
  if (fVerbose>1)
    cout << "Found " << n << " MCPoints in event." << endl;

  for(Int_t i=0;i<n;i++)
  {
    p=(CbmEcalPointLite*)fLitePoints->At(i);
    isPS=CbmEcal::GetCellCoord(p->GetDetectorID(),x,y,ten);
    if (ten!=0) continue;
//    cout << x << "	" << y << "	" << fMCX << "	" << fMCY << endl;
    x+=fCellSize/2.0-fMCX;
    y+=fCellSize/2.0-fMCY;
    num=GetLocalNumber(x,y);
    if (num==-1) continue;

    if (isPS)
      fLocalPSEnergy[num]+=p->GetEnergyLoss();
    else
      fLocalEnergy[num]+=p->GetEnergyLoss();
  }
  if (fVerbose>10)
    cout << " Done." << endl;
}

Int_t CbmEcalOShowerCalibrator::GetLocalNumber(Float_t x, Float_t y) const 
{
  static Float_t dx=fCellSize;
  static Float_t dy=fCellSize;
  static Int_t idx=fLocalXSize;
  static Int_t idy=fLocalYSize;

  Int_t ix=(Int_t)(x/dx+0.5)+idx;
  if (x/dx+0.5<0) ix--;
  Int_t iy=(Int_t)(y/dy+0.5)+idy;
  if (y/dy+0.5<0) iy--;

  if (ix<0||iy<0) return -1;
  if (ix>=fLocalL) return -1;
  if (iy>=fLocalL) return -1;

  return ix+iy*fLocalL;
}

/** Get MC parameters of incoming particle **/
void CbmEcalOShowerCalibrator::GetMCParameters()
{
  CbmEcalPoint* p;
  CbmMCTrack* track;
  TVector3 mom;
  Int_t n;
  Int_t count=0;
  Float_t mass;

  n=fEcalPoints->GetEntriesFast();
  if (fVerbose>1)
    cout << "Found " << n << " incoming particles in event." << endl;

  for(Int_t i=0;i<n;i++)
  {
    p=(CbmEcalPoint*)fEcalPoints->At(i);
    track=(CbmMCTrack*)fMCTracks->At(p->GetTrackID());
//    if (track->GetPdgCode()!=fPDGCode)
 //     continue;
    p->Momentum(mom);
    if (mom.Mag()<fLowEnergy)
      continue;
    count++;
    TParticlePDG* fParticlePDG=TDatabasePDG::Instance()->GetParticle(fPDGCode);
    if (fParticlePDG)
      mass=fParticlePDG->Mass();
    if (mass<0)
    {
      cerr << "Can't find PDG particle " << fPDGCode << "!!!";
      cerr << endl;
      Fatal("","");
    }
    fOutMCE=TMath::Sqrt(mass*mass+mom.Mag2());
    fOutMCPx=mom.Px();
    fOutMCPy=mom.Py();
    fOutMCPz=mom.Pz();
    p->Position(mom);
    GetMCXY(mom);
  }
  fCount=count;
}

/** Get MC coordinates **/
void CbmEcalOShowerCalibrator::GetMCXY(TVector3& mom)
{
  static Float_t PSLead=fPar->GetVariableStrict("pslead");
  static Float_t PSScin=fPar->GetVariableStrict("psscin");
  static Float_t PSGap=fPar->GetVariableStrict("psgap");
  static Float_t PSThickness=PSLead+PSScin+PSGap;
  static Float_t ZPos=fPar->GetVariableStrict("zpos");

  Float_t delta;

  if (fZPosition==-2) delta=ZPos+PSThickness;
  else
    if (fZPosition==-1) delta=ZPos;
    else
      delta=fZPosition;
  delta-=mom.Z();

  fMCX=mom.X()+fOutMCPx/fOutMCPz*delta;
  fMCY=mom.Y()+fOutMCPy/fOutMCPz*delta;

  Int_t ix;
  Int_t iy;

  if (fMCX>=0)
    ix=(Int_t)(fMCX/fCellSize);
  else
    ix=(Int_t)(fMCX/fCellSize)-1;
  if (fMCY>=0)
    iy=(Int_t)(fMCY/fCellSize);
  else
    iy=(Int_t)(fMCY/fCellSize)-1;


  fLocalMCX=fMCX-(0.5+ix)*fCellSize;
  fLocalMCY=fMCY-(0.5+iy)*fCellSize;
}

/** Parameter container init **/
void CbmEcalOShowerCalibrator::SetParContainers()
{
  FairRunAna* ana=FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPar=(CbmGeoEcalPar*)(rtdb->getContainer("CbmGeoEcalPar"));
}

void CbmEcalOShowerCalibrator::InitTree()
{
  TString name;
  fTree=new TTree("ECALshower","Tree of ECAL showers");
  fTree->Branch("mcx",&fOutMCX,"mcx/F");
  fTree->Branch("mcy",&fOutMCY,"mcy/F");
  fTree->Branch("dx",&fOutDX,"dx/I");
  fTree->Branch("dy",&fOutDY,"dy/I");
  fTree->Branch("simcellsize",&fCellSize,"simcellsize/F");
  fTree->Branch("e",&fOutMCE,"e/F");
  fTree->Branch("px",&fOutMCPx,"px/F");
  fTree->Branch("py",&fOutMCPy,"py/F");
  fTree->Branch("pz",&fOutMCPz,"pz/F");
  fTree->Branch("ev",&fEvent,"ev/I");

  name="ecale["; name+=fLocalSize; name+="]/F";
  fTree->Branch("ecale", fLocalEnergy, name);
  name="pse["; name+=fLocalSize; name+="]/F";
  fTree->Branch("pse", fLocalPSEnergy, name);
};

/** Finishing routine **/
void CbmEcalOShowerCalibrator::Finish()
{
  fTree->Write();
}

ClassImp(CbmEcalOShowerCalibrator)
