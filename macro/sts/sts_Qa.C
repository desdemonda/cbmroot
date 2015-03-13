// --------------------------------------------------------------------------
//
// Macro for quality check in STS 
//
// Tasks:  CbmStsFindHitsQa
//         CbmStsReconstructionQa
//
// --------------------------------------------------------------------------

#include <math.h>
void sts_Qa(Int_t nEvents = 1) {

  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Input file (MC events)
  TString inFile = "sts.mc.root";
  
  // Parameter file
  TString parFile = "params.root";
  
  // STS digitization parameter file
  TString digipar = "sts_standard.digi.par";

  // STS reconstruction file
  TString recoFile = "sts.reco.root";

  // STS digitisation file
  TString digiFile = "sts.digi.root";
  
  // Output file
  TString outFile = "sts.Qa.root";

  // Verbosity level (0=quiet, 3=debug)
  Int_t iVerbose = 0;
  // ========================================================================


//   

  // ---   Screen output   --------------------------------------------------  
  cout << "***************************************************" << endl;
  cout << "***   STS QA SCRIPT   *******" << endl;
  cout << "***************************************************" << endl;
  cout << "*** Input file          : " << inFile << endl;
  cout << "*** Parameter file      : " << parFile << endl;
  cout << "*** Digitisation file   : " << digiFile << endl;
  cout << "*** Reconstruction file : " << recoFile << endl;
  cout << "*** Output file         : " << outFile << endl;
  cout << "*** Number of events    : " << nEvents << endl;
  cout << "***************************************************" << endl;
  cout << endl;
 
 // ------------------------------------------------------------------------


        
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
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libGen");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libGlobal");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------

  

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna* run= new FairRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(recoFile);
  run->AddFriend(digiFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
// -----   STS simulation QA   ----------------------------------------
  FairTask* stsSimQa = new CbmStsSimulationQa(kTRUE,iVerbose);
  run->AddTask(stsSimQa);
  // ------------------------------------------------------------------------
  
  // -----   STS hit finding QA   ----------------------------------------
  FairTask* stsFHQa = new CbmStsFindHitsQa(kTRUE,iVerbose);
  run->AddTask(stsFHQa);
  // ------------------------------------------------------------------------
  // -----   STS reconstruction QA   ----------------------------------------
//   FairTask* stsRecoQa = new CbmStsReconstructionQa(kFALSE, 4, 0.7, 3);
//   run->AddTask(stsRecoQa);
  // ------------------------------------------------------------------------

  

  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digipar;
  cout << "digi file = " << stsDigiFile << endl;
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  
  
  // -----   Initialise and run   -------------------------------------------
  run->LoadGeometry();
  run->Init();
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();


  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "         << outFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}


