/**
 * \file create_data.C
 * \brief Macro creates text files with data produced by CBMROOT.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 * Macro creates text files with data produced by CBMROOT
 * simulations which can be later used to run
 * the standalone tracking package.
 */

#include <iostream>
using std::cout;
using std::endl;

void create_data(Int_t nEvents = 100)
{
   // Output directory
	TString dir = "/d/cbm02/andrey/test_muons/";
	// MC transport file
	TString mcFile = dir + "mc.0000.root";
	// Parameter file
	TString parFile = dir + "param.0000.root";
	// File with reconstructed events
	TString globalRecoFile = dir + "global.reco.0000.root";
	// Dummy output file
   TString dummyFile = dir + "dummy.root";

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalRecoFile);
	run->SetOutputFile(dummyFile);
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	CbmLitCreateStandaloneData* creator = new CbmLitCreateStandaloneData();
	run->AddTask(creator);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
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
	cout << "Output file is " << dummyFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
