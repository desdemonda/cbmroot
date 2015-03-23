/*
 * 	\file 	global_reco_qa_new.C
 *  \date 	Dec 1, 2014
 *  \author Jonathan Pieper <j.pieper@gsi.de>
 *  \brief	Script to find right data and compare new Algorithms
 */

#include <iostream>		// for cli output
#include <vector>		// for vectors
using std::cout;
using std::endl;
using std::vector;
using std::string;


void global_reco_qa_trd(Int_t nEvents = 5)
{
	   TTree::SetMaxTreeSize(90000000000);
	   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));	// Script Dir
	   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");	// Dir for parameters
	   TString dir = "events/trd_v13p_3e/";// Output directory
	   TString resultDir = "test/"; // Output directory for results
	   TString mcFile = dir + "mc.0000.root"; // MC transport file
	   TString parFile = dir + "param.0000.root"; // Parameter file
	   TString globalRecoFile = dir + "global.reco.0000.root"; // File with global tracks
	   TString globalHitsFile = dir + "global.hits.0000.root"; // File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
	   TString globalTracksFile = dir + "global.tracks.0000.root";// Output file with global tracks
	   TString qaFile = dir + "qa.0000.root"; // Output file with histograms

	   TList* parFileList = new TList();
	   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
	   TObjString trdDigiFile = parDir + "/trd/trd_v13p_3e.digi.par"; // TRD digi file
	   TString muchDigiFile = parDir + "/much/much_v13a.digi.root"; // MUCH digi file
	   TString stsMatBudgetFile = parDir + "/sts/sts_matbudget_v13d.root";
	   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par";// TOF digi file

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

		// ------------------------------------------------------------------------

/*		CbmKF* kalman = new CbmKF();
		run->AddTask(kalman);
		CbmL1* l1 = new CbmL1();
		l1->SetMaterialBudgetFileName(stsMatBudgetFile);
		run->AddTask(l1);*/

		CbmLitClusteringQaTrd* clusteringQa = new CbmLitClusteringQaTrd();
		clusteringQa->SetMuchDigiFileName(muchDigiFile.Data());
		clusteringQa->SetOutputDir(std::string(resultDir));
		run->AddTask(clusteringQa);

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


