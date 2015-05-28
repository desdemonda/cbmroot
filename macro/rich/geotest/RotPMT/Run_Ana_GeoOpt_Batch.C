void Run_Ana_GeoOpt_Batch(Int_t nEvents = 10,  float PMTrotX=2, float PMTrotY=2, int RotMir=-10)
{
 
  TTree::SetMaxTreeSize(90000000000);
  gRandom->SetSeed(10);

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

  TString RotMirText=GetMirText(RotMir);
  TString PMTRotText=GetPMTRotText(PMTrotX, PMTrotY);
  TString outDir=GetOutDir(GeoCase);//="/data/GeoOpt/RotPMT/NewGeo/";
  TString GeoText=GetGeoText(GeoCase);
  TString MomText=GetMomText(PtNotP,MomMin,MomMax);
  TString ExtraText="_RegularTheta.";
  ExtraText=".";

  TString ParFile = outDir + "Parameters_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";
  TString SimFile = outDir + "Sim_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";
  TString RecFile = outDir + "Rec_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";

  TString AnaFile = outDir + "Ana_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";

  TString RecFile = outDir + "Rec_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";

  TString AnaFile = outDir + "Ana_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+ExtraText+"root";

  gDebug = 0;
  TStopwatch timer;
  timer.Start();
  cout<<" going to load libraries"<<endl;
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
  loadlibs();
  cout<<" got libraries"<<endl;
  
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(SimFile);
  run->AddFriend(RecFile);
  run->SetOutputFile(AnaFile);
  
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
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
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
  //return "/data/GeoOpt/RotPMT/";
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

