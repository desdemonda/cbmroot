
void reco_urqmd(Int_t index)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  Int_t nEvents = 1000;
  
  
  
  // Load libraries -----------------------------------------------
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
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libLittrack");
  // --------------------------------------------------------------
  
  

  Int_t iVerbose = 0;
  TList *parFileList = new TList();
  
  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";
  
  TObjString stsDigiFile = paramDir + "/sts/sts_v11a.digi.par";
  parFileList->Add(&stsDigiFile);
  
  TObjString trdDigiFile =  paramDir + "/trd/trd_v11c.digi.par";
  parFileList->Add(&trdDigiFile);
  

  
  // File names ---------------------------------------------------
  char strInputFile[1000];
  char strParamFile[1000];
  char strOutputFile[1000];
  
  char *ver = getenv("CBMVER");
  
  sprintf(strInputFile, "/home/kresan/data/mc/%s/urqmd/auau/25gev/centr", ver);
  sprintf(strOutputFile, "/home/kresan/data/reco/%s/urqmd/auau/25gev/centr", ver);
  
  sprintf(strParamFile, "%s/urqmd.auau.25gev.centr.%4d.mc.param.root",
	  strInputFile, index);
  sprintf(strInputFile, "%s/urqmd.auau.25gev.centr.%4d.mc.root",
	  strInputFile, index);
  sprintf(strOutputFile, "%s/urqmd.auau.25gev.centr.%4d.reco.root",
	  strOutputFile, index);
  
  for(Int_t i = 0; i < 1000; i++) {
    if(' ' == strInputFile[i]) strInputFile[i] = '0';
    if(' ' == strParamFile[i]) strParamFile[i] = '0';
    if(' ' == strOutputFile[i]) strOutputFile[i] = '0';
  }
  // --------------------------------------------------------------
  
  
  
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(strInputFile);
  run->SetOutputFile(strOutputFile);
  

  
  // =========================================================================
  // ===             Detector Response Simulation (Digitiser)              ===
  // ===                          (where available)                        ===
  // =========================================================================
  
  // -----   MVD Digitiser   -------------------------------------------------
  CbmMvdDigitizeL* mvdDigi =
    new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
  run->AddTask(mvdDigi);
  // -------------------------------------------------------------------------
  
  // -----   STS digitizer   -------------------------------------------------
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.01;
  Int_t    nofBits    = 20;
  Double_t minStep    =  0.01,;
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
  run->AddTask(stsDigitize);
  // -------------------------------------------------------------------------
  

  // =========================================================================
  // ===                     MVD local reconstruction                      ===
  // =========================================================================
  // -----   MVD Hit Finder   ------------------------------------------------
  CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0,
                iVerbose);
  run->AddTask(mvdHitFinder);
  // -------------------------------------------------------------------------
  // ===                 End of MVD local reconstruction                   ===
  // =========================================================================


  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================

  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",iVerbose);
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------

  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------

  // -----  STS hit matching   -----------------------------------------------
  FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
  run->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------

  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
  run->AddTask(stsFindTracks);
  // -------------------------------------------------------------------------

  // ---   STS track matching   ----------------------------------------------
  FairTask* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
  run->AddTask(stsMatchTracks);
  // -------------------------------------------------------------------------

  // ---   STS track fitting   -----------------------------------------------
  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
  FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
  run->AddTask(stsFitTracks);
  // -------------------------------------------------------------------------
  // ===                 End of STS local reconstruction                   ===
  // =========================================================================


  // =========================================================================
  // ===                     TRD local reconstruction                      ===
  // =========================================================================

  // Update of the values for the radiator F.U. 17.08.07
  Int_t trdNFoils = 130; // number of polyetylene foils
  Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
  Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
  Bool_t simpleTR = kTRUE; // use fast and simple version for TR
  // production

  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils,
                trdDFoils, trdDGap);

  CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
  run->AddTask(trdHitProd);

  // -------------------------------------------------------------------------
  // ===                 End of TRD local reconstruction                   ===
  // =========================================================================


  // =========================================================================
  // ===                     TOF local reconstruction                      ===
  // =========================================================================
  // ------   TOF hit producer   ---------------------------------------------
  CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer",
                iVerbose);
  run->AddTask(tofHitProd);
  // -------------------------------------------------------------------------
  // ===                   End of TOF local reconstruction                 ===
  // =========================================================================

  
  // =========================================================================
  // ===                        Global tracking                            ===
  // =========================================================================

  CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
  // Tracking method to be used
  // "branch" - branching tracking
  // "nn" - nearest neighbor tracking
  // "weight" - weighting tracking
  finder->SetTrackingType("branch");

  // Hit-to-track merger method to be used
  // "nearest_hit" - assigns nearest hit to the track
  finder->SetMergerType("nearest_hit");

  run->AddTask(finder);
  // ===                      End of global tracking                       ===
  // =========================================================================


  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);
  // ------------------------------------------------------------------------

  // -----   TRD track matching   --------------------------------------------
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
  run->AddTask(trdMatchTracks);
  // ----------------------------------------------------

  // ----------- TRD track Pid Wkn ----------------------
  CbmTrdSetTracksPidWkn* trdSetTracksPidTask = new CbmTrdSetTracksPidWkn(
                "trdFindTracks", "trdFindTracks");
  run->AddTask(trdSetTracksPidTask);
  // ----------------------------------------------------

  // ----------- TRD track Pid Ann ----------------------
  CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new CbmTrdSetTracksPidANN(
                "Ann", "Ann");
  run->AddTask(trdSetTracksPidAnnTask);
  // ----------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(strParamFile);
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------



  run->Init();



  run->Run(0, nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

