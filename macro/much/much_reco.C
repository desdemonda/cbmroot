/**
 * \file much_reco.C
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Reconstruction macro for MUCH.
 **/

#include <iostream>
using namespace std;

void much_reco(
		Int_t nEvents = 100)
{
   TString dir  = "data/"; // Output directory
   TString mcFile = dir + "mc.root"; // MC transport file
   TString parFile = dir + "params.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Output file with reconstructed tracks and hits

   // Digi files
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TList* parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   TString muchDigiFile = parDir + "/much/much_v12b.digi.root"; // MUCH digi file
   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v13d.root";
   parFileList->Add(&stsDigiFile);

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunAna* run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalRecoFile);

	// --- STS Digitization -----------------------------------------------------
   CbmStsDigitize_old* stsDigitize = new CbmStsDigitize_old();
   run->AddTask(stsDigitize);

   FairTask* stsClusterFinder = new CbmStsClusterFinder_old();
   run->AddTask(stsClusterFinder);

   FairTask* stsFindHits = new CbmStsFindHits_old();
   run->AddTask(stsFindHits);

   FairTask* stsMatchHits = new CbmStsMatchHits();
   run->AddTask(stsMatchHits);
	// --------------------------------------------------------------------------

   // --- STS reconstruction ---------------------------------------------------
   FairTask* kalman = new CbmKF();
   run->AddTask(kalman);
   CbmL1* l1 = new CbmL1();
   l1->SetMaterialBudgetFileName(stsMatBudgetFile);
   run->AddTask(l1);
   CbmStsTrackFinder* trackFinder = new CbmL1StsTrackFinder();
   FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
   run->AddTask(findTracks);

   FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
   run->AddTask(stsMatchTracks);
   // --------------------------------------------------------------------------

   // ----- MUCH hits----------   ----------------------------------------------
   CbmMuchDigitizeGem* muchDigitize = new CbmMuchDigitizeGem(muchDigiFile.Data());
   run->AddTask(muchDigitize);
   CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws(muchDigiFile.Data());
   run->AddTask(strawDigitize);

   CbmMuchFindHitsGem* muchFindHits = new CbmMuchFindHitsGem(muchDigiFile.Data());
   run->AddTask(muchFindHits);
   CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws(muchDigiFile.Data());
   run->AddTask(strawFindHits);
   // --------------------------------------------------------------------------

   // --- Global tracking ------------------------------------------------------
   CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
	finder->SetTrackingType("branch");
	finder->SetMergerType("nearest_hit");
	run->AddTask(finder);
	// --------------------------------------------------------------------------

   CbmMatchRecoToMC* matchTask = new CbmMatchRecoToMC();
   run->AddTask(matchTask);

   // -----   Primary vertex finding   -----------------------------------------
   CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
   CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
   run->AddTask(findVertex);
   // --------------------------------------------------------------------------

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
   run->Run(0,nEvents);
   // ------------------------------------------------------------------------

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   cout << "Macro finished successfully." << endl;
   cout << "Output file is "    << globalRecoFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}
