/******************************************************************************
 ** Creation of beam pipe geometry in ROOT format (TGeo).
 **
 ** @file create_pipegeo_v13.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 11.10.2013
 **
 ** 2014-07-05 - DE - v14x - SIS 100 adapt to new position of RICH v14a, z 170.0 cm to 370.0 cm
 ** 2014-07-05 - DE - v14y - SIS 300 adapt to new position of RICH v14a, z 170.0 cm to 370.0 cm
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

typedef pair<Double_t, Double_t> point;



// -------------   Steering variables       -----------------------------------
const Double_t gkThickness = 0.05;        // thickness of beam pipe [cm]
const char*    gkMaterial  = "carbon";    // material of beam pipe


// -------------   Other global variables   -----------------------------------
// ---> TGeoManager (too lazy to write out 'Manager' all the time
TGeoManager*   gGeoMan     = NULL;  // will be set later
// ----------------------------------------------------------------------------


// ============================================================================
// ======                         Main function                           =====
// ============================================================================

void create_pipegeo_v14(const char* geoTag)
{
    
  // -----   Define pipe parts   ----------------------------------------------
  
  /** For v13a (hadron setup): only one part
  Int_t nSects1 = 12;
  Double_t z1[] = { -5.0, 2.5,  3.5, 23.95, 24.0, 26.95, 27.0, 160.0, 350.0, 449.95, 450.0, 1000.0 };
  Double_t r1[] = {  2.5, 2.5, 13.0, 13.0,  12.8,  1.0,   1.0,   3.2,  25.0,  25.0,   13.9,   13.9 };
  Int_t nSects2 = 0;
  Double_t z2[]; 
  Double_t r2[];
   **/
    
    
  /** For v14x (SIS 100 electron setup): two parts, excluding RICH region
  **/
  Int_t nSects1 = 8;
  Double_t z1[] = { -5.0, 2.5,  3.5, 23.95, 24.0, 26.95, 27.0, 170.0};  // 160.0
  Double_t r1[] = {  2.5, 2.5, 13.0, 13.0,  12.8,  1.0,   1.0,   3.2};
  Int_t nSects2 = 4;
  Double_t z2[] = { 370.00, 599.95, 600.00,   720.00 };  // 350.00
  Double_t r2[] = {  25.00,  25.00,  13.90,    13.90 };


  /** For v14y (SIS 300 electron setup): two parts, excluding RICH region
  Int_t nSects1 = 8;
  Double_t z1[] = { -5.0, 2.5,  3.5, 23.95, 24.0, 26.95, 27.0, 170.0};  // 160.0
  Double_t r1[] = {  2.5, 2.5, 13.0, 13.0,  12.8,  1.0,   1.0,   3.2};
  Int_t nSects2 = 4;
  Double_t z2[] = { 370.00, 599.95, 600.00,  1000.00 };  // 350.00
  Double_t r2[] = {  25.00,  25.00,  13.90,    13.90 };
  **/

    
  /** For v13c (muon setup). only one part
  Int_t nSects1 = 11;
  Double_t z1[] = { -5.0, 2.5,  3.5, 26.0, 27.0, 150.0, 170.0, 550.0, 649.5, 650.0, 1140.0};
  Double_t r1[] = {  2.5, 2.5, 13.0, 13.0,  1.0,   3.0,   8.5,  25.0,  25.0,  13.9,   13.9};
  Int_t nSects2 = 0;
  Double_t z2[];
  Double_t r2[];
   **/

  // --------------------------------------------------------------------------

    

  // -------------  Load the necessary FairRoot libraries   -------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  // --------------------------------------------------------------------------


  // -------   Geometry file name (output)   ----------------------------------
  TString geoFileName = "pipe_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------


  // -------   Open info file   -----------------------------------------------
  TString infoFileName = geoFileName;
  infoFileName.ReplaceAll("root", "info");
  fstream infoFile;
  infoFile.open(infoFileName.Data(), fstream::out);
  infoFile << "Beam pipe geometry created with create_pipegeo_v13.C"
           << std::endl << std::endl;
  infoFile << "Material:  " << gkMaterial << endl;
  infoFile << "Thickness: " << gkThickness << "cm" << endl << endl;
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
  // ---> carbon
  FairGeoMedium* mCarbon      = geoMedia->getMedium("carbon");
  if ( ! mCarbon ) Fatal("Main", "FairMedium carbon not found");
  geoBuild->createMedium(mCarbon);
  TGeoMedium* carbon = gGeoMan->GetMedium("carbon");
  if ( ! carbon ) Fatal("Main", "Medium carbon not found");
  // ---> vacuum
  FairGeoMedium* mCarbon  = geoMedia->getMedium("vacuum");
  if ( ! mCarbon ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mCarbon);
  TGeoMedium* vacuum = gGeoMan->GetMedium("vacuum");
  if ( ! vacuum ) Fatal("Main", "Medium vacuum not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("PIPEgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  TString pipeName = "PIPE_";
  pipeName += geoTag;
  TGeoVolume* pipe = new TGeoVolumeAssembly(pipeName.Data());
  // --------------------------------------------------------------------------



  // -----   Create Part 1   --------------------------------------------------
  infoFile << "Part 1: " << nSects1 << " sections" << endl;
  infoFile << setw(2) << "i" << setw(10) << "z" << setw(10) << "rmin" << endl;
  TGeoVolume* pipe1    = MakePipe  (1, nSects1, z1, r1, carbon, &infoFile);
  TGeoVolume* pipevac1 = MakeVacuum(1, nSects1, z1, r1, vacuum, &infoFile);
  pipe->AddNode(pipe1, 0);
  pipe->AddNode(pipevac1, 0);
  // -----   End of part 1   --------------------------------------------------


  // -----   Create Part 2 (if required)   ------------------------------------
  if ( nSects2 ) {
    infoFile << "Part 2: " << nSects2 << " sections" << endl;
    infoFile << setw(2) << "i" << setw(10) << "z" << setw(10) << "rmin" << endl;

    TGeoVolume* pipe2    = MakePipe  (2, nSects2, z2, r2, carbon, &infoFile);
    TGeoVolume* pipevac2 = MakeVacuum(2, nSects2, z2, r2, vacuum, &infoFile);
    pipe->AddNode(pipe2, 0);
    pipe->AddNode(pipevac2, 0);
  }
  // -----   End of part 2   --------------------------------------------------



  // ---------------   Finish   -----------------------------------------------
  top->AddNode(pipe, 1);
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
  geoFile->Close();
  infoFile.close();


}
// ============================================================================
// ======                   End of main function                          =====
// ============================================================================



// =====  Make the beam pipe volume   =========================================
TGeoVolume* MakePipe(Int_t iPart, Int_t nSects, Double_t* z, Double_t* r,
                     TGeoMedium* medium, fstream* infoFile) {

  // ---> Shape
  TGeoPcon* shape = new TGeoPcon(0., 360., nSects);
  for (Int_t iSect = 0; iSect < nSects; iSect++) {
    shape->DefineSection(iSect, z[iSect], r[iSect], r[iSect]+gkThickness);
    *infoFile << setw(2)  << iSect
              << setw(10) << fixed << setprecision(2) << z[iSect]
              << setw(10) << fixed << setprecision(2) << r[iSect] << endl;
  }

  // ---> Volume
  TString volName = Form("pipe%i", iPart);
  TGeoVolume* pipe = new TGeoVolume(volName.Data(), shape, medium);

  return pipe;

}
// ============================================================================



// =====   Make the volume for the vacuum inside the beam pipe   ==============
TGeoVolume* MakeVacuum(Int_t iPart, Int_t nSects, Double_t* z, Double_t* r,
                      TGeoMedium* medium, fstream* infoFile) {

  // ---> Shape
  TGeoPcon* shape = new TGeoPcon(0., 360., nSects);
  for (Int_t iSect = 0; iSect < nSects; iSect++) {
    shape->DefineSection(iSect, z[iSect], 0., r[iSect]);
  }

  // ---> Volume
  TString volName = Form("pipevac%i", iPart);
  TGeoVolume* vacuum = new TGeoVolume(volName.Data(), shape, medium);

  return vacuum;

}
// ============================================================================





   
