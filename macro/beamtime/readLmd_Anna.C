/** @file readLmd.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since December 2013
 ** @date 25.02.2014
 **
 ** @author Anna Senger <a.senger@gsi.de>
 ** @date 09.12.2014 
 ** ROOT macro to read lmd files from beamtime DEC2013 and convert the data
 ** into cbmroot format.
 ** Uses CbmSourceLmd as source task.
 */

void readLmd_Anna()
{
  
  // TString runN = TString::Itoa(runID,10);
  //  TString fileN = TString::Itoa(fileID,10);
  
  // --- Specify input file name (this is just an example)

  TString inDir = "./lmd/";
  //  TString inFile = inDir + "run" + runN + "_00" + fileN + "?.lmd";
  TString inFile = inDir + "run016_hodo_sts2_0000.lmd";
  TString inFile1 = inDir + "run016_hodo_sts2_0001.lmd";
  TString inFile2 = inDir + "run016_hodo_sts2_0002.lmd";
  TString inFile3 = inDir + "run016_hodo_sts2_0003.lmd";
  TString inFile4 = inDir + "run016_hodo_sts2_0004.lmd";
  TString inFile5 = inDir + "run016_hodo_sts2_0005.lmd";
  TString inFile6 = inDir + "run016_hodo_sts2_0006.lmd";
  TString inFile7 = inDir + "run016_hodo_sts2_0007.lmd";
  TString inFile8 = inDir + "run016_hodo_sts2_0008.lmd";
  TString inFile9 = inDir + "run016_hodo_sts2_0009.lmd";
  TString inFile10 = inDir + "run016_hodo_sts2_00010.lmd";
  TString inFile11 = inDir + "run016_hodo_sts2_00011.lmd";
  
  // --- Specify number of events to be produced.
  // --- -1 means run until the end of the input file.
  Int_t nEvents = -1;
  
  // --- Specify output file name (this is just an example)
  TString outDir = "data/";
  TString outFile = outDir + "run016_hodo_sts2.root";
  
  // --- Set log output levels
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
  
  // --- Set debug level
  gDebug = 0;
  
  std::cout << std::endl;
  std::cout << ">>> readLmd:  input file is " << inFile  << std::endl;
  std::cout << ">>> readLmd: output file is " << outFile << std::endl;

  // ========================================================================
  // ========================================================================

  std::cout << std::endl;
  std::cout << ">>> readLmd: Initialising..." << std::endl;
  
  // --- Sets
  Bool_t trigger = kFALSE;   // kFALSE - self-trigger; kTRUE - external trigger
  Int_t triggered_station = 2;
  
  // --- Source task
  CbmSourceLmd* source = new CbmSourceLmd();
  source->AddFile(inFile);
  source->AddFile(inFile1);
  source->AddFile(inFile2);
  source->AddFile(inFile3);
/*
  source->AddFile(inFile4);
  source->AddFile(inFile5);
  source->AddFile(inFile6);
  source->AddFile(inFile7);
  source->AddFile(inFile8);
  source->AddFile(inFile9);
  source->AddFile(inFile10);
  source->AddFile(inFile11);
*/
  source->SetTriggeredMode(trigger);

  // --- Event header
  FairEventHeader* event = new CbmTbEvent();
  event->SetRunId(012);

  // --- Run
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile(outFile);
  run->SetEventHeader(event);
  
  // --- Calibration  
  CbmStsCosyBL* hist= new CbmStsCosyBL();
  hist->SetTriggeredMode(trigger);
  hist->SetTriggeredStation(triggered_station);
  run->AddTask(hist);

  // --- Clusters
  StsCosyClusterFinder* sts_clust= new StsCosyClusterFinder();
  sts_clust->SetTriggeredMode(trigger);
  sts_clust->SetTriggeredStation(triggered_station);
  run->AddTask(sts_clust);
  
  CbmFiberHodoClusterFinder* hodo= new CbmFiberHodoClusterFinder();
  run->AddTask(hodo);
  
  // --- Hits
  StsCosyHitFinder* hits= new StsCosyHitFinder();
  hits->SetTriggeredMode(trigger);
  hits->SetTriggeredStation(triggered_station);
  run->AddTask(hits);

  // --- Residuals & correlations
  StsCosyTrack* track= new StsCosyTrack();
  Bool_t alignment = kFALSE; // kFALSE - without alignment; kTRUE - with alignment  
  track->SetAlignment(alignment);
//  track->SetChi2Cut(100.,100.);                            
  run->AddTask(track);
   
  run->SetWriteRunInfoFile(kFALSE);
  
  run->Init();
  
  // --- Start run
  TStopwatch timer;
  timer.Start();
  std::cout << ">>> readLmd: Starting run..." << std::endl;
  run->Run(nEvents, 0); // run until end of input file
  timer.Stop();
  
  // --- End-of-run info
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  std::cout << std::endl << std::endl;
  std::cout << ">>> readLmd: Macro finished successfully." << std::endl;
  std::cout << ">>> readLmd: Output file is " << outFile << std::endl;
  std::cout << ">>> readLmd: Real time " << rtime << " s, CPU time "
  					<< ctime << " s" << std::endl;
  std::cout << std::endl;

  /// --- Screen output for automatic tests
  std::cout << " Test passed" << std::endl;
  std::cout << " All ok " << std::endl;
}
