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

void global_reco(Int_t nEvents = 100, Int_t seed = 555)
{
	gRandom->SetSeed(seed);

   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
   TString dir = "events/much_anna_omega_8gev_10k/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Output file with reconstructed tracks and hits

   // Digi files
   TList* parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13g.digi.par"; // TRD digi file
   TString muchDigiFile = parDir + "/much/much_v12c.digi.root"; // MUCH digi file
   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v12b.root";
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par";// TOF digi file

   // Reconstruction parameters
   TString globalTrackingType = "nn"; // Global tracking type
   TString trdHitProducerType = "smearing"; // TRD hit producer type: smearing, digi, clustering
   TString muchHitProducerType = "advanced"; // MUCH hit producer type: simple, advanced

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
      parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
      globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));

      stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
      trdDigiFile = TString(gSystem->Getenv("LIT_TRD_DIGI"));
      muchDigiFile = TString(gSystem->Getenv("LIT_MUCH_DIGI"));
      tofDigiFile = TString(gSystem->Getenv("LIT_TOF_DIGI"));
      stsMatBudgetFile = TString(gSystem->Getenv("LIT_STS_MAT_BUDGET_FILE"));
   }

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunAna *run = new FairRunAna();
   run->SetInputFile(mcFile);
	run->SetOutputFile(globalRecoFile);


   // -----   STS  -------------------------------------------------
   Double_t threshold = 4;
   Double_t noiseWidth = 0.01;
   Int_t nofBits = 12;
   Double_t ElectronsPerAdc =  10.;
   Double_t StripDeadTime = 0.1;
   CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
   stsDigitize->SetRealisticResponse();
   stsDigitize->SetFrontThreshold(threshold);
   stsDigitize->SetBackThreshold(threshold);
   stsDigitize->SetFrontNoiseWidth(noiseWidth);
   stsDigitize->SetBackNoiseWidth(noiseWidth);
   stsDigitize->SetFrontNofBits(nofBits);
   stsDigitize->SetBackNofBits(nofBits);
   stsDigitize->SetFrontNofElPerAdc(ElectronsPerAdc);
   stsDigitize->SetBackNofElPerAdc(ElectronsPerAdc);
   stsDigitize->SetStripDeadTime(StripDeadTime);
   run->AddTask(stsDigitize);

   FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",iVerbose);
   run->AddTask(stsClusterFinder);

   FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
   run->AddTask(stsFindHits);

   FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
   run->AddTask(stsMatchHits);

   FairTask* kalman = new CbmKF();
   run->AddTask(kalman);
   CbmL1* l1 = new CbmL1();
   //l1->SetExtrapolateToTheEndOfSTS(true);
   l1->SetMaterialBudgetFileName(stsMatBudgetFile);
   run->AddTask(l1);
   CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
   FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
   run->AddTask(findTracks);

   FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
   run->AddTask(stsMatchTracks);
	// ------------------------------------------------------------------------

   if (IsMuch(parFile)) {
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

      CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws("MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
      run->AddTask(strawDigitize);
      CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws("MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
      strawFindHits->SetMerge(1);
      run->AddTask(strawFindHits);
      // -----------------------------------------------------------------
   }

   if (IsTrd(parFile)) {
      // ----- TRD reconstruction-----------------------------------------
      // Update of the values for the radiator F.U. 17.08.07
      Int_t trdNFoils = 130; // number of polyetylene foils
      Float_t trdDFoils = 0.0013; // thickness of 1 foil [cm]
      Float_t trdDGap = 0.02; // thickness of gap between foils [cm]
      Bool_t simpleTR = kTRUE; // use fast and simple version for TR production

      CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR, trdNFoils, trdDFoils, trdDGap);

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
         CbmTrdDigitizerPRF* trdClustering = new CbmTrdDigitizerPRF("TRD Clusterizer", "TRD task", radiator, false, true);
         run->AddTask(trdClustering);

         CbmTrdClusterFinderFast* trdClusterfindingfast = new CbmTrdClusterFinderFast(true, true, false, 5.0e-7);
         run->AddTask(trdClusterfindingfast);

         CbmTrdHitProducerCluster* trdClusterHitProducer = new CbmTrdHitProducerCluster();
         run->AddTask(trdClusterHitProducer);
         // ----- End TRD Clustering -----
      }
      // ------------------------------------------------------------------------
   }

   if (IsTof(parFile)) {
      // ------ TOF hits --------------------------------------------------------
      CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("TOF HitProducerNew",iVerbose);
      tofHitProd->SetInitFromAscii(kFALSE);
      run->AddTask(tofHitProd);
      // ------------------------------------------------------------------------
   }

   // ------ Global track reconstruction -------------------------------------
   CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
   //CbmLitFindGlobalTracksParallel* finder = new CbmLitFindGlobalTracksParallel();
   // Tracking method to be used
   // "branch" - branching tracking
   // "nn" - nearest neighbor tracking
   // "nn_parallel" - nearest neighbor parallel tracking
   finder->SetTrackingType(std::string(globalTrackingType));

   // Hit-to-track merger method to be used
   // "nearest_hit" - assigns nearest hit to the track
   finder->SetMergerType("nearest_hit");

   run->AddTask(finder);

   if (IsTrd(parFile)) {
      CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks();
      run->AddTask(trdMatchTracks);
   }

   if (IsMuch(parFile)) {
      CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
      run->AddTask(muchMatchTracks);
   }

   // -----   Primary vertex finding   --------------------------------------
   CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
   CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
   run->AddTask(findVertex);
   // -----------------------------------------------------------------------

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
   cout << "Test passed"<< endl;
   cout << " All ok " << endl;
	cout << "Output file is " << globalRecoFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time "	<< timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}
