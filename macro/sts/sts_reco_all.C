// --------------------------------------------------------------------------
//
// Macro for reconstruction in STS from MC data
//
// Tasks:  CbmStsDigitise
//         CbmStsFindHits
//         CbmStsMatchHits
//         CbmStsFindTracks
//         CbmStsMatchTracks
//         CbmStsFitTracks
//         CbmStsReconstructionQa
//
// V. Friese   12/09/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------

#include <math.h>
void sts_reco_all(Int_t nEvents, Float_t energy, Int_t index) {

  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Input file (MC events)
//  TString inFile = "sts.mc.25gev.1000.root";
  
  // Parameter file
//  TString parFile = "params.25gev.1000.root";
  
  // STS digitisation file
  TList *parFileList = new TList();

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";

  TObjString stsDigiFile = paramDir + "/sts/sts_v12b_std.digi.par";
  parFileList->Add(&stsDigiFile);
  
  // Output file
//  TString outFile = "sts.reco.25gev.1000.root";

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // ========================================================================

  
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.1;
  Int_t    nofBits    = 12;
  Double_t electronsPerAdc    =  10;
  Double_t StripDeadTime = 10.;
  




        
  // ---  ROOT settings   ---------------------------------------------------
  gStyle->SetPalette(1);  
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
  gSystem->Load("libLittrackparallel");
  gSystem->Load("libLittrack");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libGlobal");
  gSystem->Load("libL1");
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter

  // ------------------------------------------------------------------------
  char strInputFile[1000];
  char strParamFile[1000];
  char strOutputFile[1000];

  sprintf(strInputFile, "data/auau%1.0f/sts.mc.auau.%1.0fgev.centr.%4d.root",
	  energy, energy, index);
  sprintf(strParamFile, "data/auau%1.0f/params.auau.%1.0fgev.centr.%4d.root",
	  energy, energy, index);
  sprintf(strOutputFile, "data/auau%1.0f/sts.reco.auau.%1.0fgev.centr.%4d.root",
	  energy, energy, index);
//sprintf(strOutputFile, "test.root",
//	  energy, energy, index);
  
  for(Int_t i = 0; i < 1000; i++) {
    if(' ' == strInputFile[i]) strInputFile[i] = '0';
    if(' ' == strParamFile[i]) strParamFile[i] = '0';
    if(' ' == strOutputFile[i]) strOutputFile[i] = '0';
  }
  
  // ---   Screen output   --------------------------------------------------  
  cout << "***************************************************" << endl;
  cout << "***   STS REALISTIC RECONSTRUCTION SCRIPT   *******" << endl;
  cout << "***************************************************" << endl;
  cout << "*** Input file        : " << strInputFile << endl;
  cout << "*** Parameter file    : " << strParamFile << endl;
  cout << "*** Output file       : " << strOutputFile << endl;
  cout << "*** Number of events  : " << nEvents << endl;
  cout << "***************************************************" << endl;
  cout << endl;
  cout << "=============================================" << endl;
  cout << "===     Realistic response settings  ========" << endl;
  cout << "=============================================" << endl;
  cout << "===    threshold  = " << threshold  << endl;
  cout << "===    noiseWidth = " << noiseWidth << endl;
  cout << "===    nofBits    = " << nofBits    << endl;
  cout << "===    electronsPerAdc    = " << electronsPerAdc    << endl;
  cout << "===    StripDeadTime    = " << StripDeadTime*100. << "[ns] " << endl;
  cout << "=============================================" << endl;
  cout << endl << endl;
 // ------------------------------------------------------------------------
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(strInputFile);
  run->SetOutputFile(strOutputFile);
  
  // ------------------------------------------------------------------------

  // -----   STS digitiser   ------------------------------------------------
  CbmStsDigitize* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
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

  // -----   STS cluster finding   ------------------------------------------------
  CbmStsClusterFinder* findClusters = new CbmStsClusterFinder("STSFindClusters", iVerbose);
  run->AddTask(findClusters);

  
  // ---  STS hit finding   -------------------------------------------------
  CbmStsFindHits* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
  run->AddTask(findHits);

  
  // ---  STS hit matching   ------------------------------------------------
  CbmStsMatchHits* matchHits = new CbmStsMatchHits("STSMatchHits",iVerbose);
  matchHits->SetRealisticResponse();
  run->AddTask(matchHits);
  // ------------------------------------------------------------------------
  
  
  // -----   STS track finding   --------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1("L1",1,3,0);
  run->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
  run->AddTask(stsFindTracks);
  // ------------------------------------------------------------------------

  
         
  // -----   STS track matching   -------------------------------------------
  FairTask* matchTracks = new CbmStsMatchTracks("Match tracks", iVerbose);
  run->AddTask(matchTracks);
  // ------------------------------------------------------------------------
  

  
  // -----   STS track fitting   --------------------------------------------
  CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
  FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter",
					   trackFitter,
					   iVerbose);
  run->AddTask(fitTracks);
  // ------------------------------------------------------------------------
  
  
  // -----   STS simulation QA   ----------------------------------------
//  FairTask* stsSimQa = new CbmStsSimulationQa(kTRUE,iVerbose);
//  run->AddTask(stsSimQa);
  // ------------------------------------------------------------------------
  
  // -----   STS hit finding QA   ----------------------------------------
  FairTask* stsFHQa = new CbmStsFindHitsQa(kTRUE,iVerbose);
  run->AddTask(stsFHQa);
  // ------------------------------------------------------------------------

  // -----   STS reconstruction QA   ----------------------------------------
  FairTask* stsRecoQa = new CbmStsReconstructionQa(kTRUE, 4, 0.7, 0);
  run->AddTask(stsRecoQa);
  // ------------------------------------------------------------------------

  

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

  
  
  // -----   Initialise and run   -------------------------------------------
  run->Init();
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();


  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "         << strOutputFile << endl;
  cout << "Parameter file is "      << strParamFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}


