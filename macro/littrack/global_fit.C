/**
 * \file global_fit.C
 * \brief Track fit.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include <iostream>
using std::cout;
using std::endl;

void global_fit(Int_t nEvents = 100)
{
   TTree::SetMaxTreeSize(90000000000);
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
   TString dir = "events/much_v12b/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Reconstructed tracks and hits
   TString globalFitFile = dir + "global.fit.0000.root"; // Output file

   // Digi files
//   TList* parFileList = new TList();
//   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
//   TObjString trdDigiFile = parDir + "/trd/trd_v13b.digi.par"; // TRD digi file
//   TString muchDigiFile = parDir + "/much/much_v11a.digi.root"; // MUCH digi file
//   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v12b.root";
//   TString tofDigiFile = "/parameters/tof/par_tof_V13a.txt";//parDir + "/tof/par_tof_V13a.txt";

   // Directory for output results
   TString resultDir = "./test/";

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
      parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
      globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
      globalFitFile = TString(gSystem->Getenv("LIT_GLOBAL_FIT_FILE"));
      resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));

//      stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
//      trdDigiFile = TString(gSystem->Getenv("LIT_TRD_DIGI"));
//      muchDigiFile = TString(gSystem->Getenv("LIT_MUCH_DIGI"));
//      tofDigiFile = TString(gSystem->Getenv("LIT_TOF_DIGI"));
//      stsMatBudgetFile = TString(gSystem->Getenv("LIT_STS_MAT_BUDGET_FILE"));
   }

//   parFileList->Add(&stsDigiFile);
//   parFileList->Add(&trdDigiFile);

   Int_t iVerbose = 1;
   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunAna *run = new FairRunAna();
   run->SetInputFile(mcFile);
   run->AddFriend(globalRecoFile);
   run->SetOutputFile(globalFitFile);

   FairTask* kalman = new CbmKF();
   run->AddTask(kalman);
   CbmL1* l1 = new CbmL1();
  // l1->SetMaterialBudgetFileName(stsMatBudgetFile);
   run->AddTask(l1);

   CbmLitFitTracksParallel* fitter = new CbmLitFitTracksParallel();
  // CbmLitFitTracks* fitter = new CbmLitFitTracks();
   run->AddTask(fitter);

   CbmLitFitQa* fitQa = new CbmLitFitQa();
   fitQa->SetMvdMinNofHits(0);
   fitQa->SetStsMinNofHits(4);
   fitQa->SetMuchMinNofHits(10);
   fitQa->SetTrdMinNofHits(6);
   fitQa->SetOutputDir(std::string(resultDir));
   run->AddTask(fitQa);


   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
 //  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
 //  parIo2->open(parFileList, "in");
   rtdb->setFirstInput(parIo1);
//   rtdb->setSecondInput(parIo2);
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
   cout << "Output file is " << globalFitFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------
}
