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


void run_reco_pi0(Int_t nEvents = 1, Int_t urqmd = 0)
{
  if (nEvents > 500) nEvents = 500;
  Bool_t trdSmearing(1), trdClustering(0), qa(0);
  // ========================================================================
  // geometry selection for sim + reco  by Cyrano                            
  // ========================================================================
  ifstream whichTrdGeo;
  whichTrdGeo.open("whichTrdGeo",ios::in);
  TString digipar;
  if (whichTrdGeo) whichTrdGeo >> digipar;
  cout << "selected geometry : >> " << digipar << " << (to select a different geometry, edit macro/trd/whichTrdGeo file)" << endl;
  whichTrdGeo.close();
  if (digipar.Length() == 0) digipar = "trd_standard";

  // ========================================================================
  //          Adjust this part according to your requirements
  gROOT->Reset(); 
  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);                        
  gStyle->SetPadTickY(1);  
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);
  TString trdOption;
  if (trdSmearing) 
    trdOption = "smearing";
  else if (trdClustering) 
    trdOption = "cluster";
  else
    trdOption = "no_trd";
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile;// = "data/test.mc.root";
  inFile.Form("data/test.mc.%04i.root",urqmd);
  // Parameter file
  TString parFile;// = "data/params.root";
  parFile.Form("data/params.%04i.root",urqmd);
  //  Digitisation files
  TList *parFileList = new TList();

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";

  TObjString stsDigiFile = paramDir + "/sts/sts_v13d.digi.par";
  parFileList->Add(&stsDigiFile);
  /*
    TString trdDigiDir = gSystem->Getenv("VMCWORKDIR");
    trdDigiDir += "/macro/run/";
  */
  TObjString trdDigiFile = paramDir + "/trd/" + digipar + ".digi.par";
  // TObjString trdDigiFile = paramDir + "/trd/trd_standard.digi.par";
  parFileList->Add(&trdDigiFile);


  // Output file
  TString statistic;
  statistic.Form(".%03ievents",nEvents);
  TString outFile;// = "data/test.pa." + digipar + statistic + ".root";
  outFile.Form("data/test.pa.%s.%s.%04i.%03ievents.root",digipar.Data(),trdOption.Data(),urqmd,nEvents);

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
  /*dielectron*/gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
  /*dielectron*/cbmlibs();
  /*dielectron*/gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");
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
  /*
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
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libGlobal");
    gSystem->Load("libL1");
    gSystem->Load("libLittrack");
    gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
  */
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  


  
  // =========================================================================
  // ===                     MVD local reconstruction                      ===
  // =========================================================================
  

  // -----   MVD Digitiser   -------------------------------------------------
  CbmMvdDigitizeL* mvdDigi = new CbmMvdDigitizeL("MVD Digitiser", 
						 0, iVerbose);
  /*dielectron*/mvdDigi->SetPixelSize(18.4); //should be 20x20
  /*dielectron*/mvdDigi->SetEpiThickness(0.0014);
  run->AddTask(mvdDigi);
  // -------------------------------------------------------------------------
 

  // -----   MVD Hit Finder   ------------------------------------------------
  CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder",
						    0, iVerbose);
  /*dielectron*/mvdHitFinder->SetSigmaNoise(11,kTRUE);
  /*dielectron*///mvdHitFinder->ShowDebugHistograms();
  /*dielectron*/mvdHitFinder->SetAdcDynamic(150);
  /*dielectron*/mvdHitFinder->SetAdcOffset(0);
  /*dielectron*/mvdHitFinder->SetAdcBits(1);
  /*dielectron*/mvdHitFinder->SetSeedThreshold(1); //75 ele
  /*dielectron*/mvdHitFinder->SetNeighbourThreshold(1); //75 el
  run->AddTask(mvdHitFinder);
  // -------------------------------------------------------------------------

  

  // ===                 End of MVD local reconstruction                   ===
  // =========================================================================
  





  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================
  /*dielectron*/Double_t threshold  =  4;
  /*dielectron*/Double_t noiseWidth =  0.1;
  /*dielectron*/Int_t    nofBits    = 20;
  /*dielectron*/Double_t minStep    =  0.01;
  /*dielectron*/Double_t StripDeadTime = 10.;
  // -----   STS digitizer   -------------------------------------------------
  CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
  
  /*dielectron*/stsDigitize->SetRealisticResponse();
  /*dielectron*/stsDigitize->SetFrontThreshold (threshold);
  /*dielectron*/stsDigitize->SetBackThreshold  (threshold);
  /*dielectron*/stsDigitize->SetFrontNoiseWidth(noiseWidth);
  /*dielectron*/stsDigitize->SetBackNoiseWidth (noiseWidth);
  /*dielectron*/stsDigitize->SetFrontNofBits   (nofBits);
  /*dielectron*/stsDigitize->SetBackNofBits    (nofBits);
  /*dielectron*/stsDigitize->SetFrontMinStep   (minStep);
  /*dielectron*/stsDigitize->SetBackMinStep    (minStep);
  /*dielectron*/stsDigitize->SetStripDeadTime  (StripDeadTime);
  run->AddTask(stsDigitize);
  // -------------------------------------------------------------------------


  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",
						       iVerbose);
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits(iVerbose);
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
  FairTask* stsMatchHits = new CbmStsMatchHits(iVerbose);
  run->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------


  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder    = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder, /*dielectron Bool_t useMvdInTracking= */true);
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
  Int_t trdNFoils    = 130;      // number of polyetylene foils
  Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
  Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
  Bool_t simpleTR = kTRUE;       // use fast and simple version for TR
  // production

  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils,
						trdDFoils, trdDGap);
  if (trdSmearing) {
    CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
    run->AddTask(trdHitProd);
  } else if (trdClustering) {
    // -----   TRD clusterizer     ----------------------------------------------
  
    CbmTrdClusterizerFast* trdCluster = new CbmTrdClusterizerFast("TRD Clusterizer", "TRD task",radiator,false,true);
    run->AddTask(trdCluster);
    printf("Init ClusterfinderFast\n");
      
    //printf("HIER KOMMT DER CLUSTERFINDERFAST\n");
    CbmTrdClusterFinderFast* trdClusterfindingfast = new CbmTrdClusterFinderFast(true, true, false, 5.0e-7);
    run->AddTask(trdClusterfindingfast);
    printf("Finished ClusterfinderFast\n");
  
    CbmTrdHitProducerCluster* trdClusterHitProducer = new CbmTrdHitProducerCluster();
    run->AddTask(trdClusterHitProducer);
    printf("Finished Hit Producer\n");
  }
  /*
  // -------------------------------------------------------------------------
  */
  /*

  // ===                 End of TRD local reconstruction                   ===
  // =========================================================================
  */




  
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
 
  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);



  // -----   TRD track matching   --------------------------------------------
  CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(iVerbose);
  run->AddTask(trdMatchTracks);
   
  // ----------- TRD track Pid Wkn ----------------------
  CbmTrdSetTracksPidWkn* trdSetTracksPidTask = new  CbmTrdSetTracksPidWkn("CbmTrdSetTracksPidWkn","CbmTrdSetTracksPidWkn");
  run->AddTask(trdSetTracksPidTask);
  // ----------------------------------------------------

  // ----------- TRD track Pid Ann ----------------------
  CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new  CbmTrdSetTracksPidANN("CbmTrdSetTracksPidAnn","CbmTrdSetTracksPidAnn");
  run->AddTask(trdSetTracksPidAnnTask);
  // ----------------------------------------------------
  
  CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =  new CbmTrdSetTracksPidLike("CbmTrdSetTracksPidLike","CbmTrdSetTracksPidLike");
  run->AddTask(trdSetTracksPidLikeTask);
  // -------------------------------------------------------------------------
  
  // =========================================================================
  // ===                        RICH reconstruction                        ===
  // =========================================================================
  
  CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
  richHitProd->SetDetectorType(4);
  richHitProd->SetNofNoiseHits(220);
  richHitProd->SetCollectionEfficiency(1.0);
  richHitProd->SetSigmaMirror(0.06);
  run->AddTask(richHitProd);

  CbmRichReconstruction* richReco = new CbmRichReconstruction();
  run->AddTask(richReco);

  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings();
  run->AddTask(matchRings);

  //isRich
  /*
  // Reconstruction Qa
  CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
  trackingQa->SetMinNofPointsSts(4);
  trackingQa->SetUseConsecutivePointsInSts(true);
  trackingQa->SetMinNofPointsTrd(8);
  trackingQa->SetMinNofPointsMuch(10);
  trackingQa->SetMinNofPointsTof(1);
  trackingQa->SetQuota(0.7);
  trackingQa->SetMinNofHitsTrd(8);
  trackingQa->SetMinNofHitsMuch(10);
  trackingQa->SetVerbose(0);
  trackingQa->SetMinNofHitsRich(7);
  trackingQa->SetQuotaRich(0.6);
  trackingQa->SetOutputDir("recqa/");
  run->AddTask(trackingQa);
  */
  //   CbmLitFitQa* fitQa = new CbmLitFitQa();
  //   fitQa->SetMvdMinNofHits(0);
  //   fitQa->SetStsMinNofHits(4);
  //   fitQa->SetMuchMinNofHits(10);
  //   fitQa->SetTrdMinNofHits(8);
  //   //fitQa->SetOutputDir(std::string(resultDir));
  //   run->AddTask(fitQa);
  /*
    CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
    clusteringQa->SetOutputDir("recqa/");
    run->AddTask(clusteringQa);
  */

  /*
  // -----   STS-TRD-TOF track merging   -------------------------------------
  CbmL1TrackMerger* trackMerger          = new CbmL1TrackMerger();
  trackMerger->SetMethod(1);
  CbmL1TofMerger* tofMerger = new CbmL1TofMerger();
  CbmFindGlobalTracks* findGlobal = new CbmFindGlobalTracks(trackMerger,
  NULL, tofMerger,
  iVerbose);
  run->AddTask(findGlobal);
  
  //--------- TOF tracklength calculation -----------------------------------
  CbmGlobalTrackFitterKF* globalFitter = new CbmGlobalTrackFitterKF();
  CbmFitGlobalTracks* fitGlobal
  = new
  CbmFitGlobalTracks("globalfitter",iVerbose,globalFitter);
  
  run->AddTask(fitGlobal);
  //-------------------------------------------------------------------------

   
  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);
  // ------------------------------------------------------------------------
  
  // ===                      End of global tracking                       ===
  // =========================================================================

  


  // ----------- TRD track Pid Like ----------------------
  // Since in the newest version of this method depends on the global
  // track the task has to move after the global tracking
  CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask = new
  CbmTrdSetTracksPidLike("Likelihood","Likelihood");
  run->AddTask(trdSetTracksPidLikeTask);
  // ----------------------------------------------------



  /*
  // =========================================================================
  // ===                        RICH reconstruction                        ===
  // =========================================================================

  // ---------------------RICH Hit Producer ----------------------------------
  Double_t richPmtRad  = 0.4;     // PMT radius [cm]
  Double_t richPmtDist = 0.;      // Distance between PMTs [cm]
  Int_t    richDetType = 4;       // Detector type Hamamatsu H8500-03 (no WLS)
  Int_t    richNoise   = 220;     // Number of noise points per event
  Double_t richCollEff = 1.0;     // Collection Efficiency of PMT electron optics
  Double_t richSMirror = 0.06;     // Sigma for additional point smearing due to light scattering in mirror

  CbmRichHitProducer* richHitProd = new CbmRichHitProducer(richPmtRad,
  richPmtDist, richDetType, richNoise, iVerbose, richCollEff,
  richSMirror);
  run->AddTask(richHitProd);

  //--------------------------------------------------------------------------

       
  //----------------------RICH Track Extrapolation ---------------------------
  Int_t    richNSts = 4;     // minimum number of STS hits for extrapolation
  Double_t richZPos = 300.;  // z position for extrapolation [cm]
  CbmRichTrackExtrapolation* richExtra
  = new CbmRichTrackExtrapolationKF(richNSts, iVerbose);
  CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
  richExtrapolate->UseExtrapolation(richExtra,richZPos);
  run->AddTask(richExtrapolate);
  //--------------------------------------------------------------------------


  //--------------------- Rich Track Projection to photodetector -------------
  Int_t richZFlag = 1;       // Projetion from IM plane (default)
  CbmRichProjectionProducer* richProj = 
  new CbmRichProjectionProducer(iVerbose, richZFlag);
  run->AddTask(richProj);
  //--------------------------------------------------------------------------
  
  
  //--------------------- RICH Ring Finding ----------------------------------
  TString richGeoType = "compact";//choose between compact or large 
  CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose, richGeoType);
  CbmRichFindRings* richFindRings = new CbmRichFindRings();
  richFindRings->UseFinder(richFinder);
  run->AddTask(richFindRings);
  //--------------------------------------------------------------------------
    

  
  //-------------------- RICH Ring Fitting -----------------------------------
  Double_t iRingCorr = 1.;      // correction done (default), choose 0 if not
  CbmRichRingFitter* richFitter = new CbmRichRingFitterTAU(iVerbose,
  iRingCorr, 2);
  CbmRichFitRings* fitRings = new CbmRichFitRings("","",richFitter);
  run->AddTask(fitRings);
  //--------------------------------------------------------------------------
  
  
  //-------------------- RICH Ring Fitting -----------------------------------  
  CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau(iVerbose, 1, richGeoType);
  CbmRichFitRings* fitRings = new CbmRichFitRings("","",richFitter);
  run->AddTask(fitRings);
  //--------------------------------------------------------------------------



  // ------------------- RICH Ring matching  ---------------------------------
  CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
  run->AddTask(matchRings);
  // -------------------------------------------------------------------------



  //--------------------- RICH ring-track assignment ------------------------
  Double_t richDistance = 10.; // Max. dist. ring centre to track [cm]
  Int_t    richNPoints  = 5;   // Minmum number of hits on ring
  CbmRichRingTrackAssign* richAssign   =
  new CbmRichRingTrackAssignClosestD(richDistance, richNPoints, iVerbose);
  CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
  assignTrack->UseAssign(richAssign);
  run->AddTask(assignTrack);
  // ------------------------------------------------------------------------
  

  // ===                 End of RICH local reconstruction                  ===
  // =========================================================================
  




  // =========================================================================
  // ===                        ECAL reconstruction                        ===
  // =========================================================================
 
  // -----   ECAL hit producer  ----------------------------------------------
  CbmEcalHitProducerFastMC* ecalHitProd 
  = new CbmEcalHitProducerFastMC("ECAL Hitproducer");
  run->AddTask(ecalHitProd);
  // -------------------------------------------------------------------------
  
  // ===                      End of ECAL reconstruction                   ===
  // =========================================================================
  
  
  


  
  // Taken from CbmDileptonPid.C
  /*
  // Init simulation parameters from root file
  TString stsDigiFile2 = "sts_standard.digi.par";
  TString stsDigi = gSystem->Getenv("VMCWORKDIR");// + "/parameters/sts/" + stsDigiFile2;
  cout << stsDigi << endl;
  stsDigi.Append("/parameters/sts/");
  stsDigi.Append(stsDigiFile2.Data());
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile);
  parIo2->open(stsDigi.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);

  // Create and add users tasks

  // CbmKF is needed for Extrapolation
  CbmKF* kf = new CbmKF();
  run->AddTask(kf);
           
  // Electron id assignment 
  CbmDileptonAssignPid* taskPid = new CbmDileptonAssignPid(1);

  Float_t aRichCuts[] = {1., 0.4, 0., 130, 21.85, 4.35, 6.17, 0.14}; // Distance, SelectionNN, selection2D, Radial, NHitMean, NHitSigma, RMean, RSigma
  taskPid->SetRichCuts(true, true, aRichCuts); //sMomentum, sSelection (true=NN, false=2D)
    
  Float_t aTrdCuts[] = {1.5, 0.95, 1.1, 21., 0.9}; // Mom, PidLikeLow, PidLikeHigh, PidWkn, PidAnn
  taskPid->SetTrdCuts(false, false, true, false, false, aTrdCuts); //sMom, sAccept, sLike, sWkn, sAnn
    
  taskPid->SetTofCuts(true, false, 0.01); //sMom, sAccept, Mass2
    
  run->AddTask(taskPid);
   
  // MC Association, needed for simulated data stream
  CbmDileptonAssignMCid* taskMCid = new CbmDileptonAssignMCid();
  taskMCid->SetCuts(3.,130.,4); //chi2primary, radial, sts
  run->AddTask(taskMCid);
  */

  //
  if (qa) {
    CbmTrdOccupancy* trdOccupancy = new CbmTrdOccupancy("TRD Occupancy", "TRD task", digipar, 5.0e-7);
    run->AddTask(trdOccupancy);
    CbmTrdQa* trdQa = new CbmTrdQa("TRD QA", "TRD task", digipar, 5.0e-7);
    run->AddTask(trdQa);
  }
  CbmTrdPhotonAnalysis *trdphot = new CbmTrdPhotonAnalysis("PhotonAnalysis","PhotonAnalysis",iVerbose);
  run->AddTask(trdphot);


  // -----  Parameter database   --------------------------------------------
  /*
    TString stsDigi = gSystem->Getenv("VMCWORKDIR");
    stsDigi += "/parameters/sts/";
    stsDigi += (TString)stsDigiFile;
  */
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList,"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


     
  // -----   Intialise and run   --------------------------------------------
  //  run->LoadGeometry();
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0,nEvents);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
