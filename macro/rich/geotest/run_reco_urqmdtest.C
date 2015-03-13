
void run_reco_urqmdtest(Int_t nEvents = 200)
{
    TTree::SetMaxTreeSize(90000000000);

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString inFile = "/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.00001.root";
	TString outDir = "/Users/slebedev/Development/cbm/data/simulations/rich/urqmdtest/al/";
	TString parFile =  outDir + "25gev.centr.param.al_2.root";
	TString mcFile = outDir + "25gev.centr.mc.al_2.root";
	TString recoFile = outDir + "25gev.centr.reco.al_2.root";

	TString caveGeom = "cave.geo";
	TString pipeGeom   = "pipe/pipe_standard.geo";
	TString magnetGeom = "magnet/magnet_v12a.geo";
	TString stsGeom = "sts/sts_v12b.geo.root";
	TString richGeom= "rich/rich_v13c.root";
	TString fieldMap = "field_v12a";
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale =  1.0; // field scaling factor

	TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
	TObjString trdDigiFile = parDir + "/trd/trd_v13p_3e.digi.par";

	TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking
	std::string resultDir = "results_urqmd_25gev_centr_al_1/";

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		recoFile = TString(gSystem->Getenv("RECO_FILE"));
		caveGeom = TString(gSystem->Getenv("CAVE_GEOM"));
		pipeGeom = TString(gSystem->Getenv("PIPE_GEOM"));
		stsGeom = TString(gSystem->Getenv("STS_GEOM"));
		richGeom = TString(gSystem->Getenv("RICH_GEOM"));
		fieldMap = TString(gSystem->Getenv("FIELD_MAP"));
		magnetGeom = TString(gSystem->Getenv("MAGNET_GEOM"));
		fieldScale = TString(gSystem->Getenv("FIELD_MAP_SCALE")).Atof();
		stsMatBudgetFileName =  parDir + TString(gSystem->Getenv("STS_MATERIAL_BUDGET_FILE_NAME"));
		stsDigiFile =  parDir + TString(gSystem->Getenv("STS_DIGI_FILE"));
		resultDir = std::string(gSystem->Getenv("RESULT_DIR"));
	}

	gRandom->SetSeed(10);

   TList *parFileList = new TList();
   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);

   gDebug = 0;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(recoFile);

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
   Bool_t useMvd = false;
   FairTask* stsFindTracks = new CbmStsFindTracks(0, stsTrackFinder, useMvd);
   run->AddTask(stsFindTracks);

   CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
   FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, 0);
   run->AddTask(stsFitTracks);

   // =========================================================================
   	// ===                     TRD local reconstruction                      ===
   	// =========================================================================
	Bool_t simpleTR = kTRUE; // use fast and simple version for TR production
	CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , "H++");
	CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
	trdHitProd->SetUseDigiPar(false);
	//run->AddTask(trdHitProd);

	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType("branch");
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);

   CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
   richHitProd->SetDetectorType(4);
   richHitProd->SetNofNoiseHits(220);
   richHitProd->SetCollectionEfficiency(1.0);
   richHitProd->SetSigmaMirror(0.06);
   run->AddTask(richHitProd);

   CbmRichReconstruction* richReco = new CbmRichReconstruction();
   richReco->SetRunExtrapolation(true);
   richReco->SetRunProjection(true);
   richReco->SetRunTrackAssign(false);
   richReco->SetFinderName("ideal");
  // richReco->SetExtrapolationName("ideal");
   run->AddTask(richReco);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

   CbmMatchRecoToMC* match = new CbmMatchRecoToMC();
   run->AddTask(match);

   CbmRichUrqmdTest* urqmdTest = new CbmRichUrqmdTest();
   urqmdTest->SetOutputDir(resultDir);
   run->AddTask(urqmdTest);

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
