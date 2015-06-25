#include "CbmEcalShowerCalibrator.h"

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
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;


CbmEcalShowerCalibrator::CbmEcalShowerCalibrator()
  : FairTask(),
    fZPosition(-2),
    fPDGCode(22), 
    fLowEnergy(0.1), 
    fOutCellSize(3.0), 
    fSCurveRun(kFALSE),
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
    fOutEnergy(),
    fOutPSEnergy(),
    fEvent(0),
    fLocalEnergy(NULL),
    fLocalPSEnergy(NULL),
    fLocalXArr(NULL),
    fLocalYArr(NULL),
    fLocalSize(0),
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

CbmEcalShowerCalibrator::CbmEcalShowerCalibrator(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose), 
    fZPosition(-2),
    fPDGCode(22), 
    fLowEnergy(0.1), 
    fOutCellSize(3.0), 
    fSCurveRun(kFALSE),
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
    fOutEnergy(),
    fOutPSEnergy(),
    fEvent(0),
    fLocalEnergy(NULL),
    fLocalPSEnergy(NULL),
    fLocalXArr(NULL),
    fLocalYArr(NULL),
    fLocalSize(0),
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
InitStatus CbmEcalShowerCalibrator::Init()
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
 
  fLocalXSize=(Int_t)((fModuleSize+0.0001)/fCellSize);
  fLocalYSize=(Int_t)((fModuleSize+0.0001)/fCellSize);

  cout << "Inited for transport cell size " << fCellSize;
  cout << ", module size " << fModuleSize << endl;
  cout << "Size of local array: x " << fLocalXSize;
  cout << ", y " << fLocalYSize << endl;

  fLocalSize=(6*fLocalXSize+1)*(6*fLocalYSize+1);
  fLocalEnergy=new Float_t[(6*fLocalXSize+1)*(6*fLocalYSize+1)];
  fLocalPSEnergy=new Float_t[(6*fLocalXSize+1)*(6*fLocalYSize+1)];
  fLocalXArr=new Float_t[(6*fLocalXSize+1)*(6*fLocalYSize+1)];
  fLocalYArr=new Float_t[(6*fLocalXSize+1)*(6*fLocalYSize+1)];
  
  for(Int_t i=0;i<fLocalSize;i++)
    GetLocalXY(i, fLocalXArr[i], fLocalYArr[i]);    

  cout << "Local array size " << fLocalSize << endl;
  fEvent=0;
  return kSUCCESS;
}

/** Clear fLocalEnergy and fLocalPSEnergy **/
void CbmEcalShowerCalibrator::ClearLocal()
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
void CbmEcalShowerCalibrator::Exec(Option_t* option)
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

  static Int_t size=(Int_t)((fOutCellSize+0.001)/fCellSize);
  static Int_t halfsize=size/2;
  Int_t num;
  Int_t ix;
  Int_t iy;
  Int_t nb;
//  Int_t ne;
  Int_t j;
  Int_t jj;
  static Int_t xsize=6*fLocalXSize+1;
  static Int_t begin=3*fLocalXSize+3*fLocalYSize*(6*fLocalXSize+1);
  static Int_t iidx=2*size;
  static Int_t iidy=2*size;
  static Int_t idx=3*size-1;
  static Int_t idy=3*size-1;
  
  if (fVerbose>10)
    cout << "Constructing output matrices... " << flush;
  for(ix=0;ix<size;ix++)
  {
    for(iy=0;iy<size;iy++)
    {
      nb=begin+(-ix-iidx)+(-iy-iidy)*xsize;
//      ne=begin+(-ix+idx)+(-iy+idy)*xsize;
      ClearOut();
      fOutDX=ix-halfsize;
      fOutDY=iy-halfsize;
      if (fSCurveRun&&fOutDY!=0) continue;
      if (size%2==0)
      {
        fOutMCX=fLocalMCX+(fOutDX+0.5)*fCellSize;
        fOutMCY=fLocalMCY+(fOutDY+0.5)*fCellSize;
      }
      else
      {
        fOutMCX=fLocalMCX+fCellSize*fOutDX;
        fOutMCY=fLocalMCY+fCellSize*fOutDY;
      }
      for(Int_t nx=0;nx<5;nx++)
      for(Int_t ny=0;ny<5;ny++)
      {
	num=nx+ny*5;
	jj=nb+(ny*xsize+nx)*size;
        for(Int_t dx=0;dx<size;dx++)
	{
	  j=jj+dx;
          for(Int_t dy=0;dy<size;dy++)
	  {
            fOutEnergy[num]+=fLocalEnergy[j];
            fOutPSEnergy[num]+=fLocalPSEnergy[j];
	    j+=xsize;
  	  }
	}
      }
      fTree->Fill();
    }
  }
  if (fVerbose>10)
    cout << "Done." << flush;
}

/** Fill fLocalEnergy and fLocalPSEnergy **/
void CbmEcalShowerCalibrator::FillLocal()
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

/** local number -> (x,y) **/
void CbmEcalShowerCalibrator::GetLocalXY(Int_t num, Float_t& x, Float_t& y) const
{
  static Float_t dx=fCellSize;
  static Float_t dy=fCellSize;
  static Int_t idx=3*fLocalXSize;
  static Int_t idy=3*fLocalYSize;
  static Int_t xsize=6*fLocalXSize+1;
//  static Int_t ysize=6*fLocalYSize+1;

  if (num<0||num>=fLocalSize)
  {
    x=-1111; y=-1111;
    return;
  }
  Int_t ix=num%xsize-idx;
  Int_t iy=num/xsize-idy;

  x=(0.01+ix)*dx;
  y=(0.01+iy)*dy;
}

Int_t CbmEcalShowerCalibrator::GetLocalNumber(Float_t x, Float_t y) const 
{
  static Float_t dx=fCellSize;
  static Float_t dy=fCellSize;
  static Int_t idx=3*fLocalXSize;
  static Int_t idy=3*fLocalYSize;
  static Int_t xsize=6*fLocalXSize+1;
  static Int_t ysize=6*fLocalYSize+1;

  Int_t ix=(Int_t)(x/dx+0.5)+idx;
  if (x+dx/2<0) ix--;
  Int_t iy=(Int_t)(y/dy+0.5)+idy;
  if (y+dy/2<0) iy--;

  if (ix<0||iy<0) return -1;
  if (ix>=xsize) return -1;
  if (iy>=ysize) return -1;

  return ix+iy*xsize;
}

/** (x,y) -> out number. -1 is fails **/
Int_t CbmEcalShowerCalibrator::GetOutNumber(Float_t x, Float_t y) const
{
  static Float_t dx=fOutCellSize;
  static Float_t dy=fOutCellSize;
  static Int_t idx=2;
  static Int_t idy=2;
  static Int_t xsize=5;
  static Int_t ysize=5;

  Int_t ix=(Int_t)(x/dx+0.5)+idx;
  if (x+dx/2<0) ix--;
  Int_t iy=(Int_t)(y/dy+0.5)+idy;
  if (y+dy/2<0) iy--;

  if (ix<0||iy<0) return -1;
  if (ix>=xsize) return -1;
  if (iy>=ysize) return -1;

  return ix+iy*xsize;
}

/** Get MC parameters of incoming particle **/
void CbmEcalShowerCalibrator::GetMCParameters()
{
  CbmEcalPoint* p;
  CbmMCTrack* track;
  TVector3 mom;
  Int_t n;
  Int_t count=0;
  Float_t mass=-1111;

  n=fEcalPoints->GetEntriesFast();
  if (fVerbose>1)
    cout << "Found " << n << " incoming particles in event." << endl;

  for(Int_t i=0;i<n;i++)
  {
    p=(CbmEcalPoint*)fEcalPoints->At(i);
    track=(CbmMCTrack*)fMCTracks->At(p->GetTrackID());
    if (track->GetPdgCode()!=fPDGCode)
      continue;
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
      Fatal("GetMCParameters","Can't find PDG particle %d.", fPDGCode);
      return;
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
void CbmEcalShowerCalibrator::GetMCXY(TVector3& mom)
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
void CbmEcalShowerCalibrator::SetParContainers()
{
  FairRunAna* ana=FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPar=(CbmGeoEcalPar*)(rtdb->getContainer("CbmGeoEcalPar"));
}

void CbmEcalShowerCalibrator::InitTree()
{
  fTree=new TTree("ECALshower","Tree of ECAL showers");
  fTree->Branch("mcx",&fOutMCX,"mcx/F");
  fTree->Branch("mcy",&fOutMCY,"mcy/F");
  fTree->Branch("dx",&fOutDX,"dx/I");
  fTree->Branch("dy",&fOutDY,"dy/I");
  fTree->Branch("cellsize",&fOutCellSize,"cellsize/F");
  fTree->Branch("simcellsize",&fCellSize,"simcellsize/F");
  fTree->Branch("e",&fOutMCE,"e/F");
  fTree->Branch("px",&fOutMCPx,"px/F");
  fTree->Branch("py",&fOutMCPy,"py/F");
  fTree->Branch("pz",&fOutMCPz,"pz/F");
  fTree->Branch("ev",&fEvent,"ev/I");
  fTree->Branch("ecale", fOutEnergy, "ecale[25]/F");
  fTree->Branch("pse", fOutPSEnergy, "pse[25]/F");
};

/** Finishing routine **/
void CbmEcalShowerCalibrator::Finish()
{
  fTree->Write();
}

ClassImp(CbmEcalShowerCalibrator)
