/**
 * \file prop_qa.C
 * \brief Macro runs propagation analysis task.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2010
 **/

#include <iostream>
using std::cout;
using std::endl;

void prop_qa(Int_t nEvents = 1000)
{
	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

	TString dir = "/data.local1/andrey/events/events_propagation_qa_electrons/";
	TString mcFile = dir + "mc.0000.root";
	TString globalTracksFile = dir + "global.reco.0000.root";
	TString parFile = dir + "param.0000.root";
	TString propAnaFile = dir + "propagation.qa.0000.root";
	TString resultDir = "./test/";
	Int_t nofHits = 10;
	Int_t pdg = 11;
	Int_t testFastPropagation = 0;

	if(script == "yes") {
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
		globalTracksFile = TString(gSystem->Getenv("LIT_GLOBAL_TRACKS_IDEAL_FILE"));
		resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));
		propAnaFile = TString(gSystem->Getenv("LIT_PROP_ANA_FILE"));
		nofHits = TString(gSystem->Getenv("LIT_NOF_HITS")).Atoi();
		pdg = TString(gSystem->Getenv("LIT_PDG")).Atoi();
		testFastPropagation = TString(gSystem->Getenv("LIT_TEST_FAST_PROPAGATION")).Atoi();
	}

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->AddFriend(globalTracksFile);
	run->SetOutputFile(propAnaFile);

	// -------------------------------------------------------------------------
	CbmLitPropagationQa* propQa = new CbmLitPropagationQa();
	propQa->SetNofPlanes(13);
//	propQa->SetMinNofHits(nofHits);
//	propQa->SetPDGCode(pdg);
//	propQa->SetTestFastPropagation(testFastPropagation);
	propQa->SetOutputDir(std::string(resultDir));
	propQa->SetFixedBounds(true);
	run->AddTask(propQa);
	// -------------------------------------------------------------------------
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsDigiFile = parDir+ "/sts/sts_standard.digi.par";
	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(stsDigiFile.Data(),"in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Intialise and run   --------------------------------------------
	run->Init();
	run->Run(0,nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is "    << propAnaFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
