/*
 * ideal_cl_test.c
 *
 *  Created on: May 29, 2012
 *      Author: kozlov
 */

#include <iostream>
using std::cout;
using std::endl;

void ideal_cl_test(Int_t nEvents = 1)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir = "/home/kozlov/cbm/cbmroot_new/events/03/"; // Input/Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString recoFile = dir + "global.reco.0000.root"; // Reconstruction file
  	TString outFile = dir + "OUT.Ideal.test.0000.root"; // Output file

  	TString resultDir = "./test/"; // Directory for output results

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // Digi scheme for STS
   TObjString muchDigiFile = parDir + "/much/much_v11a.digi.root";
   parFileList->Add(&stsDigiFile);
   parFileList->Add(&muchDigiFile);

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/clustering/cbmrootlibs.C");
	cbmrootlibs();
	/*gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libCbmBase");
	gSystem->Load("libCbmData");
	gSystem->Load("libPassive");
	gSystem->Load("libGen");
	gSystem->Load("libSts");
	gSystem->Load("libMuch");
	gSystem->Load("libVis");*/

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(recoFile);
	run->SetOutputFile(outFile);

	/*CbmMuchClustering* testTask = new CbmMuchClustering();*/
	CbmMuchIdealClusters* testTask = new CbmMuchIdealClusters();
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
	//cout << "Output file is " << outFile << endl;
	//cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}

