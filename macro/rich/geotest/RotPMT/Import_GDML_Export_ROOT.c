void Import_GDML_Export_ROOT(float PMTrotX=5, float PMTrotY=5, int RotMir=-10)
{
  char RotMirText[256];
  if(RotMir<0){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( RotMirText,"RotMir_p%d",RotMir);}

  int ShiftXmod10=(int(PMTrotX*10)) % 10;  
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;

  char ShiftXTxt[256]; char ShiftYTxt[256];
  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}
  
  char GeoFileName[256];
  sprintf(GeoFileName,"/geometry/rich/GeoOpt/RotPMT/NewGeo/rich_geo_%s_RotPMT_%s_%s",RotMirText,ShiftXTxt,ShiftYTxt);


  stringstream ss; 
  ss<<GeoFileName;
  TString FileName=ss.str();

  TString InFile=FileName+".gdml";
  cout<<" ########## "<<InFile<<endl;
  TString OutFile=FileName+".root";
  cout<<" ########## "<<OutFile<<endl;
  TString vmcDir = TString(gSystem->Getenv("VMCWORKDIR"));
  
  TGeoManager *gdml = new TGeoManager("gdml", "FAIRGeom");
  
  TGDMLParse parser;
  // Define your input GDML file HERE
  TGeoVolume* gdmlTop = parser.GDMLReadFile (vmcDir + InFile);
  TGeoVolume* rootTop = new TGeoVolumeAssembly("TOP");
  
  gGeoManager->SetTopVolume(rootTop);
  
  // Define your position HERE
  TGeoRotation* rot = new TGeoRotation ("rot", 0., 0., 0.);
  TGeoCombiTrans* posrot = new TGeoCombiTrans (0., 0., 270., rot);
  
  rootTop->AddNode (gdmlTop, 1, posrot);
  
  gGeoManager->CloseGeometry();
  
  // Define you output ROOT file HERE
  TFile* outfile = new TFile(vmcDir + OutFile, "RECREATE");
  rootTop->Write();
  outfile->Close();
}

