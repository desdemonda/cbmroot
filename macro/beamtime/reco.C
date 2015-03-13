/** @file reco.C
 ** @author Anna Senger <a.senger@gsi.de>
 ** @date 09.12.2014 
 ** Clusters and hits
 */

void reco(Int_t runID, Int_t fileID)
{
  
	TString runN = TString::Itoa(runID,10);
	TString fileN = TString::Itoa(fileID,10);

	// --- Specify input file name (this is just an example)
	
	TString inDir = "data/";
	TString inFile = inDir + "calib.run" + runN + "_" + fileN + ".root";
	
	// --- Specify number of events to be produced.
	// --- -1 means run until the end of the input file.
	Int_t nEvents = -1;

	// --- Specify output file name (this is just an example)
	TString outDir = "data/";
	TString outFile = outDir + "hits.run" + runN + "_" + fileN + ".root";

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
  Bool_t trigger = kTRUE;   // kFALSE - self-trigger; kTRUE - external trigger
  Int_t triggered_station = 2;
  

  // --- Run
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  
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
  
  run->SetWriteRunInfoFile(kFALSE);
  run->Init();
  
  // --- Start run
  TStopwatch timer;
  timer.Start();
  std::cout << ">>> readLmd: Starting run..." << std::endl;
  run->Run(); // run until end of input file
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
