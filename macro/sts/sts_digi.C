// --------------------------------------------------------------------------
//
// Macro for digitization in STS from MC data
//
// Tasks:  CbmStsDigitise
//
// V. Friese   12/09/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------

#include <math.h>
void sts_digi(Int_t nEvents = 1000) {

  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Input file (MC events)
  TString inFile = "sts.mc.root";
  
  // Parameter file
  TString parFile = "params.root";
  
  // STS digitisation file
  TString digiFile = "sts_v11a.digi.par";
  
  // Output file
  TString outFile = "sts.digi.root";

  // Verbosity level (0=quiet, 3=debug)
  Int_t iVerbose = 0;
  // ========================================================================

  
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.01;
  Int_t    nofBits    = 20;
  Double_t minStep    =  0.01,;
  Double_t StripDeadTime = 0.1;
//   

  // ---   Screen output   --------------------------------------------------  
  cout << "***************************************************" << endl;
  cout << "***   STS DIGITIZATION SCRIPT   *******" << endl;
  cout << "***************************************************" << endl;
  cout << "*** Input file        : " << inFile << endl;
  cout << "*** Parameter file    : " << parFile << endl;
  cout << "*** Digitisation file : " << digiFile << endl;
  cout << "*** Output file       : " << outFile << endl;
  cout << "*** Number of events  : " << nEvents << endl;
  cout << "***************************************************" << endl;
  cout << endl;
  cout << "=============================================" << endl;
  cout << "===     Realistic response settings  ========" << endl;
  cout << "=============================================" << endl;
  cout << "===    threshold  = " << threshold  << endl;
  cout << "===    noiseWidth = " << noiseWidth << endl;
  cout << "===    nofBits    = " << nofBits    << endl;
  cout << "===    minStep    = " << minStep    << endl;
  cout << "===    StripDeadTime    = " << StripDeadTime*100. << "[ns] " << endl;
  cout << "=============================================" << endl;
  cout << endl << endl;
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
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----   OLD MVD Hitproducer   ----------------------------------------------
//   CbmMvdHitProducer* hitProd = new CbmMvdHitProducer("MVDHitProducer", 0, iVerbose);
//   run->AddTask(hitProd);
  // ------------------------------------------------------------------------

  // -----   STS digitiser   ------------------------------------------------
  //  CbmTask* 
  CbmStsDigitize* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
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
  // ------------------------------------------------------------------------
 
  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
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
  cout << "Parameter file is "      << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}


