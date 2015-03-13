// in root all sizes are given in cm

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

// Name of geometry version and output file
const TString geoVersion = "tof_v13-3a";
const TString FileName = geoVersion + ".root";

// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString BoxVolumeMedium         = "aluminium";
const TString NoActivGasMedium        = "RPCgas_noact";
const TString ActivGasMedium          = "RPCgas";
const TString GlasMedium              = "RPCglass";
const TString ElectronicsMedium       = "carbon";

// Distance to the center of the TOF wall [cm];
const Float_t Wall_Z_Position = 600; 

const Int_t NumberOfDifferentCounterTypes = 4;
const Float_t Glass_X[NumberOfDifferentCounterTypes] = {32., 52., 32.,32.};
const Float_t Glass_Y[NumberOfDifferentCounterTypes] = {26.9, 53.,20.,10.};
const Float_t Glass_Z[NumberOfDifferentCounterTypes] = {0.1,0.1,0.1,0.1};

const Float_t GasGap_X[NumberOfDifferentCounterTypes] = {32.,52.,32.,32.};
const Float_t GasGap_Y[NumberOfDifferentCounterTypes] = {26.9,53.,20.,10.};
const Float_t GasGap_Z[NumberOfDifferentCounterTypes] = {0.025,0.025,0.025,0.025};

const Int_t NumberOfGaps[NumberOfDifferentCounterTypes] = {8,8,8,8};
//const Int_t NumberOfGaps[NumberOfDifferentCounterTypes] = {1,1,1,1}; //deb
const Int_t NumberOfReadoutStrips[NumberOfDifferentCounterTypes] = {32,52,96,96};
//const Int_t NumberOfReadoutStrips[NumberOfDifferentCounterTypes] = {1,1,1,1}; //deb

const Float_t SingleStackStartPosition_Z[NumberOfDifferentCounterTypes] = {-0.6,-0.6,-0.6,-0.6};

const Float_t Electronics_X[NumberOfDifferentCounterTypes] = {34.0,53.0,32.0,32.};
const Float_t Electronics_Y[NumberOfDifferentCounterTypes] = {5.0,5.0,1.0,1.};
const Float_t Electronics_Z[NumberOfDifferentCounterTypes] = {0.3,0.3,0.3,0.3};

const Int_t NofModuleTypes = 6;

// Aluminum box for all module types
const Float_t Module_Size_X[NofModuleTypes] = {180.,180.,180.,180.,180.,74.};
const Float_t Module_Size_Y[NofModuleTypes] = {49.,49.,74.,28.,18.,18.};
const Float_t Module_Over_Y[NofModuleTypes] = {11.5,11.5,11.,4.5,4.5,5.1};
const Float_t Module_Size_Z[NofModuleTypes] = {10.,10.,13.,10.,10.,10.};
const Float_t Module_Thick_Alu_X_left = 5.;
const Float_t Module_Thick_Alu_X_right = 1.;
const Float_t Module_Thick_Alu_Y = 1.;
const Float_t Module_Thick_Alu_Z = 0.1;

//Type of Counter for module 
const Int_t CounterTypeInModule[NofModuleTypes] = {0,0,1,2,3,3};
const Int_t NCounterInModule[NofModuleTypes]    = {5,5,3,5,5,2};

// Placement of the counter inside the module
const Float_t CounterXStartPosition[NofModuleTypes] = {-60.0, -66.0, -56.0,-60.0,-60.0,-16.0};
const Float_t CounterXDistance[NofModuleTypes]      = {30.0,   32.0,  51.0, 30.0, 30.0, 30.0};
const Float_t CounterZDistance[NofModuleTypes]      = {2.5,     0.0,   0.0,  2.5,  2.5,  2.5};
const Float_t CounterRotationAngle[NofModuleTypes]  = {0.,      8.7,   7.0,  0.,   0.,   0.};

// Pole (support structure)
const Int_t MaxNumberOfPoles=20;
Float_t Pole_ZPos[MaxNumberOfPoles];
Float_t Pole_Col[MaxNumberOfPoles];
Int_t NumberOfPoles=0;

const Float_t Pole_Size_X = 20.;
const Float_t Pole_Size_Y = 1000.;
const Float_t Pole_Size_Z = 10.;
const Float_t Pole_Thick_X = 5.;
const Float_t Pole_Thick_Y = 5.;
const Float_t Pole_Thick_Z = 5.;

Float_t Pole_ZPos[MaxNumberOfPoles];


// Bars (support structure)
const Float_t Bar_Size_X = 20.;
const Float_t Bar_Size_Y = 20.;
Float_t Bar_Size_Z = 100.;

const Int_t MaxNumberOfBars=20;
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
TGeoVolume* gBar[MaxNumberOfBars];

Float_t Last_Size_Y=0.;
Float_t Last_Over_Y=0.;

// Forward declarations
void create_materials_from_media_file();
TGeoVolume* create_counter(Int_t);
TGeoVolume* create_new_counter(Int_t);
TGeoVolume* create_tof_module(Int_t);
TGeoVolume* create_new_tof_module(Int_t);
TGeoVolume* create_tof_pole();
TGeoVolume* create_tof_bar();
void position_tof_poles(Int_t);
void position_tof_bars(Int_t);
void position_inner_tof_modules(Int_t);
void position_side_tof_modules(Int_t);
void position_outer_tof_modules(Int_t);

void Create_TOF_Geometry_v13_3a() {
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
  gGeoMan->SetVisLevel(5);  // 2 = super modules   
  gGeoMan->SetVisOption(0);  

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

  position_side_tof_modules(1);  // keep order !!
  position_inner_tof_modules(3);
  cout << "Outer Types "<<Outer_Module_Types[0][0]<<", "<<Outer_Module_Types[1][0]
       <<", col=1:  "<<Outer_Module_Types[0][1]<<", "<<Outer_Module_Types[1][1]
       <<endl;
  cout << "Outer Number "<<Outer_Module_Number[0][0]<<", "<<Outer_Module_Number[1][0]
       <<", col=1:  "<<Outer_Module_Number[0][1]<<", "<<Outer_Module_Number[1][1]
       <<endl;
  position_outer_tof_modules(4);
  position_tof_poles(0);
  position_tof_bars(0);
  
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* outfile = new TFile(FileName,"RECREATE");
  top->Write();
  //gGeoMan->Write();
  outfile->Close();
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

  // Add 8 single stacks + one glass plate at the end to a multi stack
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
  Float_t cdz = ngaps * (gdz+ggdz) + gdz + 0.2;

  //calculate thickness and first position in coonter of single stack
  Float_t dzpos=gdz+ggdz;
  Float_t startzposglas=(-cdz+gdz)/2.;
  Float_t startzposgas=-cdz/2.+ gdz + ggdz/2.;


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
  for( Int_t igap = 0; igap < ngaps; igap++) {

    Float_t zpos_glas = startzposglas + igap*dzpos;
    Float_t zpos_gas = startzposgas + igap*dzpos;

    //    cout <<"Zpos(Glas): "<< zpos_glas << endl;
    //    cout <<"Zpos(Gas): "<< zpos_gas << endl;

    TGeoTranslation* glass_plate_trans 
      = new TGeoTranslation("", 0., 0., zpos_glas);
    TGeoTranslation* gas_gap_trans 
      = new TGeoTranslation("", 0., 0., zpos_gas);
    counter->AddNode(glass_plate_vol, igap, glass_plate_trans);
    counter->AddNode(gas_gap_vol, igap, gas_gap_trans);

  }

  Float_t zpos_glas = startzposglas + (ngaps+1)*dzpos;
  TGeoTranslation* glass_plate_trans 
    = new TGeoTranslation("", 0., 0., zpos_glas);
  counter->AddNode(glass_plate_vol, ngaps, glass_plate_trans);
  
  // create and place the electronics above and below the glas stack
  TGeoBBox* pcb = new TGeoBBox("", dxe/2., dye/2., dze/2.);
  TGeoVolume* pcb_vol = 
    new TGeoVolume("pcb", pcb, electronicsVolMed);
  pcb_vol->SetLineColor(kCyan); // set line color for electronics
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

  Float_t shift_gas_box = (Module_Thick_Alu_X_right - Module_Thick_Alu_X_left)/2;

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
  Int_t cType = CounterTypeInModule[modType];
  Float_t dx=Module_Size_X[modType];
  Float_t dy=Module_Size_Y[modType];
  Float_t dz=Module_Size_Z[modType];
  Float_t width_aluxl=Module_Thick_Alu_X_left;
  Float_t width_aluxr=Module_Thick_Alu_X_right;
  Float_t width_aluy=Module_Thick_Alu_Y;
  Float_t width_aluz=Module_Thick_Alu_Z;

  Float_t shift_gas_box = (Module_Thick_Alu_X_right - Module_Thick_Alu_X_left)/2;

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

  TGeoBBox* pole_air_box = new TGeoBBox("", dx/2.-width_alux, dy/2.-width_aluy, dz/2.-width_aluz);
  TGeoVolume* pole_air_vol = 
    new TGeoVolume("pole_air", pole_air_box, airVolMed);
  pole_air_vol->SetLineColor(kYellow); // set line color for the alu box
  pole_air_vol->SetTransparency(70); // set transparency for the TOF
  TGeoTranslation* pole_air_trans 
    = new TGeoTranslation("", 0., 0., 0.);
  pole_alu_vol->AddNode(pole_air_vol, 0, pole_air_trans);

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
  for (Int_t i=0; i<NumberOfPoles; i++){
    if(i<2) {
     pole_trans 
      = new TGeoTranslation("", -Pole_Offset+2.0, 0., Pole_ZPos[i]);
     gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
     numPoles++;
    }else{
     Float_t xPos=Pole_Offset+Pole_Size_X/2.+Pole_Col[i]*DxColl;
     Float_t zPos=Pole_ZPos[i];
     pole_trans 
      = new TGeoTranslation("", xPos, 0., zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
     numPoles++;

     pole_trans 
      = new TGeoTranslation("", -xPos, 0., zPos);
     gGeoMan->GetVolume(geoVersion)->AddNode(gPole, numPoles, pole_trans);
     numPoles++;
    }
    cout << " Position Pole "<< numPoles<<" at z="<< Pole_ZPos[i] << endl;
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

   // horizontal frame bars 
   Int_t i = NumberOfBars;
   NumberOfBars++;
   gBar[i]=create_tof_bar(2.*xPos+Pole_Size_X,Bar_Size_Y,Bar_Size_Y);

   zPos = Pole_ZPos[0]+Pole_Size_Z/2.;
   bar_trans = new TGeoTranslation("", 0., yPos, zPos);
   gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
   numBars++;

   bar_trans = new TGeoTranslation("", 0., -yPos, zPos);
   gGeoMan->GetVolume(geoVersion)->AddNode(gBar[i], numBars, bar_trans);
   numBars++;

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
