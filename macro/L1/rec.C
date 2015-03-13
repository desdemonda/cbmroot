void rec() {

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

  char *inFile  = "/d/cbm02/ikisel/data/default/sim_1000_0.root";
  char *parFile = "/d/cbm02/ikisel/data/default/sim_1000_0.params.root";
  char *bgFile  = "/d/cbm02/ikisel/data/default/sim_1000_0.root";
  char *outFile = "/d/cbm02/ikisel/data/default/rec_1000_0.root";
  //char *outFile = "out.root";
  char *version = "v05a";

  // -----   Run   ----------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
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


  CbmStsMapsHitProducer* mapsHitProd= new CbmStsMapsHitProducer(1, 2);
  mapsHitProd->SetPileupFileName(bgFile);  
  CbmStsHybridHitProducer* hybHitProd = new CbmStsHybridHitProducer(3, 3);
  CbmStsStripHitProducer* stripHitProd = new CbmStsStripHitProducer(4, 7);

  fRun->AddTask(mapsHitProd);
  fRun->AddTask(hybHitProd);
  fRun->AddTask(stripHitProd);
  
  // Kalman Filter

  CbmKF *KF = new CbmKF();
  fRun->AddTask(KF);

  // L1

  CbmL1 *L1 = new CbmL1();
  fRun->AddTask(L1);

  //******************* track finder ***************
 
  //CbmStsTrackFinderIdeal* trackFinder = new CbmStsTrackFinderIdeal();
  CbmL1StsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
  
  CbmStsFindTracks* findTask = new CbmStsFindTracks("finderTask","FairTask",trackFinder);
  fRun->AddTask(findTask);

  //****************** track fitter *************

  CbmStsKFTrackFitter* kalman = new CbmStsKFTrackFitter;
  CbmStsFitTracks *fitTask = new CbmStsFitTracks("Kalman fitter","FairTask",kalman);
  fRun->AddTask(fitTask);

  //****************** vertex finder *************

  CbmPVFinderKF *vfinder = new CbmPVFinderKF;
  CbmFindPrimaryVertex *vTask = new CbmFindPrimaryVertex("Kalman PV finder", "FairTask", vfinder);
  fRun->AddTask(vTask);

  //***************** Sts<->MC track match *********

  CbmStsMatchTracks *match = new CbmStsMatchTracks();
  fRun->AddTask(match);

  //***************** performance finder *********

  //CbmStsFinderPerformanceTask *findperf = new CbmStsFinderPerformanceTask("FinderPerformance");  //findperf->SetNEventsCalculateRmin(1000);
  //fRun->AddTask(findperf);

  //***************** performance fitter ********
  
  CbmStsFitPerformanceTask *fitperf = new CbmStsFitPerformanceTask("FitPerformance");
  fitperf->DoTrackAnalysis();
  fitperf->DoVertexAnalysis(1);
  fitperf->DoD0Analysis(0);

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
