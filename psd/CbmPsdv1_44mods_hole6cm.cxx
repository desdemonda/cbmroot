// -------------------------------------------------------------------------
// -----                       CbmPsdv1 source file                   -----
// -----                  Created 04/02/05  by Alla Maevskaya              -----
// -------------------------------------------------------------------------

#include "CbmPsdv1_44mods_hole6cm.h"

#include "CbmPsdPoint.h"
#include "CbmStack.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoBBox.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::ofstream;

// -----   Default constructor   -------------------------------------------
CbmPsdv1_44mods_hole6cm::CbmPsdv1_44mods_hole6cm() 
: CbmPsd(),
  fTrackID(0),
  fVolumeID(0),
  fModuleID(0),
  fPos(),
  fMom(),
  fTime(0.),
  fLength(0.),
  fEloss(0.),
  fPosIndex(0),
  fPsdCollection(new TClonesArray("CbmPsdPoint")),
  fXshift(0.),
  fZposition(0.),
  fRotYAngle(0.),
  fGeoFile("")
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmPsdv1_44mods_hole6cm::CbmPsdv1_44mods_hole6cm(const char* name, Bool_t active)
: CbmPsd(name, active),
  fTrackID(0),
  fVolumeID(0),
  fModuleID(0),
  fPos(),
  fMom(),
  fTime(0.),
  fLength(0.),
  fEloss(0.),
  fPosIndex(0),
  fPsdCollection(new TClonesArray("CbmPsdPoint")),
  fXshift(0.),
  fZposition(0.),
  fRotYAngle(0.),
  fGeoFile("")
{
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdv1_44mods_hole6cm::~CbmPsdv1_44mods_hole6cm() {
 if (fPsdCollection) {
   fPsdCollection->Delete(); 
   delete fPsdCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmPsdv1_44mods_hole6cm::ProcessHits(FairVolume* vol)
{
  //  cout<<" CbmPsdv1_44mods_hole6cm::ProcessHits in "<<vol->GetName()<<endl;
  if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  Int_t pdg;
  // Set parameters at entrance of volume. Reset ELoss.
  
  if ( gMC->IsTrackEntering() ) 
  {
    ResetParameters();
    fEloss   = 0.;
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fTime    = gMC->TrackTime() * 1.0e09;
    fLength  = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    Int_t copyNo;
    gMC->CurrentVolID(copyNo);
    //fVolumeID=copyNo;
    Int_t iCell, iNumm ;
    gMC->CurrentVolOffID(2, iCell);  // SELIM: change 1 -> 2
    fModuleID=iCell;//module
    gMC->CurrentVolOffID(1, iNumm);  // SELIM: change 2 -> 1
    fVolumeID=iNumm;//scint
    pdg = gMC->TrackPid();
  }
  
  if (gMC->IsTrackInside() ) 
  {
    // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
    
    if( gMC->IsTrackStop() ||
	gMC->IsTrackDisappeared()   ) 
    {
      fEloss += gMC->Edep();
      
      if (pdg == 1000791970)
      // if (strstr(fDebug,"hit"))
      Info("ProcessHits track inside","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	     fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	     fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
      
      // Create CbmPsdPoint
      CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				    TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
      fPoint->SetModuleID(fModuleID);
      fPoint->SetTrackID(fTrackID);
    }
  }
  
  if ( gMC->IsTrackExiting() /*   ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared() */  ) 
  {
     // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();    
   
    if (pdg == 1000791970)
      //    if (strstr(fDebug,"hit"))
    Info("ProcessHits","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	   fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	   fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
    
    // Create CbmPsdPoint
    CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				  TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
    fPoint->SetModuleID(fModuleID);
    fPoint->SetTrackID(fTrackID);
  }
  
  ((CbmStack*)gMC->GetStack())->AddPoint(kPSD, fTrackID);
 
  //  ResetParameters();
    
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method CreateMaterial   ----------------------------------
void CbmPsdv1_44mods_hole6cm::CreateMaterial() {
}
// -----   Public method ConstructGeometry   -------------------------------
void CbmPsdv1_44mods_hole6cm::ConstructGeometry() {
  // Create materials, media and volume

// Create materials, media and volume
  //-----------List of Materials and Mixtures--------------
  
  TGeoMaterial *mat10 = new TGeoMaterial("IRON",55.85,26,7.87);
  mat10->SetUniqueID(  10);
  TGeoMaterial *mat13 = new TGeoMaterial("LEAD",207.19,82,11.35);
  mat13->SetUniqueID(  13);
  TGeoMaterial *mat15 = new TGeoMaterial("AIR",14.61,7.3,0.1205000E-02);
  mat15->SetUniqueID(  15);
  TGeoMaterial *mat16 = new TGeoMaterial("VACUUM",0,0,0);
  mat16->SetUniqueID(  16);
  TGeoMaterial *mat20 = new TGeoMaterial("SILICON",28.09,14,2.33);
  mat20->SetUniqueID(  20);
  TGeoMaterial *mat24 = new TGeoMaterial("HE_GAS",4,2,0.1780000E-03);
  mat24->SetUniqueID(  24);
  TGeoMixture *mat25 = new TGeoMixture("PLASTIC",2,   1.03200    );
  mat25->SetUniqueID(  25);
  mat25->DefineElement(0,12.01,6,0.9225687);
  mat25->DefineElement(1,1.008,1,0.7743125E-01);
  TGeoMixture *mat38 = new TGeoMixture("TYVEC",2,  0.930000    );
  mat38->SetUniqueID(  38);
  mat38->DefineElement(0,12.011,6,0.8562772);
  mat38->DefineElement(1,1.008,1,0.1437228);
  
  //-----------List of Tracking Media--------------
  
  TGeoMedium *med1 = new TGeoMedium("AIR",      1,15,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med2 = new TGeoMedium("VACUUM",   2,16,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med9 = new TGeoMedium("IRON",     9,10,0,1,0.19,1,-1,-1,0.1,-1);
  TGeoMedium *med10 = new TGeoMedium("HE_GAS", 10,24,0,1,0.19,0.25,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med11 = new TGeoMedium("PLASTIC",11,25,1,0,0,1,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med24 = new TGeoMedium("LEAD",   24,13,0,0,0,1,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med27 = new TGeoMedium("PLASTIC",27,25,0,1,0.19,1,-1,-1,0.1000000E-02,-1);
  TGeoMedium *med32 = new TGeoMedium("TYVEC",  32,38,0,0,0,1,-1,-1,0.1000000E-02,-1);

 
  //-----------List of Rotation matrices--------------
  
  TGeoMaterial *material = 0;
  TGeoMedium   *medium   = 0;
  Float_t *buf = 0;
  
  TGeoVolume *VETO = gGeoManager->MakeBox("VETO",med1, 70, 70, 62.4);
  TGeoVolume *VHLL = gGeoManager->MakeBox("VHLL",med1,10,10,62.4);//hole
  
  //TGeoVolume *VMHL = gGeoManager->MakeBox("VMHL",med1,10,10,62.4);//mod, where 4 small modules are inserted

  /*
//Small modules
TGeoVolume *VMOD = gGeoManager->MakeBox("VMOD",med9,5,5,62.4);
VMOD->SetLineColor(4);
TGeoVolume *VFE_ = gGeoManager->MakeBox("VFE_",med9,4.85,4.95,1);
VFE_->SetLineColor(4);
TGeoVolume *VPB_ = gGeoManager->MakeBox("VPB_",med24,4.85,4.95,0.8);
VPB_->SetLineColor(4);
TGeoVolume *VTYV = gGeoManager->MakeBox("VTYV",med32,4.85,4.95,0.22);
VTYV->SetLineColor(6);
TGeoVolume *VSC_ = gGeoManager->MakeBox("VSC_",med11,4.83,4.93,0.2);
VSC_->SetLineColor(5);
TGeoVolume *VRFE = gGeoManager->MakeBox("VRFE",med27,0.15,3.7,1);
VRFE->SetLineColor(2);
TGeoVolume *VRPB = gGeoManager->MakeBox("VRPB",med27,0.15,3.7,0.8);
VRPB->SetLineColor(2);
TGeoVolume *VRTY = gGeoManager->MakeBox("VRTY",med27,0.1000000E-01,3.7,0.22);
VRTY->SetLineColor(2);
TGeoVolume *VRTA = gGeoManager->MakeBox("VRTA",med27,0.14,3.7,0.1000000E-01);
VRTA->SetLineColor(3);
TGeoVolume *VRSC = gGeoManager->MakeBox("VRSC",med27,0.14,3.7,0.2);
VRSC->SetLineColor(2);
  */

  //Large modules
  TGeoVolume *VMDL = gGeoManager->MakeBox("VMDL",med9,10,10,62.4);
  TGeoVolume *VFEL = gGeoManager->MakeBox("VFEL",med9,9.9,9.85,1);
  TGeoVolume *VPBL = gGeoManager->MakeBox("VPBL",med24,9.9,9.85,0.8);
  TGeoVolume *VTYL = gGeoManager->MakeBox("VTYL",med32,9.9,9.85,0.22);
  TGeoVolume *VSCL = gGeoManager->MakeBox("VSCL",med11,9.88,9.83,0.2);
  TGeoVolume *VRFL = gGeoManager->MakeBox("VRFL",med27,0.1,8.6,1);
  TGeoVolume *VRPL = gGeoManager->MakeBox("VRPL",med27,0.1,8.6,0.8);
  TGeoVolume *VRYL = gGeoManager->MakeBox("VRYL",med27,0.01,8.6,0.22);
  TGeoVolume *VRAL = gGeoManager->MakeBox("VRAL",med27,0.09,8.6,0.01);
  TGeoVolume *VRSL = gGeoManager->MakeBox("VRSL",med27,0.09,8.6,0.2);
 


// ================== part A =======================
//VOLUMES part A (large mod) where holes diam. 6cm will be inserted
TGeoVolume *VMHA = gGeoManager->MakeBox("VMHA",med9,10,10,62.4);//mod, where all volumes with hole are inserted
VMHA->SetLineColor(kGreen);
TGeoVolume *VFHA = gGeoManager->MakeBox("VFHA",med9,9.85,9.85,1);//Fe
VFHA->SetLineColor(kGreen);
TGeoVolume *VPHA = gGeoManager->MakeBox("VPHA",med24,9.85,9.85,0.8);//Pb
VPHA->SetLineColor(kGreen);
TGeoVolume *VTHA = gGeoManager->MakeBox("VTHA",med32,9.85,9.85,0.22);//Tyvec
VTHA->SetLineColor(kGreen);
TGeoVolume *VSAH = gGeoManager->MakeBox("VSAH",med11,9.83,9.83,0.2);//Scin.
 AddSensitiveVolume(VSAH);
VSAH->SetLineColor(kGreen);

//HOLES (TUBS) part A  in different volumes (diam. 6cm)  
TGeoVolume *VHFA = gGeoManager->MakeTubs("VHFA",med2,0.,2.788,1,0.,90.); //in Fe 
VHFA->SetLineColor(kRed+3);
TGeoVolume *VHPA = gGeoManager->MakeTubs("VHPA",med2,0.,2.788,0.8,0.,90.); //in Pb
VHPA->SetLineColor(kRed+3);
TGeoVolume *VHTA = gGeoManager->MakeTubs("VHTA",med2,0.,2.788,0.01,0.,90.); //in Tyvec
VHTA->SetLineColor(kRed+3);
TGeoVolume *VHSA = gGeoManager->MakeTubs("VHSA",med2,0.,2.76,0.2,0.,90.); //in Scin.
VHSA->SetLineColor(kRed+3);


//Large module part A  with hole
  VMHA->AddNode(VFHA,1,new TGeoTranslation(0,0,-61.4));
  //VFHA->AddNode(VRFL,1,new TGeoTranslation(4.7,0.75,0));
  VFHA->AddNode(VHFA,1,new TGeoTranslation(-9.85,-9.85,0.));
  VMHA->AddNode(VFHA,2,new TGeoTranslation(0,0,61.4));

  VMHA->AddNode(VTHA,1,new TGeoTranslation(0,0,-60.18));
  //VTHL->AddNode(VRYL,2,new TGeoTranslation(9.89,0.75,0));
  //VTHL->AddNode(VRAL,3,new TGeoTranslation(9.79,0.75,-0.21));
  //VTHL->AddNode(VRAL,4,new TGeoTranslation(9.79,0.75,0.21));
  VTHA->AddNode(VHTA,1,new TGeoTranslation(-9.85,-9.85,-0.21));
  VTHA->AddNode(VHTA,2,new TGeoTranslation(-9.85,-9.85,0.21));

  VTHA->AddNode(VSAH,1,gGeoIdentity);
  AddSensitiveVolume(VSAH);
  //VSLH->AddNode(VRSL,2,new TGeoTranslation(9.79,0.75,0));
  VSAH->AddNode(VHSA,1,new TGeoTranslation(-9.83,-9.83,0.));

  VMHA->AddNode(VPHA,2,new TGeoTranslation(0,0,-59.16));
  //VPHL->AddNode(VRPL,2,new TGeoTranslation(9.8,0.75,0));
  VPHA->AddNode(VHPA,1,new TGeoTranslation(-9.85,-9.85,0.));

  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTHA",ivol, "VMHA", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      fNbOfSensitiveVol++;
      cout <<"A TYVEC " <<ivol <<" " <<zvtyl <<endl; 
   }
  for (Int_t ivol=2; ivol<60; ivol++)
    {
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPHA",ivol+1, "VMHA", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      cout <<"A LEAD " <<ivol+1 <<" " <<zvpbl <<endl; 
    }
  cout <<"fNbOfSensitiveVol part A" <<fNbOfSensitiveVol <<endl;


// ================== part B =======================
//VOLUMES part B (large mod) where holes diam. 6cm will be inserted 
TGeoVolume *VMHB = gGeoManager->MakeBox("VMHB",med9,10,10,62.4);//mod, where all volumes with hole are inserted
VMHB->SetLineColor(kBlue);
TGeoVolume *VFHB = gGeoManager->MakeBox("VFHB",med9,9.85,9.85,1);//Fe
VFHB->SetLineColor(kBlue);
TGeoVolume *VPHB = gGeoManager->MakeBox("VPHB",med24,9.85,9.85,0.8);//Pb
VPHB->SetLineColor(kBlue);
TGeoVolume *VTHB = gGeoManager->MakeBox("VTHB",med32,9.85,9.85,0.22);//Tyvec
VTHB->SetLineColor(kBlue);
TGeoVolume *VSBH = gGeoManager->MakeBox("VSBH",med11,9.83,9.83,0.2);//Scin.
 AddSensitiveVolume(VSBH);
VSBH->SetLineColor(kBlue);

//HOLES (TUBS) part B  in different volumes (diam. 6cm)  
TGeoVolume *VHFB = gGeoManager->MakeTubs("VHFB",med2,0.,2.788,1,90.,180.); //in Fe 
VHFB->SetLineColor(kRed+3);
TGeoVolume *VHPB = gGeoManager->MakeTubs("VHPB",med2,0.,2.788,0.8,90.,180.); //in Pb
VHPB->SetLineColor(kRed+3);
TGeoVolume *VHTB = gGeoManager->MakeTubs("VHTB",med2,0.,2.788,0.01,90.,180.); //in Tyvec
VHTB->SetLineColor(kRed+3);
TGeoVolume *VHSB = gGeoManager->MakeTubs("VHSB",med2,0.,2.76,0.2,90.,180.); //in Scin.
VHSB->SetLineColor(kRed+3);


//Large module part B  with hole
  VMHB->AddNode(VFHB,1,new TGeoTranslation(0,0,-61.4));
  //VFHB->AddNode(VRFL,1,new TGeoTranslation(4.7,0.75,0));
  VFHB->AddNode(VHFB,1,new TGeoTranslation(9.85,-9.85,0.));
  VMHB->AddNode(VFHB,2,new TGeoTranslation(0,0,61.4));

  VMHB->AddNode(VTHB,1,new TGeoTranslation(0,0,-60.18));
  //VTHL->AddNode(VRYL,2,new TGeoTranslation(9.89,0.75,0));
  //VTHL->AddNode(VRAL,3,new TGeoTranslation(9.79,0.75,-0.21));
  //VTHL->AddNode(VRAL,4,new TGeoTranslation(9.79,0.75,0.21));
  VTHB->AddNode(VHTB,1,new TGeoTranslation(9.85,-9.85,-0.21));
  VTHB->AddNode(VHTB,2,new TGeoTranslation(9.85,-9.85,0.21));

  VTHB->AddNode(VSBH,1,gGeoIdentity);
  AddSensitiveVolume(VSBH);
  //VSLH->AddNode(VRSL,2,new TGeoTranslation(9.79,0.75,0));
  VSBH->AddNode(VHSB,1,new TGeoTranslation(9.83,-9.83,0.));

  VMHB->AddNode(VPHB,2,new TGeoTranslation(0,0,-59.16));
  //VPHL->AddNode(VRPL,2,new TGeoTranslation(9.8,0.75,0));
  VPHB->AddNode(VHPB,1,new TGeoTranslation(9.85,-9.85,0.));

  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTHB",ivol, "VMHB", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      fNbOfSensitiveVol++;
      cout <<"B TYVEC " <<ivol <<" " <<zvtyl <<endl; 
   }
  for (Int_t ivol=2; ivol<60; ivol++)
    {
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPHB",ivol+1, "VMHB", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      cout <<"B LEAD " <<ivol+1 <<" " <<zvpbl <<endl; 
    }
  cout <<"fNbOfSensitiveVol part B " <<fNbOfSensitiveVol <<endl;

// ================== part C =======================
//VOLUMES part C (large mod) where holes diam. 6cm will be inserted 
TGeoVolume *VMHC = gGeoManager->MakeBox("VMHC",med9,10,10,62.4);//mod, where all volumes with hole are inserted
VMHC->SetLineColor(kMagenta);
TGeoVolume *VFHC = gGeoManager->MakeBox("VFHC",med9,9.85,9.85,1);//Fe
VFHC->SetLineColor(kMagenta);
TGeoVolume *VPHC = gGeoManager->MakeBox("VPHC",med24,9.85,9.85,0.8);//Pb
VPHC->SetLineColor(kMagenta);
TGeoVolume *VTHC = gGeoManager->MakeBox("VTHC",med32,9.85,9.85,0.22);//Tyvec
VTHC->SetLineColor(kMagenta);
TGeoVolume *VSCH = gGeoManager->MakeBox("VSCH",med11,9.83,9.83,0.2);//Scin.
 AddSensitiveVolume(VSCH);
VSCH->SetLineColor(kMagenta);

//HOLES (TUBS) part C  in different volumes (diam. 6cm)  
TGeoVolume *VHFC = gGeoManager->MakeTubs("VHFC",med2,0.,2.788,1,180.,270.); //in Fe 
VHFC->SetLineColor(kRed+3);
TGeoVolume *VHPC = gGeoManager->MakeTubs("VHPC",med2,0.,2.788,0.8,180.,270.); //in Pb
VHPC->SetLineColor(kRed+3);
TGeoVolume *VHTC = gGeoManager->MakeTubs("VHTC",med2,0.,2.788,0.01,180.,270.); //in Tyvec
VHTC->SetLineColor(kRed+3);
TGeoVolume *VHSC = gGeoManager->MakeTubs("VHSC",med2,0.,2.76,0.2,180.,270.); //in Scin.
VHSC->SetLineColor(kRed+3);


//Large module part C  with hole
  VMHC->AddNode(VFHC,1,new TGeoTranslation(0,0,-61.4));
  //VFHC->AddNode(VRFL,1,new TGeoTranslation(4.7,0.75,0));
  VFHC->AddNode(VHFC,1,new TGeoTranslation(9.85,9.85,0.));
  VMHC->AddNode(VFHC,2,new TGeoTranslation(0,0,61.4));

  VMHC->AddNode(VTHC,1,new TGeoTranslation(0,0,-60.18));
  //VTHL->AddNode(VRYL,2,new TGeoTranslation(9.89,0.75,0));
  //VTHL->AddNode(VRAL,3,new TGeoTranslation(9.79,0.75,-0.21));
  //VTHL->AddNode(VRAL,4,new TGeoTranslation(9.79,0.75,0.21));
  VTHC->AddNode(VHTC,1,new TGeoTranslation(9.85,9.85,-0.21));
  VTHC->AddNode(VHTC,2,new TGeoTranslation(9.85,9.85,0.21));

  VTHC->AddNode(VSCH,1,gGeoIdentity);
  AddSensitiveVolume(VSCH);
  //VSLH->AddNode(VRSL,2,new TGeoTranslation(9.79,0.75,0));
  VSCH->AddNode(VHSC,1,new TGeoTranslation(9.83,9.83,0.));

  VMHC->AddNode(VPHC,2,new TGeoTranslation(0,0,-59.16));
  //VPHL->AddNode(VRPL,2,new TGeoTranslation(9.8,0.75,0));
  VPHC->AddNode(VHPC,1,new TGeoTranslation(9.85,9.85,0.));

  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTHC",ivol, "VMHC", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      fNbOfSensitiveVol++;
      cout <<"C TYVEC " <<ivol <<" " <<zvtyl <<endl; 
   }
  for (Int_t ivol=2; ivol<60; ivol++)
    {
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPHC",ivol+1, "VMHC", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      cout <<"C LEAD " <<ivol+1 <<" " <<zvpbl <<endl; 
    }
  cout <<"fNbOfSensitiveVol part C " <<fNbOfSensitiveVol <<endl;

// ================== part D =======================
//VOLUMES part D (large mod) where holes diam. 6cm will be inserted 
TGeoVolume *VMHD = gGeoManager->MakeBox("VMHD",med9,10,10,62.4);//mod, where all volumes with hole are inserted
VMHD->SetLineColor(kOrange);
TGeoVolume *VFHD = gGeoManager->MakeBox("VFHD",med9,9.85,9.85,1);//Fe
VFHD->SetLineColor(kOrange);
TGeoVolume *VPHD = gGeoManager->MakeBox("VPHD",med24,9.85,9.85,0.8);//Pb
VPHD->SetLineColor(kOrange);
TGeoVolume *VTHD = gGeoManager->MakeBox("VTHD",med32,9.85,9.85,0.22);//Tyvec
VTHD->SetLineColor(kOrange);
TGeoVolume *VSDH = gGeoManager->MakeBox("VSDH",med11,9.83,9.83,0.2);//Scin.
 AddSensitiveVolume(VSDH);
VSDH->SetLineColor(kOrange);

//HOLES (TUBS) part D  in different volumes (diam. 6cm)  
TGeoVolume *VHFD = gGeoManager->MakeTubs("VHFD",med2,0.,2.788,1,270.,360.); //in Fe 
VHFD->SetLineColor(kRed+3);
TGeoVolume *VHPD = gGeoManager->MakeTubs("VHPD",med2,0.,2.788,0.8,270.,360.); //in Pb
VHPD->SetLineColor(kRed+3);
TGeoVolume *VHTD = gGeoManager->MakeTubs("VHTD",med2,0.,2.788,0.01,270.,360.); //in Tyvec
VHTD->SetLineColor(kRed+3);
TGeoVolume *VHSD = gGeoManager->MakeTubs("VHSD",med2,0.,2.76,0.2,270.,360.); //in Scin.
VHSD->SetLineColor(kRed+3);


//Small module part D  with hole
  VMHD->AddNode(VFHD,1,new TGeoTranslation(0,0,-61.4));
  //VFHD->AddNode(VRFL,1,new TGeoTranslation(4.7,0.75,0));
  VFHD->AddNode(VHFD,1,new TGeoTranslation(-9.85,9.85,0.));
  VMHD->AddNode(VFHD,2,new TGeoTranslation(0,0,61.4));

  VMHD->AddNode(VTHD,1,new TGeoTranslation(0,0,-60.18));
  //VTHL->AddNode(VRYL,2,new TGeoTranslation(9.89,0.75,0));
  //VTHL->AddNode(VRAL,3,new TGeoTranslation(9.79,0.75,-0.21));
  //VTHL->AddNode(VRAL,4,new TGeoTranslation(9.79,0.75,0.21));
  VTHD->AddNode(VHTD,1,new TGeoTranslation(-9.85,9.85,-0.21));
  VTHD->AddNode(VHTD,2,new TGeoTranslation(-9.85,9.85,0.21));

  VTHD->AddNode(VSDH,1,gGeoIdentity);
  AddSensitiveVolume(VSDH);
  //VSLH->AddNode(VRSL,2,new TGeoTranslation(9.79,0.75,0));
  VSDH->AddNode(VHSD,1,new TGeoTranslation(-9.83,9.83,0.));

  VMHD->AddNode(VPHD,2,new TGeoTranslation(0,0,-59.16));
  //VPHL->AddNode(VRPL,2,new TGeoTranslation(9.8,0.75,0));
  VPHD->AddNode(VHPD,1,new TGeoTranslation(-9.85,9.85,0.));

  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTHD",ivol, "VMHD", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      fNbOfSensitiveVol++;
      cout <<"D TYVEC " <<ivol <<" " <<zvtyl <<endl; 
   }
  for (Int_t ivol=2; ivol<60; ivol++)
    {
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPHD",ivol+1, "VMHD", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      cout <<"D LEAD " <<ivol+1 <<" " <<zvpbl <<endl; 
    }
  cout <<"fNbOfSensitiveVol part D " <<fNbOfSensitiveVol <<endl;


//HOLES (BOX)   
TGeoVolume *VHMV = gGeoManager->MakeBox("VHMV",med2,0.075,1.469,62.4); //vertical hole for VMHA
TGeoVolume *VHMH = gGeoManager->MakeBox("VHMH",med2,1.394,0.075,62.4); //horizontal hole for VMHA

//Horizontal and vertical holes in modules (parts A,B,C,D) 
  VMHA->AddNode(VHMV,1,new TGeoTranslation(-9.925,-8.531,0.));
  VMHA->AddNode(VHMH,1,new TGeoTranslation(-8.456,-9.925,0.));

  VMHB->AddNode(VHMV,2,new TGeoTranslation(9.925,-8.531,0.));
  VMHB->AddNode(VHMH,2,new TGeoTranslation(8.456,-9.925,0.));

  VMHC->AddNode(VHMV,3,new TGeoTranslation(9.925,8.531,0.));
  VMHC->AddNode(VHMH,3,new TGeoTranslation(8.456,9.925,0.));

  VMHD->AddNode(VHMV,4,new TGeoTranslation(-9.925,8.531,0.));
  VMHD->AddNode(VHMH,4,new TGeoTranslation(-8.456,9.925,0.));


//HOLES (BOX)   
TGeoVolume *VHTV = gGeoManager->MakeBox("VHTV",med2,0.01,1.39,0.2); //vertical hole for Tyvec
TGeoVolume *VHTH = gGeoManager->MakeBox("VHTH",med2,1.38,0.01,0.2); //horizontal hole for Tyvec

//Horizontal and vertical holes in Tyvec (parts A,B,C,D) 
  VTHA->AddNode(VHTV,1,new TGeoTranslation(-9.84,-8.46,0.));
  VTHA->AddNode(VHTH,1,new TGeoTranslation(-8.45,-9.84,0.));

  VTHB->AddNode(VHTV,2,new TGeoTranslation(9.84,-8.46,0.));
  VTHB->AddNode(VHTH,2,new TGeoTranslation(8.45,-9.84,0.));

  VTHC->AddNode(VHTV,3,new TGeoTranslation(9.84,8.46,0.));
  VTHC->AddNode(VHTH,3,new TGeoTranslation(8.45,9.84,0.));

  VTHD->AddNode(VHTV,4,new TGeoTranslation(-9.84,8.46,0.));
  VTHD->AddNode(VHTH,4,new TGeoTranslation(-8.45,9.84,0.));

  /*  
//Test empty mod to check geometry of  central mods (parts A,B,C,D)
  TGeoVolume *VTEL = gGeoManager->MakeBox("VTEL",med2,20,20,62.4);

  gGeoManager->Node("VMHA", 1, "VTEL", 10.,10.,0, 0, kTRUE, buf, 0); 
  gGeoManager->Node("VMHB", 1, "VTEL", -10.,10.,0, 0, kTRUE, buf, 0); 
  gGeoManager->Node("VMHD", 1, "VTEL", 10.,-10.,0, 0, kTRUE, buf, 0); 
  gGeoManager->Node("VMHC", 1, "VTEL", -10.,-10.,0, 0, kTRUE, buf, 0); 
  */

  Float_t xPSD = fXshift;
  Float_t zPSD = fZposition;
  Float_t rotPSD = fRotYAngle; //in deg.//marina

  //Float_t rotPSD = 0; 

  Float_t zdcSize[3]   = {  70.,   70.,  62.4 }; // SELIM: fZposition = 1 scintillator, not PSD center
  zPSD += zdcSize[2];
  
  TGeoRotation *rot = new TGeoRotation();
  rot->RotateY(rotPSD);

  //gGeoManager->Node("VETO", 1, "cave", xPSD, 0, zPSD, 0, kTRUE, buf, 0);
  ////gGeoManager->Node("VMDL", 1, "VETO", 60, 60,0., 0, kTRUE, buf, 0);

  //TGeoCombiTrans* rottrans = new TGeoCombiTrans(xPSD, 0., zPSD, rot);
  //gGeoManager->GetVolume("cave")->AddNode(VETO, 1, rottrans);//marina

  gGeoManager->GetVolume("cave")->AddNode(VETO, 1, new TGeoCombiTrans(xPSD, 0.,zPSD, rot));//marina
  //gGeoManager->GetVolume("cave")->AddNode(VTEL, 1, new TGeoCombiTrans(xPSD, 0.,zPSD+200., rot));//marina


//Large modules
  gGeoManager->Node("VFEL",1,"VMDL", 0, 0, -61.4, 0, kTRUE, buf, 0 );
  gGeoManager->Node("VRFL",1,"VFEL", 9.8, 0.75, 0, 0,kTRUE, buf, 0);
  gGeoManager->Node("VFEL",2,"VMDL", 0, 0, 61.4, 0, kTRUE, buf, 0 );

  gGeoManager->Node("VTYL",1, "VMDL", 0, 0, -60.18, 0,kTRUE, buf, 0);
  gGeoManager->Node("VRYL",1, "VTYL", 9.89, 0.75, 0,  0,kTRUE, buf, 0);
  gGeoManager->Node("VRAL", 1, "VTYL", 9.79, 0.75, -0.21, 0, kTRUE, buf, 0);
  gGeoManager->Node("VRAL", 2, "VTYL", 9.79, 0.75, 0.21,  0,kTRUE, buf, 0);

  gGeoManager->Node("VSCL", 1, "VTYL", 0, 0, 0,  0,kTRUE, buf, 0);
  AddSensitiveVolume(VSCL);
  gGeoManager->Node("VRSL", 1, "VSCL", 9.79, 0.75, 0, 0,kTRUE, buf, 0); 

  gGeoManager->Node("VPBL",2, "VMDL", 0, 0, -59.16,  0,kTRUE, buf, 0); 
  gGeoManager->Node("VRPL",1, "VPBL", 9.8, 0.75, 0, 0, kTRUE, buf, 0);

  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTYL",ivol, "VMDL", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      fNbOfSensitiveVol++;
      cout <<"Large TYVEC " <<ivol <<" " <<zvtyl <<endl; 
   }
  for (Int_t ivol=2; ivol<60; ivol++)
    {
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPBL",ivol+1, "VMDL", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      cout <<"Large LEAD " <<ivol+1 <<" " <<zvpbl <<endl; 
    }
  cout <<"fNbOfSensitiveVol large mods " <<fNbOfSensitiveVol <<endl;
    
//HOLE in central large module 
  //VETO->AddNode(VMHL,1,new TGeoTranslation(0.,0.,0.));
  //VETO->AddNode(VMHL,23,new TGeoTranslation(0.,0.,0.));
  /*
  VMHL->AddNode(VFHL,1,new TGeoTranslation(0,0,-61.4));
  VFHL->AddNode(VRFL,2,new TGeoTranslation(9.8,0.75,0));
  VFHL->AddNode(VHFL,1,new TGeoTranslation(0.,0.,0.));
  VMHL->AddNode(VFHL,2,new TGeoTranslation(0,0,61.4));

  VMHL->AddNode(VTHL,1,new TGeoTranslation(0,0,-60.18));
  VTHL->AddNode(VRYL,2,new TGeoTranslation(9.89,0.75,0));
  VTHL->AddNode(VRAL,3,new TGeoTranslation(9.79,0.75,-0.21));
  VTHL->AddNode(VRAL,4,new TGeoTranslation(9.79,0.75,0.21));
  VTHL->AddNode(VHTL,1,new TGeoTranslation(0.,0.,-0.21));
  VTHL->AddNode(VHTL,2,new TGeoTranslation(0.,0.,0.21));

  VTHL->AddNode(VSLH,1,gGeoIdentity);
  AddSensitiveVolume(VSLH);
  VSLH->AddNode(VRSL,2,new TGeoTranslation(9.79,0.75,0));
  VSLH->AddNode(VHSL,1,new TGeoTranslation(0.,0.,0.));

  VPHL->AddNode(VRPL,2,new TGeoTranslation(9.8,0.75,0));
  VPHL->AddNode(VHPL,1,new TGeoTranslation(0.,0.,0.));
  

  for (Int_t ivol=2; ivol<61; ivol++)// ERROR -> 2 cycles
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      gGeoManager->Node("VTHL",ivol, "VMHL", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      gGeoManager->Node("VPHL",ivol+1, "VMHL", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      fNbOfSensitiveVol++;
    }
  */


  /*
  //XY positions written to the file to be read in reconstruction
  //Float_t xi[44], yi[44];
  //Float_t xi[45], yi[45];
  Float_t xi[48], yi[48];
  Float_t xCur=-60., yCur=-60.;
  Int_t iMod=0, iModNoHole=0;
  
  ofstream fxypos(fGeoFile);
  //ofstream fxypos("psd_geo_xy.txt");
  
  for(Int_t iy=0; iy<7; iy++) 
  {
    for(Int_t ix=0; ix<7; ix++) 
    {
      iMod++;
      //if(iMod==1 || iMod==7 || iMod==43 || iMod==25 || iMod==49) 
      if(iMod==1 || iMod==7 || iMod==43 || iMod==49) //for module with hole
      {
      	gGeoManager->Node("VHLL", iMod, "VETO", xCur,yCur,0,  0,kTRUE, buf, 0); 
	cout <<"HOLE::::iMod,xxxx,yyyy " <<iMod <<" " <<xCur <<" " <<yCur <<endl;
      }
      else
      {
	iModNoHole++;
	if(iModNoHole==23) {//module with hole or hole 20x20cm
	  //gGeoManager->Node("VMHL", iModNoHole, "VETO", xCur,yCur,0, 0, kTRUE, buf, 0); 
	  gGeoManager->Node("VMHA", iModNoHole, "VETO", 5.,5.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = 5. + xPSD;
	  yi[iModNoHole-1] = 5.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	

	  iModNoHole++;
	  gGeoManager->Node("VMHB", iModNoHole, "VETO", -5.,5.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = -5. + xPSD;
	  yi[iModNoHole-1] = 5.;     
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	

	  iModNoHole++;
	  gGeoManager->Node("VMHD", iModNoHole, "VETO", 5.,-5.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = 5. + xPSD;
	  yi[iModNoHole-1] = -5.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	

	  iModNoHole++;
	  gGeoManager->Node("VMHC", iModNoHole, "VETO", -5.,-5.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = -5. + xPSD;
	  yi[iModNoHole-1] = -5.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	

	  //iModNoHole++;
	}
	else {
	  gGeoManager->Node("VMDL", iModNoHole, "VETO", xCur,yCur,0, 0, kTRUE, buf, 0); 
	}
	if(iModNoHole<23 || iModNoHole>26) {//for 4 small mods
	  cout <<"MODULE :::::iMod,xxxx,yyyy " <<iModNoHole <<" " <<xCur <<" " <<yCur <<endl;
	
	  xi[iModNoHole-1] = xCur + xPSD;
	  yi[iModNoHole-1] = yCur;  // change: put fxzpos << etc here
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	}	
      }

      fNbOfSensitiveVol++;	
      xCur=xCur + 20.;      
    }
    xCur=-60;
    yCur=yCur + 20.;
  }

  fNbOfSensitiveVol -= 1;//central module should not be counted  in case of 4 small modules in it	
  cout <<"fNbOfSensitiveVol " <<fNbOfSensitiveVol <<endl;
  fxypos.close();
  */
  /*
//60 mods
  Float_t xi[64], yi[64];
  Float_t xCur=-70., yCur=-70.;
  Int_t iMod=0, iModNoHole=0;
  
  ofstream fxypos(fGeoFile);
  //ofstream fxypos("psd_geo_xy.txt");
  
  for(Int_t iy=0; iy<8; iy++) 
  {
    for(Int_t ix=0; ix<8; ix++) 
    {
      iMod++;
      if(iMod==1 || iMod==8 || iMod==57 || iMod==64) 
      {
      	gGeoManager->Node("VHLL", iMod, "VETO", xCur,yCur,0,  0,kTRUE, buf, 0); 
	cout <<"HOLE::::iMod,xxxx,yyyy " <<iMod <<" " <<xCur <<" " <<yCur <<endl;
      }
      else
      {
	iModNoHole++;
	if(iModNoHole==26) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHC", iModNoHole, "VETO", -10.,-10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = -10. + xPSD;
	  yi[iModNoHole-1] = -10.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==26)

	else if(iModNoHole==27) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHD", iModNoHole, "VETO", 10.,-10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = 10. + xPSD;
	  yi[iModNoHole-1] = -10.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==27)

	else if(iModNoHole==34) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHB", iModNoHole, "VETO", -10.,10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = -10. + xPSD;
	  yi[iModNoHole-1] = 10.;     
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==34)

	else if(iModNoHole==35) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHA", iModNoHole, "VETO", 10.,10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = 10. + xPSD;
	  yi[iModNoHole-1] = 10.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==35)

	else {
	  //iModNoHole++;
	  gGeoManager->Node("VMDL", iModNoHole, "VETO", xCur,yCur,0, 0, kTRUE, buf, 0); 
	}

	if(iModNoHole!=26 && iModNoHole!=27 && iModNoHole!=34 && iModNoHole!=35) {//for 4 mods with holes
	  cout <<"MODULE :::::iMod,xxxx,yyyy " <<iModNoHole <<" " <<xCur <<" " <<yCur <<endl;
	
	  xi[iModNoHole-1] = xCur + xPSD;
	  yi[iModNoHole-1] = yCur;  // change: put fxzpos << etc here
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	}	
      }//else

      fNbOfSensitiveVol++;	
      xCur=xCur + 20.;      
      }//for(Int_t ix=0; ix<8; ix++)
    xCur=-70;
    yCur=yCur + 20.;
    }//for(Int_t iy=0; iy<8; iy++)

    //fNbOfSensitiveVol -= 1;//central module should not be counted  in case of 4 small modules in it	
  cout <<"fNbOfSensitiveVol " <<fNbOfSensitiveVol <<endl;
  fxypos.close();
*/

//44 mods  
  Float_t xi[64], yi[64];
  Float_t xCur=-70., yCur=-50.;
  Int_t iMod=0, iModNoHole=0;
  
  ofstream fxypos(fGeoFile);
  //ofstream fxypos("psd_geo_xy.txt");
  
  for(Int_t iy=0; iy<6; iy++) 
  {
    for(Int_t ix=0; ix<8; ix++) 
    {
      iMod++;
      if(iMod==1 || iMod==8 || iMod==41 || iMod==48) 
      {
      	gGeoManager->Node("VHLL", iMod, "VETO", xCur,yCur,0,  0,kTRUE, buf, 0); 
	cout <<"HOLE::::iMod,xxxx,yyyy " <<iMod <<" " <<xCur <<" " <<yCur <<endl;
      }
      else
      {
	iModNoHole++;
	if(iModNoHole==18) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHC", iModNoHole, "VETO", -10.,-10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = -10. + xPSD;
	  yi[iModNoHole-1] = -10.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==18)

	else if(iModNoHole==19) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHD", iModNoHole, "VETO", 10.,-10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = 10. + xPSD;
	  yi[iModNoHole-1] = -10.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==19)

	else if(iModNoHole==26) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHB", iModNoHole, "VETO", -10.,10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = -10. + xPSD;
	  yi[iModNoHole-1] = 10.;     
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==26)

	else if(iModNoHole==27) {//module with hole
	  //iModNoHole++;
	  gGeoManager->Node("VMHA", iModNoHole, "VETO", 10.,10.,0, 0, kTRUE, buf, 0); 
	  xi[iModNoHole-1] = 10. + xPSD;
	  yi[iModNoHole-1] = 10.;        
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout <<iModNoHole <<" " << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	  fNbOfSensitiveVol++;	
	}//if(iModNoHole==27)

	else {
	  //iModNoHole++;
	  gGeoManager->Node("VMDL", iModNoHole, "VETO", xCur,yCur,0, 0, kTRUE, buf, 0); 
	}

	if(iModNoHole!=18 && iModNoHole!=19 && iModNoHole!=26 && iModNoHole!=27) {//for 4 mods with holes
	  cout <<"MODULE :::::iMod,xxxx,yyyy " <<iModNoHole <<" " <<xCur <<" " <<yCur <<endl;
	
	  xi[iModNoHole-1] = xCur + xPSD;
	  yi[iModNoHole-1] = yCur;  // change: put fxzpos << etc here
	  fxypos << xi[iModNoHole-1]<< " " << yi[iModNoHole-1] << endl;
	  cout << xi[iModNoHole-1] << " " << yi[iModNoHole-1] << endl;
	}	
      }//else

      fNbOfSensitiveVol++;	
      xCur=xCur + 20.;      
      }//for(Int_t ix=0; ix<8; ix++)
    xCur=-70;
    yCur=yCur + 20.;
    }//for(Int_t iy=0; iy<8; iy++)

    //fNbOfSensitiveVol -= 1;//central module should not be counted  in case of 4 small modules in it	
  cout <<"fNbOfSensitiveVol " <<fNbOfSensitiveVol <<endl;
  fxypos.close();
  


}
// -------------------------------------------------------------------------

ClassImp(CbmPsdv1_44mods_hole6cm)
