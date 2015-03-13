/******************************************************************************
 ** Creation of beam pipe geometry in ROOT format (TGeo).
 **
 ** @file create_bpipe_geometry_v13e.C
 ** @author Andrey Chernogorov <a.chernogorov@gsi.de>
 ** @date 17.10.2013
 **
 ** The beam pipe is composed of aluminium with a thickness proportional to the
 ** diameter (0.017*D(z)mm). It is placed directly into the cave as mother 
 ** volume. The beam pipe consists of few sections (including RICH/MUCH 
 ** section). Each section has a PCON shape (including windows). There are two 
 ** windows: first one @ 220mm with R600mm and 0.7mm thickness, second one @ 
 ** 6000mm with R800mm and 3mm thickness. The last three sections (and second 
 ** window @ 6000mm) of beam pipe (1800-7990mm) are rotated by an angle of 
 ** X.XX degrees relative to point (0,0,1800) mm and the OY axis, following the
 ** bend of the ions in the magnetic field (field_v12b was used).
 *****************************************************************************/




#include <iomanip>
#include <iostream>
#include "TGeoManager.h"


using namespace std;


// -------------   Steering variables       -----------------------------------
// ---> Beam pipe material name
TString pipeMediumName = "aluminium"; // "beryllium" "carbon"
// ---> The rotation angle for last three sections of beam pipe (1800-7990mm):
Double_t rotangle = 1.35; // deg
// ----------------------------------------------------------------------------


// -------------   Other global variables   -----------------------------------
// ---> Macros name to info file
TString macrosname = "create_bpipe_geometry_v13e.C";
// ---> Geometry file name (output)
TString rootFileName = "pipe_v13e.root";
// ---> Geometry name
TString pipeName = "PIPE_v13e";
// ----------------------------------------------------------------------------


// ============================================================================
// ======                         Main function                           =====
// ============================================================================

void create_bpipe_geometry_v13e()
{
  // -----   Define beam pipe sections   --------------------------------------
  /** For v13e:   **/
  TString pipe1name = "pipe1 - vacuum chamber";
  const Int_t nSects1 = 6;
  Double_t z1[nSects1]    = { -50.,  25.,   35.,  230.17, 230.17, 230.87 }; // mm
  Double_t rin1[nSects1]  = {  25.,  25.,  130.,  130.,   110.,   110.   };
  Double_t rout1[nSects1] = {  25.7, 25.7, 130.7, 130.7,  130.7,  130.7  };
  TString pipe2name = "pipe2 - first window @ 220mm, h=0.7mm, R=600mm";
  const Int_t nSects2 = 7;
  Double_t z2[nSects2]    = { 220., 220.7, 221.45, 223.71, 227.49, 230.17, 230.87 }; // mm
  Double_t rin2[nSects2]  = {  18.,  18.,   30.,    60.,    90.,   105.86, 110.   };
  Double_t rout2[nSects2] = {  18.,  28.69, 39.3,   65.55,  94.14, 110.,   110.   };
  TString pipevac1name = "pipevac1";
  const Int_t nSects01 = 10;
  Double_t z01[nSects01]    = { -50., 25.,  35., 220., 220., 220.7,  221.45, 223.71, 227.49, 230.17 }; // mm
  Double_t rin01[nSects01]  = {   0.,  0.,   0.,   0.,  18.,  28.69,  39.3,   65.55,  94.14, 110.   };
  Double_t rout01[nSects01] = {  25., 25., 130., 130., 130., 130.,   130.,   130.,   130.,   130.   };
  TString pipe3name = "pipe3 - STS section";
  const Int_t nSects3 = 5;
  Double_t z3[nSects3]    = { 220.,  494.,  1250.,   1700., 1800. }; // mm
  Double_t rin3[nSects3]  = {  17.4,  17.4,   53.17,   58.5,  58.5};
  Double_t rout3[nSects3] = {  18.,   18.,    55.,     60.,   60. };
  TString pipevac2name = "pipevac2";
  const Int_t nSects02 = 5;
  Double_t z02[nSects02]    = { 220.,  494.,  1250.,   1700., 1800. }; // mm
  Double_t rin02[nSects02]  = {   0.,    0.,     0.,      0.,    0. };
  Double_t rout02[nSects02] = {  17.4,  17.4,   53.17,   58.5,  58.5};
  TString pipe4name = "pipe4 - RICH section";
  const Int_t nSects4 = 2;
  //Double_t z4[nSects4]    = { 1800., 3700.   }; // mm
  Double_t z4[nSects4]    = {    0., 1900.   }; // mm - 1800
  Double_t rin4[nSects4]  = {   58.5, 125.24 };
  Double_t rout4[nSects4] = {   60.,  129.56 };
  TString pipevac3name = "pipevac3";
  const Int_t nSects03 = 2;
  //Double_t z03[nSects03]    = { 1800., 3700.   }; // mm
  Double_t z03[nSects03]    = {    0., 1900.   }; // mm - 1800
  //Double_t rin03[nSects03]  = {    0.,    0.   };
  Double_t rin03[nSects03]  = {    0.00001,    0.00001   }; // TODO
  Double_t rout03[nSects03] = {   58.5, 125.24 };
  TString pipe5name = "pipe5 - TRD & TOF section";
  const Int_t nSects5 = 3;
  //Double_t z5[nSects5]    = { 3700.,   5996.97, 6000.   }; // mm
  Double_t z5[nSects5]    = { 1900.,   4196.97,  4200.  }; // mm - 1800
  Double_t rin5[nSects5]  = {  125.24,   183.6,   183.6 };
  Double_t rout5[nSects5] = {  129.56,   189.92,  190.  };
  TString pipevac4name = "pipevac4";
  const Int_t nSects04 = 7;
  //Double_t z04[nSects04]    = { 3700.,   5976.19, 5976.19, 5979.19, 5982.19, 5989.13, 5996.97 }; // mm
  Double_t z04[nSects04]    = { 1900.,   4176.19, 4176.19, 4179.19, 4182.19, 4189.13, 4196.97 }; // mm - 1800
  Double_t rin04[nSects04]  = {    0.,      0.,     30.,     75.41,  102.26,  146.38,  183.6  };
  Double_t rout04[nSects04] = {  125.24,  182.86,  182.86,  182.86,  183.,    183.19,  183.6  };
  TString pipe6name = "pipe6 - second window @ 6000mm, h=3mm, R=800mm";
  const Int_t nSects6 = 6;
  //Double_t z6[nSects6]     = {    0.,      3.,      6.,     12.94,   20.78,   23.81 }; // mm - 5976.19
  //Double_t z6[nSects6]     = { 5976.19, 5979.19, 5982.19, 5989.13, 5996.97, 6000.  }; // mm
  Double_t z6[nSects6]     = { 4176.19, 4179.19, 4182.19, 4189.13, 4196.97, 4200.  }; // mm - 1800
  Double_t rin6[nSects6]   = {   30.,     30.,     75.41,  129.22,  170.33,  183.6 };
  Double_t rout6[nSects6]  = {   30.,     75.41,  102.26,  146.38,  183.6,   183.6 };
  TString pipe7name = "pipe7 - PSD section";
  const Int_t nSects7 = 2;
  //Double_t z7[nSects7]    = { 5976.19, 7990. }; // mm
  Double_t z7[nSects7]    = { 4176.19,  6190. }; // mm - 1800
  Double_t rin7[nSects7]  = {   29.,      29. };
  Double_t rout7[nSects7] = {   30.,      30. };
  TString pipevac5name = "pipevac5";
  const Int_t nSects05 = 2;
  Double_t z05[nSects05]    = { 4176.19,  6190. }; // mm - 1800
  Double_t rin05[nSects05]  = {    0.,       0. };
  Double_t rout05[nSects05] = {   29.,      29. };
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
  infoFile << "Beam pipe geometry created with " + macrosname << endl << endl;

  infoFile << " The beam pipe is composed of aluminium with a thickness proportional to the" << endl;
  infoFile << " diameter (0.017*D(z)mm). It is placed directly into the cave as mother " << endl;
  infoFile << " volume. The beam pipe consists of few sections (including RICH/MUCH " << endl;
  infoFile << " section). Each section has a PCON shape (including windows). There are two " << endl;
  infoFile << " windows: first one @ 220mm with R600mm and 0.7mm thickness, second one @ " << endl;
  infoFile << " 6000mm with R800mm and 3mm thickness. The last three sections (and second " << endl;
  infoFile << " window @ 6000mm) of beam pipe (1800-7990mm) are rotated by an angle of " << endl;
  infoFile << " " << Form("%.2f",rotangle) << 
              " degrees relative to point (0,0,1800) mm and the OY axis, following the " << endl;
  infoFile << " bend of the ions in the magnetic field (field_v12b was used)." << endl << endl;
  
  infoFile << "Material:  " << pipeMediumName << endl;
  infoFile << "Thickness: 0.017*D(z) mm" << endl << endl;
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
  //*
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
  //*/
  
  // define some rotation & translation for pipe4-pipe7 & pipevac3-pipevac5
  // Initial phi   (Euler rotation angle about Z)
  // Initial theta (Euler rotation angle about new X)
  // Initial psi   (Euler rotation angle about new Z)
  cout<<endl<<"Rotation angle (@[0;0;1800]mm)="<< rotangle <<"deg";
  TGeoRotation    *r1 = new TGeoRotation("r1", 90., rotangle, 0.); // deg - Euler angles
  r1->RegisterYourself();
  Double_t shift = rotangle/10.+0.1; // cm - volume boolean operation correction - TODO
  Double_t dx=0., dy=0., dz=1800.-10.*shift; // mm
  TGeoCombiTrans  *c1 = new TGeoCombiTrans("c1", dx/10., dy/10., dz/10., r1); // mm->cm
  c1->RegisterYourself();
  TGeoTranslation *t1 = new TGeoTranslation("t1", 0., 0., 180.-shift); // cm
  t1->RegisterYourself();
  Double_t phi, theta, psi;
  c1->GetRotation()->GetAngles(phi, theta, psi);
  
  infoFile << endl << "Beam pipe section: " << pipe4name << endl;
  infoFile << "Traslation(dx,dy,dz):    "<< dx <<" "<< dy <<" "<< dz <<" mm" << endl;
  infoFile << "Rotation(phi,theta,psi): " << phi <<" "<< theta <<" "<< psi <<" deg"<< endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  // create shape components for pipe4
  TGeoPcon* p4 = MakeShape(nSects4, "p4", z4, rin4,  rout4, &infoFile);
  TGeoBBox* b  = new TGeoBBox("b", 7., 7., shift); // cm 0.13
  // create a composite for pipe4
  //*
  TGeoCompositeShape *cs1 = new TGeoCompositeShape("cs1", "p4:r1-b");
  TGeoVolume *pipe4 = new TGeoVolume("pipe4",cs1, pipeMedium);
  pipe4->SetLineColor(kGreen+2);
  pipe->AddNode(pipe4, 0, t1);
  //*/
  // create shape components for pipevac3
  TGeoPcon* p03 = MakeShape(nSects03, "p03", z03, rin03, rout03, &infoFileEmpty);
  // create a composite for pipevac3
  TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2", "p03:r1-b");
  TGeoVolume *pipevac3 = new TGeoVolume("pipevac3",cs2, vacuum);
  pipevac3->SetLineColor(kCyan);
  pipe->AddNode(pipevac3, 0, t1);
  //*
  infoFile << endl << "Beam pipe section: " << pipe5name << endl;
  infoFile << "Traslation(dx,dy,dz):    "<< dx <<" "<< dy <<" "<< dz <<" mm" << endl;
  infoFile << "Rotation(phi,theta,psi): " << phi <<" "<< theta <<" "<< psi <<" deg"<< endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe5    = MakePipe  (5, nSects5,  z5,  rin5,  rout5,  pipeMedium, &infoFile); 
  pipe5->SetLineColor(kGreen);
  pipe->AddNode(pipe5, 0, c1);
  TGeoVolume* pipevac4 = MakeVacuum(4, nSects04, z04, rin04, rout04, vacuum,     &infoFile); 
  pipevac4->SetLineColor(kCyan);
  pipe->AddNode(pipevac4, 0, c1);
  
  infoFile << endl << "Beam pipe section: " << pipe6name << endl;
  infoFile << "Traslation(dx,dy,dz):    "<< dx <<" "<< dy <<" "<< dz <<" mm" << endl;
  infoFile << "Rotation(phi,theta,psi): " << phi <<" "<< theta <<" "<< psi <<" deg"<< endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe6    = MakePipe  (6, nSects6,  z6,  rin6,  rout6,  pipeMedium, &infoFile); 
  pipe6->SetLineColor(kBlue);
  pipe->AddNode(pipe6, 0, c1);
  
  infoFile << endl << "Beam pipe section: " << pipe7name << endl;
  infoFile << "Traslation(dx,dy,dz):    "<< dx <<" "<< dy <<" "<< dz <<" mm" << endl;
  infoFile << "Rotation(phi,theta,psi): " << phi <<" "<< theta <<" "<< psi <<" deg"<< endl;
  infoFile << setw(2) << "i" << setw(10) << "Z,mm" << setw(10) << "Rin,mm" << setw(10) << "Rout,mm" << setw(10) << "h,mm" << endl;
  TGeoVolume* pipe7    = MakePipe  (7, nSects7,  z7,  rin7,  rout7,  pipeMedium, &infoFile); 
  pipe7->SetLineColor(kGreen);
  pipe->AddNode(pipe7, 0, c1);
  TGeoVolume* pipevac5 = MakeVacuum(5, nSects05, z05, rin05, rout05, vacuum,     &infoFile); 
  pipevac5->SetLineColor(kCyan);
  pipe->AddNode(pipevac5, 0, c1);
  //*/
  
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
