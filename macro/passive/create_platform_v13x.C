/******************************************************************************
 ** Creation of beam pipe geometry in ROOT format (TGeo).
 **
 ** @file create_pipegeo_v13.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 11.10.2013
 **
 ** The beam pipe is composed of carbon with a thickness of 0.5 mm. It is
 ** placed directly into the cave as mother volume.
 ** The pipe consists of a number of parts. Each part has a PCON
 ** shape. The beam pipe inside the RICH is part of the RICH geometry;
 ** the beam pipe geometry thus excludes the z range of the RICH in case
 ** the latter is present in the setup.
  *****************************************************************************/




#include <iomanip>
#include <iostream>
#include "TGeoManager.h"


using namespace std;

// -------------   Other global variables   -----------------------------------
// ---> TGeoManager (too lazy to write out 'Manager' all the time
TGeoManager*   gGeoMan     = NULL;  // will be set later
// ----------------------------------------------------------------------------


// ============================================================================
// ======                         Main function                           =====
// ============================================================================

void create_platform_v13x()
{
    
  // -----   Define platform parts   ----------------------------------------------
  
//  /** For v13a (SIS 100)   **/
//  TString geoTag = "v13a";
//  Double_t sizeX = 725.0;  // symmetric in x
//  Double_t sizeY = 234.0;  // without rails
//  Double_t sizeZ = 297.0;  // short version
//  Double_t endZ  = 450.0;  // ends at z = 450.0

  /** For v13b (SIS 300)   **/
  TString geoTag = "v13b";
  Double_t sizeX = 725.0;  // symmetric in x
  Double_t sizeY = 234.0;  // without rails
  Double_t sizeZ = 455.0;  // long version
  Double_t endZ  = 540.0;  // ends at z = 450.0

  Double_t beamY = 570.0;  // nominal beam height
  Double_t posX =    0.0;
  Double_t posY = -beamY + sizeY/2.;  // rest on the floor at -beamY
  Double_t posZ =  endZ  - sizeZ/2.;  // end at MUCH z-end 
    
  // --------------------------------------------------------------------------

    

  // -------------  Load the necessary FairRoot libraries   -------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  // --------------------------------------------------------------------------


  // -------   Geometry file name (output)   ----------------------------------
  TString geoFileName = "platform_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------


  // -------   Open info file   -----------------------------------------------
  TString infoFileName = geoFileName;
  infoFileName.ReplaceAll("root", "info");
  fstream infoFile;
  infoFile.open(infoFileName.Data(), fstream::out);
  infoFile << "Platform geometry created with create_platform_v13.C"
           << std::endl << std::endl;
  // --------------------------------------------------------------------------


  // -------   Load media from media file   -----------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  gGeoMan = gGeoManager;
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  // ---> aluminium
  FairGeoMedium* mAluminium      = geoMedia->getMedium("aluminium");
  if ( ! mAluminium ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAluminium);
  TGeoMedium* aluminium = gGeoMan->GetMedium("aluminium");
  if ( ! aluminium ) Fatal("Main", "Medium aluminium not found");

//  // ---> air
//  FairGeoMedium* mAir  = geoMedia->getMedium("air");
//  if ( ! mAir ) Fatal("Main", "FairMedium air not found");
//  geoBuild->createMedium(mAir);
//  TGeoMedium* air = gGeoMan->GetMedium("air");
//  if ( ! air ) Fatal("Main", "Medium air not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("PLATFORMgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("top");
  gGeoMan->SetTopVolume(top);
  TString platformName = "platform_";
  platformName += geoTag;
  TGeoVolume* platform = new TGeoVolumeAssembly(platformName.Data());
  // --------------------------------------------------------------------------


  // -----   Create   ---------------------------------------------------------
  TGeoBBox* platform_base = new TGeoBBox("", sizeX /2., sizeY /2., sizeZ /2.);
  TGeoVolume* platform_vol = new TGeoVolume("platform", platform_base, aluminium);
  platform_vol->SetLineColor(kBlue);
  platform_vol->SetTransparency(70);

  TGeoTranslation* platform_trans = new TGeoTranslation("", posX, posY, posZ);
  platform->AddNode(platform_vol, 1, platform_trans);

 infoFile << "sizeX: "<< setprecision(2) << sizeX
          << "sizeY: "<< setprecision(2) << sizeY
          << "sizeZ: "<< setprecision(2) << sizeZ << endl;
 infoFile << "posX : "<< setprecision(2) << posX 
          << "posY : "<< setprecision(2) << posY 
          << "posZ : "<< setprecision(2) << posZ  << endl;

  // ---------------   Finish   -----------------------------------------------
  top->AddNode(platform, 1);
  cout << endl << endl;
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  cout << endl;
  cout << "Geometry " << top->GetName() << " written to " 
       << geoFileName << endl;

  top->Draw("ogl");

  geoFile->Close();
  infoFile.close();


}
// ============================================================================
// ======                   End of main function                          =====
// ============================================================================

