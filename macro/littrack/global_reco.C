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

void global_reco(Int_t nEvents = 5, // number of events
		TString opt = "all")
// if opt == "all" STS + hit producers + global tracking are executed
// if opt == "hits" STS + hit producers are executed
// if opt == "tracking" global tracking is executed
{
   TTree::SetMaxTreeSize(90000000000);
	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
	TString dir = "events/trd_v13p_3e/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Output file with reconstructed tracks and hits
   TString globalHitsFile = dir + "global.hits.0000.root"; // File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
   TString globalTracksFile = dir + "global.tracks.0000.root";// Output file with global tracks

   // Digi files
   TList* parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13p_3e.digi.par"; // TRD digi file
   TString muchDigiFile = parDir + "/much/much_v13f.digi.root"; // MUCH digi file
   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v13d.root";
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par";// TOF digi file

   // Reconstruction parameters
   TString globalTrackingType = "nn"; // Global tracking type
   TString stsHitProducerType = "real"; // STS hit producer type: real, ideal, new
   TString trdHitProducerType = "LW"; // TRD hit producer type: smearing, digi, clustering
   TString muchHitProducerType = "advanced"; // MUCH hit producer type: simple, advanced

	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
		globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
		globalHitsFile = TString(gSystem->Getenv("LIT_GLOBAL_HITS_FILE"));
		globalTracksFile = TString(gSystem->Getenv("LIT_GLOBAL_TRACKS_FILE"));

		globalTrackingType = TString(gSystem->Getenv("LIT_GLOBAL_TRACKING_TYPE"));
		stsHitProducerType = TString(gSystem->Getenv("LIT_STS_HITPRODUCER_TYPE"));
		trdHitProducerType = TString(gSystem->Getenv("LIT_TRD_HITPRODUCER_TYPE"));
		muchHitProducerType = TString(gSystem->Getenv("LIT_MUCH_HITPRODUCER_TYPE"));

		stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
		trdDigiFile = TString(gSystem->Getenv("LIT_TRD_DIGI"));
		muchDigiFile = TString(gSystem->Getenv("LIT_MUCH_DIGI"));
		tofDigiFile = TString(gSystem->Getenv("LIT_TOF_DIGI"));
		stsMatBudgetFile = TString(gSystem->Getenv("LIT_STS_MAT_BUDGET_FILE"));
	}

	   parFileList->Add(&stsDigiFile);
	   //parFileList->Add(&trdDigiFile);

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunAna *run = new FairRunAna();
	if (opt == "all") {
		run->SetInputFile(mcFile);
		run->SetOutputFile(globalRecoFile);
	} else if (opt == "hits") {
		run->SetInputFile(mcFile);
		run->SetOutputFile(globalHitsFile);
	} else if (opt == "tracking") {
		run->SetInputFile(mcFile);
		run->AddFriend(globalHitsFile);
		run->SetOutputFile(globalTracksFile);
	} else {
		std::cout << "-E- Incorrect opt parameter" << std::endl;
		exit(0);
	}

  // ----- MC Data Manager   ------------------------------------------------
  CbmMCDataManager* mcManager=new CbmMCDataManager("MCManager", 1);
  mcManager->AddFile(mcFile);
  run->AddTask(mcManager);
  // ------------------------------------------------------------------------


	if (opt == "all" || opt == "hits") {

		if (0){//IsMvd(parFile)) {
			// ----- MVD reconstruction    --------------------------------------------
  // -----   MVD Digitiser   -------------------------------------------------
  CbmMvdDigitizer* mvdDigitise = new CbmMvdDigitizer("MVD Digitiser", 0, iVerbose);
  run->AddTask(mvdDigitise);
  // -------------------------------------------------------------------------

  // -----   MVD Clusterfinder   ---------------------------------------------
  CbmMvdClusterfinder* mvdCluster = new CbmMvdClusterfinder("MVD Clusterfinder", 0, iVerbose);
  run->AddTask(mvdCluster);
  // -------------------------------------------------------------------------

  // -----   MVD Hit Finder   ------------------------------------------------
  CbmMvdHitfinder* mvdHitfinder = new CbmMvdHitfinder("MVD Hit Finder", 0, iVerbose);
  mvdHitfinder->UseClusterfinder(kTRUE);
  run->AddTask(mvdHitfinder);


		}

		if (stsHitProducerType == "real") {
		// ----- STS REAL reconstruction -----------------------------------------------
         CbmStsDigitize_old* stsDigitize = new CbmStsDigitize_old();
         run->AddTask(stsDigitize);

         FairTask* stsClusterFinder = new CbmStsClusterFinder_old();
         run->AddTask(stsClusterFinder);

         FairTask* stsFindHits = new CbmStsFindHits_old();
         run->AddTask(stsFindHits);

        // FairTask* stsMatchHits = new CbmStsMatchHits();
        // run->AddTask(stsMatchHits);

		} else if (stsHitProducerType == "ideal") {

         // ----- STS IDEAL reconstruction   ---------------------------------------------
         FairTask* stsDigitize =	new CbmStsIdealDigitize();
         run->AddTask(stsDigitize);

         FairTask* stsClusterFinder = new CbmStsClusterFinder_old();
         run->AddTask(stsClusterFinder);

         FairTask* stsFindHits =	new CbmStsIdealFindHits();
         run->AddTask(stsFindHits);

       //  FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
       //  run->AddTask(stsMatchHits);
		} else if (stsHitProducerType == "new") {
			  // --- The following settings correspond to the settings for the old
			  // --- digitizer in run_reco.C
			  Double_t dynRange       =   40960.;  // Dynamic range [e]
			  Double_t threshold      =    4000.;  // Digitisation threshold [e]
			  Int_t nAdc              =    4096;   // Number of ADC channels (12 bit)
			  Double_t timeResolution =       5.;  // time resolution [ns]
			  Double_t deadTime       = 9999999.;  // infinite dead time (integrate entire event)
			  Int_t digiModel         = 1;  // Model: 1 = uniform charge distribution along track

			  CbmStsDigitize* stsDigi = new CbmStsDigitize(digiModel);
			  stsDigi->SetParameters(dynRange, threshold, nAdc, timeResolution, deadTime, 0);
			  run->AddTask(stsDigi);

			  // -----   STS Cluster Finder   --------------------------------------------
			  FairTask* stsCluster = new CbmStsFindClusters();
			  run->AddTask(stsCluster);
			  // -------------------------------------------------------------------------


			  // -----   STS hit finder   ------------------------------------------------
			  FairTask* stsHit = new CbmStsFindHits();
			  run->AddTask(stsHit);
			  // -------------------------------------------------------------------------
		}

		FairTask* kalman = new CbmKF();
		run->AddTask(kalman);
		CbmL1* l1 = new CbmL1();
		l1->SetExtrapolateToTheEndOfSTS(true);
		//l1->SetMaterialBudgetFileName(stsMatBudgetFile);
		run->AddTask(l1);
		CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
		FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
		run->AddTask(findTracks);

	//	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
	//	run->AddTask(stsMatchTracks);
		// ------------------------------------------------------------------------

		if (0){//IsMuch(parFile)) {
			// -------- MUCH digitization ------------
			CbmMuchDigitizeGem* digitize = new CbmMuchDigitizeGem(muchDigiFile.Data());
			if (muchHitProducerType == "simple") {
				digitize->SetAlgorithm(0);
			} else if (muchHitProducerType == "advanced") {
				digitize->SetAlgorithm(1);
			}
			run->AddTask(digitize);
			CbmMuchFindHitsGem* findHits = new CbmMuchFindHitsGem(muchDigiFile.Data());
			run->AddTask(findHits);

			CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws(muchDigiFile.Data());
			strawDigitize->SetDimens(2);
			run->AddTask(strawDigitize);
			CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws(muchDigiFile.Data());
			//strawFindHits->SetMerge(1);
			strawFindHits->SetDimens(2);
			run->AddTask(strawFindHits);
			// -----------------------------------------------------------------
		}

		if (1){//IsTrd(parFile)) {
			// ----- TRD reconstruction-----------------------------------------
   parFileList->Add(&trdDigiFile);
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
				// ----- TRD clustering -----
			   CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
			   run->AddTask(trdDigiPrf);

			   CbmTrdClusterFinderFast* trdCluster = new CbmTrdClusterFinderFast();
			   run->AddTask(trdCluster);

			   CbmTrdHitProducerCluster* trdHit = new CbmTrdHitProducerCluster();
			   run->AddTask(trdHit);
                        } else if (trdHitProducerType == "simple") {
 //                        CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
                           CbmTrdDigitizer* trdDigitizer = new CbmTrdDigitizer(radiator);
                           run->AddTask(trdDigitizer);

                           CbmLitFindSimpleTrdClusters* trdSimpleFinder = new CbmLitFindSimpleTrdClusters();
                           run->AddTask(trdSimpleFinder);
                        } else if (trdHitProducerType == "LW") {
                                // ----- TRD clustering -----
                           CbmTrdDigitizerPRF* trdDigiPrf = new CbmTrdDigitizerPRF(radiator);
                           run->AddTask(trdDigiPrf);

                           CbmLitFindLWClusters* trdCluster = new CbmLitFindLWClusters();
                           run->AddTask(trdCluster);

                           CbmLitShowClusters* trdQA = new CbmLitShowClusters();
                           trdQA->SetOutputDir(std::string("test/"));
                           run->AddTask(trdQA);

                           /** ToDo: Write following Class **/
 //                        CbmLitFindLWHits* trdHit = new CbmLitFindLWHits();
 //                        run->AddTask(trdHit);
				// ----- End TRD Clustering -----
			}
			// ------------------------------------------------------------------------
		}

		if (1){//IsTof(parFile)) {
   parFileList->Add(&tofDigiFile);
			// ------ TOF hits --------------------------------------------------------
		   CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("TOF HitProducerNew",iVerbose);
		   tofHitProd->SetInitFromAscii(kFALSE);
		   run->AddTask(tofHitProd);
			// ------------------------------------------------------------------------
		}
	}

	if (opt == "all" || opt == "tracking") {
		// ------ Global track reconstruction -------------------------------------
		CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
		//CbmLitFindGlobalTracksParallel* finder = new CbmLitFindGlobalTracksParallel();
		// Tracking method to be used
		// "branch" - branching tracking
		// "nn" - nearest neighbor tracking
		// "nn_parallel" - nearest neighbor parallel tracking
		finder->SetTrackingType(std::string(globalTrackingType));

		// Hit-to-track merger method to be used
		// "nearest_hit" - assigns nearest hit to track
		// "all_hits" - assigns all hits in the searching area to track
		finder->SetMergerType("nearest_hit");

		run->AddTask(finder);
	}

	if (opt == "all" || opt == "tracking") {
	  if (1){//IsRich(parFile)) {
	     if (opt == "tracking") {
	        FairTask* kalman = new CbmKF();
	        run->AddTask(kalman);
	        FairTask* l1 = new CbmL1();
	        run->AddTask(l1);
	     }

	     CbmRichHitProducer* richHitProd  = new CbmRichHitProducer();
	     run->AddTask(richHitProd);

		  CbmRichReconstruction* richReco = new CbmRichReconstruction();
		  run->AddTask(richReco);

		  CbmRichMatchRings* matchRings = new CbmRichMatchRings();
		  run->AddTask(matchRings);
	   }
    }


    if (opt == "all" || opt == "tracking") {
      if (opt == "tracking") {
         FairTask* kalman = new CbmKF();
         run->AddTask(kalman);
         FairTask* l1 = new CbmL1();
         run->AddTask(l1);
      }
		// -----   Primary vertex finding   --------------------------------------
		CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
		CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
		run->AddTask(findVertex);
		// -----------------------------------------------------------------------
	}

   CbmMatchRecoToMC* matchTask = new CbmMatchRecoToMC();
   run->AddTask(matchTask);

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

	gStyle->SetPalette(51); // Set Drawing Color Palette

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
