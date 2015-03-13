/**
 * \file mvd_reco.C
 * \brief Macro runs littrack MVD tracking.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/

#include <iostream>
using std::cout;
using std::endl;

void mvd_reco(Int_t nEvents = 100)
{
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   TString dir = "events/mvd/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString mvdDeltaFile = dir + "mc.delta.0000.root"; // Delta files for MVD
   TString mvdRecoFile = dir + "mvd.reco.0000.root"; // Output file with reconstructed tracks and hits

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // Digi scheme for STS
   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v13d.root";

   TString resultDir = "./test/"; // Directory for output results

   Int_t normStsPoints = 4; // STS normalization for efficiency
   TString stsHitProducerType = "real"; // STS hit producer type: real, ideal
   Int_t nofMvdDeltaEvents = 0;

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
		mvdDeltaFile = TString(gSystem->Getenv("LIT_MVD_DELTA_FILE"));
		mvdRecoFile = TString(gSystem->Getenv("LIT_MVD_RECO_FILE"));
		resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));
		stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
		stsMatBudgetFile = TString(gSystem->Getenv("LIT_STS_MAT_BUDGET_FILE"));
		normStsPoints = TString(gSystem->Getenv("LIT_NORM_STS_POINTS")).Atoi();
		nofMvdDeltaEvents = TString(gSystem->Getenv("LIT_NOF_MVD_DELTA_EVENTS")).Atoi();
	}
   parFileList->Add(&stsDigiFile);

   Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(mvdRecoFile);

   if (IsMvd(parFile)) {
      // ----- MVD reconstruction    --------------------------------------------
      CbmMvdDigitizeL* mvdDigi = new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
  	  if (nofMvdDeltaEvents > 0) {
  		  mvdDigi->SetDeltaName(mvdDeltaFile);
  		  mvdDigi->SetDeltaEvents(nofMvdDeltaEvents);
  	  }
      run->AddTask(mvdDigi);

      CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
      run->AddTask(mvdHitFinder);
      // -------------------------------------------------------------------------
   }

   if (stsHitProducerType == "real") {
   // ----- STS REAL reconstruction -----------------------------------------------
      CbmStsDigitize_old* stsDigitize = new CbmStsDigitize_old();
      run->AddTask(stsDigitize);

      FairTask* stsClusterFinder = new CbmStsClusterFinder_old();
      run->AddTask(stsClusterFinder);

      FairTask* stsFindHits = new CbmStsFindHits_old();
      run->AddTask(stsFindHits);

   //   FairTask* stsMatchHits = new CbmStsMatchHits();
   //   run->AddTask(stsMatchHits);

   } else if (stsHitProducerType == "ideal") {
      // ----- STS IDEAL reconstruction   ---------------------------------------------
      FairTask* stsDigitize = new CbmStsIdealDigitize();
      run->AddTask(stsDigitize);

      FairTask* stsClusterFinder = new CbmStsClusterFinder();
      run->AddTask(stsClusterFinder);

      FairTask* stsFindHits = new CbmStsIdealFindHits();
      run->AddTask(stsFindHits);

   //   FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
   //   run->AddTask(stsMatchHits);
   }

	FairTask* kalman = new CbmKF();
	run->AddTask(kalman);
	FairTask* l1 = new CbmL1();
	run->AddTask(l1);
	CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
	//	CbmStsTrackFinder* trackFinder    = new CbmStsTrackFinderIdeal();
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder, false);
	run->AddTask(findTracks);

//	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
//	run->AddTask(stsMatchTracks);

	CbmLitFindMvdTracks* mvdFinder = new CbmLitFindMvdTracks();
	run->AddTask(mvdFinder);

	CbmLitFindGlobalTracks* globalFinder = new CbmLitFindGlobalTracks();
	run->AddTask(globalFinder);

   // -----   Primary vertex finding   ---------------------------------------
   CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
   CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
   run->AddTask(findVertex);
   // ------------------------------------------------------------------------

   CbmMatchRecoToMC* matchTask = new CbmMatchRecoToMC();
   run->AddTask(matchTask);

	// -----   Reconstruction QA tasks   ------------------------------------
	CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
	trackingQa->SetMinNofPointsSts(normStsPoints);
	trackingQa->SetQuota(0.7);
	trackingQa->SetVerbose(1);
	trackingQa->SetOutputDir(std::string(resultDir));
	run->AddTask(trackingQa);

	CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(1);
   fitQa->SetStsMinNofHits(normStsPoints);
	fitQa->SetOutputDir(std::string(resultDir));
   run->AddTask(fitQa);
   // ------------------------------------------------------------------------

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
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << mvdRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
   cout << "Test passed"<< endl;
   cout << " All ok " << endl;
	// ------------------------------------------------------------------------
}

