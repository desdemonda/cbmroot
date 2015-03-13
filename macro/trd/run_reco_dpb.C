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
// 20130605 - checked by DE
// --------------------------------------------------------------------------


void run_reco_dpb(Int_t nEvents = 1, Int_t urqmd = 0000) 
{
  gStyle->SetNumberContours(99); 
  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  // ========================================================================
  // geometry selection for sim + reco  by Cyrano                            
  // ========================================================================
  ifstream whichTrdGeo;
  whichTrdGeo.open("whichTrdGeo",ios::in);
  TString selectGeo;
  if (whichTrdGeo) whichTrdGeo >> selectGeo;
  TString digipar = selectGeo(0,11);
  digipar.ReplaceAll(".ge","");     
  cout << "selected geometry : >> " << selectGeo << " << (to select a different geometry, edit macro/trd/whichTrdGeo file)" << endl;
  cout << "selected digipar  : >> " << digipar << " << " << endl;
  whichTrdGeo.close();
  if (digipar.Length() == 0) digipar = "trd_standard";

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  TString fileName;
  // Input file (MC events)
  fileName.Form("test.mc.%04i.root",urqmd);
  TString inFile = "data/sim/" + fileName;//test.mc.root";

  // Parameter file
  fileName.Form("test.params.%04i.root",urqmd);
  TString parFile = "data/sim/" + fileName;//params.root";

  // Output file
  fileName.Form("test.eds.%04i.root",urqmd);
  TString outFile = "data/reco/" + fileName;//test.eds.root";

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

  //  TObjString trdDigiFile =  paramDir + "/trd/trd_v13o.digi.par";
  //  parFileList->Add(&trdDigiFile);

  TObjString trdDigiFile = paramDir + "/trd/" + digipar + ".digi.par";
  parFileList->Add(&trdDigiFile);


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

  // =========================================================================
  // ===                     TRD local reconstruction                      ===
  // =========================================================================
  /*
  // Update of the values for the radiator F.U. 17.08.07
  Int_t   trdNFoils = 130;    // number of polyethylene foils
  Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
  Float_t trdDGap   = 0.02;   // thickness of gap between foils [cm]
  Bool_t  simpleTR  = kTRUE;  // use fast and simple version for TR production

  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils, trdDFoils, trdDGap);

  // -----   TRD hit producer   ----------------------------------------------
  //CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);

  // CbmTrdDigitizerPRF* trdHitProd = new CbmTrdDigitizerPRF(radiator);
  //run->AddTask(trdHitProd);

  //  CbmTrdHitRateFastQa* trdHFQ = new CbmTrdHitRateFastQa();
  //  run->AddTask(trdHFQ);
  //
  //  CbmTrdDigitizer* trdDigitizer = new CbmTrdDigitizer(radiator);
  //  run->AddTask(trdDigitizer);
  //
  //  CbmTrdHitProducerDigi* trdHitProd = new CbmTrdHitProducerDigi();
  //  run->AddTask(trdHitProd);

  //Double_t triggerThreshold = 0.5e-6;//SIS100
  Bool_t triangularPads = false;
  Double_t triggerThreshold = 1.0e-6;//SIS300
  //Double_t triggerThreshold = 3.0e-7;//0.5cm homogeniuse pad height
  //CbmTrdDigitizer* trdDigi = new CbmTrdDigitizer(radiator);
  //run->AddTask(trdDigi);
  
  CbmTrdHitProducerDigi* trdHit = new CbmTrdHitProducerDigi();
  run->AddTask(trdHit);
  
  CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
  trdDigiPrf->SetTriangularPads(triangularPads);
  run->AddTask(trdDigiPrf);
  
  CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
  trdCluster->SetNeighbourTrigger(true);
  trdCluster->SetTriggerThreshold(triggerThreshold);//1e-6
  trdCluster->SetNeighbourRowTrigger(false);
  trdCluster->SetPrimaryClusterRowMerger(true);
  trdCluster->SetTriangularPads(triangularPads);
  run->AddTask(trdCluster);
  
  CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
  trdHit->SetTriangularPads(triangularPads);
  run->AddTask(trdHit);
  if (false && nEvents == 1){
  CbmTrdHitRateFastQa* hitrate = new CbmTrdHitRateFastQa();
  run->AddTask(hitrate);
  } else if (nEvents == 1){
  CbmTrdRecoQa* trdrecoqa = new CbmTrdRecoQa("CbmTrdRecoQa","CbmTrdRecoQa");
  trdrecoqa->SetTriangularPads(triangularPads);
  trdrecoqa->SetTriggerThreshold(triggerThreshold);//1e-6
  run->AddTask(trdrecoqa);
  } else {    
    
  CbmTrdQa* trdqa = new CbmTrdQa();
  trdqa->SetTriggerThreshold(triggerThreshold);
  run->AddTask(trdqa);
    
  CbmTrdOccupancyQa* trdocc = new CbmTrdOccupancyQa();
  trdocc->SetNeighbourTrigger(true);
  trdocc->SetTriggerThreshold(triggerThreshold);
  trdocc->SetNeighbourReadout(true);
  run->AddTask(trdocc);
      
  CbmTrdHitDensityQa* trdhitdens = new CbmTrdHitDensityQa();
  trdhitdens->SetNeighbourTrigger(false);
  run->AddTask(trdhitdens);
      
  }
  */

  Bool_t  simpleTR  = kTRUE; 
  CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR,"K++");
  // -----   TRD hit producer   ----------------------------------------------

  Bool_t triangularPads = false;// Bucharest triangular pad-plane layout
  //Double_t triggerThreshold = 0.5e-6;//SIS100
  Double_t triggerThreshold = 1.0e-6;//SIS300
  //Double_t triggerThreshold = 3.0e-7;//0.5cm homogeniuse pad height
  Double_t trdNoiseSigma_keV = 0.1;

  CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
  trdDigiPrf->SetTriangularPads(triangularPads);
  trdDigiPrf->SetNoiseLevel(trdNoiseSigma_keV);
  run->AddTask(trdDigiPrf);
  
  CbmTrdSPADIC* trdSPADIC = new CbmTrdSPADIC();
  run->AddTask(trdSPADIC);

  CbmTrdClusterFinderDPB* trdCluster = new CbmTrdClusterFinderDPB();
  run->AddTask(trdCluster);

  /*
    CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
    trdCluster->SetNeighbourTrigger(true);
    trdCluster->SetTriggerThreshold(triggerThreshold);
    trdCluster->SetNeighbourRowTrigger(false);
    trdCluster->SetPrimaryClusterRowMerger(true);
    trdCluster->SetTriangularPads(triangularPads);
    run->AddTask(trdCluster);
  */

  CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
  trdHit->SetTriangularPads(triangularPads);
  run->AddTask(trdHit);
  /*
    if (false && nEvents == 1){
    CbmTrdHitRateFastQa* hitrate = new CbmTrdHitRateFastQa();
    run->AddTask(hitrate);
    } else if (nEvents == 1){
  */
  CbmTrdRecoQa* trdrecoqa = new CbmTrdRecoQa("CbmTrdRecoQa","CbmTrdRecoQa");
  trdrecoqa->SetTriangularPads(triangularPads);
  trdrecoqa->SetTriggerThreshold(triggerThreshold);//1e-6
  run->AddTask(trdrecoqa);
  /*} else
    {    
    /*
    CbmTrdQa* trdqa = new CbmTrdQa(radiator);
    trdqa->SetTriggerThreshold(triggerThreshold);
    run->AddTask(trdqa);
    
    /*
    CbmTrdOccupancyQa* trdocc = new CbmTrdOccupancyQa();
    trdocc->SetNeighbourTrigger(true);
    trdocc->SetTriggerThreshold(triggerThreshold);
    trdocc->SetNeighbourReadout(true);
    run->AddTask(trdocc);
    
    CbmTrdHitDensityQa* trdhitdens = new CbmTrdHitDensityQa();
    trdhitdens->SetNeighbourTrigger(false);
    trdhitdens->SetPlotResults(false);
    trdhitdens->SetScaleCentral2mBias(1.0);
    run->AddTask(trdhitdens);
  
}
*/
  // -------------------------------------------------------------------------
  // ===                 End of TRD local reconstruction                   ===
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
