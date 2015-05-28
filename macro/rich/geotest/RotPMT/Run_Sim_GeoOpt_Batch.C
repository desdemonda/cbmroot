void Run_Sim_GeoOpt_Batch(Int_t nEvents = 10,  float PMTrotX=2, float PMTrotY=2, int RotMir=-10)
{

  int GeoCase=2;
  int PtNotP=1;  float MomMin=0.; float MomMax=4.;
  //int PtNotP=0;  float MomMin=3.95; float MomMax=4.;

  TString script = TString(gSystem->Getenv("SCRIPT"));
  if (script == "yes"){
    cout<<" ----------------- running with script --------------------"<<endl;
    nEvents=TString(gSystem->Getenv("N_EVS")).Atof();
    RotMir=TString(gSystem->Getenv("ROTMIR")).Atof();
    PMTrotX=TString(gSystem->Getenv("PMT_ROTX")).Atof();
    PMTrotY=TString(gSystem->Getenv("PMT_ROTY")).Atof();
    
    GeoCase=TString(gSystem->Getenv("GEO_CASE")).Atof();
    
    PtNotP=TString(gSystem->Getenv("PT_NOT_P")).Atof();
    MomMin=TString(gSystem->Getenv("MOM_MIN")).Atof();
    MomMax=TString(gSystem->Getenv("MOM_MAX")).Atof();

  }  
  cout<<RotMir<<", "<<PMTrotX<<", "<<PMTrotY<<", "<<GeoCase<<endl;
  
  TTree::SetMaxTreeSize(90000000000);
  //******************************
  TString RotMirText=GetMirText(RotMir);
  TString PMTRotText=GetPMTRotText(PMTrotX, PMTrotY);
  TString richGeom=GetRICH_GeoFile( RotMirText, PMTRotText, GeoCase);
  TString pipeGeom=GetPipe_GeoFile( GeoCase);
  pipeGeom="";
  //******************************

  cout<<"RotMirText = "<<RotMirText<<endl;
  cout<<"PMTRotText = "<<PMTRotText<<endl;
  cout<<"rich geo = "<<richGeom<<endl;

  TString outDir=GetOutDir(GeoCase);//="/data/GeoOpt/RotPMT/NewGeo/";
  TString GeoText=GetGeoText(GeoCase);
  TString MomText=GetMomText(PtNotP,MomMin,MomMax);
  TString ExtraText="_RegularTheta.";
  ExtraText=".";

  TString ParFile = outDir + "Parameters_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";
  TString SimFile = outDir + "Sim_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";
  TString OutPutGeoFile = outDir + "OutPutGeo_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";
  gSystem->Exec( ("rm "+ParFile).Data() );
  gSystem->Exec( ("rm "+SimFile).Data() );
  gSystem->Exec( ("rm "+OutPutGeoFile).Data() );

  cout<<"par: "<<ParFile<<endl;
  cout<<"sim: "<<SimFile<<endl;
  cout<<"++++++++++++++++++++++++++++++++++++++++++++"<<endl; 
  //return;
  TString caveGeom = "cave.geo";
  TString magnetGeom = "magnet/magnet_v12b.geo.root";
  //TString magnetGeom = "";
  TString stsGeom = "";
  TString fieldMap = "field_v12a";
  Double_t fieldZ = 50.; // field center z position
  Double_t fieldScale =  1.0; // field scaling factor
  
  ///////////////////////////////////////////////////
  gDebug = 0;
  TStopwatch timer;
  timer.Start();
  cout<<" going to load libraries"<<endl;
  
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
  loadlibs();
  cout<<" got libraries"<<endl;
  
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3"); // Transport engine
  fRun->SetOutputFile(SimFile);
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  
  fRun->SetMaterials("media.geo"); // Materials
  
  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
  }
  
  if ( pipeGeom != "") {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
  }
  
  if ( magnetGeom != "") {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
  }
  
  if ( stsGeom != "") {
    FairDetector* sts = new CbmStsMC(kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
  }
  
  if ( richGeom != "") {
    //FairDetector* rich = new CbmRich("RICH", kTRUE);
    CbmRich* rich = new CbmRich("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    rich->SetRegisterPhotonsOnSensitivePlane(kTRUE); // Cerenkov photons are also registered in the sim tree
    fRun->AddModule(rich);
  }
  
  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  
  // e+/-
  float StartPhi=90.1, EndPhi=180.;
  float StartTheta=2.5, EndTheta=35.;
  FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 1);
  if(PtNotP==1){boxGen1->SetPtRange(MomMin,MomMax); }
  else{boxGen1->SetPRange(MomMin,MomMax); }
  // boxGen1->SetPRange(0.,10.);
  //boxGen1->SetPtRange(0.,4.);
  boxGen1->SetPhiRange(StartPhi,EndPhi);//0.,360.);
  boxGen1->SetThetaRange(StartTheta,EndTheta);//2.5,25.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);
  
  FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, 1);
  if(PtNotP==1){boxGen2->SetPtRange(MomMin,MomMax); }
  else{boxGen2->SetPRange(MomMin,MomMax); }
  // boxGen2->SetPtRange(0.,4.);
  // boxGen2->SetPRange(0.,10.);
  boxGen2->SetPhiRange(StartPhi,EndPhi);//0.,360.);
  boxGen2->SetThetaRange(StartTheta,EndTheta);//2.5,25.);
  boxGen2->SetCosTheta();
  boxGen2->Init();
  primGen->AddGenerator(boxGen2);
  
  
  fRun->SetGenerator(primGen);
  fRun->Init();
    
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(ParFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  
  fRun->Run(nEvents);
  fRun->CreateGeometryFile(OutPutGeoFile);
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << SimFile << endl;
  cout << "Parameter file is " << ParFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
////////////////////////////////////////////
TString GetMomText(int PtNotP, float MomMin, float MomMax){
  TString Pstring="P"; if(PtNotP==1){Pstring="Pt";}
  if( (MomMax-MomMin)<0.1){}
  char Ptxt[256];
  
  if( (MomMax-MomMin)<0.1){sprintf(Ptxt,"%sFixed%d",Pstring.Data(),MomMax);}
  else{sprintf(Ptxt,"%s%dto%d",Pstring.Data(),MomMin,MomMax);}
  stringstream ss; 
  ss<<Ptxt;
  return ss.str();
}

////////////////////////////////////////////
TString GetGeoText(int GeoCase){
  //GeoCase=-2 ==> old geometry with rich_v08a.geo (RICH starts at 1600, Mirror tilt -1)
  //GeoCase=-1 ==> old geometry with rich_v14a.gdml (RICH starts at 1800, Mirror tilt -1)
  //GeoCase=0 ==> old geometry with *.geo (own creation)(RICH starts at 1600, Mirror tilt -1)
  //GeoCase=1 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does NOT cover full acceptance)
  //GeoCase=2 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does cover full acceptance)

 if(GeoCase==-2){return "RichGeo_v08a";}
  if(GeoCase==-1){return "RichGeo_v14a";}
  if(GeoCase==0){return "RichGeo_ascii";}
  if(GeoCase==1){return "RichGeo_OldGdml";}
  if(GeoCase==2){return "RichGeo_NewGdml";}
}
////////////////////////////////////////////
TString GetOutDir(int GeoCase){
  return "/data/GeoOpt/RotPMT/";

  return "/hera/cbm/users/tariq/GeoOptRootFiles/RotPMT/";
  // if(GeoCase<=0){return "/data/GeoOpt/RotPMT/OlderGeo/";}
  // if(GeoCase==1){return "/data/GeoOpt/RotPMT/OldGeo/";}
  // if(GeoCase==2){return "/data/GeoOpt/RotPMT/NewGeo/";}
}
////////////////////////////////////////////
TString GetMirText(int RotMir){
  char RotMir_txt[256];
  if(RotMir<0){sprintf( RotMir_txt,"RotMir_m%d",RotMir*-1);}
  else{sprintf(RotMir_txt,"RotMir_p%d",RotMir);}

  stringstream ss; 
  ss<<RotMir_txt;
  return ss.str();
}
////////////////////////////////////////////////////////
TString  GetPMTRotText(float PMTrotX, float PMTrotY){
  int ShiftXmod10=(int(PMTrotX*10)) % 10;
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;
  
  char ShiftXTxt[256]; char ShiftYTxt[256];  
  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}
  
  stringstream ss; 
  ss<<"RotPMT_"<<ShiftXTxt<<"_"<<ShiftYTxt;
  return ss.str();
}

////////////////////////////////////////////////////////
TString GetRICH_GeoFile( char *RotMirText, TString  PMTRotText, int GeoCase){
  //GeoCase=-2 ==> old geometry with rich_v08a.geo (RICH starts at 1600, Mirror tilt -1)
  //GeoCase=-1 ==> old geometry with rich_v14a.gdml (RICH starts at 1800, Mirror tilt -1)
  if(GeoCase==-2){return "rich/rich_v08a.geo";}
  if(GeoCase==-1){return "rich/rich_v14a.root";}
  //GeoCase=0 ==> old geometry with *.geo (own creation)(RICH starts at 1600, Mirror tilt -1)
  //GeoCase=1 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does NOT cover full acceptance)
  //GeoCase=2 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does cover full acceptance)
  
  TString Dir="rich/GeoOpt/RotPMT/";
  
  TString Dir2="NewGeo/";
  TString Endung=".root";
  if(GeoCase==0){Dir2="OldGeo/"; Endung=".geo";}
  if(GeoCase==1){Dir2="OldGeo/";}
  if(GeoCase==2){Dir2="NewGeo/";}
  stringstream ss; 
  ss<<Dir<<Dir2<<"rich_geo_"<<RotMirText<<"_"<<PMTRotText<<Endung;
  
  return ss.str();
}
////////////////////////////////////////////////////////
TString GetPipe_GeoFile(int GeoCase){
  if(GeoCase == -2 || GeoCase == 0){return "pipe/pipe_standard.geo";}
  else{return "pipe/pipe_v14h.root";}
}

/*
  with old geo files (rotx=5, and roty=5) Notice that the actual rotx is 6.2 !!
  
PMT position in (x,y,z) [cm]: 51  172.525  225.928
Tilted PMT position in (x,y,z) [cm]: 31.115  196.327  209.516
PMT size in x and y [cm]: 50  30
PMT was tilted around x by 6.20664 degrees
PMT was tilted around y by -5 degrees
Refractive index for lowest photon energies: 1.00045, (n-1)*10000: 4.46242
Mirror center (x,y,z): 0 140.394 54.0827
Mirror radius: 300
Mirror tilting angle: -10 degrees
*/
