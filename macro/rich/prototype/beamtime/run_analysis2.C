enum enu_calibMode {
   etn_IMPORT,  // import calibration tables from the file and use them
   etn_ONLINE,  // use first data to calibrate; the channel has to get at least fCalibrationPeriod messages to get calibrated
   etn_NOCALIB, // use linear function going from origin to (512, n) which means that the fine time is not calibrated
   etn_IDEAL,   // use almost linear function - close to real calibration but idealized
   etn_NOFINE   // ignore fine time counter at all
};

void run_analysis2()
{
   TStopwatch timer;
   timer.Start();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   TString outputDir = "NewUnpacker/";
   TString outRootFileName = outputDir + "testtes.root";
   TString runTitle = "NewUnpackerTest";

   TString script = TString(gSystem->Getenv("SCRIPT"));

   if (script == "yes") {
	   hldFullFileName = TString(gSystem->Getenv("INPUT_HLD_FILE"));
	   outRootFileName = TString(gSystem->Getenv("OUTPUT_ROOT_FILE"));
	   outputDir = TString(gSystem->Getenv("OUTPUT_DIR"));
	   runTitle = TString(gSystem->Getenv("RUN_TITLE"));
   }

   // --- Specify number of events to be produced.
   // --- -1 means run until the end of the input file.
   Int_t nEvents = -1;

   // --- Set log output levels
   FairLogger::GetLogger()->SetLogScreenLevel("INFO");
   FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");

   // --- Set debug level
   gDebug = 0;

   CbmRichTrbUnpack2* source = new CbmRichTrbUnpack2();

   // --- Set the input files
   TString hldFileDir = "/mnt/data/WLS/WLS_off/nonstretched/ringH2/offset00250/";
   source->AddInputFile(hldFileDir + "te14326024158.hld");
   source->AddInputFile(hldFileDir + "te14326024342.hld");
   source->AddInputFile(hldFileDir + "te14326024531.hld");
   source->AddInputFile(hldFileDir + "te14326024721.hld");

   CbmTrbCalibrator* fgCalibrator = CbmTrbCalibrator::Instance();
   //fgCalibrator->SetCalibrationPeriod(20000);
   fgCalibrator->SetInputFilename("calibration2.root");            // does not actually import data - only defines
                                                                  // the file that will be used if you specidy mode etn_IMPORT
   fgCalibrator->SetMode(etn_IMPORT);
                                                                  // Also note the (un)commented line in the end of the macro with export func

   // --- Event header
   //FairEventHeader* event = new CbmTbEvent();
   //event->SetRunId(260);

   // --- Run
   FairRunOnline *run = new FairRunOnline(source);
   run->SetOutputFile(outRootFileName);
   //run->SetEventHeader(event);

   CbmTrbEdgeMatcher* matcher = new CbmTrbEdgeMatcher();
   matcher->SetDrawHits();
   run->AddTask(matcher);

   run->Init();

   // --- Start run
   TStopwatch timer;
   timer.Start();
   run->Run(nEvents, 0); // run until end of input file
   timer.Stop();

   // --- export calibration tables
   //fgCalibrator->Export("calibration2.root");

   // You may try to draw the histograms showing which channels are calibrated
   //fgCalibrator->Draw();

   // --- End-of-run info
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   std::cout << std::endl << std::endl;
   std::cout << "Macro finished successfully." << std::endl;
   std::cout << "Output file is " << outRootFileName << std::endl;
   std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << std::endl;
   std::cout << " Test passed" << std::endl;
   std::cout << " All ok " << std::endl;
}

