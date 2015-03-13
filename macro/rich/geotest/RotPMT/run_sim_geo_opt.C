void run_sim_geo_opt(Int_t nEvents = 10,  float PMTrotX=2, float PMTrotY=2, int RotMir=-10, int GeoFile=1)
{
  //GeoFile=0 ==> old geometry with *.geo (RICH starts at 1800, Mirror tilt -1)
  //GeoFile=1 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does NOT cover full acceptance)
  //GeoFile=2 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does cover full acceptance)
 
  int MomCase=0; //momentum of generated electrons:
  //if(MomCase==0){boxGen1->SetPtRange(0.,4.); }
  //if(MomCase==1){boxGen1->SetPtRange(0.,3.); }
  //if(MomCase==2){boxGen1->SetPRange(0.,10.); }
  
  TTree::SetMaxTreeSize(90000000000);
  bool StoreTraj=0;
  
  char RotMirText[256];
  if(RotMir==-10){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else if(RotMir==1){sprintf( RotMirText,"RotMir_p%d",RotMir);}
  else{
    cout<<" The mirror rotation variable takes only one of two values: 1 or -10"<<endl;
    cout<<" You set it (RotMir ="<<RotMir<<"). Going to exit."<<endl;
    exit();
  }
  
  
  int ShiftXmod10=(int(PMTrotX*10)) % 10;
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;

  char ShiftXTxt[256]; char ShiftYTxt[256];  
  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}

  stringstream ss; 
  ss<<RotMirText<<"_RotPMT_"<<ShiftXTxt<<"_"<<ShiftYTxt;
  TString RotTxt=ss.str();

  // TString richGeom="rich/GeoOpt/RotPMT/GeoFiles/NewGeo/rich_geo_"+RotTxt+".gdml";
  // if(GeoFile==0){richGeom="rich/GeoOpt/RotPMT/GeoFiles/OldGeo/rich_geo_"+RotTxt+".gdml";}
  TString richGeom=GetRICH_GeoFile( PMTrotX,PMTrotY, RotMir, GeoFile);
  if(GeoFile==0){richGeom="rich/rich_v08a.geo";}//this is just for testing the code. thereafter the geofile should be taken from the function GetRICH_GeoFile()
  
  cout<<"geo: "<<richGeom<<endl;
  //return;
  TString outDir="/data/GeoOpt/RotPMT/NewGeo/";
  if(GeoFile==0){outDir="/data/GeoOpt/RotPMT/VeryOldGeo/";}
  if(GeoFile==1){outDir="/data/GeoOpt/RotPMT/OldGeo/";}
  if(GeoFile==2){outDir="/data/GeoOpt/RotPMT/NewGeo/";}

  TString ParFile = outDir + "Parameters_RichGeo_"+RotTxt+"_Pt0to4.root";
  TString SimFile = outDir + "Sim_RichGeo_"+RotTxt+"_Pt0to4.root";
  TString OutPutGeoFile = outDir + "OutPutGeo_RichGeo_"+RotTxt+"_Pt0to4.root";
  if(MomCase==1){
    ParFile = outDir + "Parameters_RichGeo_"+RotTxt+"_Pt0to3.root";
    SimFile = outDir + "Sim_RichGeo_"+RotTxt+"_Pt0to3.root";
    OutPutGeoFile = outDir + "OutPutGeo_RichGeo_"+RotTxt+"_Pt0to3.root";
  }
  if(MomCase==2){
    ParFile = outDir + "Parameters_RichGeo_"+RotTxt+"_P0to10.root";
    SimFile = outDir + "Sim_RichGeo_"+RotTxt+"_P0to10.root";
    OutPutGeoFile = outDir + "OutPutGeo_RichGeo_"+RotTxt+"_P0to10.root";
  }
  cout<<"par: "<<ParFile<<endl;
  cout<<"sim: "<<SimFile<<endl;
  cout<<"++++++++++++++++++++++++++++++++++++++++++++"<<endl; 

  //return;
  TString pipeGeom   = "pipe/pipe_standard.geo";
  pipeGeom   = "pipe/pipe_standard_Shift20cm.geo"; //new geometry always shifted.
  TString caveGeom = "cave.geo";
  //TString targetGeom = "target/target_au_250mu.geo";
  //TString magnetGeom = "magnet/magnet_v12a.geo";
  TString magnetGeom = "magnet/magnet_v12b.geo.root";
  TString stsGeom = "";
  //TString stsGeom = "sts/sts_v12b.geo.root";
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
    rich->SetRegisterPhotons(kTRUE); // Cerenkov photons are also registered in the sim tree
    fRun->AddModule(rich);
  }
  
  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  
  // e+/-
  float StartPhi=90.1, EndPhi=180.;
  float StartTheta=2.5, EndTheta=25.;
  FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 1);
  if(MomCase==0){boxGen1->SetPtRange(0.,4.); }
  if(MomCase==1){boxGen1->SetPtRange(0.,3.); }
  if(MomCase==2){boxGen1->SetPRange(0.,10.); }
  // boxGen1->SetPRange(0.,10.);
  //boxGen1->SetPtRange(0.,4.);
  boxGen1->SetPhiRange(StartPhi,EndPhi);//0.,360.);
  boxGen1->SetThetaRange(StartTheta,EndTheta);//2.5,25.);
  boxGen1->SetCosTheta();
  boxGen1->Init();
  primGen->AddGenerator(boxGen1);
  
  FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, 1);
  if(MomCase==0){boxGen2->SetPtRange(0.,4.); }
  if(MomCase==1){boxGen2->SetPtRange(0.,3.); }
  if(MomCase==2){boxGen2->SetPRange(0.,10.); }
  // //boxGen2->SetPtRange(0.,4.);
  //  boxGen2->SetPRange(0.,10.);
  boxGen2->SetPhiRange(StartPhi,EndPhi);//0.,360.);
  boxGen2->SetThetaRange(StartTheta,EndTheta);//2.5,25.);
  boxGen2->SetCosTheta();
  boxGen2->Init();
  primGen->AddGenerator(boxGen2);
  
  
  fRun->SetGenerator(primGen);
  if(StoreTraj){fRun->SetStoreTraj(kTRUE);}
  fRun->Init();
  
  if(StoreTraj){
    FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    trajFilter->SetStepSizeCut(0.01); // 1 cm
    trajFilter->SetVertexCut(-2000., -2000., -2000., 2000., 2000., 2000.);
    trajFilter->SetMomentumCutP(0.); // p_lab > 0
    trajFilter->SetEnergyCut(0., 10.); // 0 < Etot < 10 GeV
    trajFilter->SetStorePrimaries(kTRUE);//kFALSE);//kTRUE);
  }
  
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
  //fRun->CreateGeometryFile(OutPutGeoFile);
  
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

TString GetRICH_GeoFile( float PMTrotX=2, float PMTrotY=2, int RotMir=-10, int GeoFile=1){
  //GeoFile=0 ==> old geometry with *.geo (RICH starts at 1600, Mirror tilt -1)
  //GeoFile=1 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does NOT cover full acceptance)
  //GeoFile=2 ==> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10, 
  //                        mirror does cover full acceptance)
  TString Dir="/data/cbmroot/geometry/rich/GeoOpt/RotPMT/GeoFiles/";
  TString Dir2="NewGeo/";
  TString Endung=".gdml";
  int ShiftXmod10=(int(PMTrotX*10)) % 10;  
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;
  char ShiftXTxt[256]; char ShiftYTxt[256];
  
  TString RotMirText="RotMir_p1";
  if(RotMir==-10){RotMirText="RotMir_m10";}
  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}

  if(GeoFile==0){Dir2="VeryOldGeo/"; Endung=".geo";}
  if(GeoFile==1){Dir2="OldGeo/";}
  if(GeoFile==2){Dir2="NewGeo/";}
  stringstream ss; 
  ss<<Dir<<Dir2<<"rich_geo_"<<RotMirText<<"_RotPMT_"<<ShiftXTxt<<"_"<<ShiftYTxt<<Endung;
  
  TString RotTxt=ss.str();
  return RotTxt;
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
