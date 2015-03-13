// 2012-11-01 - DE - added Sprossenfenster as CompositeShape

#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TFile.h"
#include "TString.h"
#include "TList.h"

#include <iostream>

const int withgasholes = 1; // 0;  // 0 = no holes, 1 = with holes

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

void Create_TRD_Geometry_v12sp() {

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
  Float_t Frame_width;         // Width of detector frames in cm   // root geometry uses cm as unit (was mm)
  Float_t Inner_radius[3];     // Inner acceptance in cm   // root geometry uses cm as unit (was mm)
  Float_t Outer_radius[3];     // Outer acceptance in cm   // root geometry uses cm as unit (was mm)

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
   //   top->Raytrace();

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

  Float_t radiator_thickness    =  25.000 /2.;  // 250 mm
  Float_t radiator_position     =  - Layer_thickness /2. + radiator_thickness;

  Float_t lattice_thickness     =   1.0000;  // 10 mm
  Float_t lattice_position      =  radiator_position + radiator_thickness + lattice_thickness/2.;

  Float_t gas_thickness         =   1.2000 /2.;  // 12 mm
  Float_t gas_position          =  lattice_position + lattice_thickness/2. + gas_thickness;
  Float_t padplane_thickness    =   0.0030 /2.;  // 30 my - in real life 0.036 cm (Optiprint) + 0.0025 copper
  Float_t padplane_position     =  gas_position + gas_thickness + padplane_thickness;
  Float_t mylar_thickness       =   0.1500 /2.;  // Muenster - Bucharest padplane??
  Float_t mylar_position        =  padplane_position + padplane_thickness + mylar_thickness;
  Float_t electronics_thickness =   0.0070 /2.;
  Float_t electronics_position  =  mylar_position + mylar_thickness + electronics_thickness;

  Float_t frame_thickness       =  gas_thickness + padplane_thickness + mylar_thickness + electronics_thickness;
  Float_t frame_position        =  - Layer_thickness /2. + radiator_thickness*2 + lattice_thickness + frame_thickness;

  // new order of material in v12g (Nov 2012)
  // 1. pefoam radiator               // known material: 25.0    cm - pefoam20
  // 2. lattice grid                  // known material:  1.0    cm - G10
  // 3. kapton foil                   // known material:  0.0025 cm - kapton
  // 4. gas volume                    // known material:  1.2    cm - TRDgas
  // 5. copper pads                   // known material:  0.0025 cm - copper
  // 6. pad plane                     // check material:  0.0360 cm - FR4 = G10 ??
  // 2.-6. surrounded by frame        // known material:  2.241  cm - G10
  // 6. backpanel                     // check alice   :  2.316  cm -  composite structure
  // 6.1 glue layer                   // alice         : 
  // 6.2 carbon layers	              // alice         : 
  // 6.3 honeycomb support structure  // alice         : 
  // 7. front-end electronics     // check alice   :  0.0486 cm  


  // from AliTRDgeometry.cxx -- start
  // material is defined in AliTRD.cxx

  //   // Polypropylene (C3H6) for radiator fibers
  //   Float_t app[2]    = { 12.011 ,  1.0079 };
  //   Float_t zpp[2]    = {  6.0   ,  1.0    };
  //   Float_t wpp[2]    = {  3.0   ,  6.0    };
  //   Float_t dpp       = 0.068;
  //   AliMixture(60,"Polypropylene",app,   zpp,   dpp,   -2,wpp   );

  // from media.geo
  // polypropylene      -2   12.01 1.008 6. 1. 0.074 3  6
  //                    0  0  20.  .001
  //                    0

  //   // Araldite, low density epoxy (C18H19O3)
  //   Float_t aAral[3]  = { 15.9994,  1.0079, 12.011  }; 
  //   Float_t zAral[3]  = {  8.0   ,  1.0   ,  6.0    }; 
  //   Float_t wAral[3]  = {  3.0   , 19.0   , 18.0    }; 
  //   Float_t dAral     = 1.12; // Hardener: 1.15, epoxy: 1.1, mixture: 1/2
  //   AliMixture(58,"Araldite",     aAral, zAral, dAral, -3,wAral );

  // new to put into media.geo
  // araldite           -3   15.9994 1.0079 12.011 8.0 1.0 6.0 1.12 3.0 19.0 18.0 
  //                    0  0  20.  .001
  //                    0

  //   Material layers inside sensitive area:
  //     Name    Description                     Mat.      Thick.   Dens.    Radl.    X/X_0
  //                                                        
  //     URMYxx  Mylar layers (x2)               Mylar     0.0015   1.39     28.5464  0.005%
  //     URCBxx  Carbon layer (x2)               Carbon    0.0055   1.75     24.2824  0.023%
  //     URGLxx  Glue on the carbon layers (x2)  Araldite  0.0065   1.12     37.0664  0.018%
  //     URRHxx  Rohacell layer (x2)             Rohacell  0.8      0.075    536.005  0.149%
  //     URFBxx  Fiber mat layer                 PP        3.186    0.068    649.727  0.490%
  //     
  //     UJxx    Drift region                    Xe/CO2    3.0      0.00495  1792.37  0.167%
  //     UKxx    Amplification region            Xe/CO2    0.7      0.00495  1792.37  0.039%
  //     UWxx    Wire planes (x2)                Copper    0.00011  8.96     1.43503  0.008%
  //
  //     UPPDxx  Copper of pad plane             Copper    0.0025   8.96     1.43503  0.174%
  //     UPPPxx  PCB of pad plane                G10       0.0356   2.0      14.9013  0.239%
  //     UPGLxx  Glue on pad planes              Araldite  0.0923   1.12     37.0664  0.249%
  //             + add. glue (ca. 600g)          Araldite  0.0505   1.12     37.0663  0.107%
  //     UPCBxx  Carbon fiber mats (x2)          Carbon    0.019    1.75     24.2824  0.078%
  //     UPHCxx  Honeycomb structure             Aramide   2.0299   0.032    1198.84  0.169%
  //     UPPCxx  PCB of readout board            G10       0.0486   2.0      14.9013  0.326%
  //     UPRDxx  Copper of readout board         Copper    0.0057   8.96     1.43503  0.404%
  //     UPELxx  Electronics + cables            Copper    0.0029   8.96     1.43503  0.202%
  // from AliTRDgeometry.cxx -- end


  Int_t module_number = (Int_t)Position_Station[0][2];
  
   TGeoBBox *trd_box = new TGeoBBox("", Detector_size_x/2, Detector_size_y/2, Layer_thickness/2);
   TGeoShape *trd_shape = trd_box;
   // Volume: trd1
   TString name;
   name.Form("trd%dmod%d", station, module_number);
   TGeoVolume* trdmod1_box = new TGeoVolume(name, trd_shape, man->GetMedium("air"));

   // Radiator
   //   TGeoBBox *trd_radiator = new TGeoBBox("", Active_area_x/2, Active_area_y/2, radiator_thickness);
   TGeoBBox *trd_radiator = new TGeoBBox("", Detector_size_x/2, Detector_size_y/2, radiator_thickness);
   TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd%dmod%dradiator", station, module_number), trd_radiator, man->GetMedium("pefoam20"));
//   TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd%dmod%dradiator", station, module_number), trd_radiator, man->GetMedium("polypropylene"));
   trdmod1_radvol->SetLineColor(kBlue);
   trdmod1_radvol->SetTransparency(70);
   TGeoTranslation *trd_radiator_trans = new TGeoTranslation("", 0., 0., radiator_position);
   man->GetVolume(name)->AddNode(trdmod1_radvol, 0, trd_radiator_trans);


   // Gas
   TGeoBBox *trd_gas = new TGeoBBox("", Active_area_x/2, Active_area_y/2, gas_thickness);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd%dmod%dgas", station, module_number), trd_gas, man->GetMedium("TRDgas"));
   TGeoTranslation *trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   trdmod1_gasvol->SetLineColor(kOrange);
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
   //   trdmod1_elvol->SetLineColor(kGreen);
   trdmod1_elvol->SetLineColor(kGray);
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


// DEDE -----------------------------------------

   Float_t Lattice_o_width   = 2.0; // 0.2;   // Width of outer lattice frame in cm
   Float_t Lattice_i_width   = 0.4; // 5.0;   // Width of inner lattice frame in cm
   //   Float_t lattice_thickness = 1.0;           // Thickness of lattice frames in cm
   //   lattice_thickness = 1.0;           // Thickness of lattice frames in cm


   // drift window - lattice grid - sprossenfenster
   TGeoBBox *trd_lattice_ho = new TGeoBBox("Sho", Detector_size_x/2, Lattice_o_width/2, lattice_thickness/2);  // horizontal
   TGeoBBox *trd_lattice_hi = new TGeoBBox("Shi", Detector_size_x/2, Lattice_i_width/2, lattice_thickness/2);  // horizontal

   TGeoBBox *trd_lattice_vo = new TGeoBBox("Svo", Lattice_o_width/2, Detector_size_x/2, lattice_thickness/2);  // vertical
   TGeoBBox *trd_lattice_vi = new TGeoBBox("Svi", Lattice_i_width/2, Detector_size_x/2, lattice_thickness/2);  // vertical

   //   TGeoBBox *trd_lattice2 = new TGeoBBox("Fr2", Frame_width/2, Active_area_y/2, frame_thickness);

   TGeoTranslation *t10 = new TGeoTranslation("t10", 0.,  (1.00*Active_area_y/2+Lattice_o_width/2), 0);
   t10->RegisterYourself();
   TGeoTranslation *t11 = new TGeoTranslation("t11", 0.,  (0.60*Active_area_y/2)                  , 0);
   t11->RegisterYourself();
   TGeoTranslation *t12 = new TGeoTranslation("t12", 0.,  (0.20*Active_area_y/2)                  , 0);
   t12->RegisterYourself();
   TGeoTranslation *t13 = new TGeoTranslation("t13", 0., -(0.20*Active_area_y/2)                  , 0);
   t13->RegisterYourself();
   TGeoTranslation *t14 = new TGeoTranslation("t14", 0., -(0.60*Active_area_y/2)                  , 0);
   t14->RegisterYourself();
   TGeoTranslation *t15 = new TGeoTranslation("t15", 0., -(1.00*Active_area_y/2+Lattice_o_width/2), 0);
   t15->RegisterYourself();

   TGeoTranslation *t20 = new TGeoTranslation("t20",  (1.00*Active_area_x/2+Lattice_o_width/2), 0., 0);
   t20->RegisterYourself();
   TGeoTranslation *t21 = new TGeoTranslation("t21",  (0.60*Active_area_x/2)                  , 0., 0);
   t21->RegisterYourself();
   TGeoTranslation *t22 = new TGeoTranslation("t22",  (0.20*Active_area_x/2)                  , 0., 0);
   t22->RegisterYourself();
   TGeoTranslation *t23 = new TGeoTranslation("t23", -(0.20*Active_area_x/2)                  , 0., 0);
   t23->RegisterYourself();
   TGeoTranslation *t24 = new TGeoTranslation("t24", -(0.60*Active_area_x/2)                  , 0., 0);
   t24->RegisterYourself();
   TGeoTranslation *t25 = new TGeoTranslation("t25", -(1.00*Active_area_x/2+Lattice_o_width/2), 0., 0);
   t25->RegisterYourself();

//   // with additional cross in the center - a la Roland
//   Float_t Lattice_1_width   = 0.1; // Width of inner lattice frame in cm
//   TGeoBBox *trd_lattice_h1 = new TGeoBBox("Sh1", Active_area_x/5/2, Lattice_1_width/2, lattice_thickness/2);  // horizontal
//   TGeoBBox *trd_lattice_v1 = new TGeoBBox("Sv1", Lattice_1_width/2, Active_area_x/5/2, lattice_thickness/2);  // vertical
//   TGeoCompositeShape *cs = new TGeoCompositeShape("cs", 
//   "(Sho:t10 + Shi:t11 + Shi:t12 + Shi:t13 + Shi:t14 + Sho:t15 + Svo:t20 + Svi:t21 + Svi:t22 + Svi:t23 + Svi:t24 + Svo:t25 + Sh1 + Sv1)");

   if (withgasholes)
   {
   // start gas inlets
   // gas hole (additive)
   float hole_irad = 0.75;
   float hole_orad = hole_irad + 1.0;
   float hole_xoff = hole_irad;
   float hole_yoff = hole_irad;
   float hole_ang1 = 0.0;
   float hole_ang2 = 360.0;

   // frame around gas hole
   float tube_irad = sqrt(2)*hole_irad + hole_irad;  // sqrt(2)*r+1
   float tube_orad = sqrt(2)*hole_irad + hole_orad;  // sqrt(2)*r+2
   float tube_xoff = 0.0;
   float tube_yoff = 0.0;

   TGeoTubeSeg *trd_corner_01 = new TGeoTubeSeg("Co1", tube_irad, tube_orad, lattice_thickness/2, 0.0, 90.0);
   TGeoTranslation *t30 = new TGeoTranslation("t30", -(1.0*Active_area_x/2-tube_xoff), -(1.0*Active_area_y/2-tube_yoff), 0);
   t30->RegisterYourself();

   TGeoTubeSeg *trd_hole_01 = new TGeoTubeSeg("Ho1", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
   TGeoTranslation *t31 = new TGeoTranslation("t31", -(1.0*Active_area_x/2-hole_xoff), -(1.0*Active_area_y/2-hole_yoff), 0);
   t31->RegisterYourself();

   TGeoTubeSeg *trd_corner_02 = new TGeoTubeSeg("Co2", tube_irad, tube_orad, lattice_thickness/2, 90.0, 180.0);
   TGeoTranslation *t32 = new TGeoTranslation("t32", +(1.0*Active_area_x/2-tube_xoff), -(1.0*Active_area_y/2-tube_yoff), 0);
   t32->RegisterYourself();

   TGeoTubeSeg *trd_hole_02 = new TGeoTubeSeg("Ho2", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
   TGeoTranslation *t33 = new TGeoTranslation("t33", +(1.0*Active_area_x/2-hole_xoff), -(1.0*Active_area_y/2-hole_yoff), 0);
   t33->RegisterYourself();

   TGeoTubeSeg *trd_corner_03 = new TGeoTubeSeg("Co3", tube_irad, tube_orad, lattice_thickness/2, 180.0, 270.0);
   TGeoTranslation *t34 = new TGeoTranslation("t34", +(1.0*Active_area_x/2-tube_xoff), +(1.0*Active_area_y/2-tube_yoff), 0);
   t34->RegisterYourself();

   TGeoTubeSeg *trd_hole_03 = new TGeoTubeSeg("Ho3", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
   TGeoTranslation *t35 = new TGeoTranslation("t35", +(1.0*Active_area_x/2-hole_xoff), +(1.0*Active_area_y/2-hole_yoff), 0);
   t35->RegisterYourself();

   TGeoTubeSeg *trd_corner_04 = new TGeoTubeSeg("Co4", tube_irad, tube_orad, lattice_thickness/2, 270.0, 360.0);
   TGeoTranslation *t36 = new TGeoTranslation("t36", -(1.0*Active_area_x/2-tube_xoff), +(1.0*Active_area_y/2-tube_yoff), 0);
   t36->RegisterYourself();

   TGeoTubeSeg *trd_hole_04 = new TGeoTubeSeg("Ho4", hole_irad, hole_orad, lattice_thickness/2, hole_ang1, hole_ang2);
   TGeoTranslation *t37 = new TGeoTranslation("t37", -(1.0*Active_area_x/2-hole_xoff), +(1.0*Active_area_y/2-hole_yoff), 0);
   t37->RegisterYourself();
   // end gas inlets

   TGeoCompositeShape *lattice_grid = new TGeoCompositeShape("lattice_grid", 
   "(Sho:t10 + Shi:t11 + Shi:t12 + Shi:t13 + Shi:t14 + Sho:t15 + Svo:t20 + Svi:t21 + Svi:t22 + Svi:t23 + Svi:t24 + Svo:t25\
   + Co1:t30 + Co2:t32 + Co3:t34 + Co4:t36 + Ho1:t31 + Ho2:t33 + Ho3:t35 + Ho4:t37)");
   }
   else  // no gas holes in lattice grid
     TGeoCompositeShape *lattice_grid = new TGeoCompositeShape("lattice_grid", 
     "(Sho:t10 + Shi:t11 + Shi:t12 + Shi:t13 + Shi:t14 + Sho:t15 + Svo:t20 + Svi:t21 + Svi:t22 + Svi:t23 + Svi:t24 + Svo:t25)");

   TGeoVolume *lattice = new TGeoVolume(Form("trd%dmod%dgrid1", station, module_number), lattice_grid, man->GetMedium("G10"));
   lattice->SetLineColor(kYellow);

   //   TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., -Layer_thickness /2.-lattice_thickness/2);
   TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
   man->GetVolume(name)->AddNode(lattice, 1, trd_lattice_trans);

// DEDE -----------------------------------------

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
