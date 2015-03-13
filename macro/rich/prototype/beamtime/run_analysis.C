enum enu_calibMode {
   etn_IMPORT,  // import calibration tables from the file and use them
   etn_ONLINE,  // use first data to calibrate; the channel has to get at least fCalibrationPeriod messages to get calibrated
   etn_NOCALIB, // use linear function going from origin to (512, n) which means that the fine time is not calibrated
   etn_IDEAL   // use almost linear function - close to real calibration but idealized
};

enum CbmRichAnaTypeEnum{
	kCbmRichBeamEvent,  // hodoscope ( beam trigger)
	kCbmRichLaserPulserEvent, // Laser pulser events
	kCbmRichLedPulserEvent // UV LED events
};

void run_analysis()
{
   TStopwatch timer;
   timer.Start();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   TString hldFileDir = "";//"/mnt/data/WLS/WLS_off/nonstretched/ringH2/offset00250/";
   TString hldFileName = "/Users/slebedev/Development/cbm/data/cern_beamtime_2014/te14328171010.hld";
   TString hldFullFileName;
   hldFullFileName = hldFileDir + hldFileName;

   TString outRootFileName;

   TString outDir = "";//"/home/pusan/nov2014res/";
   outRootFileName = outDir + hldFileName + ".root";

   TString outputDir = "recoqa/";
   TString runTitle = "123123";

   CbmRichAnaTypeEnum anaType = kCbmRichBeamEvent; // Type of analysis you want to perform: kCbmRichBeamEvent, kCbmRichLaserPulserEvent, kCbmRichLedPulserEvent

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

   CbmRichTrbUnpack* source = new CbmRichTrbUnpack(hldFullFileName);
   source->SetAnaType(anaType);
   source->SetDrawHist(true);
   source->SetUseFingerSci(false);

   CbmTrbCalibrator* fgCalibrator = CbmTrbCalibrator::Instance();
   fgCalibrator->SetInputFilename("calibration.root");            // does not actually import data - only defines
                                                                  // the file that will be used if you specidy mode etn_IMPORT
   fgCalibrator->SetMode(etn_NOCALIB);
                                                                  // Also note the (un)commented line in the end of the macro with export func

   // --- Event header
   //FairEventHeader* event = new CbmTbEvent();
   //event->SetRunId(260);

   // --- Run
   FairRunOnline *run = new FairRunOnline(source);
   run->SetOutputFile(outRootFileName);
   //run->SetEventHeader(event);

   if (anaType == kCbmRichBeamEvent) {
	   CbmRichReconstruction* richReco = new CbmRichReconstruction();
	   richReco->SetFinderName("hough_prototype");
	   richReco->SetRunTrackAssign(false);
	   richReco->SetRunExtrapolation(false);
	   richReco->SetRunProjection(false);
	   richReco->SetRunFitter(false);
	   run->AddTask(richReco);

	   CbmRichTrbRecoQa* qaReco = new CbmRichTrbRecoQa();
	   qaReco->SetMaxNofEventsToDraw(0);
	   qaReco->SetOutputDir(outputDir);
	   qaReco->SetRunTitle(runTitle);
	   qaReco->SetDrawHist(true);
	   run->AddTask(qaReco);
   } else {
	   CbmRichTrbPulserQa* qaPulser = new CbmRichTrbPulserQa();
	   run->AddTask(qaPulser);
   }

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

   // output png is out.png
   // try to rename it here

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

