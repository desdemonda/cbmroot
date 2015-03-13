void run_litqa(Int_t nEvents = 1000)
{
   TTree::SetMaxTreeSize(90000000000);

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//gRandom->SetSeed(10);

    TString dir = "/hera/cbm/users/slebedev/mc/dielectron/sep13/25gev/trd/1.0field/nomvd/rho0/";
	TString mcFile = dir + "mc.auau.25gev.centr.00001.root";
	TString parFile = dir + "/params.auau.25gev.centr.00001.root";
	TString recoFile = dir + "/test.reco.test.auau.25gev.centr.00001.root";
	TString qaFile = dir + "/test.litqa.test.auau.25gev.centr.00001.root";

	TString delta = "no"; // if "yes" Delta electrons will be embedded
	TString deltaFile = "";

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13c_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13p_3e.digi.par"; // TRD digi file
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par"; // TRD digi file

   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v13c.root"; // Material budget file for L1 STS tracking

   TString resultDir = "results_litqa/";
   Double_t trdAnnCut = 0.85;
   Int_t minNofPointsTrd = 6;

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		recoFile = TString(gSystem->Getenv("RECO_FILE"));
		qaFile = TString(gSystem->Getenv("LITQA_FILE"));

		delta = TString(gSystem->Getenv("DELTA"));
		deltaFile = TString(gSystem->Getenv("DELTA_FILE"));

		stsDigiFile = TString(gSystem->Getenv("STS_DIGI"));
		trdDigiFile = TString(gSystem->Getenv("TRD_DIGI"));
		tofDigiFile = TString(gSystem->Getenv("TOF_DIGI"));

		resultDir = TString(gSystem->Getenv("RESULT_DIR"));

		stsMatBudgetFileName = TString(gSystem->Getenv("STS_MATERIAL_BUDGET_FILE"));
		trdAnnCut = TString(gSystem->Getenv("TRD_ANN_CUT")).Atof();
		minNofPointsTrd = TString(gSystem->Getenv("MIN_NOF_POINTS_TRD")).Atof();
	}

   parFileList->Add(&stsDigiFile);
   if (trdDigiFile.GetString() != "") parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);

   TStopwatch timer;
   timer.Start();

	// ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	if (mcFile != "") run->SetInputFile(mcFile);
	if (recoFile != "") run->AddFriend(recoFile);
	if (qaFile != "") run->SetOutputFile(qaFile);

	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
	l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
	run->AddTask(l1);

   // Reconstruction Qa
   CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
   trackingQa->SetMinNofPointsSts(4);
   trackingQa->SetUseConsecutivePointsInSts(true);
   trackingQa->SetMinNofPointsTrd(minNofPointsTrd);
   trackingQa->SetMinNofPointsMuch(10);
   trackingQa->SetMinNofPointsTof(1);
   trackingQa->SetQuota(0.7);
   trackingQa->SetMinNofHitsTrd(minNofPointsTrd);
   trackingQa->SetMinNofHitsMuch(10);
   trackingQa->SetVerbose(0);
   trackingQa->SetMinNofHitsRich(7);
   trackingQa->SetQuotaRich(0.6);
   trackingQa->SetPRange(30, 0., 3.);
   trackingQa->SetOutputDir(std::string(resultDir));
   std::vector<std::string> trackCat, richCat;
   trackCat.push_back("All");
   trackCat.push_back("Electron");
   richCat.push_back("All");
   richCat.push_back("Electron");
   richCat.push_back("ElectronReference");
   trackingQa->SetTrackCategories(trackCat);
   trackingQa->SetRingCategories(richCat);
   trackingQa->SetTrdAnnCut(trdAnnCut);
   run->AddTask(trackingQa);

   CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(0);
   fitQa->SetStsMinNofHits(4);
   fitQa->SetMuchMinNofHits(10);
   fitQa->SetTrdMinNofHits(minNofPointsTrd);
   fitQa->SetPRange(30, 0., 3.);
   fitQa->SetOutputDir(std::string(resultDir));
   run->AddTask(fitQa);

   CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
  // clusteringQa->SetMuchDigiFileName(muchDigiFile.Data());
   clusteringQa->SetOutputDir(std::string(resultDir));
   run->AddTask(clusteringQa);

   CbmLitTofQa* tofQa = new CbmLitTofQa();
   tofQa->SetOutputDir(std::string(resultDir));
   run->AddTask(tofQa);


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
    run->Run(0, nEvents);

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Output file is "    << recoFile << std::endl;
    std::cout << "Parameter file is " << parFile << std::endl;
    std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
    std::cout << " Test passed" << std::endl;
    std::cout << " All ok " << std::endl;
}
