// --------------------------------------------------------------------------
//
// Macro for local reconstruction in STS from RAW data
//
// Tasks: CbmStsFindHits
//        CbmStsFindTracks
//        CbmStsFitTracks
/
// V. Friese   12/09/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------


void sts_reco_raw(Int_t nEvents = 1)
{

  // ========================================================================
  //          Adjust this part according to your requirements

   

  // Input file (raw data)
  TString inFile = "sts.raw.root";

  // Output file
  TString outFile = "sts.eds.root";

  // Parameter file
  TString parFile = "params.root";
 
  // Digitisation file
  TString digiFile = "sts_standard.digi.par";

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // ========================================================================
 
  

 
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
  gSystem->Load("libKF");
  gSystem->Load("libL1");
   // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // ---  STS hit finding   -------------------------------------------------
  FairTask* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
  run->AddTask(findHits);
  // ------------------------------------------------------------------------
   
  
   
  // -----   STS track finding   --------------------------------------------
  FairTask* kalman= new CbmKF();
  run->AddTask(kalman);
  FairTask* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
  FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
  run->AddTask(findTracks);
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
  
  
  
  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
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

  
     
  // -----   Intialise and run   --------------------------------------------
  run->LoadGeometry();
  run->Init();
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


}
