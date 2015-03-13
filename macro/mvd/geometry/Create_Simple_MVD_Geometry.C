///                                             
/// \file Create_Simple_MVD_Geometry
/// \brief Generates simple MVD geometry in Root format.
///                                             

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

// here come the definitions for the differnt geometry versions 
// please uncomment the version for which you want to create the geo file
// comment all other geometries

/*
// v07a
const TString tagVersion   = "v07a";
const Int_t nrOfStations = 2;

const Double_t zPosStation[nrOfStations] = { 10., 20. };
const Double_t rMin[nrOfStations] = { 0.55, 1.05 };
const Double_t rMax[nrOfStations] = { 5., 10.};
const Double_t thick[nrOfStations] = { 0.0150, 0.0150 };

// v07b
const TString tagVersion   = "v07b";
const Int_t nrOfStations = 3;

const Double_t zPosStation[nrOfStations] = { 5., 10., 20. };
const Double_t rMin[nrOfStations] = { 0.55, 0.55, 1.05 };
const Double_t rMax[nrOfStations] = { 2.5, 5., 10.};
const Double_t thick[nrOfStations] = { 0.0150, 0.0150, 0.0150 };
*/

// v08a
const TString tagVersion   = "v08a";
const Int_t nrOfStations = 2;

const Double_t zPosStation[nrOfStations] = { 5., 10. };
const Double_t rMin[nrOfStations] = { 0.55, 0.55 };
const Double_t rMax[nrOfStations] = { 2.5, 5.};
const Double_t thick[nrOfStations] = { 0.0300, 0.0500 };


const TString geoVersion   = "mvd_" + tagVersion;
const TString FileNameSim  = geoVersion + ".geo.root";
const TString FileNameGeo  = geoVersion + "_geo.root";
const TString FileNameInfo = geoVersion + ".geo.info";

//

// Names of the different used materials which are used to build the modules
// The materials are defined in the global media.geo file 
const TString KeepingVolumeMedium     = "air";
const TString DetectorVolumeMedium    = "silicon";

// some global variables
TGeoManager* gGeoMan = NULL;  // Pointer to TGeoManager instance

// Forward declarations
void create_materials_from_media_file();

void Create_Simple_MVD_Geometry() {
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
  TGeoBBox* topbox= new TGeoBBox("", 1000., 1000., 2000.);
  TGeoVolume* top = new TGeoVolume("top", topbox, gGeoMan->GetMedium("air"));
  gGeoMan->SetTopVolume(top);

  TGeoVolume* mvd = new TGeoVolumeAssembly(geoVersion);
  top->AddNode(mvd, 1);

  // Create target volume and add it as node to the mother volume
  for (Int_t iStation = 0; iStation < nrOfStations; ++iStation) {
    TString name;
    name.Form("mvdstation%02d", iStation+1);
    TGeoVolume* mvd1 = gGeoMan->MakeTube(name, gGeoMan->GetMedium("silicon"), rMin[iStation],
                                        rMax[iStation], thick[iStation]/2);
    TGeoTranslation* mvd1_trans = new TGeoTranslation(0., 0., zPosStation[iStation]);
    mvd->AddNode(mvd1, 0, mvd1_trans);
  }
  
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

  top->Draw("ogl");

  //top->Raytrace();
}


/*
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

  fclose(ifile);
}
*/

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
  FairGeoMedium* silicon          = geoMedia->getMedium(DetectorVolumeMedium);

  geoBuild->createMedium(air);
  geoBuild->createMedium(silicon);
}


