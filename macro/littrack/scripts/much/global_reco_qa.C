/**
 * \file global_reco_qa.C
 * \brief Macro runs CbmLitTrackingQa class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;
using std::string;

void global_reco_qa(Int_t nEvents = 10000)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir = "events/much_v12c_omega_8gev_10k/"; // Output directory
	TString resultDir = "test/"; // Output directory for results
	TString mcFile = dir + "mc.0000.root"; // MC transport file
	TString parFile = dir + "param.0000.root"; // Parameter file
   TString globalRecoFile = dir + "global.reco.0000.root"; // File with global tracks
	TString qaFile = dir + "qa.0000.root"; // Output file with histograms

   TList* parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13g.digi.par"; // TRD digi file
   TString muchDigiFile = parDir + "/much/much_v12c.digi.root"; // MUCH digi file
   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v12b.root";
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par";// TOF digi file

   // Normalization for efficiency
   Int_t normStsPoints = 4;
   Int_t normTrdPoints = 6;
   Int_t normMuchPoints = 6;
   Int_t normTofPoints = 1;
   Int_t normTrdHits = 6;
   Int_t normMuchHits = 6;
   Int_t normTofHits = 1;

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
      parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
      globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
      qaFile = TString(gSystem->Getenv("LIT_QA_FILE"));

      resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));

      normStsPoints = TString(gSystem->Getenv("LIT_NORM_STS_POINTS")).Atoi();
      normTrdPoints = TString(gSystem->Getenv("LIT_NORM_TRD_POINTS")).Atoi();
      normMuchPoints = TString(gSystem->Getenv("LIT_NORM_MUCH_POINTS")).Atoi();
      normTofPoints = TString(gSystem->Getenv("LIT_NORM_TOF_POINTS")).Atoi();
      normTrdHits = TString(gSystem->Getenv("LIT_NORM_TRD_HITS")).Atoi();
      normMuchHits = TString(gSystem->Getenv("LIT_NORM_MUCH_HITS")).Atoi();
      normTofHits = TString(gSystem->Getenv("LIT_NORM_TOF_HITS")).Atoi();

      stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
      trdDigiFile = TString(gSystem->Getenv("LIT_TRD_DIGI"));
      muchDigiFile = TString(gSystem->Getenv("LIT_MUCH_DIGI"));
      tofDigiFile = TString(gSystem->Getenv("LIT_TOF_DIGI"));
      stsMatBudgetFile = TString(gSystem->Getenv("LIT_STS_MAT_BUDGET_FILE"));
   }

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);

	TStopwatch timer;
	timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run = new FairRunAna();
   run->SetInputFile(mcFile);
   run->AddFriend(globalRecoFile);
   run->SetOutputFile(qaFile);
	run->SetOutputFile(qaFile);
	// ------------------------------------------------------------------------

	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
	l1->SetMaterialBudgetFileName(stsMatBudgetFile);
	run->AddTask(l1);

   // ----- Reconstruction QA tasks -----------------------------------------
	vector<string> trackCategories;
	trackCategories.push_back("All");
	//trackCategories.push_back("Electron");
	trackCategories.push_back("Muon");
	vector<string> ringCategories;
	ringCategories.push_back("Electron");
	ringCategories.push_back("ElectronReference");
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
  // trackingQa->SetTrackCategories(trackCategories);
  // trackingQa->SetRingCategories(ringCategories);
   trackingQa->SetOutputDir(std::string(resultDir));
   run->AddTask(trackingQa);

   CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(0);
   fitQa->SetStsMinNofHits(normStsPoints);
   fitQa->SetMuchMinNofHits(normMuchPoints);
   fitQa->SetTrdMinNofHits(normTrdPoints);
   fitQa->SetOutputDir(std::string(resultDir));
   run->AddTask(fitQa);

   CbmLitClusteringQa* clusteringQa = new CbmLitClusteringQa();
   clusteringQa->SetMuchDigiFile(muchDigiFile.Data());
   clusteringQa->SetOutputDir(std::string(resultDir));
   run->AddTask(clusteringQa);

   CbmLitTofQa* tofQa = new CbmLitTofQa();
   tofQa->SetOutputDir(std::string(resultDir));
   run->AddTask(tofQa);
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
	cout << "Output file is " << qaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
