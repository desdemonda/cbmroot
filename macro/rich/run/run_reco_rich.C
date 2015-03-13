void run_reco_rich(Int_t nEvents = 100) {
	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	gRandom->SetSeed(10);

	TString inFile1 = "", inFile2 = "", inFile3 = "", parFile = "", outFile ="";

	Bool_t isRichTrackingOn = false;
	if (script != "yes") {
	   TString inFile1 ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.mc.root";
	   TString inFile2 ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.reco.root";
	   TString parFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.params.root";
	   TString outFile ="/d/cbm02/slebedev/rich/JUL09/auau.25gev.centr.0000.recorich.root";
	} else {
		isRichTrackingOn = true;
		inFile1 = TString(gSystem->Getenv("MCFILE"));
		inFile2 = TString(gSystem->Getenv("RECOFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		outFile = TString(gSystem->Getenv("RECORICHFILE"));
	}

	TString stsDigiFile = "sts_standard.digi.par";

	gDebug = 0;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
    cbmlibs();

	FairRunAna *run = new FairRunAna();
	if (inFile1 != "") run->SetInputFile(inFile2);
	if (inFile2 != "") run->AddFriend(inFile1);
	if (outFile != "") run->SetOutputFile(outFile);

   FairTask* kalman= new CbmKF();
   run->AddTask(kalman);

	// ---------------------RICH Hit Producer ----------------------------------
   CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
   richHitProd->SetDetectorType(4);
   richHitProd->SetNofNoiseHits(220);
   richHitProd->SetCollectionEfficiency(1.0);
   richHitProd->SetSigmaMirror(0.06);
   run->AddTask(richHitProd);

	CbmRichReconstruction* richReco = new CbmRichReconstruction();
	richReco->SetRunExtrapolation(isRichTrackingOn);
	richReco->SetRunProjection(isRichTrackingOn);
   run->AddTask(richReco);

	// ------------------- RICH Ring matching  ---------------------------------
	CbmRichMatchRings* matchRings = new CbmRichMatchRings();
	run->AddTask(matchRings);

	// ===                 End of RICH local reconstruction                  ===
	// =========================================================================

//	CbmLitReconstructionQa* reconstructionQa = new CbmLitReconstructionQa();
//	reconstructionQa->SetMinNofPointsSts(4);
//	reconstructionQa->SetMinNofPointsTrd(9);
//	reconstructionQa->SetMinNofPointsMuch(10);
//	reconstructionQa->SetMinNofPointsTof(1);
//	reconstructionQa->SetQuota(0.7);
//	reconstructionQa->SetMinNofHitsTrd(9);
//	reconstructionQa->SetMinNofHitsMuch(10);
//	reconstructionQa->SetVerbose(0);
//	reconstructionQa->SetMomentumRange(0, 12);
//	reconstructionQa->SetNofBinsMom(12);
//	reconstructionQa->SetMinNofHitsRich(7);
//	reconstructionQa->SetQuotaRich(0.6);
//	reconstructionQa->SetOutputDir("recoIm/");
//	run->AddTask(reconstructionQa);

	// -----  Parameter database   --------------------------------------------
	TString stsDigi = gSystem->Getenv("VMCWORKDIR");
	stsDigi += "/parameters/sts/";
	stsDigi += stsDigiFile;
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(stsDigi.Data(), "in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();

	run->Init();
	cout << "Starting run" << endl;
	run->Run(0, nEvents);

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	cout << " Test passed" << endl;
	cout << " All ok " << endl;
	exit(0);
}
