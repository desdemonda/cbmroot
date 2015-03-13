/** @file readLmd.C
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since December 2013
 ** @date 25.02.2014
 **
 ** ROOT macro to read lmd files from beamtime DEC2013 and convert the data
 ** into cbmroot format.
 ** Uses CbmSourceLmd as source task.
 */


void readLmd()
{

	// --- Specify input file name (this is just an example)
	TString inFile = "lmd/run260_0000.lmd";
	inFile = "/Users/vfriese/Work/beamtimeDEC13/run260_0000.lmd";

	// --- Specify number of events to be produced.
	// --- -1 means run until the end of the input file.
	Int_t nEvents = -1;

	// --- Specify output file name (this is just an example)
	TString outFile = "data/run260_0000.raw.root";

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

  CbmDaqMapCosy2013* daqMap = new CbmDaqMapCosy2013();

  // --- Source task
  CbmSourceLmd* source = new CbmSourceLmd();
  source->SetDaqMap(daqMap);
  source->AddFile(inFile);

  // --- Event header
  FairEventHeader* event = new CbmTbEvent();
  event->SetRunId(260);

  // --- Run
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile(outFile);
  run->SetEventHeader(event);
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
