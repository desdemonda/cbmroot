///                                             
/// \file Create_TRD_Geometry_v13a.C
/// \brief Generates TRD geometry in Root format.
///                                             

// 2014-05-23 - DE - v14x_3e - inner part consisting of 6x4 modules with 2x2 hole for the wide beampipe version
// 2014-05-02 - DE - v14a_3e - redesign inner part of station 3, now with 16x module type 2 instead of 8x module type 5
// 2014-05-02 - DE - v14a_3e - include optional GBTX readout boards on each module
// 2014-05-02 - DE - v14a_3e - introduce 3x5=15 Spadic FEBs for ultimate density on module type 1
//
// 2013-11-14 - DE - v13q_3e - generate information about pad plane layout (CbmTrdPads_v14a.h) for all module types in this macro
//
// 2013-11-04 - DE - v13p4 - adapt the number of front-end boards to the pad layout of the 540 mm modules
// 2013-11-04 - DE - v13p4 - use 8 module types (4x S + 4x L) to better match the occupancy
// 2013-10-31 - DE - v13p4 - modify the support structure of station 1 to match with the MUCH/RICH platform
// 2013-10-29 - DE - v13p4 - build lattice grid as TGeoBBox instead of VolumeAssembly - in run_sim.C save  9% of time compared to v13p7
// 2013-10-29 - DE - v13p4 - build lattice grid as TGeoBBox instead of CompositeShape - in run_sim.C save 18% of time compared to v13p6
//
// 2013-10-28 - DE - introduce new geometry naming scheme: v13p1 - SIS 100 hadron
// 2013-10-28 - DE - introduce new geometry naming scheme: v13p2 - SIS 100 electron
// 2013-10-28 - DE - introduce new geometry naming scheme: v13p3 - SIS 100 muon
// 2013-10-28 - DE - introduce new geometry naming scheme: v13p4 - SIS 300 electron
// 2013-10-28 - DE - introduce new geometry naming scheme: v13p5 - SIS 300 muon
// 2013-10-28 - DE - add option to draw the magnetic field vector in the magnet
// 2013-09-27 - DE - do not use TGeoXtru to build the supports, use TGeoBBox instead
//
// 2013-06-25 - DE - v13g trd300_rich             (10 layers, z = 4100 ) - TRD right behind SIS300 RICH
// 2013-06-25 - DE - v13h trd100_sts              ( 4 layers, z = 2600 ) - TRD completely on RICH/MUCH platform to allow TOF to move upstream
// 2013-06-25 - DE - v13i trd100_rich             ( 2 layers, z = 4100 ) - TRD right behind RICH			      
// 2013-06-25 - DE - v13j trd100_rich             ( 3 layers, z = 4100 ) - TRD right behind RICH			      
// 2013-06-25 - DE - v13k trd100_rich             ( 4 layers, z = 4100 ) - TRD right behind RICH			      
// 2013-06-25 - DE - ---  trd100_much_2_absorbers ( 4 layers, z = 4300 ) - same as version at z = 4600			      
// 2013-06-25 - DE - v13l trd100_much_3_absorbers ( 4 layers, z = 4600 ) - TRD right behind SIS100 MUCH
// 2013-06-25 - DE - v13m trd300_much_6_absorbers (10 layers, z = 5500 ) - TRD right behind SIS300 MUCH
// 2013-06-25 - DE - v13n trd300_rich_stretched   (10 layers, z = 4600 ) - TRD stretched behind SIS300 RICH
//
// 2013-06-19 - DE - add TRD (I, II, III) labels on support structure
// 2013-05-29 - DE - allow for flexible TRD z-positions defined by position of layer01
// 2013-05-23 - DE - remove "trd_" prefix from node names (except top node)
// 2013-05-22 - DE - radiators G30 (z=240 mm) 
// 2013-05-22 - DE - radiators H (z=275 mm - 125 * 2.2mm), (H++ z=335 mm)
// 2013-05-22 - DE - radiators B++ (z=254 mm - 350 * 0.724 mm), K++ (z=254 mm - 350 * 0.724 mm)
// 2013-04-17 - DE - introduce volume assembly for layers, e.g. trd_layer03
// 2013-03-26 - DE - use Air as ASIC material
// 2013-03-26 - DE - put support structure into its own assembly
// 2013-03-26 - DE - move TRD upstream to z=400m
// 2013-03-26 - DE - RICH will probably end at z=380 cm, TRD can move to 400 cm
// 2013-03-25 - DE - shrink active area from 570 to 540 mm and 960 to 910 mm
// 2013-03-06 - DE - add ASICs on FEBs
// 2013-03-05 - DE - introduce supports for SIS100 and SIS300
// 2013-03-05 - DE - replace all Float_t by Double_t
// 2013-01-21 - DE - introduce TRD media, use TRDG10 as material for pad plane and FEBs
// 2013-01-21 - DE - put backpanel into the geometry
// 2013-01-11 - DE - allow for misalignment of TRD modules
// 2012-11-04 - DE - add kapton foil, add FR4 padplane
// 2012-11-03 - DE - add lattice grid on entrance window as CompositeShape

// TODO: 
// - use Silicon as ASIC material

// in root all sizes are given in cm

#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TGeoXtru.h"
#include "TFile.h"
#include "TString.h"
#include "TList.h"
#include "TRandom3.h"
#include "TDatime.h"

#include <iostream>

// Name of output file with geometry
const TString tagVersion   = "v14x";
//const TString subVersion   = "_1h";
//const TString subVersion   = "_1e";
//const TString subVersion   = "_1m";
const TString subVersion   = "_3e";
//const TString subVersion   = "_3m";
const TString geoVersion   = "trd_" + tagVersion + subVersion;
const TString FileNameSim  = geoVersion + ".geo.root";
const TString FileNameGeo  = geoVersion + "_geo.root";
const TString FileNameInfo = geoVersion + ".geo.info";
const TString FileNamePads = "CbmTrdPads_" + tagVersion + ".h";

// display switches
const Bool_t IncludeRadiator    = true;  // false;  // true, if radiator is included in geometry
const Bool_t IncludeLattice     = true;  // false;  // true, if lattice grid is included in geometry
const Bool_t IncludeFebs        = true;  // false;  // true, if FEBs are included in geometry
const Bool_t IncludeRobs        = false; // false;  // true, if ROBs are included in geometry
const Bool_t IncludeAsics       = true;  // false;  // true, if ASICs are included in geometry
const Bool_t IncludeSupports    = true;  // false;  // true, if support structure is included in geometry
const Bool_t IncludeLabels      = true;  // false;  // true, if TRD (I, II, III) labels are plotted in (VisLevel 5)
const Bool_t IncludeFieldVector = false; // false;  // true, if magnetic field vector to be shown (in the magnet)

const Double_t feb_rotation_angle = 45; //0.1; // 65.; // 70.; // 0.;   // rotation around x-axis, should be < 90 degrees  

// positioning switches
const Bool_t DisplaceRandom  = false;  // true; // false;  // add random displacement of modules for alignment study
const Bool_t RotateRandom    = false;  // true; // false;  // add random rotation of modules for alignment study
const Bool_t DoExplode       = false;  // true, // false;  // add random displacement of modules for alignment study

// positioning parameters
const Double_t maxdx    = 0.2;   // max +- 0.1 cm shift in x
const Double_t maxdy    = 0.2;   // max +- 0.1 cm shift in y
const Double_t maxdz    = 1.0;   // max +- 1.0 cm shift in z

const Double_t maxdrotx = 2.0;   // 20.0; // max rotation around x
const Double_t maxdroty = 2.0;   // 20.0; // max rotation around y
const Double_t maxdrotz = 2.0;   // 20.0; // max rotation around z

const Double_t ExplodeFactor = 1.02;   // 1.02; // Factor by which modules are exploded in the x/y plane

// initialise random numbers
TRandom3 r3(0);

// Parameters defining the layout of the complete detector build out of different detector layers.
const Int_t   MaxLayers = 10;   // max layers

// select layers to display
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //  1st layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  //  2nd layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };  //  5th layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };  //  6th layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };  //  9th layer only
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };  // 10th layer only
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  // Station 1, layer 1, 2
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 };  // Station 2, layer 5, 6
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 };  // Station 3, layer 9,10
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, 0 };  // Station 1 and 2
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 1, 1, 1, 0, 1, 1 };  // Station 1, 2 and 3
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };  // SIS100-2l  // 1: plot, 0: hide
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };  // SIS100-3l  // 1: plot, 0: hide
//
//const Int_t    ShowLayer[MaxLayers] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };  // SIS100-4l  // 1: plot, 0: hide
//const Int_t    ShowLayer[MaxLayers] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };  // SIS300-mu  // 1: plot, 0: hide
const Int_t    ShowLayer[MaxLayers] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };  // SIS300-e   // 1: plot, 0: hide

Int_t    PlaneId[MaxLayers]; // automatically filled with layer ID

const Int_t   LayerType[MaxLayers]        = { 10, 11, 10, 11, 20, 21, 20, 21, 30, 31 };  // ab: a [1-3] - layer type, b [0,1] - vertical/horizontal pads
const Int_t   LayerNrInStation[MaxLayers] = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2 };

// 5x z-positions from 260 till 550 cm
//Double_t LayerPosition[MaxLayers] = { 260. }; // start position - 2013-10-28 - DE - v13x1 - SIS 100 hadron         ( 4 layers, z = 2600 )
//Double_t LayerPosition[MaxLayers] = { 410. }; // start position - 2013-10-28 - DE - v13x2 - SIS 100 electron       ( 4 layers, z = 4100 )
//Double_t LayerPosition[MaxLayers] = { 460. }; // start position - 2013-10-28 - DE - v13x3 - SIS 100 muon     3_abs ( 4 layers, z = 4600 )
Double_t LayerPosition[MaxLayers] = { 410. }; // start position - 2013-10-28 - DE - v13x4 - SIS 300 electron       (10 layers, z = 4100 )
//Double_t LayerPosition[MaxLayers] = { 550. }; // start position - 2013-10-28 - DE - v13x5 - SIS 300 muon     6_abs (10 layers, z = 5500 )
//
// obsolete variants
//Double_t LayerPosition[MaxLayers] = { 410. }; // start position - 2013-06-25 - DE - v13i trd100_rich             ( 2 layers, z = 4100 )
//Double_t LayerPosition[MaxLayers] = { 410. }; // start position - 2013-06-25 - DE - v13j trd100_rich             ( 3 layers, z = 4100 )
//Double_t LayerPosition[MaxLayers] = { 430. }; // start position - 2013-06-25 - DE - ---  trd100_much_2_absorbers ( 4 layers, z = 4300 )
//Double_t LayerPosition[MaxLayers] = { 460. }; // start position - 2013-06-25 - DE - v13n trd300_rich_stretched   (10 layers, z = 4600 )


const Double_t LayerThickness = 45.0; // Thickness of one TRD layer in cm

const Double_t LayerOffset[MaxLayers] = {   0.,   0.,   0.,   0.,   5.,   0.,   0.,   0.,   5.,   0. };  // v13x[4,5] - z offset in addition to LayerThickness 
//const Double_t LayerOffset[MaxLayers] = {   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0.,   0. };  // SIS100 - z offset in addition to LayerThickness 
//const Double_t LayerOffset[MaxLayers] = {   0.,   0.,   0.,   0.,  95.,   0.,   0.,   0.,   5.,   0. };  // v13n       - z offset in addition to LayerThickness 

//const Int_t LayerArraySize[3][4] =  { { 5, 5, 9, 11 },    // for layer[1-3][i,o] below
//                                      { 5, 5, 9, 11 },
//                                      { 5, 5, 9, 11 } };
const Int_t LayerArraySize[3][4] =  { { 4, 6, 8, 11 },    // for layer[1-3][i,o] below
                                      { 4, 6, 8, 11 },
                                      { 4, 6, 8, 11 } };
// ### Layer Type 1
// v12x - module types in the inner sector of layer1 - looking upstream
const Int_t layer1i[4][6] = { { 423,  323,  323,  321,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              { 223,  123,    0,    0,  121,  221 },  
                              { 203,  103,    0,    0,  101,  201 },  
                              { 403,  303,  303,  301,  301,  401 } };
// number of modules 1x0, 8x1, 4x2, 8x3, 4x4

// v12x - module types in the outer sector of layer1 - looking upstream
const Int_t layer1o[8][11]= { {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,  823,  823,  723,  721,  721,  821,  821,    0,    0 },
                              {  0,    0,  823,  623,    0,    0,    0,  621,  821,    0,    0 },
                              {  0,    0,  803,  603,    0,    0,    0,  601,  801,    0,    0 },
                              {  0,    0,  803,  803,  703,  701,  701,  801,  801,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };
// number of modules 73x0, 0x5, 0x6, 12x7, 14x8
// Layer1 =  24 + 26;   // v12x

// ### Layer Type 11 is Layer Type 1 with detector modules rotated by 90°
// in the subroutine creating the layers, this is recognized automatically 


// ### Layer Type 2
// v12x - module types in the inner sector of layer2 - looking upstream
const Int_t layer2i[4][6] = { { 423,  323,  323,  321,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              { 223,  123,    0,    0,  121,  221 },  
                              { 203,  103,    0,    0,  101,  201 },  
                              { 403,  303,  303,  301,  301,  401 } };
// number of modules 1x0, 4x1, 4x2, 12x3, 4x4

// v12x - module types in the outer sector of layer2 - looking upstream
const Int_t layer2o[8][11]= { {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
                              {   0,  823,  823,  823,  823,  821,  821,  821,  821,  821,    0 },
                              {   0,  823,  823,  723,  723,  721,  721,  721,  821,  821,    0 },
                              {   0,  823,  723,  623,    0,    0,    0,  621,  721,  821,    0 },
                              {   0,  803,  703,  603,    0,    0,    0,  601,  701,  801,    0 },
                              {   0,  803,  803,  703,  703,  701,  701,  701,  801,  801,    0 },
                              {   0,  803,  803,  803,  803,  801,  801,  801,  801,  801,    0 },
                              {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };
// number of modules 45x0, 0x5, 12x6, 0x7, 42x8
// Layer2 =  78;   // v12x

// ### Layer Type 21 is Layer Type 2 with detector modules rotated by 90°
// In the subroutine creating the layers this is recognized automatically 


// ### Layer Type 3
// v12x - module types in the inner sector of layer2 - looking upstream
////const Int_t layer3i[5][5] = { { 223,  223,  221,  221,  221 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers 
////                              { 223,    0,    0,    0,  221 },
////                              { 203,    0,    0,    0,  201 },
////                              { 203,    0,    0,    0,  201 },
////                              { 203,  203,  201,  201,  201 } };
//const Int_t layer3i[5][5] = { { 423,  323,  321,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers 
//                              { 223,  123,  121,  121,  221 },
//                              { 203,    0,    0,    0,  201 },
//                              { 203,  103,  101,  101,  201 },
//                              { 403,  303,  301,  301,  401 } };
const Int_t layer3i[4][6] = { { 423,  323,  323,  321,  321,  421 },    // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              { 223,  123,    0,    0,  121,  221 },  
                              { 203,  103,    0,    0,  101,  201 },  
                              { 403,  303,  303,  301,  301,  401 } };
// number of modules 25x0 
// needed only for convenience in the function

// v12x - module types in the outer sector of layer3 - looking upstream
const Int_t layer3o[8][11] = { { 823,  823,  823,  823,  823,  821,  821,  821,  821,  821,  821 },
                               { 823,  823,  823,  723,  723,  721,  721,  721,  821,  821,  821 },
                               { 823,  823,  723,  723,  623,  621,  621,  721,  721,  821,  821 },
                               { 823,  723,  623,  523,    0,    0,    0,  521,  621,  721,  821 },
                               { 803,  703,  603,  503,    0,    0,    0,  501,  601,  701,  801 },
                               { 803,  803,  703,  703,  603,  601,  601,  701,  701,  801,  801 },
                               { 803,  803,  803,  703,  703,  701,  701,  701,  801,  801,  801 },
                               { 803,  803,  803,  803,  803,  801,  801,  801,  801,  801,  801 } };
// number of modules 1x0, 8x5, 12x6, 24x7, 54x8
// Layer3 = 98;   // v12x

/* only L type modules
// ### Layer Type 3
// v12x - module types in the inner sector of layer2 - looking upstream
const Int_t layer3i[5][5] = { {  0,  0,  0,  0,  0 },     // abc: a module type - b orientation (x90 deg) in odd - c even layers
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 },
                              {  0,  0,  0,  0,  0 } };
// number of modules 25x0 
// needed only for convenience in the function

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
*/

// ### Layer Type 31 is Layer Type 3 with detector modules rotated by 90
// In the subroutine creating the layers this is recognized automatically 


// Parameters defining the layout of the different detector modules
const Int_t NofModuleTypes = 8;
const Int_t ModuleType[NofModuleTypes]    = {  0,  0,  0,  0,  1,  1,  1,  1 }; // 0 = small module, 1 = large module

// GBTx ROB definitions
const Int_t RobsPerModule[NofModuleTypes] = {  2,  2,  1,  1,  2,  1,  1,  1 }; // number of GBTx ROBs on module
const Int_t GbtxPerRob[NofModuleTypes]    = {107,105,105,103,107,107,105,103 }; // number of GBTx ASICs on ROB

//// ultimate density - 540 mm
//const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  6,  4, 12,  6,  4,  3 }; // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {315,210,105,105,108,108,108,108 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
// ginat modules - 1100 mm
const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  6,  4, 14,  7,  4,  3 }; // number of FEBs on backside - reduced FEBs (64 ch ASICs)
const Int_t AsicsPerFeb[NofModuleTypes]   = {315,210,105,105,110,110,110,110 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
//const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  3,  2,  6,  3,  4,  3 }; // min number of FEBs // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {315,210,210,210,216,216,108,108 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
//const Int_t AsicsPerFeb[NofModuleTypes]   = {216,210,210,210,216,216,108,108 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
//
////// super density - 540 mm
//const Int_t FebsPerModule[NofModuleTypes] = {  9,  5,  6,  4, 12,  6,  4,  3 }; // light // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {210,210,105,105,108,108,108,108 }; // %100 gives number of ASICs on FEB, /100 gives grouping
//
//// normal density - 540 mm
//const Int_t FebsPerModule[NofModuleTypes] = { 18, 10,  6,  4, 12,  6,  4,  3 }; // number of FEBs on backside (linked to pad layout) - mod4 = mod3, therefore same
//const Int_t AsicsPerFeb[NofModuleTypes]   = {105,105,105,105,108,108,108,108 }; // %100 gives number of ASICs on FEB, /100 gives grouping

// ultimate density - 570 mm
//const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  3,  2,  5,  3,  2,  1 }; // min number of FEBs // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {216,210,210,210,216,216,216,216 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
//
//const Int_t FebsPerModule[NofModuleTypes] = {  6,  5,  3,  3, 10,  5,  3,  3 }; // min (6) module types // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {216,210,210,210,108,108,108,108 }; //  %100 gives number of ASICs on FEB, /100 gives grouping
//// super density - 570 mm
//const Int_t FebsPerModule[NofModuleTypes] = { 10,  5,  5,  5, 12,  6,  4,  3 }; // light // number of FEBs on backside - reduced FEBs (64 ch ASICs)
//const Int_t AsicsPerFeb[NofModuleTypes]   = {210,210,105,105,108,108,108,108 }; // %100 gives number of ASICs on FEB, /100 gives grouping
//
//// normal density - 570 mm
//const Int_t FebsPerModule[NofModuleTypes] = { 19, 10,  5,  5, 12,  6,  4,  3 }; // number of FEBs on backside (linked to pad layout) - mod4 = mod3, therefore same
//const Int_t AsicsPerFeb[NofModuleTypes]   = {105,105,105,105,108,108,108,108 }; // %100 gives number of ASICs on FEB, /100 gives grouping


/* TODO: activate connector grouping info below
// ultimate - grouping of pads to connectors
const Int_t RowsPerConnector[NofModuleTypes]  = {  6,  4,  2,  2,  2,  2,  2,  2 };
const Int_t ColsPerConnector[NofModuleTypes]  = { 16, 16, 16, 16, 16, 16, 16, 16 };
// super    - grouping of pads to connectors
const Int_t RowsPerConnector[NofModuleTypes]  = {  4,  4,  2,  2,  2,  2,  2,  2 };
const Int_t ColsPerConnector[NofModuleTypes]  = { 16, 16, 16, 16, 16, 16, 16, 16 };
// normal   - grouping of pads to connectors
const Int_t RowsPerConnector[NofModuleTypes]  = {  2,  2,  2,  2,  2,  2,  2,  2 };
const Int_t ColsPerConnector[NofModuleTypes]  = { 16, 16, 16, 16, 16, 16, 16, 16 };
*/


const Double_t feb_z_offset = 0.1;  // 1 mm - offset in z of FEBs to backpanel
const Double_t asic_offset  = 0.2;  // 2 mm - offset of ASICs to FEBs to avoid overlaps

// ASIC parameters
const Double_t asic_thickness = 0.25; // asic_thickness; // 2.5 mm
const Double_t asic_width     = 3.0;  // 2.0;  1.0;   // 1 cm
//const Double_t asic_distance  = 0.4;  // 0.40; // a factor of width for ASIC pairs
Double_t asic_distance; //  = 0.40; // for 10 ASICs - a factor of width for ASIC pairs

const Double_t FrameWidth[2]    = { 1.5, 2.0 };   // Width of detector frames in cm
// mini - production
//const Double_t DetectorSizeX[2] = { 57., 95.};   // => 54 x 54 cm2 & 91 x 91 cm2 active area
//const Double_t DetectorSizeY[2] = { 57., 95.};   // quadratic modules
const Double_t DetectorSizeX[2] = { 57., 114.};   // => 54 x 54 cm2 & 91 x 91 cm2 active area
const Double_t DetectorSizeY[2] = { 57., 114.};   // quadratic modules
//// default
//const Double_t DetectorSizeX[2] = { 60., 100.};   // => 57 x 57 cm2 & 96 x 96 cm2 active area
//const Double_t DetectorSizeY[2] = { 60., 100.};   // quadratic modules

// Parameters tor the lattice grid reinforcing the entrance window
const Double_t lattice_o_width[2] = { 1.5, 2.0 };   // Width of outer lattice frame in cm
const Double_t lattice_i_width[2] = { 0.2, 0.2 };   // { 0.4, 0.4 };   // Width of inner lattice frame in cm
// Thickness (in z) of lattice frames in cm - see below

// statistics
Int_t ModuleStats[MaxLayers][NofModuleTypes] = { 0 };

// z - geometry of TRD modules
//const Double_t radiator_thickness     =  35.0;    // 35 cm thickness of radiator
const Double_t radiator_thickness     =  30.0;    // 30 cm thickness of radiator + shift pad plane to integer multiple of 1 mm
const Double_t radiator_position      =  - LayerThickness/2. + radiator_thickness/2.;

//const Double_t lattice_thickness      =   1.0;  // 1.0;  // 10 mm thick lattice frames
const Double_t lattice_thickness      =   1.0 - 0.0025; // 0.9975;  // 1.0;  // 10 mm thick lattice frames
const Double_t lattice_position       =  radiator_position + radiator_thickness/2. + lattice_thickness/2.;

const Double_t kapton_thickness       =   0.0025; //  25 micron thickness of kapton
const Double_t kapton_position        =  lattice_position + lattice_thickness/2. + kapton_thickness/2.;

const Double_t gas_thickness          =   1.2;    //  12 mm thickness of gas
const Double_t gas_position           =  kapton_position + kapton_thickness/2. + gas_thickness/2.;

// frame thickness
const Double_t frame_thickness        =  gas_thickness;   // frame covers gas volume: from kapton foil to pad plane
const Double_t frame_position         =  - LayerThickness /2. + radiator_thickness + lattice_thickness + kapton_thickness + frame_thickness/2.;

// pad plane
const Double_t padcopper_thickness    =   0.0025; //  25 micron thickness of copper pads
const Double_t padcopper_position     =  gas_position + gas_thickness/2. + padcopper_thickness/2.;

const Double_t padplane_thickness     =   0.0360; // 360 micron thickness of padplane
const Double_t padplane_position      =  padcopper_position + padcopper_thickness/2. + padplane_thickness/2.;

// backpanel components
const Double_t carbon_thickness       =   0.0190 * 2; // use 2 layers!!   // 190 micron thickness for 1 layer of carbon fibers
const Double_t honeycomb_thickness    =   2.3 - kapton_thickness - padcopper_thickness - padplane_thickness - carbon_thickness;    //  ~ 2.3 mm thickness of honeycomb
const Double_t honeycomb_position     =  padplane_position + padplane_thickness/2. + honeycomb_thickness/2.;
const Double_t carbon_position        =  honeycomb_position + honeycomb_thickness/2. + carbon_thickness/2.;

// readout boards
const  Double_t febvol_thickness      =  10.0;    // 10 cm length of FEBs
const  Double_t febvol_position       =  carbon_position + carbon_thickness/2. + febvol_thickness/2.;
const  Double_t feb_thickness         =   0.25;  // light //  2.5 mm thickness of FEBs



// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString RadiatorVolumeMedium    = "TRDpefoam20";
const TString LatticeVolumeMedium     = "TRDG10";
const TString KaptonVolumeMedium      = "TRDkapton";
const TString GasVolumeMedium         = "TRDgas";
const TString PadCopperVolumeMedium   = "TRDcopper";
const TString PadPcbVolumeMedium      = "TRDG10";    // todo - put correct FEB material here
const TString HoneycombVolumeMedium   = "TRDaramide";
const TString CarbonVolumeMedium      = "TRDcarbon";
const TString FebVolumeMedium         = "TRDG10";    // todo - put correct FEB material here
const TString AsicVolumeMedium        = "air";       // todo - put correct ASIC material here
const TString TextVolumeMedium        = "air";       // leave as air
const TString FrameVolumeMedium       = "TRDG10";
const TString AluminiumVolumeMedium   = "aluminium";
//const TString MylarVolumeMedium       = "mylar";
//const TString RadiatorVolumeMedium    = "polypropylene";
//const TString ElectronicsVolumeMedium = "goldcoatedcopper";


// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance
TGeoVolume*  gModules[NofModuleTypes]; // Global storage for module types

// Forward declarations
void create_materials_from_media_file();
void create_trd_module_type(Int_t moduleType);
void create_detector_layers(Int_t layer);
void create_xtru_supports();
void create_box_supports();
void add_trd_labels();
void create_mag_field_vector();
void dump_info_file();
void dump_digi_file();


void Create_TRD_Geometry_v14x_3e() {
  // Load the necessary FairRoot libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");

  // Load needed material definition from media.geo file
  create_materials_from_media_file();

  // Position the layers in z
  for (Int_t iLayer = 1; iLayer < MaxLayers; iLayer++)
    LayerPosition[iLayer] = LayerPosition[iLayer-1] + LayerThickness + LayerOffset[iLayer];  // add offset for extra gaps

  // Get the GeoManager for later usage
  gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
  gGeoMan->SetVisLevel(10);  
  
  // Create the top volume 
  TGeoBBox* topbox= new TGeoBBox("", 1000., 1000., 2000.);
  TGeoVolume* top = new TGeoVolume("top", topbox, gGeoMan->GetMedium("air"));
  gGeoMan->SetTopVolume(top);

  TGeoVolume* trd = new TGeoVolumeAssembly(geoVersion);
  top->AddNode(trd, 1);

  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++) {
     Int_t moduleType = iModule + 1;
     gModules[iModule] = create_trd_module_type(moduleType);
  }

  Int_t nLayer = 0;  // active layer counter
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++) {
    //    if ((iLayer != 0) && (iLayer != 3))  continue;  // first layer only - comment later on
    //    if (iLayer != 0) continue;  // first layer only - comment later on
    if (ShowLayer[iLayer])
    {
      PlaneId[iLayer]=++nLayer;
      create_detector_layers(iLayer);
      //      printf("calling layer %2d\n",iLayer);
    }
  }

  // TODO: remove or comment out
  // test PlaneId
  printf("generated TRD layers: ");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      printf(" %2d",PlaneId[iLayer]);
  printf("\n");

  if (IncludeSupports)
  {
    //    create_xtru_supports();
    create_box_supports();
  }

  if (IncludeFieldVector)
    create_mag_field_vector();
  
  gGeoMan->CloseGeometry();
//  gGeoMan->CheckOverlaps(0.001);
//  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* outfile = new TFile(FileNameSim,"RECREATE");
  top->Write();      // use this as input to simulations (run_sim.C)
  outfile->Close();
  TFile* outfile = new TFile(FileNameGeo,"RECREATE");
  gGeoMan->Write();  // use this is you want GeoManager format in the output
  outfile->Close();

  dump_info_file();
  dump_digi_file();

  top->Draw("ogl");

  //top->Raytrace();

//  cout << "Press Return to exit" << endl;
//  cin.get();
//  exit();
}


//==============================================================
void dump_digi_file()
{
  TDatime  datetime;   // used to get timestamp

  const Double_t ActiveAreaX[2] = { DetectorSizeX[0] - 2 * FrameWidth[0],
                                    DetectorSizeX[1] - 2 * FrameWidth[1] };
  const Int_t NofSectors = 3;
  //  const Int_t NofPadsInRow[2]  = { 80, 128 };  // number of pads in rows
  const Int_t NofPadsInRow[2]  = { 80, 160 };  // number of pads in rows

  const Double_t PadHeightInSector[NofModuleTypes][NofSectors] =  // pad height
        { {  1.50,  1.50,  1.50 },   // module type 1
          {  2.75,  2.50,  2.75 },   // module type 2
          {  4.50,  4.50,  4.50 },   // module type 3
          {  6.75,  6.75,  6.75 },   // module type 4

          {  4.00,  3.75,  4.00 },   // module type 5
          {  8.00,  7.75,  8.00 },   // module type 6
//        { 11.00, 11.00, 11.00 },   // module type 7
          { 13.75, 13.75, 13.75 },   // module type 7
          { 18.50, 18.25, 18.50 } }; // module type 8

//          {  3.75,  4.00,  3.75 },   // module type 5
//          {  7.50,  7.75,  7.50 },   // module type 6
//          { 11.25, 11.50, 11.25 },   // module type 7
//          { 15.00, 15.50, 15.00 } }; // module type 8

  const Int_t NofRowsInSector[NofModuleTypes][NofSectors] =   // number of rows per sector
        { {  12,  12,  12 },         // module type 1
          {   8,   4,   8 },         // module type 2
          {   1,  10,   1 },         // module type 3
          {   2,   4,   2 },         // module type 4

          {  10,   8,  10 },         // module type 5
          {   3,   8,   3 },         // module type 6
//        {   3,   4,   3 },         // module type 7
          {   2,   4,   2 },         // module type 7
          {   1,   4,   1 } };       // module type 8

//          {  10,   4,  10 },         // module type 5
//          {   4,   4,   4 },         // module type 6
//          {   2,   4,   2 },         // module type 7
//          {   2,   2,   2 } };       // module type 8

  Double_t HeightOfSector[NofModuleTypes][NofSectors];
  Double_t PadWidth[NofModuleTypes];

  // calculate pad width
  for (Int_t im = 0; im < NofModuleTypes; im++)
    PadWidth[im] = ActiveAreaX[ModuleType[im]] / NofPadsInRow[ModuleType[im]];

  // calculate height of sectors
  for (Int_t im = 0; im < NofModuleTypes; im++)
    for (Int_t is = 0; is < NofSectors; is++)
      HeightOfSector[im][is] = NofRowsInSector[im][is] * PadHeightInSector[im][is];

  // check, if the entire module size is covered by pads
  for (Int_t im = 0; im < NofModuleTypes; im++)
    if (ActiveAreaX[ModuleType[im]] - (HeightOfSector[im][0] + HeightOfSector[im][1] + HeightOfSector[im][2]) != 0)
    {
      printf("WARNING: sector size does not add up to module size for module type %d\n", im+1);
      printf("%.2f = %.2f + %.2f + %.2f\n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][0], HeightOfSector[im][1], HeightOfSector[im][2]);
      exit();
    }

//==============================================================

  printf("writing trd pad information file: %s\n", FileNamePads.Data());

  FILE *ifile;
  ifile = fopen(FileNamePads.Data(),"w");

  if (ifile == NULL)
    {
      printf("error opening %s\n", FileNamePads.Data());
      exit(1);
    }

  fprintf(ifile,"//\n");
  fprintf(ifile,"//   TRD pad layout for geometry %s\n", tagVersion.Data());
  fprintf(ifile,"//\n");
  fprintf(ifile,"// automatically generated by Create_TRD_Geometry_%s%s.C\n", tagVersion.Data(), subVersion.Data());
  fprintf(ifile,"// created %d\n", datetime.GetDate());
  fprintf(ifile,"//\n");

  fprintf(ifile,"\n");
  fprintf(ifile,"#ifndef CBMTRDPADS_H\n");
  fprintf(ifile,"#define CBMTRDPADS_H\n");
  fprintf(ifile,"\n");
  fprintf(ifile,"Int_t fst1_sect_count = 3;\n");
  fprintf(ifile,"// array of pad geometries in the TRD (trd1mod[1-8])\n");
  fprintf(ifile,"// 8 modules  // 3 sectors  // 4 values \n");
  fprintf(ifile,"Float_t fst1_pad_type[8][3][4] =        \n");
//fprintf(ifile,"Double_t fst1_pad_type[8][3][4] =       \n");
  fprintf(ifile,"			 		 \n");

  for (Int_t im = 0; im < NofModuleTypes; im++)
  {
    if (im+1 == 5)
      fprintf(ifile,"//---\n\n");
    fprintf(ifile,"// module type %d\n", im+1);
    for (Int_t is = 0; is < NofSectors; is++)
    {
      if ((im == 0) && (is == 0))   fprintf(ifile,"  { { ");
      else if (is == 0)             fprintf(ifile,"    { ");
      else                          fprintf(ifile,"      ");
  
      fprintf(ifile,"{ %.1f, %5.2f, %.1f/%3d, %5.2f }", 
              ActiveAreaX[ModuleType[im]], HeightOfSector[im][is], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][is]);
  
      if ((im == NofModuleTypes-1) && (is == 2))   fprintf(ifile," } };");
      else if (is == 2)                            fprintf(ifile," },");
      else                                         fprintf(ifile,",");
  
      fprintf(ifile,"\n");
    }

    fprintf(ifile,"\n");
  }

  fprintf(ifile,"#endif\n");

//  Int_t im = 0;
//  fprintf(ifile,"// module type %d	 		   \n", im+1);
//  fprintf(ifile,"  { { { %.1f, %5.2f, %.1f/%3d, %5.2f },    \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][0], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][0]);
//  fprintf(ifile,"      { %.1f, %5.2f, %.1f/%3d, %5.2f },    \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][1], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][1]);
//  fprintf(ifile,"      { %.1f, %5.2f, %.1f/%3d, %5.2f } },  \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][2], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][2]);
//  fprintf(ifile,"\n");
//
//  for (Int_t im = 1; im < NofModuleTypes-1; im++)
//  {
//    fprintf(ifile,"// module type %d	 		     \n", im+1);
//    fprintf(ifile,"    { { %.1f, %5.2f, %.1f/%3d, %5.2f },    \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][0], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][0]);
//    fprintf(ifile,"      { %.1f, %5.2f, %.1f/%3d, %5.2f },    \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][1], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][1]);
//    fprintf(ifile,"      { %.1f, %5.2f, %.1f/%3d, %5.2f } },  \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][2], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][2]);
//    fprintf(ifile,"\n");
//  }
//
//  Int_t im = 7;
//  fprintf(ifile,"// module type %d	 		   \n", im+1);
//  fprintf(ifile,"    { { %.1f, %5.2f, %.1f/%3d, %5.2f },    \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][0], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][0]);
//  fprintf(ifile,"      { %.1f, %5.2f, %.1f/%3d, %5.2f },    \n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][1], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][1]);
//  fprintf(ifile,"      { %.1f, %5.2f, %.1f/%3d, %5.2f } } };\n", ActiveAreaX[ModuleType[im]], HeightOfSector[im][2], ActiveAreaX[ModuleType[im]], NofPadsInRow[ModuleType[im]], PadHeightInSector[im][2]);
//  fprintf(ifile,"\n");

  fclose(ifile);
}


void dump_info_file()
{
  TDatime  datetime;   // used to get timestamp

  Double_t z_first_layer = 2000;   // z position of first layer (front)
  Double_t z_last_layer  = 0;      // z position of last  layer (front)

  Double_t xangle;   // horizontal angle
  Double_t yangle;   // vertical   angle

  Double_t total_surface = 0;   // total surface
  Double_t total_actarea = 0;   // total active area

  Int_t    channels_per_module[NofModuleTypes+1] = { 0 };   // number of channels per module
  Int_t    channels_per_feb[NofModuleTypes+1]    = { 0 };   // number of channels per feb
  Int_t    asics_per_module[NofModuleTypes+1]    = { 0 };   // number of asics per module

  Int_t    total_modules[NofModuleTypes+1]       = { 0 };   // total number of modules
  Int_t    total_febs[NofModuleTypes+1]          = { 0 };   // total number of febs
  Int_t    total_asics[NofModuleTypes+1]         = { 0 };   // total number of asics
  Int_t    total_channels[NofModuleTypes+1]      = { 0 };   // total number of channels

  Int_t    total_channels_u = 0;  // total number of ultimate channels
  Int_t    total_channels_s = 0;  // total number of super    channels
  Int_t    total_channels_r = 0;  // total number of regular  channels

  printf("writing summary information file: %s\n", FileNameInfo.Data());

  FILE *ifile;
  ifile = fopen(FileNameInfo.Data(),"w");

  if (ifile == NULL)
    {
      printf("error opening %s\n", FileNameInfo.Data());
      exit(1);
    }

  fprintf(ifile,"#\n##   %s information file\n#\n\n", geoVersion.Data());

  fprintf(ifile,"# created %d\n\n", datetime.GetDate());

  // determine first and last TRD layer
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
  {
    if (ShowLayer[iLayer])
    {
      if (z_first_layer > LayerPosition[iLayer])
        z_first_layer = LayerPosition[iLayer];
      if (z_last_layer < LayerPosition[iLayer])
        z_last_layer  = LayerPosition[iLayer];
    }
  }

  fprintf(ifile,"# envelope\n");
  // Show extension of TRD
  fprintf(ifile,"%4d cm   start of TRD (z)\n", z_first_layer);
  fprintf(ifile,"%4d cm   end   of TRD (z)\n", z_last_layer + LayerThickness);
  fprintf(ifile,"\n");

  // Layer thickness
  fprintf(ifile,"# thickness\n");
  fprintf(ifile,"%4d cm   per single layer (z)\n", LayerThickness);
  fprintf(ifile,"\n");

  // Show extra gaps
  fprintf(ifile,"# extra gaps\n ");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      fprintf(ifile,"%3d ", LayerOffset[iLayer]);
  fprintf(ifile,"   extra gaps in z (cm)\n");
  fprintf(ifile,"\n");

  // Show layer flags
  fprintf(ifile,"# generated TRD layers\n ");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      fprintf(ifile,"%2d ", PlaneId[iLayer]);
  fprintf(ifile,"   planeID\n");
  fprintf(ifile,"\n");

  // Dimensions in x
  fprintf(ifile,"# dimensions in x\n");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      if (PlaneId[iLayer] < 5)
        fprintf(ifile,"%5d cm to %5d cm x-dimension of layer %2d\n", -3.5 * DetectorSizeX[1], 3.5 * DetectorSizeX[1], PlaneId[iLayer]);
      else if (PlaneId[iLayer] < 9)
        fprintf(ifile,"%5d cm to %5d cm x-dimension of layer %2d\n", -4.5 * DetectorSizeX[1], 4.5 * DetectorSizeX[1], PlaneId[iLayer]);
      else
        fprintf(ifile,"%5d cm to %5d cm x-dimension of layer %2d\n", -5.5 * DetectorSizeX[1], 5.5 * DetectorSizeX[1], PlaneId[iLayer]);
  fprintf(ifile,"\n");

  // Dimensions in y
  fprintf(ifile,"# dimensions in y\n");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      if (PlaneId[iLayer] < 5)
        fprintf(ifile,"%5d cm to %5d cm y-dimension of layer %2d\n", -2.5 * DetectorSizeY[1], 2.5 * DetectorSizeY[1], PlaneId[iLayer]);
      else if (PlaneId[iLayer] < 9)
        fprintf(ifile,"%5d cm to %5d cm y-dimension of layer %2d\n", -3.5 * DetectorSizeY[1], 3.5 * DetectorSizeY[1], PlaneId[iLayer]);
      else
        fprintf(ifile,"%5d cm to %5d cm y-dimension of layer %2d\n", -4.5 * DetectorSizeY[1], 4.5 * DetectorSizeY[1], PlaneId[iLayer]);
  fprintf(ifile,"\n");

  // Show layer positions
  fprintf(ifile,"# z-positions of layer front\n");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
  {
    if (ShowLayer[iLayer])
      fprintf(ifile,"%5d cm   z-position of layer %2d\n", LayerPosition[iLayer], PlaneId[iLayer]);
  }
  fprintf(ifile,"\n");

  // flags
  fprintf(ifile,"# flags\n");

  fprintf(ifile,"support structure is    : ");
  if (!IncludeSupports) fprintf(ifile,"NOT ");
  fprintf(ifile,"included\n");

  fprintf(ifile,"radiator is             : ");
  if (!IncludeRadiator) fprintf(ifile,"NOT ");
  fprintf(ifile,"included\n");

  fprintf(ifile,"lattice grid is         : ");
  if (!IncludeLattice ) fprintf(ifile,"NOT ");
  fprintf(ifile,"included\n");

  fprintf(ifile,"asics are               : ");
  if (!IncludeAsics   ) fprintf(ifile,"NOT ");
  fprintf(ifile,"included\n");

  fprintf(ifile,"front-end boards are    : ");
  if (!IncludeFebs    ) fprintf(ifile,"NOT ");
  fprintf(ifile,"included\n");

  fprintf(ifile,"GBTX readout boards are : ");
  if (!IncludeRobs    ) fprintf(ifile,"NOT ");
  fprintf(ifile,"included\n");

  fprintf(ifile,"\n");


  // module statistics
//  fprintf(ifile,"#\n##   modules\n#\n\n");
//  fprintf(ifile,"number of modules per type and layer:\n");
  fprintf(ifile,"# modules\n");

  for (Int_t iModule = 1; iModule <= NofModuleTypes; iModule++)
    fprintf(ifile,"     mod%1d", iModule);
  fprintf(ifile,"    total");

  fprintf(ifile,"\n---------------------------------------------------------------------------------\n");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
    {
      for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
      {
        fprintf(ifile," %8d", ModuleStats[iLayer][iModule]);
        total_modules[iModule] += ModuleStats[iLayer][iModule];  // sum up modules across layers
      }
      fprintf(ifile,"            layer %2d\n", PlaneId[iLayer]);
    }
  fprintf(ifile,"\n---------------------------------------------------------------------------------\n");

  // total statistics
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    fprintf(ifile," %8d", total_modules[iModule]);
    total_modules[NofModuleTypes] += total_modules[iModule];
  }
  fprintf(ifile," %8d", total_modules[NofModuleTypes]);
  fprintf(ifile,"   number of modules\n");

  // number of FEBs
  //  fprintf(ifile,"\n#\n##   febs\n#\n\n");
  fprintf(ifile,"# febs\n");

  fprintf(ifile," ");
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    if ((AsicsPerFeb[iModule] / 100) == 3)
      fprintf(ifile,"%8du", FebsPerModule[iModule]);
    else if ((AsicsPerFeb[iModule] / 100) == 2)
      fprintf(ifile,"%8ds", FebsPerModule[iModule]);
    else 
      fprintf(ifile,"%8d ", FebsPerModule[iModule]);
  }
  fprintf(ifile,"           FEBs per module\n");

  // FEB total per type
  total_febs[NofModuleTypes] = 0; // reset sum to 0
  fprintf(ifile," ");
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    if ((AsicsPerFeb[iModule] / 100) == 3)
    {
      total_febs[iModule] = total_modules[iModule] * FebsPerModule[iModule];
      fprintf(ifile,"%8du", total_febs[iModule]);
      total_febs[NofModuleTypes] += total_febs[iModule];
    }
    else
      fprintf(ifile,"         ");
  }
  fprintf(ifile,"%8d", total_febs[NofModuleTypes]);
  fprintf(ifile,"   ultimate  FEBs\n");

  // FEB total per type
  total_febs[NofModuleTypes] = 0; // reset sum to 0
  fprintf(ifile," ");
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    if ((AsicsPerFeb[iModule] / 100) == 2)
    {
      total_febs[iModule] = total_modules[iModule] * FebsPerModule[iModule];
      fprintf(ifile,"%8ds", total_febs[iModule]);
      total_febs[NofModuleTypes] += total_febs[iModule];
    }
    else
      fprintf(ifile,"         ");
  }
  fprintf(ifile,"%8d", total_febs[NofModuleTypes]);
  fprintf(ifile,"   super     FEBs\n");

  // FEB total per type
  total_febs[NofModuleTypes] = 0; // reset sum to 0
  fprintf(ifile," ");
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    if ((AsicsPerFeb[iModule] / 100) == 1)
    {
      total_febs[iModule] = total_modules[iModule] * FebsPerModule[iModule];
      fprintf(ifile,"%8d ", total_febs[iModule]);
      total_febs[NofModuleTypes] += total_febs[iModule];
    }
    else
      fprintf(ifile,"         ");
  }
  fprintf(ifile,"%8d", total_febs[NofModuleTypes]);
  fprintf(ifile,"   regular   FEBs\n");

  // FEB total over all types
  total_febs[NofModuleTypes] = 0; // reset sum to 0
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    total_febs[iModule] = total_modules[iModule] * FebsPerModule[iModule];
    fprintf(ifile," %8d", total_febs[iModule]);
    total_febs[NofModuleTypes] += total_febs[iModule];
  }
  fprintf(ifile," %8d", total_febs[NofModuleTypes]);
  fprintf(ifile,"   number of FEBs\n");


  // number of ASICs
  //  fprintf(ifile,"\n#\n##   asics\n#\n\n");
  fprintf(ifile,"# asics\n");

  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    fprintf(ifile," %8d", AsicsPerFeb[iModule] %100);
  }
  fprintf(ifile,"            ASICs per FEB\n");

  // ASICs per module
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    asics_per_module[iModule] = FebsPerModule[iModule] * (AsicsPerFeb[iModule] %100);
    fprintf(ifile," %8d", asics_per_module[iModule]);
  }
  fprintf(ifile,"            ASICs per module\n");

  // ASICs per module type
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    total_asics[iModule] = total_febs[iModule] * (AsicsPerFeb[iModule] %100);
    fprintf(ifile," %8d", total_asics[iModule]);
    total_asics[NofModuleTypes] += total_asics[iModule];
  }
  fprintf(ifile," %8d", total_asics[NofModuleTypes]);
  fprintf(ifile,"   number of ASICs\n");

  // number of channels
  fprintf(ifile,"# channels\n");

  // channels per module
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    if ((AsicsPerFeb[iModule] %100) == 16)
    {
      channels_per_feb[iModule] =  80 * 6;   // rows  // 84, if 63 of 64 ch used
      channels_per_module[iModule] = channels_per_feb[iModule] * FebsPerModule[iModule];
    }
    if ((AsicsPerFeb[iModule] %100) == 15)
    {
      channels_per_feb[iModule] =  80 * 6;   // rows
      channels_per_module[iModule] = channels_per_feb[iModule] * FebsPerModule[iModule];
    }
    if ((AsicsPerFeb[iModule] %100) == 10)
    {
      channels_per_feb[iModule] =  80 * 4;   // rows
      channels_per_module[iModule] = channels_per_feb[iModule] * FebsPerModule[iModule];
    }
    if ((AsicsPerFeb[iModule] %100) ==  5)
    {
      channels_per_feb[iModule] =  80 * 2;   // rows
      channels_per_module[iModule] = channels_per_feb[iModule] * FebsPerModule[iModule];
    }

    if ((AsicsPerFeb[iModule] %100) ==  8)
    {
      channels_per_feb[iModule] = 128 * 2;   // rows
      channels_per_module[iModule] = channels_per_feb[iModule] * FebsPerModule[iModule];
    }
  }

  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
    fprintf(ifile," %8d", channels_per_module[iModule]);
  fprintf(ifile,"            channels per module\n");

  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
    fprintf(ifile," %8d", channels_per_feb[iModule]);
  fprintf(ifile,"            channels per feb\n");

  // channels used
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    total_channels[iModule] = channels_per_module[iModule] * total_modules[iModule]; 
    fprintf(ifile," %8d", total_channels[iModule]);
    total_channels[NofModuleTypes] += total_channels[iModule];
  }
  fprintf(ifile," %8d", total_channels[NofModuleTypes]);
  fprintf(ifile,"   channels used\n");

  // channels available
  fprintf(ifile," ");
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
  {
    if ((AsicsPerFeb[iModule] / 100) == 3)
    {
      fprintf(ifile,"%8du", total_asics[iModule] * 32);
      total_channels_u += total_asics[iModule] * 32;
    }
    else if ((AsicsPerFeb[iModule] / 100) == 2)
    {
      fprintf(ifile,"%8ds", total_asics[iModule] * 32);
      total_channels_s += total_asics[iModule] * 32;
    }
    else 
    {
      fprintf(ifile,"%8d ", total_asics[iModule] * 32);
      total_channels_r += total_asics[iModule] * 32;
    }
  }
  fprintf(ifile,"%8d", total_asics[NofModuleTypes] * 32);
  fprintf(ifile,"   channels available\n");

  // channel ratio for u,s,r density
  fprintf(ifile," ");
  fprintf(ifile,"%7.1f%%u", (float)total_channels_u / (total_asics[NofModuleTypes] * 32) * 100);
  fprintf(ifile,"%7.1f%%s", (float)total_channels_s / (total_asics[NofModuleTypes] * 32) * 100);
  fprintf(ifile,"%7.1f%%r", (float)total_channels_r / (total_asics[NofModuleTypes] * 32) * 100);
  fprintf(ifile,"                                                        channel ratio\n");

  fprintf(ifile,"\n");
  fprintf(ifile,"%8.1f%%   channel efficiency\n", 1. * total_channels[NofModuleTypes] / (total_asics[NofModuleTypes] * 32) * 100);

  // total surface of TRD
  for (Int_t iModule = 0; iModule < NofModuleTypes; iModule++)
    if (iModule <= 3)     
    {
      total_surface += total_modules[iModule] * DetectorSizeX[0] / 100 * DetectorSizeY[0] / 100;
      total_actarea += total_modules[iModule] * (DetectorSizeX[0]-FrameWidth[0]) / 100 * (DetectorSizeY[0]-FrameWidth[0]) / 100;
    }
    else
    {
      total_surface += total_modules[iModule] * DetectorSizeX[1] / 100 * DetectorSizeY[1] / 100;
      total_actarea += total_modules[iModule] * (DetectorSizeX[1]-FrameWidth[1]) / 100 * (DetectorSizeY[1]-FrameWidth[1]) / 100;
    }
  fprintf(ifile,"\n");

  // summary
  fprintf(ifile,"%7.2f m2      total surface    \n", total_surface);
  fprintf(ifile,"%7.2f m2      total active area\n", total_actarea);
  fprintf(ifile,"%7.2f m3      total gas volume \n", total_actarea * gas_thickness / 100);  // convert cm to m for thickness

  fprintf(ifile,"%7.2f cm2/ch  average channel size\n", 100. * 100 * total_actarea / total_channels[NofModuleTypes]);
  fprintf(ifile,"%7.2f ch/m2   channels per m2 active area\n", 1. * total_channels[NofModuleTypes] / total_actarea);
  fprintf(ifile,"\n");

  // gas volume position
  fprintf(ifile,"# gas volume position\n");
  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
      fprintf(ifile,"%10.4f cm   position of gas volume - layer %2d\n", LayerPosition[iLayer] + LayerThickness/2. + gas_position,  PlaneId[iLayer]);
  fprintf(ifile,"\n");
   
  // angles
  fprintf(ifile,"# angles of acceptance\n");

  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
    {
      if (iLayer < 4)
      {	
	//        fprintf(ifile,"y %10.4f cm   x %10.4f cm\n", 2.5 * DetectorSizeY[1], 3.5 * DetectorSizeX[1]);
        yangle = atan(2.0 * DetectorSizeY[1] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
        xangle = atan(3.5 * DetectorSizeX[1] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
      }
      if ((iLayer >= 4) && (iLayer < 8))
      {	
	//        fprintf(ifile,"y %10.4f cm   x %10.4f cm\n", 3.5 * DetectorSizeY[1], 4.5 * DetectorSizeX[1]);
        yangle = atan(3.0 * DetectorSizeY[1] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
        xangle = atan(4.5 * DetectorSizeX[1] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
      }
      if ((iLayer >= 8) && (iLayer <10))
      {	
	//        fprintf(ifile,"y %10.4f cm   x %10.4f cm\n", 4.5 * DetectorSizeY[1], 5.5 * DetectorSizeX[1]);
        yangle = atan(4.0 * DetectorSizeY[1] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
        xangle = atan(5.5 * DetectorSizeX[1] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
      }
      fprintf(ifile,"v: %5.2f deg, h: %5.2f deg - vertical/horizontal - layer %2d\n", yangle, xangle, PlaneId[iLayer]);
    }  
  fprintf(ifile,"\n");

  // aperture
  fprintf(ifile,"# inner aperture\n");

  for (Int_t iLayer = 0; iLayer < MaxLayers; iLayer++)
    if (ShowLayer[iLayer])
    {
      if (iLayer < 4)
      {	
	//        fprintf(ifile,"y %10.4f cm   x %10.4f cm\n", 2.5 * DetectorSizeY[1], 3.5 * DetectorSizeX[1]);
        yangle = atan(1.0 * DetectorSizeY[0] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
        xangle = atan(1.0 * DetectorSizeX[0] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
      }
      if ((iLayer >= 4) && (iLayer < 8))
      {	
	//        fprintf(ifile,"y %10.4f cm   x %10.4f cm\n", 3.5 * DetectorSizeY[1], 4.5 * DetectorSizeX[1]);
        yangle = atan(1.0 * DetectorSizeY[0] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
        xangle = atan(1.0 * DetectorSizeX[0] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
      }
      if ((iLayer >= 8) && (iLayer <10))
      {	
	//        fprintf(ifile,"y %10.4f cm   x %10.4f cm\n", 4.5 * DetectorSizeY[1], 5.5 * DetectorSizeX[1]);
        yangle = atan(1.0 * DetectorSizeY[0] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
        xangle = atan(1.0 * DetectorSizeX[0] / (LayerPosition[iLayer] + LayerThickness/2. + padplane_position)) * 180. / acos(-1.);
      }
      fprintf(ifile,"v: %5.2f deg, h: %5.2f deg - vertical/horizontal - layer %2d\n", yangle, xangle, PlaneId[iLayer]);
    }  
  fprintf(ifile,"\n");

  fclose(ifile);
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

  FairGeoMedium* air              = geoMedia->getMedium(KeepingVolumeMedium);
  FairGeoMedium* pefoam20         = geoMedia->getMedium(RadiatorVolumeMedium);
  FairGeoMedium* G10              = geoMedia->getMedium(LatticeVolumeMedium);
  FairGeoMedium* kapton           = geoMedia->getMedium(KaptonVolumeMedium);
  FairGeoMedium* trdGas           = geoMedia->getMedium(GasVolumeMedium);
  FairGeoMedium* copper           = geoMedia->getMedium(PadCopperVolumeMedium);
  FairGeoMedium* carbon           = geoMedia->getMedium(CarbonVolumeMedium);
  FairGeoMedium* honeycomb        = geoMedia->getMedium(HoneycombVolumeMedium);
  FairGeoMedium* aluminium        = geoMedia->getMedium(AluminiumVolumeMedium);

//  FairGeoMedium* goldCoatedCopper = geoMedia->getMedium("goldcoatedcopper");
//  FairGeoMedium* polypropylene    = geoMedia->getMedium("polypropylene");
//  FairGeoMedium* mylar            = geoMedia->getMedium("mylar");

  geoBuild->createMedium(air);
  geoBuild->createMedium(pefoam20);
  geoBuild->createMedium(trdGas);
  geoBuild->createMedium(honeycomb);
  geoBuild->createMedium(carbon);
  geoBuild->createMedium(G10);
  geoBuild->createMedium(copper);
  geoBuild->createMedium(kapton);
  geoBuild->createMedium(aluminium);

//  geoBuild->createMedium(goldCoatedCopper);
//  geoBuild->createMedium(polypropylene);
//  geoBuild->createMedium(mylar);
}

TGeoVolume* create_trd_module_type(Int_t moduleType)
{
  Int_t type = ModuleType[moduleType - 1];
  Double_t sizeX = DetectorSizeX[type];
  Double_t sizeY = DetectorSizeY[type];
  Double_t frameWidth = FrameWidth[type];
  Double_t activeAreaX = sizeX - 2 * frameWidth;
  Double_t activeAreaY = sizeY - 2 * frameWidth;

  TGeoMedium* keepVolMed        = gGeoMan->GetMedium(KeepingVolumeMedium);
  TGeoMedium* radVolMed         = gGeoMan->GetMedium(RadiatorVolumeMedium);
  TGeoMedium* latticeVolMed     = gGeoMan->GetMedium(LatticeVolumeMedium);
  TGeoMedium* kaptonVolMed      = gGeoMan->GetMedium(KaptonVolumeMedium);
  TGeoMedium* gasVolMed         = gGeoMan->GetMedium(GasVolumeMedium);
  TGeoMedium* padcopperVolMed   = gGeoMan->GetMedium(PadCopperVolumeMedium);
  TGeoMedium* padpcbVolMed      = gGeoMan->GetMedium(PadPcbVolumeMedium);
  TGeoMedium* honeycombVolMed   = gGeoMan->GetMedium(HoneycombVolumeMedium);
  TGeoMedium* carbonVolMed      = gGeoMan->GetMedium(CarbonVolumeMedium);
//  TGeoMedium* mylarVolMed       = gGeoMan->GetMedium(MylarVolumeMedium);
//  TGeoMedium* electronicsVolMed = gGeoMan->GetMedium(ElectronicsVolumeMedium);
  TGeoMedium* frameVolMed       = gGeoMan->GetMedium(FrameVolumeMedium);
  TGeoMedium* febVolMed         = gGeoMan->GetMedium(FebVolumeMedium);
  TGeoMedium* asicVolMed        = gGeoMan->GetMedium(AsicVolumeMedium);
//  TGeoMedium* aluminiumVolMed   = gGeoMan->GetMedium(AluminiumVolumeMedium);

  TString name = Form("module%d", moduleType);
  TGeoVolume* module = new TGeoVolumeAssembly(name);


  if(IncludeRadiator)
   {
     // Radiator
     //   TGeoBBox* trd_radiator = new TGeoBBox("", activeAreaX /2., activeAreaY /2., radiator_thickness /2.);
     TGeoBBox* trd_radiator = new TGeoBBox("", sizeX /2., sizeY /2., radiator_thickness /2.);
     TGeoVolume* trdmod1_radvol = new TGeoVolume("radiator", trd_radiator, radVolMed);
     //     TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("module%d_radiator", moduleType), trd_radiator, radVolMed);
     //     TGeoVolume* trdmod1_radvol = new TGeoVolume(Form("trd1mod%dradiator", moduleType), trd_radiator, radVolMed);
     trdmod1_radvol->SetLineColor(kBlue);
     trdmod1_radvol->SetTransparency(70);  // (60);  // (70);  // set transparency for the TRD
     TGeoTranslation* trd_radiator_trans = new TGeoTranslation("", 0., 0., radiator_position);
     module->AddNode(trdmod1_radvol, 1, trd_radiator_trans);
   }

   // Lattice grid
   if(IncludeLattice) 
   {

     if (type==0)  // inner modules
     {
       //     printf("lattice type %d\n", type);
       // drift window - lattice grid - sprossenfenster
       TGeoBBox* trd_lattice_mod0_ho = new TGeoBBox("", sizeX/2.,                       lattice_o_width[type]/2.,       lattice_thickness/2.);                // horizontal outer
       TGeoBBox* trd_lattice_mod0_hi = new TGeoBBox("", sizeX/2.-lattice_o_width[type], lattice_i_width[type]/2.,       lattice_thickness/2.);                // horizontal inner
       TGeoBBox* trd_lattice_mod0_vo = new TGeoBBox("", lattice_o_width[type]/2.,       sizeX/2.-lattice_o_width[type], lattice_thickness/2.);                // vertical outer
       TGeoBBox* trd_lattice_mod0_vi = new TGeoBBox("", lattice_i_width[type]/2.,       0.20*activeAreaY/2.-lattice_i_width[type]/2., lattice_thickness/2.);  // vertical inner
       TGeoBBox* trd_lattice_mod0_vb = new TGeoBBox("", lattice_i_width[type]/2.,       0.20*activeAreaY/2.-lattice_i_width[type]/4., lattice_thickness/2.);  // vertical border 

       TGeoVolume* trd_lattice_mod0_vol_ho = new TGeoVolume("lattice0ho", trd_lattice_mod0_ho, latticeVolMed);
       TGeoVolume* trd_lattice_mod0_vol_hi = new TGeoVolume("lattice0hi", trd_lattice_mod0_hi, latticeVolMed);
       TGeoVolume* trd_lattice_mod0_vol_vo = new TGeoVolume("lattice0vo", trd_lattice_mod0_vo, latticeVolMed);
       TGeoVolume* trd_lattice_mod0_vol_vi = new TGeoVolume("lattice0vi", trd_lattice_mod0_vi, latticeVolMed);
       TGeoVolume* trd_lattice_mod0_vol_vb = new TGeoVolume("lattice0vb", trd_lattice_mod0_vb, latticeVolMed);

       trd_lattice_mod0_vol_ho->SetLineColor(kYellow);  // kBlue);  
       trd_lattice_mod0_vol_vo->SetLineColor(kYellow);	// kOrange);
       trd_lattice_mod0_vol_hi->SetLineColor(kYellow);	// kRed);   
       trd_lattice_mod0_vol_vi->SetLineColor(kYellow);  // kWhite); 
       trd_lattice_mod0_vol_vb->SetLineColor(kYellow);
    
       TGeoTranslation *tv010 = new TGeoTranslation("tv010", 0.,  (1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
       TGeoTranslation *tv015 = new TGeoTranslation("tv015", 0., -(1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);

       TGeoTranslation *th020 = new TGeoTranslation("th020",  (1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
       TGeoTranslation *th025 = new TGeoTranslation("th025", -(1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
    
       Double_t hypos0[4] = {  (0.60*activeAreaY/2.),
		               (0.20*activeAreaY/2.),
			      -(0.20*activeAreaY/2.),
                              -(0.60*activeAreaY/2.) };

       Double_t vxpos0[4] = {  (0.60*activeAreaX/2.),
			       (0.20*activeAreaX/2.),
                              -(0.20*activeAreaX/2.),
                              -(0.60*activeAreaX/2.) };
    
       Double_t vypos0[5] = {  (0.80*activeAreaY/2.+lattice_i_width[type]/4.),
			       (0.40*activeAreaY/2.)                         ,
			       (0.00*activeAreaY/2.)                         ,
                              -(0.40*activeAreaY/2.)                         ,
                              -(0.80*activeAreaY/2.+lattice_i_width[type]/4.) };

//       TGeoVolumeAssembly* trdmod0_lattice = new TGeoVolumeAssembly("mod0lattice"); // volume for lattice grid

       TGeoBBox* trd_lattice_mod0  = new TGeoBBox("", sizeX /2., sizeY /2., lattice_thickness /2.);
       TGeoVolume* trdmod0_lattice = new TGeoVolume("lat_grid_mod0", trd_lattice_mod0, keepVolMed);

       // outer frame
       trdmod0_lattice->AddNode(trd_lattice_mod0_vol_ho, 1, tv010);
       trdmod0_lattice->AddNode(trd_lattice_mod0_vol_ho, 2, tv015);

       trdmod0_lattice->AddNode(trd_lattice_mod0_vol_vo, 3, th020);
       trdmod0_lattice->AddNode(trd_lattice_mod0_vol_vo, 4, th025);

       // lattice piece number
       Int_t lat0_no = 5;

       // horizontal bars
       for (Int_t y = 0; y < 4; y++) 
       {
	 TGeoTranslation *t0xy = new TGeoTranslation("", 0, hypos0[y], 0);
         trdmod0_lattice->AddNode(trd_lattice_mod0_vol_hi, lat0_no, t0xy);
         lat0_no++;
       }

       // vertical bars
       for (Int_t x = 0; x < 4; x++) 
         for (Int_t y = 0; y < 5; y++) 
         {
	   TGeoTranslation *t0xy = new TGeoTranslation("", vxpos0[x], vypos0[y], 0);
           if ( (y==0) || (y==4) )
             trdmod0_lattice->AddNode(trd_lattice_mod0_vol_vb, lat0_no, t0xy);   // border piece
           else
             trdmod0_lattice->AddNode(trd_lattice_mod0_vol_vi, lat0_no, t0xy);   // middle piece
           lat0_no++;
         }

       // add lattice to module
       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
       module->AddNode(trdmod0_lattice, 1, trd_lattice_trans);
     }

//     else if (type==1)  // outer modules
//     {
//       //     printf("lattice type %d\n", type);
//       // drift window - lattice grid - sprossenfenster
//       TGeoBBox *trd_lattice_mod1_ho = new TGeoBBox("", sizeX/2.,                       lattice_o_width[type]/2.,       lattice_thickness/2.);                 // horizontal outer
//       TGeoBBox *trd_lattice_mod1_hi = new TGeoBBox("", sizeX/2.-lattice_o_width[type], lattice_i_width[type]/2.,       lattice_thickness/2.);                 // horizontal inner
//       TGeoBBox *trd_lattice_mod1_vo = new TGeoBBox("", lattice_o_width[type]/2.,       sizeX/2.-lattice_o_width[type], lattice_thickness/2.);                 // vertical outer  
//       TGeoBBox *trd_lattice_mod1_vi = new TGeoBBox("", lattice_i_width[type]/2.,       0.125*activeAreaY/2.-lattice_i_width[type]/2., lattice_thickness/2.);  // vertical inner  
//       TGeoBBox *trd_lattice_mod1_vb = new TGeoBBox("", lattice_i_width[type]/2.,       0.125*activeAreaY/2.-lattice_i_width[type]/4., lattice_thickness/2.);  // vertical border 
//
//       TGeoVolume* trd_lattice_mod1_vol_ho = new TGeoVolume("lattice1ho", trd_lattice_mod1_ho, latticeVolMed);
//       TGeoVolume* trd_lattice_mod1_vol_hi = new TGeoVolume("lattice1hi", trd_lattice_mod1_hi, latticeVolMed);
//       TGeoVolume* trd_lattice_mod1_vol_vo = new TGeoVolume("lattice1vo", trd_lattice_mod1_vo, latticeVolMed);
//       TGeoVolume* trd_lattice_mod1_vol_vi = new TGeoVolume("lattice1vi", trd_lattice_mod1_vi, latticeVolMed);
//       TGeoVolume* trd_lattice_mod1_vol_vb = new TGeoVolume("lattice1vb", trd_lattice_mod1_vb, latticeVolMed);
//
//       trd_lattice_mod1_vol_ho->SetLineColor(kYellow);  // kBlue);  
//       trd_lattice_mod1_vol_vo->SetLineColor(kYellow);	// kOrange);
//       trd_lattice_mod1_vol_hi->SetLineColor(kYellow);	// kRed);   
//       trd_lattice_mod1_vol_vi->SetLineColor(kYellow);  // kWhite); 
//       trd_lattice_mod1_vol_vb->SetLineColor(kYellow);
//    
//       TGeoTranslation *tv110 = new TGeoTranslation("tv110", 0.,  (1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
//       TGeoTranslation *tv118 = new TGeoTranslation("tv118", 0., -(1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
//    
//       TGeoTranslation *th120 = new TGeoTranslation("th120",  (1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
//       TGeoTranslation *th128 = new TGeoTranslation("th128", -(1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
//
//       Double_t hypos1[7] = {  (0.75*activeAreaY/2.),
//                               (0.50*activeAreaY/2.),
//			         (0.25*activeAreaY/2.),
//			         (0.00*activeAreaY/2.),
//                              -(0.25*activeAreaY/2.),
//			        -(0.50*activeAreaY/2.),
//			        -(0.75*activeAreaY/2.) };
//
//       Double_t vxpos1[7] = {  (0.75*activeAreaX/2.),
//                               (0.50*activeAreaX/2.),
//			         (0.25*activeAreaX/2.),
//			         (0.00*activeAreaX/2.),
//                              -(0.25*activeAreaX/2.),
//			        -(0.50*activeAreaX/2.),
//			        -(0.75*activeAreaX/2.) };
//
//       Double_t vypos1[8] = {  (0.875*activeAreaY/2.+lattice_i_width[type]/4.),
//                               (0.625*activeAreaY/2.)                         ,
//                               (0.375*activeAreaY/2.)                         ,
//                               (0.125*activeAreaY/2.)                         ,
//                              -(0.125*activeAreaY/2.)                         ,
//                              -(0.375*activeAreaY/2.)                         ,
//                              -(0.625*activeAreaY/2.)                         ,
//			        -(0.875*activeAreaY/2.+lattice_i_width[type]/4.) };
//    
////       TGeoVolumeAssembly* trdmod1_lattice = new TGeoVolumeAssembly("mod1lattice"); // volume for lattice grid
//
//       TGeoBBox* trd_lattice_mod1  = new TGeoBBox("", sizeX /2., sizeY /2., lattice_thickness /2.);
//       TGeoVolume* trdmod1_lattice = new TGeoVolume("lat_grid_mod1", trd_lattice_mod1, keepVolMed);
//
//       // outer frame
//       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_ho, 1, tv110);
//       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_ho, 2, tv118);
//
//       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vo, 3, th120);
//       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vo, 4, th128);
//
//       // lattice piece number
//       Int_t lat1_no = 5;
//
//       // horizontal bars
//       for (Int_t y = 0; y < 7; y++)
//	 {
//	   TGeoTranslation *t1xy = new TGeoTranslation("", 0, hypos1[y], 0);
//	   trdmod1_lattice->AddNode(trd_lattice_mod1_vol_hi, lat1_no, t1xy);
//	   lat1_no++;
//	 }
//
//       // vertical bars
//       for (Int_t x = 0; x < 7; x++)
//         for (Int_t y = 0; y < 8; y++)
//	   {
//	     TGeoTranslation *t1xy = new TGeoTranslation("", vxpos1[x], vypos1[y], 0);
//	     if ( (y==0) || (y==7) )
//	       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vb, lat1_no, t1xy);   // border piece
//	     else
//	       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vi, lat1_no, t1xy);   // middle piece
//	     lat1_no++;
//	   }
//
//       // add lattice to module
//       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
//       module->AddNode(trdmod1_lattice, 1, trd_lattice_trans);
//     }

     else if (type==1)  // 114 cm giant modules 
     {
       //     printf("lattice type %d\n", type);
       // drift window - lattice grid - sprossenfenster
       TGeoBBox *trd_lattice_mod1_ho = new TGeoBBox("", sizeX/2.,                       lattice_o_width[type]/2.,       lattice_thickness/2.);                 // horizontal outer
       TGeoBBox *trd_lattice_mod1_hi = new TGeoBBox("", sizeX/2.-lattice_o_width[type], lattice_i_width[type]/2.,       lattice_thickness/2.);                 // horizontal inner
       TGeoBBox *trd_lattice_mod1_vo = new TGeoBBox("", lattice_o_width[type]/2.,       sizeX/2.-lattice_o_width[type], lattice_thickness/2.);                 // vertical outer  
       TGeoBBox *trd_lattice_mod1_vi = new TGeoBBox("", lattice_i_width[type]/2.,       0.10*activeAreaY/2.-lattice_i_width[type]/2., lattice_thickness/2.);  // vertical inner  
       TGeoBBox *trd_lattice_mod1_vb = new TGeoBBox("", lattice_i_width[type]/2.,       0.10*activeAreaY/2.-lattice_i_width[type]/4., lattice_thickness/2.);  // vertical border 

       TGeoVolume* trd_lattice_mod1_vol_ho = new TGeoVolume("lattice1ho", trd_lattice_mod1_ho, latticeVolMed);
       TGeoVolume* trd_lattice_mod1_vol_hi = new TGeoVolume("lattice1hi", trd_lattice_mod1_hi, latticeVolMed);
       TGeoVolume* trd_lattice_mod1_vol_vo = new TGeoVolume("lattice1vo", trd_lattice_mod1_vo, latticeVolMed);
       TGeoVolume* trd_lattice_mod1_vol_vi = new TGeoVolume("lattice1vi", trd_lattice_mod1_vi, latticeVolMed);
       TGeoVolume* trd_lattice_mod1_vol_vb = new TGeoVolume("lattice1vb", trd_lattice_mod1_vb, latticeVolMed);

       trd_lattice_mod1_vol_ho->SetLineColor(kYellow);  // kBlue);  
       trd_lattice_mod1_vol_vo->SetLineColor(kYellow);	// kOrange);
       trd_lattice_mod1_vol_hi->SetLineColor(kYellow);	// kRed);   
       trd_lattice_mod1_vol_vi->SetLineColor(kYellow);  // kWhite); 
       trd_lattice_mod1_vol_vb->SetLineColor(kYellow);
    
       TGeoTranslation *tv110 = new TGeoTranslation("tv110", 0.,  (1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
       TGeoTranslation *tv118 = new TGeoTranslation("tv118", 0., -(1.00*activeAreaY/2.+lattice_o_width[type]/2.), 0);
    
       TGeoTranslation *th120 = new TGeoTranslation("th120",  (1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);
       TGeoTranslation *th128 = new TGeoTranslation("th128", -(1.00*activeAreaX/2.+lattice_o_width[type]/2.), 0., 0);

       Double_t hypos1[9] = {  (0.80*activeAreaY/2.),
                               (0.60*activeAreaY/2.),
			       (0.40*activeAreaY/2.),
			       (0.20*activeAreaY/2.),
			       (0.00*activeAreaY/2.),
                              -(0.20*activeAreaY/2.),
                              -(0.40*activeAreaY/2.),
			      -(0.60*activeAreaY/2.),
			      -(0.80*activeAreaY/2.) };

       Double_t vxpos1[9] = {  (0.80*activeAreaX/2.),
                               (0.60*activeAreaX/2.),
			       (0.40*activeAreaX/2.),
			       (0.20*activeAreaX/2.),
			       (0.00*activeAreaX/2.),
                              -(0.20*activeAreaX/2.),
                              -(0.40*activeAreaX/2.),
			      -(0.60*activeAreaX/2.),
			      -(0.80*activeAreaX/2.) };

       Double_t vypos1[10] = { (0.90*activeAreaY/2.+lattice_i_width[type]/4.),
                               (0.70*activeAreaY/2.)                         ,
                               (0.50*activeAreaY/2.)                         ,
                               (0.30*activeAreaY/2.)                         ,
                               (0.10*activeAreaY/2.)                         ,
                              -(0.10*activeAreaY/2.)                         ,
                              -(0.30*activeAreaY/2.)                         ,
                              -(0.50*activeAreaY/2.)                         ,
                              -(0.70*activeAreaY/2.)                         ,
			      -(0.90*activeAreaY/2.+lattice_i_width[type]/4.) };
    
//       TGeoVolumeAssembly* trdmod1_lattice = new TGeoVolumeAssembly("mod1lattice"); // volume for lattice grid

       TGeoBBox* trd_lattice_mod1  = new TGeoBBox("", sizeX /2., sizeY /2., lattice_thickness /2.);
       TGeoVolume* trdmod1_lattice = new TGeoVolume("lat_grid_mod1", trd_lattice_mod1, keepVolMed);

       // outer frame
       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_ho, 1, tv110);
       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_ho, 2, tv118);

       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vo, 3, th120);
       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vo, 4, th128);

       // lattice piece number
       Int_t lat1_no = 5;   // no 1-4 are already defined

       // horizontal bars
       for (Int_t y = 0; y < 9; y++)
	 {
	   TGeoTranslation *t1xy = new TGeoTranslation("", 0, hypos1[y], 0);
	   trdmod1_lattice->AddNode(trd_lattice_mod1_vol_hi, lat1_no, t1xy);
	   lat1_no++;
	 }

       // vertical bars
       for (Int_t x = 0; x < 9; x++)
         for (Int_t y = 0; y < 10; y++)
	   {
	     TGeoTranslation *t1xy = new TGeoTranslation("", vxpos1[x], vypos1[y], 0);
	     if ( (y==0) || (y==9) )
	       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vb, lat1_no, t1xy);   // border piece
	     else
	       trdmod1_lattice->AddNode(trd_lattice_mod1_vol_vi, lat1_no, t1xy);   // middle piece
	     lat1_no++;
	   }

       // add lattice to module
       TGeoTranslation *trd_lattice_trans = new TGeoTranslation("", 0., 0., lattice_position);
       module->AddNode(trdmod1_lattice, 1, trd_lattice_trans);
     }

   }  // with lattice grid 

   // Kapton Foil
   TGeoBBox* trd_kapton = new TGeoBBox("", sizeX /2., sizeY /2., kapton_thickness /2.);
   TGeoVolume* trdmod1_kaptonvol = new TGeoVolume("kaptonfoil", trd_kapton, kaptonVolMed);
   //   TGeoVolume* trdmod1_kaptonvol = new TGeoVolume(Form("module%d_kaptonfoil", moduleType), trd_kapton, kaptonVolMed);
   //   TGeoVolume* trdmod1_kaptonvol = new TGeoVolume(Form("trd1mod%dkapton", moduleType), trd_kapton, kaptonVolMed);
   trdmod1_kaptonvol->SetLineColor(kGreen);
   TGeoTranslation* trd_kapton_trans = new TGeoTranslation("", 0., 0., kapton_position);
   module->AddNode(trdmod1_kaptonvol, 1, trd_kapton_trans);

   // start of Frame in z
   // Gas
   TGeoBBox* trd_gas = new TGeoBBox("", activeAreaX /2., activeAreaY /2., gas_thickness /2.);
   TGeoVolume* trdmod1_gasvol = new TGeoVolume("gas", trd_gas, gasVolMed);
   //   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("module%d_gas", moduleType), trd_gas, gasVolMed);
   //   TGeoVolume* trdmod1_gasvol = new TGeoVolume(Form("trd1mod%dgas", moduleType), trd_gas, gasVolMed);
   //   trdmod1_gasvol->SetLineColor(kBlue);
   trdmod1_gasvol->SetLineColor(kGreen); // to avoid blue overlaps in the screenshots
   TGeoTranslation* trd_gas_trans = new TGeoTranslation("", 0., 0., gas_position);
   module->AddNode(trdmod1_gasvol, 1, trd_gas_trans);
   // end of Frame in z


   // frame1
   TGeoBBox* trd_frame1 = new TGeoBBox("", sizeX /2., frameWidth /2., frame_thickness/2.);
   TGeoVolume* trdmod1_frame1vol = new TGeoVolume("frame1", trd_frame1, frameVolMed);
   //   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("module%d_frame1", moduleType), trd_frame1, frameVolMed);
   //   TGeoVolume* trdmod1_frame1vol = new TGeoVolume(Form("trd1mod%dframe1", moduleType), trd_frame1, frameVolMed);
   trdmod1_frame1vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame1_trans = new TGeoTranslation("", 0., activeAreaY /2. + frameWidth /2., frame_position);
   module->AddNode(trdmod1_frame1vol, 1, trd_frame1_trans);
   trd_frame1_trans = new TGeoTranslation("", 0., -(activeAreaY /2. + frameWidth /2.), frame_position);
   module->AddNode(trdmod1_frame1vol, 2, trd_frame1_trans);


   // frame2
   TGeoBBox* trd_frame2 = new TGeoBBox("", frameWidth /2., activeAreaY /2., frame_thickness /2.);
   TGeoVolume* trdmod1_frame2vol = new TGeoVolume("frame2", trd_frame2, frameVolMed);
   //   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("module%d_frame2", moduleType), trd_frame2, frameVolMed);
   //   TGeoVolume* trdmod1_frame2vol = new TGeoVolume(Form("trd1mod%dframe2", moduleType), trd_frame2, frameVolMed);
   trdmod1_frame2vol->SetLineColor(kRed);

   // translations 
   TGeoTranslation* trd_frame2_trans = new TGeoTranslation("", activeAreaX /2. + frameWidth /2., 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 1, trd_frame2_trans);
   trd_frame2_trans = new TGeoTranslation("", -(activeAreaX /2. + frameWidth /2.), 0., frame_position);
   module->AddNode(trdmod1_frame2vol, 2, trd_frame2_trans);


   // Pad Copper
   TGeoBBox *trd_padcopper = new TGeoBBox("", sizeX /2., sizeY /2., padcopper_thickness /2.);
   TGeoVolume* trdmod1_padcoppervol = new TGeoVolume("padcopper", trd_padcopper, padcopperVolMed);
   //   TGeoVolume* trdmod1_padcoppervol = new TGeoVolume(Form("module%d_padcopper", moduleType), trd_padcopper, padcopperVolMed);
   //   TGeoVolume* trdmod1_padcoppervol = new TGeoVolume(Form("trd1mod%dpadcopper", moduleType), trd_padcopper, padcopperVolMed);
   trdmod1_padcoppervol->SetLineColor(kOrange);
   TGeoTranslation *trd_padcopper_trans = new TGeoTranslation("", 0., 0., padcopper_position);
   module->AddNode(trdmod1_padcoppervol, 1, trd_padcopper_trans);

   // Pad Plane
   TGeoBBox* trd_padpcb = new TGeoBBox("", sizeX /2., sizeY /2., padplane_thickness /2.);
   TGeoVolume* trdmod1_padpcbvol = new TGeoVolume("padplane", trd_padpcb, padpcbVolMed);
   //   TGeoVolume* trdmod1_padpcbvol = new TGeoVolume(Form("module%d_padplane", moduleType), trd_padpcb, padpcbVolMed);
   //   TGeoVolume* trdmod1_padpcbvol = new TGeoVolume(Form("trd1mod%dpadplane", moduleType), trd_padpcb, padpcbVolMed);
   trdmod1_padpcbvol->SetLineColor(kBlue);
   TGeoTranslation *trd_padpcb_trans = new TGeoTranslation("", 0., 0., padplane_position);
   module->AddNode(trdmod1_padpcbvol, 1, trd_padpcb_trans);

   // Honeycomb
   TGeoBBox* trd_honeycomb = new TGeoBBox("", sizeX /2., sizeY /2., honeycomb_thickness /2.);
   TGeoVolume* trdmod1_honeycombvol = new TGeoVolume("honeycomb", trd_honeycomb, honeycombVolMed);
   //   TGeoVolume* trdmod1_honeycombvol = new TGeoVolume(Form("module%d_honeycomb", moduleType), trd_honeycomb, honeycombVolMed);
   //   TGeoVolume* trdmod1_honeycombvol = new TGeoVolume(Form("trd1mod%dhoneycomb", moduleType), trd_honeycomb, honeycombVolMed);
   trdmod1_honeycombvol->SetLineColor(kOrange);
   TGeoTranslation* trd_honeycomb_trans = new TGeoTranslation("", 0., 0., honeycomb_position);
   module->AddNode(trdmod1_honeycombvol, 1, trd_honeycomb_trans);

   // Carbon fiber layers
   TGeoBBox* trd_carbon = new TGeoBBox("", sizeX /2., sizeY /2., carbon_thickness /2.);
   TGeoVolume* trdmod1_carbonvol = new TGeoVolume("carbonsheet", trd_carbon, carbonVolMed);
   //   TGeoVolume* trdmod1_carbonvol = new TGeoVolume(Form("module%d_carbonsheet", moduleType), trd_carbon, carbonVolMed);
   //   TGeoVolume* trdmod1_carbonvol = new TGeoVolume(Form("trd1mod%dcarbon", moduleType), trd_carbon, carbonVolMed);
   trdmod1_carbonvol->SetLineColor(kGreen);
   TGeoTranslation* trd_carbon_trans = new TGeoTranslation("", 0., 0., carbon_position);
   module->AddNode(trdmod1_carbonvol, 1, trd_carbon_trans);


   // FEBs
   if (IncludeFebs) 
   {
      // assemblies
      TGeoVolumeAssembly* trd_feb_vol = new TGeoVolumeAssembly("febvol");  // the mother volume of all FEBs
      TGeoVolumeAssembly* trd_feb_box = new TGeoVolumeAssembly("febbox");  // volume for inclined FEBs, then shifted along y
      //TGeoVolumeAssembly* trd_feb_vol = new TGeoVolumeAssembly(Form("module%d_febvol", moduleType));  // the mother volume of all FEBs
      //TGeoVolumeAssembly* trd_feb_box = new TGeoVolumeAssembly(Form("module%d_febbox", moduleType));  // volume for inclined FEBs, then shifted along y
      //TGeoVolumeAssembly* trd_feb_vol = new TGeoVolumeAssembly(Form("trd1mod%dfebvol", moduleType));  // the mother volume of all FEBs
      //TGeoVolumeAssembly* trd_feb_box = new TGeoVolumeAssembly(Form("trd1mod%dfebbox", moduleType));  // volume for inclined FEBs, then shifted along y

      // translations + rotations
      TGeoTranslation *trd_feb_trans1;     // center to corner
      TGeoRotation    *trd_feb_rotation;   // rotation around x axis
      TGeoTranslation *trd_feb_trans2;     // corner back
      TGeoTranslation *trd_feb_y_position; // shift to y position on TRD
//      TGeoTranslation *trd_feb_null;       // no displacement

// replaced by matrix operation (see below)
//  //      Double_t yback, zback;
//  //      TGeoCombiTrans  *trd_feb_placement;
//  //      // fix Z back offset 0.3 at some point
//  //      yback = -    sin(feb_rotation_angle/180*3.141)  * febvol_thickness /2.;
//  //      zback = - (1-cos(feb_rotation_angle/180*3.141)) * febvol_thickness /2. + 0.3;
//  //      trd_feb_placement = new TGeoCombiTrans(0, feb_pos_y + yback, zback, trd_feb_rotation);
//  //      trd_feb_box->AddNode(trdmod1_feb, iFeb+1, trd_feb_placement);

//      trd_feb_null       = new TGeoTranslation("", 0., 0., 0.);  // empty operation
      trd_feb_trans1     = new TGeoTranslation("", 0.,-feb_thickness/2.,-febvol_thickness/2.);  // move bottom right corner to center
      trd_feb_trans2     = new TGeoTranslation("", 0., feb_thickness/2., febvol_thickness/2.);  // move bottom right corner back
      trd_feb_rotation   = new TGeoRotation(); trd_feb_rotation->RotateX(feb_rotation_angle);

      TGeoHMatrix *incline_feb = new TGeoHMatrix("");

//        (*incline_feb) = (*trd_feb_null);        // OK
//        (*incline_feb) = (*trd_feb_y_position);  // OK
//        (*incline_feb) = (*trd_feb_trans1);      // OK
//        (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_y_position);  // OK
//        (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_trans2);      // OK
//        (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_rotation);    // OK
//        (*incline_feb) =  (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2) * (*trd_feb_y_position);  // not OK 
          // trd_feb_y_position is displaced in rotated coordinate system

      // matrix operation to rotate FEB PCB around its corner on the backanel 
      (*incline_feb) = (*trd_feb_trans1) * (*trd_feb_rotation) * (*trd_feb_trans2);  // OK

      // Create all FEBs and place them in an assembly which will be added to the TRD module
      TGeoBBox* trd_feb = new TGeoBBox("", activeAreaX/2., feb_thickness/2., febvol_thickness/2.);   // the FEB itself - as a cuboid
      TGeoVolume* trdmod1_feb = new TGeoVolume("feb", trd_feb, febVolMed);  // the FEB made of a certain medium
      //      TGeoVolume* trdmod1_feb = new TGeoVolume(Form("module%d_feb", moduleType), trd_feb, febVolMed);  // the FEB made of a certain medium
      //      TGeoVolume* trdmod1_feb = new TGeoVolume(Form("trd1mod%dfeb", moduleType), trd_feb, febVolMed);  // the FEB made of a certain medium
      trdmod1_feb->SetLineColor(kYellow);    // set yellow color
      trd_feb_box->AddNode(trdmod1_feb, 1, incline_feb);  
      // now we have an inclined FEB

      // ASICs
      Double_t asic_pos;
      Double_t asic_pos_x;
      TGeoTranslation *trd_asic_trans1;     // center to corner

      if (IncludeAsics) 
      {
        // put many ASICs on each inclined FEB
        TGeoBBox* trd_asic = new TGeoBBox("", asic_width/2., asic_thickness/2., asic_width/2.);              // ASIC dimensions
        // TODO: use Silicon as ASICs material
        TGeoVolume* trdmod1_asic = new TGeoVolume("asic", trd_asic, asicVolMed);   // the ASIC made of a certain medium
	//        TGeoVolume* trdmod1_asic = new TGeoVolume(Form("module%d_asic", moduleType), trd_asic, asicVolMed);   // the ASIC made of a certain medium
	//        TGeoVolume* trdmod1_asic = new TGeoVolume(Form("trd1mod%dasic", moduleType), trd_asic, asicVolMed);   // the ASIC made of a certain medium
        trdmod1_asic->SetLineColor(kBlue);                                                                   // set blue color for ASICs
  
        Int_t nofAsics   = AsicsPerFeb[ moduleType - 1 ] % 100;
        Int_t groupAsics = AsicsPerFeb[ moduleType - 1 ] / 100;   // either 1 or 2 or 3 (new ultimate)

        if ((nofAsics == 16) && (activeAreaX < 60))
          asic_distance = 0.0;  // for 57 cm  // 0.1;  // for 60 cm
        else
          asic_distance = 0.4;

        for (Int_t iAsic = 0; iAsic < (nofAsics / groupAsics); iAsic++) 
        {
          if (groupAsics == 1)   // single ASICs 
	  {
            asic_pos   = (iAsic + 0.5) / nofAsics - 0.5;   // equal spacing of ASICs on the FEB, e.g. for no=3 : -1/3, 0, +1/3

            // ASIC 1
            asic_pos_x = asic_pos * activeAreaX;
	    //            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2., 0.);  // move asic on top of FEB
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2.+asic_offset, 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, iAsic+1, incline_asic);  // now we have ASICs on the inclined FEB
          }

          if (groupAsics == 2)   // pairs of ASICs
	  {
            asic_pos   = (iAsic + 0.5) / (nofAsics / groupAsics) - 0.5;   // equal spacing of ASICs on the FEB, e.g. for no=3 : -1/3, 0, +1/3

            // ASIC 1
            asic_pos_x = asic_pos * activeAreaX + (0.5 + asic_distance/2.) * asic_width;
	    //            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2., 0.);  // move asic on top of FEB
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2.+asic_offset, 0.);  // move asic on top of FEB);
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 2*iAsic+1, incline_asic);  // now we have ASICs on the inclined FEB

            // ASIC 2
            asic_pos_x = asic_pos * activeAreaX - (0.5 + asic_distance/2.) * asic_width;
	    //            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2., 0.);  // move asic on top of FEB
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2.+asic_offset, 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 2*iAsic+2, incline_asic);  // now we have ASICs on the inclined FEB
          }

          if (groupAsics == 3)   // triplets of ASICs
	  {
            asic_pos   = (iAsic + 0.5) / (nofAsics / groupAsics) - 0.5;   // equal spacing of ASICs on the FEB, e.g. for no=3 : -1/3, 0, +1/3

            // ASIC 1
            asic_pos_x = asic_pos * activeAreaX + 1.1 * asic_width; // (0.5 + asic_distance/2.) * asic_width;
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2.+asic_offset, 0.);  // move asic on top of FEB);
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 3*iAsic+1, incline_asic);  // now we have ASICs on the inclined FEB

            // ASIC 2
            asic_pos_x = asic_pos * activeAreaX;
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2.+asic_offset, 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 3*iAsic+2, incline_asic);  // now we have ASICs on the inclined FEB

            // ASIC 3
            asic_pos_x = asic_pos * activeAreaX - 1.1 * asic_width; // (0.5 + asic_distance/2.) * asic_width;
            trd_asic_trans1     = new TGeoTranslation("", asic_pos_x, feb_thickness/2.+asic_thickness/2.+asic_offset, 0.);  // move asic on top of FEB
            TGeoHMatrix *incline_asic = new TGeoHMatrix("");
            (*incline_asic) = (*trd_asic_trans1) * (*incline_feb);
            trd_feb_box->AddNode(trdmod1_asic, 3*iAsic+3, incline_asic);  // now we have ASICs on the inclined FEB
          }
 
        }
        // now we have an inclined FEB with ASICs
      }


      // now go on with FEB placement
      Double_t feb_pos;
      Double_t feb_pos_y;

      Int_t nofFebs = FebsPerModule[ moduleType - 1 ];
      for (Int_t iFeb = 0; iFeb < nofFebs; iFeb++) 
      {
        feb_pos   = (iFeb + 0.5) / nofFebs - 0.5;   // equal spacing of FEBs on the backpanel
        feb_pos_y = feb_pos * activeAreaY;

        // shift inclined FEB in y to its final position
        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, feb_z_offset);  // with additional fixed offset in z direction
	//        trd_feb_y_position = new TGeoTranslation("", 0., feb_pos_y, 0.0);  // touching the backpanel with the corner
        trd_feb_vol->AddNode(trd_feb_box, iFeb+1, trd_feb_y_position);  // position FEB in y

      }

      if (IncludeRobs) 
      {
        // GBTx ROBs
        Double_t rob_size_x =  9.0; //  4.5; //  45 mm
        Double_t rob_size_y = 20.0; // 13.0; // 130 mm
        Double_t rob_thickness = feb_thickness;
  
        TGeoVolumeAssembly* trd_rob_box = new TGeoVolumeAssembly("robbox");  // volume for inclined FEBs, then shifted along y
        TGeoBBox* trd_rob = new TGeoBBox("", rob_size_x/2., rob_size_y/2., rob_thickness/2.);   // the ROB itself
        TGeoVolume* trdmod1_rob = new TGeoVolume("rob", trd_rob, febVolMed);  // the ROB made of a certain medium
        trdmod1_rob->SetLineColor(kRed);    // set color
  
        //      TGeoHMatrix *incline_rob = new TGeoHMatrix("");
        trd_rob_box->AddNode(trdmod1_rob, 1);//, "" ); // incline_feb);  
  
  
  
        // GBTXs
        Double_t gbtx_pos;
        Double_t gbtx_pos_x;
        Double_t gbtx_pos_y;
        TGeoTranslation *trd_gbtx_trans1;     // center to corner
  
        // GBTX parameters
        const Double_t gbtx_thickness = 0.25; // 2.5 mm
        const Double_t gbtx_width     = 3.0;  // 2.0;  1.0;   // 1 cm
  
        // put many GBTXs on each inclined FEB
        TGeoBBox* trd_gbtx = new TGeoBBox("", gbtx_width/2., gbtx_width/2., gbtx_thickness/2.); // GBTX dimensions
        TGeoVolume* trdmod1_gbtx = new TGeoVolume("gbtx", trd_gbtx, asicVolMed);   // the GBTX made of a certain medium
        trdmod1_gbtx->SetLineColor(kGreen);  // set color for GBTXs
  
        Int_t nofGbtxs   = GbtxPerRob[ moduleType - 1 ] % 100;
        Int_t groupGbtxs = GbtxPerRob[ moduleType - 1 ] / 100;   // usually 1
  
  //      nofGbtxs   = 7;
  //      groupGbtxs = 1;
  
        Int_t nofGbtxX = 2;
        Int_t nofGbtxY = (nofGbtxs - 1) / 2. + 1; // +1 is for GBTx master
  
        Double_t gbtx_distance = 0.4;
        Int_t iGbtx = 1;
  
        for (Int_t iGbtxY = 0; iGbtxY < nofGbtxY; iGbtxY++) 
        {
  
          gbtx_pos   = (iGbtxY + 0.5) / nofGbtxY - 0.5;   // equal spacing of GBTXs on the FEB, e.g. for no=3 : -1/3, 0, +1/3
          gbtx_pos_y = -gbtx_pos * rob_size_y;
  
          if (iGbtxY > 0)
            for (Int_t iGbtxX = 0; iGbtxX < nofGbtxX; iGbtxX++) 
            {
              gbtx_pos   = (iGbtxX + 0.5) / nofGbtxX - 0.5;   // equal spacing of GBTXs on the FEB, e.g. for no=3 : -1/3, 0, +1/3
              gbtx_pos_x = gbtx_pos * rob_size_x;
  
              trd_gbtx_trans1     = new TGeoTranslation("", gbtx_pos_x, gbtx_pos_y, rob_thickness/2.+gbtx_thickness/2.);  // move gbtx on top of ROB
              trd_rob_box->AddNode(trdmod1_gbtx, iGbtx++, trd_gbtx_trans1);  // now we have GBTXs on the ROB
            }
          else
            {
              gbtx_pos_x = 0;
     
              trd_gbtx_trans1     = new TGeoTranslation("", gbtx_pos_x, gbtx_pos_y, rob_thickness/2.+gbtx_thickness/2.);  // move gbtx on top of ROB
              trd_rob_box->AddNode(trdmod1_gbtx, iGbtx++, trd_gbtx_trans1);  // now we have GBTXs on the ROB
            }
  
        }

        // now go on with ROB placement
        Double_t rob_pos;
        Double_t rob_pos_y;
        TGeoTranslation *trd_rob_y_position; // shift to y position on TRD
  
        Int_t nofRobs = RobsPerModule[ moduleType - 1 ];
        for (Int_t iRob = 0; iRob < nofRobs; iRob++) 
        {
          rob_pos   = (iRob + 0.5) / nofRobs - 0.5;   // equal spacing of ROBs on the backpanel
          rob_pos_y = rob_pos * activeAreaY;
  
          // shift inclined ROB in y to its final position
          trd_rob_y_position = new TGeoTranslation("", 0., rob_pos_y, febvol_thickness/2. - rob_thickness);  // approximate pos at end of feb volume
          trd_feb_vol->AddNode(trd_rob_box, iRob+1, trd_rob_y_position);  // position FEB in y
  
        }

      }  // IncludeGbtx

      // put FEB box on module
      TGeoTranslation* trd_febvol_trans = new TGeoTranslation("", 0., 0., febvol_position);
      gGeoMan->GetVolume(name)->AddNode(trd_feb_vol, 1, trd_febvol_trans);  // put febvol at correct z position wrt to the module
   }

   return module;
}

Int_t copy_nr(Int_t stationNr, Int_t copyNr, Int_t isRotated, Int_t planeNr, Int_t modinplaneNr)
{
  if (modinplaneNr > 128)
    printf("Warning: too many modules in this layer %02d (max 128 according to CbmTrdAddress)\n", planeNr);

  return (stationNr      * 100000000    // 1 digit
        + copyNr         *   1000000    // 2 digit
        + isRotated      *    100000    // 1 digit
        + planeNr        *      1000    // 2 digit
        + modinplaneNr   *         1 ); // 3 digit
}

void create_detector_layers(Int_t layerId)
{
  Int_t module_id = 0;
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

// add layer keeping volume
  TString layername = Form("layer%02d", PlaneId[layerId]);
  TGeoVolume* layer = new TGeoVolumeAssembly(layername);

  // compute layer copy number
  Int_t i = LayerType[layerId] / 10   * 10000   // 1 digit  // fStation
          + LayerType[layerId] % 10   *  1000   // 1 digit  // isRotated
          + LayerNrInStation[layerId] *   100   // 1 digit  // fLayer
          + PlaneId[layerId];                   // 2 digits // fPlane   // layer type as leading digit in copy number of layer
  gGeoMan->GetVolume(geoVersion)->AddNode(layer, i);

//  Int_t i = 100 + PlaneId[layerId];
//  gGeoMan->GetVolume(geoVersion)->AddNode(layer, 1);
//  cout << layername << endl;

  Double_t ExplodeScale = 1.00;
  if(DoExplode)  // if explosion, set scale
    ExplodeScale = ExplodeFactor;

  Int_t modId = 0;  // module id, only within this layer
  
  Int_t copyNrIn[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 1; type <= 4; type++) {
    for ( Int_t j = (innerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < innerarray_size2; i++) {
        module_id = *(innerLayer + (j * innerarray_size2 + i));
        if ( module_id /100 == type) {
//          Int_t y = -(j-2);
//          Int_t x =   i-2;
          Float_t y = -(j-1.5);
          Float_t x =   i-2.5;

          // displacement
          Double_t dx = 0;
          Double_t dy = 0;
          Double_t dz = 0;

          if(DisplaceRandom)
	  {
            dx = (r3.Rndm()-.5) * 2 * maxdx;  // max +- 0.1 cm shift
            dy = (r3.Rndm()-.5) * 2 * maxdy;  // max +- 0.1 cm shift
            dz = (r3.Rndm()-.5) * 2 * maxdz;  // max +- 1.0 cm shift
          }

          Double_t xPos = DetectorSizeX[0] * x * ExplodeScale + dx;
          Double_t yPos = DetectorSizeY[0] * y * ExplodeScale + dy;
          copyNrIn[type - 1]++;
          modId++;

          // statistics per layer and module type
          ModuleStats[layerId][type - 1]++;

//          Int_t copy = copy_nr_modid(stationNr, layerNrInStation, copyNrIn[type - 1], PlaneId[layerId], modId);  // with modID
//          Int_t copy = copy_nr(stationNr, copyNrIn[type - 1], isRotated, PlaneId[layerId], modId);

          // take care of FEB orientation - away from beam
          Int_t copy = 0;
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
	  {
            copy = copy_nr(stationNr, copyNrIn[type - 1], module_id /10 %10, PlaneId[layerId], modId);
   	    module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate module by   0 or 180 degrees, see layer[1-3][i,o] - vertical pads
	  }
          else  // layer 2,4 ...
	  {
            copy = copy_nr(stationNr, copyNrIn[type - 1], module_id %10    , PlaneId[layerId], modId);
   	    module_rotation->RotateZ( (module_id %10) * 90. );      // rotate module by  90 or 270 degrees, see layer[1-3][i,o] - horizontal pads
	  }

          // rotation
          Double_t drotx = 0;
          Double_t droty = 0;
          Double_t drotz = 0;

          if(RotateRandom)
	  {
            drotx = (r3.Rndm()-.5) * 2 * maxdrotx;
            droty = (r3.Rndm()-.5) * 2 * maxdroty;
            drotz = (r3.Rndm()-.5) * 2 * maxdrotz;

            module_rotation->RotateZ( drotz );
            module_rotation->RotateY( droty );
            module_rotation->RotateX( drotx );
          }

          TGeoCombiTrans* module_placement = new TGeoCombiTrans(xPos, yPos, LayerPosition[layerId] + LayerThickness/2 + dz, module_rotation);  // shift by half layer thickness
//          gGeoMan->GetVolume(geoVersion)->AddNode(gModules[type - 1], copy, module_placement);
// add module to layer
          gGeoMan->GetVolume(layername)->AddNode(gModules[type - 1], copy, module_placement);
//
        }
      }
    }
  }
  
  Int_t copyNrOut[4] = { 0, 0, 0, 0 }; // copy number for each module type
  for ( Int_t type = 5; type <= 8; type++) {
    for ( Int_t j = (outerarray_size1-1); j >= 0; j--)  { // start from the bottom 
      for ( Int_t i = 0; i < outerarray_size2; i++) {
	module_id = *(outerLayer + (j * outerarray_size2 + i));
        if ( module_id /100 == type) {
//          Int_t y = -(j-4);
//          Int_t x =   i-5;
          Float_t y = -(j-3.5);
          Float_t x =   i-5;

          // displacement
          Double_t dx = 0;
          Double_t dy = 0;
          Double_t dz = 0;

          if(DisplaceRandom)
	  {
            dx = (r3.Rndm()-.5) * 2 * maxdx;  // max +- 0.1 cm shift
            dy = (r3.Rndm()-.5) * 2 * maxdy;  // max +- 0.1 cm shift
            dz = (r3.Rndm()-.5) * 2 * maxdz;  // max +- 1.0 cm shift
          }

          Double_t xPos = DetectorSizeX[1] * x * ExplodeScale + dx;
          Double_t yPos = DetectorSizeY[1] * y * ExplodeScale + dy;
          copyNrOut[type - 5]++;
          modId++;

          // statistics per layer and module type
          ModuleStats[layerId][type - 1]++;

//          Int_t copy = copy_nr_modid(stationNr, layerNrInStation, copyNrOut[type - 5],  PlaneId[layerId], modId);  // with modID
//          Int_t copy = copy_nr(stationNr, copyNrOut[type - 5], isRotated, PlaneId[layerId], modId);

          // take care of FEB orientation - away from beam
          Int_t copy = 0;
          module_rotation = new TGeoRotation();   // need to renew rotation to start from 0 degree angle
          if ( isRotated == 0 )  // layer 1,3 ...
	  {
            copy = copy_nr(stationNr, copyNrOut[type - 5], module_id /10 %10, PlaneId[layerId], modId);
            module_rotation->RotateZ( (module_id /10 %10) * 90. );  // rotate module by   0 or 180 degrees, see layer[1-3][i,o] - vertical pads
	  }
          else  // layer 2,4 ...
	  {
            copy = copy_nr(stationNr, copyNrOut[type - 5], module_id %10    , PlaneId[layerId], modId);
            module_rotation->RotateZ( (module_id %10) * 90. );      // rotate module by  90 or 270 degrees, see layer[1-3][i,o] - horizontal pads
	  }
    
          // rotation
          Double_t drotx = 0;
          Double_t droty = 0;
          Double_t drotz = 0;

          if(RotateRandom)
	  {
            drotx = (r3.Rndm()-.5) * 2 * maxdrotx;
            droty = (r3.Rndm()-.5) * 2 * maxdroty;
            drotz = (r3.Rndm()-.5) * 2 * maxdrotz;

            module_rotation->RotateZ( drotz );
            module_rotation->RotateY( droty );
            module_rotation->RotateX( drotx );
          }

          TGeoCombiTrans* module_placement = new TGeoCombiTrans(xPos, yPos, LayerPosition[layerId] + LayerThickness/2 + dz, module_rotation);  // shift by half layer thickness
//          gGeoMan->GetVolume(geoVersion)->AddNode(gModules[type - 1], copy, module_placement);
// add module to layer
          gGeoMan->GetVolume(layername)->AddNode(gModules[type - 1], copy, module_placement);
//
        }
      }
    }
  }  
}


void create_mag_field_vector()
{
  const TString cbmfield_01 = "cbm_field";
  TGeoVolume* cbmfield_1 = new TGeoVolumeAssembly(cbmfield_01);

  TGeoMedium* copperVolMed   = gGeoMan->GetMedium(PadCopperVolumeMedium);  // define Volume Medium

  TGeoRotation  *rotx090 = new TGeoRotation("rotx090"); rotx090->RotateX(  90.); // rotate  90 deg around x-axis                     
  TGeoRotation  *rotx270 = new TGeoRotation("rotx270"); rotx270->RotateX( 270.); // rotate 270 deg around x-axis                     

  Int_t tube_length = 500;
  Int_t cone_length = 120;
  Int_t cone_width  = 280;

  // field tube
  TGeoTube* trd_field = new TGeoTube("", 0., 100/2., tube_length/2.);
  TGeoVolume* trdmod1_fieldvol = new TGeoVolume("tube", trd_field, copperVolMed);
  trdmod1_fieldvol->SetLineColor(kRed);
  trdmod1_fieldvol->SetTransparency(30);   // transparency for the TRD
  TGeoTranslation* trd_field_trans = new TGeoTranslation("", 0., 0., 0.);   // tube position
  cbmfield_1->AddNode(trdmod1_fieldvol, 1, trd_field_trans);

  // field cone
  TGeoCone* trd_cone = new TGeoCone("", cone_length/2., 0., cone_width/2., 0., 0.);
  TGeoVolume* trdmod1_conevol = new TGeoVolume("cone", trd_cone, copperVolMed);
  trdmod1_conevol->SetLineColor(kRed);
  trdmod1_conevol->SetTransparency(30);   // transparency for the TRD
  TGeoTranslation* trd_cone_trans = new TGeoTranslation("", 0., 0., (tube_length+cone_length)/2.);   // cone position
  cbmfield_1->AddNode(trdmod1_conevol, 1, trd_cone_trans);

  TGeoCombiTrans* field_combi01 = new TGeoCombiTrans(0., 0., 40., rotx270);   // point in +y direction
  gGeoMan->GetVolume(geoVersion)->AddNode(cbmfield_1, 1, field_combi01);

  //   TGeoCombiTrans* field_combi02 = new TGeoCombiTrans( 200., 0., 0., rotx090);   // point in -y direction
  //   gGeoMan->GetVolume(geoVersion)->AddNode(cbmfield_1, 2, field_combi02);
}


void create_xtru_supports()
{
  const TString trd_01 = "support_trd1";
  TGeoVolume* trd_1 = new TGeoVolumeAssembly(trd_01);

  const TString trd_02 = "support_trd2";
  TGeoVolume* trd_2 = new TGeoVolumeAssembly(trd_02);

  const TString trd_03 = "support_trd3";
  TGeoVolume* trd_3 = new TGeoVolumeAssembly(trd_03);

//  const TString trdSupport = "supportframe";
//  TGeoVolume* trdsupport = new TGeoVolumeAssembly(trdSupport);
//
//  trdsupport->AddNode(trd_1, 1);
//  trdsupport->AddNode(trd_2, 2);
//  trdsupport->AddNode(trd_3, 3);

  TGeoMedium* aluminiumVolMed   = gGeoMan->GetMedium(AluminiumVolumeMedium);  // define Volume Medium

  const Double_t x[12] = { -15,-15, -1, -1,-15,-15, 15, 15,  1,  1, 15, 15 };  // IPB 400
  const Double_t y[12] = { -20,-18,-18, 18, 18, 20, 20, 18, 18,-18,-18,-20 };  // 30 x 40 cm in size, 2 cm wall thickness
  const Double_t Hwid = -2*x[0];  // 30  
  const Double_t Hhei = -2*y[0];  // 40

  Double_t AperX[3] = { 450., 550., 600.};  // inner aperture in X of support structure for stations 1,2,3
  Double_t AperY[3] = { 350., 450., 500.};  // inner aperture in Y of support structure for stations 1,2,3
  Double_t PilPosX;
  Double_t BarPosY;

  const Double_t BeamHeight = 570;  // beamline is at 5.7m above floor

  Double_t PilPosZ[6];  // PilPosZ
//  PilPosZ[0] = LayerPosition[0] + LayerThickness/2.;
//  PilPosZ[1] = LayerPosition[3] + LayerThickness/2.;
//  PilPosZ[2] = LayerPosition[4] + LayerThickness/2.;
//  PilPosZ[3] = LayerPosition[7] + LayerThickness/2.;
//  PilPosZ[4] = LayerPosition[8] + LayerThickness/2.;
//  PilPosZ[5] = LayerPosition[9] + LayerThickness/2.;

  PilPosZ[0] = LayerPosition[0] + 15;
  PilPosZ[1] = LayerPosition[3] - 15 + LayerThickness;
  PilPosZ[2] = LayerPosition[4] + 15;		      
  PilPosZ[3] = LayerPosition[7] - 15 + LayerThickness;
  PilPosZ[4] = LayerPosition[8] + 15;		      
  PilPosZ[5] = LayerPosition[9] - 15 + LayerThickness;

//  cout << "PilPosZ[0]: " << PilPosZ[0] << endl;
//  cout << "PilPosZ[1]: " << PilPosZ[1] << endl;

  TGeoRotation  *rotx090 = new TGeoRotation("rotx090"); rotx090->RotateX( 90.); // rotate  90 deg around x-axis                     
  TGeoRotation  *roty090 = new TGeoRotation("roty090"); roty090->RotateY( 90.); // rotate  90 deg around y-axis                     
  TGeoRotation  *rotz090 = new TGeoRotation("rotz090"); rotz090->RotateZ( 90.); // rotate  90 deg around y-axis                     
  TGeoRotation  *roty270 = new TGeoRotation("roty270"); roty270->RotateY(270.); // rotate 270 deg around y-axis                     

  TGeoRotation  *rotzx01 = new TGeoRotation("rotzx01"); 
  rotzx01->RotateZ(  90.); // rotate  90 deg around z-axis
  rotzx01->RotateX(  90.); // rotate  90 deg around x-axis

//  TGeoRotation  *rotxz01 = new TGeoRotation("rotxz01"); 
//  rotxz01->RotateX(  90.); // rotate  90 deg around x-axis
//  rotxz01->RotateZ(  90.); // rotate  90 deg around z-axis

  Double_t ang1 = atan(3./4.) * 180. / acos(-1.);
  //  cout << "DEDE " << ang1 << endl;
  //  Double_t sin1 = acos(-1.);
  //  cout << "DEDE " << sin1 << endl;
  TGeoRotation  *rotx080 = new TGeoRotation("rotx080"); rotx080->RotateX( 90.-ang1); // rotate  80 deg around x-axis                     
  TGeoRotation  *rotx100 = new TGeoRotation("rotx100"); rotx100->RotateX( 90.+ang1); // rotate 100 deg around x-axis                     

  TGeoRotation  *rotxy01 = new TGeoRotation("rotxy01"); 
  rotxy01->RotateX(  90.); // rotate  90 deg around x-axis                     
  rotxy01->RotateZ(-ang1); // rotate  ang1   around rotated y-axis                     

  TGeoRotation  *rotxy02 = new TGeoRotation("rotxy02"); 
  rotxy02->RotateX(  90.); // rotate  90 deg around x-axis                     
  rotxy02->RotateZ( ang1); // rotate  ang1   around rotated y-axis                     


//-------------------
// vertical pillars (Y)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoXtru* trd_H_vert1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_vert1->DefinePolygon(12,x,y);
      trd_H_vert1->DefineSection( 0,-(AperY[0]+Hhei), 0, 0, 1.0);
      trd_H_vert1->DefineSection( 1, BeamHeight,      0, 0, 1.0);  
      TGeoVolume* trd_H_vert_vol1 = new TGeoVolume("trd_H_y_01", trd_H_vert1, aluminiumVolMed);
      trd_H_vert_vol1->SetLineColor(kYellow);
      PilPosX = AperX[0];
    
      TGeoCombiTrans* trd_H_vert_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[0], rotzx01);
      trd_1->AddNode(trd_H_vert_vol1, 11, trd_H_vert_combi01);
      TGeoCombiTrans* trd_H_vert_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[0], rotzx01);
      trd_1->AddNode(trd_H_vert_vol1, 12, trd_H_vert_combi02);
      TGeoCombiTrans* trd_H_vert_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[1], rotzx01);
      trd_1->AddNode(trd_H_vert_vol1, 13, trd_H_vert_combi03);
      TGeoCombiTrans* trd_H_vert_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[1], rotzx01);
      trd_1->AddNode(trd_H_vert_vol1, 14, trd_H_vert_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoXtru* trd_H_vert1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_vert1->DefinePolygon(12,x,y);
      trd_H_vert1->DefineSection( 0,-(AperY[1]+Hhei), 0, 0, 1.0);
      trd_H_vert1->DefineSection( 1, BeamHeight,      0, 0, 1.0);
      TGeoVolume* trd_H_vert_vol1 = new TGeoVolume("trd_H_y_02", trd_H_vert1, aluminiumVolMed);
      trd_H_vert_vol1->SetLineColor(kYellow);
      PilPosX = AperX[1];
    
      TGeoCombiTrans* trd_H_vert_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[2], rotzx01);
      trd_2->AddNode(trd_H_vert_vol1, 21, trd_H_vert_combi01);
      TGeoCombiTrans* trd_H_vert_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[2], rotzx01);
      trd_2->AddNode(trd_H_vert_vol1, 22, trd_H_vert_combi02);
      TGeoCombiTrans* trd_H_vert_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[3], rotzx01);
      trd_2->AddNode(trd_H_vert_vol1, 23, trd_H_vert_combi03);
      TGeoCombiTrans* trd_H_vert_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[3], rotzx01);
      trd_2->AddNode(trd_H_vert_vol1, 24, trd_H_vert_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoXtru* trd_H_vert1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_vert1->DefinePolygon(12,x,y);
      trd_H_vert1->DefineSection( 0,-(AperY[2]+Hhei), 0, 0, 1.0);
      trd_H_vert1->DefineSection( 1, BeamHeight,      0, 0, 1.0);
      TGeoVolume* trd_H_vert_vol1 = new TGeoVolume("trd_H_y_03", trd_H_vert1, aluminiumVolMed);
      trd_H_vert_vol1->SetLineColor(kYellow);
      PilPosX = AperX[2];
      
      TGeoCombiTrans* trd_H_vert_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[4], rotzx01);
      trd_3->AddNode(trd_H_vert_vol1, 31, trd_H_vert_combi01);
      TGeoCombiTrans* trd_H_vert_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[4], rotzx01);
      trd_3->AddNode(trd_H_vert_vol1, 32, trd_H_vert_combi02);
      TGeoCombiTrans* trd_H_vert_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.), 0., PilPosZ[5], rotzx01);
      trd_3->AddNode(trd_H_vert_vol1, 33, trd_H_vert_combi03);
      TGeoCombiTrans* trd_H_vert_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), 0., PilPosZ[5], rotzx01);
      trd_3->AddNode(trd_H_vert_vol1, 34, trd_H_vert_combi04);
    }


//-------------------
// horizontal supports (X)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoXtru* trd_H_hori1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_hori1->DefinePolygon(12,x,y);
      trd_H_hori1->DefineSection( 0,-AperX[0], 0, 0, 1.0);
      trd_H_hori1->DefineSection( 1, AperX[0], 0, 0, 1.0);
      TGeoVolume* trd_H_hori_vol1 = new TGeoVolume("trd_H_x_01", trd_H_hori1, aluminiumVolMed);
      trd_H_hori_vol1->SetLineColor(kRed);
      BarPosY = AperY[0];
    
      TGeoCombiTrans* trd_H_hori_combi01 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[0], roty090);
      trd_1->AddNode(trd_H_hori_vol1, 11, trd_H_hori_combi01);
      TGeoCombiTrans* trd_H_hori_combi02 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[0], roty090);
      trd_1->AddNode(trd_H_hori_vol1, 12, trd_H_hori_combi02);
      TGeoCombiTrans* trd_H_hori_combi03 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[1], roty090);
      trd_1->AddNode(trd_H_hori_vol1, 13, trd_H_hori_combi03);
      TGeoCombiTrans* trd_H_hori_combi04 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[1], roty090);
      trd_1->AddNode(trd_H_hori_vol1, 14, trd_H_hori_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoXtru* trd_H_hori1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_hori1->DefinePolygon(12,x,y);
      trd_H_hori1->DefineSection( 0,-AperX[1], 0, 0, 1.0);
      trd_H_hori1->DefineSection( 1, AperX[1], 0, 0, 1.0);
      TGeoVolume* trd_H_hori_vol1 = new TGeoVolume("trd_H_x_02", trd_H_hori1, aluminiumVolMed);
      trd_H_hori_vol1->SetLineColor(kRed);
      BarPosY = AperY[1];
    
      TGeoCombiTrans* trd_H_hori_combi01 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[2], roty090);
      trd_2->AddNode(trd_H_hori_vol1, 21, trd_H_hori_combi01);
      TGeoCombiTrans* trd_H_hori_combi02 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[2], roty090);
      trd_2->AddNode(trd_H_hori_vol1, 22, trd_H_hori_combi02);
      TGeoCombiTrans* trd_H_hori_combi03 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[3], roty090);
      trd_2->AddNode(trd_H_hori_vol1, 23, trd_H_hori_combi03);
      TGeoCombiTrans* trd_H_hori_combi04 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[3], roty090);
      trd_2->AddNode(trd_H_hori_vol1, 24, trd_H_hori_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoXtru* trd_H_hori1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_hori1->DefinePolygon(12,x,y);
      trd_H_hori1->DefineSection( 0,-AperX[2], 0, 0, 1.0);
      trd_H_hori1->DefineSection( 1, AperX[2], 0, 0, 1.0);
      TGeoVolume* trd_H_hori_vol1 = new TGeoVolume("trd_H_x_03", trd_H_hori1, aluminiumVolMed);
      trd_H_hori_vol1->SetLineColor(kRed);
      BarPosY = AperY[2];
    
      TGeoCombiTrans* trd_H_hori_combi01 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[4], roty090);
      trd_3->AddNode(trd_H_hori_vol1, 31, trd_H_hori_combi01);
      TGeoCombiTrans* trd_H_hori_combi02 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[4], roty090);
      trd_3->AddNode(trd_H_hori_vol1, 32, trd_H_hori_combi02);
      TGeoCombiTrans* trd_H_hori_combi03 = new TGeoCombiTrans(0., (BarPosY+Hhei/2.), PilPosZ[5], roty090);
      trd_3->AddNode(trd_H_hori_vol1, 33, trd_H_hori_combi03);
      TGeoCombiTrans* trd_H_hori_combi04 = new TGeoCombiTrans(0.,-(BarPosY+Hhei/2.), PilPosZ[5], roty090);
      trd_3->AddNode(trd_H_hori_vol1, 34, trd_H_hori_combi04);
    }


//-------------------
// horizontal supports (Z)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoXtru* trd_H_slope1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_slope1->DefinePolygon(12,x,y);
      trd_H_slope1->DefineSection( 0,-(PilPosZ[1]-PilPosZ[0]-Hwid)/2., 0, 0, 1.0);
      trd_H_slope1->DefineSection( 1,+(PilPosZ[1]-PilPosZ[0]-Hwid)/2., 0, 0, 1.0);
      TGeoVolume* trd_H_slope_vol1 = new TGeoVolume("trd_H_z_01", trd_H_slope1, aluminiumVolMed);
      trd_H_slope_vol1->SetLineColor(kGreen);
      PilPosX = AperX[0];
      BarPosY = AperY[0];
        
      TGeoCombiTrans* trd_H_slope_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_H_slope_vol1, 11, trd_H_slope_combi01);
      TGeoCombiTrans* trd_H_slope_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_H_slope_vol1, 12, trd_H_slope_combi02);
      TGeoCombiTrans* trd_H_slope_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_H_slope_vol1, 13, trd_H_slope_combi03);
      TGeoCombiTrans* trd_H_slope_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_H_slope_vol1, 14, trd_H_slope_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoXtru* trd_H_slope1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_slope1->DefinePolygon(12,x,y);
      trd_H_slope1->DefineSection( 0,-(PilPosZ[3]-PilPosZ[2]-Hwid)/2., 0, 0, 1.0);
      trd_H_slope1->DefineSection( 1,+(PilPosZ[3]-PilPosZ[2]-Hwid)/2., 0, 0, 1.0);
      TGeoVolume* trd_H_slope_vol1 = new TGeoVolume("trd_H_z_02", trd_H_slope1, aluminiumVolMed);
      trd_H_slope_vol1->SetLineColor(kGreen);
      PilPosX = AperX[1];
      BarPosY = AperY[1];
        
      TGeoCombiTrans* trd_H_slope_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_H_slope_vol1, 21, trd_H_slope_combi01);
      TGeoCombiTrans* trd_H_slope_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_H_slope_vol1, 22, trd_H_slope_combi02);
      TGeoCombiTrans* trd_H_slope_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_H_slope_vol1, 23, trd_H_slope_combi03);
      TGeoCombiTrans* trd_H_slope_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_H_slope_vol1, 24, trd_H_slope_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoXtru* trd_H_slope1 = new TGeoXtru(2);  // define Xtrusion of 2 planes
      trd_H_slope1->DefinePolygon(12,x,y);
      trd_H_slope1->DefineSection( 0,-(PilPosZ[5]-PilPosZ[4]-Hwid)/2., 0, 0, 1.0);
      trd_H_slope1->DefineSection( 1,+(PilPosZ[5]-PilPosZ[4]-Hwid)/2., 0, 0, 1.0);
      TGeoVolume* trd_H_slope_vol1 = new TGeoVolume("trd_H_z_03", trd_H_slope1, aluminiumVolMed);
      trd_H_slope_vol1->SetLineColor(kGreen);
      PilPosX = AperX[2];
      BarPosY = AperY[2];
        
      TGeoCombiTrans* trd_H_slope_combi01 = new TGeoCombiTrans( (PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_H_slope_vol1, 31, trd_H_slope_combi01);
      TGeoCombiTrans* trd_H_slope_combi02 = new TGeoCombiTrans(-(PilPosX+Hhei/2.), (BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_H_slope_vol1, 32, trd_H_slope_combi02);
      TGeoCombiTrans* trd_H_slope_combi03 = new TGeoCombiTrans( (PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_H_slope_vol1, 33, trd_H_slope_combi03);
      TGeoCombiTrans* trd_H_slope_combi04 = new TGeoCombiTrans(-(PilPosX+Hhei/2.),-(BarPosY+Hhei-Hwid/2.), (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_H_slope_vol1, 34, trd_H_slope_combi04);
    }

  if (IncludeLabels)
  {

    Int_t text_height    = 40;
    Int_t text_thickness =  8;
  
    TGeoTranslation *tr200 = new TGeoTranslation(0., (AperY[0]+Hhei+ text_height/2.), PilPosZ[0]-15+ text_thickness/2.);
    TGeoTranslation *tr201 = new TGeoTranslation(0., (AperY[1]+Hhei+ text_height/2.), PilPosZ[2]-15+ text_thickness/2.);
    TGeoTranslation *tr202 = new TGeoTranslation(0., (AperY[2]+Hhei+ text_height/2.), PilPosZ[4]-15+ text_thickness/2.);
  
    TGeoCombiTrans  *tr203 = new TGeoCombiTrans(-(AperX[0]+Hhei+ text_thickness/2.), (AperY[0]+Hhei-Hwid-text_height/2.), (PilPosZ[0]+PilPosZ[1])/2., roty090);
    TGeoCombiTrans  *tr204 = new TGeoCombiTrans(-(AperX[1]+Hhei+ text_thickness/2.), (AperY[1]+Hhei-Hwid-text_height/2.), (PilPosZ[2]+PilPosZ[3])/2., roty090);
    TGeoCombiTrans  *tr205 = new TGeoCombiTrans(-(AperX[2]+Hhei+ text_thickness/2.), (AperY[2]+Hhei-Hwid-text_height/2.), (PilPosZ[4]+PilPosZ[5])/2., roty090);
  
    TGeoCombiTrans  *tr206 = new TGeoCombiTrans( (AperX[0]+Hhei+ text_thickness/2.), (AperY[0]+Hhei-Hwid-text_height/2.), (PilPosZ[0]+PilPosZ[1])/2., roty270);
    TGeoCombiTrans  *tr207 = new TGeoCombiTrans( (AperX[1]+Hhei+ text_thickness/2.), (AperY[1]+Hhei-Hwid-text_height/2.), (PilPosZ[2]+PilPosZ[3])/2., roty270);
    TGeoCombiTrans  *tr208 = new TGeoCombiTrans( (AperX[2]+Hhei+ text_thickness/2.), (AperY[2]+Hhei-Hwid-text_height/2.), (PilPosZ[4]+PilPosZ[5])/2., roty270);
  
    TGeoVolume* trdbox1 = new TGeoVolumeAssembly("trdbox1"); // volume for TRD text (108, 40, 8)
    TGeoVolume* trdbox2 = new TGeoVolumeAssembly("trdbox2"); // volume for TRD text (108, 40, 8)
    TGeoVolume* trdbox3 = new TGeoVolumeAssembly("trdbox3"); // volume for TRD text (108, 40, 8)
    add_trd_labels(trdbox1, trdbox2, trdbox3);
  
    // final placement
    if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      //    trd_1->AddNode(trdbox1, 1, tr200);
      trd_1->AddNode(trdbox1, 4, tr203);
      trd_1->AddNode(trdbox1, 7, tr206);
    }
    if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      //    trd_2->AddNode(trdbox2, 2, tr201);
      trd_2->AddNode(trdbox2, 5, tr204);
      trd_2->AddNode(trdbox2, 8, tr207);
    }
    if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      //    trd_3->AddNode(trdbox3, 3, tr202);
      trd_3->AddNode(trdbox3, 6, tr205);
      trd_3->AddNode(trdbox3, 9, tr208);
    }
  }

  //  gGeoMan->GetVolume(geoVersion)->AddNode(trdsupport,1);

  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    gGeoMan->GetVolume(geoVersion)->AddNode(trd_1, 1);
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    gGeoMan->GetVolume(geoVersion)->AddNode(trd_2, 2);
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    gGeoMan->GetVolume(geoVersion)->AddNode(trd_3, 3);

}


add_trd_labels(TGeoVolume* trdbox1, TGeoVolume* trdbox2, TGeoVolume* trdbox3)
{
// write TRD (the 3 characters) in a simple geometry
  TGeoMedium* textVolMed        = gGeoMan->GetMedium(TextVolumeMedium);

  Int_t Tcolor = kBlue;  // kRed;
  Int_t Rcolor = kBlue;  // kRed;  // kRed;
  Int_t Dcolor = kBlue;  // kRed;  // kYellow;
  Int_t Icolor = kBlue;  // kRed;  

// define transformations for letter pieces
// T
  TGeoTranslation *tr01 = new TGeoTranslation(  0. , -4., 0.);
  TGeoTranslation *tr02 = new TGeoTranslation(  0. , 16., 0.);

// R
  TGeoTranslation *tr11 = new TGeoTranslation( 10,  0., 0.);
  TGeoTranslation *tr12 = new TGeoTranslation(  2,  0., 0.);
  TGeoTranslation *tr13 = new TGeoTranslation(  2, 16., 0.);
  TGeoTranslation *tr14 = new TGeoTranslation( -2,  8., 0.);
  TGeoTranslation *tr15 = new TGeoTranslation( -6,  0., 0.);

// D
  TGeoTranslation *tr21 = new TGeoTranslation( 12.,  0., 0.);
  TGeoTranslation *tr22 = new TGeoTranslation(  6., 16., 0.);
  TGeoTranslation *tr23 = new TGeoTranslation(  6.,-16., 0.);
  TGeoTranslation *tr24 = new TGeoTranslation(  4.,  0., 0.);

// I
  TGeoTranslation *tr31 = new TGeoTranslation(  0. ,  0., 0.);
  TGeoTranslation *tr32 = new TGeoTranslation(  0. , 16., 0.);
  TGeoTranslation *tr33 = new TGeoTranslation(  0. ,-16., 0.);

// make letter T
//   TGeoVolume *T = geom->MakeBox("T", Vacuum, 25., 25., 5.);
//   T->SetVisibility(kFALSE);
  TGeoVolume* T = new TGeoVolumeAssembly("Tbox"); // volume for T

  TGeoBBox *Tbar1b = new TGeoBBox("", 4., 16., 4.);  // | vertical
  TGeoVolume *Tbar1 = new TGeoVolume("Tbar1", Tbar1b, textVolMed);
  Tbar1->SetLineColor(Tcolor);
  T->AddNode(Tbar1, 1, tr01);
  TGeoBBox *Tbar2b = new TGeoBBox("", 16, 4., 4.);  // - top
  TGeoVolume *Tbar2 = new TGeoVolume("Tbar2", Tbar2b, textVolMed);
  Tbar2->SetLineColor(Tcolor);
  T->AddNode(Tbar2, 1, tr02);

// make letter R
//   TGeoVolume *R = geom->MakeBox("R", Vacuum, 25., 25., 5.);
//   R->SetVisibility(kFALSE);
  TGeoVolume* R = new TGeoVolumeAssembly("Rbox"); // volume for R

  TGeoBBox *Rbar1b = new TGeoBBox("", 4., 20, 4.);
  TGeoVolume *Rbar1 = new TGeoVolume("Rbar1", Rbar1b, textVolMed);
  Rbar1->SetLineColor(Rcolor);
  R->AddNode(Rbar1, 1, tr11);
  TGeoBBox *Rbar2b = new TGeoBBox("", 4., 4., 4.);
  TGeoVolume *Rbar2 = new TGeoVolume("Rbar2", Rbar2b, textVolMed);
  Rbar2->SetLineColor(Rcolor);
  R->AddNode(Rbar2, 1, tr12);
  R->AddNode(Rbar2, 2, tr13);
  TGeoTubeSeg *Rtub1b = new TGeoTubeSeg("", 4., 12, 4., 90., 270.);
  TGeoVolume *Rtub1 = new TGeoVolume("Rtub1", Rtub1b, textVolMed);
  Rtub1->SetLineColor(Rcolor);
  R->AddNode(Rtub1, 1, tr14);
  TGeoArb8 *Rbar3b = new TGeoArb8("", 4.);
  TGeoVolume *Rbar3 = new TGeoVolume("Rbar3", Rbar3b, textVolMed);
  Rbar3->SetLineColor(Rcolor);
  TGeoArb8 *arb = (TGeoArb8*)Rbar3->GetShape();
  arb->SetVertex(0, 12.,  -4.);
  arb->SetVertex(1,  0., -20.);
  arb->SetVertex(2, -8., -20.);
  arb->SetVertex(3,  4.,  -4.);
  arb->SetVertex(4, 12.,  -4.);
  arb->SetVertex(5,  0., -20.);
  arb->SetVertex(6, -8., -20.);
  arb->SetVertex(7,  4.,  -4.);
  R->AddNode(Rbar3, 1, tr15);

// make letter D
//   TGeoVolume *D = geom->MakeBox("D", Vacuum, 25., 25., 5.);
//   D->SetVisibility(kFALSE);
  TGeoVolume* D = new TGeoVolumeAssembly("Dbox"); // volume for D

  TGeoBBox *Dbar1b = new TGeoBBox("", 4., 20, 4.);
  TGeoVolume *Dbar1 = new TGeoVolume("Dbar1", Dbar1b, textVolMed);
  Dbar1->SetLineColor(Dcolor);
  D->AddNode(Dbar1, 1, tr21);
  TGeoBBox *Dbar2b = new TGeoBBox("", 2., 4., 4.);
  TGeoVolume *Dbar2 = new TGeoVolume("Dbar2", Dbar2b, textVolMed);
  Dbar2->SetLineColor(Dcolor);
  D->AddNode(Dbar2, 1, tr22);
  D->AddNode(Dbar2, 2, tr23);
  TGeoTubeSeg *Dtub1b = new TGeoTubeSeg("", 12, 20, 4., 90., 270.);
  TGeoVolume *Dtub1 = new TGeoVolume("Dtub1", Dtub1b, textVolMed);
  Dtub1->SetLineColor(Dcolor);
  D->AddNode(Dtub1, 1, tr24);

// make letter I
  TGeoVolume* I = new TGeoVolumeAssembly("Ibox"); // volume for I

  TGeoBBox *Ibar1b = new TGeoBBox("", 4., 12., 4.);  // | vertical
  TGeoVolume *Ibar1 = new TGeoVolume("Ibar1", Ibar1b, textVolMed);
  Ibar1->SetLineColor(Icolor);
  I->AddNode(Ibar1, 1, tr31);
  TGeoBBox *Ibar2b = new TGeoBBox("", 10., 4., 4.);  // - top
  TGeoVolume *Ibar2 = new TGeoVolume("Ibar2", Ibar2b, textVolMed);
  Ibar2->SetLineColor(Icolor);
  I->AddNode(Ibar2, 1, tr32);
  I->AddNode(Ibar2, 2, tr33);


// build text block "TRD"  <32> + 8 + <28> + 8 + <32> = 108

//  TGeoBBox *trdboxb = new TGeoBBox("", 108./2, 40./2, 8./2);
//  TGeoVolume *trdbox = new TGeoVolume("trdboxb", trdboxb, textVolMed);
//  trdbox->SetVisibility(kFALSE);

//  TGeoVolume* trdbox[0] = new TGeoVolumeAssembly("trdbox1"); // volume for TRD text (108, 40, 8)
//  TGeoVolume* trdbox[1] = new TGeoVolumeAssembly("trdbox2"); // volume for TRD text (108, 40, 8)
//  TGeoVolume* trdbox[2] = new TGeoVolumeAssembly("trdbox3"); // volume for TRD text (108, 40, 8)

  TGeoTranslation *tr100 = new TGeoTranslation( 38., 0., 0.);
  TGeoTranslation *tr101 = new TGeoTranslation(  0., 0., 0.);
  TGeoTranslation *tr102 = new TGeoTranslation(-38., 0., 0.);

//  TGeoTranslation *tr103 = new TGeoTranslation( -70., 0., 0.);  // on the same line
//  TGeoTranslation *tr104 = new TGeoTranslation( -86., 0., 0.);  // on the same line
//  TGeoTranslation *tr105 = new TGeoTranslation(-102., 0., 0.);  // on the same line

  TGeoTranslation *tr110 = new TGeoTranslation(   0., -50., 0.);
  TGeoTranslation *tr111 = new TGeoTranslation(   8., -50., 0.);
  TGeoTranslation *tr112 = new TGeoTranslation(  -8., -50., 0.);
  TGeoTranslation *tr113 = new TGeoTranslation(  16., -50., 0.);
  TGeoTranslation *tr114 = new TGeoTranslation( -16., -50., 0.);

  TGeoTranslation *tr200 = new TGeoTranslation(   0.,   0., 0.);
  TGeoTranslation *tr201 = new TGeoTranslation(   0., -50., 0.);
  TGeoTranslation *tr202 = new TGeoTranslation(   0.,-100., 0.);

  TGeoTranslation *tr210 = new TGeoTranslation(   0.,-150., 0.);
  TGeoTranslation *tr213 = new TGeoTranslation(  16.,-150., 0.);
  TGeoTranslation *tr214 = new TGeoTranslation( -16.,-150., 0.);

// station 1
  trdbox1->AddNode(T, 1, tr100);
  trdbox1->AddNode(R, 1, tr101);
  trdbox1->AddNode(D, 1, tr102);

  trdbox1->AddNode(I, 1, tr110);

// station 2
  trdbox2->AddNode(T, 1, tr100);
  trdbox2->AddNode(R, 1, tr101);
  trdbox2->AddNode(D, 1, tr102);

  trdbox2->AddNode(I, 1, tr111);
  trdbox2->AddNode(I, 2, tr112);

//// station 3
//  trdbox3->AddNode(T, 1, tr100);
//  trdbox3->AddNode(R, 1, tr101);
//  trdbox3->AddNode(D, 1, tr102);
//
//  trdbox3->AddNode(I, 1, tr110);
//  trdbox3->AddNode(I, 2, tr113);
//  trdbox3->AddNode(I, 3, tr114);

// station 3
  trdbox3->AddNode(T, 1, tr200);
  trdbox3->AddNode(R, 1, tr201);
  trdbox3->AddNode(D, 1, tr202);

  trdbox3->AddNode(I, 1, tr210);
  trdbox3->AddNode(I, 2, tr213);
  trdbox3->AddNode(I, 3, tr214);

//  TGeoScale *sc100 = new TGeoScale( 36./50., 36./50., 1.);  // text is vertical 50 cm, H-bar opening is 36 cm
//
//  // scale text
//  TGeoHMatrix *mat100 = new TGeoHMatrix("");
//  TGeoHMatrix *mat101 = new TGeoHMatrix("");
//  TGeoHMatrix *mat102 = new TGeoHMatrix("");
//  (*mat100) = (*tr100) * (*sc100);
//  (*mat101) = (*tr101) * (*sc100);
//  (*mat102) = (*tr102) * (*sc100);
//
//  trdbox->AddNode(T, 1, mat100);
//  trdbox->AddNode(R, 1, mat101);
//  trdbox->AddNode(D, 1, mat102);

//   // final placement
//   //   TGeoTranslation *tr103 = new TGeoTranslation(0., 400., 500.);
//   gGeoMan->GetVolume(geoVersion)->AddNode(trdbox, 1, new TGeoTranslation(0., 400., 500.));
//   gGeoMan->GetVolume(geoVersion)->AddNode(trdbox, 2, new TGeoTranslation(0., 500., 600.));
//   gGeoMan->GetVolume(geoVersion)->AddNode(trdbox, 3, new TGeoTranslation(0., 600., 700.));

//  return trdbox;

}   
   

void create_box_supports()
{
  const TString trd_01 = "support_trd1";
  TGeoVolume* trd_1 = new TGeoVolumeAssembly(trd_01);

  const TString trd_02 = "support_trd2";
  TGeoVolume* trd_2 = new TGeoVolumeAssembly(trd_02);

  const TString trd_03 = "support_trd3";
  TGeoVolume* trd_3 = new TGeoVolumeAssembly(trd_03);

//  const TString trdSupport = "supportframe";
//  TGeoVolume* trdsupport = new TGeoVolumeAssembly(trdSupport);
//
//  trdsupport->AddNode(trd_1, 1);
//  trdsupport->AddNode(trd_2, 2);
//  trdsupport->AddNode(trd_3, 3);

  TGeoMedium* keepVolMed        = gGeoMan->GetMedium(KeepingVolumeMedium);
  TGeoMedium* aluminiumVolMed   = gGeoMan->GetMedium(AluminiumVolumeMedium);  // define Volume Medium

  const Int_t I_height = 40; // cm // I profile properties
  const Int_t I_width  = 30; // cm // I profile properties
  const Int_t I_thick  =  2; // cm // I profile properties

  const Double_t BeamHeight     = 570;   // beamline is at 5.7m above the floor
  const Double_t PlatformHeight = 234;   // platform is   2.34m above the floor
  const Double_t PlatformOffset =   1;   // distance to platform

//  Double_t AperX[3] = { 450., 550., 600.};  // 100 cm modules  // inner aperture in X of support structure for stations 1,2,3
//  Double_t AperY[3] = { 350., 450., 500.};  // 100 cm modules  // inner aperture in Y of support structure for stations 1,2,3

  const Double_t AperX[3] = { 4.5*DetectorSizeX[1], 5.5*DetectorSizeX[1], 6*DetectorSizeX[1] };  // inner aperture in X of support structure for stations 1,2,3
  const Double_t AperY[3] = { 3.5*DetectorSizeY[1], 4.5*DetectorSizeY[1], 5*DetectorSizeY[1] };  // inner aperture in Y of support structure for stations 1,2,3
  // platform
  const Double_t AperYbot[3] = { BeamHeight-(PlatformHeight+PlatformOffset+I_height), 4.5*DetectorSizeY[1], 5*DetectorSizeY[1] };  // inner aperture for station1

  const Double_t xBarPosYtop[3] = { AperY[0]   +I_height/2., AperY[1]+I_height/2., AperY[2]+I_height/2. };
  const Double_t xBarPosYbot[3] = { AperYbot[0]+I_height/2., xBarPosYtop[1]      , xBarPosYtop[2]       };

  const Double_t zBarPosYtop[3] = { AperY[0]   +I_height-I_width/2., AperY[1]+I_height-I_width/2., AperY[2]+I_height-I_width/2. };
  const Double_t zBarPosYbot[3] = { AperYbot[0]+I_height-I_width/2., zBarPosYtop[1]              , zBarPosYtop[2]               };

  Double_t PilPosX;
  Double_t PilPosZ[6];  // PilPosZ

  PilPosZ[0] = LayerPosition[0] + I_width/2.;
  PilPosZ[1] = LayerPosition[3] - I_width/2. + LayerThickness;
  PilPosZ[2] = LayerPosition[4] + I_width/2.;		      
  PilPosZ[3] = LayerPosition[7] - I_width/2. + LayerThickness;
  PilPosZ[4] = LayerPosition[8] + I_width/2.;		      
  PilPosZ[5] = LayerPosition[9] - I_width/2. + LayerThickness;

//  cout << "PilPosZ[0]: " << PilPosZ[0] << endl;
//  cout << "PilPosZ[1]: " << PilPosZ[1] << endl;

  TGeoRotation  *rotx090 = new TGeoRotation("rotx090"); rotx090->RotateX( 90.); // rotate  90 deg around x-axis                     
  TGeoRotation  *roty090 = new TGeoRotation("roty090"); roty090->RotateY( 90.); // rotate  90 deg around y-axis                     
  TGeoRotation  *rotz090 = new TGeoRotation("rotz090"); rotz090->RotateZ( 90.); // rotate  90 deg around y-axis                     
  TGeoRotation  *roty270 = new TGeoRotation("roty270"); roty270->RotateY(270.); // rotate 270 deg around y-axis                     

  TGeoRotation  *rotzx01 = new TGeoRotation("rotzx01"); 
  rotzx01->RotateZ(  90.); // rotate  90 deg around z-axis
  rotzx01->RotateX(  90.); // rotate  90 deg around x-axis

  TGeoRotation  *rotzx02 = new TGeoRotation("rotzx02"); 
  rotzx02->RotateZ( 270.); // rotate 270 deg around z-axis
  rotzx02->RotateX(  90.); // rotate  90 deg around x-axis

  Double_t ang1 = atan(3./4.) * 180. / acos(-1.);
  //  cout << "DEDE " << ang1 << endl;
  //  Double_t sin1 = acos(-1.);
  //  cout << "DEDE " << sin1 << endl;
  TGeoRotation  *rotx080 = new TGeoRotation("rotx080"); rotx080->RotateX( 90.-ang1); // rotate  80 deg around x-axis                     
  TGeoRotation  *rotx100 = new TGeoRotation("rotx100"); rotx100->RotateX( 90.+ang1); // rotate 100 deg around x-axis                     

  TGeoRotation  *rotxy01 = new TGeoRotation("rotxy01"); 
  rotxy01->RotateX(  90.); // rotate  90 deg around x-axis                     
  rotxy01->RotateZ(-ang1); // rotate  ang1   around rotated y-axis                     

  TGeoRotation  *rotxy02 = new TGeoRotation("rotxy02"); 
  rotxy02->RotateX(  90.); // rotate  90 deg around x-axis                     
  rotxy02->RotateZ( ang1); // rotate  ang1   around rotated y-axis                     


//-------------------
// vertical pillars (Y)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
//      TGeoBBox* trd_I_vert1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, (BeamHeight + (AperY[0]+I_height) ) /2.);
      TGeoBBox* trd_I_vert1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, ( (AperYbot[0]+I_height) + (AperY[0]+I_height) ) /2.);
      TGeoVolume* trd_I_vert1     = new TGeoVolume("trd_I_y11", trd_I_vert1_keep, aluminiumVolMed);
      //      TGeoBBox* trd_I_vert2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., (BeamHeight + (AperY[0]+I_height) ) /2.);
      TGeoBBox* trd_I_vert2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., ( (AperYbot[0]+I_height) + (AperY[0]+I_height) ) /2.);
      TGeoVolume* trd_I_vert2     = new TGeoVolume("trd_I_y12", trd_I_vert2_keep, aluminiumVolMed);

      trd_I_vert1->SetLineColor(kGreen);  // kBlue);  // Yellow);  // kOrange);
      trd_I_vert2->SetLineColor(kGreen);  // kBlue);  // Yellow);  // kOrange);
    
      TGeoTranslation *ty01 = new TGeoTranslation("ty01", 0.,                      0., 0.);
      TGeoTranslation *ty02 = new TGeoTranslation("ty02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *ty03 = new TGeoTranslation("ty03", 0., -(I_height-I_thick) /2., 0.);

      //      TGeoBBox* trd_I_vert_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., (BeamHeight + (AperY[0]+I_height) ) /2.);
      TGeoBBox* trd_I_vert_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., ( (AperYbot[0]+I_height) + (AperY[0]+I_height) ) /2.);
      TGeoVolume* trd_I_vert_vol1    = new TGeoVolume("trd_I_y10", trd_I_vert_vol1_keep, keepVolMed);

      // build I-bar trd_I_vert_vol1
      trd_I_vert_vol1->AddNode(trd_I_vert1, 1, ty01);
      trd_I_vert_vol1->AddNode(trd_I_vert2, 2, ty02);
      trd_I_vert_vol1->AddNode(trd_I_vert2, 3, ty03);

      // close gap to horizontal z-bars
      TGeoBBox* trd_I_vert3_keep  = new TGeoBBox("", (I_width-I_thick)/2. /2., I_height /2. - I_thick, I_thick /2.);
      TGeoVolume* trd_I_vert3     = new TGeoVolume("trd_I_y13", trd_I_vert3_keep, aluminiumVolMed);
      trd_I_vert3->SetLineColor(kGreen);
//      TGeoTranslation *ty04 = new TGeoTranslation("ty04",  (I_thick/2. + (I_width-I_thick)/2./2.), 0., -( (AperYbot[0]+I_height) + (AperY[0]+I_height) - I_width) /2.);  // top
//      TGeoTranslation *ty05 = new TGeoTranslation("ty05",  (I_thick/2. + (I_width-I_thick)/2./2.), 0.,  ( (AperYbot[0]+I_height) + (AperY[0]+I_height) - I_width) /2.);  // bottom
      TGeoTranslation *ty04 = new TGeoTranslation("ty04",  (I_thick/2. + (I_width-I_thick)/2./2.), 0., -( zBarPosYbot[0] + zBarPosYtop[0] )/2. );  // top	  
      TGeoTranslation *ty05 = new TGeoTranslation("ty05",  (I_thick/2. + (I_width-I_thick)/2./2.), 0.,  ( zBarPosYbot[0] + zBarPosYtop[0] )/2. );  // bottom
      trd_I_vert_vol1->AddNode(trd_I_vert3, 4, ty04);
      trd_I_vert_vol1->AddNode(trd_I_vert3, 5, ty05);

      PilPosX = AperX[0];
    
      TGeoCombiTrans* trd_I_vert_combi01 = new TGeoCombiTrans( (PilPosX+I_height/2.), -( (AperYbot[0]+I_height) - (AperY[0]+I_height))/2., PilPosZ[0], rotzx01);
      trd_1->AddNode(trd_I_vert_vol1, 11, trd_I_vert_combi01);
      TGeoCombiTrans* trd_I_vert_combi02 = new TGeoCombiTrans(-(PilPosX+I_height/2.), -( (AperYbot[0]+I_height) - (AperY[0]+I_height))/2., PilPosZ[0], rotzx01);
      trd_1->AddNode(trd_I_vert_vol1, 12, trd_I_vert_combi02);
      TGeoCombiTrans* trd_I_vert_combi03 = new TGeoCombiTrans( (PilPosX+I_height/2.), -( (AperYbot[0]+I_height) - (AperY[0]+I_height))/2., PilPosZ[1], rotzx02);
      trd_1->AddNode(trd_I_vert_vol1, 13, trd_I_vert_combi03);
      TGeoCombiTrans* trd_I_vert_combi04 = new TGeoCombiTrans(-(PilPosX+I_height/2.), -( (AperYbot[0]+I_height) - (AperY[0]+I_height))/2., PilPosZ[1], rotzx02);
      trd_1->AddNode(trd_I_vert_vol1, 14, trd_I_vert_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoBBox* trd_I_vert1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, (BeamHeight + (AperY[1]+I_height) ) /2.);
      TGeoVolume* trd_I_vert1     = new TGeoVolume("trd_I_y21", trd_I_vert1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_vert2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., (BeamHeight + (AperY[1]+I_height) ) /2.);
      TGeoVolume* trd_I_vert2     = new TGeoVolume("trd_I_y22", trd_I_vert2_keep, aluminiumVolMed);

      trd_I_vert1->SetLineColor(kGreen);
      trd_I_vert2->SetLineColor(kGreen);
    
      TGeoTranslation *ty01 = new TGeoTranslation("ty01", 0.,                      0., 0.);
      TGeoTranslation *ty02 = new TGeoTranslation("ty02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *ty03 = new TGeoTranslation("ty03", 0., -(I_height-I_thick) /2., 0.);

      TGeoBBox* trd_I_vert_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., (BeamHeight + (AperY[1]+I_height) ) /2.);
      TGeoVolume* trd_I_vert_vol1    = new TGeoVolume("trd_I_y20", trd_I_vert_vol1_keep, keepVolMed);

      // build I-bar trd_I_vert_vol1
      trd_I_vert_vol1->AddNode(trd_I_vert1, 1, ty01);
      trd_I_vert_vol1->AddNode(trd_I_vert2, 2, ty02);
      trd_I_vert_vol1->AddNode(trd_I_vert2, 3, ty03);

      // close gap to horizontal z-bars
      TGeoBBox* trd_I_vert3_keep  = new TGeoBBox("", (I_width-I_thick)/2. /2., I_height /2. - I_thick, I_thick /2.);
      TGeoVolume* trd_I_vert3     = new TGeoVolume("trd_I_y23", trd_I_vert3_keep, aluminiumVolMed);
      trd_I_vert3->SetLineColor(kGreen);
      TGeoTranslation *ty04 = new TGeoTranslation("ty04",  (I_thick/2. + (I_width-I_thick)/2./2.), 0., -(BeamHeight + (AperY[1]+I_height) - I_width) /2.);          // top
      TGeoTranslation *ty05 = new TGeoTranslation("ty05",  (I_thick/2. + (I_width-I_thick)/2./2.), 0., -(BeamHeight - (AperY[1]+I_height) )/2. + zBarPosYbot[1] );  // bottom
      trd_I_vert_vol1->AddNode(trd_I_vert3, 4, ty04);
      trd_I_vert_vol1->AddNode(trd_I_vert3, 5, ty05);

      PilPosX = AperX[1];
    
      TGeoCombiTrans* trd_I_vert_combi01 = new TGeoCombiTrans( (PilPosX+I_height/2.), -(BeamHeight - (AperY[1]+I_height))/2., PilPosZ[2], rotzx01);
      trd_2->AddNode(trd_I_vert_vol1, 21, trd_I_vert_combi01);
      TGeoCombiTrans* trd_I_vert_combi02 = new TGeoCombiTrans(-(PilPosX+I_height/2.), -(BeamHeight - (AperY[1]+I_height))/2., PilPosZ[2], rotzx01);
      trd_2->AddNode(trd_I_vert_vol1, 22, trd_I_vert_combi02);
      TGeoCombiTrans* trd_I_vert_combi03 = new TGeoCombiTrans( (PilPosX+I_height/2.), -(BeamHeight - (AperY[1]+I_height))/2., PilPosZ[3], rotzx02);
      trd_2->AddNode(trd_I_vert_vol1, 23, trd_I_vert_combi03);
      TGeoCombiTrans* trd_I_vert_combi04 = new TGeoCombiTrans(-(PilPosX+I_height/2.), -(BeamHeight - (AperY[1]+I_height))/2., PilPosZ[3], rotzx02);
      trd_2->AddNode(trd_I_vert_vol1, 24, trd_I_vert_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoBBox* trd_I_vert1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, (BeamHeight + (AperY[2]+I_height) ) /2.);
      TGeoVolume* trd_I_vert1     = new TGeoVolume("trd_I_y31", trd_I_vert1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_vert2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., (BeamHeight + (AperY[2]+I_height) ) /2.);
      TGeoVolume* trd_I_vert2     = new TGeoVolume("trd_I_y32", trd_I_vert2_keep, aluminiumVolMed);

      trd_I_vert1->SetLineColor(kGreen);
      trd_I_vert2->SetLineColor(kGreen);
    
      TGeoTranslation *ty01 = new TGeoTranslation("ty01", 0.,                      0., 0.);
      TGeoTranslation *ty02 = new TGeoTranslation("ty02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *ty03 = new TGeoTranslation("ty03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_vert_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., (BeamHeight + (AperY[2]+I_height) ) /2.);
      TGeoVolume* trd_I_vert_vol1    = new TGeoVolume("trd_I_y30", trd_I_vert_vol1_keep, keepVolMed);

      // build I-bar trd_I_vert_vol1
      trd_I_vert_vol1->AddNode(trd_I_vert1, 1, ty01);
      trd_I_vert_vol1->AddNode(trd_I_vert2, 2, ty02);
      trd_I_vert_vol1->AddNode(trd_I_vert2, 3, ty03);

      // close gap to horizontal z-bars
      TGeoBBox* trd_I_vert3_keep  = new TGeoBBox("", (I_width-I_thick)/2. /2., I_height /2. - I_thick, I_thick /2.);
      TGeoVolume* trd_I_vert3     = new TGeoVolume("trd_I_y33", trd_I_vert3_keep, aluminiumVolMed);
      trd_I_vert3->SetLineColor(kGreen);
      TGeoTranslation *ty04 = new TGeoTranslation("ty04",  (I_thick/2. + (I_width-I_thick)/2./2.), 0., -(BeamHeight + (AperY[2]+I_height) - I_width) /2.);          // top
      TGeoTranslation *ty05 = new TGeoTranslation("ty05",  (I_thick/2. + (I_width-I_thick)/2./2.), 0., -(BeamHeight - (AperY[2]+I_height) )/2. + zBarPosYbot[2] );  // bottom
      trd_I_vert_vol1->AddNode(trd_I_vert3, 4, ty04);
      trd_I_vert_vol1->AddNode(trd_I_vert3, 5, ty05);

      PilPosX = AperX[2];
      
      TGeoCombiTrans* trd_I_vert_combi01 = new TGeoCombiTrans( (PilPosX+I_height/2.), -(BeamHeight - (AperY[2]+I_height))/2., PilPosZ[4], rotzx01);
      trd_3->AddNode(trd_I_vert_vol1, 31, trd_I_vert_combi01);
      TGeoCombiTrans* trd_I_vert_combi02 = new TGeoCombiTrans(-(PilPosX+I_height/2.), -(BeamHeight - (AperY[2]+I_height))/2., PilPosZ[4], rotzx01);
      trd_3->AddNode(trd_I_vert_vol1, 32, trd_I_vert_combi02);
      TGeoCombiTrans* trd_I_vert_combi03 = new TGeoCombiTrans( (PilPosX+I_height/2.), -(BeamHeight - (AperY[2]+I_height))/2., PilPosZ[5], rotzx02);
      trd_3->AddNode(trd_I_vert_vol1, 33, trd_I_vert_combi03);
      TGeoCombiTrans* trd_I_vert_combi04 = new TGeoCombiTrans(-(PilPosX+I_height/2.), -(BeamHeight - (AperY[2]+I_height))/2., PilPosZ[5], rotzx02);
      trd_3->AddNode(trd_I_vert_vol1, 34, trd_I_vert_combi04);
    }


//-------------------
// horizontal supports (X)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoBBox* trd_I_hori1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, AperX[0]);
      TGeoVolume* trd_I_hori1     = new TGeoVolume("trd_I_x11", trd_I_hori1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_hori2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., AperX[0]);
      TGeoVolume* trd_I_hori2     = new TGeoVolume("trd_I_x12", trd_I_hori2_keep, aluminiumVolMed);

      trd_I_hori1->SetLineColor(kRed);  // Yellow);
      trd_I_hori2->SetLineColor(kRed);  // Yellow);
    
      TGeoTranslation *tx01 = new TGeoTranslation("tx01", 0.,                      0., 0.);
      TGeoTranslation *tx02 = new TGeoTranslation("tx02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *tx03 = new TGeoTranslation("tx03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_hori_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., AperX[0]);
      TGeoVolume* trd_I_hori_vol1    = new TGeoVolume("trd_I_x10", trd_I_hori_vol1_keep, keepVolMed);

      // build I-bar trd_I_hori_vol1
      trd_I_hori_vol1->AddNode(trd_I_hori1, 1, tx01);
      trd_I_hori_vol1->AddNode(trd_I_hori2, 2, tx02);
      trd_I_hori_vol1->AddNode(trd_I_hori2, 3, tx03);

      TGeoCombiTrans* trd_I_hori_combi01 = new TGeoCombiTrans(0., xBarPosYtop[0], PilPosZ[0], roty090);
      trd_1->AddNode(trd_I_hori_vol1, 11, trd_I_hori_combi01);
      TGeoCombiTrans* trd_I_hori_combi02 = new TGeoCombiTrans(0.,-xBarPosYbot[0], PilPosZ[0], roty090);
      trd_1->AddNode(trd_I_hori_vol1, 12, trd_I_hori_combi02);
      TGeoCombiTrans* trd_I_hori_combi03 = new TGeoCombiTrans(0., xBarPosYtop[0], PilPosZ[1], roty090);
      trd_1->AddNode(trd_I_hori_vol1, 13, trd_I_hori_combi03);
      TGeoCombiTrans* trd_I_hori_combi04 = new TGeoCombiTrans(0.,-xBarPosYbot[0], PilPosZ[1], roty090);
      trd_1->AddNode(trd_I_hori_vol1, 14, trd_I_hori_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoBBox* trd_I_hori1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, AperX[1]);
      TGeoVolume* trd_I_hori1     = new TGeoVolume("trd_I_x21", trd_I_hori1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_hori2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., AperX[1]);
      TGeoVolume* trd_I_hori2     = new TGeoVolume("trd_I_x22", trd_I_hori2_keep, aluminiumVolMed);

      trd_I_hori1->SetLineColor(kRed);
      trd_I_hori2->SetLineColor(kRed);
    
      TGeoTranslation *tx01 = new TGeoTranslation("tx01", 0.,                      0., 0.);
      TGeoTranslation *tx02 = new TGeoTranslation("tx02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *tx03 = new TGeoTranslation("tx03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_hori_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., AperX[1]);
      TGeoVolume* trd_I_hori_vol1    = new TGeoVolume("trd_I_x20", trd_I_hori_vol1_keep, keepVolMed);

      // build I-bar trd_I_hori_vol1
      trd_I_hori_vol1->AddNode(trd_I_hori1, 1, tx01);
      trd_I_hori_vol1->AddNode(trd_I_hori2, 2, tx02);
      trd_I_hori_vol1->AddNode(trd_I_hori2, 3, tx03);
    
      TGeoCombiTrans* trd_I_hori_combi01 = new TGeoCombiTrans(0., xBarPosYtop[1], PilPosZ[2], roty090);
      trd_2->AddNode(trd_I_hori_vol1, 21, trd_I_hori_combi01);
      TGeoCombiTrans* trd_I_hori_combi02 = new TGeoCombiTrans(0.,-xBarPosYbot[1], PilPosZ[2], roty090);
      trd_2->AddNode(trd_I_hori_vol1, 22, trd_I_hori_combi02);
      TGeoCombiTrans* trd_I_hori_combi03 = new TGeoCombiTrans(0., xBarPosYtop[1], PilPosZ[3], roty090);
      trd_2->AddNode(trd_I_hori_vol1, 23, trd_I_hori_combi03);
      TGeoCombiTrans* trd_I_hori_combi04 = new TGeoCombiTrans(0.,-xBarPosYbot[1], PilPosZ[3], roty090);
      trd_2->AddNode(trd_I_hori_vol1, 24, trd_I_hori_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoBBox* trd_I_hori1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, AperX[2]);
      TGeoVolume* trd_I_hori1     = new TGeoVolume("trd_I_x31", trd_I_hori1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_hori2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., AperX[2]);
      TGeoVolume* trd_I_hori2     = new TGeoVolume("trd_I_x32", trd_I_hori2_keep, aluminiumVolMed);

      trd_I_hori1->SetLineColor(kRed);
      trd_I_hori2->SetLineColor(kRed);
    
      TGeoTranslation *tx01 = new TGeoTranslation("tx01", 0.,                      0., 0.);
      TGeoTranslation *tx02 = new TGeoTranslation("tx02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *tx03 = new TGeoTranslation("tx03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_hori_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., AperX[2]);
      TGeoVolume* trd_I_hori_vol1    = new TGeoVolume("trd_I_x30", trd_I_hori_vol1_keep, keepVolMed);

      // build I-bar trd_I_hori_vol1
      trd_I_hori_vol1->AddNode(trd_I_hori1, 1, tx01);
      trd_I_hori_vol1->AddNode(trd_I_hori2, 2, tx02);
      trd_I_hori_vol1->AddNode(trd_I_hori2, 3, tx03);

      TGeoCombiTrans* trd_I_hori_combi01 = new TGeoCombiTrans(0., xBarPosYtop[2], PilPosZ[4], roty090);
      trd_3->AddNode(trd_I_hori_vol1, 31, trd_I_hori_combi01);	            
      TGeoCombiTrans* trd_I_hori_combi02 = new TGeoCombiTrans(0.,-xBarPosYbot[2], PilPosZ[4], roty090);
      trd_3->AddNode(trd_I_hori_vol1, 32, trd_I_hori_combi02);	            
      TGeoCombiTrans* trd_I_hori_combi03 = new TGeoCombiTrans(0., xBarPosYtop[2], PilPosZ[5], roty090);
      trd_3->AddNode(trd_I_hori_vol1, 33, trd_I_hori_combi03);	            
      TGeoCombiTrans* trd_I_hori_combi04 = new TGeoCombiTrans(0.,-xBarPosYbot[2], PilPosZ[5], roty090);
      trd_3->AddNode(trd_I_hori_vol1, 34, trd_I_hori_combi04);
    }


//-------------------
// horizontal supports (Z)
//-------------------

  // station 1
  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      TGeoBBox* trd_I_slope1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, (PilPosZ[1]-PilPosZ[0]-I_width)/2.);
      TGeoVolume* trd_I_slope1     = new TGeoVolume("trd_I_z11", trd_I_slope1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_slope2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., (PilPosZ[1]-PilPosZ[0]-I_width)/2.);
      TGeoVolume* trd_I_slope2     = new TGeoVolume("trd_I_z12", trd_I_slope2_keep, aluminiumVolMed);

      trd_I_slope1->SetLineColor(kYellow);
      trd_I_slope2->SetLineColor(kYellow);
    
      TGeoTranslation *tz01 = new TGeoTranslation("tz01", 0.,                      0., 0.);
      TGeoTranslation *tz02 = new TGeoTranslation("tz02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *tz03 = new TGeoTranslation("tz03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_slope_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., (PilPosZ[1]-PilPosZ[0]-I_width)/2.);
      TGeoVolume* trd_I_slope_vol1    = new TGeoVolume("trd_I_z10", trd_I_slope_vol1_keep, keepVolMed);

      // build I-bar trd_I_slope_vol1
      trd_I_slope_vol1->AddNode(trd_I_slope1, 1, tz01);
      trd_I_slope_vol1->AddNode(trd_I_slope2, 2, tz02);
      trd_I_slope_vol1->AddNode(trd_I_slope2, 3, tz03);

      PilPosX = AperX[0];
        
      TGeoCombiTrans* trd_I_slope_combi01 = new TGeoCombiTrans( (PilPosX+I_height/2.), zBarPosYtop[0], (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_I_slope_vol1, 11, trd_I_slope_combi01);
      TGeoCombiTrans* trd_I_slope_combi02 = new TGeoCombiTrans(-(PilPosX+I_height/2.), zBarPosYtop[0], (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_I_slope_vol1, 12, trd_I_slope_combi02);
      TGeoCombiTrans* trd_I_slope_combi03 = new TGeoCombiTrans( (PilPosX+I_height/2.),-zBarPosYbot[0], (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_I_slope_vol1, 13, trd_I_slope_combi03);
      TGeoCombiTrans* trd_I_slope_combi04 = new TGeoCombiTrans(-(PilPosX+I_height/2.),-zBarPosYbot[0], (PilPosZ[0]+PilPosZ[1])/2., rotz090);
      trd_1->AddNode(trd_I_slope_vol1, 14, trd_I_slope_combi04);
    }

  // station 2
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      TGeoBBox* trd_I_slope1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, (PilPosZ[3]-PilPosZ[2]-I_width)/2.);
      TGeoVolume* trd_I_slope1     = new TGeoVolume("trd_I_z21", trd_I_slope1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_slope2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., (PilPosZ[3]-PilPosZ[2]-I_width)/2.);
      TGeoVolume* trd_I_slope2     = new TGeoVolume("trd_I_z22", trd_I_slope2_keep, aluminiumVolMed);

      trd_I_slope1->SetLineColor(kYellow);
      trd_I_slope2->SetLineColor(kYellow);
    
      TGeoTranslation *tz01 = new TGeoTranslation("tz01", 0.,                      0., 0.);
      TGeoTranslation *tz02 = new TGeoTranslation("tz02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *tz03 = new TGeoTranslation("tz03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_slope_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., (PilPosZ[3]-PilPosZ[2]-I_width)/2.);
      TGeoVolume* trd_I_slope_vol1    = new TGeoVolume("trd_I_z20", trd_I_slope_vol1_keep, keepVolMed);

      // build I-bar trd_I_slope_vol1
      trd_I_slope_vol1->AddNode(trd_I_slope1, 1, tz01);
      trd_I_slope_vol1->AddNode(trd_I_slope2, 2, tz02);
      trd_I_slope_vol1->AddNode(trd_I_slope2, 3, tz03);

      PilPosX = AperX[1];
        
      TGeoCombiTrans* trd_I_slope_combi01 = new TGeoCombiTrans( (PilPosX+I_height/2.), zBarPosYtop[1], (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_I_slope_vol1, 21, trd_I_slope_combi01);
      TGeoCombiTrans* trd_I_slope_combi02 = new TGeoCombiTrans(-(PilPosX+I_height/2.), zBarPosYtop[1], (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_I_slope_vol1, 22, trd_I_slope_combi02);
      TGeoCombiTrans* trd_I_slope_combi03 = new TGeoCombiTrans( (PilPosX+I_height/2.),-zBarPosYbot[1], (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_I_slope_vol1, 23, trd_I_slope_combi03);
      TGeoCombiTrans* trd_I_slope_combi04 = new TGeoCombiTrans(-(PilPosX+I_height/2.),-zBarPosYbot[1], (PilPosZ[2]+PilPosZ[3])/2., rotz090);
      trd_2->AddNode(trd_I_slope_vol1, 24, trd_I_slope_combi04);
    }

  // station 3
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      TGeoBBox* trd_I_slope1_keep  = new TGeoBBox("", I_thick /2., I_height /2. - I_thick, (PilPosZ[5]-PilPosZ[4]-I_width)/2.);
      TGeoVolume* trd_I_slope1     = new TGeoVolume("trd_I_z31", trd_I_slope1_keep, aluminiumVolMed);
      TGeoBBox* trd_I_slope2_keep  = new TGeoBBox("", I_width /2.,            I_thick /2., (PilPosZ[5]-PilPosZ[4]-I_width)/2.);
      TGeoVolume* trd_I_slope2     = new TGeoVolume("trd_I_z32", trd_I_slope2_keep, aluminiumVolMed);

      trd_I_slope1->SetLineColor(kYellow);
      trd_I_slope2->SetLineColor(kYellow);
    
      TGeoTranslation *tz01 = new TGeoTranslation("tz01", 0.,                      0., 0.);
      TGeoTranslation *tz02 = new TGeoTranslation("tz02", 0.,  (I_height-I_thick) /2., 0.);
      TGeoTranslation *tz03 = new TGeoTranslation("tz03", 0., -(I_height-I_thick) /2., 0.);
    
      TGeoBBox* trd_I_slope_vol1_keep = new TGeoBBox("", I_width /2., I_height /2., (PilPosZ[5]-PilPosZ[4]-I_width)/2.);
      TGeoVolume* trd_I_slope_vol1    = new TGeoVolume("trd_I_z30", trd_I_slope_vol1_keep, keepVolMed);

      // build I-bar trd_I_slope_vol1
      trd_I_slope_vol1->AddNode(trd_I_slope1, 1, tz01);
      trd_I_slope_vol1->AddNode(trd_I_slope2, 2, tz02);
      trd_I_slope_vol1->AddNode(trd_I_slope2, 3, tz03);

      PilPosX = AperX[2];
        
      TGeoCombiTrans* trd_I_slope_combi01 = new TGeoCombiTrans( (PilPosX+I_height/2.), zBarPosYtop[2], (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_I_slope_vol1, 31, trd_I_slope_combi01);
      TGeoCombiTrans* trd_I_slope_combi02 = new TGeoCombiTrans(-(PilPosX+I_height/2.), zBarPosYtop[2], (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_I_slope_vol1, 32, trd_I_slope_combi02);
      TGeoCombiTrans* trd_I_slope_combi03 = new TGeoCombiTrans( (PilPosX+I_height/2.),-zBarPosYbot[2], (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_I_slope_vol1, 33, trd_I_slope_combi03);
      TGeoCombiTrans* trd_I_slope_combi04 = new TGeoCombiTrans(-(PilPosX+I_height/2.),-zBarPosYbot[2], (PilPosZ[4]+PilPosZ[5])/2., rotz090);
      trd_3->AddNode(trd_I_slope_vol1, 34, trd_I_slope_combi04);
    }

  if (IncludeLabels)
  {

    Int_t text_height    = 40;
    Int_t text_thickness =  8;
  
    TGeoTranslation *tr200 = new TGeoTranslation(0., (AperY[0]+I_height+ text_height/2.), PilPosZ[0]-I_width/2.+ text_thickness/2.);
    TGeoTranslation *tr201 = new TGeoTranslation(0., (AperY[1]+I_height+ text_height/2.), PilPosZ[2]-I_width/2.+ text_thickness/2.);
    TGeoTranslation *tr202 = new TGeoTranslation(0., (AperY[2]+I_height+ text_height/2.), PilPosZ[4]-I_width/2.+ text_thickness/2.);
  
    TGeoCombiTrans  *tr203 = new TGeoCombiTrans(-(AperX[0]+I_height+ text_thickness/2.), (AperY[0]+I_height-I_width-text_height/2.), (PilPosZ[0]+PilPosZ[1])/2., roty090);
    TGeoCombiTrans  *tr204 = new TGeoCombiTrans(-(AperX[1]+I_height+ text_thickness/2.), (AperY[1]+I_height-I_width-text_height/2.), (PilPosZ[2]+PilPosZ[3])/2., roty090);
    TGeoCombiTrans  *tr205 = new TGeoCombiTrans(-(AperX[2]+I_height+ text_thickness/2.), (AperY[2]+I_height-I_width-text_height/2.), (PilPosZ[4]+PilPosZ[5])/2., roty090);
  
    TGeoCombiTrans  *tr206 = new TGeoCombiTrans( (AperX[0]+I_height+ text_thickness/2.), (AperY[0]+I_height-I_width-text_height/2.), (PilPosZ[0]+PilPosZ[1])/2., roty270);
    TGeoCombiTrans  *tr207 = new TGeoCombiTrans( (AperX[1]+I_height+ text_thickness/2.), (AperY[1]+I_height-I_width-text_height/2.), (PilPosZ[2]+PilPosZ[3])/2., roty270);
    TGeoCombiTrans  *tr208 = new TGeoCombiTrans( (AperX[2]+I_height+ text_thickness/2.), (AperY[2]+I_height-I_width-text_height/2.), (PilPosZ[4]+PilPosZ[5])/2., roty270);
  
    TGeoVolume* trdbox1 = new TGeoVolumeAssembly("trdbox1"); // volume for TRD text (108, 40, 8)
    TGeoVolume* trdbox2 = new TGeoVolumeAssembly("trdbox2"); // volume for TRD text (108, 40, 8)
    TGeoVolume* trdbox3 = new TGeoVolumeAssembly("trdbox3"); // volume for TRD text (108, 40, 8)
    add_trd_labels(trdbox1, trdbox2, trdbox3);
  
    // final placement
    if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    {
      //    trd_1->AddNode(trdbox1, 1, tr200);
      trd_1->AddNode(trdbox1, 4, tr203);
      trd_1->AddNode(trdbox1, 7, tr206);
    }
    if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    {
      //    trd_2->AddNode(trdbox2, 2, tr201);
      trd_2->AddNode(trdbox2, 5, tr204);
      trd_2->AddNode(trdbox2, 8, tr207);
    }
    if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    {
      //    trd_3->AddNode(trdbox3, 3, tr202);
      trd_3->AddNode(trdbox3, 6, tr205);
      trd_3->AddNode(trdbox3, 9, tr208);
    }
  }

  if (ShowLayer[0])  // if geometry contains layer 1 (1st layer of station 1)
    gGeoMan->GetVolume(geoVersion)->AddNode(trd_1, 1);
  if (ShowLayer[4])  // if geometry contains layer 5 (1st layer of station 2)
    gGeoMan->GetVolume(geoVersion)->AddNode(trd_2, 2);
  if (ShowLayer[8])  // if geometry contains layer 9 (1st layer of station 3)
    gGeoMan->GetVolume(geoVersion)->AddNode(trd_3, 3);

}

