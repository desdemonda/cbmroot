// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events
//
// All hitproducers (STS, RICH, TRD, TOF, ECAL), Fast MC for ECAL
// STS track finding and fitting
// TRD track finding and fitting
// RICH ring finding (ideal) and fitting
// Global track finding (ideal), rich assignment
// Primary vertex finding (ideal)
// Matching of reconstructed and MC tracks in STS, RICH and TRD
//
// V. Friese   24/02/2006
//
// --------------------------------------------------------------------------


{
 
  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)

  //TString File   = "/d/cbm01/diego/CBM/JUN06/SIMUL/AUAU_CENT_25GEV/bor";
  //TString File   = "/d/cbm01/diego/CBM/FEB07/SIMUL/AUAU_CENT_25GEV/bor_mono";
  TString File   = "/misc/diego/cbmroot/macro/run/bor";
  TString inFile = File + ".root";

  // Number of events to process
  Int_t nEvents = 10;

  // Background file (for MAPS pileup)
  //TString bgFile = "auau.25gev.mbias.mc.root";

  // Parameter file
  TString parFile = "params.root";

  // Output file
  // TString outFile = File +"_reco_new_HITPROD.root";
  TString outFile = "/misc/diego/cbmroot/macro/run/bor_reco.root";

  // Version of STS digitisation scheme
  TString stsDigiVersion = "v05a";

  // Specify MAPS digitisation parameters
  Int_t    iMapsF        = 1;           // First MAPS station
  Int_t    iMapsL        = 2;           // Last MAPS station
  Double_t mapsSigmaX    = 0.0005;      // resolution in x [cm]
  Double_t mapsSigmaY    = 0.0005;      // resolution in y [cm]
  Double_t mapsEff       = 0.99;        // single hit efficiency
  Double_t mapsFakeRate  = 0.03;        // fake hit rate 
  Double_t mapsMergeDist = 0.00;        // distance for hit merging [cm]
  Int_t    mapsPileUp    = 1;           // number of pileup events
  
  // Specify Hybrid digitisation parameters
  Int_t    iHybF   = 3;           // First hybrid station
  Int_t    iHybL   = 4;           // Last hybrid station
  Double_t hybLx   = 0.0050;      // Pixel x size [cm]
  Double_t hybLy   = 0.0050;      // Pixel y size [cm]

  // Specify Strip digitisation parameters
  Int_t iStripF = 5;           // First strip station
  Int_t iStripL = 8;           // Last strip station

  // Specify parameters for RICH track extrapolation
  Int_t    richNSts = 6;     // minimum number of STS hits for extrapolation
  Double_t richZPos = 300.;  // z position for extrapolation [cm]

  // Specify parameters for RICH projektion to PM plane
  Int_t richZFlag = 1;       // Projetion from IM plane (default)
  
 // Specify parameters for RICH hit producer
  Double_t richPmtRad  = 0.4;     // PMT radius [cm]
  Double_t richPmtDist = 0.;      // Distance between PMTs [cm]
  Int_t    richDetType = 1;       // Detector type Protvino
  Int_t    richNoise   = 220;     // Number of noise points per event
    
  // Specify TRD digitisation parameters
  Double_t trdSigmaX[] = {300, 400, 500}; // Resolution in x [mum]
  // Resolutions in y - station and angle dependent [mum]
  Double_t trdSigmaY1[] = {2700,   3700, 15000, 27600, 33000, 33000, 33000 };
  Double_t trdSigmaY2[] = {6300,   8300, 33000, 33000, 33000, 33000, 33000 };
  Double_t trdSigmaY3[] = {10300, 15000, 33000, 33000, 33000, 33000, 33000 };  
  Int_t trdNFoils    = 100;   // number of polyetylene foils,  default = 100
  Float_t trdDFoils = 0.001; // thickness of 1 foil [cm], default = 0.001
  Float_t trdDGap   = 0.008; // thickness of gap between foils [cm], default = 0.008
 
  // Specify parameters for TOF digitisation
  Double_t tofSigmaT  = 0.08; //80 ps
  Double_t tofSigmaY  = 0.7;  //7 mm
  Double_t tofSigmaXY = 0.5;  //5 mm (old hit producer)

  // Specify parameters for RICH ring-track assignment
  Double_t richDistance = 1.; // Maximum distance between ring centre and track [cm]
  Int_t    richNPoints  = 5;  // Minmum number of hits on ring

  
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------


  // ---  Now choose concrete engines for the different tasks   -------------


  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/sts_digi_new_standard.par";
  parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----   TOF hit producer   ---------------------------------------------
  CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer",
  						iVerbose);
  tofHitProd->SetSigmaT(tofSigmaT);
  //tofHitProd->SetSigmaY(tofSigmaY);
  tofHitProd->SetSigmaXY(tofSigmaXY);
  
  fRun->AddTask(tofHitProd);
  // ------------------------------------------------------------------------
  
  // =====                 End of HitProducers                           =====
  // =========================================================================
 
  
     
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  
  fRun->Run(0,nEvents);
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


}
