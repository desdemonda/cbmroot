// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events with standard settings
//
// HitProducers in MVD, RICH, TRD, TOF, ECAL
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
//
// --------------------------------------------------------------------------

#ifdef __CLING__
static TString caveGeom;
static TString pipeGeom;
static TString magnetGeom;
static TString mvdGeom;
static TString stsGeom;
static TString richGeom;
static TString muchGeom;
static TString shieldGeom;
static TString trdGeom;
static TString tofGeom; 
static TString ecalGeom;
static TString platformGeom;
static TString psdGeom;
static Double_t psdZpos; 
static Double_t psdXpos;

static TString stsTag;  
static TString trdTag;
static TString tofTag;

static TString stsDigi;
static TString trdDigi;
static TString tofDigi;  

static TString  fieldMap;
static Double_t fieldZ;   
static Double_t fieldScale;
static Int_t    fieldSymType;

static TString defaultInputFile;
#endif

void run_reco(Int_t nEvents = 1) {

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "data/test.mc.root";

  // Parameter file
  TString parFile = "data/test.params.root";

  // Output file
  TString outFile = "data/test.eds.root";


  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString inDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = inDir + "/parameters/";

  TString setupFile = inDir + "/geometry/setup/sis300_electron_setup.C";
  TString setupFunct = "sis300_electron";
  setupFunct += "_setup()";

  gROOT->LoadMacro(setupFile);
  gInterpreter->ProcessLine(setupFunct);

  // --- STS digipar file is there only for L1. It is no longer required
  // ---  for STS digitisation and should be eventually removed.
  TString tmp = paramDir + stsDigi;
  TObjString stsDigiFile;
  stsDigiFile.SetString(tmp); 
  parFileList->Add(&stsDigiFile);
  cout << "macro/run/run_reco.C using: " << tmp << endl;

  tmp = paramDir + trdDigi;
  TObjString trdDigiFile; 
  trdDigiFile.SetString(tmp);
  parFileList->Add(&trdDigiFile);
  cout << "macro/run/run_reco.C using: " << trdDigi << endl;

  tmp = paramDir + tofDigi;
  TObjString tofDigiFile; 
  tofDigiFile.SetString(tmp);
  parFileList->Add(&tofDigiFile);
  cout << "macro/run/run_reco.C using: " << tofDigi << endl;

  // In general, the following parts need not be touched
  // ========================================================================


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run = new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----   MVD Digitiser   -------------------------------------------------
  CbmMvdDigitizer* mvdDigitise = new CbmMvdDigitizer("MVD Digitiser", 0, iVerbose);
  run->AddTask(mvdDigitise);
  // -------------------------------------------------------------------------

  // -----   MVD Clusterfinder   ---------------------------------------------
  CbmMvdClusterfinder* mvdCluster = new CbmMvdClusterfinder("MVD Clusterfinder", 0, iVerbose);
  run->AddTask(mvdCluster);
  // -------------------------------------------------------------------------


  // -----   STS digitizer   -------------------------------------------------
  // -----   The parameters of the STS digitizer are set such as to match
  // -----   those in the old digitizer. Change them only if you know what you
  // -----   are doing.
  Double_t dynRange       =   40960.;  // Dynamic range [e]
  Double_t threshold      =    4000.;  // Digitisation threshold [e]
  Int_t nAdc              =    4096;   // Number of ADC channels (12 bit)
  Double_t timeResolution =       5.;  // time resolution [ns]
  Double_t deadTime       = 9999999.;  // infinite dead time (integrate entire event)
  Double_t noise          =       0.;  // ENC [e]
  Int_t digiModel         =       1;   // Model: 1 = uniform charge distribution along track

  CbmStsDigitize* stsDigi = new CbmStsDigitize(digiModel);
  stsDigi->SetParameters(dynRange, threshold, nAdc, timeResolution,
                                 deadTime, noise);
  run->AddTask(stsDigi);
  // -------------------------------------------------------------------------



  // =========================================================================
  // ===                     MVD local reconstruction                      ===
  // =========================================================================

  // -----   MVD Hit Finder   ------------------------------------------------
  CbmMvdHitfinder* mvdHitfinder = new CbmMvdHitfinder("MVD Hit Finder", 0, iVerbose);
  mvdHitfinder->UseClusterfinder(kTRUE);
  run->AddTask(mvdHitfinder);
  // -------------------------------------------------------------------------

  // ===                 End of MVD local reconstruction                   ===
  // =========================================================================

  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================


  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsFindClusters();
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits();
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
//  FairTask* stsMatchHits = new CbmStsMatchHits();
//  run->AddTask(stsMatchHits);
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

  Bool_t  simpleTR  = kTRUE;  // use fast and simple version for TR production
  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR,"K++");
  //"K++" : micro structured POKALON
  //"H++" : PE foam foils
  //"G30" : ALICE fibers 30 layers

  Bool_t triangularPads = false;// Bucharest triangular pad-plane layout
  //Double_t triggerThreshold = 0.5e-6;//SIS100
  Double_t triggerThreshold = 1.0e-6;//SIS300
  Double_t trdNoiseSigma_keV = 0.1; //default best matching to test beam PRF

  CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
  trdDigiPrf->SetTriangularPads(triangularPads);
  trdDigiPrf->SetNoiseLevel(trdNoiseSigma_keV);
  run->AddTask(trdDigiPrf);

  CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
  trdCluster->SetNeighbourTrigger(true);
  trdCluster->SetTriggerThreshold(triggerThreshold);
  trdCluster->SetNeighbourRowTrigger(false);
  trdCluster->SetPrimaryClusterRowMerger(true);
  trdCluster->SetTriangularPads(triangularPads);
  run->AddTask(trdCluster);

  CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
  trdHit->SetTriangularPads(triangularPads);
  run->AddTask(trdHit);



 // =========================================================================
  // ===                     TOF local reconstruction                      ===
  // =========================================================================


  // ------   TOF hit producer   ---------------------------------------------
  CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("TOF HitProducerNew",iVerbose);
  tofHitProd->SetInitFromAscii(kFALSE);
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

  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);
  // ------------------------------------------------------------------------

  // ===                      End of global tracking                       ===
  // =========================================================================


  // -----   RECO-to-MC matching   --------------------------------------------
  CbmMatchRecoToMC* matchTask = new CbmMatchRecoToMC();
  run->AddTask(matchTask);
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

  // ----------- TRD track Pid Like ----------------------
  // Since in the newest version of this method depends on the global
  // track the task has to move after the global tracking
// FU 08.02.12 Switch the task off since the input file needed for the new geometry has to be generated first.
//  CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =
//              new CbmTrdSetTracksPidLike("Likelihood", "Likelihood");
//  run->AddTask(trdSetTracksPidLikeTask);
  // ----------------------------------------------------


  // =========================================================================
  // ===                        RICH reconstruction                        ===
  // =========================================================================

  if (richGeom.Length() != 0)  // if RICH is defined
    {
  // ---------------------RICH Hit Producer ----------------------------------
  CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
  richHitProd->SetDetectorType(4);
  richHitProd->SetNofNoiseHits(220);
  richHitProd->SetCollectionEfficiency(1.0);
  richHitProd->SetSigmaMirror(0.06);
  run->AddTask(richHitProd);
  //--------------------------------------------------------------------------

  //--------------------- RICH Reconstruction ----------------------------------
  CbmRichReconstruction* richReco = new CbmRichReconstruction();
  run->AddTask(richReco);

  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings();
  run->AddTask(matchRings);
  // -------------------------------------------------------------------------
    }
  // ===                 End of RICH local reconstruction                  ===
  // =========================================================================


  // =========================================================================
  // ===                        ECAL reconstruction                        ===
  // =========================================================================

  // -----   ECAL hit producer  ----------------------------------------------

  CbmEcalStructureFiller* ecalFiller=new CbmEcalStructureFiller("ecalFiller", iVerbose, "ecal/ecal_v12a.geo");                                                 
  ecalFiller->SetUseMCPoints(kTRUE); 
  ecalFiller->StoreTrackInformation();
  run->AddTask(ecalFiller);

  CbmEcalDigi* ecalDigi=new CbmEcalDigi("ecalDigi", iVerbose);
  run->AddTask(ecalDigi);

  CbmEcalPrepare* ecalPrepare=new CbmEcalPrepare("ecalPrepare", iVerbose);
  run->AddTask(ecalPrepare);

  CbmEcalTracking* ecalTracking=new CbmEcalTracking("ecalTracking", iVerbose);
  CbmEcalTrackExtrapolationKF* ecalTrackExtrapolation=new CbmEcalTrackExtrapolationKF(iVerbose);
  ecalTracking->SetTrackExtrapolation(ecalTrackExtrapolation);
  run->AddTask(ecalTracking);

  CbmEcalSCurveLib* ecalSCurve=new CbmEcalSCurveLib("ecalSCurveLib", 0);
  ecalSCurve->AddFile("$VMCWORKDIR/parameters/ecal/scurve2_x22.root");
  ecalSCurve->AddFile("$VMCWORKDIR/parameters/ecal/scurve4_x25.root");
  run->AddTask(ecalSCurve);

  CbmEcalCalibrationV2* ecalECalibration=new CbmEcalCalibrationV2("ecalECalibration", iVerbose, "$VMCWORKDIR/parameters/ecal/calibratione_x22.cfg");
  run->AddTask(ecalECalibration);

  CbmEcalMaximumLocator* ecalLocator=new CbmEcalMaximumLocator("ecalMaximumLocator", 10);
  run->AddTask(ecalLocator);
  
  CbmEcalIdentification* ecalId=new CbmEcalIdentification("ecalId", iVerbose, "$VMCWORKDIR/parameters/ecal/id_jun25.cfg");
  run->AddTask(ecalId);

  CbmEcalCalibrationV2* ecalCalibration=new CbmEcalCalibrationV2("ecalECalibration", iVerbose, "$VMCWORKDIR/parameters/ecal/calibration_x22.cfg");
  run->AddTask(ecalCalibration);

  CbmEcalClusterFinder* ecalCls=new CbmEcalClusterFinder("ecalCls", iVerbose, "$VMCWORKDIR/parameters/ecal/clusterfinder_light.cfg");
  run->AddTask(ecalCls);

  CbmEcalShLibTable* ecalShLib=new CbmEcalShLibTable("ecalShLib", iVerbose);
  ecalShLib->AddFile("$VMCWORKDIR/parameters/ecal/shlib2_x22.root");
  ecalShLib->AddFile("$VMCWORKDIR/parameters/ecal/shlib4_x22.root");
  run->AddTask(ecalShLib);

  CbmEcalReco* ecalReco=new CbmEcalReco("ecalReco", iVerbose, "$VMCWORKDIR/parameters/ecal/reco_light.cfg");
  run->AddTask(ecalReco);

  CbmEcalMatching* ecalMatch=new CbmEcalMatching("ecalMatching", iVerbose, "$VMCWORKDIR/parameters/ecal/matching_light.cfg");
  run->AddTask(ecalMatch);

  // -------------------------------------------------------------------------

  // ===                      End of ECAL reconstruction                   ===
  // =========================================================================

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
