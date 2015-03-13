/**
 * \file global_reco.C
 * \brief Event reconstruction macro.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 * Macro runs event reconstruction in MVD, STS, TRD, MUCH and TOF.
 * Macro has 3 options "all", "hits" and "tracking".
 */

#include <iostream>
using std::cout;
using std::endl;

void global_reco100box(Int_t nEvents = 100, // number of events
	Int_t seed = 555)
{
	gRandom->SetSeed(seed);

	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
   //TString dir = "/Users/andrey/Development/cbm/d/events/sts_tof/"; // Output directory
   TString dir = "data/"; // Output directory
   TString mcFile = dir + "prot.mc.root"; // MC transport file
   TString parFile = dir + "prot.params.root"; // Parameters file
   TString globalRecoFile = dir + "prot.global.reco.root"; // Output file with reconstructed tracks and hits

   // Digi files
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root";

   TString TofGeoPar = "/parameters/tof/par_tof_V13a.txt";  // 6 m version 

   // Directory for output results
   TString resultDir = "./data/prot100box/";

   // Reconstruction parameters
   TString globalTrackingType = "branch"; // Global tracking type
   TString stsHitProducerType = "real"; // STS hit producer type: real, ideal

   // Normalization for efficiency
   Int_t normStsPoints = 4;
   Int_t normTrdPoints = 0;
   Int_t normMuchPoints = 0;
   Int_t normTofPoints = 1;
   Int_t normTrdHits = 0;
   Int_t normMuchHits = 0;
   Int_t normTofHits = 1;

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
		globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
		resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));
	}

	parFileList->Add(&stsDigiFile);

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();
	gSystem->Load("libHadron");

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalRecoFile);

	// ----- STS REAL reconstruction -----------------------------------------------
	// -----   STS digitizer   -------------------------------------------------
	Double_t threshold  =  4;
	Double_t noiseWidth =  0.01;
	Int_t    nofBits    = 12;
	Double_t ElectronsPerAdc    =  10.;
	Double_t StripDeadTime = 0.1;
	CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
	stsDigitize->SetRealisticResponse();
	stsDigitize->SetFrontThreshold (threshold);
	stsDigitize->SetBackThreshold  (threshold);
	stsDigitize->SetFrontNoiseWidth(noiseWidth);
	stsDigitize->SetBackNoiseWidth (noiseWidth);
	stsDigitize->SetFrontNofBits   (nofBits);
	stsDigitize->SetBackNofBits    (nofBits);
	stsDigitize->SetFrontNofElPerAdc  (ElectronsPerAdc);
	stsDigitize->SetBackNofElPerAdc   (ElectronsPerAdc);
	stsDigitize->SetStripDeadTime  (StripDeadTime);
	run->AddTask(stsDigitize);
	// -------------------------------------------------------------------------

	 FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",iVerbose);
	 run->AddTask(stsClusterFinder);

	 FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
	 run->AddTask(stsFindHits);

	 FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
	 run->AddTask(stsMatchHits);


	FairTask* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
        l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
	run->AddTask(l1);
	CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
	run->AddTask(findTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
	run->AddTask(stsMatchTracks);

	// ------------------------------------------------------------------------


	// ------ TOF hits --------------------------------------------------------
	CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
        tofHitProd->SetParFileName(std::string(TofGeoPar));

	run->AddTask(tofHitProd);
	// ------------------------------------------------------------------------

	// ------ Global track reconstruction -------------------------------------
	CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	// Tracking method to be used
	// "branch" - branching tracking
	// "nn" - nearest neighbor tracking
	// "nn_parallel" - nearest neighbor parallel tracking
	finder->SetTrackingType(std::string(globalTrackingType));

	// Hit-to-track merger method to be used
	// "nearest_hit" - assigns nearest hit to the track
	finder->SetMergerType("nearest_hit");

	run->AddTask(finder);
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
        CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
	//run->AddTask(richHitProd);

        CbmRichReconstruction* richReco = new CbmRichReconstruction();
	//run->AddTask(richReco);

        CbmRichMatchRings* matchRings = new CbmRichMatchRings();
	//run->AddTask(matchRings);
        // ------------------------------------------------------------------------


	// -----   Primary vertex finding   ---------------------------------------
	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
	run->AddTask(findVertex);
	// ------------------------------------------------------------------------


	// -----   Track finding QA check   ------------------------------------
	CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
	trackingQa->SetMinNofPointsSts(normStsPoints);
	trackingQa->SetMinNofPointsTrd(normTrdPoints);
	trackingQa->SetMinNofPointsMuch(normMuchPoints);
	trackingQa->SetMinNofPointsTof(normTofPoints);
	trackingQa->SetQuota(0.7);
	trackingQa->SetMinNofHitsTrd(normTrdHits);
	trackingQa->SetMinNofHitsMuch(normMuchHits);
	trackingQa->SetMinNofHitsRich(7);
	trackingQa->SetQuotaRich(0.6);
	trackingQa->SetVerbose(normTofHits);
	trackingQa->SetOutputDir(std::string(resultDir));
	run->AddTask(trackingQa);
	// ------------------------------------------------------------------------

	CbmLitFitQa* fitQa = new CbmLitFitQa();
	fitQa->SetMvdMinNofHits(0);
	fitQa->SetStsMinNofHits(normStsPoints);
	fitQa->SetMuchMinNofHits(normMuchPoints);
	fitQa->SetTrdMinNofHits(normTrdPoints);
	fitQa->SetOutputDir(std::string(resultDir));
	run->AddTask(fitQa);

	CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
	clusteringQa->SetOutputDir(std::string(resultDir));
	run->AddTask(clusteringQa);

	CbmProduceDst *produceDst = new CbmProduceDst(); // in hadron
	run->AddTask(produceDst);

	CbmHadronAnalysis *HadronAna = new CbmHadronAnalysis(); // in hadron
	run->AddTask(HadronAna);

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
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
 	cout << "Macro finished successfully." << endl;
   cout << "Test passed"<< endl;
   cout << " All ok " << endl;
	cout << "Output file is " << globalRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time "	<< timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
