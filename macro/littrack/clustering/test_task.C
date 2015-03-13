/**
 * \file test_task.C
 * \brief Macro test task.
 * \author Grigory Kozlov <email@email.com>
 * \date 2012
 **/

#include <iostream>
using std::cout;
using std::endl;

void test_task(Int_t nEvents = 1)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//TString dir = "../littrack/commit_tests/events_muon/"; // Input/Output directory
	//TString dir = "/home/kozlov/cbm/cbmroot_clustering/cbmroot/macro/littrack/commit_tests/events_muon/"; // Input/Output directory
	TString dir = "/home/kozlov/cbm/cbmroot_clustering/events/03/"; // Input/Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString recoFile = dir + "global.reco.0000.root"; // Reconstruction file
  	TString outFile = dir + "test.0000.root"; // Output file

  	TString resultDir = "./test/"; // Directory for output results

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // Digi scheme for STS
   parFileList->Add(&stsDigiFile);

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/clustering/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(recoFile);
	run->SetOutputFile(outFile);

	CbmLitTestTask* testTask = new CbmLitTestTask();
   run->AddTask(testTask);

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
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}

