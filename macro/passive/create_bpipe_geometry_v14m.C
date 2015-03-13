/******************************************************************************
 ** Creation of beam pipe geometry in ROOT format (TGeo).
 **
 ** @file create_bpipe_geometry_v14m.C
 ** @author Andrey Chernogorov <a.chernogorov@gsi.de>
 ** @date 11.07.2014
 **
 ** SIS-300
 ** pipe_v14m = pipe_v14g + fixed sizes of vacuum chamber for mvd_v14a
 **
 ** The beam pipe is composed of aluminium with a thickness proportional to the
 ** diameter (D(z)mm/60). It is placed directly into the cave as mother volume.
 ** The beam pipe consists of few sections (including RICH/MUCH section). Each 
 ** section has a PCON shape (including windows). There are two windows: first 
 ** one @ 220mm with R600mm and 0.7mm thickness, second one of iron @ 8800mm 
 ** with R600mm and 0.2mm thickness. The STS section is composed of cylinder 
 ** D(z=220-500mm)=22mm and cone (z=500-1700mm). The STS (conical part) and 
 ** RICH sections have half opening angle 1.6deg as a result of optimization. 
 ** For downstream detectors the half opening angle of sections is left the 
 ** same (2.5deg) as in SIS-100 and section have cylindrical shape at z=6000-
 ** 8800mm - it must be regarded as an intermediate iteration, expecting 
 ** feedback from subdetectors' groups. The PSD section of the beam pipe is 
 ** missing because it is planned that it will be part of PSD geometry.
 *****************************************************************************/




#include <iomanip>
#include <iostream>
#include "TGeoManager.h"


using namespace std;


// -------------   Steering variables       -----------------------------------
// ---> Beam pipe material name
TString pipeMediumName = "aluminium"; // "beryllium" "carbon"
// ----------------------------------------------------------------------------


// -------------   Other global variables   -----------------------------------
// ---> Macros name to info file
TString macrosname = "create_bpipe_geometry_v14m.C";
// ---> Geometry file name (output)
TString rootFileName = "pipe_v14m.root";
// ---> Geometry name
TString pipeName = "pipe_v14m";
// ----------------------------------------------------------------------------


// ============================================================================
// ======                         Main function                           =====
// ============================================================================

void create_bpipe_geometry_v14m()
{
  // -----   Define beam pipe sections   --------------------------------------
  /** For v14m:   **/
  TString pipe1name = "pipe1 - vacuum chamber";
  const Int_t nSects1 = 6;
  Double_t z1[nSects1]    = { -50.,  -5.,   -5.,  230.17, 230.17, 230.87 }; // mm
  Double_t rin1[nSects1]  = {  25.,  25.,  400.,  400.,   110.,   110.   };
  Double_t rout1[nSects1] = {  25.7, 25.7, 400.7, 400.7,  400.7,  130.7  };
  TString pipe2name = "pipe2 - first window @ 220mm, h=0.7mm, R=600mm";
  const Int_t nSects2 = 7;
  Double_t z2[nSects2]    = { 220., 220.7, 221.45, 223.71, 227.49, 230.17, 230.87 }; // mm
  Double_t rin2[nSects2]  = {  11.,  11.,   30.,    60.,    90.,   105.86, 110.   };
  Double_t rout2[nSects2] = {  11.,  28.69, 39.3,   65.55,  94.14, 110.,   110.   };
  TString pipevac1name = "pipevac1";
  const Int_t nSects01 = 10;
  Double_t z01[nSects01]    = { -50., -5.,  -5., 220., 220., 220.7,  221.45, 223.71, 227.49, 230.17 }; // mm
  Double_t rin01[nSects01]  = {   0.,  0.,   0.,   0.,  18.,  28.69,  39.3,   65.55,  94.14, 110.   };
  Double_t rout01[nSects01] = {  25., 25., 400., 400., 400., 400.,   400.,   400.,   400.,   400.   };
  
  TString pipe3name = "pipe3 - STS section";
  const Int_t nSects3 = 4;
  Double_t z3[nSects3]    = { 220.,  530.,  1250.,  1700.   }; // mm
  Double_t rout3[nSects3] = {  11.,   11.,    34.9,   47.5 };
  Double_t rin3[nSects3]; for(Int_t i=0; i<nSects3; i++) { rin3[i] = rout3[i] - rout3[i]/30.; }
  TString pipevac2name = "pipevac2";
  const Int_t nSects02 = nSects3;
  Double_t z02[nSects02]    = { 220.,  530.,  1250.,   1700. }; // mm
  Double_t rin02[nSects02]  = {   0.,    0.,     0.,      0. };
  Double_t rout02[nSects02]; for(Int_t i=0; i<nSects02; i++) { rout02[i]=rin3[i]; }
  
  TString pipe4name = "pipe4 - RICH section";
  const Int_t nSects4 = 3;
  Double_t z4[nSects4]    = { 1700.,   1800.,   3700.   }; // mm
  Double_t rout4[nSects4] = {   47.5,    47.5,   103.35 };
  Double_t rin4[nSects4]; for(Int_t i=0; i<nSects4; i++) { rin4[i] = rout4[i] - rout4[i]/30.; }
  TString pipevac3name = "pipevac3";
  const Int_t nSects03 = nSects4;
  Double_t z03[nSects03]    = { 1700.,   1800.,   3700.   }; // mm
  Double_t rin03[nSects03]  = {    0.,      0.,      0.   };
  Double_t rout03[nSects03]; for(Int_t i=0; i<nSects03; i++) { rout03[i]=rin4[i]; }
  
  TString pipe5name = "pipe5 - TRD & TOF section";
  const Int_t nSects5 = 5;
  Double_t z5[nSects5]    = { 3700.,  3702.,  3702.,  6000.,   8800.   }; // mm
  Double_t rout5[nSects5] = {  161.6,  161.6,  161.6,  262.,    262.   };
  Double_t rin5[nSects5]  = {   99.9,   99.9,  156.2,  253.27,  253.27 };
  //Double_t rin5[nSects5]; for(Int_t i=0; i<nSects5; i++) { rin5[i] = rout5[i] - rout5[i]/30.; }
  
  TString pipevac4name = "pipevac4";
  const Int_t nSects04 = 10;
  Double_t z04[nSects04]    = { 3700.,  3702.,  3702.,  6000.,  8743.24,  8743.44, 8747.34, 8759.8,  8781.19, 8799.8  }; // mm
  Double_t rin04[nSects04]  = {    0.,     0.,     0.,     0.,     0.,       3.42,   70.,    140.,    210.,    253.23 };
  Double_t rout04[nSects04] = {   99.9,   99.9,  156.17, 253.27,  253.27,   253.27,  253.27,  253.27,  253.27,  253.27 };
  
  TString pipe6name = "pipe6 - second window @ 6000mm, h=0.2mm, R=600mm"; // iron !!!
  const Int_t nSects6 = 7;
  Double_t z6[nSects6]     = { 8743.24, 8743.44, 8747.34, 8759.8,  8781.19, 8799.8,  8800.   }; // mm
  Double_t rin6[nSects6]   = {    0.,      0.,     66.58,  138.88,  209.35,  252.28,  253.27 };
  Double_t rout6[nSects6]  = {    0.,      3.42,   70.,    140.,    210.,    253.27,  253.27 };


  // --------------------------------------------------------------------------

  // -------------  Load the necessary FairRoot libraries   -------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  // --------------------------------------------------------------------------


  // -------   Open info file   -----------------------------------------------
  TString infoFileName = rootFileName;
  infoFileName.ReplaceAll("root", "info");
  fstream infoFile;
  fstream infoFileEmpty;
  infoFile.open(infoFileName.Data(), fstream::out);
  infoFile << "SIS-300. Beam pipe geometry created with " + macrosname << endl << endl;
  infoFile << "	  pipe_v14m = pipe_v14g + fixed sizes of vacuum chamber for mvd_v14a" << endl << endl;
  infoFile << " The beam pipe is composed of aluminium with a thickness proportional to the" << endl;
  infoFile << " diameter (D(z)mm/60). It is placed directly into the cave as mother volume." << endl;
  infoFile << " The beam pipe consists of few sections (including RICH/MUCH section). Each " << endl;
  infoFile << " section has a PCON shape (including windows). There are two windows: first " << endl;
  infoFile << " one @ 220mm with R600mm and 0.7mm thickness, second one of iron @ 8800mm " << endl;
  infoFile << " with R600mm and 0.2mm thickness. The STS section is composed of cylinder " << endl;
  infoFile << " D(z=220-500mm)=22mm and cone (z=500-1700mm). The STS (conical part) and " << endl;
  infoFile << " RICH sections have half opening angle 1.6deg as a result of optimization. " << endl;
  infoFile << " For downstream detectors the half opening angle of sections is left the " << endl;
  infoFile << " same (2.5deg) as in SIS-100 and section have cylindrical shape at z=6000-" << endl;
  infoFile << " 8800mm - it must be regarded as an intermediate iteration, expecting " << endl;
  infoFile << " feedback from subdetectors' groups. The PSD section of the beam pipe is " << endl;
  infoFile << " missing because it is planned that it will be part of PSD geometry." << endl << endl;
  
  infoFile << "Material:  " << pipeMediumName << endl;
  infoFile << "Thickness: D(z)mm/60" << endl << endl;
  // --------------------------------------------------------------------------


  // -------   Load media from media file   -----------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  TGeoManager* gGeoMan = gGeoManager;
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  
  // ---> pipe medium
  FairGeoMedium* fPipeMedium = geoMedia->getMedium(pipeMediumName.Data());
  TString fairError = "FairMedium " + pipeMediumName + " not found";
  if ( ! fPipeMedium ) Fatal("Main", fairError.Data());
  geoBuild->createMedium(fPipeMedium);
  TGeoMedium* pipeMedium = gGeoMan->GetMedium(pipeMediumName.Data());
  TString geoError = "Medium " + pipeMediumName + " not found";
  if ( ! pipeMedium ) Fatal("Main", geoError.Data());
  // ---> iron
  FairGeoMedium* mIron = geoMedia->getMedium("iron");
  if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mIron);
  TGeoMedium* iron = gGeoMan->GetMedium("iron");
  if ( ! iron ) Fatal("Main", "Medium iron not found");
  // ---> vacuum
  FairGeoMedium* mVacuum = geoMedia->getMedium("vacuum");
  if ( ! mVacuum ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVacuum);
  TGeoMedium* vacuum = gGeoMan->GetMedium("vacuum");
  if ( ! vacuum ) Fatal("Main", "Medium vacuum not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("PIPEgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  TGeoVolume* pipe = new TGeoVolumeAssembly(pipeName.Data());
  // --------------------------------------------------------------------------


  // -----   Create sections  -------------------------------------------------
  infoFile << endl << "Beam pipe section: " << pipe1name << endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  
  TGeoVolume* pipe1    = MakePipe  (1, nSects1,  z1,  rin1,  rout1,  pipeMedium, &infoFile); 
  pipe1->SetLineColor(kGray);
  pipe->AddNode(pipe1, 0);
  
  infoFile << endl << "Beam pipe section: " << pipe2name << endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe2    = MakePipe  (2, nSects2,  z2,  rin2,  rout2,  pipeMedium, &infoFile); 
  pipe2->SetLineColor(kBlue);
  pipe->AddNode(pipe2, 0);
  TGeoVolume* pipevac1 = MakeVacuum(1, nSects01, z01, rin01, rout01, vacuum,     &infoFile); 
  pipevac1->SetLineColor(kCyan);
  pipe->AddNode(pipevac1, 0);
  
  infoFile << endl << "Beam pipe section: " << pipe3name << endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe3    = MakePipe  (3, nSects3,  z3,  rin3,  rout3,  pipeMedium, &infoFile); 
  pipe3->SetLineColor(kGreen);
  pipe->AddNode(pipe3, 0);
  TGeoVolume* pipevac2 = MakeVacuum(2, nSects02, z02, rin02, rout02, vacuum,     &infoFile); 
  pipevac2->SetLineColor(kCyan);
  pipe->AddNode(pipevac2, 0);
  
  infoFile << endl << "Beam pipe section: " << pipe4name << endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe4    = MakePipe  (4, nSects4,  z4,  rin4,  rout4,  pipeMedium, &infoFile); 
  pipe4->SetLineColor(kGreen+2);
  pipe->AddNode(pipe4, 0);
  TGeoVolume* pipevac3 = MakeVacuum(3, nSects03, z03, rin03, rout03, vacuum,     &infoFile); 
  pipevac3->SetLineColor(kCyan);
  pipe->AddNode(pipevac3, 0);
  
  infoFile << endl << "Beam pipe section: " << pipe5name << endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe5    = MakePipe  (5, nSects5,  z5,  rin5,  rout5,  pipeMedium, &infoFile); 
  pipe5->SetLineColor(kGreen);
  pipe->AddNode(pipe5, 0);
  TGeoVolume* pipevac4 = MakeVacuum(4, nSects04, z04, rin04, rout04, vacuum,     &infoFile); 
  pipevac4->SetLineColor(kCyan);
  pipe->AddNode(pipevac4, 0);

  infoFile << endl << "Beam pipe section: " << pipe6name << ", material: iron" << endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe6 = MakePipe(6, nSects6,  z6,  rin6,  rout6, iron, &infoFile);
  pipe6->SetLineColor(kBlue);
  pipe->AddNode(pipe6, 0);
  
  
  // -----   End   --------------------------------------------------

  // ---------------   Finish   -----------------------------------------------
  top->AddNode(pipe, 1);
  cout << endl << endl;
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  // visualize it with ray tracing, OGL/X3D viewer
  //top->Raytrace();
  top->Draw("ogl");
  //top->Draw("x3d");

  TFile* rootFile = new TFile(rootFileName, "RECREATE");
  top->Write();
  cout << endl;
  cout << "Geometry " << top->GetName() << " written to " 
       << rootFileName << endl;
  rootFile->Close();
  infoFile.close();


}
// ============================================================================
// ======                   End of main function                          =====
// ============================================================================


// =====  Make the beam pipe volume   =========================================
TGeoPcon* MakeShape(Int_t nSects, char* name, Double_t* z, Double_t* rin, 
                    Double_t* rout, fstream* infoFile) {

  // ---> Shape
  TGeoPcon* shape = new TGeoPcon(name, 0., 360., nSects);
  for (Int_t iSect = 0; iSect < nSects; iSect++) {
    shape->DefineSection(iSect, z[iSect]/10., rin[iSect]/10., rout[iSect]/10.); // mm->cm
    *infoFile << setw(2)  << iSect+1
              << setw(10) << fixed << setprecision(2) << z[iSect]
              << setw(10) << fixed << setprecision(2) << rin[iSect]
              << setw(10) << fixed << setprecision(2) << rout[iSect]
              << setw(10) << fixed << setprecision(2) << rout[iSect]-rin[iSect] << endl;
  }

  return shape;

}
// ============================================================================




// =====  Make the beam pipe volume   =========================================
TGeoVolume* MakePipe(Int_t iPart, Int_t nSects, Double_t* z, Double_t* rin, 
	                 Double_t* rout, TGeoMedium* medium, fstream* infoFile) {

  // ---> Shape
  TGeoPcon* shape = new TGeoPcon(0., 360., nSects);
  for (Int_t iSect = 0; iSect < nSects; iSect++) {
    shape->DefineSection(iSect, z[iSect]/10., rin[iSect]/10., rout[iSect]/10.); // mm->cm
    *infoFile << setw(2)  << iSect+1
              << setw(10) << fixed << setprecision(2) << z[iSect]
              << setw(10) << fixed << setprecision(2) << rin[iSect]
              << setw(10) << fixed << setprecision(2) << rout[iSect]
              << setw(10) << fixed << setprecision(2) << rout[iSect]-rin[iSect] << endl;
  }

  // ---> Volume
  TString volName = Form("pipe%i", iPart);
  TGeoVolume* pipe = new TGeoVolume(volName.Data(), shape, medium);

  return pipe;

}
// ============================================================================



// =====   Make the volume for the vacuum inside the beam pipe   ==============
TGeoVolume* MakeVacuum(Int_t iPart, Int_t nSects, Double_t* z, Double_t* rin, 
	                   Double_t* rout, TGeoMedium* medium, fstream* infoFile) {

  // ---> Shape
  TGeoPcon* shape = new TGeoPcon(0., 360., nSects);
  for (Int_t iSect = 0; iSect < nSects; iSect++) {
    shape->DefineSection(iSect, z[iSect]/10., rin[iSect]/10., rout[iSect]/10.); // mm->cm
  }

  // ---> Volume
  TString volName = Form("pipevac%i", iPart);
  TGeoVolume* pipevac = new TGeoVolume(volName.Data(), shape, medium);

  return pipevac;

}
// ============================================================================
