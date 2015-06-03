
void run_reco_geotest(Int_t nEvents = 10)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("SCRIPT"));
   gRandom->SetSeed(10);

   TString outDir = "/Users/slebedev/Development/cbm/data/simulations/rich/geotest/";
   TString inFile = outDir + "mc.0005.root";
   TString parFile = outDir + "param.0005.root";
   TString outFile = outDir + "reco.0005.root";
   std::string resultDir = "";// "results_1/";

   if (script == "yes") {
      inFile = TString(gSystem->Getenv("MC_FILE"));
      outFile = TString(gSystem->Getenv("RECO_FILE"));
      parFile = TString(gSystem->Getenv("PAR_FILE"));
      resultDir = TString(gSystem->Getenv("RICH_GEO_TEST_RESULT_DIR"));
   }

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);

   CbmKF *kalman = new CbmKF();
   run->AddTask(kalman);

	CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
	richHitProd->SetDetectorType(6);
	richHitProd->SetNofNoiseHits(220);
	richHitProd->SetCollectionEfficiency(1.0);
	richHitProd->SetSigmaMirror(0.06);
	richHitProd->SetCrossTalkHitProb(0.0);
	run->AddTask(richHitProd);

	CbmRichReconstruction* richReco = new CbmRichReconstruction();
	richReco->SetRunExtrapolation(false);
	richReco->SetRunProjection(false);
	richReco->SetRunTrackAssign(false);
	richReco->SetFinderName("ideal");
	run->AddTask(richReco);


   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

	CbmMatchRecoToMC* matchRecoToMc = new CbmMatchRecoToMC();
	run->AddTask(matchRecoToMc);

   CbmRichGeoTest* geoTest = new CbmRichGeoTest();
   geoTest->SetOutputDir(resultDir);
   run->AddTask(geoTest);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   run->Init();
   run->Run(0,nEvents);

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished successfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}
