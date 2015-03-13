void run_reco(Int_t nEvents = 1000)
{
   TTree::SetMaxTreeSize(90000000000);

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//gRandom->SetSeed(10);

  /* TString parFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.param.root";
   TString mcFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.mc.root";
   TString recoFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.reco.root";     */

       TString dir = "/hera/cbm/users/slebedev/mc/dielectron/feb15/3.5gev/stsv13d/richv14a_1e/notrd/tofv13/1.0field/nomvd/rho0/";
	TString mcFile = dir + "/mc.auau.3.5gev.centr.00134.root";
	TString parFile = dir + "/params.auau.3.5gev.centr.00134.root";
	TString recoFile = dir + "/reco.auau.3.5gev.centr.00134.root";
	//  TString analysisFile = dir + "/test.analysis.test.auau.25gev.centr.00001.root";

	

	TString delta = "no"; // if "yes" Delta electrons will be embedded
	TString deltaFile = "";

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v14a_1e.digi.par"; // TRD digi file
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par"; // TOF digi file

   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v13d.root"; // Material budget file for L1 STS tracking

   TString resultDir = "recqa/";
   Double_t trdAnnCut = 0.85;
   Int_t minNofPointsTrd = 2;
   TString tofHitProducerType = "v13"; //v07 or v13
   TString trdHitProducerType = "smearing"; //"digi" or "smearing" or "clustering"

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		recoFile = TString(gSystem->Getenv("RECO_FILE"));
		delta = TString(gSystem->Getenv("DELTA"));
		deltaFile = TString(gSystem->Getenv("DELTA_FILE"));

		stsDigiFile = TString(gSystem->Getenv("STS_DIGI"));
		trdDigiFile = TString(gSystem->Getenv("TRD_DIGI"));
		tofDigiFile = TString(gSystem->Getenv("TOF_DIGI"));

		tofHitProducerType = TString(gSystem->Getenv("TOF_HIT_PRODUCER_TYPE"));
		trdHitProducerType = TString(gSystem->Getenv("TRD_HIT_PRODUCER_TYPE"));

		resultDir = TString(gSystem->Getenv("RESULT_DIR"));

		stsMatBudgetFileName = TString(gSystem->Getenv("STS_MATERIAL_BUDGET_FILE"));
		trdAnnCut = TString(gSystem->Getenv("TRD_ANN_CUT")).Atof();
      minNofPointsTrd = TString(gSystem->Getenv("MIN_NOF_POINTS_TRD")).Atof();
	}
       

   parFileList->Add(&stsDigiFile);
   if (trdDigiFile.GetString() != "") parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);

   cout << "mcFile=" << mcFile << endl;
   cout << "parFile=" << parFile << endl;
   cout << "recoFile=" << recoFile << endl;
  // cout << "trdDigiFile=" << trdDigiFile->GetString() << endl;
   cout << "trdSmearingType=" << trdHitProducerType << endl;

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
	Bool_t useMvdInTracking = kFALSE;
	if (IsMvd(parFile)) {
	   useMvdInTracking = kTRUE;
      CbmMvdDigitizeL* mvdDigitizeL = new CbmMvdDigitizeL("CbmMvdDigitizeL", 0, 1);
      mvdDigitizeL->SetPixelSize(18.4); //should be 20x20
      mvdDigitizeL->SetEpiThickness(0.0014);
      //mvdDigitizeL->ShowDebugHistograms();
      //mvdDigitizeL->SetBgFileName(bgFile);
      //mvdDigitizeL->SetPileUp(0);
      if (delta == "yes"){
         mvdDigitizeL->SetDeltaName(deltaFile);
         mvdDigitizeL->SetDeltaEvents(1000); //1/1000 target
      }
      run->AddTask(mvdDigitizeL);

      CbmMvdFindHits* mvdFindHits = new CbmMvdFindHits("CbmMvdFindHits", 0, 1);
      mvdFindHits->SetSigmaNoise(11,kTRUE);
      //mvdFindHits->ShowDebugHistograms();
      mvdFindHits->SetAdcDynamic(150);
      mvdFindHits->SetAdcOffset(0);
      mvdFindHits->SetAdcBits(1);
      mvdFindHits->SetSeedThreshold(1); //75 ele
      mvdFindHits->SetNeighbourThreshold(1); //75 el
      run->AddTask(mvdFindHits);

      // MVD ideal
      //{
      //CbmMvdHitProducer* mvdHitProd = new CbmMvdHitProducer("MVDHitProducer", 0, 1);
      //run->AddTask(mvdHitProd);
      //}
	}

	// =========================================================================
	// ===                      STS local reconstruction                     ===
	// =========================================================================
	Double_t threshold  =  4;
	Double_t noiseWidth =  0.1;
	Int_t    nofBits    = 20;
	Double_t minStep    =  0.01;
	Double_t StripDeadTime = 10.;

	Double_t threshold  =  4;
	Double_t noiseWidth =  0.01;
	Int_t    nofBits    = 12;
	Double_t electronsPerAdc    =  10;
	Double_t StripDeadTime = 0.1;
	CbmStsDigitize_old* stsDigitize = new CbmStsDigitize_old();
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

	FairTask* stsClusterFinder = new CbmStsClusterFinder_old();
	run->AddTask(stsClusterFinder);

	FairTask* stsFindHits = new CbmStsFindHits_old();
	run->AddTask(stsFindHits);

	// CbmStsMatchHits* stsMatchHits = new CbmStsMatchHits();
	// run->AddTask(stsMatchHits);


	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
	l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
//	CbmL1* l1 = new CbmL1("CbmL1",1, 3); //to fill L1 histo
	run->AddTask(l1);

	CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
	//Bool_t useMvdInTracking = kTRUE;
	FairTask* stsFindTracks = new CbmStsFindTracks(1, stsTrackFinder, useMvdInTracking);
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

    // CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils, trdDFoils, trdDGap);
      CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , "H++");

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
		if (tofHitProducerType == "v07") {
			CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("CbmTofHitProducer", 1);
			run->AddTask(tofHitProd);
		} else if (tofHitProducerType == "v13"){
			CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("CbmTofHitProducer", 1);
			tofHitProd->SetInitFromAscii(kFALSE);
			run->AddTask(tofHitProd);
		}
	} //isTof

	// =========================================================================
	// ===                        Global tracking                            ===
	// =========================================================================

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType("branch");
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);

	// -----   Primary vertex finding   ---------------------------------------
	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
	run->AddTask(findVertex);
	// ------------------------------------------------------------------------

	if (IsTrd(parFile)) {
		CbmTrdSetTracksPidANN* trdSetTracksPidAnnTask = new CbmTrdSetTracksPidANN("CbmTrdSetTracksPidANN","CbmTrdSetTracksPidANN");
      trdSetTracksPidAnnTask->SetTRDGeometryType("h++");
		run->AddTask(trdSetTracksPidAnnTask);
	}//isTrd

    // =========================================================================
    // ===                        RICH reconstruction                        ===
    // =========================================================================
	if (IsRich(parFile)){
      CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
      richHitProd->SetDetectorType(4);
      richHitProd->SetNofNoiseHits(220);
      richHitProd->SetCollectionEfficiency(1.0);
      richHitProd->SetSigmaMirror(0.06);
      run->AddTask(richHitProd);

      CbmRichReconstruction* richReco = new CbmRichReconstruction();
      //richReco->SetFinderName("ideal");
      run->AddTask(richReco);

      // ------------------- RICH Ring matching  ---------------------------------
      CbmRichMatchRings* matchRings = new CbmRichMatchRings();
      run->AddTask(matchRings);

	}//isRich

	CbmMatchRecoToMC* match = new CbmMatchRecoToMC();
	run->AddTask(match);

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
