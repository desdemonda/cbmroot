
void run_reco_geo_opt(Int_t nEvents = 200)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking
   gRandom->SetSeed(10);

   TString outDir = "/Users/slebedev/Development/cbm/data/simulations/rich/urqmdtest/";
   TString mcFile = outDir + "25gev.centr.mc.0000.root";
   TString parFile = outDir + "25gev.centr.param.0000.root";
   TString recFile = outDir + "25gev.centr.reco.0000.root";
   std::string resultDir = "results_urqmd_25gev_centr/";
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TList *parFileList = new TList();
   //TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   //parFileList->Add(&stsDigiFile);

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		recFile = TString(gSystem->Getenv("RECO_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
	}

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(recFile);

   CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
   richHitProd->SetDetectorType(4);
   richHitProd->SetNofNoiseHits(220);
   richHitProd->SetCollectionEfficiency(1.0);
   richHitProd->SetSigmaMirror(0.06);
   run->AddTask(richHitProd);

   CbmRichReconstruction* richReco = new CbmRichReconstruction();
   richReco->SetRunExtrapolation(false);
   richReco->SetRunProjection(false);
   richReco->SetRunTrackAssign(false);
   richReco->SetFinderName("ideal");
  // richReco->SetExtrapolationName("ideal");
   run->AddTask(richReco);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(parFileList, "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();


   run->Init();
   cout << "Starting run" << endl;
   run->Run(0,nEvents);

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished successfully." << endl;
   cout << "Output file is "    << recFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}
