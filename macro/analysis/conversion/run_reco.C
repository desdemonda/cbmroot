void run_reco(Int_t nEvents = 2, Int_t mode = 0, Int_t file_nr = 1)
	// mode 1 = tomography
	// mode 2 = urqmd
	// mode 3 = pluto
{
	if(mode == 0) {
		cout << "ERROR: No mode specified! Exiting..." << endl;
		exit();
	}
   TTree::SetMaxTreeSize(90000000000);

	Int_t iVerbose = 0;
	
	Char_t filenr[5];
	sprintf(filenr,"%05d",file_nr);
	printf("Filenr: %s\n", filenr);
	TString temp = filenr;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking

	gRandom->SetSeed(10);

	TString mcFile = "";
	TString parFile = "";
	TString recoFile ="";
	
	
	TString outName = "tomo_0to60deg";
	if(mode == 1) {	// tomography
		mcFile = "/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".mc.0001.root";
		parFile = "/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".param.0001.root";
		recoFile ="/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".reco.0001.root";
	}
	if(mode == 2) {	// urqmd
		mcFile = "/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".mc." + temp + ".root";
		parFile = "/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".param." + temp + ".root";
		recoFile ="/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".reco." + temp + ".root";
	}
	if(mode == 3) {	// pluto
		mcFile = "/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".mc.0001.root";
		parFile = "/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".param.0001.root";
		recoFile ="/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".reco.0001.root";
	}
	
	
	
	std::string resultDir = "/common/home/reinecke/CBM-Simulationen/outputs/pics_reco/";
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
	}

   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13g.digi.par"; // TRD digi file
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

	Double_t threshold  =  4;
	Double_t noiseWidth =  0.01;
	Int_t    nofBits    = 12;
	Double_t electronsPerAdc    =  10;
	Double_t StripDeadTime = 0.1;
	CbmStsDigitize* stsDigitize = new CbmStsDigitize();
	stsDigitize->SetRealisticResponse();
	stsDigitize->SetFrontThreshold (threshold);
	stsDigitize->SetBackThreshold  (threshold);
	stsDigitize->SetFrontNoiseWidth(noiseWidth);
	stsDigitize->SetBackNoiseWidth (noiseWidth);
	stsDigitize->SetFrontNofBits   (nofBits);
	stsDigitize->SetBackNofBits    (nofBits);
	stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
	stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
	stsDigitize->SetStripDeadTime  (StripDeadTime);
	run->AddTask(stsDigitize);

	FairTask* stsClusterFinder = new CbmStsClusterFinder();
	run->AddTask(stsClusterFinder);

	FairTask* stsFindHits = new CbmStsFindHits();
	run->AddTask(stsFindHits);


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
		// Update of the values for the radiator F.U. 17.08.07
		Int_t trdNFoils = 130; // number of polyetylene foils
		Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
		Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
		Bool_t simpleTR = kTRUE; // use fast and simple version for TR production

		//CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils, trdDFoils, trdDGap);
		CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , "H++");
      CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
      run->AddTask(trdHitProd);
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
   //run->AddTask(trackingQa);

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

   //	CbmAnaConversion* conversionAna = new CbmAnaConversion();
   //	conversionAna->SetMode(mode);
   //	run->AddTask(conversionAna);

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

    cout << "Init run" << endl;
    run->Init();
    cout << "Starting run" << endl;
    run->Run(0,nEvents);

    // -----   Finish   -------------------------------------------------------
    cout << "About to finish..." << endl;
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
	
	Int_t analyseMode = mode;
	ofstream outputfile("log.txt", mode = ios_base::app);
	if(!outputfile) {
		cout << "Error!" << endl;
	}
	else {
		TTimeStamp testtime;
		outputfile << "########## run_reco.C ##########" << endl;
		outputfile << "Date (of end): " << testtime.GetDate() << "\t Time (of end): " << testtime.GetTime() << " +2" << endl;
		outputfile << "Output file is "    << recoFile << endl;
		outputfile << "Parameter file is " << parFile << endl;
		outputfile << "Number of events: " << nEvents << "\t mode: " << analyseMode << endl;
		outputfile << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
		outputfile.close();
	}
}
