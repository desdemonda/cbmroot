void run_reco(Int_t nEvents = 5)
{
   TTree::SetMaxTreeSize(90000000000);

	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
    TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v13d.root"; // Material budget file for L1 STS tracking

	gRandom->SetSeed(10);

	TString mcFile = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/mc.0001.root";
	TString parFile = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/param.0001.root";
	TString recoFile ="/Users/slebedev/Development/cbm/data/simulations/rich/richreco/reco.0001.root";
	std::string resultDir = "recqa_0001/";
	TString trdHitProducerType = "smearing";
	int nofNoiseHitsInRich = 220;
	double collectionEff = 1.0;
	double sigmaErrorRich = 0.06;
	double crosstalkRich = 0.02;
	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		recoFile = TString(gSystem->Getenv("RECO_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));
		nofNoiseHitsInRich = TString(gSystem->Getenv("NOF_NOISE_HITS_IN_RICH")).Atoi();
		collectionEff = TString(gSystem->Getenv("RICH_COLLECTION_EFF")).Atof();
        sigmaErrorRich = TString(gSystem->Getenv("SIGMA_ERROR_RICH")).Atof();
        crosstalkRich = TString(gSystem->Getenv("CROSSTALK_RICH")).Atof();
        trdHitProducerType = TString(gSystem->Getenv("TRD_HIT_PRODUCER_TYPE"));
	}

   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v14a_3e.digi.par"; // TRD digi file
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par"; // TOF digi file

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);
   gDebug = 0;

    TStopwatch timer;
    timer.Start();

	// ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	if (mcFile != "") run->SetInputFile(mcFile);
	if (recoFile != "") run->SetOutputFile(recoFile);


    // =========================================================================
    // ===                     MVD local reconstruction                      ===
	// =========================================================================
	if (IsMvd(parFile)) {
	CbmMvdDigitizeL* mvdDigitizeL = new CbmMvdDigitizeL("MVD Digitizer", 0, iVerbose);
    run->AddTask(mvdDigitizeL);

    CbmMvdFindHits* mvdFindHits = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
    run->AddTask(mvdFindHits);

    // MVD ideal
    //{
    //CbmMvdHitProducer* mvdHitProd = new CbmMvdHitProducer("MVDHitProducer", 0, iVerbose);
    //run->AddTask(mvdHitProd);
    //}
	}

	// =========================================================================
	// ===                      STS local reconstruction                     ===
	// =========================================================================

	Double_t dynRange       =   40960.;  // Dynamic range [e]
	Double_t threshold      =    4000.;  // Digitisation threshold [e]
	Int_t nAdc              =    4096;   // Number of ADC channels (12 bit)
	Double_t timeResolution =       5.;  // time resolution [ns]
	Double_t deadTime       = 9999999.;  // infinite dead time (integrate entire event)
	Double_t noise          =       0.;  // ENC [e]
	Int_t digiModel         = 1;  // Model: 1 = uniform charge distribution along track

	CbmStsDigitize* stsDigi = new CbmStsDigitize(digiModel);
	stsDigi->SetParameters(dynRange, threshold, nAdc, timeResolution, deadTime, noise);
	run->AddTask(stsDigi);

	FairTask* stsCluster = new CbmStsFindClusters();
	run->AddTask(stsCluster);

	FairTask* stsHit = new CbmStsFindHits();
	run->AddTask(stsHit);


	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
    l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
	run->AddTask(l1);

	CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
	Bool_t useMvd = kTRUE;
	FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder, useMvd);
	run->AddTask(stsFindTracks);

	// =========================================================================
	// ===                     TRD local reconstruction                      ===
	// =========================================================================
	if (IsTrd(parFile)) {
		CbmTrdRadiator *radiator = new CbmTrdRadiator(kTRUE , "H++");

		if (trdHitProducerType == "smearing") {
			CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
			trdHitProd->SetUseDigiPar(false);
			run->AddTask(trdHitProd);
		} else if (trdHitProducerType == "digi") {
			CbmTrdDigitizer* trdDigitizer = new CbmTrdDigitizer(radiator);
			run->AddTask(trdDigitizer);

			CbmTrdHitProducerDigi* trdHitProd = new CbmTrdHitProducerDigi();
			run->AddTask(trdHitProd);
		} else if (trdHitProducerType == "clustering") {
			CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
			run->AddTask(trdDigiPrf);

			CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
			run->AddTask(trdCluster);

			CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
			run->AddTask(trdHit);
		}
	}// isTRD

	// =========================================================================
	// ===                     TOF local reconstruction                      ===
	// =========================================================================
	if (IsTof(parFile)) {
		// ------   TOF hit producer   ---------------------------------------------
		//CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("CbmTofHitProducer", iVerbose);
		CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("TOF HitProducerNew",iVerbose);
		tofHitProd->SetInitFromAscii(kFALSE);
		run->AddTask(tofHitProd);
	} //isTof

	// =========================================================================
	// ===                        Global tracking                            ===
	// =========================================================================

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType(std::string("branch"));
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);

	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
	run->AddTask(findVertex);


	if (IsTrd(parFile)) {
		CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new CbmTrdSetTracksPidANN("CbmTrdSetTracksPidANN","CbmTrdSetTracksPidANN");
		trdSetTracksPidAnnTask->SetTRDGeometryType("h++");
		run->AddTask(trdSetTracksPidAnnTask);

		//CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =
		//		new CbmTrdSetTracksPidLike("CbmTrdSetTracksPidLike","CbmTrdSetTracksPidLike");
		//run->AddTask(trdSetTracksPidLikeTask);
	}//isTrd

    // =========================================================================
    // ===                        RICH reconstruction                        ===
    // =========================================================================
	if (IsRich(parFile)){
		CbmRichHitProducer* richHitProd	= new CbmRichHitProducer();
		richHitProd->SetDetectorType(4);
		richHitProd->SetNofNoiseHits(nofNoiseHitsInRich);
		richHitProd->SetCollectionEfficiency(collectionEff);
		richHitProd->SetSigmaMirror(sigmaErrorRich);
		richHitProd->SetCrossTalkHitProb(crosstalkRich);
		run->AddTask(richHitProd);

		CbmRichReconstruction* richReco = new CbmRichReconstruction();
		run->AddTask(richReco);

		CbmRichMatchRings* matchRings = new CbmRichMatchRings();
		run->AddTask(matchRings);
	}//isRich


	CbmMatchRecoToMC* matchRecoToMc = new CbmMatchRecoToMC();
	run->AddTask(matchRecoToMc);

	// Reconstruction Qa
   CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
   trackingQa->SetMinNofPointsSts(4);
   trackingQa->SetUseConsecutivePointsInSts(true);
   trackingQa->SetMinNofPointsTrd(8);
   trackingQa->SetMinNofPointsMuch(10);
   trackingQa->SetMinNofPointsTof(1);
   trackingQa->SetQuota(0.7);
   trackingQa->SetMinNofHitsTrd(8);
   trackingQa->SetMinNofHitsMuch(10);
   trackingQa->SetVerbose(0);
   trackingQa->SetMinNofHitsRich(7);
   trackingQa->SetQuotaRich(0.6);
   trackingQa->SetOutputDir(resultDir);
   trackingQa->SetPRange(20, 0., 10.);

   std::vector<std::string> trackCat, richCat;
   trackCat.push_back("All");
   trackCat.push_back("Electron");
   richCat.push_back("Electron");
   richCat.push_back("ElectronReference");
   trackingQa->SetTrackCategories(trackCat);
   trackingQa->SetRingCategories(richCat);
   run->AddTask(trackingQa);

   CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(0);
   fitQa->SetStsMinNofHits(4);
   fitQa->SetMuchMinNofHits(10);
   fitQa->SetTrdMinNofHits(6);
   fitQa->SetOutputDir(resultDir);
  // run->AddTask(fitQa);

   CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
   clusteringQa->SetOutputDir(resultDir);
  // run->AddTask(clusteringQa);


    // =========================================================================
    // ===                        ECAL reconstruction                        ===
    // =========================================================================

//  // -----   ECAL hit producer  ----------------------------------------------
//  CbmEcalHitProducerFastMC* ecalHitProd
//    = new CbmEcalHitProducerFastMC("ECAL Hitproducer");
//  run->AddTask(ecalHitProd);

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
    cout << "Output file is "    << recoFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
}
