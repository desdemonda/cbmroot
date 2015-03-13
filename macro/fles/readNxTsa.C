void readNxTsa()
{

  // --- Specify input file name (this is just an example)
  TString inFile = "nxdata.tsa";
  

  // --- Specify number of events to be produced.
  // --- -1 means run until the end of the input file.
  Int_t nEvents = -1;

  // --- Specify output file name (this is just an example)
  TString outFile = "data/nxdata.root";

  // --- Set log output levels
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");

  // --- Set debug level
  gDebug = 0;
  
  std::cout << std::endl;
  std::cout << ">>> readTsa:  input file is " << inFile  << std::endl;
  std::cout << ">>> readTsa: output file is " << outFile << std::endl;

  // ========================================================================
  // ========================================================================

  std::cout << std::endl;
  std::cout << ">>> readTsa: Initialising..." << std::endl;

  // --- Source task
  CbmNxFlibFileSource* source = new CbmNxFlibFileSource();
  source->SetFileName(inFile);

  // --- Event header
//  FairEventHeader* event = new CbmTbEvent();
//  event->SetRunId(260);

  // --- Run
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile(outFile);
//  run->SetEventHeader(event);
  run->Init();

  
  // --- Start run
  TStopwatch timer;
  timer.Start();
  std::cout << ">>> readNxTsa: Starting run..." << std::endl;
  run->Run(nEvents, 0); // run until end of input file
  timer.Stop();
  
  // --- End-of-run info
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  std::cout << std::endl << std::endl;
  std::cout << ">>> readTsa: Macro finished successfully." << std::endl;
  std::cout << ">>> readTsa: Output file is " << outFile << std::endl;
  std::cout << ">>> readTsa: Real time " << rtime << " s, CPU time "
  					<< ctime << " s" << std::endl;
  std::cout << std::endl;

  /// --- Screen output for automatic tests
  std::cout << " Test passed" << std::endl;
  std::cout << " All ok " << std::endl;
}
