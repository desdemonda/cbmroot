/******************************************************************************
 ** Creation of STS geometry in ROOT format (TGeo).
 **
 ** @file create_stsgeo_v12.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 15 June 2012
 ** @date 20.06.2012
 **
 ** The geometry hierarachy is:
 **
 ** 1. Sensors  (see function CreateSensors)
 **    The sensors are the active volumes and the lowest geometry level.
 **    They are built as TGeoVolumes, shape box, material silicon.
 **    x size is determined by strip pitch 58 mu and 1024 strips 
 **    plus guard ring of 1.3 mm at each border -> 6.1992 cm.
 **    Sensor type 1 is half of that (3.0792 cm).
 **    y size is determined by strip length (2.2 / 4.2 / 6.3 cm) plus
 **    guard ring of 1.3 mm at top and bottom -> 2.46 / 4.46 / 6.46 cm.
 **    z size is a parameter, to be set by gkSensorThickness.
 **
 ** 2. Sectors  (see function CreateSectors)
 **    Sectors consist of several chained sensors. These are arranged
 **    vertically on top of each other with a gap to be set by
 **    gkChainGapY. Sectors are constructed as TGeoVolumeAssembly.
 **    The sectors are auxiliary volumes used for proper placement
 **    of the sensor(s) in the module. They do not show up in the
 **    final geometry.
 **
 ** 3. Modules (see function ConstructModule)
 **    A module is a readout unit, consisting of one sensor or
 **    a chain of sensors (see sector) and a cable.
 **    The cable extends from the top of the sector vertically to the
 **    top of the halfladder the module is placed in. The cable and module
 **    volume thus depend on the vertical position of the sector in 
 **    the halfladder. The cables consist of silicon with a thickness to be
 **    set by gkCableThickness.
 **    Modules are constructed as TGeoVolume, shape box, medium gStsMedium.
 **    The module construction can be switched off (gkConstructCables)
 **    to reproduce older geometries.
 **
 ** 4. Halfladders (see function ConstructHalfLadder)
 **    A halfladder is a vertical assembly of several modules. The modules
 **    are placed vertically such that their sectors overlap by 
 **    gkSectorOverlapY. They are displaced in z direction to allow for the 
 **    overlap in y by gkSectorGapZ.
 **    The horizontal placement of modules in the halfladder can be choosen
 **    to left aligned or right aligned, which only matters if sensors of
 **    different x size are involved.
 **    Halfladders are constructed as TGeoVolumeAssembly.
 **
 ** 5. Ladders (see function CreateLadders and ConstructLadder)
 **    A ladder is a vertical assembly of two halfladders, and is such the
 **    vertical building block of a station. The second (bottom) half ladder
 **    is rotated upside down. The vertical arrangement is such that the
 **    inner sectors of the two halfladders have the overlap gkSectorOverlapY
 **    (function CreateLadder) or that there is a vertical gap for the beam
 **    hole (function CreateLadderWithGap).
 **    Ladders are constructed as TGeoVolumeAssembly.
 **   
 ** 6. Stations (see function ConstructStation)
 **    A station represents one layer of the STS geometry: one measurement
 **    at (approximately) a given z position. It consist of several ladders
 **    arranged horizontally to cover the acceptance.
 **    The ladders are arranged such that there is a horizontal overlap
 **    between neighbouring ladders (gkLadderOverLapX) and a vertical gap
 **    to allow for this overlap (gkLadderGapZ). Each second ladder is
 **    rotated around its y axis to face away from or into the beam.
 **    Stations are constructed as TGeoVolumes, shape box minus tube (for
 **    the beam hole), material gStsMedium.
 **
 ** 7. STS
 **    The STS is a volume hosting the entire detectors system. It consists
 **    of several stations located at different z positions.
 **    The STS is constructed as TGeoVolume, shape box minus cone (for the
 **    beam pipe), material gStsMedium. The size of the box is computed to
 **    enclose all stations.
 *****************************************************************************/


// Remark: With the proper steering variables, this should exactly reproduce
// the geometry version v11b of A. Kotynia's described in the ASCII format.
// The only exception is a minimal difference in the z position of the 
// sectors/sensors. This is because of ladder types 2 and 4 containing the half
// sensors around the beam hole (stations 1,2 and 3). In v11b, the two ladders
// covering the beam hole cannot be transformed into each other by rotations,
// but only by a reflection. This means they are constructionally different.
// To avoid introducing another two ladder types, the difference in z position
// was accepted.


#include <iomanip>
#include <iostream>
#include "TGeoManager.h"


// -------------   Steering variables       -----------------------------------

// ---> Thickness of sensors [cm]
const Double_t gkSensorThickness = 0.03;

// ---> Vertical gap between chained sensors [cm]
const Double_t gkChainGapY       = 0.10;

// ---> Thickness of cables [cm]
const Double_t gkCableThickness  = 0.01;

// ---> Vertical overlap of neighbouring sectors in a ladder [cm]
const Double_t gkSectorOverlapY  = 0.30;

// ---> Gap in z between neighbouring sectors in a ladder [cm]
const Double_t gkSectorGapZ      = 0.02;

// ---> Horizontal overlap of neighbouring ladders [cm]
const Double_t gkLadderOverlapX  = 0.30;

// ---> Gap in z between neighbouring ladders [cm]
const Double_t gkLadderGapZ      = 0.00;

// ---> Switch to construct / not to construct readout cables
const Bool_t   gkConstructCables = kFALSE;
// ----------------------------------------------------------------------------



// --------------   Parameters of beam pipe in the STS region    --------------
// ---> Needed to compute stations and STS such as to avoid overlaps
const Double_t gkPipeZ1 =  27.0;
const Double_t gkPipeR1 =   1.05;
const Double_t gkPipeZ2 = 160.0;
const Double_t gkPipeR2 =   3.25;
// ----------------------------------------------------------------------------



// -------------   Other global variables   -----------------------------------
// ---> STS medium (for every volume except silicon)
TGeoMedium*    gStsMedium        = NULL;  // will be set later
// ---> TGeoManager (too lazy to write out 'Manager' all the time
TGeoManager*   gGeoMan           = NULL;  // will be set later
// ----------------------------------------------------------------------------




// ============================================================================
// ======                         Main function                           =====
// ============================================================================

void create_stsgeo_v12(const char* geoTag)
{

  // -------------  Load the necessary FairRoot libraries   -------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  // --------------------------------------------------------------------------


  // -------   Geometry file name (output)   ----------------------------------
  TString geoFileName = "sts_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------


  // -------   Open info file   -----------------------------------------------
  TString infoFileName = geoFileName;
  infoFileName.ReplaceAll("root", "info");
  fstream infoFile;
  infoFile.open(infoFileName.Data(), fstream::out);
  infoFile << "STS geometry created with create_stsgeo_v12.C" << endl << endl;
  infoFile << "Global variables: " << endl;
  infoFile << "Sensor thickness = " << gkSensorThickness << " cm" << endl;
  infoFile << "Vertical gap in sensor chain = " 
	   << gkChainGapY << " cm" << endl;
  infoFile << "Vertical overlap of sensors = " 
	   << gkSectorOverlapY << " cm" << endl;
  infoFile << "Gap in z between neighbour sensors = " 
	   << gkSectorGapZ << " cm" << endl;
  infoFile << "Horizontal overlap of sensors = " 
	   << gkLadderOverlapX << " cm" << endl;
  infoFile << "Gap in z between neighbour ladders = " 
	   << gkLadderGapZ << " cm" << endl;
  if ( gkConstructCables ) 
    infoFile << "Cable thickness = " << gkCableThickness << " cm" << endl;
  else
    infoFile << "No cables" << endl;
  infoFile << endl;
  infoFile << "Beam pipe: R1 = " << gkPipeR1 << " cm at z = " 
	   << gkPipeZ1 << " cm" << endl;
  infoFile << "Beam pipe: R2 = " << gkPipeR2 << " cm at z = " 
	   << gkPipeZ2 << " cm" << endl;
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
  // ---> air
  FairGeoMedium* mAir      = geoMedia->getMedium("air");
  if ( ! mAir ) Fatal("Main", "FairMedium air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* air = gGeoMan->GetMedium("air");
  if ( ! air ) Fatal("Main", "Medium air not found");
  // ---> silicon
  FairGeoMedium* mSilicon  = geoMedia->getMedium("silicon");
  if ( ! mSilicon ) Fatal("Main", "FairMedium silicon not found");
  geoBuild->createMedium(mSilicon);
  TGeoMedium* silicon = gGeoMan->GetMedium("silicon");
  if ( ! silicon ) Fatal("Main", "Medium silicon not found");
  gStsMedium = air;
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("STSgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------




  // --------------   Create media   ------------------------------------------
  /*
  cout << endl;
  cout << "===> Creating media....";
  cout << CreateMedia();
  cout << " media created" << endl;
  TList* media = gGeoMan->GetListOfMedia();
  for (Int_t iMedium = 0; iMedium < media->GetSize(); iMedium++ ) {
    cout << "Medium " << iMedium << ": " 
	 << ((TGeoMedium*) media->At(iMedium))->GetName() << endl;
  }
  gStsMedium = gGeoMan->GetMedium("air");
  if ( ! gStsMedium ) Fatal("Main", "medium sts_air not found");
  */
  // --------------------------------------------------------------------------

  


  // ---------------   Create sensors   ---------------------------------------
  cout << endl << endl;
  cout << "===> Creating sensors...." << endl << endl;
  infoFile << endl << "Sensors: " << endl;
  Int_t nSensors = CreateSensors();
  for (Int_t iSensor = 1; iSensor <= nSensors; iSensor++) {
    TString name = Form("Sensor%02d",iSensor);
    TGeoVolume* sensor = gGeoMan->GetVolume(name);
    if (iSensor == 1) 
      sensor->SetLineColor(kYellow);
    if (iSensor == 2) 
      sensor->SetLineColor(kRed);
    if (iSensor == 3) 
      sensor->SetLineColor(kGreen);
    if (iSensor == 4) 
      sensor->SetLineColor(kBlue);
    CheckVolume(sensor);
    CheckVolume(sensor, infoFile);
  }
  // --------------------------------------------------------------------------




  // ----------------   Create sectors   --------------------------------------
  cout << endl << endl;
  cout << "===> Creating sectors...." << endl;
  infoFile << endl << "Sectors: " << endl;
  Int_t nSectors = CreateSectors();
  for (Int_t iSector = 1; iSector <= nSectors; iSector++) {
    cout << endl;
    TString name = Form("Sector%02d", iSector);
    TGeoVolume* sector = gGeoMan->GetVolume(name);
    CheckVolume(sector);
    CheckVolume(sector, infoFile);
  }
  // --------------------------------------------------------------------------




  // ----------------   Create ladders   --------------------------------------
  cout << endl << endl;
  cout << "===> Creating ladders...." << endl;
  infoFile << endl << "Ladders:" << endl;
  Int_t nLadders = CreateLadders();
  for (Int_t iLadder = 1; iLadder <= nLadders; iLadder++) {
    cout << endl;
    TString name = Form("Ladder%02d", iLadder);
    TGeoVolume* ladder = gGeoMan->GetVolume(name);
    CheckVolume(ladder);
    CheckVolume(ladder, infoFile);
    CheckVolume(ladder->GetNode(0)->GetVolume(), infoFile);
  }
  // --------------------------------------------------------------------------




  // ----------------   Create stations   -------------------------------------
  Int_t statPos[8] = {30., 40., 50., 60., 70., 80., 90., 100.};
  cout << endl << endl;
  cout << "===> Creating stations...." << endl;
  infoFile << endl << "Stations: ";
  Int_t nLadders = 0;
  Int_t ladderTypes[20];
  Double_t statZ = 0.;
  Double_t rHole = 0.;
  TGeoBBox*        statShape = NULL;
  TGeoTranslation* statTrans = NULL;


  // --- Station 01: 8 ladders, type 1 1 1 2 2 1 1 1
  cout << endl;
  statZ = 30.;
  rHole = 1.30;
  nLadders = 8;
  ladderTypes[0] = 1;
  ladderTypes[1] = 1;
  ladderTypes[2] = 1;
  ladderTypes[3] = 2;
  ladderTypes[4] = 2;
  ladderTypes[5] = 1;
  ladderTypes[6] = 1;
  ladderTypes[7] = 1;
  TGeoVolume* station01 = ConstructStation("Station01", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station01);
  CheckVolume(station01, infoFile);
  infoFile << "Position z = " << statPos[0] << endl;
  
  // --- Station 02: 12 ladders, type 1 1 3 3 3 4 4 3 3 3 1 1
  cout << endl;
  statZ = 40.;
  rHole = 1.50;
  nLadders = 12;
  ladderTypes[0]  = 1;
  ladderTypes[1]  = 1;
  ladderTypes[2]  = 3;
  ladderTypes[3]  = 3;
  ladderTypes[4]  = 3;
  ladderTypes[5]  = 4;
  ladderTypes[6]  = 4;
  ladderTypes[7]  = 3;
  ladderTypes[8]  = 3;
  ladderTypes[9]  = 3;
  ladderTypes[10] = 1;
  ladderTypes[11] = 1;
  TGeoVolume* station02 = ConstructStation("Station02", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station02);
  CheckVolume(station02, infoFile);
  infoFile << "Position z = " << statPos[1] << endl;

  // --- Station 03: 12 ladders, type 1 3 3 3 3 4 4 3 3 3 3 1
  cout << endl;
  statZ = 50.;
  rHole = 1.70;
  nLadders = 12;
  ladderTypes[0]  = 1;
  ladderTypes[1]  = 3;
  ladderTypes[2]  = 3;
  ladderTypes[3]  = 3;
  ladderTypes[4]  = 3;
  ladderTypes[5]  = 4;
  ladderTypes[6]  = 4;
  ladderTypes[7]  = 3;
  ladderTypes[8]  = 3;
  ladderTypes[9]  = 3;
  ladderTypes[10] = 3;
  ladderTypes[11] = 1;
  TGeoVolume* station03 = ConstructStation("Station03", nLadders, ladderTypes, rHole);
  CheckVolume(station03);
  CheckVolume(station03, infoFile);
  infoFile << "Position z = " << statPos[2] << endl;

  // --- Station 04: 14 ladders, type 1 3 3 5 5 5 6 6 5 5 5 3 3 1
  cout << endl;
  statZ = 60.;
  rHole = 1.90;
  nLadders = 14;
  ladderTypes[0]  = 1;
  ladderTypes[1]  = 3;
  ladderTypes[2]  = 3;
  ladderTypes[3]  = 5;
  ladderTypes[4]  = 5;
  ladderTypes[5]  = 5;
  ladderTypes[6]  = 6;
  ladderTypes[7]  = 6;
  ladderTypes[8]  = 5;
  ladderTypes[9]  = 5;
  ladderTypes[10] = 5;
  ladderTypes[11] = 3;
  ladderTypes[12] = 3;
  ladderTypes[13] = 1;
  TGeoVolume* station04 = ConstructStation("Station04", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station04);
  CheckVolume(station04, infoFile);
  infoFile << "Position z = " << statPos[3] << endl;

  // --- Station 05: 14 ladders, type 1 3 5 5 5 5 6 6 5 5 5 5 3 1
  cout << endl;
  statZ = 70.;
  rHole = 2.1;
  nLadders = 14;
  ladderTypes[0]  = 1;
  ladderTypes[1]  = 3;
  ladderTypes[2]  = 5;
  ladderTypes[3]  = 5;
  ladderTypes[4]  = 5;
  ladderTypes[5]  = 5;
  ladderTypes[6]  = 6;
  ladderTypes[7]  = 6;
  ladderTypes[8]  = 5;
  ladderTypes[9]  = 5;
  ladderTypes[10] = 5;
  ladderTypes[11] = 5;
  ladderTypes[12] = 3;
  ladderTypes[13] = 1;
  TGeoVolume* station05 = ConstructStation("Station05", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station05);
  CheckVolume(station05, infoFile);
  infoFile << "Position z = " << statPos[4] << endl;

  // --- Station 06: 14 ladders, type 3 5 5 7 7 7 8 8 7 7 7 5 5 3
  cout << endl;
  statZ = 80.;
  rHole = 2.3;
  nLadders = 14;
  ladderTypes[0]  = 3;
  ladderTypes[1]  = 5;
  ladderTypes[2]  = 5;
  ladderTypes[3]  = 7;
  ladderTypes[4]  = 7;
  ladderTypes[5]  = 7;
  ladderTypes[6]  = 8;
  ladderTypes[7]  = 8;
  ladderTypes[8]  = 7;
  ladderTypes[9]  = 7;
  ladderTypes[10] = 7;
  ladderTypes[11] = 5;
  ladderTypes[12] = 5;
  ladderTypes[13] = 3;
  TGeoVolume* station06 = ConstructStation("Station06", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station06);
  CheckVolume(station06, infoFile);
  infoFile << "Position z = " << statPos[5] << endl;

  // --- Station 07: 16 ladders, type 3 3 5 7 7 7 7 8 8 7 7 7 7 5 3 3
  cout << endl;
  statZ = 90.;
  rHole = 2.5;
  nLadders = 16;
  ladderTypes[0]  = 3;
  ladderTypes[1]  = 3;
  ladderTypes[2]  = 5;
  ladderTypes[3]  = 7;
  ladderTypes[4]  = 7;
  ladderTypes[5]  = 7;
  ladderTypes[6]  = 7;
  ladderTypes[7]  = 8;
  ladderTypes[8]  = 8;
  ladderTypes[9]  = 7;
  ladderTypes[10] = 7;
  ladderTypes[11] = 7;
  ladderTypes[12] = 7;
  ladderTypes[13] = 5;
  ladderTypes[14] = 3;
  ladderTypes[15] = 3;
  TGeoVolume* station07 = ConstructStation("Station07", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station07);
  CheckVolume(station07, infoFile);
  infoFile << "Position z = " << statPos[6] << endl;

  // --- Station 08: 16 ladders, type 3 5 7 7 7 7 7 8 8 7 7 7 7 7 5 3
  cout << endl;
  statZ = 100.;
  rHole = 2.7;
  nLadders = 16;
  ladderTypes[0]  = 3;
  ladderTypes[1]  = 5;
  ladderTypes[2]  = 7;
  ladderTypes[3]  = 7;
  ladderTypes[4]  = 7;
  ladderTypes[5]  = 7;
  ladderTypes[6]  = 7;
  ladderTypes[7]  = 8;
  ladderTypes[8]  = 8;
  ladderTypes[9]  = 7;
  ladderTypes[10] = 7;
  ladderTypes[11] = 7;
  ladderTypes[12] = 7;
  ladderTypes[13] = 7;
  ladderTypes[14] = 5;
  ladderTypes[15] = 3;
  TGeoVolume* station08 = ConstructStation("Station08", nLadders, ladderTypes, 
					rHole);
  CheckVolume(station08);
  CheckVolume(station08, infoFile);
  infoFile << "Position z = " << statPos[7] << endl;
  // --------------------------------------------------------------------------




  // ---------------   Create STS volume   ------------------------------------
  cout << endl << endl;
  cout << "===> Creating STS...." << endl;

  // --- Determine size of STS box
  Double_t stsX = 0.;
  Double_t stsY = 0.;
  Double_t stsZ = 0.;
  Double_t stsBorder = 5.;
  for (Int_t iStation = 1; iStation<8; iStation++) {
    TString statName = Form("Station%02d", iStation);
    TGeoVolume* station = gGeoMan->GetVolume(statName);
    TGeoBBox* shape = (TGeoBBox*) station->GetShape();
    stsX = TMath::Max(stsX, 2.* shape->GetDX() );
    stsY = TMath::Max(stsY, 2.* shape->GetDY() );
  }
  // --- Some border around the stations
  stsX += stsBorder;  
  stsY += stsBorder; 
  stsZ = ( statPos[7] - statPos[0] ) + stsBorder;
  Double_t stsPosZ = 0.5 * ( statPos[7] + statPos[0] );

  // --- Create box  around the stations
  new TGeoBBox("stsBox", stsX/2., stsY/2., stsZ/2.);

  // --- Create cone hosting the beam pipe
  Double_t z1 = statPos[0] - 0.5 * stsBorder;  // start of STS box
  Double_t z2 = statPos[7] + 0.5 * stsBorder;  // end of STS box
  Double_t slope = (gkPipeR2 - gkPipeR1) / (gkPipeZ2 - gkPipeZ1);
  Double_t r1 = gkPipeR1 + slope * (z1 - gkPipeZ1); // at start of STS
  Double_t r2 = gkPipeR1 + slope * (z2 - gkPipeZ1); // at end of STS
  r1 += 0.1;    // safety margin
  r2 += 0.1;    // safety margin
  new TGeoCone("stsCone", stsZ/2., 0., r1, 0., r2);

  // --- Create STS volume
  TString stsName = "STS_";  
  stsName += geoTag;
  TGeoShape* stsShape = new TGeoCompositeShape("stsShape", 
					       "stsBox-stsCone");
  TGeoVolume* sts = new TGeoVolume(stsName.Data(), stsShape, gStsMedium);

  // --- Place stations in the STS
  for (Int_t iStation = 1; iStation <=8; iStation++) {
    TString statName = Form("Station%02d", iStation);
    TGeoVolume* station = gGeoMan->GetVolume(statName);
    Double_t posZ = statPos[iStation-1] - stsPosZ;
    TGeoTranslation* trans = new TGeoTranslation(0., 0., posZ);
    sts->AddNode(station, iStation, trans);
  }
  cout << endl;
  CheckVolume(sts);
  // --------------------------------------------------------------------------




  // ---------------   Finish   -----------------------------------------------
  TGeoTranslation* stsTrans = new TGeoTranslation(0., 0., stsPosZ);
  top->AddNode(sts, 1, stsTrans);
  cout << endl << endl;
  CheckVolume(top);
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

  TString geoFileName_ = "sts_";
  geoFileName_ = geoFileName_ + geoTag + "_geo.root";
  //DEDE
  TFile* geoFile = new TFile(geoFileName_, "RECREATE");
  gGeoMan->Write();  // use this is you want GeoManager format in the output
  geoFile->Close();

  infoFile.close();

}
// ============================================================================
// ======                   End of main function                          =====
// ============================================================================





// ****************************************************************************
// *****      Definition of media, sensors, sectors and ladders           *****
// *****                                                                  *****
// *****     Decoupled from main function for better readability          *****
// ****************************************************************************


/** ===========================================================================
 ** Create media
 **
 ** Currently created: air, active silicon, passive silion
 **
 ** Not used for the time being
 **/
Int_t CreateMedia() {

  Int_t nMedia     = 0;
  Double_t density = 0.;

  // --- Material air
  density = 1.205e-3.;  // [g/cm^3]
  TGeoMixture* matAir = new TGeoMixture("sts_air", 3, density);
  matAir->AddElement(14.0067, 7, 0.755);      // Nitrogen
  matAir->AddElement(15.999,  8, 0.231);      // Oxygen
  matAir->AddElement(39.948, 18, 0.014);      // Argon

  // --- Material silicon
  density    = 2.33;     // [g/cm^3]
  TGeoElement* elSi   = gGeoMan->GetElementTable()->GetElement(14);
  TGeoMaterial* matSi = new TGeoMaterial("matSi", elSi, density);


  // --- Air (passive)
  TGeoMedium* medAir = new TGeoMedium("air", nMedia++, matAir);
  medAir->SetParam(0, 0.);       // is passive
  medAir->SetParam(1, 1.);       // is in magnetic field
  medAir->SetParam(2, 20.);      // max. field [kG]
  medAir->SetParam(6, 0.001);    // boundary crossing precision [cm]
  
  
  // --- Active silicon for sensors
  TGeoMedium* medSiAct = new TGeoMedium("silicon", 
					nMedia++, matSi);
  medSiAct->SetParam(0, 1.);     // is active
  medSiAct->SetParam(1, 1.);     // is in magnetic field
  medSiAct->SetParam(2, 20.);    // max. field [kG]
  medSiAct->SetParam(6, 0.001);  // boundary crossing precisison [cm]

  // --- Passive silicon for cables
  TGeoMedium* medSiPas = new TGeoMedium("carbon", 
					nMedia++, matSi);
  medSiPas->SetParam(0, 0.);     // is passive
  medSiPas->SetParam(1, 1.);     // is in magnetic field
  medSiPas->SetParam(2, 20.);    // max. field [kG]
  medSiPas->SetParam(6, 0.001);  // boundary crossing precisison [cm]

  return nMedia;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Create sensors
 **
 ** Sensors are created as volumes with box shape and active silicon as medium.
 ** Four kinds of sensors: 3.2x2.2, 6.2x2.2, 6.2x4.2, 6.2x6.2
 **/
Int_t CreateSensors() {

  Int_t nSensors = 0;

  Double_t xSize      = 0.;
  Double_t ySize      = 0.;
  Double_t zSize      = gkSensorThickness;
  TGeoMedium* silicon = gGeoMan->GetMedium("silicon");


  // --- Sensor Type 01: Half small sensor (3.1 cm x 2.46 cm)
  xSize = 3.0792;
  ySize = 2.46;
  TGeoBBox* shape_sensor01 = new TGeoBBox("sensor01", 
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor01", shape_sensor01, silicon);
  nSensors++;


  // --- Sensor type 02: Small sensor (6.2 cm x 2.46 cm)
  xSize = 6.1992;
  ySize = 2.46;
  TGeoBBox* shape_sensor02 = new TGeoBBox("sensor02", 
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor02", shape_sensor02, silicon);
  nSensors++;


  // --- Sensor type 03: Medium sensor (6.2 cm x 4.46 cm)
  xSize = 6.1992;
  ySize = 4.46;
  TGeoBBox* shape_sensor03 = new TGeoBBox("sensor03", 
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor03", shape_sensor03, silicon);
  nSensors++;


  // ---  Sensor type 04: Big sensor (6.2 cm x 6.46 cm)
  xSize = 6.1992;
  ySize = 6.46;
  TGeoBBox* shape_sensor04 = new TGeoBBox("sensor04", 
					  xSize/2., ySize/2., zSize/2.);
  new TGeoVolume("Sensor04", shape_sensor04, silicon);
  nSensors++;

  return nSensors;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Create sectors
 **
 ** A sector is either a single sensor or several chained sensors.
 ** It is implemented as TGeoVolumeAssembly.
 ** Currently available:
 ** - single sensors of type 1 - 4
 ** - two chained sensors of type 4
 ** - three chained sensors of type 4
 **/
Int_t CreateSectors() {

  Int_t nSectors = 0;

  TGeoVolume* sensor01 = gGeoMan->GetVolume("Sensor01");
  TGeoVolume* sensor02 = gGeoMan->GetVolume("Sensor02");
  TGeoVolume* sensor03 = gGeoMan->GetVolume("Sensor03");
  TGeoVolume* sensor04 = gGeoMan->GetVolume("Sensor04");
  TGeoBBox*   box4     = (TGeoBBox*) sensor04->GetShape();

  // --- Sector type 1: single sensor of type 1
  TGeoVolumeAssembly* sector01 = new TGeoVolumeAssembly("Sector01");
  sector01->AddNode(sensor01, 1);
  nSectors++;

  // --- Sector type 2: single sensor of type 2
  TGeoVolumeAssembly* sector02 = new TGeoVolumeAssembly("Sector02");
  sector02->AddNode(sensor02, 1);
  nSectors++;

  // --- Sector type 3: single sensor of type 3
  TGeoVolumeAssembly* sector03 = new TGeoVolumeAssembly("Sector03");
  sector03->AddNode(sensor03, 1);
  nSectors++;

  // --- Sector type 4: single sensor of type 4
  TGeoVolumeAssembly* sector04 = new TGeoVolumeAssembly("Sector04");
  sector04->AddNode(sensor04, 1);
  nSectors++;

  // --- Sector type 5: two sensors of type 4
  TGeoVolumeAssembly* sector05 = new TGeoVolumeAssembly("Sector05");
  Double_t shift5 = 0.5 * gkChainGapY + box4->GetDY();
  TGeoTranslation* transD5 = 
    new TGeoTranslation("td", 0., -1. * shift5, 0.);
  TGeoTranslation* transU5 = 
    new TGeoTranslation("tu", 0., shift5, 0.);
  sector05->AddNode(sensor04, 1, transD5);
  sector05->AddNode(sensor04, 2, transU5);
  nSectors++;

  // --- Sector type 6: three sensors of type 4
  TGeoVolumeAssembly* sector06 = new TGeoVolumeAssembly("Sector06");
  Double_t shift6 = gkChainGapY + 2. * box4->GetDY();
  TGeoTranslation* transD6 = 
    new TGeoTranslation("td", 0., -1. * shift6, 0.);
  TGeoTranslation* transU6 = 
    new TGeoTranslation("tu", 0., shift6, 0.);
  sector06->AddNode(sensor04, 1, transD6);
  sector06->AddNode(sensor04, 2);
  sector06->AddNode(sensor04, 3, transU6);
  nSectors++;

  return nSectors;
}
/** ======================================================================= **/


  
    
/** ===========================================================================
 ** Create ladders
 **
 ** Ladders are the building blocks of the stations. They contain 
 ** several modules placed one after the other along the z axis
 ** such that the sectors are arranged vertically (with overlap).
 ** 
 ** A ladder is constructed out of two half ladders, the second of which
 ** is rotated in the x-y plane by 180 degrees and displaced
 ** in z direction.
 **/
Int_t CreateLadders() {

  Int_t nLadders = 0;

  // --- Some variables
  Int_t nSectors = 0;
  Int_t sectorTypes[10];
  TGeoBBox*   shape = NULL;
  TString     s0name;
  TGeoVolume* s0vol = NULL;
  TGeoVolume* halfLadderU = NULL;
  TGeoVolume* halfLadderD = NULL;
  Double_t shiftZ  = 0.;
  Double_t ladderY = 0.;
  Double_t gapY    = 0.;

  
  // --- Ladder 01: 10 sectors, type 3 3 2 2 2 2 2 2 3 3
  nSectors       = 5;
  sectorTypes[0] = 2;
  sectorTypes[1] = 2;
  sectorTypes[2] = 2;
  sectorTypes[3] = 3;
  sectorTypes[4] = 3;
  s0name = Form("Sector%02d", sectorTypes[0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();
  shiftZ = 2. * shape->GetDZ() + gkSectorGapZ;
  halfLadderU = ConstructHalfLadder("HalfLadder01", nSectors, sectorTypes, 'l');
  halfLadderD = halfLadderU;
  ConstructLadder("Ladder01", halfLadderU, halfLadderD, shiftZ);
  nLadders++;


  // --- Ladder 02: 10 sectors, type 3 3 2 2 1 1 2 2 3 3
  nSectors       = 5;
  sectorTypes[0] = 1;
  sectorTypes[1] = 2;
  sectorTypes[2] = 2;
  sectorTypes[3] = 3;
  sectorTypes[4] = 3;
  s0name = Form("Sector%02d", sectorTypes[0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();
  shiftZ = 2. * shape->GetDZ() + gkSectorGapZ;
  halfLadderU = ConstructHalfLadder("HalfLadder02u", nSectors, sectorTypes, 'l');
  halfLadderD = ConstructHalfLadder("HalfLadder02d", nSectors, sectorTypes, 'r');
  ConstructLadder("Ladder02", halfLadderU, halfLadderD, shiftZ);
  nLadders++;


  // --- Ladder 03: 10 sectors, type 4 4 3 3 2 2 3 3 4 4
  nSectors       = 5;
  sectorTypes[0] = 2;
  sectorTypes[1] = 3;
  sectorTypes[2] = 3;
  sectorTypes[3] = 4;
  sectorTypes[4] = 4;
  s0name = Form("Sector%02d", sectorTypes[0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();
  shiftZ = 2. * shape->GetDZ() + gkSectorGapZ;
  halfLadderU = ConstructHalfLadder("HalfLadder03", nSectors, sectorTypes, 'l');
  halfLadderD = halfLadderU;
  ConstructLadder("Ladder03", halfLadderU, halfLadderD, shiftZ);
  nLadders++;


  // --- Ladder 04: 10 sectors, type 4 4 3 3 1 1 3 3 4 4
  nSectors       = 5;
  sectorTypes[0] = 1;
  sectorTypes[1] = 3;
  sectorTypes[2] = 3;
  sectorTypes[3] = 4;
  sectorTypes[4] = 4;
  s0name = Form("Sector%02d", sectorTypes[0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();
  shiftZ = 2. * shape->GetDZ() + gkSectorGapZ;
  halfLadderU = ConstructHalfLadder("HalfLadder04u", nSectors, sectorTypes, 'l');
  halfLadderD = ConstructHalfLadder("HalfLadder04d", nSectors, sectorTypes, 'r');
  ConstructLadder("Ladder04", halfLadderU, halfLadderD, shiftZ);
  nLadders++;


  // --- Ladder 05: 10 sectors, type 5 4 4 3 3 3 3 4 4 5
  nSectors       = 5;
  sectorTypes[0] = 3;
  sectorTypes[1] = 3;
  sectorTypes[2] = 4;
  sectorTypes[3] = 4;
  sectorTypes[4] = 5;
  s0name = Form("Sector%02d", sectorTypes[0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();
  shiftZ = 2. * shape->GetDZ() + gkSectorGapZ;
  halfLadderU = ConstructHalfLadder("HalfLadder05", nSectors, sectorTypes, 'l');
  halfLadderD = halfLadderU;
  ConstructLadder("Ladder05", halfLadderU, halfLadderD, shiftZ);
  nLadders++;


  // --- Ladder 06: 8 sectors, type 3 4 4 5, with gap, same height as Ladder 05
  nSectors       = 4;
  sectorTypes[0] = 3;
  sectorTypes[1] = 4;
  sectorTypes[2] = 4;
  sectorTypes[3] = 5;
  s0vol  = gGeoMan->GetVolume("Ladder05");
  shape  = (TGeoBBox*) s0vol->GetShape();
  ladderY = 2. * shape->GetDY();
  halfLadderU = ConstructHalfLadder("HalfLadder06", nSectors, sectorTypes, 'l');
  halfLadderD = halfLadderU;
  shape = (TGeoBBox*) halfLadderU->GetShape();
  gapY = ladderY - 4. * shape->GetDY();
  ConstructLadderWithGap("Ladder06", halfLadderU, halfLadderD, gapY);
  nLadders++;


  // --- Ladder 07: 10 sectors, type 6 5 4 3 3 3 3 4 5 6
  nSectors       = 5;
  sectorTypes[0] = 3;
  sectorTypes[1] = 3;
  sectorTypes[2] = 4;
  sectorTypes[3] = 5;
  sectorTypes[4] = 6;
  s0name = Form("Sector%02d", sectorTypes[0]);
  s0vol  = gGeoMan->GetVolume(s0name);
  shape  = (TGeoBBox*) s0vol->GetShape();
  shiftZ = 2. * shape->GetDZ() + gkSectorGapZ;
  halfLadderU = ConstructHalfLadder("HalfLadder07", nSectors, sectorTypes, 'l');
  halfLadderD = halfLadderU;
  ConstructLadder("Ladder07", halfLadderU, halfLadderD, shiftZ);
  nLadders++;


  // --- Ladder 08: 8 sensors, type 3 4 5 6, with gap, same height as Ladder 07
  nSectors       = 4;
  sectorTypes[0] = 3;
  sectorTypes[1] = 4;
  sectorTypes[2] = 5;
  sectorTypes[3] = 6;
  s0vol  = gGeoMan->GetVolume("Ladder07");
  shape  = (TGeoBBox*) s0vol->GetShape();
  ladderY = 2. * shape->GetDY();
  halfLadderU = ConstructHalfLadder("HalfLadder08", nSectors, sectorTypes, 'l');
  halfLadderD = halfLadderU;
  shape = (TGeoBBox*) halfLadderU->GetShape();
  gapY = ladderY - 4. * shape->GetDY();
  ConstructLadderWithGap("Ladder08", halfLadderU, halfLadderD, gapY);
  nLadders++;

  return nLadders;
}
/** ======================================================================= **/

 

// ****************************************************************************
// *****                                                                  *****
// *****    Generic functions  for the construction of STS elements       *****
// *****                                                                  *****
// *****  module:     volume (made of a sector and a cable)               *****
// *****  haf ladder: assembly (made of modules)                          *****
// *****  ladder:     assembly (made of two half ladders)                 *****
// *****  station:    volume (made of ladders)                            *****
// *****                                                                  *****
// ****************************************************************************


    
/** ===========================================================================
 ** Construct a module
 **
 ** A module is a sector plus the readout cable extending from the
 ** top of the sector. The cable is made from passive silicon.
 ** The cable has the same x size as the sector.
 ** Its thickness is given by the global variable gkCableThickness.
 ** The cable length is a parameter.
 ** The sensor(s) of the sector is/are placed directly in the module;
 ** the sector is just auxiliary for the proper placement.
 **
 ** Arguments: 
 **            name             volume name
 **            sector           pointer to sector volume
 **            cableLength      length of cable
 **/
TGeoVolume* ConstructModule(const char* name, 
			    TGeoVolume* sector,
			    Double_t cableLength) {

  // --- Check sector volume
  if ( ! sector ) Fatal("CreateModule", "Sector volume not found!");

  // --- Get size of sector
  TGeoBBox* box = (TGeoBBox*) sector->GetShape(); 
  Double_t sectorX = 2. * box->GetDX();
  Double_t sectorY = 2. * box->GetDY();
  Double_t sectorZ = 2. * box->GetDZ();

  // --- Get size of cable
  Double_t cableX   = sectorX;
  Double_t cableY   = cableLength;
  Double_t cableZ   = gkCableThickness;

  // --- Create module volume
  Double_t moduleX = TMath::Max(sectorX, cableX);
  Double_t moduleY = sectorY + cableLength;
  Double_t moduleZ = TMath::Max(sectorZ, cableZ);
  TGeoVolume* module = gGeoManager->MakeBox(name, gStsMedium,
					    moduleX/2.,
					    moduleY/2.,
					    moduleZ/2.);

  // --- Position of sector in module
  // --- Sector is centred in x and z and aligned to the bottom
  Double_t sectorXpos = 0.;
  Double_t sectorYpos = 0.5 * (sectorY - moduleY);
  Double_t sectorZpos = 0.;


  // --- Get sensor(s) from sector
  Int_t nSensors = sector->GetNdaughters();
  for (Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
    TGeoNode* sensor = sector->GetNode(iSensor);

    // --- Calculate position of sensor in module
    Double_t* xSensTrans = sensor->GetMatrix()->GetTranslation();
    Double_t sensorXpos = 0.;
    Double_t sensorYpos = sectorYpos + xSensTrans[1];
    Double_t sensorZpos = 0.;
    TGeoTranslation* sensTrans = new TGeoTranslation("sensTrans",
						     sensorXpos,
						     sensorYpos,
						     sensorZpos);

    // --- Add sensor volume to module
    TGeoVolume* sensVol = sensor->GetVolume();
    module->AddNode(sensor->GetVolume(), iSensor+1, sensTrans);
  }


  // --- Create cable volume, if necessary, and place it in module
  // --- Cable is centred in x and z and aligned to the top
  if ( gkConstructCables && cableLength > 0.0001 ) {
    TString cableName = TString(name) + "_cable";
    TGeoMedium* cableMedium = gGeoMan->GetMedium("silicon");
    if ( ! cableMedium ) Fatal("CreateModule", "Medium silicon not found!");
    TGeoVolume* cable = gGeoManager->MakeBox(cableName.Data(),
					     cableMedium,
					     cableX / 2.,
					     cableY / 2.,
					     cableZ / 2.);
    Double_t cableXpos = 0.;
    Double_t cableYpos = sectorY + 0.5 * cableY - 0.5 * moduleY;
    Double_t cableZpos = 0.;
    TGeoTranslation* cableTrans = new TGeoTranslation("cableTrans",
						      cableXpos,
						      cableYpos,
						      cableZpos);
    module->AddNode(cable, 1, cableTrans);
  }

  return module;
}
/** ======================================================================= **/
 



/** ===========================================================================
 ** Construct a half ladder
 **
 ** A half ladder is a virtual volume (TGeoVolumeAssembly) consisting
 ** of several modules arranged on top of each other. The modules
 ** have a given overlap in y and a displacement in z to allow for the
 ** overlap.
 **
 ** The typ of sectors / modules to be placed must be specified:
 **    1 = sensor01
 **    2 = sensor02
 **    3 = sensor03
 **    4 = sensor04
 **    5 = 2 x sensor04 (chained)
 **    6 = 3 x sensor04 (chained)
 ** The cable is added automatically from the top of each sensor to
 ** the top of the half ladder.
 ** The alignment can be left (l) or right (r), which matters in the
 ** case of different x sizes of sensors (e.g. SensorType01).
 **
 ** Arguments: 
 **            name             volume name
 **            nSectors         number of sectors
 **            sectorTypes      array with sector types
 **            align            horizontal alignment of sectors
 **/
TGeoVolume* ConstructHalfLadder(const TString& name,
				Int_t nSectors,
				Int_t* sectorTypes,
				char align) {

  // --- Create half ladder volume assembly
  TGeoVolumeAssembly* halfLadder = new TGeoVolumeAssembly(name);

  // --- Determine size of ladder
  Double_t ladderX = 0.;
  Double_t ladderY = 0.;
  Double_t ladderZ = 0.;
  for (Int_t iSector = 0; iSector < nSectors; iSector++) {
    TString sectorName = Form("Sector%02d", 
			      sectorTypes[iSector]);
    TGeoVolume* sector = gGeoMan->GetVolume(sectorName);
    if ( ! sector )
      Fatal("ConstructHalfLadder", Form("Volume %s not found", sectorName));
    TGeoBBox* box = sector->GetShape();
    // --- Ladder x size equals largest sector x size
    ladderX = TMath::Max(ladderX, 2. * box->GetDX());
    // --- Ladder y size is sum of sector ysizes
    ladderY += 2. * box->GetDY();
    // --- Ladder z size is sum of sector z sizes
    ladderZ += 2. * box->GetDZ();
  }
  // --- Subtract overlaps in y
  ladderY -= Double_t(nSectors-1) * gkSectorOverlapY;
  // --- Add gaps in z direction
  ladderZ += Double_t(nSectors-1) * gkSectorGapZ;


  // --- Create and place modules
  Double_t yPosSect = -0.5 * ladderY;
  Double_t zPosMod  = -0.5 * ladderZ;
  for (Int_t iSector = 0; iSector < nSectors; iSector++) {
    TString sectorName = Form("Sector%02d", 
			      sectorTypes[iSector]);
    TGeoVolume* sector = gGeoMan->GetVolume(sectorName);
    TGeoBBox* box = (TGeoBBox*) sector->GetShape();
    Double_t sectorX = 2. * box->GetDX();
    Double_t sectorY = 2. * box->GetDY();
    Double_t sectorZ = 2. * box->GetDZ();
    yPosSect += 0.5 * sectorY;   // Position of sector in ladder
    Double_t cableLength = 0.5 * ladderY - yPosSect - 0.5 * sectorY;
    TString moduleName = name + "_" + Form("Module%02d", 
					   sectorTypes[iSector]);
    TGeoVolume* module = ConstructModule(moduleName.Data(),
					 sector, cableLength);

    TGeoBBox* shapeMod = (TGeoBBox*) module->GetShape();
    Double_t moduleX = 2. * shapeMod->GetDX();
    Double_t moduleY = 2. * shapeMod->GetDY();
    Double_t moduleZ = 2. * shapeMod->GetDZ();
    Double_t xPosMod = 0.;
    if ( align == 'l' ) 
      xPosMod = 0.5 * (moduleX - ladderX);  // left aligned
    else if ( align == 'r' )
      xPosMod = 0.5 * (ladderX - moduleX);  // right aligned
    else 
      xPosMod = 0.;                         // centred in x
    Double_t yPosMod = 0.5 * (ladderY - moduleY);  // top aligned
    zPosMod += 0.5 * moduleZ;
    TGeoTranslation* trans = new TGeoTranslation("t", xPosMod, 
						 yPosMod, zPosMod);
    halfLadder->AddNode(module, iSector+1, trans);
    yPosSect += 0.5 * sectorY - gkSectorOverlapY;
    zPosMod  += 0.5 * moduleZ + gkSectorGapZ;
  }

  CheckVolume(halfLadder);
  cout << endl;

  return halfLadder;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Construct a ladder out of two half ladders
 ** 
 ** The second half ladder will be rotated by 180 degrees 
 ** in the x-y plane. The two half ladders will be put on top of each
 ** other with a vertical overlap and displaced in z bz shiftZ.
 **
 ** Arguments: 
 **            name             volume name
 **            halfLadderU      pointer to upper half ladder
 **            halfLadderD      pointer to lower half ladder
 **            shiftZ           relative displacement along the z axis
 **/
 TGeoVolume* ConstructLadder(const char* name,
			     TGeoVolume* halfLadderU,
			     TGeoVolume* halfLadderD,
			     Double_t shiftZ) {

  // --- Some variables
  TGeoBBox* shape = NULL;

  // --- Dimensions of half ladders
  shape = (TGeoBBox*) halfLadderU->GetShape();
  Double_t xu = 2. * shape->GetDX();
  Double_t yu = 2. * shape->GetDY();
  Double_t zu = 2. * shape->GetDZ();
  shape = (TGeoBBox*) halfLadderD->GetShape();
  Double_t xd = 2. * shape->GetDX();
  Double_t yd = 2. * shape->GetDY();
  Double_t zd = 2. * shape->GetDZ();

  // --- Create ladder volume assembly
  Double_t ladderX = TMath::Max(xu, xd);
  Double_t ladderY = yu + yd - gkSectorOverlapY;
  Double_t ladderZ = TMath::Max(zu, zd + shiftZ);
  TGeoVolumeAssembly* ladder = new TGeoVolumeAssembly(name);

  // --- Place half ladders
  Double_t xPosU = 0.;                      // centred in x
  Double_t yPosU = 0.5 * ( ladderY - yu );  // top aligned
  Double_t zPosU = 0.5 * ( ladderZ - zu );  // front aligned
  TGeoTranslation* tu = new TGeoTranslation("tu", xPosU,
					    yPosU, zPosU);
  ladder->AddNode(halfLadderU, 1, tu);
  Double_t xPosD = 0.;                      // centred in x
  Double_t yPosD = 0.5 * ( yd - ladderY );  // bottom aligned
  Double_t zPosD = 0.5 * ( zd - ladderZ );  // back aligned
  TGeoRotation* rd = new TGeoRotation();
  rd->RotateZ(180.);
  TGeoCombiTrans* cd = new TGeoCombiTrans(xPosD, yPosD, zPosD,  rd);
  ladder->AddNode(halfLadderD, 2, cd);

  return ladder;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Construct a ladder out of two half ladders with vertical gap
 ** 
 ** The second half ladder will be rotated by 180 degrees 
 ** in the x-y plane. The two half ladders will be put on top of each
 ** other with a vertical gap.
 **
 ** Arguments: 
 **            name             volume name
 **            halfLadderU      pointer to upper half ladder
 **            halfLadderD      pointer to lower half ladder
 **            gapY             vertical gap
 **/
 TGeoVolume* ConstructLadderWithGap(const char* name,
				    TGeoVolume* halfLadderU,
				    TGeoVolume* halfLadderD,
				    Double_t gapY) {

  // --- Some variables
  TGeoBBox* shape = NULL;

  // --- Dimensions of half ladders
  shape = (TGeoBBox*) halfLadderU->GetShape();
  Double_t xu = 2. * shape->GetDX();
  Double_t yu = 2. * shape->GetDY();
  Double_t zu = 2. * shape->GetDZ();
  shape = (TGeoBBox*) halfLadderD->GetShape();
  Double_t xd = 2. * shape->GetDX();
  Double_t yd = 2. * shape->GetDY();
  Double_t zd = 2. * shape->GetDZ();

  // --- Create ladder volume assembly
  Double_t ladderX = TMath::Max(xu, xd);
  Double_t ladderY = yu + yd + gapY;
  Double_t ladderZ = TMath::Max(zu, zd);
  TGeoVolumeAssembly* ladder = new TGeoVolumeAssembly(name);

  // --- Place half ladders
  Double_t xPosU = 0.;                      // centred in x
  Double_t yPosU = 0.5 * ( ladderY - yu );  // top aligned
  Double_t zPosU = 0.5 * ( ladderZ - zu );  // front aligned
  TGeoTranslation* tu = new TGeoTranslation("tu", xPosU,
					    yPosU, zPosU);
  ladder->AddNode(halfLadderU, 1, tu);
  Double_t xPosD = 0.;                      // centred in x
  Double_t yPosD = 0.5 * ( yd - ladderY );  // bottom aligned
  Double_t zPosD = 0.5 * ( zd - ladderZ );  // back aligned
  TGeoRotation* rd = new TGeoRotation();
  rd->RotateZ(180.);
  TGeoCombiTrans* cd = new TGeoCombiTrans(xPosD, yPosD, zPosD,  rd);
  ladder->AddNode(halfLadderD, 2, cd);

  return ladder;
}
/** ======================================================================= **/




/** ===========================================================================
 ** Construct a station
 **
 ** The station volume is the minimal  box comprising all ladders
 ** minus a tube accomodating the beam pipe.
 **
 ** The ladders are arranged horizontally from left to right with
 ** a given overlap in x.
 ** Every second ladder is slightly displaced upstream from the centre
 ** z plane and facing downstream, the others are slightly displaced
 ** downstream and facing upstream (rotated around the y axis).
 **
 ** Arguments: 
 **            name             volume name
 **            nLadders         number of ladders
 **            ladderTypes      array of ladder types
 **            rHole            radius of inner hole
 **/
 TGeoVolume* ConstructStation(const char* name, Int_t nLadders,
			      Int_t* ladderTypes, Double_t rHole) {

  // --- Some local variables
  TGeoShape* statShape  = NULL;
  TGeoBBox* ladderShape = NULL;
  TGeoVolume* ladder    = NULL;
  TString ladderName;


  // --- Determine size of station from ladders
  Double_t statX     = 0.;
  Double_t statY     = 0.;
  Double_t statZeven = 0.;
  Double_t statZodd  = 0.;
  Double_t statZ     = 0.;  
  for (Int_t iLadder = 0; iLadder < nLadders; iLadder++) {
    Int_t ladderType = ladderTypes[iLadder];
    ladderName = Form("Ladder%02d", ladderType);
    ladder = gGeoManager->GetVolume(ladderName);
    if ( ! ladder ) Fatal("ConstructStation", 
			  Form("Volume %s not found", ladderName.Data()));
    shape = (TGeoBBox*) ladder->GetShape();
    statX += 2. * shape->GetDX();
    statY = TMath::Max(statY, 2. * shape->GetDY());
    if ( iLadder % 2 ) statZeven = TMath::Max(statZeven, 2. * shape->GetDZ() );
    else statZodd = TMath::Max(statZodd, 2. * shape->GetDZ() );
  }
  statX -= Double_t(nLadders-1) * gkLadderOverlapX;
  statZ = statZeven + gkLadderGapZ + statZodd;

  // --- Create station volume
  TString boxName(name);
  boxName += "_box";
  TGeoBBox* statBox = new TGeoBBox(boxName, statX/2., statY/2., statZ/2.);
  TString tubName(name);
  tubName += "_tub";
  TString expression = boxName + "-" + tubName;
  TGeoTube* statTub = new TGeoTube(tubName, 0., rHole, statZ/2.);
  statShape = new TGeoCompositeShape(name, expression.Data());
  TGeoVolume* station = new TGeoVolume(name, statShape, gStsMedium);

  // --- Place ladders in station
  Double_t xPos = -0.5 * statX;
  Double_t yPos = 0.;
  Double_t zPos = 0.;
  for (Int_t iLadder = 0; iLadder < nLadders; iLadder++) {
    Int_t ladderType = ladderTypes[iLadder];
    ladderName = Form("Ladder%02d", ladderType);
    ladder = gGeoManager->GetVolume(ladderName);
    shape = (TGeoBBox*) ladder->GetShape();
    xPos += shape->GetDX();
    yPos = 0.;    // vertically centred  
    TGeoRotation* rot = new TGeoRotation();
    // --- Unrotated ladders
    if ( (nLadders/2 + iLadder) % 2 ) {
      zPos = -0.5 * gkLadderGapZ - shape->GetDZ();
    }
    // --- Rotated ladders
    else {
      zPos = 0.5 * gkLadderGapZ + shape->GetDZ();
      rot->RotateY(180.);
    }
    TGeoCombiTrans* trans = new TGeoCombiTrans(xPos, yPos, zPos, rot);
    station->AddNode(ladder, iLadder+1, trans);
    xPos += shape->GetDX() - gkLadderOverlapX;
  }

  return station;
 }
/** ======================================================================= **/




/** ===========================================================================
 ** Volume information for debugging
 **/
void CheckVolume(TGeoVolume* volume) {

  TGeoBBox* shape = (TGeoBBox*) volume->GetShape();
  cout << volume->GetName() << ": size " << fixed << setprecision(4)
       << setw(7) << 2. * shape->GetDX() << " x " << setw(7)
       << 2. * shape->GetDY() << " x " << setw(7)
       << 2. * shape->GetDZ();
  if ( volume->IsAssembly() ) cout << ", assembly";
  else {
    if ( volume->GetMedium() ) 
      cout << ", medium " << volume->GetMedium()->GetName();
    else cout << ", " << "\033[31m" << " no medium" << "\033[0m";
  }
  cout << endl;
  if ( volume->GetNdaughters() ) {
    cout << "Daughters: " << endl;
    for (Int_t iNode = 0; iNode < volume->GetNdaughters(); iNode++) {
      TGeoNode* node = volume->GetNode(iNode);
      TGeoBBox* shape = (TGeoBBox*) node->GetVolume()->GetShape();
      cout << setw(15) << node->GetName() << ", size " 
	   << fixed << setprecision(3)
	   << setw(6) << 2. * shape->GetDX() << " x " 
	   << setw(6) << 2. * shape->GetDY() << " x " 
	   << setw(6) << 2. * shape->GetDZ() << ", position ( ";
      TGeoMatrix* matrix = node->GetMatrix();
      Double_t* pos = matrix->GetTranslation();
      cout << setfill(' ');
      cout << fixed << setw(8) << pos[0] << ", " 
	   << setw(8) << pos[1] << ", "
	   << setw(8) << pos[2] << " )" << endl;
    }
  }

}
/** ======================================================================= **/

 
/** ===========================================================================
 ** Volume information for output to file
 **/
void CheckVolume(TGeoVolume* volume, fstream& file) {

  if ( ! file ) return;

  TGeoBBox* shape = (TGeoBBox*) volume->GetShape();
  file << volume->GetName() << ": size " << fixed << setprecision(4)
       << setw(7) << 2. * shape->GetDX() << " x " << setw(7)
       << 2. * shape->GetDY() << " x " << setw(7)
       << 2. * shape->GetDZ();
  if ( volume->IsAssembly() ) file << ", assembly";
  else {
    if ( volume->GetMedium() ) 
      file << ", medium " << volume->GetMedium()->GetName();
    else file << ", " << "\033[31m" << " no medium" << "\033[0m";
  }
  file << endl;
  if ( volume->GetNdaughters() ) {
    file << "Contains: ";
    for (Int_t iNode = 0; iNode < volume->GetNdaughters(); iNode++) 
      file << volume->GetNode(iNode)->GetVolume()->GetName() << " ";
    file << endl;
  }

}
/** ======================================================================= **/


   
