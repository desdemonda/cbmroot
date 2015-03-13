/*
 * mvd_test.C
 *
 *  Created on: Mar 6, 2012
 *      Author: kozlov
 */

#include <iostream>
using std::cout;
using std::endl;

void mvd_test(Int_t nEvents = 100)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	TString dir = "/u/gkozlov/cbm/events/mvd/100events/"; // Input/Output directory
   //TString mcFile = dir + "auau.25gev.centr.2maps_v11a_10events.mc.root"; // MC transport file
   //TString parFile = dir + "auau.25gev.centr.2maps_v11a_10events.params.root"; // Parameters file
   //TString recoFile = dir + "auau.25gev.centr.2maps_v11a_10events.rec.root"; // Reconstruction file
   //---pc.30gev.centr---
	/*TString mcFile = dir + "pc.30gev.centr_draw10.mc.root"; // MC transport file
   TString parFile = dir + "pc.30gev.centr_draw10.params.root"; // Parameters file
   TString recoFile = dir + "pc.30gev.centr_draw10.rec.root"; // Reconstruction file*/
   //---cc.25gev.centr---
   TString mcFile = dir + "cc.25gev.centr_draw10.mc.root"; // MC transport file
   TString parFile = dir + "cc.25gev.centr_draw10.params.root"; // Parameters file
   TString recoFile = dir + "cc.25gev.centr_draw10.rec.root"; // Reconstruction file
   //---pAu.30gev.centr---
   /*TString mcFile = dir + "pAu.30gev.centr_draw10.mc.root"; // MC transport file
   TString parFile = dir + "pAu.30gev.centr_draw10.params.root"; // Parameters file
   TString recoFile = dir + "pAu.30gev.centr_draw10.rec.root"; // Reconstruction file*/
   //---
   TString outFile = dir + "mvd.OUT.test.0000.root"; // Output file

  	TString resultDir = "./test/"; // Directory for output results

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a_0-8.digi.par";//sts_v11a.digi.par"; // Digi scheme for STS
   //TObjString muchDigiFile = parDir + "/much/much_v11a.digi.root";
   parFileList->Add(&stsDigiFile);
   //parFileList->Add(&muchDigiFile);

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/clustering/cbmrootlibs.C");
	cbmrootlibs();
	gSystem->Load("libGeoBase");
	  gSystem->Load("libParBase");
	  gSystem->Load("libBase");
	  gSystem->Load("libCbmBase");
	  gSystem->Load("libCbmData");
	  gSystem->Load("libField");
	  gSystem->Load("libGen");
	  gSystem->Load("libPassive");
	  gSystem->Load("libMvd");
	  gSystem->Load("libSts");
	  gSystem->Load("libTrd");
	  gSystem->Load("libTof");
	  gSystem->Load("libEcal");
	  gSystem->Load("libGlobal");
	  gSystem->Load("libKF");
	  gSystem->Load("libRich");
	  gSystem->Load("libL1");
	  gSystem->Load("libMinuit2"); // Needs for rich ellipse fitter

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(recoFile);
	run->SetOutputFile(outFile);

	CbmMvdClustering* testTask = new CbmMvdClustering();
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

