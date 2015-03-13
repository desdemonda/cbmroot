///                                             
/// \file Create_TOF_Geometry_v13_4x.C
/// \brief Generates TOF geometry in Root format.
///  

// Changelog
//
// 2014-06-30 - NH - prepare tof_v14_0  geometry - SIS 300 hadron  : TOF_Z_Front =  880 cm //Bucharest 
// 2014-06-27 - NH - prepare tof_v13_6b geometry - SIS 300 hadron  : TOF_Z_Front =  880 cm //external input
// 2013-10-16 - DE - prepare tof_v13_5a geometry - SIS 100 hadron  : TOF_Z_Front =  450 cm
// 2013-10-16 - DE - prepare tof_v13_5b geometry - SIS 100 electron: TOF_Z_Front =  600 cm
// 2013-10-16 - DE - prepare tof_v13_5c geometry - SIS 100 muon    : TOF_Z_Front =  650 cm
// 2013-10-16 - DE - prepare tof_v13_5d geometry - SIS 300 electron: TOF_Z_Front =  880 cm
// 2013-10-16 - DE - prepare tof_v13_5e geometry - SIS 300 muon    : TOF_Z_Front = 1020 cm
// 2013-10-16 - DE - patch pole_alu bug - skip 0 thickness air volume in pole
// 2013-09-04 - DE - prepare tof_v13_4a geometry - SIS 100 hadron  : TOF_Z_Front =  450 cm
// 2013-09-04 - DE - prepare tof_v13_4b geometry - SIS 100 electron: TOF_Z_Front =  600 cm
// 2013-09-04 - DE - prepare tof_v13_4c geometry - SIS 100 muon    : TOF_Z_Front =  650 cm
// 2013-09-04 - DE - prepare tof_v13_4d geometry - SIS 300 electron: TOF_Z_Front =  880 cm
// 2013-09-04 - DE - prepare tof_v13_4e geometry - SIS 300 muon    : TOF_Z_Front = 1020 cm
// 2013-09-04 - DE - dump z-positions to .geo.info file
// 2013-09-04 - DE - define front z-position of TOF wall (TOF_Z_Front)
// 2013-09-04 - DE - fix arrangement of glass plates in RPC cells 

// in root all sizes are given in cm
// read positions of modules from dat - file 

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
#include "TROOT.h"

#include <iostream>
#include <sstream>

// Name of geometry version and output file
const TString geoVersion = "tof_v14-0a";     //6 m
//const TString geoVersion = "tof_v14-0b";   //10 m
//const TString geoVersion = "tof_v13-6b";   //10 m
//const TString geoVersion = "tof_v13-5b";
//const TString geoVersion = "tof_v13-5c";
//const TString geoVersion = "tof_v13-5d";
//const TString geoVersion = "tof_v13-5e";
const TString FileNameSim  = geoVersion + ".geo.root";
const TString FileNameGeo  = geoVersion + "_geo.root";
const TString FileNameInfo = geoVersion + ".geo.info";

// TOF_Z_Front corresponds to front cover of outer super module towers
const Float_t TOF_Z_Front =  450;  // SIS 100 hadron
//const Float_t TOF_Z_Front =  600;  // SIS 100 electron
//const Float_t TOF_Z_Front =  650;  // SIS 100 muon
//const Float_t TOF_Z_Front =  880;  // SIS 300 electron
//const Float_t TOF_Z_Front = 1020;  // SIS 300 muon
//
//const Float_t TOF_Z_Front = 951.5;   // Wall_Z_Position = 1050 cm


// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString BoxVolumeMedium         = "aluminium";
const TString NoActivGasMedium        = "RPCgas_noact";
const TString ActivGasMedium          = "RPCgas";
const TString GlasMedium              = "RPCglass";
const TString ElectronicsMedium       = "carbon";

const Int_t NumberOfDifferentCounterTypes = 4;
const Float_t Glass_X[NumberOfDifferentCounterTypes] = {32., 32., 30.0,30.0};
const Float_t Glass_Y[NumberOfDifferentCounterTypes] = {26.9, 53.,20.,10.};
const Float_t Glass_Z[NumberOfDifferentCounterTypes] = {0.1,0.1,0.1,0.1};

const Float_t GasGap_X[NumberOfDifferentCounterTypes] = {32.,32.,30.0,30.0};
const Float_t GasGap_Y[NumberOfDifferentCounterTypes] = {26.9,53.,20.,10.};
const Float_t GasGap_Z[NumberOfDifferentCounterTypes] = {0.025,0.025,0.025,0.025};

const Int_t NumberOfGaps[NumberOfDifferentCounterTypes] = {8,8,8,8};
//const Int_t NumberOfGaps[NumberOfDifferentCounterTypes] = {1,1,1,1}; //deb
const Int_t NumberOfReadoutStrips[NumberOfDifferentCounterTypes] = {32,32,64,64};
//const Int_t NumberOfReadoutStrips[NumberOfDifferentCounterTypes] = {1,1,1,1}; //deb

const Float_t SingleStackStartPosition_Z[NumberOfDifferentCounterTypes] = {-0.6,-0.6,-0.6,-0.6};

const Float_t Electronics_X[NumberOfDifferentCounterTypes] = {34.0,34.0,32.0,32.};
const Float_t Electronics_Y[NumberOfDifferentCounterTypes] = {5.0,5.0,0.5,0.5};
const Float_t Electronics_Z[NumberOfDifferentCounterTypes] = {0.3,0.3,0.3,0.3};

const Int_t NofModuleTypes = 6;
const Int_t MaxNofModules =500;
Int_t ActNofModuleTypes=2;
Int_t NModules[NofModuleTypes]=0;
Float_t xPosMod[NofModuleTypes][MaxNofModules];
Float_t yPosMod[NofModuleTypes][MaxNofModules];
Float_t zPosMod[NofModuleTypes][MaxNofModules];
Float_t FlipMod[NofModuleTypes][MaxNofModules];

const Int_t NMm=4;
const Int_t NMTm=3;
// modules are placed in master reference frame (0,0,0) is the target position 
// x - horizontal, y vertical, z along beam
// all number for the 6m position  

Float_t xPosModm[NMTm][NMm] = {{0.,0.,0.,0.},             //x - coordinates of center of M1 modules
			       {129.5,-129.5,0.,0.},      //x - coordinates of center of M2 modules 
			       {146.,-146.,146.,-146.}};  //x - coordinates of center of M3 modules
Float_t yPosModm[NMTm][NMm] = {{-92.1,92.1,0.,0.},        //y - coordinates of center of M1 modules
			       {0.,0.,0.,0.},             //y - coordinates of center of M2 modules
			       {-92.1,-92.1,92.1,92.1}};      //y - coordinates of center of M3 modules

// original z- positions
/*
Float_t zPosModm[NMTm][NMm] = {{608.32,608.32,0.,0.},       //z - coordinates of center of M1 modules
			       {632.22,632.22,0.,0.},        //z - coordinates of center of M2 modules
			       {656.12,656.12,656.12,656.12}};//z - coordinates of center of M3 modules
*/
// inverted M1/M2 positions

Float_t zPosModm[NMTm][NMm] = {{632.22,632.22,0.,0.},        //z - coordinates of center of M1 modules
			       {608.32,608.32,0.,0.},        //z - coordinates of center of M2 modules
			       {656.12,656.12,656.12,656.12}};//z - coordinates of center of M3 modules

Float_t FlipModm[NMTm][NMm] = {{0.,180.,0.,0.},           //M1 rotation angle with respect to normal of surface
			       {0.,180.,0.,0.},           //M2 rotation angle
			       {0.,0.,0.,0.}};        //M3 rotation angle

/*
Float_t xPosModm[NMTm][NMm] = {{0.,0.,0.,0.},            //M1 coordinates
			       {132.8,-132.8,0.,0.},     //M2 coordinates
			       {140.,140.,-140.,-140.}}; //M3 coordinates
Float_t yPosModm[NMTm][NMm] = {{89.,-89.,0.,0.},         //M1 coordinates
			       {0.,0.,0.,0.},            //M2 coordinates
			       {89.,-89.,89.,-89.}};     //M3 coordinates
Float_t zPosModm[NMTm][NMm] = {{1082.,1082.,0.,0.},      //M1 coordinates
			       {1045.,1045,0.,0.},       //M2 coordinates
			       {1015.,1015.,1015.,1015.}};//M3 coordinates
Float_t FlipModm[NMTm][NMm] = {{0.,180.,0.,0.},           //M1 angle
			       {0.,180.,0.,0.},           //M2 angle
			       {0.,0.,180.,180.}};        //M3 angle

const Float_t CounterXstart1[NMTm] = {-44.,-27.,-64.}; 
const Float_t CounterYstart1[NMTm] = {-37.,-19.,-55.}; 
const Float_t CounterDyoff1[NMTm][2] = {{3.,-3.},
					{-3.,3.},
					{3.,-3.}}; 
const Float_t CounterDzoff1[NMTm][2] = {{5.,-5.},
					{-5.,5.},
					{5.,-5.}}; 
const Float_t CounterDxpos1[NMTm] = {30.,32.,31.}; 
const Float_t CounterDypos1[NMTm] = {18.,18.,18.}; 

const Int_t NCounter2Y[NMTm]       = {2,6,0}; 
const Float_t CounterXstart2[NMTm] = {-44.,-85.,-62.}; 
const Float_t CounterYstart2[NMTm] = {-59.,-23.,-55.}; 
const Float_t CounterDyoff2[NMTm][2] = {{ 3.,-3.},
					{-3.,3.},
					{3.,-3.}}; 
const Float_t CounterDzoff2[NMTm][2] = {{-5.,5.},
					{-5.,5.},
					{5.,-5.}}; 
const Float_t CounterDxpos2[NMTm] = {30.,30.,30.}; 
const Float_t CounterDypos2[NMTm] = { 8.,9.,8.}; 
*/

//*************************************************************
const Float_t xPosCounter1[32] = {43.5,14.5,-14.5,-43.5,43.5,14.5,-14.5,-43.5,43.5,14.5,-14 .5,-43.5,43.5,14.5,-14.5,-43.5,
43.5,14.5,-14.5,-43.5,43.5,14.5,-14.5,-43.5,43.5,14.5,-14.5,-43.5,43.5,14.5,-14.5,-43.5};
const Float_t yPosCounter1[32] = {-54.6,-54.6,-54.6,-54.6,-36.4,-36.4,-36.4,-36.4,-18.2,-18.2,-18.2,-18.2,0.,0.,0.,0.,18.2,18.2,18.2,18.2,
				  36.4,36.4,36.4,36.4,50.5,50.5,50.5,50.5,59.6,59.6,59.6,59.6};
const Float_t zPosCounter1[32] = {-3.01,-7.55,-3.01,-7.55,6.07,1.53,6.07,1.53,-3.01,-7.55,-3.01,-7.55,6.07,1.53,6.07,1.53,
-3.01,-7.55,-3.01,-7.55,6.07,1.53,6.07,1.53,-3.01,-7.55,-3.01,-7.55,6.07,1.53,6.07,1.53};
/*
const Float_t xPosCounter2[27] = {87.,58.,29.,0.,-29.,-58.,-87.,-58.,-87.,87.,58.,29.,0.,-29.,-58.,-87.,-58.,-87.,
87.,58.,29.,0.,-29.,-58.,-87.,-58.,-87.};
const Float_t yPosCounter2[27] = {-19.,-19.,-19.,-19.,-19.,-24.,-24.,-14.4,-14.4,0.,0.,0.,0.,0.,-4.8,-4.8,4.8,4.8,
19.,19.,19.,19.,19.,14.4,14.4,24.,24.};
const Float_t zPosCounter2[27] = {-7.55,-3.01,-7.55,-3.01,-7.55,-3.01,-7.55,6.07,1.53,1.53,6.07,1.53,6.07,1.53,
-3.01,-7.55,6.07,1.53,-7.55,-3.01,-7.55,-3.01,-7.55,-3.01,-7.55,6.07,1.53};
*/
// rearranged: first all large, than the small counters 
const Float_t xPosCounter2[27] = {87.,58.,29.,0.,-29.,87.,58.,29.,0.,-29.,87.,58.,29.,0.,-29.,
-58.,-87.,-58.,-87.,-58.,-87.,-58.,-87.,-58.,-87.,-58.,-87.};
const Float_t yPosCounter2[27] = {-19.,-19.,-19.,-19.,-19.,0.,0.,0.,0.,0.,19.,19.,19.,19.,19.,
-24.,-24.,-14.4,-14.4,-4.8,-4.8,4.8,4.8,14.4,14.4,24.,24.};
const Float_t zPosCounter2[27] = {-7.55,-3.01,-7.55,-3.01,-7.55,1.53,6.07,1.53,6.07,1.53,-7.55,-3.01,-7.55,-3.01,-7.55,
-3.01,-7.55,6.07,1.53,-3.01,-7.55,6.07,1.53,-3.01,-7.55,6.07,1.53};

const Float_t xPosCounter3[42] = {72.5,43.5,14.5,-14.5,-43.5,-72.5,72.5,43.5,14.5,-14.5,-43.5,-72.5,72.5,43.5,14.5,-14.5,-43.5,-72.5,
72.5,43.5,14.5,-14.5,-43.5,-72.5,72.5,43.5,14.5,-14.5,-43.5,-72.5,72.5,43.5,14.5,-14.5,-43.5,-72.5,72.5,43.5,14.5,-14.5,-43.5,-72.5};
const Float_t yPosCounter3[42] = {-54.6,-54.6,-54.6,-54.6,-54.6,-54.6,-34.6,-34.6,-34.6,-34.6,-34.6,-34.6,-18.2,-18.2,-18.2,-18.2,-18.2,-18.2,
0.,0.,0.,0.,0.,0.,18.2,18.2,18.2,18.2,18.2,18.2,34.6,34.6,34.6,34.6,34.6,34.6,54.6,54.6,54.6,54.6,54.6,54.6};
const Float_t zPosCounter3[42] = {-7.55,-3.01,-7.55,-3.01,-7.55,-3.01,1.53,6.07,1.53,6.07,1.53,6.07,-7.55,-3.01,-7.55,-3.01,-7.55,-3.01,
1.53,6.07,1.53,6.07,1.53,6.07,-7.55,-3.01,-7.55,-3.01,-7.55,-3.01,1.53,6.07,1.53,6.07,1.53,6.07,-7.55,-3.01,-7.55,-3.01,-7.55,-3.01};

//  trebuie recalculat pt a da centrul detectorului, nu coltul lui
const Int_t NCounter1Y[NMTm]       = {6,3,7}; 
const Float_t CounterXstart1[NMTm] = {-43.5,87.,72.5};  // position of left most counter in x - direction (for un-rotated modules)
const Float_t CounterYstart1[NMTm] = {-54.6,-19.,-54.6};  // position of lowest counter in y direction (for un-rotated modules)
const Float_t CounterDyoff1[NMTm][2] = {{-4.54,4.54},       // staggering in z direction due to row number  
		    			{4.54,-4.54},
					{4.54,-4.54.}}; 
const Float_t CounterDzoff1[NMTm][2] = {{4.54,-4.54},       // staggering in z direction due to column number 
					{4.54,-4.54},
					{4.54,-4.54}}; 
const Float_t CounterDxpos1[NMTm] = {29.,29.,29.};      // distance of counters in x - direction 
const Float_t CounterDypos1[NMTm] = {18.2.,19.,18.2};      // distance of counters in y - direction

const Int_t NCounter2Y[NMTm]       = {2,6,0}; //number of counters of 100 mm strip length in a column within each module type

const Float_t CounterXstart2[NMTm] = {-43.5,-58.,0.}; 
const Float_t CounterYstart2[NMTm] = {50.5,-24.,0.}; 
const Float_t CounterDyoff2[NMTm][2] = {{-4.54,4.54.},
					{-4.54,4.54},
				      	{0.,0.}}; 
const Float_t CounterDzoff2[NMTm][2] = {{4.54,-4.54},
				      	{4.54,-4.54},		    
					{0.,0.}}; 
const Float_t CounterDxpos2[NMTm] = {29.,29.,29.}; 
const Float_t CounterDypos2[NMTm] = { 9.1,9.6,0.}; 

// Aluminum box for all module types
//m,s,b,m1,m2,m3
const Float_t Module_Size_X[NofModuleTypes] = {180.2,180.2,180.2,127.0,214.0,185.0};
const Float_t Module_Size_Y[NofModuleTypes] = {49.,49.,74.,141.7,70.5,141.7};
const Float_t Module_Over_Y[NofModuleTypes] = {11.5,11.5,11.,4.5,4.5,5.1};
const Float_t Module_Size_Z[NofModuleTypes] = {10.,10.,10.,23.9,23.9,23.9};
const Float_t Module_Thick_Alu_X_left = 1.;
const Float_t Module_Thick_Alu_X_right = 0.1;
const Float_t Module_Thick_Alu_Y = 0.1;
const Float_t Module_Thick_Alu_Z = 0.1;

const Float_t shift_gas_box = (Module_Thick_Alu_X_right - Module_Thick_Alu_X_left)/2;

// Distance to the center of the TOF wall [cm];
// for 0m 
/*
const Float_t InnerWall_Z_PositionShift = -600.;  // inner wall placed at 600
const Float_t Wall_Z_PositionShift      = -998.;  // outer wall placed at 1000
*/

// for 6m 
const Float_t InnerWall_Z_PositionShift = 0.;    // -600.;  // inner wall placed at 600
const Float_t Wall_Z_PositionShift      = -425.; // -998.;  // outer wall placed at 1000
/*
// for 10 m
const Float_t InnerWall_Z_PositionShift = 400.;    // -600.;  // inner wall placed at 600
const Float_t Wall_Z_PositionShift      = -25.; // -998.;  // outer wall placed at 1000
*/
const Float_t Wall_Z_Position = TOF_Z_Front + 0.5 * Module_Size_Z[0] + 3.5 * Module_Size_Z[1] + 4.5 * Module_Size_Z[2];
//const Float_t Wall_Z_Position = TOF_Z_Front + 98.5;  // corresponds to center of front module in the inner tower
//const Float_t Wall_Z_Position = 1050;  // corresponds to center of front module in the inner tower
                                       //
                                       // TOF z front = Wall_Z_Position - 0.5 * Module_Size_Z[0] 
                                       //                               - 3.5 * Module_Size_Z[1]
                                       //                               - 4.5 * Module_Size_Z[2] 
                                       //             = 1050 - 4.0 * 10 - 4.5 * 13 = 951.5 cm
                                       //
                                       // TOF z back  = Wall_Z_Position + 1.5 * Module_Size_Z[0]
                                       //             = 1050 + 1.5 * 10 = 1065

//Type of Counter for module 
const Int_t CounterTypeInModule[NofModuleTypes] = {0,0,1,23,23,23};
const Int_t NCounterInModule[NofModuleTypes]    = {5,5,5,2408,1512,4200};

// Placement of the counter inside the module
const Float_t CounterXStartPosition[NofModuleTypes] = {-60.0, -67.02, -67.02,-60.0,-60.0,-16.0};
const Float_t CounterXDistance[NofModuleTypes]      = {30.0,   30.351, 30.351, 30.0, 30.0, 30.0};
const Float_t CounterZDistance[NofModuleTypes]      = {2.5,     0.0,   0.0,  2.5,  2.5,  2.5};
const Float_t CounterRotationAngle[NofModuleTypes]  = {0.,      8.7,   8.7,  0.,   0.,   0.};

// Pole (support structure)
const Int_t MaxNumberOfPoles=200;
Float_t Pole_ZPos[MaxNumberOfPoles];
Float_t Pole_XPos[MaxNumberOfPoles];
Float_t Pole_Col[MaxNumberOfPoles];
Int_t NumberOfPoles=0;

const Float_t Pole_Size_X =  8.;
const Float_t Pole_Size_Y = 1000.;
const Float_t PoleShort_Size_Y = 370.;
const Float_t Pole_Size_Z =  2.;
const Float_t Pole_Thick_X = 0.4;
const Float_t Pole_Thick_Y = 0.4;
const Float_t Pole_Thick_Z = 0.4;
const Float_t XLimInner=180.;


// Bars (support structure)
const Float_t Bar_Size_X = 20.;
const Float_t Bar_Size_Y = 20.;
Float_t Bar_Size_Z = 100.;
const Float_t Bar_XLen = 1400;
const Float_t Bar_YLen = Pole_Size_Y + 2.*Bar_Size_Y;
Float_t Bar_Pos_Z;

const Int_t MaxNumberOfBars=200;
Float_t Bar_ZPos[MaxNumberOfBars];
Float_t Bar_XPos[MaxNumberOfBars];
Int_t NumberOfBars=0;

const Float_t ChamberOverlap=40;
const Float_t DxColl=158.0; //Module_Size_X-ChamberOverlap;
//const Float_t Pole_Offset=Module_Size_X/2.+Pole_Size_X/2.;
const Float_t Pole_Offset=90.0+Pole_Size_X/2.;

// Position for module placement
const Float_t Inner_Module_First_Y_Position=16.;
const Float_t Inner_Module_Last_Y_Position=480.;
const Float_t Inner_Module_X_Offset=2.;
const Float_t Inner_Module_NTypes = 3;
const Float_t Inner_Module_Types[Inner_Module_NTypes]  = {4.,3.,0.};
const Float_t Inner_Module_Number[Inner_Module_NTypes] = {2.,2.,6.}; //V13_3a
//const Float_t Inner_Module_Number[Inner_Module_NTypes] = {0.,0.,0.}; //debugging

const Float_t InnerSide_Module_X_Offset=51.;
const Float_t InnerSide_Module_NTypes = 1;
const Float_t InnerSide_Module_Types[Inner_Module_NTypes]  = {5.};
const Float_t InnerSide_Module_Number[Inner_Module_NTypes] = {2.};  //v13_3a
//const Float_t InnerSide_Module_Number[Inner_Module_NTypes] = {0.};  //debug

const Float_t Outer_Module_First_Y_Position=0.;
const Float_t Outer_Module_Last_Y_Position=480.;
const Float_t Outer_Module_X_Offset=3.;
const Float_t Outer_Module_Col = 4;
const Float_t Outer_Module_NTypes = 2;
const Float_t Outer_Module_Types [Outer_Module_NTypes][Outer_Module_Col] = {1.,1.,1.,1.,  2.,2.,2.,2.};
const Float_t Outer_Module_Number[Outer_Module_NTypes][Outer_Module_Col] = {9.,9.,2.,0.,  0.,0.,3.,4.};//V13_3a
//const Float_t Outer_Module_Number[Outer_Module_NTypes][Outer_Module_Col] = {1.,1.,0.,0.,  0.,0.,0.,0.};//debug

// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume* gModules[NofModuleTypes]; // Global storage for module types
TGeoVolume* gCounter[NumberOfDifferentCounterTypes];
TGeoVolume* gPole;
TGeoVolume* gPoleShort;
TGeoVolume* gBar[MaxNumberOfBars];

Float_t Last_Size_Y=0.;
Float_t Last_Over_Y=0.;

// Forward declarations
void create_materials_from_media_file();
TGeoVolume* create_counter(Int_t);
TGeoVolume* create_new_counter(Int_t);
TGeoVolume* create_tof_module(Int_t);
TGeoVolume* create_tof_module_m(Int_t);
TGeoVolume* create_new_tof_module(Int_t);
TGeoVolume* create_tof_pole();
TGeoVolume* create_tof_bar();
void position_tof_poles(Int_t);
void position_tof_bars(Int_t);
void position_inner_tof_modules(Int_t);
void position_side_tof_modules(Int_t);
void position_outer_tof_modules(Int_t);
void position_tof_modules(Int_t);
void position_tof_modules_m(Int_t,Int_t);
void dump_info_file();
void read_module_positions();

void Create_TOF_Geometry_v14_0() {
  // Load the necessary FairRoot libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  // read input Data
  read_module_positions();
  // Load needed material definition from media.geo file
  create_materials_from_media_file();

  // Get the GeoManager for later usage
  gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
  gGeoMan->SetVisLevel(7);  // 2 = super modules   
  gGeoMan->SetVisOption(1);  

  // Create the top volume 
  /*
  TGeoBBox* topbox= new TGeoBBox("", 1000., 1000., 1000.);
  TGeoVolume* top = new TGeoVolume("top", topbox, gGeoMan->GetMedium("air"));
  gGeoMan->SetTopVolume(top);
  */

  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
 
  TGeoVolume* tof = new TGeoVolumeAssembly(geoVersion);
  top->AddNode(tof, 1);
  
  for(Int_t counterType = 0; counterType < NumberOfDifferentCounterTypes; counterType++) { 
    gCounter[counterType] = create_new_counter(counterType);
  }

  for(Int_t moduleType = 0; moduleType < NofModuleTypes; moduleType++) { 
    gModules[moduleType] = create_new_tof_module(moduleType);
    gModules[moduleType]->SetVisContainers(1); 
  }  

  gPole = create_tof_pole();
  gPoleShort = create_tof_poleshort();

  position_tof_modules(2);
  position_tof_modules_m(3,5); // Bucharest modules 
  //position_tof_modules_m(3,3); // Bucharest modules 
  /*
  position_side_tof_modules(1);  // keep order !!
  position_inner_tof_modules(3);
  cout << "Outer Types "<<Outer_Module_Types[0][0]<<", "<<Outer_Module_Types[1][0]
       <<", col=1:  "<<Outer_Module_Types[0][1]<<", "<<Outer_Module_Types[1][1]
       <<endl;
  cout << "Outer Number "<<Outer_Module_Number[0][0]<<", "<<Outer_Module_Number[1][0]
       <<", col=1:  "<<Outer_Module_Number[0][1]<<", "<<Outer_Module_Number[1][1]
       <<endl;
  position_outer_tof_modules(4);
  */
  position_tof_poles(0);
  position_tof_bars(0);
  
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* outfile = new TFile(FileNameSim,"RECREATE");
  top->Write();
  //gGeoMan->Write();
  outfile->Close();

  TFile* outfile = new TFile(FileNameGeo,"RECREATE");
  gGeoMan->Write();
  outfile->Close();

  dump_info_file();

  top->SetVisContainers(1); 
  gGeoMan->SetVisLevel(5); 
  top->Draw("ogl");
  //top->Draw();
  //gModules[0]->Draw("ogl");
  //  gModules[0]->Draw("");
  gModules[0]->SetVisContainers(1); 
  //  gModules[1]->Draw("");
  gModules[1]->SetVisContainers(1); 
  //gModules[5]->Draw("");
  //  top->Raytrace();

}

void read_module_positions()
{
  //TFile * fPosInput = new TFile( "TOF_10M.dat", "READ");
ifstream inFile;
inFile.open ("TOF_10M.dat");
if(!inFile.is_open()){
  cout<<"<E> cannot open input file "<<endl; 
  return;
}

 std::string strdummy;
 std::getline(inFile,strdummy);
 cout<<strdummy<<endl;
 Int_t iNum;
 Int_t iX;
 Int_t iY;
 Int_t iZ;
 Char_t cMod;
 Char_t cPos;
 Int_t iModType=0;
 //while( !inFile.eof()  )
 //for(Int_t iL=0; iL<2; iL++)
 while( std::getline(inFile,strdummy) )
 {
   //  std::getline(inFile,strdummy);
  cout<<strdummy<<endl;
  stringstream ss;
  ss<<strdummy;
  ss>>iNum>>iX>>iY>>iZ>>cMod>>cPos;
  ss<<strdummy;
  //  ss>>iNum>>iX>>iY>>iZ>>cType[0]>>cType[1];
  Char_t cType[2];
    cType[0]=cMod;
    cType[1]=cPos;

    //    cout<<iNum<<" "<<iX<<" "<<iY<<" "<<iZ<<" "<<cMod<<" "<<cPos<<" "<<cType<<endl;

  switch(cMod){
  case 'b':
   iModType=2;
   break;
  case 's':
   iModType=1;
   break;
  default: 
    cout <<" case "<< cMod <<" not implemented"<<endl;
  }
  if(cPos=='m') iModType=0;

  //cout<<" ModType "<<iModType<<endl;
  Int_t iMod=NModules[iModType]++;
  //cout<<" ModType "<<iModType<<", # "<<iMod<<endl;

  xPosMod[iModType][iMod]=(Double_t)iX/10.;
  yPosMod[iModType][iMod]=(Double_t)iY/10.;
  zPosMod[iModType][iMod]=(Double_t)iZ/10. + Wall_Z_PositionShift;
  if(cPos=='l'){
   FlipMod[iModType][iMod]=1.;
  }else{
   FlipMod[iModType][iMod]=0.;
  }
  //  if (iModType==1 && iMod==1) return; 
  /*
  cout<<" ModType "<<iModType<<", Mod "<<iMod<<", x "<<xPosMod[iModType][iMod]<<", y "
      <<yPosMod[iModType][iMod]<<", z "<<zPosMod[iModType][iMod]<<endl;
  */
 }
 cout <<"Data reading finished for "<<NModules[0]<<" modules of type 0, "<<NModules[1]<<" of type 1, "<<NModules[2]<<" of type 2  "<< endl;
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
  FairGeoMedium* aluminium        = geoMedia->getMedium("aluminium");
  FairGeoMedium* RPCgas           = geoMedia->getMedium("RPCgas");
  FairGeoMedium* RPCgas_noact     = geoMedia->getMedium("RPCgas_noact");
  FairGeoMedium* RPCglass         = geoMedia->getMedium("RPCglass");
  FairGeoMedium* carbon           = geoMedia->getMedium("carbon");

  // include check if all media are found

  geoBuild->createMedium(air);
  geoBuild->createMedium(aluminium);
  geoBuild->createMedium(RPCgas);
  geoBuild->createMedium(RPCgas_noact);
  geoBuild->createMedium(RPCglass);
  geoBuild->createMedium(carbon);
}

TGeoVolume* create_counter(Int_t modType)
{

  //glass
  Float_t gdx=Glass_X[modType]; 
  Float_t gdy=Glass_Y[modType];
  Float_t gdz=Glass_Z[modType];

  //gas gap
  Int_t  nstrips=NumberOfReadoutStrips[modType];
  Int_t  ngaps=NumberOfGaps[modType];


  Float_t ggdx=GasGap_X[modType];  
  Float_t ggdy=GasGap_Y[modType];
  Float_t ggdz=GasGap_Z[modType];
  Float_t gsdx=ggdx/float(nstrips);

  //single stack
  Float_t dzpos=gdz+ggdz;
  Float_t startzpos=SingleStackStartPosition_Z[modType];

  // electronics
  //pcb dimensions 
  Float_t dxe=Electronics_X[modType]; 
  Float_t dye=Electronics_Y[modType];
  Float_t dze=Electronics_Z[modType];
  Float_t yele=(gdy+0.1)/2.+dye/2.;
 
  // needed materials
  TGeoMedium* glassPlateVolMed   = gGeoMan->GetMedium(GlasMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);
  TGeoMedium* activeGasVolMed    = gGeoMan->GetMedium(ActivGasMedium);
  TGeoMedium* electronicsVolMed  = gGeoMan->GetMedium(ElectronicsMedium);

  // Single glass plate
  TGeoBBox* glass_plate = new TGeoBBox("", gdx/2., gdy/2., gdz/2.);
  TGeoVolume* glass_plate_vol = 
    new TGeoVolume("tof_glass", glass_plate, glassPlateVolMed);
  glass_plate_vol->SetLineColor(kMagenta); // set line color for the glass plate
  glass_plate_vol->SetTransparency(20); // set transparency for the TOF
  TGeoTranslation* glass_plate_trans 
    = new TGeoTranslation("", 0., 0., 0.);

  // Single gas gap
  TGeoBBox* gas_gap = new TGeoBBox("", ggdx/2., ggdy/2., ggdz/2.);
  //TGeoVolume* gas_gap_vol = 
  //new TGeoVolume("tof_gas_gap", gas_gap, noActiveGasVolMed);
  TGeoVolume* gas_gap_vol = 
    new TGeoVolume("tof_gas_active", gas_gap, activeGasVolMed);
  gas_gap_vol->Divide("Strip",1,nstrips,-ggdx/2.,0);

  gas_gap_vol->SetLineColor(kRed); // set line color for the gas gap
  gas_gap_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* gas_gap_trans 
    = new TGeoTranslation("", 0., 0., (gdz+ggdz)/2.);

 
  // Single subdivided active gas gap 
  /*
    TGeoBBox* gas_active = new TGeoBBox("", gsdx/2., ggdy/2., ggdz/2.);
    TGeoVolume* gas_active_vol = 
    new TGeoVolume("tof_gas_active", gas_active, activeGasVolMed);
  gas_active_vol->SetLineColor(kBlack); // set line color for the gas gap
  gas_active_vol->SetTransparency(70); // set transparency for the TOF
  */

  // Add glass plate, inactive gas gap and active gas gaps to a single stack
  TGeoVolume* single_stack = new TGeoVolumeAssembly("single_stack");
  single_stack->AddNode(glass_plate_vol, 0, glass_plate_trans);
  single_stack->AddNode(gas_gap_vol, 0, gas_gap_trans);

  /*
  for (Int_t l=0; l<nstrips; l++){
    TGeoTranslation* gas_active_trans 
      = new TGeoTranslation("", -ggdx/2+(l+0.5)*gsdx, 0., 0.);
    gas_gap_vol->AddNode(gas_active_vol, l, gas_active_trans);
    //    single_stack->AddNode(gas_active_vol, l, gas_active_trans);
  }
  */  

  // Add 8 single stacks + one glass plate at the e09.750nd to a multi stack
  TGeoVolume* multi_stack = new TGeoVolumeAssembly("multi_stack");
  for (Int_t l=0; l<ngaps; l++){
    TGeoTranslation* single_stack_trans 
      = new TGeoTranslation("", 0., 0., startzpos + l*dzpos);
    multi_stack->AddNode(single_stack, l, single_stack_trans);
  }
  TGeoTranslation* single_glass_back_trans 
    = new TGeoTranslation("", 0., 0., startzpos + ngaps*dzpos);
  multi_stack->AddNode(glass_plate_vol, l, single_glass_back_trans);
  
  // Add electronics above and below the glass stack to build a complete counter
  TGeoVolume* counter = new TGeoVolumeAssembly("counter");
  TGeoTranslation* multi_stack_trans 
      = new TGeoTranslation("", 0., 0., 0.);
  counter->AddNode(multi_stack, l, multi_stack_trans);

  TGeoBBox* pcb = new TGeoBBox("", dxe/2., dye/2., dze/2.);
  TGeoVolume* pcb_vol = 
    new TGeoVolume("pcb", pcb, electronicsVolMed);
  pcb_vol->SetLineColor(kCyan); // set line color for the gas gap
  pcb_vol->SetTransparency(10); // set transparency for the TOF
  for (Int_t l=0; l<2; l++){
    yele *= -1.;
    TGeoTranslation* pcb_trans 
      = new TGeoTranslation("", 0., yele, 0.);
    counter->AddNode(pcb_vol, l, pcb_trans);
  }

  return counter;

}

TGeoVolume* create_new_counter(Int_t modType)
{

  //glass
  Float_t gdx=Glass_X[modType]; 
  Float_t gdy=Glass_Y[modType];
  Float_t gdz=Glass_Z[modType];

  //gas gap
  Int_t  nstrips=NumberOfReadoutStrips[modType];
  Int_t  ngaps=NumberOfGaps[modType];


  Float_t ggdx=GasGap_X[modType];  
  Float_t ggdy=GasGap_Y[modType];
  Float_t ggdz=GasGap_Z[modType];
  Float_t gsdx=ggdx/(Float_t)(nstrips);

  // electronics
  //pcb dimensions 
  Float_t dxe=Electronics_X[modType]; 
  Float_t dye=Electronics_Y[modType];
  Float_t dze=Electronics_Z[modType];
  Float_t yele=gdy/2.+dye/2.;
 
  // counter size (calculate from glas, gap and electronics sizes)
  Float_t cdx = TMath::Max(gdx, ggdx);
  cdx = TMath::Max(cdx, dxe)+ 0.2;
  Float_t cdy = TMath::Max(gdy, ggdy) + 2*dye + 0.2;
  Float_t cdz = ngaps * ggdz + (ngaps+1) * gdz + 0.2; // ngaps * (gdz+ggdz) + gdz + 0.2; // ok

  //calculate thickness and first position in counter of single stack
  Float_t dzpos = gdz+ggdz;
  Float_t startzposglas= -ngaps * (gdz + ggdz) /2.;        // -cdz/2.+0.1+gdz/2.; // ok  // (-cdz+gdz)/2.; // not ok
  Float_t startzposgas = startzposglas + gdz/2. + ggdz/2.; // -cdz/2.+0.1+gdz   +ggdz/2.;  // ok


  // needed materials
  TGeoMedium* glassPlateVolMed   = gGeoMan->GetMedium(GlasMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);
  TGeoMedium* activeGasVolMed    = gGeoMan->GetMedium(ActivGasMedium);
  TGeoMedium* electronicsVolMed  = gGeoMan->GetMedium(ElectronicsMedium);


  // define counter volume
  TGeoBBox* counter_box = new TGeoBBox("", cdx/2., cdy/2., cdz/2.);
  TGeoVolume* counter = 
    new TGeoVolume("counter", counter_box, noActiveGasVolMed);
  counter->SetLineColor(kCyan); // set line color for the counter
  counter->SetTransparency(70); // set transparency for the TOF

  // define single glass plate volume
  TGeoBBox* glass_plate = new TGeoBBox("", gdx/2., gdy/2., gdz/2.);
  TGeoVolume* glass_plate_vol = 
    new TGeoVolume("tof_glass", glass_plate, glassPlateVolMed);
  glass_plate_vol->SetLineColor(kMagenta); // set line color for the glass plate
  glass_plate_vol->SetTransparency(20); // set transparency for the TOF
  // define single gas gap volume
  TGeoBBox* gas_gap = new TGeoBBox("", ggdx/2., ggdy/2., ggdz/2.);
  TGeoVolume* gas_gap_vol = 
    new TGeoVolume("Gap", gas_gap, activeGasVolMed);
  gas_gap_vol->Divide("Cell",1,nstrips,-ggdx/2.,0);
  gas_gap_vol->SetLineColor(kRed); // set line color for the gas gap
  gas_gap_vol->SetTransparency(99); // set transparency for the TOF
 
  // place 8 gas gaps and 9 glas plates in the counter
  for( Int_t igap = 0; igap <= ngaps; igap++) {
    // place (ngaps+1) glass plates
    Float_t zpos_glas = startzposglas + igap*dzpos;
    TGeoTranslation* glass_plate_trans 
      = new TGeoTranslation("", 0., 0., zpos_glas);
    counter->AddNode(glass_plate_vol, igap, glass_plate_trans);
    // place ngaps gas gaps
    if (igap < ngaps) 
    {
      Float_t zpos_gas  = startzposgas  + igap*dzpos;
      TGeoTranslation* gas_gap_trans 
        = new TGeoTranslation("", 0., 0., zpos_gas);
      counter->AddNode(gas_gap_vol, igap, gas_gap_trans);
    }
//    cout <<"Zpos(Glas): "<< zpos_glas << endl;
//    cout <<"Zpos(Gas): "<< zpos_gas << endl;
  }
  
  // create and place the electronics above and below the glas stack
  TGeoBBox* pcb = new TGeoBBox("", dxe/2., dye/2., dze/2.);
  TGeoVolume* pcb_vol = 
    new TGeoVolume("pcb", pcb, electronicsVolMed);
  pcb_vol->SetLineColor(kYellow);  // kCyan); // set line color for electronics
  pcb_vol->SetTransparency(10); // set transparency for the TOF
  for (Int_t l=0; l<2; l++){
    yele *= -1.;
    TGeoTranslation* pcb_trans 
      = new TGeoTranslation("", 0., yele, 0.);
    counter->AddNode(pcb_vol, l, pcb_trans);
  }
 

  return counter;

}

TGeoVolume* create_tof_module(Int_t modType)
{
  Int_t cType = CounterTypeInModule[modType];
  Float_t dx=Module_Size_X[modType];
  Float_t dy=Module_Size_Y[modType];
  Float_t dz=Module_Size_Z[modType];
  Float_t width_aluxl=Module_Thick_Alu_X_left;
  Float_t width_aluxr=Module_Thick_Alu_X_right;
  Float_t width_aluy=Module_Thick_Alu_Y;
  Float_t width_aluz=Module_Thick_Alu_Z;


  Float_t dxpos=CounterXDistance[modType];
  Float_t startxpos=CounterXStartPosition[modType];
  Float_t dzoff=CounterZDistance[modType];
  Float_t rotangle=CounterRotationAngle[modType];

  TGeoMedium* boxVolMed          = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);

  TString moduleName = Form("module_%d", modType);
  TGeoVolume* module = new TGeoVolumeAssembly(moduleName);

  TGeoBBox* alu_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* alu_box_vol = 
    new TGeoVolume("alu_box", alu_box, boxVolMed);
  alu_box_vol->SetLineColor(kGreen); // set line color for the alu box
  alu_box_vol->SetTransparency(20); // set transparency for the TOF
  TGeoTranslation* alu_box_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  module->AddNode(alu_box_vol, 0, alu_box_trans);

  TGeoBBox* gas_box = new TGeoBBox("", (dx-(width_aluxl+width_aluxr))/2., (dy-2*width_aluy)/2., (dz-2*width_aluz)/2.);
  TGeoVolume* gas_box_vol = 
    new TGeoVolume("gas_box", gas_box, noActiveGasVolMed);
  gas_box_vol->SetLineColor(kYellow); // set line color for the gas box
  gas_box_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* gas_box_trans 
    = new TGeoTranslation("", shift_gas_box, 0., 0.);
  alu_box_vol->AddNode(gas_box_vol, 0, gas_box_trans);
  
  for (Int_t j=0; j<5; j++){ //loop over counters (modules)
    Float_t zpos;
    if (0 == modType) {
      zpos = dzoff *=-1;
    } else {
      zpos = 0.;
    }
    TGeoTranslation* counter_trans 
      = new TGeoTranslation("", startxpos+ j*dxpos , 0.0 , zpos);

    TGeoRotation* counter_rot = new TGeoRotation();
    counter_rot->RotateY(rotangle);
    TGeoCombiTrans* counter_combi_trans = new TGeoCombiTrans(*counter_trans, *counter_rot);
    gas_box_vol->AddNode(gCounter[cType], j, counter_combi_trans);
  }

  return module;
}

TGeoVolume* create_new_tof_module(Int_t modType)
{
  if(modType>2) return create_new_tof_module_m(modType); // Bucharest modules 

  Int_t cType = CounterTypeInModule[modType];
  Float_t dx=Module_Size_X[modType];
  Float_t dy=Module_Size_Y[modType];
  Float_t dz=Module_Size_Z[modType];
  Float_t width_aluxl=Module_Thick_Alu_X_left;
  Float_t width_aluxr=Module_Thick_Alu_X_right;
  Float_t width_aluy=Module_Thick_Alu_Y;
  Float_t width_aluz=Module_Thick_Alu_Z;

  Float_t dxpos=CounterXDistance[modType];
  Float_t startxpos=CounterXStartPosition[modType];
  Float_t dzoff=CounterZDistance[modType];
  Float_t rotangle=CounterRotationAngle[modType];

  TGeoMedium* boxVolMed          = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);

  TString moduleName = Form("module_%d", modType);

  TGeoBBox* module_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* module = 
  new TGeoVolume(moduleName, module_box, boxVolMed);
  module->SetLineColor(kGreen); // set line color for the alu box
  module->SetTransparency(20); // set transparency for the TOF

  TGeoBBox* gas_box = new TGeoBBox("", (dx-(width_aluxl+width_aluxr))/2., (dy-2*width_aluy)/2., (dz-2*width_aluz)/2.);
  TGeoVolume* gas_box_vol = 
    new TGeoVolume("gas_box", gas_box, noActiveGasVolMed);
  gas_box_vol->SetLineColor(kBlue); // set line color for the alu box
  gas_box_vol->SetTransparency(50); // set transparency for the TOF
  TGeoTranslation* gas_box_trans 
    = new TGeoTranslation("", shift_gas_box, 0., 0.);
  module->AddNode(gas_box_vol, 0, gas_box_trans);
  
  for (Int_t j=0; j< NCounterInModule[modType]; j++){ //loop over counters (modules)
    Float_t zpos;
    if (0 == modType || 3 == modType || 4 == modType || 5 == modType) {
      zpos = dzoff *=-1;
    } else {
      zpos = 0.;
    }
    TGeoTranslation* counter_trans 
      = new TGeoTranslation("", startxpos+ j*dxpos , 0.0 , zpos);

    TGeoRotation* counter_rot = new TGeoRotation();
    counter_rot->RotateY(rotangle);
    TGeoCombiTrans* counter_combi_trans = new TGeoCombiTrans(*counter_trans, *counter_rot);
    gas_box_vol->AddNode(gCounter[cType], j, counter_combi_trans);
  }

  return module;
}


TGeoVolume* create_new_tof_module_m(Int_t modType)
{
  Int_t cType2 = CounterTypeInModule[modType]%10;
  Int_t cType1 =(CounterTypeInModule[modType]-cType2)/10;

  Int_t iNum2=NCounterInModule[modType]%100;
  Int_t iNum1=(NCounterInModule[modType]-iNum2)/100;

  Int_t jm=modType-3;
  const Int_t iNum=iNum1+iNum2;

  cout << " module_m: " <<modType<<", "<<jm<<": "<< iNum1 << " counters type " << cType1
       << ", " << iNum2 << " type " << cType2 << endl;
 
  Float_t dx=Module_Size_X[modType];
  Float_t dy=Module_Size_Y[modType];
  Float_t dz=Module_Size_Z[modType];
  Float_t width_aluxl=Module_Thick_Alu_X_left;
  Float_t width_aluxr=Module_Thick_Alu_X_right;
  Float_t width_aluy=Module_Thick_Alu_Y;
  Float_t width_aluz=Module_Thick_Alu_Z;

  Float_t dxpos=CounterXDistance[modType];
  Float_t startxpos=CounterXStartPosition[modType];
  Float_t dzoff=CounterZDistance[modType];
  Float_t rotangle=CounterRotationAngle[modType];

  TGeoMedium* boxVolMed          = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* noActiveGasVolMed  = gGeoMan->GetMedium(NoActivGasMedium);

  TString moduleName = Form("module_%d", modType);

  TGeoBBox* module_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* module = 
  new TGeoVolume(moduleName, module_box, boxVolMed);
  module->SetLineColor(kGreen); // set line color for the alu box
  module->SetTransparency(20); // set transparency for the TOF

  TGeoBBox* gas_box = new TGeoBBox("", (dx-(width_aluxl+width_aluxr))/2., (dy-2*width_aluy)/2., (dz-2*width_aluz)/2.);
  TGeoVolume* gas_box_vol = 
    new TGeoVolume("gas_box", gas_box, noActiveGasVolMed);
  gas_box_vol->SetLineColor(kBlue); // set line color for the alu box
  gas_box_vol->SetTransparency(50); // set transparency for the TOF
  TGeoTranslation* gas_box_trans 
    = new TGeoTranslation("", shift_gas_box, 0., 0.);
  module->AddNode(gas_box_vol, 0, gas_box_trans);
  Float_t xpos;
  Float_t ypos;
  Float_t zpos;
  Int_t ii=0;
  for (Int_t j=0; j< iNum1; j++){ //loop over counters (in modules)
    Int_t iX  = (j - j%NCounter1Y[jm])/NCounter1Y[jm];
    Int_t iY  = j%NCounter1Y[jm];
    Int_t iX2 = iX%2;
    Int_t iY2 = iY%2;
    //    cout << "Put counter "<<j<<", jm: "<<jm<<", iX "<<iX<<", iY "<<iY<<", iX2 "<<iX2<<endl;
    /*
    Float_t xpos = CounterXstart1[jm] + iX*CounterDxpos1[jm];
    Float_t ypos = CounterYstart1[jm] + iY*CounterDypos1[jm];
    Float_t zpos = CounterDyoff1[jm][iY2]+CounterDzoff1[jm][iX2];
    */

    switch(jm){
    case 0:
      xpos = xPosCounter1[ii];
      ypos = yPosCounter1[ii];
      zpos = zPosCounter1[ii];
      break;
    case 1:
      xpos = xPosCounter2[ii];
      ypos = yPosCounter2[ii];
      zpos = zPosCounter2[ii];
      break;
    case 2:
      xpos = xPosCounter3[ii];
      ypos = yPosCounter3[ii];
      zpos = zPosCounter3[ii];
      break;
    }

    cout << "Place counter "<<j<<" "<<ii<<", iX "<<iX<<", iY "<<iY<<" at "<<xpos<<", "<<ypos<<", "<<zpos<<endl;

    TGeoTranslation* counter_trans 
      = new TGeoTranslation("", xpos , ypos, zpos);
    TGeoRotation* counter_rot = new TGeoRotation();
    counter_rot->RotateY(rotangle);
    TGeoCombiTrans* counter_combi_trans = new TGeoCombiTrans(*counter_trans, *counter_rot);
    gas_box_vol->AddNode(gCounter[cType1], j, counter_combi_trans);
    ii++;
  }

  for (Int_t j=0; j< iNum2; j++){ //loop over counters (in modules)
    Int_t iX  = (j - j%NCounter2Y[jm])/NCounter2Y[jm];
    Int_t iY  = j%NCounter2Y[jm];
    Int_t iX2 = iX%2;
    Int_t iY2 = iY%2;
    cout << "Put counter type 2 j="<<j<<", jm: "<<jm<<", iX "<<iX<<", iY "<<iY<<", iX2 "<<iX2<<endl;
    /*
    Float_t xpos = CounterXstart2[jm] + iX*CounterDxpos2[jm];
    Float_t ypos = CounterYstart2[jm] + iY*CounterDypos2[jm];
    Float_t zpos = CounterDyoff2[jm][iY2]+CounterDzoff2[jm][iX2];
    */

    switch(jm){
    case 0:
      xpos = xPosCounter1[ii];
      ypos = yPosCounter1[ii];
      zpos = zPosCounter1[ii];
      break;
    case 1:
      xpos = xPosCounter2[ii];
      ypos = yPosCounter2[ii];
      zpos = zPosCounter2[ii];
      break;
    case 2:
      xpos = xPosCounter3[ii];
      ypos = yPosCounter3[ii];
      zpos = zPosCounter3[ii];
      break;
    }


    cout << "Place counter "<<j<<", iX "<<iX<<", iY "<<iY<<" at "<<xpos<<", "<<ypos<<", "<<zpos<<endl;
    TGeoTranslation* counter_trans 
      = new TGeoTranslation("", xpos , ypos, zpos);
    TGeoRotation* counter_rot = new TGeoRotation();
    counter_rot->RotateY(rotangle);
    TGeoCombiTrans* counter_combi_trans = new TGeoCombiTrans(*counter_trans, *counter_rot);
    gas_box_vol->AddNode(gCounter[cType2], j+iNum1, counter_combi_trans);
    ii++;
  }

  return module;
}


TGeoVolume* create_tof_pole()
{
  // needed materials
  TGeoMedium* boxVolMed   = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* airVolMed   = gGeoMan->GetMedium(KeepingVolumeMedium);
   
  Float_t dx=Pole_Size_X;
  Float_t dy=Pole_Size_Y;
  Float_t dz=Pole_Size_Z;
  Float_t width_alux=Pole_Thick_X;
  Float_t width_aluy=Pole_Thick_Y;
  Float_t width_aluz=Pole_Thick_Z;
  
  TGeoVolume* pole = new TGeoVolumeAssembly("Pole");
  TGeoBBox*   pole_alu_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* pole_alu_vol = 
    new TGeoVolume("pole_alu", pole_alu_box, boxVolMed);
  pole_alu_vol->SetLineColor(kGreen); // set line color for the alu box
  pole_alu_vol->SetTransparency(20); // set transparency for the TOF
  TGeoTranslation* pole_alu_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  pole->AddNode(pole_alu_vol, 0, pole_alu_trans);

  Float_t air_dx = dx/2. - width_alux;
  Float_t air_dy = dy/2. - width_aluy; 
  Float_t air_dz = dz/2. - width_aluz;

  //  cout << "My pole." << endl;
  if (air_dx <= 0.)
    cout << "ERROR - No air volume in pole X, size: "<< air_dx << endl;
  if (air_dy <= 0.)
    cout << "ERROR - No air volume in pole Y, size: "<< air_dy << endl;
  if (air_dz <= 0.)
    cout << "ERROR - No air volume in pole Z, size: "<< air_dz << endl;

  if ((air_dx > 0.) && (air_dy > 0.) && (air_dz > 0.))  // crate air volume only, if larger than zero
  {
    TGeoBBox* pole_air_box = new TGeoBBox("", air_dx, air_dy, air_dz);
    //  TGeoBBox* pole_air_box = new TGeoBBox("", dx/2.-width_alux, dy/2.-width_aluy, dz/2.-width_aluz);
    TGeoVolume* pole_air_vol = 
      new TGeoVolume("pole_air", pole_air_box, airVolMed);
    pole_air_vol->SetLineColor(kYellow); // set line color for the alu box
    pole_air_vol->SetTransparency(70); // set transparency for the TOF
    TGeoTranslation* pole_air_trans 
      = new TGeoTranslation("", 0., 0., 0.);
    pole_alu_vol->AddNode(pole_air_vol, 0, pole_air_trans);
  }
  else
    cout << "Skipping pole_air_vol, no thickness: " << air_dx << " " << air_dy << " " << air_dz << endl;

  return pole;
}

TGeoVolume* create_tof_poleshort()
{
  // needed materials
  TGeoMedium* boxVolMed   = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* airVolMed   = gGeoMan->GetMedium(KeepingVolumeMedium);
   
  Float_t dx=Pole_Size_X;
  Float_t dy=PoleShort_Size_Y;
  Float_t dz=Pole_Size_Z;
  Float_t width_alux=Pole_Thick_X;
  Float_t width_aluy=Pole_Thick_Y;
  Float_t width_aluz=Pole_Thick_Z;
  
  TGeoVolume* pole = new TGeoVolumeAssembly("PoleShort");
  TGeoBBox*   pole_alu_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* pole_alu_vol = 
    new TGeoVolume("pole_alu", pole_alu_box, boxVolMed);
  pole_alu_vol->SetLineColor(kGreen); // set line color for the alu box
  pole_alu_vol->SetTransparency(20); // set transparency for the TOF
  TGeoTranslation* pole_alu_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  pole->AddNode(pole_alu_vol, 0, pole_alu_trans);

  Float_t air_dx = dx/2. - width_alux;
  Float_t air_dy = dy/2. - width_aluy; 
  Float_t air_dz = dz/2. - width_aluz;
  //  cout << "My pole." << endl;
  if (air_dx <= 0.)
    cout << "ERROR - No air volume in pole X, size: "<< air_dx << endl;
  if (air_dy <= 0.)
    cout << "ERROR - No air volume in pole Y, size: "<< air_dy << endl;
  if (air_dz <= 0.)
    cout << "ERROR - No air volume in pole Z, size: "<< air_dz << endl;

  if ((air_dx > 0.) && (air_dy > 0.) && (air_dz > 0.))  // crate air volume only, if larger than zero
  {
    TGeoBBox* pole_air_box = new TGeoBBox("", air_dx, air_dy, air_dz);
    //  TGeoBBox* pole_air_box = new TGeoBBox("", dx/2.-width_alux, dy/2.-width_aluy, dz/2.-width_aluz);
    TGeoVolume* pole_air_vol = 
      new TGeoVolume("pole_air", pole_air_box, airVolMed);
    pole_air_vol->SetLineColor(kYellow); // set line color for the alu box
    pole_air_vol->SetTransparency(70); // set transparency for the TOF
    TGeoTranslation* pole_air_trans 
      = new TGeoTranslation("", 0., 0., 0.);
    pole_alu_vol->AddNode(pole_air_vol, 0, pole_air_trans);
  }
  else
    cout << "Skipping pole_air_vol, no thickness: " << air_dx << " " << air_dy << " " << air_dz << endl;

  return pole;
}


TGeoVolume* create_tof_bar(Float_t dx, Float_t dy, Float_t dz)
{
  // needed materials
  TGeoMedium* boxVolMed = gGeoMan->GetMedium(BoxVolumeMedium);
  TGeoMedium* airVolMed = gGeoMan->GetMedium(KeepingVolumeMedium);
   
  Float_t width_alux=Pole_Thick_X;
  Float_t width_aluy=Pole_Thick_Y;
  Float_t width_aluz=Pole_Thick_Z;
  
  TGeoVolume* bar = new TGeoVolumeAssembly("Bar");
  TGeoBBox*   bar_alu_box = new TGeoBBox("", dx/2., dy/2., dz/2.);
  TGeoVolume* bar_alu_vol = 
    new TGeoVolume("bar_alu", bar_alu_box, boxVolMed);
  bar_alu_vol->SetLineColor(kGreen); // set line color for the alu box
  bar_alu_vol->SetTransparency(20); // set transparency for the TOF
  TGeoTranslation* bar_alu_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  bar->AddNode(bar_alu_vol, 0, bar_alu_trans);

  TGeoBBox* bar_air_box = new TGeoBBox("", dx/2.-width_alux, dy/2.-width_aluy, dz/2.-width_aluz);
  TGeoVolume* bar_air_vol = 
    new TGeoVolume("bar_air", bar_air_box, airVolMed);
  bar_air_vol->SetLineColor(kYellow); // set line color for the alu box
  bar_air_vol->SetTransparency(70);   // set transparency for the TOF
  TGeoTranslation* bar_air_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  bar_alu_vol->AddNode(bar_air_vol, 0, bar_air_trans);

  return bar;
}

void position_tof_poles(Int_t modType)
{

  TGeoTranslation* pole_trans=NULL;

  Int_t numPoles=0;
  Int_t numPolesShort=0;
  for (Int_t i=0; i<NumberOfPoles; i++){

     Float_t xPos=Pole_XPos[i];
     Float_t zPos=Pole_ZPos[i];
     if(TMath::Abs(xPos)>XLimInner)
     {
       pole_trans 
	 = new TGeoTranslation("", xPos, 0., zPos);
       gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
       numPoles++;
     }else{ // position 2 short poles 

       pole_trans 
	 = new TGeoTranslation("", xPos, PoleShort_Size_Y/2.-Pole_Size_Y/2., zPos);
       gGeoMan->GetVolume(geoVersion)->AddNode(gPoleShort, numPolesShort, pole_trans);
       numPolesShort++;

       pole_trans 
	 = new TGeoTranslation("", xPos, Pole_Size_Y/2.-PoleShort_Size_Y/2., zPos);
       gGeoMan->GetVolume(geoVersion)->AddNode(gPoleShort, numPolesShort, pole_trans);
       numPolesShort++;

     }
     //     cout << " Position Pole "<< numPoles<<" at z="<< Pole_ZPos[i] <<", x "<<Pole_XPos[i]<< endl;
  }
}

void position_tof_bars(Int_t modType)
{

  TGeoTranslation* bar_trans=NULL;

  Int_t numBars=0;
  for (Int_t i=0; i<NumberOfBars; i++){

     Float_t xPos=Bar_XPos[i];
     Float_t zPos=Bar_ZPos[i];
     Float_t yPos=Pole_Size_Y/2.+Bar_Size_Y/2.;

     bar_trans = new TGeoTranslation("", xPos, yPos, zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
     numBars++;

     bar_trans = new TGeoTranslation("", xPos,-yPos, zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
     numBars++;

     bar_trans = new TGeoTranslation("", -xPos, yPos, zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
     numBars++;

     bar_trans = new TGeoTranslation("", -xPos, -yPos, zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
     numBars++;

   }
   cout << " Position Bar "<< numBars<<" at z="<< Bar_ZPos[i] << endl;

   // outer horizontal and vertical frame bars 

    NumberOfBars++;
    Int_t i = NumberOfBars;
    gBar[i]=create_tof_bar(Bar_XLen,Bar_Size_Y,Bar_Size_Y);
    j=i+1;
    gBar[j]=create_tof_bar(Bar_Size_X,Bar_YLen,Bar_Size_Y);
    Float_t numBarY=0;
    numBars=0;

   for(Float_t dZ=-1.; dZ<2.; dZ+=2.)
   {
    Float_t zPos = Bar_Pos_Z - dZ*Bar_Size_Z/2.;
    Float_t yPos = Pole_Size_Y/2.+1.5*Bar_Size_Y;

    bar_trans = new TGeoTranslation("", 0., yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
    numBars++;

    bar_trans = new TGeoTranslation("", 0., -yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
    numBars++;

    Float_t xPos=Bar_XLen/2-Bar_Size_Y/2.;
    bar_trans = new TGeoTranslation("", xPos, 0., zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gBar[j], numBarY, bar_trans);
    numBarY++;

    bar_trans = new TGeoTranslation("", -xPos, 0., zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gBar[j], numBarY, bar_trans);
    numBarY++;

   }
}

void position_inner_tof_modules(Int_t modNType)
{
 TGeoTranslation* module_trans=NULL;

 //  Int_t numModules=(Int_t)( (Inner_Module_Last_Y_Position-Inner_Module_First_Y_Position)/Module_Size_Y[modType])+1;
 Float_t yPos=Inner_Module_First_Y_Position;
 Int_t ii=0; 
 Float_t xPos  = Inner_Module_X_Offset;
 Float_t zPos  = Wall_Z_Position;

 Pole_ZPos[NumberOfPoles] = zPos;
 Pole_Col[NumberOfPoles] = 0;
 NumberOfPoles++;

 Float_t DzPos =0.;
 for (Int_t j=0; j<modNType; j++){
   if (Module_Size_Z[j]>DzPos){
       DzPos = Module_Size_Z[j];
   }
 }
 Pole_ZPos[NumberOfPoles]=zPos+DzPos;
 Pole_Col[NumberOfPoles] = 0;
 NumberOfPoles++;

 for (Int_t j=0; j<modNType; j++){
  Int_t modType= Inner_Module_Types[j];
  Int_t modNum = 0;
  for(Int_t i=0; i<Inner_Module_Number[j]; i++) { 
    ii++; 
    cout << "Inner ii "<<ii<<" Last "<<Last_Size_Y<<","<<Last_Over_Y<<endl;
    Float_t DeltaY=Module_Size_Y[modType]+Last_Size_Y-2.*(Module_Over_Y[modType]+Last_Over_Y);
    yPos += DeltaY;
    Last_Size_Y=Module_Size_Y[modType];
    Last_Over_Y=Module_Over_Y[modType];
    cout <<"Position Inner Module "<<i<<" of "<<Inner_Module_Number[j]<<" Type "<<modType
         <<" at Y = "<<yPos<<" Ysize = "<<Module_Size_Y[modType]
	 <<" DeltaY = "<<DeltaY<<endl;

    module_trans 
      = new TGeoTranslation("", xPos, yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;
    module_trans 
      = new TGeoTranslation("", xPos, -yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;
    if (ii>0) {
      module_trans 
	= new TGeoTranslation("", xPos, yPos-DeltaY/2, zPos+Module_Size_Z[modType]);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;
      module_trans 
	= new TGeoTranslation("", xPos, -(yPos-DeltaY/2), zPos+Module_Size_Z[modType]);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;
    }
  }
 }
}

void position_tof_modules_m(Int_t ModType1, Int_t ModType2)
{

 TGeoTranslation* module_trans=NULL;
 TGeoRotation* module_rot0 = new TGeoRotation();
 module_rot0->RotateZ(0.); 
 TGeoRotation* module_rot1 = new TGeoRotation();
 module_rot1->RotateZ(180.);
 TGeoCombiTrans* module_combi_trans = NULL;

 NModules[3]=2; //M1
 NModules[4]=2; //M2
 NModules[5]=4; //M3

 Int_t ii=0; 
 for (Int_t j=ModType1; j<ModType2+1; j++){
  Int_t modType= j;
  Int_t jm=j-3;
  Int_t modNum = 0;
  Float_t SignZ;
  //  if(modType != 0) continue; // debugging  
  for(Int_t i=0; i<NModules[j]; i++) { 
    //if(i != 0) continue; // debugging  
    ii++; 
    Float_t xPos  = xPosModm[jm][i];
    Float_t yPos  = yPosModm[jm][i];
    Float_t zPos  = zPosModm[jm][i] + InnerWall_Z_PositionShift;
    cout<<"Place Mod Type "<<j<<" at x "<<xPos<<", y "<<yPos<<", z "<<zPos<<", Flip "<<FlipModm[jm][i]<<endl;

    module_trans       = new TGeoTranslation("", xPos, yPos, zPos);
    if(FlipModm[jm][i]==180.){
      module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot1);
    }else{
      module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot0);
    }
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_combi_trans);
    modNum++;
  }
 }
}

void position_tof_modules(Int_t NModTypes)
{
 TGeoTranslation* module_trans=NULL;
 TGeoRotation* module_rot0 = new TGeoRotation();
 module_rot0->RotateZ(0.); 
 TGeoRotation* module_rot1 = new TGeoRotation();
 module_rot1->RotateZ(180.);
 TGeoCombiTrans* module_combi_trans = NULL;
 const Int_t MaxLayer=20;
 Int_t NLayer=0;
 Float_t zPosLayer[MaxLayer]=0.;
 const Int_t MaxCol=100;
 Int_t NCol=0;
 Float_t xPosCol[MaxCol]=0.;
 const Float_t zAcc=0.1;
 const Float_t xAcc=2.;

 Int_t ii=0; 
 for (Int_t j=0; j<NModTypes+1; j++){
  Int_t modType= j;
  Int_t modNum = 0;
  Float_t SignZ;
  //  if(modType != 0) continue; // debugging  
  for(Int_t i=0; i<NModules[j]; i++) { 
    //if(i != 0) continue; // debugging  
    ii++; 
    Float_t xPos  = xPosMod[j][i]-shift_gas_box;
    Float_t yPos  = yPosMod[j][i];
    Float_t zPos  = zPosMod[j][i];
    //cout<<"Place Mod Type "<<j<<" at x "<<xPos<<", y "<<yPos<<", z "<<zPos<<", Flip "<<FlipMod[j][i]<<endl;

    module_trans       = new TGeoTranslation("", xPos, yPos, zPos);
    if(FlipMod[j][i]==1){
      module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot1);
    }else{
      module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot0);
    }
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_combi_trans);
    modNum++;

    // add structural material 

    Bool_t BPexist=kFALSE;
    Float_t SignX;
    if(xPosMod[j][i] != 0.) {
	SignX=xPos/TMath::Abs(xPos);
    }else{
	SignX=1.;
    } 

    Int_t iLz=-1;
    for (Int_t iL=0; iL<NLayer; iL++){
      if(zPos==zPosLayer[iL])
	{
	  iLz=iL;
	  break;
	}
    }
    if(iLz==-1){
	zPosLayer[NLayer]=zPos;
	iLz=NLayer;
	//	cout <<"Add zLayer "<<NLayer<<" at z="<<zPos<<" from ModType "<<j<<", Module "<<i<<endl;
	NLayer++;
    }
    if(iLz%2){
     SignZ = 1;
    }
    else{
     SignZ = -1;
    }
    switch(iLz){  // fix inconsistencies with above
    case 6: SignZ=1;
      break;
    case 7: SignZ=-1;
      break;
    default:
    }

    Float_t zPosPole = zPos + (Module_Size_Z[j]/2. + Pole_Size_Z/2.)*SignZ;
    Float_t xPosPole = xPos + (Module_Size_X[j]/2.)*SignX;

    Int_t iCx=-1;
    for (Int_t iC=0; iC<NCol; iC++){
      if( TMath::Abs(xPosPole-xPosCol[iC])<xAcc)
	{
	  iCx=iC;
	  break;
	}
    }
    if(iCx==-1){
	xPosCol[NCol]=xPosPole;
	iCx=NCol;
	//	cout <<"Add xCol "<<NCol<<" at x="<<xPos<<" from ModType "<<j<<", Module "<<i<<endl;
	NCol++;

	xPosCol[NCol]=xPos - (Module_Size_X[j]/2.)*SignX;
	iCx=NCol;
	//	cout <<"Add xCol "<<NCol<<" at x="<<xPos<<" from ModType "<<j<<", Module "<<i<<endl;
	NCol++;
    }

    for (Int_t iP=0; iP<NumberOfPoles; iP++){
      if(TMath::Abs(zPosPole - Pole_ZPos[iP])<zAcc && TMath::Abs(xPosPole - Pole_XPos[iP])<xAcc) {
       	BPexist=kTRUE;
	break; 
      }
    }

    if(!BPexist)
    {
      Pole_ZPos[NumberOfPoles] = zPosPole;
      Pole_XPos[NumberOfPoles] = xPosPole;
      Pole_Col[NumberOfPoles]  = 0;
      NumberOfPoles++;

      Pole_ZPos[NumberOfPoles] = zPosPole;
      Pole_XPos[NumberOfPoles] = xPos - (Module_Size_X[j]/2.)*SignX;
      Pole_Col[NumberOfPoles]  = 0;
      NumberOfPoles++;

      if( xPosMod[j][i] != 0.) {
	for(iP=0; iP<2; iP++){
	Int_t iLast=NumberOfPoles-2;
	Pole_ZPos[NumberOfPoles] =   Pole_ZPos[iLast];
	Pole_XPos[NumberOfPoles] = - Pole_XPos[iLast]; 
	Pole_Col[NumberOfPoles]  = 0;
	NumberOfPoles++;
	}
      }
      /*
      cout << " Add pole "<< NumberOfPoles <<" at z = "<<zPos<<", "<<zPos<<", x = "<< Pole_XPos[NumberOfPoles-1] 
	   <<", xsiz = "<<Module_Size_X[j]<<endl;
      */
    }
  }
 }
 cout << " Total number of modules: "<< modNum <<", poles: "<< NumberOfPoles<<endl;

 //  Pole related bars
 Float_t BZMin= 10000.;
 Float_t BZMax=-10000.;
 for(iL=0; iL<NLayer; iL++){
   if(zPosLayer[iL]>BZMax) BZMax=zPosLayer[iL];
   if(zPosLayer[iL]<BZMin) BZMin=zPosLayer[iL];
 }

 Bar_Size_Z = BZMax - BZMin;
 Bar_Pos_Z  = (BZMax+BZMin)/2.;

 //cout << "Place "<< NCol << " bars of z-length "<< Bar_Size_Z <<" at z = "<<Bar_Pos_Z<< endl;

 for (iC=0; iC<NCol; iC++)
 {
   gBar[NumberOfBars] = create_tof_bar(Bar_Size_X, Bar_Size_Y, Bar_Size_Z);
   Bar_ZPos[NumberOfBars] = Bar_Pos_Z; //PosPole+Bar_Size_Z/2.-Pole_Size_Z/2.;
   Bar_XPos[NumberOfBars] = xPosCol[iC];
   NumberOfBars++;
 }
}


void position_side_tof_modules(Int_t modNType)
{
 TGeoTranslation* module_trans=NULL;
 TGeoRotation* module_rot = new TGeoRotation();
 module_rot->RotateZ(180.);
 TGeoCombiTrans* module_combi_trans = NULL;

 //  Int_t numModules=(Int_t)( (Inner_Module_Last_Y_Position-Inner_Module_First_Y_Position)/Module_Size_Y[modType])+1;
 Float_t yPos=0.; //Inner_Module_First_Y_Position;
 Int_t ii=0; 
 for (Int_t j=0; j<modNType; j++){
  Int_t modType= InnerSide_Module_Types[j];
  Int_t modNum = 0;
  for(Int_t i=0; i<InnerSide_Module_Number[j]; i++) { 
    ii++; 
    cout << "InnerSide ii "<<ii<<" Last "<<Last_Size_Y<<","<<Last_Over_Y<<endl;
    Float_t DeltaY=Module_Size_Y[modType]+Last_Size_Y-2.*(Module_Over_Y[modType]+Last_Over_Y);
    if (ii>1){yPos += DeltaY;}
    Last_Size_Y=Module_Size_Y[modType];
    Last_Over_Y=Module_Over_Y[modType];
    Float_t xPos  = InnerSide_Module_X_Offset;
    Float_t zPos  = Wall_Z_Position;
    cout <<"Position InnerSide Module "<<i<<" of "<<InnerSide_Module_Number[j]<<" Type "<<modType
         <<" at Y = "<<yPos<<" Ysize = "<<Module_Size_Y[modType]
	 <<" DeltaY = "<<DeltaY<<endl;

    module_trans 
      = new TGeoTranslation("", xPos, yPos, zPos);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;

    module_trans 
      = new TGeoTranslation("", -xPos, yPos, zPos);
    module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_combi_trans);
    modNum++;

    if (ii>1) {
    module_trans 
      = new TGeoTranslation("", xPos, -yPos, zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;

    module_trans 
      = new TGeoTranslation("", -xPos, -yPos, zPos);
    module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_combi_trans);
    modNum++;

    module_trans 
      = new TGeoTranslation("", xPos, yPos-DeltaY/2, zPos+Module_Size_Z[modType]);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;

    module_trans 
      = new TGeoTranslation("", -xPos, yPos-DeltaY/2, zPos+Module_Size_Z[modType]);
    module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_combi_trans);
    modNum++;

    module_trans 
      = new TGeoTranslation("", xPos, -(yPos-DeltaY/2), zPos+Module_Size_Z[modType]);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_trans);
    modNum++;

    module_trans 
     = new TGeoTranslation("", -xPos,-(yPos-DeltaY/2), zPos+Module_Size_Z[modType]);
    module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
    gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum, module_combi_trans);
    modNum++;

    }
  }
 }
}

void position_outer_tof_modules(Int_t nCol) //modType, Int_t col1, Int_t col2)
{
  TGeoTranslation* module_trans=NULL;
  TGeoRotation* module_rot = new TGeoRotation();
  module_rot->RotateZ(180.);
  TGeoCombiTrans* module_combi_trans = NULL;

  //  Int_t numModules=(Int_t)( (Outer_Module_Last_Y_Position-Outer_Module_First_Y_Position)/Module_Size_Y[modType])+1;
  
  Int_t modNum[NofModuleTypes];
  for (Int_t k=0; k<NofModuleTypes; k++){
     modNum[k]=0;
  }

  Float_t zPos = Wall_Z_Position;
  for(Int_t j=0; j<nCol; j++){ 
   Float_t xPos = Outer_Module_X_Offset + ((j+1)*DxColl);
   Last_Size_Y=0.;
   Last_Over_Y=0.;
   Float_t yPos = 0.;
   Int_t   ii=0; 
   Float_t DzPos =0.;
   for(Int_t k=0; k<Outer_Module_NTypes; k++){
    Int_t modType= Outer_Module_Types[k][j];
    if(Module_Size_Z[modType]>DzPos){
      if(Outer_Module_Number[k][j]>0){
       DzPos = Module_Size_Z[modType];
      }
     }
   }   

   zPos -= 2.*DzPos; //((j+1)*2*Module_Size_Z[modType]);

   Pole_ZPos[NumberOfPoles] = zPos;
   Pole_Col[NumberOfPoles] = j+1;
   NumberOfPoles++;
   Pole_ZPos[NumberOfPoles] = zPos+DzPos;
   Pole_Col[NumberOfPoles] = j+1;
   NumberOfPoles++;
   //if (j+1==nCol) {
   if (1) {
    Pole_ZPos[NumberOfPoles] = Pole_ZPos[0];
    Pole_Col[NumberOfPoles] = j+1;
    NumberOfPoles++;

    Bar_Size_Z = Pole_ZPos[0] - zPos;
    gBar[NumberOfBars] = create_tof_bar(Bar_Size_X, Bar_Size_Y, Bar_Size_Z);
    Bar_ZPos[NumberOfBars] = zPos+Bar_Size_Z/2.-Pole_Size_Z/2.;
    Bar_XPos[NumberOfBars] = xPos + Pole_Offset;
    NumberOfBars++;
   }

   for (Int_t k=0; k<Outer_Module_NTypes; k++) {
    Int_t modType    = Outer_Module_Types[k][j];
    Int_t numModules = Outer_Module_Number[k][j];

    cout <<" Outer: position "<<numModules<<" of type "<<modType<<" in col "<<j
         <<" at z = "<<zPos<<", DzPos = "<<DzPos<<endl;    
    for(Int_t i=0; i<numModules; i++) {  
      ii++; 
      cout << "Outer ii "<<ii<<" Last "<<Last_Size_Y<<","<<Last_Over_Y<<endl;
      Float_t DeltaY=Module_Size_Y[modType]+Last_Size_Y-2.*(Module_Over_Y[modType]+Last_Over_Y);      
      if (ii>1){yPos += DeltaY;}
      Last_Size_Y=Module_Size_Y[modType];
      Last_Over_Y=Module_Over_Y[modType];
      cout <<"Position Outer Module "<<i<<" of "<<Outer_Module_Number[k][j]<<" Type "<<modType
           <<"(#"<<modNum[modType]<<") "<<" at Y = "<<yPos<<" Ysize = "<<Module_Size_Y[modType]
  	   <<" DeltaY = "<<DeltaY<<endl;

      module_trans = new TGeoTranslation("", xPos, yPos, zPos);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_trans);
      modNum[modType]++;
       
      module_trans = new TGeoTranslation("", -xPos, yPos, zPos);
      module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
      gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_combi_trans);
      modNum[modType]++;

      if (ii>1) {
	module_trans 
	  = new TGeoTranslation("", xPos, -yPos, zPos);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_trans);
	modNum[modType]++;
	module_trans 
	  = new TGeoTranslation("", -xPos, -yPos, zPos);
	module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_combi_trans);
	modNum[modType]++;

	// second layer
	module_trans 
	  = new TGeoTranslation("", xPos, yPos-DeltaY/2., zPos+DzPos);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_trans);
	modNum[modType]++;
	module_trans 
	  = new TGeoTranslation("", -xPos, yPos-DeltaY/2., zPos+DzPos);
	module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_combi_trans);
	modNum[modType]++;

	module_trans 
	  = new TGeoTranslation("", xPos, -(yPos-DeltaY/2.), zPos+DzPos);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_trans);
	modNum[modType]++;
	module_trans 
	  = new TGeoTranslation("", -xPos, -(yPos-DeltaY/2.), zPos+DzPos);
	module_combi_trans = new TGeoCombiTrans(*module_trans, *module_rot);
	gGeoMan->GetVolume(geoVersion)->AddNode(gModules[modType], modNum[modType], module_combi_trans);
	modNum[modType]++;

      }
    }
   }
  }
}


void dump_info_file()
{
  TDatime  datetime;   // used to get timestamp

  printf("writing info file: %s\n", FileNameInfo.Data());

  FILE *ifile;
  ifile = fopen(FileNameInfo.Data(),"w");

  if (ifile == NULL)
    {
      printf("error opening %s\n", FileNameInfo.Data());
      exit(1);
    }

  fprintf(ifile,"#\n##   %s information file\n#\n\n", geoVersion.Data());

  fprintf(ifile,"# created %d\n\n", datetime.GetDate());

  fprintf(ifile,"# TOF setup\n");
  if (TOF_Z_Front ==  450)
    fprintf(ifile,"SIS 100 hadron setup\n");
  if (TOF_Z_Front ==  600) 
    fprintf(ifile,"SIS 100 electron\n");
  if (TOF_Z_Front ==  650) 
    fprintf(ifile,"SIS 100 muon\n");
  if (TOF_Z_Front ==  880) 
    fprintf(ifile,"SIS 300 electron\n");
  if (TOF_Z_Front == 1020) 
    fprintf(ifile,"SIS 300 muon\n");
  fprintf(ifile,"\n");

  const Float_t TOF_Z_Back =  Wall_Z_Position + 1.5 * Module_Size_Z[0]; // back of TOF wall

  fprintf(ifile,"# envelope\n");
  // Show extension of TRD
  fprintf(ifile,"%7.2f cm   start of TOF (z)\n", TOF_Z_Front);
  fprintf(ifile,"%7.2f cm   end   of TOF (z)\n", TOF_Z_Back);
  fprintf(ifile,"\n");

  // Layer thickness
  fprintf(ifile,"# central tower position\n");
  fprintf(ifile,"%7.2f cm   center of staggered, front RPC cell at x=0\n", Wall_Z_Position);
  fprintf(ifile,"\n");

  fclose(ifile);
}
