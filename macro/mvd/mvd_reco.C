// --------------------------------------------------------------------------
//
// Macro for local MVD reconstruction from MC data
//
// Tasks:  CbmMvdDigitiser
//         CbmMvdClusterfinder
//         CbmMvdHitfinder
// 
//
// V. Friese   23/04/2009
// Update: P. Sitzmann Juli 2014
// --------------------------------------------------------------------------


void mvd_reco(Int_t  nEvents = 5,
              Int_t  iVerbose = 0)
{

  // ========================================================================
  //          Adjust this part according to your requirements

   
  // Input file (MC events)
  TString inFile = "data/mvd.mc.root";
  
  // Background file (MC events, for pile-up)
  TString bgFile = "data/mvd_bg.mc.root";

  // Output file
  TString outFile = "data/mvd.reco.root";

  // Parameter file
  TString parFile = "data/params.root";
 
  TString mvdGeom = "data/fullgeometry.root";

 
  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  run->SetGeomFile(mvdGeom);

  // ------------------------------------------------------------------------
 
  // -----   MVD Digitiser   ----------------------------------------------
  CbmMvdDigitizer* mvdDigitise = new CbmMvdDigitizer("MVD Digitiser", 0, iVerbose); 
  run->AddTask(mvdDigitise);
  // ----------------------------------------------------------------------

  // -----   MVD Clusterfinder   --------------------------------------------
  CbmMvdClusterfinder* mvdCluster = new CbmMvdClusterfinder("MVD Clusterfinder", 0, iVerbose); 
  run->AddTask(mvdCluster);
  // ----------------------------------------------------------------------

  // -----   MVD Hit Finder   ---------------------------------------------
  CbmMvdHitfinder* mvdHitfinder = new CbmMvdHitfinder("MVD Hit Finder", 0, iVerbose);
  mvdHitfinder->UseClusterfinder(kTRUE);
  run->AddTask(mvdHitfinder);
  // ----------------------------------------------------------------------
  

  // -----   Matching   ---------------------------------------------
  CbmMatchRecoToMC* matcher = new CbmMatchRecoToMC();
  run->AddTask(matcher);
  // ----------------------------------------------------------------------
  


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo = new FairParRootFileIo();
  parIo->open(parFile.Data());
  rtdb->setFirstInput(parIo);
  rtdb->setOutput(parIo);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  // -----   Run initialisation   -------------------------------------------
  run->Init();
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
     
  // -----   Start run   ----------------------------------------------------
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
