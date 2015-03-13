void perf() {

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("/$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libBase");
  gSystem->Load("libSts");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libTof");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  //gSystem->Load("libAnalysis");

  char *inFile  = "/d/cbm02/ikisel/data/default/sim_1000_0.root";
  char *parFile = "/d/cbm02/ikisel/data/default/sim_1000_0.params.root";
  char *recFile = "/d/cbm02/ikisel/data/default/rec_1000_0.root";
  char *outFile = "/d/cbm02/ikisel/data/default/perf_1000_0.root";
  //char *outFile = "out.root";

  // -----   Run   ----------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(recFile);

  fRun->SetOutputFile(outFile);

  fRun->SetContainerStatic();

  // Init Simulation Parameters from Root File
  FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
  FairParRootFileIo* input=new FairParRootFileIo();
  input->open(parFile);

  // Init Digitization Parameters from Ascii File
  FairParAsciiFileIo* input2 = new FairParAsciiFileIo();
  TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
  StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
  input2->open(StsDigiFile.Data(),"in");

  rtdb->setFirstInput(input);
  rtdb->setSecondInput(input2);

  cout<<" LoadGeometry starting..."<<endl;
  fRun->LoadGeometry();
  cout<<"LoadGeometry ok"<<endl;
  cout << " - I TGeoManager " << gGeoManager << endl;
 
  // Kalman Filter

  CbmKF *KF = new CbmKF();
  fRun->AddTask(KF);

  //****************** track fitter *************

  //CbmStsKFTrackFitter* kalman = new CbmStsKFTrackFitter;
  //CbmStsFitTracks *fitTask = new CbmStsFitTracks("Kalman fitter","FairTask",kalman);
  //fRun->AddTask(fitTask);

  //****************** vertex finder *************

  //CbmPVFinderKF *vfinder = new CbmPVFinderKF;
  //CbmFindPrimaryVertex *vTask = new CbmFindPrimaryVertex("Kalman PV finder", "FairTask", vfinder);
  //fRun->AddTask(vTask);

  //CbmD0Analysis *D0 = new CbmD0Analysis;
  //fRun->AddTask(D0);

  //***************** performance fitter ********
  
  CbmStsFitPerformanceTask *fitperf = new CbmStsFitPerformanceTask("FitPerformance");
  fitperf->DoTrackAnalysis();
  fitperf->DoVertexAnalysis();
  fitperf->DoD0Analysis();
  fRun->AddTask(fitperf);
  
  //******************

  cout << " INITIALISATION " << endl;

  fRun->Init();
  fRun->Run();//0,10);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
