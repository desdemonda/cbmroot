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

void create_keeping_volumes(const char* name, Int_t station, Float_t* first,
			    Float_t* second, Float_t* radius1,
			    Float_t* radius2, Float_t* distance, 
			    Float_t thickness, 
			    TGeoVolume* vol, TGeoMedium* med);

void create_trd_body(Int_t station, Int_t layer, Float_t Frame_width,
		     Float_t Layer_thickness, Float_t Layer_pitch,
		     Int_t Chambers_per_station, Float_t Position_Station[][4],
		     Float_t Detector_size_x, Float_t Detector_size_y,
		     TGeoManager*);

//void create_trd_feb();
//void create_trd_module();

void Create_TRD_Geometry_v12b() {

  // Load the necessary FairRoot libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  // Use the FairRoot geometry interface to load the media which are
  // already defined 
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString geoFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(geoFile);
  geoFace->readMedia();
  //  geoFace->print();

  // Read the required media and create them in the GeoManager
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* FairMediumAir      = geoMedia->getMedium("air");
  FairGeoMedium* FairMediumPeFoam20 = geoMedia->getMedium("pefoam20");
//  FairGeoMedium* FairMediumPolyPropylene   = 
//    geoMedia->getMedium("polypropylene");
  FairGeoMedium* FairMediumTrdGas = geoMedia->getMedium("TRDgas");
  FairGeoMedium* FairMediumGoldCoatedCopper  = 
    geoMedia->getMedium("goldcoatedcopper");
  FairGeoMedium* FairMediumMylar   = geoMedia->getMedium("mylar");
  FairGeoMedium* FairMediumG10   = geoMedia->getMedium("G10");

  geoBuild->createMedium(FairMediumAir);
  geoBuild->createMedium(FairMediumPeFoam20);
//  geoBuild->createMedium(FairMediumPolyPropylene);
  geoBuild->createMedium(FairMediumTrdGas);
  geoBuild->createMedium(FairMediumGoldCoatedCopper);
  geoBuild->createMedium(FairMediumMylar);
  geoBuild->createMedium(FairMediumG10);

  // Get the GeoManager for later usage
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  
  // Create the top volume 
  TGeoVolume* top = new TGeoVolumeAssembly("TRD");
  gGeoMan->SetTopVolume(top);
  
  // Some debug output (if needed)

  //  gGeoManager->Dump();
  //
  //  TList* matList = gGeoMan->GetListOfMaterials();  
  //  TIter next(matList);
  //  TGeoMaterial* mat;
  //  while (( mat = (TGeoMaterial*)next() ))
  //    mat->Print();
  //
  //  TList* media_ptr = gGeoMan->GetListOfMedia();
  //  Int_t entries = media_ptr->GetEntries();
  //  cout <<"Entries: "<<entries<<endl;
  //  TIter next(media_ptr);
  //  TGeoMedium* med;
  //  while (( med = (TGeoMedium*)next() ))
  //    med->Print();

  //  exit(0);
  // Here comes now the creation of needed TRD modules.

  // Add the keeping volumes for each trd station to the top
  // volume

  //---------- Declaration of parameters ----------------------------------
  Float_t Distance[3];         // Distance between target an the front 
                               // of the TRD stations
  Float_t Station_thickness;   // Thickness of one TRD station
  Float_t Layer_thickness;     // Thickness of one layer of a TRD station
  Float_t Layer_pitch;         // Distance between 2 adjacent layers of a 
                               // TRD station
  int   Layer_number;          // Number of detector layers per station
  int   Station_number;        // Number of TRD stations in the setup
  Float_t Detector_size_x[3];  // length in mm of a detector module in 
                               // x-direction
  Float_t Detector_size_y[3];  // length in mm of a detector module in 
                               // y-direction
  Float_t Frame_width;         // Width of detector frames in mm
  Float_t Inner_radius[3];     // Inner acceptance in mm
  Float_t Outer_radius[3];     // Outer acceptance in mm

  //--------- Initialization of parameters -------------------------------------

  Distance[0]          = 450;
  Distance[1]          = 675;
  Distance[2]          = 900;

  Station_number       = 3;
  Layer_number         = 4;
  Layer_thickness      = 13.5;   
  Layer_pitch          = 15.0;   
  Station_thickness    = Layer_number * Layer_pitch;
  Frame_width          = 2;

// 29.06.2010 - DE - hardcoded keeping volumes
  Inner_radius[0] =  24.;  //  250 -  10
  Outer_radius[0] = 460.;  // 4500 + 100
  Inner_radius[1] =  24.;  //  250 -  10
  Outer_radius[1] = 660.;  // 6500 + 100
  Inner_radius[2] =  49.;  //  500 -  10
  Outer_radius[2] = 860.;  // 8500 + 100
 
  // fixed detector sizes, independent of z Position
  Detector_size_x[0] =  50;                 // x =  500 mm
  Detector_size_y[0] = Detector_size_x[0];   // y =  500 mm
  
  Detector_size_x[1] =  75;                 // x =  750 mm
  Detector_size_y[1] = Detector_size_x[1];   // y =  750 mm
  
  Detector_size_x[2] = 100;                 // x = 1000 mm
  Detector_size_y[2] = Detector_size_x[2];   // y = 1000 mm
  

  Float_t first_layer[3];
  Float_t second_layer[3];
  
  first_layer[0]=-130.;
  first_layer[1]=430.;
  first_layer[2]=460.;

  second_layer[0]=-30.;
  second_layer[1]=150.;
  second_layer[2]=460.;

  create_keeping_volumes("trd1", 1, first_layer, second_layer, 
			 Inner_radius, Outer_radius, Distance, 
			 Station_thickness, top, gGeoMan->GetMedium("air"));

  first_layer[0]=-180.;
  first_layer[1]=620.;
  first_layer[2]=660.;

  second_layer[0]=-30.;
  second_layer[1]=250.;
  second_layer[2]=660.;

  create_keeping_volumes("trd2", 2, first_layer, second_layer, 
			 Inner_radius, Outer_radius, Distance, 
			 Station_thickness, top, gGeoMan->GetMedium("air"));

  first_layer[0]=-230.;
  first_layer[1]=810.;
  first_layer[2]=860.;

  second_layer[0]=-30.;
  second_layer[1]=350.;
  second_layer[2]=860.;

  create_keeping_volumes("trd3", 3, first_layer, second_layer, 
			 Inner_radius, Outer_radius, Distance, 
			 Station_thickness, top, gGeoMan->GetMedium("air"));

  Int_t Chambers_per_station = 1;
  Float_t x101 = Detector_size_x[0] * 1;
  Float_t y101 = Detector_size_y[0] * 0;
  Float_t x102 = Detector_size_x[0] *-1;
  Float_t y102 = Detector_size_y[0] * 0;
  Float_t x103 = Detector_size_x[0] * 0;
  Float_t y103 = Detector_size_y[0] * 1;
  Float_t x104 = Detector_size_x[0] * 0;
  Float_t y104 = Detector_size_y[0] *-1;

  Float_t x105 = Detector_size_x[0] * 1;
  Float_t y105 = Detector_size_y[0] * 1;
  Float_t x106 = Detector_size_x[0] *-1;
  Float_t y106 = Detector_size_y[0] * 1;
  Float_t x107 = Detector_size_x[0] * 1;
  Float_t y107 = Detector_size_y[0] *-1;
  Float_t x108 = Detector_size_x[0] *-1;
  Float_t y108 = Detector_size_y[0] *-1;

  Float_t Position_Station1[][4] = { {x101, y101, 1, 0}, {x102, y102, 1, 0} {x103, y103, 1, 0} {x104, y104, 1, 0},
                                     {x105, y105, 1, 0}, {x106, y106, 1, 0} {x107, y107, 1, 0} {x108, y108, 1, 0} };

  create_trd_body(1, 1, Frame_width, Layer_thickness, Layer_pitch,
		  Chambers_per_station, Position_Station1,
		  Detector_size_x[0], Detector_size_y[0],
		  gGeoMan);

//  create_trd_body(1, 2, Frame_width, Layer_thickness, Layer_pitch,
//		  Chambers_per_station, Position_Station1,
//		  Detector_size_x[0], Detector_size_y[0],
//		  gGeoMan);
//
//  create_trd_body(1, 3, Frame_width, Layer_thickness, Layer_pitch,
//		  Chambers_per_station, Position_Station1,
//		  Detector_size_x[0], Detector_size_y[0],
//		  gGeoMan);
//
//  create_trd_body(1, 4, Frame_width, Layer_thickness, Layer_pitch,
//		  Chambers_per_station, Position_Station1,
//		  Detector_size_x[0], Detector_size_y[0],
//		  gGeoMan);


   gGeoMan->CloseGeometry();
   TFile* outfile = new TFile("TRD_geom_v12b.root","RECREATE");
   top->Write();
   //gGeoMan->Write();
   outfile->Close();
   top->Draw("ogl");
   //top->Raytrace();

}

void create_keeping_volumes(const char* name, Int_t station, Float_t* first,
			    Float_t* second, Float_t* radius1,
			    Float_t* radius2, Float_t* distance, 
			    Float_t thickness, TGeoVolume* vol, TGeoMedium* med)

{
   Double_t phi1, dphi, nedges, nz, z,rmin, rmax;
   // Shape: trd1 type: TGeoPgon
   phi1    = 45.;
   dphi    = 360.;
   nedges = 4;
   nz      = 4;

   TGeoPgon *trd_pgon = new TGeoPgon(name,phi1,dphi,nedges,nz);
   z     = first[0];
   rmin  = first[1];
   rmax  = first[2];
   trd_pgon->DefineSection(0, z,rmin,rmax);
   z     = second[0];
   rmin  = second[1];
   rmax  = second[2];
   trd_pgon->DefineSection(1, z,rmin,rmax);
   z     = -thickness/2;
   rmin  = radius1[station-1];
   rmax  = radius2[station-1];
   trd_pgon->DefineSection(2, z,rmin,rmax);
   z     = thickness/2;
   rmin  = radius1[station-1];
   rmax  = radius2[station-1];
   trd_pgon->DefineSection(3, z,rmin,rmax);

   TGeoShape *trd_shape = trd_pgon;
   // Volume: trd1
   TGeoVolume* trd = new TGeoVolume(name, trd_shape, med);
   trd->SetLineColor(kGreen);
   trd->SetVisibility(kFALSE);

   // Translation: 
   Float_t position = distance[station-1] + thickness/2;
   TGeoTranslation *trd_trans = new TGeoTranslation("",0. ,0. , position);
   vol->AddNode(trd, 0, trd_trans);
}

void create_trd_body(Int_t station, Int_t layer, Float_t Frame_width,
		     Float_t Layer_thickness, Float_t Layer_pitch,
		     Int_t Chambers_per_station, Float_t Position_Station[][4],
		     Float_t Detector_size_x, Float_t Detector_size_y,
		     TGeoManager* man)
{
  Float_t Active_area_x = Detector_size_x - 2 * Frame_width;
  Float_t Active_area_y = Detector_size_y - 2 * Frame_width;

  cout << "det size x " << Detector_size_x << endl;
  cout << "fra size x " << Frame_width     << endl;
  cout << "act size x " << Active_area_x   << endl;
 
  // 12 mm gas (Jun10) - intelligent

  Float_t radiator_thickness    =  2.9000 /2.;
  Float_t radiator_position     =  - Layer_thickness /2. + radiator_thickness;
  Float_t gas_thickness         =  1.2000 /2.;
  Float_t gas_position          =  radiator_position + radiator_thickness + gas_thickness;
  Float_t padplane_thickness    =   0.0030 /2.;
  Float_t padplane_position     =  gas_position + gas_thickness + padplane_thickness;
  Float_t mylar_thickness       =   0.1500 /2.;
  Float_t mylar_position        =  padplane_position + padplane_thickness + mylar_thickness;
  Float_t electronics_thickness =   0.0070 /2.;
  Float_t electronics_position  =  mylar_position + mylar_thickness + electronics_thickness;
  Float_t frame_thickness       =  radiator_thickness + gas_thickness + padplane_thickness + mylar_thickness + electronics_thickness;
  Float_t frame_position        =  frame_thickness - Layer_thickness /2.;
 
  Int_t module_number = (Int_t)Position_Station[0][2];


  
   TGeoBBox *trd_box = new TGeoBBox("", Detector_size_x/2, Detector_size_y/2, Layer_thickness/2);
   TGeoShape *trd_shape = trd_box;
   // Volume: trd1
   TString name;
   name.Form("trd%dmod%d", station, module_number);
   TGeoVolume* trdmod1_box = new TGeoVolume(name, trd_shape, man->GetMedium("air"));

   // Radiator
   TGeoBBox *trd_radiator = new TGeoBBox("", Active_area_x/2, Active_area_y/2, radiator_thickness);
   TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd%dmod%dradiator", station, module_number), trd_radiator, man->GetMedium("pefoam20"));
//   TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd%dmod%dradiator", station, module_number), trd_radiator, man->GetMedium("polypropylene"));
   trdmod1_radvol->SetLineColor(kBlue);
   TGeoTranslation *trd_radiator_trans = new TGeoTranslation("", 0., 0., radiator_position);
   man->GetVolume(name)->AddNode(trdmod1_radvol, 0, trd_radiator_trans);


   // Gas
   TGeoBBox *trd_gas = new TGeoBBox("", Active_area_x/2, Active_area_y/2, gas_thickness);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd%dmod%dgas", station, module_number), trd_gas, man->GetMedium("TRDgas"));
   TGeoTranslation *trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   man->GetVolume(name)->AddNode(trdmod1_gasvol, 0, trd_gas_trans);


   // Pad Plane
   TGeoBBox *trd_pad = new TGeoBBox("", Active_area_x/2, Active_area_y/2, padplane_thickness);
   TGeoVolume* trdmod1_padvol = new TGeoVolume(Form("trd%dmod%dpadplane", station, module_number), trd_pad, man->GetMedium("goldcoatedcopper"));
   TGeoTranslation *trd_pad_trans = new TGeoTranslation("", 0., 0., padplane_position);
   man->GetVolume(name)->AddNode(trdmod1_padvol, 0, trd_pad_trans);


   // mylar
   TGeoBBox *trd_mylar = new TGeoBBox("", Active_area_x/2, Active_area_y/2, mylar_thickness);
   TGeoVolume* trdmod1_mylarvol = new TGeoVolume(Form("trd%dmod%dmylar", station, module_number), trd_mylar, man->GetMedium("mylar"));
   TGeoTranslation *trd_mylar_trans = new TGeoTranslation("", 0., 0., mylar_position);
   man->GetVolume(name)->AddNode(trdmod1_mylarvol, 0, trd_mylar_trans);


   // electronics
   TGeoBBox *trd_el = new TGeoBBox("", Active_area_x/2, Active_area_y/2, electronics_thickness);
   TGeoVolume* trdmod1_elvol = new TGeoVolume(Form("trd%dmod%delectronics", station, module_number), trd_el, man->GetMedium("goldcoatedcopper"));
   trdmod1_elvol->SetLineColor(kGreen);
   TGeoTranslation *trd_el_trans = new TGeoTranslation("", 0., 0., electronics_position);
   man->GetVolume(name)->AddNode(trdmod1_elvol, 0, trd_el_trans);


   // frame1
   TGeoBBox *trd_frame1 = new TGeoBBox("", Detector_size_x/2, Frame_width/2, frame_thickness);
   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("trd%dmod%dframe1", station, module_number), trd_frame1, man->GetMedium("G10"));
   trdmod1_frame1vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation *trd_frame1_trans = new TGeoTranslation("", 0., Active_area_y/2+Frame_width/2, frame_position);
   man->GetVolume(name)->AddNode(trdmod1_frame1vol, 1, trd_frame1_trans);
   trd_frame1_trans = new TGeoTranslation("", 0., -(Active_area_y/2+Frame_width/2), frame_position);
   man->GetVolume(name)->AddNode(trdmod1_frame1vol, 2, trd_frame1_trans);


   // frame2
   TGeoBBox *trd_frame2 = new TGeoBBox("", Frame_width/2, Active_area_y/2, frame_thickness);
   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("trd%dmod%dframe2", station, module_number), trd_frame2, man->GetMedium("G10"));
   trdmod1_frame2vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation *trd_frame2_trans = new TGeoTranslation("", Active_area_x/2+Frame_width/2, 0., frame_position);
   man->GetVolume(name)->AddNode(trdmod1_frame2vol, 1, trd_frame2_trans);
   trd_frame2_trans = new TGeoTranslation("", -(Active_area_x/2+Frame_width/2), 0., frame_position);
   man->GetVolume(name)->AddNode(trdmod1_frame2vol, 2, trd_frame2_trans);

   for (Int_t i=0; i<8; i++)
   {
     TGeoTranslation *trd_trans = new TGeoTranslation("", Position_Station[i][0], Position_Station[i][1], 0.);
     Int_t copynr = 1000 * layer + i;
     man->GetVolume("trd1")->AddNode(trdmod1_box, copynr, trd_trans);
//     TGeoTranslation *trd_trans = new TGeoTranslation("", Position_Station[0][0], Position_Station[0][1], 0.);
//     Int_t copynr = 1000 * layer + 1;
//     man->GetVolume("trd1")->AddNode(trdmod1_box, copynr, trd_trans);
   }
}

//void create_trd_feb(){}
//void create_trd_module(){}
