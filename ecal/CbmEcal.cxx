// -------------------------------------------------------------------------
// -----                       CbmEcal source file                   -----
// -----                  Created 04/02/05  by Yuri.Kharlov              -----
// -------------------------------------------------------------------------

/* $Id: CbmEcal.cxx,v 1.26 2006/08/30 13:16:45 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcal.cxx,v $
 * Revision 1.26  2006/08/30 13:16:45  prokudin
 * Bug huntong
 *
 * Revision 1.25  2006/08/15 19:04:39  prokudin
 * Bug hunting
 *
 * Revision 1.24  2006/08/15 16:18:01  prokudin
 * Bug hunting
 *
 * Revision 1.23  2006/07/29 00:33:53  prokudin
 * Commenting. Bringing code to coding conventions. New way of parameter file processing
 *
 * Revision 1.22  2006/07/26 22:49:23  prokudin
 * Minor bug fixed
 *
 * Revision 1.21  2006/07/24 21:45:02  prokudin
 * Now we write information to parameter file
 *
 * Revision 1.20  2006/07/20 12:44:42  kharlov
 * Switching off TObject streamer for ECAL points and MC tracks
 *
 * Revision 1.19  2006/07/18 17:10:14  kharlov
 * Track position and momentum are put into correct place
 *
 * Revision 1.18  2006/07/18 11:41:21  prokudin
 * Corrected for MCpoints production
 *
 * Revision 1.17  2006/07/17 13:59:06  prokudin
 * fMat1 removed. At last.
 *
 * Revision 1.16  2006/07/15 18:30:09  kharlov
 * Move gMC->TrackPosition and gMC->TrackMomentum to FillWallPoint
 *
 * Revision 1.15  2006/07/14 15:09:18  prokudin
 * New functionality added
 *
 * Revision 1.14  2006/07/13 14:42:16  prokudin
 * GetCellCoord now static
 *
 * Revision 1.13  2006/07/13 10:19:25  prokudin
 * Coordinate calcalation fixed
 *
 * Revision 1.12  2006/07/12 12:04:59  prokudin
 * CbmEcalInf now singleton.
 *
 * Revision 1.11  2006/07/12 08:41:38  prokudin
 * String compare removed. Using FairVolume instead.
 *
 * Revision 1.10  2006/07/04 08:29:24  kharlov
 * Remove #include TGeant3.h
 *
 * Revision 1.9  2006/06/30 13:04:22  kharlov
 * Added a function SetSpecialPhysicsCuts()
 *
 * Revision 1.8  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.7  2006/06/28 14:16:00  kharlov
 * Minor polishing
 *
 * Revision 1.6  2006/06/26 14:27:15  kharlov
 * Change storing MCpoints at ECAL face, change the order of medium definition
 *
 * Revision 1.5  2006/06/12 11:59:47  kharlov
 * Passing geometry file name via the constructor argument
 *
 * Revision 1.4  2006/04/25 06:52:17  kharlov
 * Remove CbmEcalv[1,2] and leave the only class CbmEcal
 *
 * Revision 1.6  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.4  2006/03/03 12:01:16  turany
 * Remove FairVolume, framework is responsible for sensitivity
 *
 * Revision 1.3  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcal
 *
 * Revision 1.2  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

/**  CbmEcal.cxx
 *@author Yuri Kharlov
 **
 ** Defines the active detector ECAL with geometry coded here.
 **/

#include "CbmEcal.h"

#include "CbmEcalPoint.h"
#include "CbmGeoEcalPar.h"

#include "CbmDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "CbmDetectorList.h"
#include "FairVolume.h"
#include "FairGeoMedium.h"
#include "FairGeoMedia.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TList.h"
#include "TTree.h"

#include <iostream>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::cerr;

//Defines for debuging
//#define _DECAL
//#define _DECALCELL
#define kN kNumberOfECALSensitiveVolumes

// -----   Default constructor   -------------------------------------------
CbmEcal::CbmEcal() 
  : FairDetector("ECAL", kTRUE, kECAL), 
    fInf(NULL),
    fDebug(NULL),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1.),
    fPosIndex(0),
    fEcalCollection(new TClonesArray("CbmEcalPoint")),
    fLiteCollection(new TClonesArray("CbmEcalPointLite")),
    fEcalSize(),
    fEcalHole(),
    fCellSize(0.),
    fZEcal(0.),
    fThicknessLead(0.),
    fThicknessScin(0.),
    fThicknessTyvk(0.),
    fThicknessLayer(0.),
    fNLayers(0),
    fGeoScale(0.),
    fNColumns1(0),
    fNRows1(0),
    fNColumns2(0),
    fNRows2(0),
    fThicknessPSlead(0.),
    fThicknessPSscin(0.),
    fEcalPSgap(0.),
    fNColumns(0),
    fNRows(0),
    fVolIdMax(0),
    fFirstNumber(0),
    fAbsorber(""),
    fVolArr()
{
  fVerboseLevel = 1;

  Int_t i;

  for(i=kN-1;i>-1;i--)
    fVolArr[i]=-1111;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmEcal::CbmEcal(const char* name, Bool_t active, const char* fileGeo)
  : FairDetector(name, active, kECAL),
    fInf(NULL),
    fDebug(NULL),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1.),
    fPosIndex(0),
    fEcalCollection(NULL),
    fLiteCollection(NULL),
    fEcalSize(),
    fEcalHole(),
    fCellSize(0.),
    fZEcal(0.),
    fThicknessLead(0.),
    fThicknessScin(0.),
    fThicknessTyvk(0.),
    fThicknessLayer(0.),
    fNLayers(0),
    fGeoScale(0.),
    fNColumns1(0),
    fNRows1(0),
    fNColumns2(0),
    fNRows2(0),
    fThicknessPSlead(0.),
    fThicknessPSscin(0.),
    fEcalPSgap(0.),
    fNColumns(0),
    fNRows(0),
    fVolIdMax(0),
    fFirstNumber(0),
    fAbsorber(""),
    fVolArr()
{
  /** CbmEcal constructor:
   ** reads geometry parameters from the ascii file <fileGeo>,
   ** creates the ECAL geometry container CbmEcalInf
   ** and initializes basic geometry parameters needed to construct
   ** TGeo geometry
   **/

  Int_t i;
  printf("ECAL geometry is read from file %s\n",fileGeo);
  fInf=CbmEcalInf::GetInstance(fileGeo);
  fInf->DumpContainer();
  if (fInf==NULL)
  {
    cerr << "Error. CbmEcal: Can't read geometry from " << fileGeo;
    cerr << "!!!" << endl;
  }
  fGeoScale=1.;
  fEcalSize[0]=fInf->GetEcalSize(0);
  fEcalSize[1]=fInf->GetEcalSize(1);
  fEcalSize[2]=fInf->GetEcalSize(2);

  fEcalHole[0]=fInf->GetEcalHole(0);
  fEcalHole[1]=fInf->GetEcalHole(1);
  fEcalHole[2]=fInf->GetEcalHole(2);
  
  fCellSize=fInf->GetCellSize();
  fZEcal=fInf->GetZPos();

  fThicknessLead=fInf->GetLead();
  fThicknessScin=fInf->GetScin();
  fThicknessTyvk=fInf->GetTyveec();
  fNLayers=fInf->GetNLayers();

  fThicknessPSlead=fInf->GetPSLead();
  fThicknessPSscin=fInf->GetPSScin();
  fEcalPSgap=fInf->GetPSGap();
  
  fNColumns1 = TMath::Nint((fEcalSize[0]/2/fCellSize)*2);
  fNRows1    = TMath::Nint(((fEcalSize[1]-fEcalHole[1])/4/fCellSize)*2);

  fNColumns2 = TMath::Nint(((fEcalSize[0]-fEcalHole[0])/4/fCellSize)*2);
  fNRows2    = TMath::Nint((fEcalHole[1]/2/fCellSize)*2);

  fNColumns = TMath::Max(fNColumns1,fNColumns2);
  fNRows    = TMath::Max(fNRows1,fNRows2);
  fVolIdMax=fNColumns*fNRows+fNRows+(2*(4-1)+2-1)*fNColumns*fNRows;
  fPosIndex=0;
  fDebug="";
  fEcalCollection=new TClonesArray("CbmEcalPoint");
  fLiteCollection=NULL;
  fVerboseLevel = 1;
  fAbsorber=fInf->GetStringVariable("absorber");

  fInf->AddVariable("ecalversion", "0");
  for(i=kN-1;i>-1;i--)
    fVolArr[i]=-1111;

  if (fInf->GetFastMC()==1) return;
  fLiteCollection=new TClonesArray("CbmEcalPointLite");
}

/** size in Mb (2^20) **/
void CbmEcal::SetMaximumFileSize(Long64_t size)
{
  TTree::SetMaxTreeSize(size*1024*1024);
}

// -------------------------------------------------------------------------

void CbmEcal::Initialize()
{
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  CbmGeoEcalPar *par=new CbmGeoEcalPar();
  fInf->FillGeoPar(par,0);
  rtdb->addContainer(par);
}

// -----   Destructor   ----------------------------------------------------
CbmEcal::~CbmEcal()
{
  if (fEcalCollection) {
    fEcalCollection->Delete(); 
    delete fEcalCollection;
    fEcalCollection=NULL;
  }
  if (fLiteCollection) {
    fLiteCollection->Delete();
    delete fLiteCollection;
    fLiteCollection=NULL;
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetEcalCuts   ------------------------------------
void CbmEcal::SetEcalCuts(Int_t medium)
{
  /** Set GEANT3 tracking energy cuts for selected medium **/
  if (fInf->GetElectronCut() > 0) {
    gMC->Gstpar(medium,"CUTGAM",fInf->GetElectronCut());
    gMC->Gstpar(medium,"CUTELE",fInf->GetElectronCut());
    gMC->Gstpar(medium,"BCUTE" ,fInf->GetElectronCut());
    gMC->Gstpar(medium,"BCUTM" ,fInf->GetElectronCut());
  }

  if (fInf->GetHadronCut() > 0) {
    gMC->Gstpar(medium,"CUTNEU",fInf->GetHadronCut());
    gMC->Gstpar(medium,"CUTHAD",fInf->GetHadronCut());
    gMC->Gstpar(medium,"CUTMUO",fInf->GetHadronCut());
    gMC->Gstpar(medium,"PPCUTM",fInf->GetHadronCut());
  }
}
// -------------------------------------------------------------------------

void CbmEcal::FinishPrimary()
{
  if (fInf->GetFastMC()==1) return;
  fFirstNumber=fLiteCollection->GetEntriesFast();
}

//_____________________________________________________________________________
void CbmEcal::ChangeHit(CbmEcalPointLite* oldHit)
{
  Double_t edep = gMC->Edep();
  Double_t el=oldHit->GetEnergyLoss();
  Double_t ttime=gMC->TrackTime()*1.0e9;
  oldHit->SetEnergyLoss(el+edep);
  if(ttime<oldHit->GetTime())
    oldHit->SetTime(ttime);
}

//_____________________________________________________________________________
void CbmEcal::SetSpecialPhysicsCuts()
{
  /** Change the special tracking cuts for
   ** two ECAL media, Scintillator and Lead
   **/

  if (fInf->GetFastMC()==1) return;

  FairRun* fRun = FairRun::Instance();
  if (strcmp(fRun->GetName(),"TGeant3") == 0) {
    Int_t mediumID;
    mediumID = gGeoManager->GetMedium("Scintillator")->GetId();
    SetEcalCuts(mediumID);
  if (fInf->GetFastMC()==1) return;
  if (fInf->GetFastMC()==1) return;
    mediumID = gGeoManager->GetMedium(fAbsorber)->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("Tyvek")->GetId();
    SetEcalCuts(mediumID);
    mediumID = gGeoManager->GetMedium("SensVacuum")->GetId();
    SetEcalCuts(mediumID);
  }
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmEcal::ProcessHits(FairVolume* vol)
{
  /** Fill MC point for sensitive ECAL volumes **/

  fELoss   = gMC->Edep();
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  fTime    = gMC->TrackTime()*1.0e09;
  fLength  = gMC->TrackLength();

  Int_t num=GetVolType(vol->getVolumeId());
  if (num<0)
  {
     cout << "-I- CbmEcal: Unknown volume number: " << vol->getVolumeId() << endl;
     return kFALSE;
  }
  if (num!=0) { //not a WALL
    if (fELoss==0) return kFALSE;
    if (!FindVolumeID(num) )
      return kFALSE;
    if (!FillLitePoint(num) ) return kFALSE;
  }
  else
    if ( gMC->IsTrackEntering() )
    { // WALL hit
      FillWallPoint();
    }
    else
      return kFALSE;

  // Increment number of ecal points for TParticle

  ((CbmStack*)gMC->GetStack())->AddPoint(kECAL, fTrackID);
  
  ResetParameters();
  
  return kTRUE;
}

/** returns type of volume **/
Int_t CbmEcal::GetVolType(Int_t volnum)
{
	Int_t i;
	for(i=kN-1;i>-1;i--) {
	  if (fVolArr[i]==volnum) break;
	}
        
	return i;
}

//-----------------------------------------------------------------------------
Bool_t CbmEcal::FindVolumeID(Int_t volnum)
{
  /** Find the cell ID for the current step **/

  if (volnum<1||volnum>5) return kFALSE;
#ifdef _DECALSTEP 
  Int_t copyNo, copyNo1;
#endif
  Int_t copyNo2, copyNo3, copyNo4, copyNo5, copyNo6, copyNo7;
  
  Int_t iColumn=0, iRow=0, iEcalBlockCopy=0, iEcalBlock=0;
  char sEcalBlock[5];
  
  static Int_t nColumns = TMath::Max(fNColumns1,fNColumns2);
  static Int_t nRows    = TMath::Max(fNRows1,fNRows2);
  
/*  Bool_t scinCell = strstr(gMC->CurrentVolName(),"ECALscin");
  Bool_t leadCell = strstr(gMC->CurrentVolName(),"ECALlead");
  Bool_t tyvkCell = strstr(gMC->CurrentVolName(),"ECALtyvk");
  Bool_t prshCell = strstr(gMC->CurrentVolName(),"ECALPScell");
  Bool_t PSScCell = strstr(gMC->CurrentVolName(),"ECALPSscin");*/
/*  cout << vol->getVolumeId();
  if (PSScCell) cout << "	PSCell";
  if (prshCell) cout << "	PSscin";
  if (scinCell) cout << "	scin";
  if (tyvkCell) cout << "	tyvk";
  if (leadCell) cout << "	lead";

  if (num==2) cout << "	PSCell";
  if (num==1) cout << "	PSscin";
  if (num==16) cout << "	scin";
  if (num==8) cout << "	tyvk";
  if (num==4) cout << "	lead";
  cout <<  endl;
  if (!leadCell&&!scinCell&&!tyvkCell&&!prshCell&&!PSScCell) return kFALSE;*/
 
#ifdef _DECALSTEP
  gMC->CurrentVolID(copyNo);
  gMC->CurrentVolOffID(1, copyNo1); // ECALPScell || ECALtyvk
  gMC->CurrentVolOffID(2, copyNo2); // ECAL?row || ECALlayer
  gMC->CurrentVolOffID(3, copyNo3); // ECAL?column || ECALcell
  gMC->CurrentVolOffID(4, copyNo4);
#endif
  gMC->CurrentVolOffID(4, copyNo4);
  
  if (volnum==2) {
    
    gMC->CurrentVolOffID(2, copyNo2); // ECAL?row || ECALlayer
    gMC->CurrentVolOffID(3, copyNo3); // ECAL?column || ECALcell
    iRow = copyNo2;
    iColumn = copyNo3;
    iEcalBlockCopy = copyNo4;
    sscanf(gMC->CurrentVolOffName(4),"%4s%1d",sEcalBlock,&iEcalBlock);
    
  }
  else
    if (volnum==1) {
      gMC->CurrentVolOffID(1, copyNo2); // ECAL?row || ECALlayer
      gMC->CurrentVolOffID(2, copyNo3); // ECAL?column || ECALcell
      iRow = copyNo2;
      iColumn = copyNo3;
      gMC->CurrentVolOffID(3, copyNo4);
      iEcalBlockCopy = copyNo4;
      sscanf(gMC->CurrentVolOffName(3),"%4s%1d",sEcalBlock,&iEcalBlock);
	
    }
    else {
      if (volnum==5) {
	if (fThicknessTyvk!=0.0) {
	  gMC->CurrentVolOffID(5, copyNo5); // ECAL? || ECAL?row
	  gMC->CurrentVolOffID(6, copyNo6);
	  gMC->CurrentVolOffID(7, copyNo7);
	}
	else {
	  gMC->CurrentVolOffID(4, copyNo5); // ECAL? || ECAL?row
	  gMC->CurrentVolOffID(5, copyNo6);
	  gMC->CurrentVolOffID(6, copyNo7);
	};
	iRow = copyNo5;
	iColumn = copyNo6;
	iEcalBlockCopy = copyNo7;
	if (fThicknessTyvk!=0.0)
	  sscanf(gMC->CurrentVolOffName(6),"%4s%1d",sEcalBlock,&iEcalBlock);
	else
	  sscanf(gMC->CurrentVolOffName(5),"%4s%1d",sEcalBlock,&iEcalBlock);
	iEcalBlock+=2; // =3 or 4 for ecalCell //BVZ
      }
      else
	if (volnum==3) {
	  gMC->CurrentVolOffID(4, copyNo4);
	  gMC->CurrentVolOffID(5, copyNo5); // ECAL? || ECAL?row
	  gMC->CurrentVolOffID(6, copyNo6);
	  iRow = copyNo4;
	  iColumn = copyNo5;
	  iEcalBlockCopy = copyNo6;
	  sscanf(gMC->CurrentVolOffName(5),"%4s%1d",sEcalBlock,&iEcalBlock);
	  iEcalBlock+=2; // =3 or 4 for ecalCell //BVZ
	}
	else
	  if (volnum==4) {
	    gMC->CurrentVolOffID(3, copyNo3); // ECAL?column || ECALcell
	    gMC->CurrentVolOffID(4, copyNo4);
	    gMC->CurrentVolOffID(5, copyNo5); // ECAL? || ECAL?row
	    iRow = copyNo3;
	    iColumn = copyNo4;
	    iEcalBlockCopy = copyNo5;
	    sscanf(gMC->CurrentVolOffName(4),"%4s%1d",sEcalBlock,&iEcalBlock);
	    iEcalBlock+=2; // =3 or 4 for ecalCell //BVZ
	  }
    }
#ifdef _DECAL
  if(iEcalBlock<1||iEcalBlock>4||iEcalBlockCopy<1||iEcalBlockCopy>2) //BVZ
    cout<<"!!!!!!!!! iEcalBlock,iEcalBlockCopy = "<<iEcalBlock<<","<<iEcalBlockCopy<<endl;
#endif

  // Unique cell id
  fVolumeID = (2*(iEcalBlock-1)+iEcalBlockCopy-1)*nColumns*nRows +
    (iColumn-1)*nRows + iRow-1;

#ifdef _DECALSTEP
  if (volnum>2)
    printf("Track %d, Volume %d %s in %d %s in %d %s in %d %s in %d %s in %d %s in %d %s in %d %s in %d %s\n",
	   fTrackID,
	   copyNo ,gMC->CurrentVolName(),
	   copyNo1,gMC->CurrentVolOffName(1),
	   copyNo2,gMC->CurrentVolOffName(2),
	   copyNo3,gMC->CurrentVolOffName(3),
	   copyNo4,gMC->CurrentVolOffName(4),
	   copyNo5,gMC->CurrentVolOffName(5),
	   copyNo6,gMC->CurrentVolOffName(6),
	   copyNo7,gMC->CurrentVolOffName(7),
	   iEcalBlock,sEcalBlock);
  else
    printf("Track %d, Volume %d %s in %d %s in %d %s in %d %s in %d %s in %d %s\n",
	   fTrackID,
	   copyNo ,gMC->CurrentVolName(),
	   copyNo1,gMC->CurrentVolOffName(1),
	   copyNo2,gMC->CurrentVolOffName(2),
	   copyNo3,gMC->CurrentVolOffName(3),
	   copyNo4,gMC->CurrentVolOffName(4),
	   iEcalBlock,sEcalBlock);
#endif
#ifdef _DECALCELL
  printf("%s in ECAL block %d, block copy %d, column %d, row %d\n",
	 gMC->CurrentVolName(),iEcalBlock,iEcalBlockCopy,iColumn,iRow);
#endif   
  return kTRUE;
}

//-----------------------------------------------------------------------------
void CbmEcal::FillWallPoint()
{
  /** Fill MC points on the ECAL front wall **/

  gMC->TrackPosition(fPos);
  gMC->TrackMomentum(fMom);
  fVolumeID = -1;
  Double_t mass = gMC->TrackMass();
  // Calculate kinetic energy
  Double_t ekin = TMath::Sqrt( fMom.Px()*fMom.Px() +
			       fMom.Py()*fMom.Py() +
			       fMom.Pz()*fMom.Pz() +
			       mass * mass ) - mass;
  fELoss = ekin;
  // Create CbmEcalPoint at the entrance of calorimeter
  // for particles with pz>0 coming through the front wall
  if (fMom.Pz() > 0 && fPos.Z() < fZEcal+0.01)
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, 
	   fELoss);
  fTrackID=gMC->GetStack()->GetCurrentTrackNumber();
}

CbmEcalPointLite* CbmEcal::FindHit(Int_t VolId, Int_t TrackId)
{
  for(Int_t i=fFirstNumber;i<fLiteCollection->GetEntriesFast();i++)
  {
    CbmEcalPointLite* point=(CbmEcalPointLite*)fLiteCollection->At(i);
    if (point->GetTrackID()==TrackId&&point->GetDetectorID()==VolId)
      return point;
  }
  return NULL;
}
//-----------------------------------------------------------------------------
Bool_t CbmEcal::FillLitePoint(Int_t volnum)
{
  /** Fill MC points inside the ECAL for non-zero deposited energy **/

  //Search for input track
  
  static Float_t zmin=fZEcal-0.0001;
  static Float_t zmax=fZEcal+fEcalSize[2];
  static Float_t xecal=fEcalSize[0]/2;
  static Float_t yecal=fEcalSize[1]/2;
  static Float_t xhole=fEcalHole[0]/2-0.01;
  static Float_t yhole=fEcalHole[1]/2-0.01;
  TParticle* part=gMC->GetStack()->GetCurrentTrack();
  fTrackID=gMC->GetStack()->GetCurrentTrackNumber();

  while (part->GetFirstMother()>=0&&\
      part->Vz()>=zmin&&part->Vz()<=zmax&& \
      TMath::Abs(part->Vx())<=xecal&&\
      TMath::Abs(part->Vy())<=yecal&&\
      (TMath::Abs(part->Vx())>=xhole||TMath::Abs(part->Vy())>=yhole))
    {
      fTrackID=part->GetFirstMother();
      part =((CbmStack*)gMC->GetStack())->GetParticle(fTrackID);
    }
#ifdef _DECAL
  if (fTrackID<0) cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!fTrackID="<<fTrackID<<endl;
#endif
  CbmEcalPointLite* oldHit;
  CbmEcalPointLite* newHit;
  
  if (volnum==2||volnum==5) {
    if ((oldHit=FindHit(fVolumeID,fTrackID))!=NULL)
      ChangeHit(oldHit);
    else
    {	    
      // Create CbmEcalPoint for scintillator volumes
      newHit = AddLiteHit(fTrackID, fVolumeID, fTime, fELoss);
    }
  }
      
  fVolumeID+=fVolIdMax;
  if ((oldHit=FindHit(fVolumeID,fTrackID))!=NULL)
  {
    ChangeHit(oldHit);
    return kTRUE;
  }

  // Create CbmEcalPoint for any ECAL volumes
  newHit = AddLiteHit(fTrackID, fVolumeID, fTime, fELoss);

  if (newHit==NULL) return kFALSE;
  return kTRUE;
}

// -----   Public method EndOfEvent   --------------------------------------
void CbmEcal::EndOfEvent() {
  if (fVerboseLevel) Print();
  fEcalCollection->Delete();

  if (fInf->GetFastMC()==0)
    fLiteCollection->Delete();
  fPosIndex = 0;
  fFirstNumber=0;
}
// -------------------------------------------------------------------------

// -----   Public method GetCollection   -----------------------------------
TClonesArray* CbmEcal::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fEcalCollection;
  if (fInf->GetFastMC()==1) return NULL;
  if (iColl == 1) return fLiteCollection;
  else return NULL;
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void CbmEcal::Reset()
{
  fEcalCollection->Delete();
  if (fInf->GetFastMC()==0)
    fLiteCollection->Delete();
  ResetParameters();
  fFirstNumber=0;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmEcal::Print() const 
{
  Int_t nHits = fEcalCollection->GetEntriesFast();
  Int_t nLiteHits=0;
  Int_t i;

  cout << "-I- CbmEcal: " << nHits << " points registered in this event.";
  cout << endl;

  if (fInf->GetFastMC()==0) {
    nLiteHits = fLiteCollection->GetEntriesFast();
    cout << "-I- CbmEcal: " << nLiteHits << " lite points registered in this event.";
    cout << endl;
  }

  if (fVerboseLevel>1) {
    for (i=0; i<nHits; i++) (*fEcalCollection)[i]->Print();
    if (fInf->GetFastMC()==0)
      for (i=0; i<nLiteHits; i++) (*fLiteCollection)[i]->Print();
  }
}
// -------------------------------------------------------------------------

// -----   Public method CopyClones   --------------------------------------
void CbmEcal::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{   
  Int_t nEntries = cl1->GetEntriesFast();
  Int_t i;
  Int_t index;
  cout << "-I- CbmEcal: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  if (cl1->GetClass()==CbmEcalPoint::Class()) {
    CbmEcalPoint* oldpoint = NULL;
    for (i=0; i<nEntries; i++) {
      oldpoint = (CbmEcalPoint*) cl1->At(i);
      index = oldpoint->GetTrackID()+offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmEcalPoint(*oldpoint);
      fPosIndex++;
    }
    cout << "-I- CbmEcal: " << cl2->GetEntriesFast() << " merged entries."
         << endl;
  }
  else if (cl1->GetClass()==CbmEcalPointLite::Class()) {
    CbmEcalPointLite* oldpoint = NULL;
    for (i=0; i<nEntries; i++) {
      oldpoint = (CbmEcalPointLite*) cl1->At(i);
      index = oldpoint->GetTrackID()+offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmEcalPointLite(*oldpoint);
      fPosIndex++;
    }
    cout << "-I- CbmEcal: " << cl2->GetEntriesFast() << " merged entries."
         << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Register   ----------------------------------------
void CbmEcal::Register()
{
  FairRootManager::Instance()->Register("EcalPoint","Ecal",fEcalCollection,kTRUE);
  if (fInf->GetFastMC()==0)
    FairRootManager::Instance()->Register("EcalPointLite","EcalLite",fLiteCollection,kTRUE);
}
// -------------------------------------------------------------------------

// -----   Public method ConstructGeometry   -------------------------------
void CbmEcal::ConstructGeometry()
{
  FairGeoLoader*geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  TGeoVolume *volume;
  FairGeoMedium *CbmMedium;
  TGeoPgon *spl;

  Float_t *buf = 0;
//  Float_t sumWeight;
//  Int_t i;

  // create SensVacuum which is defined in the media file

  CbmMedium=Media->getMedium("SensVacuum");
  if (!CbmMedium) {
    Fatal("CbmEcal","Material SensVacuum not defined in media file.");
  }
  Int_t kMedSVac = geobuild->createMedium(CbmMedium);


   // Calculate other geometry parameters
  fCellSize       *= fGeoScale;
  fThicknessLead  *= fGeoScale;
  fThicknessScin  *= fGeoScale;
  fThicknessTyvk  *= fGeoScale;
  fThicknessLayer  = fThicknessLead + fThicknessScin + 2*fThicknessTyvk;
  fNLayers         = TMath::Nint(fNLayers/fGeoScale);

  // --------------- Container volume for the whole ECAL
  Double_t par[10], xPos, yPos, zPos;
  par[0] =  45; // phi_min
  par[1] = 360; // phi range
  par[2] =   4; // N of edges
  par[3] =   2; // N of z-planes
  //BVZ --> outer sensitive wall for control
  par[4] = -(fEcalSize[2]+.01)/2; // z1

  if (fEcalHole[0]<fEcalHole[1])
    par[5]=(fEcalHole[0]-.01)/2; // r1_min
  else
    par[5]=(fEcalHole[1]-.01)/2; // r1_min

  if (fEcalSize[0]>fEcalSize[1])
    par[6]=fEcalSize[0]/2;             // r1_max
  else
    par[6]=fEcalSize[1]/2;             // r1_max

  par[7] =  fEcalSize[2]/2; // z2
  par[8] =  par[5];              // r2_min
  par[9] =  par[6];              // r2_max
  // This wall is created differently to satisfy 
  // CBM framework around sensitive volumes

  spl = new TGeoPgon(&par[0]);

  volume = new TGeoVolume("WALL",spl,gGeoManager->GetMedium(kMedSVac));

  gGeoManager->Node("WALL", 1, "cave", 0.,0.,fZEcal+fEcalSize[2]/2,0,kTRUE,buf,0);

  fVolArr[0]=volume->GetNumber();
  AddSensitiveVolume(volume);

  if (fInf->GetFastMC()==1) return;

  //create necessary media which are defined in the media file

  CbmMedium=Media->getMedium("ECALVacuum");
  if (!CbmMedium) {
    Fatal("CbmEcal","Material ECALVacuum not defined in media file.");
  }
  Int_t kMedVac = geobuild->createMedium(CbmMedium);

  CbmMedium=Media->getMedium(fAbsorber);
  if (!CbmMedium) {
    Fatal("CbmEcal","Material %s not defined in media file.", fAbsorber.Data());
  }
  Int_t kMedLead = geobuild->createMedium(CbmMedium);

  CbmMedium=Media->getMedium("Scintillator");
  if (!CbmMedium) {
    Fatal("CbmEcal","Material Scintillator not defined in media file.");
  }
  Int_t kMedScin = geobuild->createMedium(CbmMedium);

  CbmMedium=Media->getMedium("Tyvek");
  if (!CbmMedium) {
    Fatal("CbmEcal","Material Tyvek not defined in media file.");
  }
  Int_t kMedTyvk = geobuild->createMedium(CbmMedium);



  par[4] = -fEcalSize[2]/2; // z1
  if (fEcalHole[0]<fEcalHole[1])
    par[5]=fEcalHole[0]/2; // r1_min
  else
    par[5]=fEcalHole[1]/2; // r1_min

  if (fEcalSize[0]>fEcalSize[1])
    par[6]=fEcalSize[0]/2;             // r1_max
  else
    par[6]=fEcalSize[1]/2;             // r1_max
  //par[5] =  fEcalHole[0]/2; // r1_min
  //par[6] =  fEcalSize[0]/2; // r1_max
  par[7] = -par[4];        // z2
  par[8] =  par[5];        // r2_min
  par[9] =  par[6];        // r2_max

  spl=new TGeoPgon(&par[0]);
  volume=new TGeoVolume("ECAL",spl,gGeoManager->GetMedium(kMedVac));

  gGeoManager->Node("ECAL", 1, "WALL", 0.,0.,0.,0,kTRUE,buf,0);

  // --------------- Upper and bottom parts of ECAL
  par[0] =  fEcalSize[0]/2;
  par[1] = (fEcalSize[1]-fEcalHole[1])/4;
  par[2] =  fEcalSize[2]/2;

  volume = gGeoManager->Volume("ECAL1", "BOX", kMedVac, par, 3);
  yPos = par[1] + fEcalHole[1]/2;
  gGeoManager->Node("ECAL1", 1, "ECAL", 0.,+yPos,0.,0,kTRUE,buf,0);
  gGeoManager->Node("ECAL1", 2, "ECAL", 0.,-yPos,0.,0,kTRUE,buf,0);

  volume = gGeoManager->Division("ECAL1column","ECAL1"      ,1,-1,-par[0],fCellSize,0,"S");
  volume = gGeoManager->Division("ECAL1row"   ,"ECAL1column",2,-1,-par[1],fCellSize,0,"S");
  
  fNColumns1 = TMath::Nint((par[0]/fCellSize)*2);
  fNRows1    = TMath::Nint((par[1]/fCellSize)*2);

  // --------------- Left and right parts of ECAL
  par[0] = (fEcalSize[0]-fEcalHole[0])/4;
  par[1] =  fEcalHole[1]/2;
  par[2] =  fEcalSize[2]/2;
  
  volume = gGeoManager->Volume("ECAL2", "BOX", kMedVac, par, 3);
  
  xPos = par[0] + fEcalHole[0]/2;
  gGeoManager->Node("ECAL2", 1, "ECAL", +xPos,0.,0.,0,kTRUE,buf,0);
  gGeoManager->Node("ECAL2", 2, "ECAL", -xPos,0.,0.,0,kTRUE,buf,0);
  
  volume = gGeoManager->Division("ECAL2column","ECAL2"      ,1,-1,-par[0],fCellSize,0,"S");
  volume = gGeoManager->Division("ECAL2row"   ,"ECAL2column",2,-1,-par[1],fCellSize,0,"S");
  
  fNColumns2 = TMath::Nint((par[0]/fCellSize)*2);
  fNRows2    = TMath::Nint((par[1]/fCellSize)*2);

  Int_t colmax=TMath::Max(fNColumns1,fNColumns2);
  Int_t rowmax=TMath::Max(fNRows1,fNRows2);
  Int_t fVolumeIDmax = (2*(4-1)+2-1)*colmax*rowmax +
    colmax*rowmax + rowmax;
#ifdef _DECAL
  cout << "fNColumns1=" << fNColumns1 << ", fNColumns2=" << fNColumns2 << endl;
  cout << "fNRows1=" << fNRows1 << ", fNRows2=" << fNRows2 << endl;
  cout << "fVolumeIDmax=" <<fVolumeIDmax << ".fVolIDmax=" << fVolIdMax <<endl;
#endif  
  fVolIdMax=fVolumeIDmax;
  // --------------- Preshower cell (lead + scintillator)
  Float_t psThickness = fThicknessPSlead + fThicknessPSscin;
  TGeoBBox *shape;
  par[0] = fCellSize/2;
  par[1] = fCellSize/2;
  par[2] = psThickness/2;

  if (psThickness>0)
  {
    volume = gGeoManager->Volume("ECALPScell", "BOX", kMedLead, par, 3);
    fVolArr[1]=volume->GetNumber();
    AddSensitiveVolume(volume);
  
    zPos = fEcalSize[2]/2 - par[2];
    gGeoManager->Node("ECALPScell", 1, "ECAL1row", 0.,0.,-zPos,0,kTRUE,buf,0);
    gGeoManager->Node("ECALPScell", 2, "ECAL2row", 0.,0.,-zPos,0,kTRUE,buf,0);

    // Preshower Scintillator tile is a sensitive volume
    par[2] = fThicknessPSscin/2;
    shape = new TGeoBBox("ECALPSscin", par[0], par[1], par[2]);
  
    volume = new TGeoVolume("ECALPSscin",shape,gGeoManager->GetMedium(kMedScin));

    fVolArr[2]=volume->GetNumber();
    
    //  gGeoManager->AddVolume(volume);
  
    zPos = psThickness/2 - par[2];
    gGeoManager->Node("ECALPSscin", 1, "ECALPScell", 0.,0.,zPos,0,kTRUE,buf,0);

    AddSensitiveVolume(volume);
  }
  else
  {
    fVolArr[2]=-1111;
    fVolArr[1]=-1111;
    Info("ConstructGeometry"," Constructing calorimeter without PS.");
  }

  // --------------- ECAL cell
  par[0] = fCellSize/2;
  par[1] = fCellSize/2;
  par[2] = (fThicknessLayer * fNLayers)/2;
  volume = gGeoManager->Volume("ECALcell", "BOX", 1, par, 3);
  zPos = fEcalSize[2]/2 - par[2] - psThickness - fEcalPSgap/2;
  gGeoManager->Node("ECALcell", 1, "ECAL1row", 0.,0.,-zPos,0,kTRUE,buf,0);
  gGeoManager->Node("ECALcell", 2, "ECAL2row", 0.,0.,-zPos,0,kTRUE,buf,0);

  // ----- One layer of the cell consisting of Pb, Sci and Tyvek

  volume = gGeoManager->Division("ECALlayer" ,"ECALcell"  ,3,-1,-par[2],fThicknessLayer,0,"S");

  par[2]=fThicknessTyvk+(fThicknessScin+fThicknessLead)/2;
  shape=new TGeoBBox("ECALlead",par[0],par[1],par[2]);
  volume = new TGeoVolume("ECALlead",shape,gGeoManager->GetMedium(kMedLead));

  fVolArr[4]=volume->GetNumber();
  
  //gGeoManager->AddVolume(volume);
  AddSensitiveVolume(volume);
  
  gGeoManager->Node("ECALlead",1,"ECALlayer",0.0,0.0,0.0,0,kTRUE,buf,0);
  
  if (fThicknessTyvk!=0.0) {	  
    par[2] = fThicknessTyvk + fThicknessScin/2;
    shape=new TGeoBBox("ECALtyvk",par[0],par[1],par[2]);
    volume = new TGeoVolume("ECALtyvk",shape,gGeoManager->GetMedium(kMedTyvk));

    fVolArr[3]=volume->GetNumber();
    //gGeoManager->AddVolume(volume);
    
    zPos = fThicknessLayer/2 - par[2];
    gGeoManager->Node("ECALtyvk",1,"ECALlead",0.0,0.0,zPos,0,kTRUE,buf,0);
    AddSensitiveVolume(volume);
   
    // Scintillator tile is a sensitive volume
    par[2] = fThicknessScin/2;
    shape = new TGeoBBox("ECALscin", par[0], par[1], par[2]);
    volume = new TGeoVolume("ECALscin",shape,gGeoManager->GetMedium(kMedScin));

    fVolArr[5]=volume->GetNumber();
    //gGeoManager->AddVolume(volume);

    gGeoManager->Node("ECALscin", 1, "ECALtyvk", 0.,0.,0.,0,kTRUE,buf,0);
    
   AddSensitiveVolume(volume);

  }
  else {
    cout << "Constructing ECAL without tyvec" << endl;
    par[2] = fThicknessScin/2;
    shape = new TGeoBBox("ECALscin", par[0], par[1], par[2]);
    volume = new TGeoVolume("ECALscin",shape,gGeoManager->GetMedium(kMedScin));

    fVolArr[5]=volume->GetNumber();
    //gGeoManager->AddVolume(volume);
    zPos = fThicknessLayer/2 - par[2];
    gGeoManager->Node("ECALscin", 1, "ECALlead", 0.,0.,zPos,0,kTRUE,buf,0);
    AddSensitiveVolume(volume);
  }

}
// -------------------------------------------------------------------------

// ----- Public method BeginEvnt  ------------------------------------------
void CbmEcal::BeginEvent()
{
}
// -------------------------------------------------------------------------

Int_t CbmEcal::GetVolIdMaxInf()
{
  static CbmEcalInf* inf=NULL;
  if (inf==NULL)
  {
    inf=CbmEcalInf::GetInstance(NULL);
    if (inf==NULL)
    {
      cerr << "Can't get geometry information!!!" << endl;
      return kFALSE;
    }
  }
  static Double_t cellsize=inf->GetCellSize();
  static Double_t xcalosize=inf->GetEcalSize(0);
  static Double_t ycalosize=inf->GetEcalSize(1);
  static Double_t xholesize=inf->GetEcalHole(0);
  static Double_t yholesize=inf->GetEcalHole(1);
  static Int_t NColumns1 = TMath::Nint((xcalosize/2/cellsize)*2);
  static Int_t NRows1    = TMath::Nint(((ycalosize-yholesize)/4/cellsize)*2);
  static Int_t NColumns2 = TMath::Nint(((xcalosize-xholesize)/4/cellsize)*2);
  static Int_t NRows2    = TMath::Nint((yholesize/2/cellsize)*2);
  static Int_t colmax=TMath::Max(NColumns1,NColumns2);
  static Int_t rowmax=TMath::Max(NRows1,NRows2);
  static Int_t VolIdMax=colmax*rowmax+rowmax+(2*(4-1)+2-1)*colmax*rowmax;
  return VolIdMax;
}

Int_t CbmEcal::GetVolIdMax()
{
  static FairRunAna* ana = FairRunAna::Instance();
  if (ana==NULL)
  {
    return GetVolIdMaxInf();
  }
  static FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  static CbmGeoEcalPar* par=(CbmGeoEcalPar*)(rtdb->getContainer("CbmGeoEcalPar"));
  static Double_t cellsize=par->GetVariable("cellsize");
  static Double_t xcalosize=par->GetVariable("xecalsize");
  static Double_t ycalosize=par->GetVariable("yecalsize");
  static Double_t xholesize=par->GetVariable("xholesize");
  static Double_t yholesize=par->GetVariable("yholesize");
  if (cellsize==-1111||xcalosize==-1111||ycalosize==-1111||xholesize==-1111||yholesize==-1111)
  {
    static Int_t here=0;
    if (here==0)
    {
      cerr << endl;
      cerr << "-W-: CbmEcal. Not enough information in";
      cerr << " container file!" << endl;
      cerr << "Using CbmEcalInf as information source";
      cerr << endl;
      here=1;
    }
    return GetVolIdMaxInf();
  }
  static Int_t NColumns1 = TMath::Nint((xcalosize/2/cellsize)*2);
  static Int_t NRows1    = TMath::Nint(((ycalosize-yholesize)/4/cellsize)*2);
  static Int_t NColumns2 = TMath::Nint(((xcalosize-xholesize)/4/cellsize)*2);
  static Int_t NRows2    = TMath::Nint((yholesize/2/cellsize)*2);
  static Int_t colmax=TMath::Max(NColumns1,NColumns2);
  static Int_t rowmax=TMath::Max(NRows1,NRows2);
  static Int_t VolIdMax=colmax*rowmax+rowmax+(2*(4-1)+2-1)*colmax*rowmax;
  return VolIdMax;

}

// ----- Public method GetCellCoordInf ----------------------------------------
Bool_t CbmEcal::GetCellCoordInf(Int_t fVolID, Float_t &x, Float_t &y, Int_t& tenergy)
{
  static CbmEcalInf* inf=NULL;
  if (inf==NULL)
  {
    inf=CbmEcalInf::GetInstance(NULL);
    if (inf==NULL)
    {
      cerr << "Can't get geometry information!!!" << endl;
      return kFALSE;
    }
  }
  static Double_t cellsize=inf->GetCellSize();
  static Double_t xcalosize=inf->GetEcalSize(0);
  static Double_t ycalosize=inf->GetEcalSize(1);
  static Double_t xholesize=inf->GetEcalHole(0);
  static Double_t yholesize=inf->GetEcalHole(1);
  static Int_t NColumns1 = TMath::Nint((xcalosize/2/cellsize)*2);
  static Int_t NRows1    = TMath::Nint(((ycalosize-yholesize)/4/cellsize)*2);
  static Int_t NColumns2 = TMath::Nint(((xcalosize-xholesize)/4/cellsize)*2);
  static Int_t NRows2    = TMath::Nint((yholesize/2/cellsize)*2);
  static Int_t colmax=TMath::Max(NColumns1,NColumns2);
  static Int_t rowmax=TMath::Max(NRows1,NRows2);
  static Int_t VolIdMax=colmax*rowmax+rowmax+(2*(4-1)+2-1)*colmax*rowmax;
  Int_t fVolumeID=fVolID;

  Int_t irow;
  Int_t icol;
  Int_t iblock;
  Int_t icopy;
  Int_t code;

  if (fVolumeID<0) return kFALSE;
  if (fVolumeID>VolIdMax) {
    tenergy=1;
    fVolumeID-=VolIdMax;
  }
  else
    tenergy=0;
  irow=fVolumeID%rowmax;
  icol=((fVolumeID-irow)/rowmax)%colmax;

  code=(fVolumeID-icol*rowmax-irow)/(rowmax*colmax);
  icopy=code%2;
  iblock=(code-icopy)/2+1;

  x=0.0;
  y=0.0;

  if (iblock%2==1) {
    x=(icol*cellsize-xcalosize/2.0);
    y=(irow*cellsize+yholesize/2.0);
    if (1==icopy) y-=(ycalosize+yholesize)/2.0;
    if (iblock>2) return kFALSE; else return kTRUE;
  }
  x=(icol*cellsize+xholesize/2.0);
  y=(irow*cellsize-yholesize/2.0);
  if (1==icopy) x-=(xcalosize+xholesize)/2.0;
  if (iblock>2) return kFALSE; else return kTRUE;
}

Bool_t CbmEcal::GetCellCoord(Int_t fVolID, Float_t &x, Float_t &y, Int_t& tenergy)
{
  static FairRunAna* ana = FairRunAna::Instance();
  if (ana==NULL)
  {
    return GetCellCoordInf(fVolID, x, y, tenergy);
  }
  static FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  static CbmGeoEcalPar* par=(CbmGeoEcalPar*)(rtdb->getContainer("CbmGeoEcalPar"));
  static Double_t cellsize=par->GetVariable("cellsize");
  static Double_t xcalosize=par->GetVariable("xecalsize");
  static Double_t ycalosize=par->GetVariable("yecalsize");
  static Double_t xholesize=par->GetVariable("xholesize");
  static Double_t yholesize=par->GetVariable("yholesize");
  if (cellsize==-1111||xcalosize==-1111||ycalosize==-1111||xholesize==-1111||yholesize==-1111)
  {
    static Int_t here=0;
    if (here==0)
    {
      cerr << endl;
      cerr << "-W-: CbmEcal. Not enough information in";
      cerr << " container file!" << endl;
      cerr << "Using CbmEcalInf as information source";
      cerr << endl;
      here=1;
    }
    return GetCellCoordInf(fVolID, x, y, tenergy);
  }
  
  static Int_t NColumns1 = TMath::Nint((xcalosize/2/cellsize)*2);
  static Int_t NRows1    = TMath::Nint(((ycalosize-yholesize)/4/cellsize)*2);
  static Int_t NColumns2 = TMath::Nint(((xcalosize-xholesize)/4/cellsize)*2);
  static Int_t NRows2    = TMath::Nint((yholesize/2/cellsize)*2);
  static Int_t colmax=TMath::Max(NColumns1,NColumns2);
  static Int_t rowmax=TMath::Max(NRows1,NRows2);
  static Int_t VolIdMax=colmax*rowmax+rowmax+(2*(4-1)+2-1)*colmax*rowmax;
  Int_t fVolumeID=fVolID;

  Int_t irow;
  Int_t icol;
  Int_t iblock;
  Int_t icopy;
  Int_t code;

  if (fVolumeID<0) return kFALSE;
  if (fVolumeID>VolIdMax) {
    tenergy=1;
    fVolumeID-=VolIdMax;
  }
  else
    tenergy=0;
  irow=fVolumeID%rowmax;
  icol=((fVolumeID-irow)/rowmax)%colmax;

  code=(fVolumeID-icol*rowmax-irow)/(rowmax*colmax);
  icopy=code%2;
  iblock=(code-icopy)/2+1;

  x=0.0;
  y=0.0;

  if (iblock%2==1) {
    x=(icol*cellsize-xcalosize/2.0);
    y=(irow*cellsize+yholesize/2.0);
    if (1==icopy) y-=(ycalosize+yholesize)/2.0;
    if (iblock>2) return kFALSE; else return kTRUE;
  }
  x=(icol*cellsize+xholesize/2.0);
  y=(irow*cellsize-yholesize/2.0);
  if (1==icopy) x-=(xcalosize+xholesize)/2.0;
  if (iblock>2) return kFALSE; else return kTRUE;
}

// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------    
CbmEcalPoint* CbmEcal::AddHit(Int_t trackID, Int_t detID, TVector3 pos,         
			      TVector3 mom, Double_t time, Double_t length,       
			      Double_t eLoss)
{
  TClonesArray& clref = *fEcalCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmEcalPoint(trackID, detID, pos, mom,
                                      time, length, eLoss);
}                                                                               
// -------------------------------------------------------------------------

// -----   Private method AddHit   -----------------------------------------    
CbmEcalPointLite* CbmEcal::AddLiteHit(Int_t trackID, Int_t detID, Double32_t time, Double32_t eLoss)
{
  TClonesArray& clref = *fLiteCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmEcalPointLite(trackID, detID, time, eLoss);
}
// -------------------------------------------------------------------------

ClassImp(CbmEcal)
