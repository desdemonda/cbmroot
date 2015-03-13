//
//  script to run tracking routine with LHEtrack's classes
//  Modified 21/06/2005

void lhetrack() {

  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
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
  gSystem->Load("libLHETrack");

  TString inPath = "";
  TString inFile = inPath + "auaucentr.root";
  TString bgFile = inPath + "auaucentr.root";
  TString outFile = "sts_tracks.root";
  TString version = "v05a";


  // -----   MAPS hit producer   --------------------------------------------
  Double_t sigmaX  = 0.0005;      // resolution in x
  Double_t sigmaY  = 0.0005;      // resolution in y
  Double_t eff     = 0.99;        // single hit efficiency
  Double_t fakes   = 0.03;        // fake hit rate
  Double_t dist    = 0.00;        // distance for hit merging
  Int_t    nPileUp = 1;           // number of pileup events
  
  CbmStsMapsHitProducer* mapsHitProd 
    = new CbmStsMapsHitProducer(1, 2, sigmaX, sigmaY, eff, fakes, dist,
				nPileUp, version);

  /* Since these values are the default ones, this is equivalent to
     CbmMapsHitProducer(1, 2). Note that in case you define nPileUp>1,
     you must provide the pileup file name by
     mapsHitProd->SetPileupFileName(bgFile);
  */
 //  mapsHitProd->SetPileupFileName(bgFile);
  // ------------------------------------------------------------------------


  // -----   Hybrid hit producer   ------------------------------------------
   Double_t lx = 0.005;    // Pixel x size
   Double_t ly = 0.005;    // Pixel y size
   CbmStsHybridHitProducer* hybHitProd
     = new CbmStsHybridHitProducer(3, 3, lx, ly, version);

  /* Since these parameters are the default ones, this is equivalent to
     CbmHybridHitProducer(3, 3) */
  // ------------------------------------------------------------------------

  
  // -----   Strip hit producer   -------------------------------------------
   CbmStsStripHitProducer* stripHitProd
     = new CbmStsStripHitProducer(4, 7, version);
  // ------------------------------------------------------------------------


  
  
  // -----   Run   ----------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // Init Simulation Parameters from Root File
  FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
  FairParRootFileIo* input=new FairParRootFileIo();
  input->open("parfiles/testparams.root");
  // Init Digitization Parameters from Ascii File
  FairParAsciiFileIo* input2 = new FairParAsciiFileIo();
  TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
  StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
  input2->open(StsDigiFile.Data(),"in");

  rtdb->setFirstInput(input);
  rtdb->setSecondInput(input2);


 // Add Hit Producer Tasks
  fRun->AddTask(mapsHitProd);
  fRun->AddTask(hybHitProd);
  fRun->AddTask(stripHitProd);

  CbmStsMapsStripTracker *trackMS = new CbmStsMapsStripTracker("Tracking routine");
  trackMS->SetOption("smear");
  fRun->AddTask(trackMS);

  CbmStsCMTrackFinder *trf= new CbmStsCMTrackFinder("Track finder");
  fRun->AddTask(trf);

  CbmStsPolyFitter *fit= new CbmStsPolyFitter("Fitting tracks");
  fit->SetOption("found fit");
  fRun->AddTask(fit);
  
  fRun->Init();

  fRun->Run(3);
     
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
 
}