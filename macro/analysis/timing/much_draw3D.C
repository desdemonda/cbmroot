void much_draw3D(char* mcfile = "data5/geofile.root"){

//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//   basiclibs();
//
//   gSystem->Load("libGeoBase");
//   gSystem->Load("libParBase");
//   gSystem->Load("libBase");
//   gSystem->Load("libCbmBase");
//   gSystem->Load("libCbmData");
//   gSystem->Load("libField");
//   gSystem->Load("libGen");
//   gSystem->Load("libPassive");
//   gSystem->Load("libSts");
//   gSystem->Load("libMuch");
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
   muchlibs();

   TFile* f = new TFile(mcfile);
   f->Get("FairBaseContFact");
   TGeoManager* gGeoManager = (TGeoManager*) f->Get("FAIRGeom");
   gGeoManager->SetVisLevel(0);
//
   // Check overlaps
//   gGeoManager->CheckOverlaps(0.0000001);
//   gGeoManager->PrintOverlaps();

   TGeoVolume* master = gGeoManager->GetMasterVolume();

   // Draw all
   //master->Draw("ogl");

   // Draw much
   TGeoVolume* much = master->FindNode("much_0")->GetVolume();
//   
//   for (Int_t i=0;i<5;i++){
//   TGeoVolume* abs1 = much->FindNode(For"muchabsorber01_0")->GetVolume();
//   printf("Color: %i %i\n",abs1->GetLineColor(), kRed);
//   abs1->SetLineColor(kRed);
//   abs1->Draw("ogl");
   much->Draw("ogl");
//   
//   TGeoVolume* mantle = master->FindNode("mantle_0")->GetVolume();
//   mantle->SetLineColor(kRed);
//   mantle->Draw("ogl");

}
