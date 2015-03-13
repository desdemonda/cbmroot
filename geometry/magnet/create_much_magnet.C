// This macro removes yoke screens and adds absorber inside the magnet
void create_much_magnet(){
  const char* infilename  = "magnet_v12b.geo.root";
  const char* outfilename = "magnet_v12b_much.geo.root";
  // absorber parameters - check compatibility with much geometry
  Double_t atanmin = 0.1;    // Acceptance tangent min
  Double_t atanmax = 0.5;    // Acceptance tangent max
  Double_t edge = 5;         // [cm] edge margin added at max radius (max allowed edge ~ 5 cm if screens removed, ->0 with screens)
  Double_t z1 = 125;         // [cm] Absorber z (global frame)
  Double_t lz = 34;          // [cm] Absorber thickness
  Char_t cmat = 'C';         // Absorber material - check much geometry for notations
  Bool_t remove_screens = 1; // Flag = 1 to remove yoke screens 
  
  // -------------  Load the necessary FairRoot libraries   -------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  // --------------------------------------------------------------------------

  // -------   Load media from media file and create absober material ---------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  FairGeoMedia* geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  TString smat = "MUCHcarbon";
  switch (cmat) {
    case 'C': smat ="MUCHcarbon";       break;
    case 'I': smat ="MUCHiron";         break;
    case 'L': smat ="MUCHlead";         break;
    case 'W': smat ="MUCHwolfram";      break;
    case 'P': smat ="MUCHpolyethylene"; break;
  }
  FairGeoMedium* fmat = geoMedia->getMedium(smat.Data());
  geoBuild->createMedium(fmat);
  TGeoMedium* tmat = gGeoManager->GetMedium(smat.Data());
  // --------------------------------------------------------------------------
  
  
  TFile* fin = TFile::Open(infilename);
  TGeoVolumeAssembly* geometryFromGDML = fin->Get("geometryFromGDML");
  gGeoManager->SetTopVolume(geometryFromGDML);
  //draw magnet before modifications
  //geometryFromGDML->Draw("ogl");
  //return;
  
  TGeoNode* magnet_node = geometryFromGDML->FindNode("Magnet_container_1");
  TGeoVolume* magnet_volume = magnet_node->GetVolume();
  
  if (remove_screens) { // remove yoke screens
    TGeoNode* screen1 = magnet_volume->FindNode("Yoke_screen_box-cons_22");
    TGeoNode* screen2 = magnet_volume->FindNode("Yoke_screen_box-cons_23");
    TGeoNode* screen3 = magnet_volume->FindNode("Yoke_screen_box-cyl_26");
    TGeoNode* screen4 = magnet_volume->FindNode("Yoke_screen_box-cyl_27");
    magnet_volume->RemoveNode(screen1);
    magnet_volume->RemoveNode(screen2);
    magnet_volume->RemoveNode(screen3);
    magnet_volume->RemoveNode(screen4);
  }
  
  // Calculate absorber dimensions and position
  Double_t dz = lz/2.;
  Double_t z2 = z1 + lz;
  Double_t rmin1 = z1*atanmin;
  Double_t rmin2 = z2*atanmin;
  Double_t rmax1 = z1*atanmax+edge;
  Double_t rmax2 = z2*atanmax+edge;
  Double_t magnetZ0 = magnet_node->GetMatrix()->GetTranslation()[2];
  Double_t z0 = z1+dz-magnetZ0;
  
  // Create absorber shape, volume, translation and node
  TGeoShape* absorber_shape    = new TGeoCone(dz, rmin1, rmax1, rmin2, rmax2);
  TGeoVolume* absorber_volume  = new TGeoVolume("Absorber",absorber_shape,tmat);
  TGeoTranslation* translation = new TGeoTranslation("absorber_translation",0,0,z0);
  magnet_volume->AddNodeOverlap(absorber_volume,0,translation);
  
  // Draw result
  geometryFromGDML->Draw("ogl");
  
  // Write new magnet to a file
  TFile* fout = new TFile(outfilename,"recreate");
  geometryFromGDML->Write();
  fout->Close();
}
