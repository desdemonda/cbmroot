void sts_reco(  Int_t nEvents = 1,  Int_t iVerbose = 0){
  TString inFile  = "data/mc_sector.root";
  TString outFile = "data/sts_sector.root";
  TString parFile = "data/params.root";
  TString stsFile = "sts_v11a.digi.par";
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parInput1 = new FairParRootFileIo();
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput1->open(parFile.Data());
  parInput2->open(Form("%s/parameters/sts/%s",gSystem->Getenv("VMCWORKDIR"),stsFile.Data()),"in");
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parInput2);

  Double_t threshold  =  4;
  Double_t noiseWidth =  0.01;
  Int_t    nofBits    = 20;
  Double_t minStep    =  0.01;
  Double_t StripDeadTime = 0.1;
  CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
  stsDigitize->SetRealisticResponse();
  stsDigitize->SetFrontThreshold (threshold);
  stsDigitize->SetBackThreshold  (threshold);
  stsDigitize->SetFrontNoiseWidth(noiseWidth);
  stsDigitize->SetBackNoiseWidth (noiseWidth);
  stsDigitize->SetFrontNofBits   (nofBits);
  stsDigitize->SetBackNofBits    (nofBits);
  stsDigitize->SetFrontMinStep   (minStep);
  stsDigitize->SetBackMinStep    (minStep);
  stsDigitize->SetStripDeadTime  (StripDeadTime);
  fRun->AddTask(stsDigitize);

  FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",iVerbose);
  fRun->AddTask(stsClusterFinder);

  FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
  fRun->AddTask(stsFindHits);

  FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
  fRun->AddTask(stsMatchHits);

  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  fRun->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  fRun->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder    = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);  
  fRun->AddTask(stsFindTracks);

  CbmStsMatchTracks* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
  fRun->AddTask(stsMatchTracks);

  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
  FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
  fRun->AddTask(stsFitTracks);

  CbmPrimaryVertexFinder* pvFinder     = new CbmPVFinderKF();
  CbmFindPrimaryVertex* pvFindTask = new CbmFindPrimaryVertex(pvFinder);
  fRun->AddTask(pvFindTask);

  fRun->Init();
  fRun->Run(0,nEvents);
}
