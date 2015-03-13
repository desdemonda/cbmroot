/** @file readLmd.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since December 2013
 ** @date 25.02.2014
 **
 ** ROOT macro to read lmd files from beamtime DEC2013 and convert the data
 ** into cbmroot format.
 ** Uses CbmSourceLmd as source task.
 */


void readLmdNew()
{

	// --- Specify run
	TString runTag = "run053_0mm_8deg_0004";
 // TString runTag = "run022_0000";

	// --- Specify input file name (this is just an example)
 	TString inFile = "lmd/" + runTag + ".lmd";

	// --- Specify number of events to be produced.
	// --- -1 means run until the end of the input file.
	Int_t nEvents = -1;

	// --- Specify output file name (this is just an example)
	TString outFile = "data/" + runTag + ".root";

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

  // Create unpackers for all messages which will be in the data stream 
  // Up to now there is no unpacker for sync messages so these will be
  // simply dropped by the dummy unpacker
  CbmROCUnpackEpoch* unpackEpoch = new CbmROCUnpackEpoch();
  CbmROCUnpackDummy* unpackDummy = new CbmROCUnpackDummy();
  CbmROCUnpackAux* unpackAux = new CbmROCUnpackAux();
  CbmROCUnpackSystemMessage* unpackSystem = new CbmROCUnpackSystemMessage();
  CbmROCUnpackHodo* unpackHodo = new CbmROCUnpackHodo();
  CbmROCUnpackSts* unpackSts = new CbmROCUnpackSts();

  // Create an event builder
  CbmTbEventBuilderFixedTimeWindow* eventBuilder = new CbmTbEventBuilderFixedTimeWindow();

  // Create the correct mapping for the beamtime
  CbmDaqMapCosy2014* daqMap = new CbmDaqMapCosy2014();


  // --- Source task
  // Register all unpackers, the event builder and the mapping in the source class 
  CbmSourceLmdNew* source = new CbmSourceLmdNew();
  source->AddUnpacker(unpackEpoch, 2);
  source->AddUnpacker(unpackDummy, 3);
  source->AddUnpacker(unpackAux, 4);
  source->AddUnpacker(unpackSystem, 7);
  source->AddUnpacker(unpackSts, 12);
  source->AddUnpacker(unpackHodo, 20);
  source->SetEventBuilder(eventBuilder);
  source->SetDaqMap(daqMap);
  source->AddFile(inFile);

  // --- Event header
  FairEventHeader* event = new CbmTbEvent();
  event->SetRunId(021);

  // --- Run
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile(outFile);
  run->SetEventHeader(event);

  /*
  // --- Calibration
  CbmStsCosyBL* hist= new CbmStsCosyBL();
  run->AddTask(hist);

  // --- Cluster finding for the sts detectors
  StsCosyClusterFinder* sts_clust= new StsCosyClusterFinder();
  run->AddTask(sts_clust);

 // --- Hit finding for the sts
  StsCosyHitFinder* hits= new StsCosyHitFinder();
  run->AddTask(hits);

  */


  // --- Cluster finding for the hodoscopes
  CbmFiberHodoClusterFinder* fhodoClust = new CbmFiberHodoClusterFinder();
  // If one wants to fill the spectra without basline calibrated data the following
  // line has to be uncommented
  //fhodoClust->SetInputDataLevelName("HodoDigi");
  run->AddTask(fhodoClust);  


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
