void trd_elid_reco(Int_t nEvents = 200)
{
   Int_t trdNFoils = 70;
   Float_t trdDFoils = 0.0014;
   Float_t trdDGap = 0.04;
   Bool_t simpleTR = kTRUE;
   TString radiatorType = "K++";

   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString inFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.mc.root";
	TString parFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.params.root";
	TString outFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.reco.root";

   TList *parFileList = new TList();
   //TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v14a_3e.digi.par"; // TRD digi file
   parFileList->Add(&trdDigiFile);

	gDebug = 0;

	// ----  Load libraries   -------------------------------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(inFile);
	// run->AddFile(inFile2);
	run->SetOutputFile(outFile);

	//CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils, trdDFoils, trdDGap);
	CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , "K++");
	TString trdHitProducerType = "clustering";
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
	/*	CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
		run->AddTask(trdDigiPrf);

		CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
		run->AddTask(trdCluster);

		CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
		run->AddTask(trdHit);*/

		Bool_t triangularPads = false;// Bucharest triangular pad-plane layout
		//Double_t triggerThreshold = 0.5e-6;//SIS100
		Double_t triggerThreshold = 1.0e-6;//SIS300
		//Double_t triggerThreshold = 3.0e-7;//0.5cm homogeniuse pad height
		//Double_t trdNoiseSigma_keV = 0.1;

		CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
		trdDigiPrf->SetTriangularPads(triangularPads);
		//trdDigiPrf->SetNoiseLevel(trdNoiseSigma_keV);
		//trdDigiPrf->SetNCluster(1); // limits the number of iterations per MC-point to one
		trdDigiPrf->SetPadPlaneScanArea(3,3); // limits the number of channels participating to the charge sharing (col,row)
		run->AddTask(trdDigiPrf);

		CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
		trdCluster->SetNeighbourTrigger(true);
		trdCluster->SetTriggerThreshold(triggerThreshold);
		trdCluster->SetNeighbourRowTrigger(false);
		trdCluster->SetPrimaryClusterRowMerger(true);
		trdCluster->SetTriangularPads(triangularPads);
		run->AddTask(trdCluster);

		CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
		trdHit->SetTriangularPads(triangularPads);
		run->AddTask(trdHit);
	}

	CbmMatchRecoToMC* matchRecoToMc = new CbmMatchRecoToMC();
	run->AddTask(matchRecoToMc);

	CbmTrdTrackFinder* trdTrackFinder = new CbmTrdTrackFinderIdeal();
	CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("CbmTrdFindTracks");
	trdFindTracks->UseFinder(trdTrackFinder);
	run->AddTask(trdFindTracks);

	CbmMatchRecoToMC* matchRecoToMc = new CbmMatchRecoToMC();
	run->AddTask(matchRecoToMc);

	CbmTrdElectronsTrainAnn* elAnn = new CbmTrdElectronsTrainAnn(10);
	run->AddTask(elAnn);

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
}

