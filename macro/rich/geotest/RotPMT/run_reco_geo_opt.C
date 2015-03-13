void run_reco_geo_opt(Int_t nEvents = 10,  float PMTrotX=2, float PMTrotY=2, int RotMir=-10, int GeoFile=1)
{
 
  int MomCase=0;//momentum of generated electrons:
  //if(MomCase==0){boxGen1->SetPtRange(0.,4.); }
  //if(MomCase==1){boxGen1->SetPtRange(0.,3.); }
  //if(MomCase==2){boxGen1->SetPRange(0.,10.); }
  TTree::SetMaxTreeSize(90000000000);
  gRandom->SetSeed(10);
  char RotMirText[256];
  if(RotMir<0){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( RotMirText,"RotMir_p%d",RotMir);}
  
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
  TString outDir="/data/GeoOpt/RotPMT/NewGeo/";
  if(GeoFile==0){outDir="/data/GeoOpt/RotPMT/VeryOldGeo/";}
  if(GeoFile==1){outDir="/data/GeoOpt/RotPMT/OldGeo/";}
  if(GeoFile==2){outDir="/data/GeoOpt/RotPMT/NewGeo/";}
  
  TString ParFile = outDir + "Parameters_RichGeo_"+RotTxt+"_Pt0to4.root";
  TString SimFile = outDir + "Sim_RichGeo_"+RotTxt+"_Pt0to4.root";
  TString RecFile = outDir + "Rec_RichGeo_"+RotTxt+"_Pt0to4.root";
  if(MomCase==1){
    ParFile = outDir + "Parameters_RichGeo_"+RotTxt+"_Pt0to3.root";
    SimFile = outDir + "Sim_RichGeo_"+RotTxt+"_Pt0to3.root";
    RecFile = outDir + "Rec_RichGeo_"+RotTxt+"_Pt0to3.root";
  }
  if(MomCase==2){
    ParFile = outDir + "Parameters_RichGeo_"+RotTxt+"_P0to10.root";
    SimFile = outDir + "Sim_RichGeo_"+RotTxt+"_P0to10.root";
    RecFile = outDir + "Rec_RichGeo_"+RotTxt+"_P0to10.root";
  }
  
  cout<<ParFile<<endl;
  cout<<SimFile<<endl;  
  cout<<RecFile<<endl;  
  
  gDebug = 0;
  TStopwatch timer;
  timer.Start();
  cout<<" going to load libraries"<<endl;
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
  loadlibs();
  cout<<" got libraries"<<endl;
  
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(SimFile);
  run->SetOutputFile(RecFile);
  
  CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
  richHitProd->SetDetectorType(6);
  richHitProd->SetNofNoiseHits(220);
  richHitProd->SetCollectionEfficiency(1.0);
  richHitProd->SetSigmaMirror(0.06);
  richHitProd->SetCrossTalkHitProb(0.0);
  run->AddTask(richHitProd);
  
  CbmRichReconstruction* richReco = new CbmRichReconstruction();
  richReco->SetRunExtrapolation(false);
  richReco->SetRunProjection(false);
  richReco->SetRunTrackAssign(false);
  richReco->SetFinderName("ideal");
  // richReco->SetExtrapolationName("ideal");
  run->AddTask(richReco);
  
  CbmRichMatchRings* matchRings = new CbmRichMatchRings();
  run->AddTask(matchRings);
  
  //Tariq's routine for geometry optimization
  CbmRichGeoOpt* richGeoOpt = new CbmRichGeoOpt();
  run->AddTask(richGeoOpt);
  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(ParFile.Data());
  //parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  //rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  
  
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0,nEvents);
  
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << RecFile << endl;
  cout << "Parameter file is " << ParFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
