// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events with standard settings
//
// HitProducers in (MVD, RICH, TRD, TOF, ECAL,) PSD (detector in brackets not included by default)
// Digitizer and HitFinder in STS
// FAST MC for ECAL
// STS track finding and fitting (L1 / KF)
// TRD track finding and fitting (L1 / KF)
// RICH ring finding (ideal) and fitting
// Global track finding (ideal), rich assignment
// Primary vertex finding (ideal)
// Matching of reconstructed and MC tracks in STS, RICH and TRD
//
// V. Friese   24/02/2006
// Version     24/04/2007 (V. Friese)
// Version     22/10/2014 (Selim)
// --------------------------------------------------------------------------


void run_reco_flow(Double_t En=10, Int_t nEvents = 2, Int_t fileNum = 5)
{
  Int_t gen = 0;
  TString sGen = "";
  if (gen == 0) sGen = "urqmd";
  if (gen == 1) sGen = "shield";
  
  TString numEvt = "";
  if(nEvents<10) numEvt="000";
  if(nEvents>=10 && nEvents<100) numEvt="00";                                                                       
  if(nEvents>=100 && nEvents<1000) numEvt="0";
  if(nEvents>=1000 && nEvents<10000) numEvt="";
  numEvt += nEvents;
  
  TString sEn = "";
  sEn += En;

  TString sfileNum = "";
  if(fileNum<10) sfileNum="000";
  if(fileNum>=10 && fileNum<100) sfileNum="00";
  if(fileNum>=100 && fileNum<1000) sfileNum="0";
  if(fileNum>=1000 && fileNum<10000) sfileNum="";
  sfileNum += fileNum;
  
  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  TString dir = "/hera/cbm/users/sseddiki/test/";
  TString inFile = dir + "sim/mc_" + numEvt +  "evt_" + sfileNum +  ".root";
  TString parFile = dir + "sim/params_" + numEvt +  "evt_" + sfileNum +  ".root";    
  TString outFile = dir + "reco/reco_" + numEvt +  "evt_" + sfileNum +  ".root";

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly 
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString workDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = workDir + "/parameters";

  TObjString stsDigiFile = paramDir + "/sts/sts_v13d_std.digi.par";
  parFileList->Add(&stsDigiFile);

  TString stsMatBudgetFileName = paramDir + "/sts/sts_matbudget_v13d.root";
  cout << "STS MB : " << stsMatBudgetFileName << endl;
  
  //TObjString trdDigiFile =  paramDir + "/trd/trd_v13o.digi.par";
  //parFileList->Add(&trdDigiFile);

  //TObjString tofDigiFile =  paramDir + "/tof/tof_v13b.digi.par";
  //parFileList->Add(&tofDigiFile);


  // In general, the following parts need not be touched
  // ========================================================================


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // ----  Load libraries   -------------------------------------------------
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
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libLittrack");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libGlobal");
  gSystem->Load("libL1");
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
  gSystem->Load("libPsd");

      // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run = new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // =========================================================================
  // ===             Detector Response Simulation (Digitiser)              ===
  // ===                          (where available)                        ===
  // =========================================================================


  // -----   MVD Digitiser   -------------------------------------------------
  CbmMvdDigitizeL* mvdDigi =
  		new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
  //run->AddTask(mvdDigi);
  // -------------------------------------------------------------------------
 

  // -----   STS digitizer   -------------------------------------------------
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.01;
  Int_t    nofBits    = 12;
  Double_t electronsPerAdc    =  10;
  Double_t StripDeadTime = 0.1;
  CbmStsDigitize* stsDigitize = new CbmStsDigitize();
  stsDigitize->SetRealisticResponse();
  stsDigitize->SetFrontThreshold (threshold);
  stsDigitize->SetBackThreshold  (threshold);
  stsDigitize->SetFrontNoiseWidth(noiseWidth);
  stsDigitize->SetBackNoiseWidth (noiseWidth);
  stsDigitize->SetFrontNofBits   (nofBits);
  stsDigitize->SetBackNofBits    (nofBits);
  stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
  stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
  stsDigitize->SetStripDeadTime  (StripDeadTime);
  run->AddTask(stsDigitize);
  // -------------------------------------------------------------------------


  // =========================================================================
  // ===                     MVD local reconstruction                      ===
  // =========================================================================


  // -----   MVD Hit Finder   ------------------------------------------------
  CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0,
  		iVerbose);
  //run->AddTask(mvdHitFinder);
  // -------------------------------------------------------------------------


  // ===                 End of MVD local reconstruction                   ===
  // =========================================================================




  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================


  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsClusterFinder();
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits();
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
  FairTask* stsMatchHits = new CbmStsMatchHits();
  run->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------


  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  
  CbmL1* l1 = new CbmL1();
  l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
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
  //run->AddTask(trdHitProd);

  // -------------------------------------------------------------------------
  // ===                 End of TRD local reconstruction                   ===
  // =========================================================================


  // =========================================================================
  // ===                     TOF local reconstruction                      ===
  // =========================================================================


  // ------   TOF hit producer   ---------------------------------------------
  CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("TOF HitProducerNew",iVerbose); 
  tofHitProd->SetInitFromAscii(kFALSE);
  //run->AddTask(tofHitProd);
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

  //run->AddTask(finder);

  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);
  // ------------------------------------------------------------------------

  // ===                      End of global tracking                       ===
  // =========================================================================


  // -----   TRD track matching   --------------------------------------------
  //CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
  //run->AddTask(trdMatchTracks);
  // ----------------------------------------------------

  // ----------- TRD track Pid Wkn ----------------------
  //CbmTrdSetTracksPidWkn* trdSetTracksPidTask = new CbmTrdSetTracksPidWkn(  		"trdFindTracks", "trdFindTracks");
  //run->AddTask(trdSetTracksPidTask);
  // ----------------------------------------------------

  // ----------- TRD track Pid Ann ----------------------
  //CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new CbmTrdSetTracksPidANN(  		"Ann", "Ann");
  //run->AddTask(trdSetTracksPidAnnTask);
  // ----------------------------------------------------

  // ----------- TRD track Pid Like ----------------------
  // Since in the newest version of this method depends on the global
  // track the task has to move after the global tracking
// FU 08.02.12 Switch the task off since the input file needed for the new geometry has to be generated first.
//  CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =
//  		new CbmTrdSetTracksPidLike("Likelihood", "Likelihood");
//  run->AddTask(trdSetTracksPidLikeTask);
  // ----------------------------------------------------


  // =========================================================================
  // ===                        RICH reconstruction                        ===
  // =========================================================================

  // ---------------------RICH Hit Producer ----------------------------------
  CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
  richHitProd->SetDetectorType(4);
  richHitProd->SetNofNoiseHits(220);
  richHitProd->SetCollectionEfficiency(1.0);
  richHitProd->SetSigmaMirror(0.06);
  //run->AddTask(richHitProd);
  //--------------------------------------------------------------------------

  //--------------------- RICH Reconstruction ----------------------------------
  CbmRichReconstruction* richReco = new CbmRichReconstruction();
  //run->AddTask(richReco);

  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings();
  //run->AddTask(matchRings);
  // -------------------------------------------------------------------------
  // ===                 End of RICH local reconstruction                  ===
  // =========================================================================


/*
  // =========================================================================
  // ===                        ECAL reconstruction                        ===
  // =========================================================================

  // -----   ECAL hit producer  ----------------------------------------------
  CbmEcalHitProducerFastMC* ecalHitProd = new CbmEcalHitProducerFastMC(
  		"ECAL Hitproducer");
  run->AddTask(ecalHitProd);
  // -------------------------------------------------------------------------

  // ===                      End of ECAL reconstruction                   ===
  // =========================================================================
*/

  // =========================================================================
  // ===                        PSD reconstruction                        ===
  // =========================================================================

  // -----   PSD hit producer  ----------------------------------------------
  CbmPsdIdealDigitizer* psdDigi = new CbmPsdIdealDigitizer();
  run->AddTask(psdDigi);

  CbmPsdHitProducer* psdHit = new CbmPsdHitProducer();
  run->AddTask(psdHit);

  // ------  KF particle finder
  /*
  float cuts[2][3] = {{3.,3..-100.},{3.,3.,-100.}};
  CbmKFParticlesFinder* kfPartFinder = new CbmKFParticlesFinder(cuts,1);
  run->AddTask(kfPartFinder);
  
  CbmKFParticlesFinderQA* kfPartFinderQA = new CbmKFParticlesFinderQA(kfPartFinder,1,3);
  kfPartFinderQA->SaveParticles(1);
  kfPartFinderQA->SaveMCParticles(1);
  run->AddTask(kfPartFinderQA);
  */
  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is " << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

//  delete run;

  cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
