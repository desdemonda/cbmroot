

void eventDisplay(int PMTrotX=10, int PMTrotY=15, int RotMir=1)
{
  int GeoCase=2;
  //int PtNotP=0;  float MomMin=3.95; float MomMax=4.;
  int PtNotP=1;  float MomMin=0.; float MomMax=4.;
  
  
  TString RotMirText=GetMirText(RotMir);
  TString PMTRotText=GetPMTRotText(PMTrotX, PMTrotY);
  TString outDir=GetOutDir(GeoCase);//="/data/GeoOpt/RotPMT/NewGeo/";
  TString GeoText=GetGeoText(GeoCase);
  TString MomText=GetMomText(PtNotP,MomMin,MomMax);
  TString ParFile = outDir + "Parameters_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+"_display.root";
  TString SimFile = outDir + "Sim_"+GeoText+"_"+RotMirText+"_"+PMTRotText+"_"+MomText+"_display.root";
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter


  FairRunAna *fRun= new FairRunAna();


  fRun->SetInputFile(SimFile);
  fRun->SetOutputFile("test.root");


  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);


  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *RichPoint =   new FairMCPointDraw ("RichPoint",kOrange,  kFullSquare);
  fMan->AddTask(Track);
  fMan->AddTask(RichPoint);


  fMan->Init();
}

////////////////////////////////////////////
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
  //return "/hera/cbm/users/tariq/GeoOptRootFiles/RotPMT/";
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



/*
float A=88.0, B=65.0;// rotx 5 roty 5 ==>N=664.568
A=87.0, B=52.0;// rotx 20 roty 90 ==>N=525.613
A=81.0, B=50.0;// rotx 41 roty 11 ==>N=470.542
float Plane=TMath::Pi()*A*B
float PMT=5.2*5.2
float Number=Plane/PMT
cout<<Number<<endl;
*/

/*
  (0,0):  x: -101.05 -- -0.98   y: 82.75 -- 143.58
  (0,5):  x: -100.83 -- -1.09   y: 82.75 -- 143.58
  (0,10): x: -100.33 -- -1.69    y: 
  (0,15): x: -97.83  -- -2.69    y: 
  (0,20): x: -85.44  -- -3.99    y: 
  (0,25): x: -77.99  -- -5.59    y: 
  (0,30): x: -72.75  -- -7.59    y: 
  (0,35): x: -68.95  -- -9.99    y: 
  (0,40): x: -65.87  -- -12.59   y: 
  (0,45): x: -63.55  -- -15.59   y: 
  (0,50): x: -61.75  -- -18.79   y: 82.75 -- 143.58

  *************************************************
  *************************************************
  (0,0):  y: 82.75 -- 143.58     x: -101.05 --  -0.98
  (5,0):  y: 82.68 -- 143.52
  (10,0): y: 82.75 -- 143.49
  (15,0): y: 83.75 -- 142.58
  (20,0): y: 84.75 -- 141.58
  (25,0): y: 85.75 -- 140.57
  (30,0): y: 90.73 -- 139.59
  (35,0): y: 94.72 -- 138.59
  (40,0): y: 97.71 -- 136.59
  (45,0): y: 99.71 -- 134.60
  (50,0): y: 101.7 -- 132.60
  *********************************************************

*/
