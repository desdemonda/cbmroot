
// -------------------------------------------------------------------------
// -----                       CbmPsdWoutv1 source file                   -----
// -----                  Created 04/02/05  by Alla Maevskaya              -----
// -------------------------------------------------------------------------

#include "CbmPsdWoutv1_for48mods_sub2.h"

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


// -----   Default constructor   -------------------------------------------
CbmPsdWoutv1_for48mods_sub2::CbmPsdWoutv1_for48mods_sub2() {
  fPsdCollection = new TClonesArray("CbmPsdPoint");
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmPsdWoutv1_for48mods_sub2::CbmPsdWoutv1_for48mods_sub2(const char* name, Bool_t active)
  : CbmPsd(name, active) {
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdWoutv1_for48mods_sub2::~CbmPsdWoutv1_for48mods_sub2() {
 if (fPsdCollection) {
   fPsdCollection->Delete(); 
   delete fPsdCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmPsdWoutv1_for48mods_sub2::ProcessHits(FairVolume* vol)
{
  //  cout<<" CbmPsdWoutv1_for48mods_sub2::ProcessHits in "<<vol->GetName()<<endl;
  //if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  TVector3 position;
  Int_t pdg;
    Int_t copyNo,copyNoWOUT,copyNoVHLE;
    Int_t iCell, iNumm ;
  // Set parameters at entrance of volume. Reset ELoss.
  //if(gMC->VolId("VHSS") == gMC->CurrentVolID(copyNoVHLE)) return kFALSE;
  //if(gMC->VolId("WOUT") == gMC->CurrentVolID(copyNoWOUT)) {
  //if(gMC->VolId("WOUT") != gMC->CurrentVolID(copyNoWOUT)) {
    //cout <<"MARINA " <<endl;

  if ( gMC->IsTrackEntering() ) {
    ResetParameters();
    fEloss   = 0.;
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fTime    = gMC->TrackTime() * 1.0e09;
    fLength  = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    /*
    //Int_t copyNo;
    gMC->CurrentVolID(copyNo);
    //fVolumeID=copyNo;
    //Int_t iCell, iNumm ;
    gMC->CurrentVolOffID(1, iCell);
    fModuleID=iCell;
    gMC->CurrentVolOffID(2, iNumm);
    fVolumeID=iNumm;
    */
    fVolumeID=1;
    fModuleID=1;

    pdg = gMC->TrackPid();

      CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				    TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
      fPoint->SetModuleID(fModuleID);
      fPoint->SetTrackID(fTrackID);
    }

  /* 
  if (gMC->IsTrackInside() ) {
    // Sum energy loss for all steps in the active volume
    fEloss += gMC->Edep();
    if( gMC->IsTrackStop() ||
	gMC->IsTrackDisappeared()   ) {
      fEloss += gMC->Edep();
      
     if (pdg == 1000791970)
     // if (strstr(fDebug,"hit"))
       Info("ProcessHits track inside","track %d pdg %d volume %d %s, x=(%.1f,%.1f,%.1f), p=(%.2e,%.2e,%.2e), time %f fLength %f dE=%.3e",
	     fTrackID, pdg, fVolumeID, gMC->CurrentVolName(),
	     fPos.X(),fPos.Y(),fPos.Z(),fMom.Px(),fMom.Py(),fMom.Pz(),fTime, fLength, fEloss);
      
     // Create CbmPsdPoin

      CbmPsdPoint *fPoint =  AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
				    TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),fTime, fLength,fEloss);
      fPoint->SetModuleID(fModuleID);
      fPoint->SetTrackID(fTrackID);

      //fPoint->Position(position); //marina to print
      //cout <<"MARINA points fModuleID,fTrackID " <<fModuleID <<" " <<fTrackID <<" " <<fPos.X() <<" " <<fPos.Y() <<" " <<fPos.Z() <<" " <<position.X() <<" " <<position.Y() <<" " <<position.Z() <<endl;
    }
  }
  */
  /*
  if ( gMC->IsTrackExiting()  ) {
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
  */  

   ((CbmStack*)gMC->GetStack())->AddPoint(kPSD, fTrackID);
 
  //  ResetParameters();
    
  return kTRUE;
  //}//if(gMC->VolId("WOUT") == gMC->CurrentVolID(copyNoWOUT))
  //else return kFALSE;

}
// -------------------------------------------------------------------------



// -----   Public method CreateMaterial   ----------------------------------
void CbmPsdWoutv1_for48mods_sub2::CreateMaterial() {
}
// -----   Public method ConstructGeometry   -------------------------------
void CbmPsdWoutv1_for48mods_sub2::ConstructGeometry() {
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
  //TGeoVolume *WOUT = gGeoManager->MakeBox("WOUT",med11, 70, 70, 0.2);
  TGeoVolume *VWOT = gGeoManager->MakeBox("VWOT",med2, 40, 40, 0.05);
  TGeoVolume *WOUT = gGeoManager->MakeBox("WOUT",med2, 40, 40, 0.05);
  TGeoVolume *VHLL = gGeoManager->MakeBox("VHLL",med1,10,10,62.4);//hole
  //TGeoVolume *VHLE = gGeoManager->MakeBox("VHLE",med1,10,10,0.2);//hole
  //TGeoVolume *VHLT = gGeoManager->MakeTube("VHLT",med1,0,3,0.2);//hole
  TGeoVolume *VHLE = gGeoManager->MakeBox("VHLE",med2,10,10,0.05);//hole
  TGeoVolume *VHSS = gGeoManager->MakeBox("VHSS",med2,20,20,0.05);//hole
  TGeoVolume *VHLT = gGeoManager->MakeTube("VHLT",med2,0,3,0.05);//hole
  
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
 
  
  Float_t xPSD = fXshift;
  Float_t zPSD = fZposition;
  Float_t zdcSize[3]   = {  70.,   70.,  62.4 }; // SELIM: fZposition = 1 scintillator, not PSD center
  //zPSD += zdcSize[2];

  //gGeoManager->Node("VETO", 1, "cave", xPSD, 0, zPSD, 0, kTRUE, buf, 0);
  //gGeoManager->Node("VETO", 1, "cave", xPSD, 0, zPSD+62.4, 0, kTRUE, buf, 0);//marina
  //gGeoManager->Node("WOUT", 1, "cave", xPSD, 0, zPSD+2.44, 0, kTRUE, buf, 0);//marina
  //gGeoManager->Node("WOUT", 1, "cave", xPSD, 0, zPSD+0.05, 0, kTRUE, buf, 0);//marina

  gGeoManager->Node("VWOT", 1, "cave", xPSD, 0, zPSD+0.05, 0, kTRUE, buf, 0);//marina
  gGeoManager->Node("WOUT", 1, "VWOT", 0, 0, 0, 0, kTRUE, buf, 0);//marina

  AddSensitiveVolume(WOUT);

//Holes in WOUT
  gGeoManager->Node("VHSS", 1, "WOUT", 0., 0., 0., 0,kTRUE, buf, 0); 

  /*
  //gGeoManager->Node("VHLE", 1, "WOUT", 0., 0., 0., 0,kTRUE, buf, 0);  gGeoManager->Node("VHLT", 1, "WOUT", 0., 0., 0., 0,kTRUE, buf, 0); //comment for w/o hole
  gGeoManager->Node("VHLE", 1, "WOUT", 70., 50., 0., 0,kTRUE, buf, 0); 
  gGeoManager->Node("VHLE", 2, "WOUT", -70., 50., 0., 0,kTRUE, buf, 0); 
  gGeoManager->Node("VHLE", 3, "WOUT", 70., -50., 0., 0,kTRUE, buf, 0); 
  gGeoManager->Node("VHLE", 4, "WOUT", -70., -50., 0., 0,kTRUE, buf, 0); 
  */
  /*
  //  gGeoManager->Node("VMDL", 1, "VETO", 60, 60,0., 0, kTRUE, buf, 0);
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
  */

  for (Int_t ivol=2; ivol<61; ivol++)
    {
      Float_t zvtyl = -60.18 + 2.04*(ivol-1); 
      //gGeoManager->Node("VTYL",ivol, "VMDL", 0, 0, zvtyl , 0,kTRUE, buf, 0);
      Float_t zvpbl = -59.16 + 2.04*(ivol-1); 
      //gGeoManager->Node("VPBL",ivol+1, "VMDL", 0, 0, zvpbl , 0, kTRUE, buf, 0); 
      //fNbOfSensitiveVol++; //marina -> comment for WOUT
    }

  //XY positions written to the file to be read in reconstruction
  Float_t xi[45] = {0};
  Float_t yi[45] = {0};

  /*
  Float_t xCur=-60., yCur=-60.;
  Int_t iMod=0, iModNoHole=0;
  for(Int_t iy=0; iy<7; iy++) {
    for(Int_t ix=0; ix<7; ix++) {
      iMod++;
      if(iMod==1 || iMod==7 || iMod==43 || iMod==25 || iMod==49) {
      	//gGeoManager->Node("VHLL", iMod, "VETO", xCur,yCur,0,  0,kTRUE, buf, 0); 
	cout <<"HOLE::::iMod,xxxx,yyyy " <<iMod <<" " <<xCur <<" " <<yCur <<endl;
      }
      else
	{
	  iModNoHole++;
	  //gGeoManager->Node("VMDL", iModNoHole, "VETO", xCur,yCur,0, 0, kTRUE, buf, 0); 
	cout <<"MODULE :::::iMod,xxxx,yyyy " <<iModNoHole <<" " <<xCur <<" " <<yCur <<endl;
	xi[iModNoHole-1] = xCur + xPSD;
	yi[iModNoHole-1] = yCur;
	}


      //fNbOfSensitiveVol++;//marina -> comment for WOUT
	// }
      // if(ix==6) xCur=80.;
      xCur=xCur + 20.;      
    }//for(Int_t ix==0; ix<9; ix++)
    xCur=-60;
    yCur=yCur + 20.;
  }//for(Int_t iy==0; iy<9; iy++)
  */

  fNbOfSensitiveVol = 1;//marina -> for WOUT
  ofstream fxypos("psd_geo_xy.txt");
  for (Int_t ii=0; ii<45; ii++) {
    fxypos<<xi[ii]<<" "<<yi[ii]<<endl;
    cout<<xi[ii]<<" "<<yi[ii]<<endl;
  }
  fxypos.close();
  

}
// -------------------------------------------------------------------------

ClassImp(CbmPsdWoutv1_for48mods_sub2)
