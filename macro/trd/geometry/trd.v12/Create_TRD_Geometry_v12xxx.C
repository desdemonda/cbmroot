#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TFile.h"
#include "TString.h"
#include "TList.h"

#include <iostream>

// in root all sizes are diven in cm
// febs are made of pefoam20 - which is not their final density (!!)

// Name of output file with geometry
const TString FileName = "trd_v12x.root";

// Parameters defining the layout of the complete detector build out of different
// detector layers.
const Int_t NofLayers = 10;
const Int_t LayerType[NofLayers] = { 10, 11, 10, 11, 20, 21, 20, 21, 30, 31 };
const Float_t LayerPosition[NofLayers] = { 450., 500., 550., 600., 675., 725., 775., 825., 900., 950. };
const Float_t LayerNrInStation[NofLayers] = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2 };
const Float_t LayerThickness = 49.5; // Thickness of one TRD layer
const Bool_t IncludeFebs = true; // true if FEBs are included in geometry
//const Bool_t IncludeFebs = false; // true if FEBs are included in geometry

const Int_t LayerArraySize[3][4] =  { { 5, 5, 9, 11 },
                                      { 5, 5, 9, 11 },
                                      { 5, 5, 9, 11 } };
// ### Layer Type 1
// v12x - module types in the inner sector of layer1 - looking upstream
const Int_t layer1i[5][5] = { { 423,  323,  221,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              { 323,  123,  121,  121,  321 },  
                              { 203,  103,    0,  101,  201 },  
                              { 303,  103,  101,  101,  301 },  
                              { 403,  303,  201,  301,  401 } };
// number of modules 1x0, 8x1, 4x2, 8x3, 4x4

// v12x - module types in the outer sector of layer1 - looking upstream
const Int_t layer1o[9][11]= { {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,  823,  823,  723,  721,  721,  821,  821,    0,    0 },
                              {  0,    0,  823,  723,    0,    0,    0,  721,  821,    0,    0 },
                              {  0,    0,  803,  703,    0,    0,    0,  701,  801,    0,    0 },
                              {  0,    0,  803,  703,    0,    0,    0,  701,  801,    0,    0 },
                              {  0,    0,  803,  803,  703,  701,  701,  801,  801,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };
// number of modules 73x0, 0x5, 0x6, 12x7, 14x8
// Layer1 =  24 + 26;   // v12x

// ### Layer Type 11 is Layer Type 1 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 

// ### Layer Type 2

// v12x - module types in the inner sector of layer2 - looking upstream
const Int_t layer2i[5][5] = { { 423,  323,  321,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers 
                              { 323,  223,  121,  221,  321 },
                              { 303,  103,    0,  101,  301 },
                              { 303,  203,  101,  201,  301 },
                              { 403,  303,  301,  301,  401 } };
// number of modules 1x0, 4x1, 4x2, 12x3, 4x4

// v12x - module types in the outer sector of layer2 - looking upstream
const Int_t layer2o[9][11]= { {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {   0,  823,  823,  823,  823,  821,  821,  821,  821,  821,    0 },
                              {   0,  823,  823,  823,  623,  621,  621,  821,  821,  821,    0 },
                              {   0,  823,  823,  623,    0,    0,    0,  621,  821,  821,    0 },
                              {   0,  803,  803,  603,    0,    0,    0,  601,  801,  801,    0 },
                              {   0,  803,  803,  603,    0,    0,    0,  601,  801,  801,    0 },
                              {   0,  803,  803,  803,  603,  601,  601,  801,  801,  801,    0 },
                              {   0,  803,  803,  803,  803,  801,  801,  801,  801,  801,    0 },
                              {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };
// number of modules 45x0, 0x5, 12x6, 0x7, 42x8
// Layer2 =  78;   // v12x

// ### Layer Type 21 is Layer Type 2 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 

// ### Layer Type 3

// v12x - module types in the inner sector of layer2 - looking upstream
const Int_t layer3i[5][5] = { {  0,  0,  0,  0,  0 },     // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 } };
// number of modules 25x0 
// Only for convinience in the function needed


// v12x - module types in the outer sector of layer3 - looking upstream
const Int_t layer3o[9][11] = { { 823,  823,  823,  823,  823,  821,  821,  821,  821,  821,  821 },
                               { 823,  823,  823,  723,  723,  721,  721,  721,  821,  821,  821 },
                               { 823,  823,  723,  723,  623,  621,  621,  721,  721,  821,  821 },
                               { 823,  823,  723,  623,  523,  521,  521,  621,  721,  821,  821 },
                               { 803,  803,  703,  603,  503,    0,  501,  601,  701,  801,  801 },
                               { 803,  803,  703,  603,  503,  501,  501,  601,  701,  801,  801 },
                               { 803,  803,  703,  703,  603,  601,  601,  701,  701,  801,  801 },
                               { 803,  803,  803,  703,  703,  701,  701,  701,  801,  801,  801 },
                               { 803,  803,  803,  803,  803,  801,  801,  801,  801,  801,  801 } };
// number of modules 1x0, 8x5, 12x6, 24x7, 54x8
// Layer3 = 98;   // v12x

// ### Layer Type 31 is Layer Type 3 with detector modules rotated by 90
// In the subroutine creating the layers this is recognized automatically 

// Parameters defining the layout of the different detector modules
const Int_t NofModuleTypes = 8;
const Int_t ModuleType[NofModuleTypes]    = { 0, 0, 0, 0, 1, 1, 1, 1 };
const Int_t FebsPerModule[NofModuleTypes] = { 19, 10, 5, 5, 12, 6, 4, 3 }; // mod4 = mod3

const Float_t FrameWidth[2]    = { 1.5, 2.0 };   // Width of detector frames in cm
const Float_t DetectorSizeX[2] = { 60., 100.};
const Float_t DetectorSizeY[2] = { 60., 100.};

const Float_t radiator_thickness     =  36.0;    //  36 cm thickness of radiator
const Float_t radiator_position      =  - LayerThickness/2. + radiator_thickness/2.;

const Float_t gas_thickness          =   1.2;    //  12 mm thickness of gas
const Float_t gas_position           =  radiator_position + radiator_thickness/2. + gas_thickness/2.;

// todo - put realistic thickness of padplane
const Float_t padplane_thickness     =   0.0030; //  30 micron thickness of padplane
const Float_t padplane_position      =  gas_position + gas_thickness/2. + padplane_thickness/2.;

// todo - replace by backpanel
const Float_t mylar_thickness        =   0.1500; //  1.5 mm thickness of mylar
const Float_t mylar_position         =  padplane_position + padplane_thickness/2. + mylar_thickness/2.;

// todo - skip electronics plane, we have FEBs now
const Float_t electronics_thickness  =   0.0070; //  70 micron thickness of padplane
const Float_t electronics_position   =  mylar_position + mylar_thickness/2. + electronics_thickness/2.;

const  Float_t febbox_thickness      =  10.0;    // 10 cm length of FEBs
const  Float_t febbox_position       =  electronics_position + electronics_thickness/2. + febbox_thickness/2.;
const  Float_t feb_thickness         =   0.5; //2.0;   //  5 mm thickness of FEBs

const Float_t frame_thickness        =  radiator_thickness + gas_thickness + padplane_thickness 
                                        + mylar_thickness + electronics_thickness;   // frames cover radiator up to the backpanel
const Float_t frame_position         =  - LayerThickness /2. + frame_thickness/2.;

// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString RadiatorVolumeMedium    = "pefoam20";
//const TString RadiatorVolumeMedium = "polypropylene";
const TString GasVolumeMedium         = "TRDgas";
const TString PadVolumeMedium         = "goldcoatedcopper";
const TString MylarVolumeMedium       = "mylar";
const TString ElectronicsVolumeMedium = "goldcoatedcopper";
const TString febVolumeMedium         = "pefoam20";  // todo - put correct FEB material here
const TString FrameVolumeMedium       = "G10";


// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume* gModules[NofModuleTypes]; // Global storage for module types

// Forward declarations
void create_materials_from_media_file();
void create_trd_module(Int_t moduleType);
void create_detector_layers(Int_t layer);

void Create_TRD_Geometry_v12xxx() {
  // Load the necessary FairRoot libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  // Load needed material definition from media.geo file
  create_materials_from_media_file();

  // Get the GeoManager for later usage
  gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
  gGeoMan->SetVisLevel(10);  
  
  // Create the top volume 
  TGeoBBox* topbox= new TGeoBBox("", 1000., 1000., 1000.);
  TGeoVolume* top = new TGeoVolume("top", topbox, gGeoMan->GetMedium("air"));
  gGeoMan->SetTopVolume(top);

  TGeoVolume* trd = new TGeoVolumeAssembly("trd1");
  top->AddNode(trd, 1);

  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++) {
     Int_t moduleType = iModule + 1;
     gModules[iModule] = create_trd_module(moduleType);
  }

  for (Int_t iLayer = 0; iLayer < NofLayers; iLayer++) {
    //    if ((iLayer != 0) && (iLayer != 3))  continue;  // first layer only - comment later on
    //    if (iLayer != 0) continue;  // first layer only - comment later on
    create_detector_layers(iLayer);
  }
  
  gGeoMan->CloseGeometry();
//  gGeoMan->CheckOverlaps(0.001);
//  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* outfile = new TFile(FileName,"RECREATE");
  top->Write();
  //gGeoMan->Write();
  outfile->Close();
  top->Draw("ogl");
  //top->Raytrace();

}

void create_materials_from_media_file()
{
  // Use the FairRoot geometry interface to load the media which are already defined
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString geoFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(geoFile);
  geoFace->readMedia();

  // Read the required media and create them in the GeoManager
  FairGeoMedia* geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* air              = geoMedia->getMedium("air");
  FairGeoMedium* pefoam20         = geoMedia->getMedium("pefoam20");
//  FairGeoMedium* polypropylene  = geoMedia->getMedium("polypropylene");
  FairGeoMedium* trdGas           = geoMedia->getMedium("TRDgas");
  FairGeoMedium* goldCoatedCopper = geoMedia->getMedium("goldcoatedcopper");
  FairGeoMedium* mylar            = geoMedia->getMedium("mylar");
  FairGeoMedium* G10              = geoMedia->getMedium("G10");
  FairGeoMedium* PeFoam20         = geoMedia->getMedium("pefoam20");

  geoBuild->createMedium(air);
  geoBuild->createMedium(pefoam20);
//  geoBuild->createMedium(polypropylene);
  geoBuild->createMedium(trdGas);
  geoBuild->createMedium(goldCoatedCopper);
  geoBuild->createMedium(mylar);
  geoBuild->createMedium(G10);
  geoBuild->createMedium(PeFoam20);
}

TGeoVolume* create_trd_module(Int_t moduleType)
{
  Int_t type = ModuleType[moduleType - 1];
  Float_t sizeX = DetectorSizeX[type];
  Float_t sizeY = DetectorSizeY[type];
  Float_t frameWidth = FrameWidth[type];
  Float_t activeAreaX = sizeX - 2 * frameWidth;
  Float_t activeAreaY = sizeY - 2 * frameWidth;

  TGeoMedium* keepVolMed        = gGeoMan->GetMedium(KeepingVolumeMedium);
  TGeoMedium* radVolMed         = gGeoMan->GetMedium(RadiatorVolumeMedium);
  TGeoMedium* gasVolMed         = gGeoMan->GetMedium(GasVolumeMedium);
  TGeoMedium* padVolMed         = gGeoMan->GetMedium(PadVolumeMedium);
  TGeoMedium* mylarVolMed       = gGeoMan->GetMedium(MylarVolumeMedium);
  TGeoMedium* electronicsVolMed = gGeoMan->GetMedium(ElectronicsVolumeMedium);
  TGeoMedium* frameVolMed       = gGeoMan->GetMedium(FrameVolumeMedium);
  TGeoMedium* febVolMed         = gGeoMan->GetMedium(febVolumeMedium);

  TString name = Form("trd1mod%d", moduleType);
  TGeoVolume* module = new TGeoVolumeAssembly(name);
   
   // Radiator
   TGeoBBox* trd_radiator = new TGeoBBox("", activeAreaX / 2, activeAreaY / 2, radiator_thickness / 2.);
   TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd1mod%dradiator", moduleType), trd_radiator, radVolMed);
   trdmod1_radvol->SetLineColor(kBlue);
   TGeoTranslation* trd_radiator_trans = new TGeoTranslation("", 0., 0., radiator_position);
   module->AddNode(trdmod1_radvol, 0, trd_radiator_trans);


   // Gas
   TGeoBBox* trd_gas = new TGeoBBox("", activeAreaX / 2, activeAreaY / 2, gas_thickness / 2.);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd1mod%dgas", moduleType), trd_gas, gasVolMed);
   trdmod1_gasvol->SetLineColor(kYellow);
   TGeoTranslation* trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   module->AddNode(trdmod1_gasvol, 0, trd_gas_trans);


   // Pad Plane
   TGeoBBox *trd_pad = new TGeoBBox("", activeAreaX / 2, activeAreaY / 2, padplane_thickness / 2.);
   TGeoVolume* trdmod1_padvol = new TGeoVolume(Form("trd1mod%dpadplane", moduleType), trd_pad, padVolMed);
   TGeoTranslation *trd_pad_trans = new TGeoTranslation("", 0., 0., padplane_position);
   module->AddNode(trdmod1_padvol, 0, trd_pad_trans);


   // mylar
   TGeoBBox* trd_mylar = new TGeoBBox("", activeAreaX / 2, activeAreaY / 2, mylar_thickness / 2.);
   TGeoVolume* trdmod1_mylarvol = new TGeoVolume(Form("trd1mod%dmylar", moduleType), trd_mylar, mylarVolMed);
   TGeoTranslation* trd_mylar_trans = new TGeoTranslation("", 0., 0., mylar_position);
   module->AddNode(trdmod1_mylarvol, 0, trd_mylar_trans);


   // electronics
   TGeoBBox* trd_el = new TGeoBBox("", activeAreaX / 2, activeAreaY / 2, electronics_thickness / 2.);
   TGeoVolume* trdmod1_elvol = new TGeoVolume(Form("trd1mod%delectronics", moduleType), trd_el, electronicsVolMed);
   trdmod1_elvol->SetLineColor(kGreen);
   TGeoTranslation* trd_el_trans = new TGeoTranslation("", 0., 0., electronics_position);
   module->AddNode(trdmod1_elvol, 0, trd_el_trans);


   // frame1
   TGeoBBox* trd_frame1 = new TGeoBBox("", sizeX / 2, frameWidth / 2, frame_thickness/2.);
   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("trd1mod%dframe1", moduleType), trd_frame1, frameVolMed);
   trdmod1_frame1vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame1_trans = new TGeoTranslation("", 0., activeAreaY / 2 + frameWidth / 2, frame_position);
   module->AddNode(trdmod1_frame1vol, 1, trd_frame1_trans);
   trd_frame1_trans = new TGeoTranslation("", 0., -(activeAreaY / 2 + frameWidth / 2), frame_position);
   module->AddNode(trdmod1_frame1vol, 2, trd_frame1_trans);


   // frame2
   TGeoBBox *trd_frame2 = new TGeoBBox("", frameWidth / 2, activeAreaY / 2, frame_thickness / 2.);
   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("trd1mod%dframe2", moduleType), trd_frame2, frameVolMed);
   trdmod1_frame2vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation *trd_frame2_trans = new TGeoTranslation("", activeAreaX / 2 + frameWidth / 2, 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 1, trd_frame2_trans);
   trd_frame2_trans = new TGeoTranslation("", -(activeAreaX / 2 + frameWidth / 2), 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 2, trd_frame2_trans);


   // FEBs
   if (IncludeFebs) {
      // Create all FEBs and place them in an assembly which will be added to the TRD module
      TGeoBBox* trd_feb = new TGeoBBox("", activeAreaX / 2, feb_thickness/2, febbox_thickness/2);       // the FEB itself - as a cuboid
      TGeoVolume* trdmod1_feb = new TGeoVolume(Form("trd1mod%dfeb", moduleType), trd_feb, febVolMed);   // the FEB made of a certain medium
      trdmod1_feb->SetLineColor(kBlue);    // set blue color
      TGeoVolumeAssembly* trd_feb_inclined = new TGeoVolumeAssembly(Form("trd1mod%dfebincl", moduleType)); // volume for inclined FEBs, then shifted along y
      TGeoVolumeAssembly* trd_feb_box      = new TGeoVolumeAssembly(Form("trd1mod%dfebbox", moduleType));  // the mother volume of all FEBs

      Float_t feb_rotation_angle = 45.; // 65.; // 45.; // 0.; // 70.;   // rotation around x-axis, should be < 90 degrees  

      // translations / rotations
//      TGeoTranslation *trd_feb_null;       // no displacement
      TGeoTranslation *trd_feb_trans1;     // center to corner
      TGeoRotation    *trd_feb_rotation;   // rotation around x axis
      TGeoTranslation *trd_feb_trans2;     // corner back
      TGeoTranslation *trd_feb_y_position; // shift to y position on TRD

      Float_t feb_pos;
      Float_t feb_pos_y;

// replaced by matrix operation (see below)
//  //      Float_t yback, zback;
//  //      TGeoCombiTrans  *trd_feb_placement;
//  //      // fix Z back offset 0.3 at some point
//  //      yback = -    sin(feb_rotation_angle/180*3.141)  * febbox_thickness / 2.;
//  //      zback = - (1-cos(feb_rotation_angle/180*3.141)) * febbox_thickness / 2. + 0.3;
//  //      trd_feb_placement = new TGeoCombiTrans(0, feb_pos_y + yback, zback, trd_feb_rotation);
//  //      trd_feb_inclined->AddNode(trdmod1_feb, iFeb+1, trd_feb_placement);

//      trd_feb_null       = new TGeoTranslation("", 0., 0., 0.);  // empty operation
      trd_feb_trans1     = new TGeoTranslation("", 0.,-feb_thickness/2,-febbox_thickness/2);  // move bottom right corner to center
      trd_feb_trans2     = new TGeoTranslation("", 0., feb_thickness/2, febbox_thickness/2);  // move bottom right corner back
      trd_feb_rotation   = new TGeoRotation();
      trd_feb_rotation->RotateX(feb_rotation_angle);

      TGeoHMatrix *trd_feb_incline = new TGeoHMatrix("");

//        (*trd_feb_incline) = (*trd_feb_null);        // OK
//        (*trd_feb_incline) = (*trd_feb_y_position);  // OK
//        (*trd_feb_incline) = (*trd_feb_trans1);      // OK
//        (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_y_position);  // OK
//        (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_trans2);      // OK
//        (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_rotation);    // OK
//        (*trd_feb_incline) =  (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2) * (*trd_feb_y_position);  // not OK 
          // trd_feb_y_position is displaced in rotated coordinate system

      // matrix operation to rotate FEB PCB around its corner on the backanel 
      (*trd_feb_incline) = (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2);  // OK
      trd_feb_inclined->AddNode(trdmod1_feb, 1, trd_feb_incline);

      Int_t nofFebs = FebsPerModule[ moduleType - 1 ];
      for (Int_t iFeb = 0; iFeb < nofFebs; iFeb++) {
	//        feb_pos   = (2. * iFeb + 1) / (2 * nofFebs) - 0.5;   // equal spacing o f FEBs on the backpanel
        feb_pos   = (iFeb + 0.5) / nofFebs - 0.5;   // equal spacing o f FEBs on the backpanel
        feb_pos_y = feb_pos * activeAreaY;

        // shift inclined FEB in y to its final position
//      trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, 0.1);  // with additional 1 mm offset in z direction
        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, 0.0);  // touching the backpanel with the corner
        trd_feb_box->AddNode(trd_feb_inclined, iFeb+1, trd_feb_y_position);  // position FEB in y

      }
      TGeoTranslation* trd_febbox_trans = new TGeoTranslation("", 0., 0., febbox_position);
      gGeoMan->GetVolume(name)->AddNode(trd_feb_box, 1, trd_febbox_trans);  // put febbox at correct z position wrt to the module
   }

   return module;
}

Int_t copy_nr(Int_t stationNr, Int_t layerNr, Int_t copyNr)
{
   return stationNr * 10000 + layerNr * 1000 + copyNr;
}

void create_detector_layers(Int_t layerId)
{
  Int_t module_id = 0;
  Int_t layerNrInStation = LayerNrInStation[layerId];
  Int_t layerType = LayerType[layerId] / 10;  // this is also a station number
  Int_t isRotated = LayerType[layerId] % 10;  // is 1 for layers 2,4, ...
  TGeoRotation* module_rotation = new TGeoRotation();

  Int_t stationNr = layerType;

// rotation is now done in the for loop for each module individually
//  if ( isRotated == 1 ) {
//    module_rotation = new TGeoRotation();
//    module_rotation->RotateZ(90.);
//  } else {
//    module_rotation = new TGeoRotation();
//    module_rotation->RotateZ( 0.);
//  }
  
  Int_t innerarray_size1 = LayerArraySize[layerType - 1][0];
  Int_t innerarray_size2 = LayerArraySize[layerType - 1][1];
  Int_t* innerLayer;
  
  Int_t outerarray_size1 = LayerArraySize[layerType - 1][2];
  Int_t outerarray_size2 = LayerArraySize[layerType - 1][3];
  Int_t* outerLayer;
  
  if ( 1 == layerType ) {
    innerLayer = layer1i;      
    outerLayer = layer1o; 
  } else if ( 2 == layerType ) {
    innerLayer = layer2i;      
    outerLayer = layer2o; 
  } else if ( 3 == layerType ) {
    innerLayer = layer3i;      
    outerLayer = layer3o; 
  } else {
    std::cout << "Type of layer not known" << std::endl;
  } 

  
  Int_t copyNrIn[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 1; type <= 4; type++) {
    for ( Int_t j = (innerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < innerarray_size2; i++) {
        module_id = *(innerLayer + (j * innerarray_size2 + i));
        if ( module_id /100 == type) {
          Int_t y = -(j-2);
          Int_t x =   i-2;
          Int_t xPos = DetectorSizeX[0] * x;
          Int_t yPos = DetectorSizeY[0] * y;
          copyNrIn[type - 1]++;
          Int_t copy = copy_nr(stationNr, layerNrInStation, copyNrIn[type - 1]);

          // take care of FEB orientation away from beam
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
   	     module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
          else  // layer 2,4 ...
   	     module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]

          TGeoCombiTrans* module_placement = new TGeoCombiTrans(xPos, yPos, LayerPosition[layerId], module_rotation);
          gGeoMan->GetVolume("trd1")->AddNode(gModules[type - 1], copy, module_placement);
        }
      }
    }
  }
  
  Int_t copyNrOut[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 5; type <= 8; type++) {
    for ( Int_t j = (outerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < outerarray_size2; i++) {
	module_id = *(outerLayer + (j * outerarray_size2 + i));
        if ( module_id  /100 == type) {
          Int_t y = -(j-4);
          Int_t x =   i-5;
          Int_t xPos = DetectorSizeX[1] * x;
          Int_t yPos = DetectorSizeY[1] * y;
          copyNrOut[type - 5]++;
          Int_t copy = copy_nr(stationNr, layerNrInStation, copyNrOut[type - 5]);
  
          // take care of FEB orientation - away from beam
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
          module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate   0 or 180 degrees, see layer[1-3][i,o]
          else  // layer 2,4 ...
          module_rotation->RotateZ( (module_id %10) * 90. );      // rotate  90 or 270 degrees, see layer[1-3][i,o]
    
          TGeoCombiTrans* module_placement = new TGeoCombiTrans(xPos, yPos, LayerPosition[layerId], module_rotation);
          gGeoMan->GetVolume("trd1")->AddNode(gModules[type - 1], copy, module_placement);
        }
      }
    }
  }  
}
