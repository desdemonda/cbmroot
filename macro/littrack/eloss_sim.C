void eloss_sim(Int_t nEvents = 20000)
{
	TString dir = "/home/d/andrey/eloss/";
	TString outFile = dir + "eloss.mc.root";
	TString parFile = dir + "eloss.params.root";

	TString caveGeom   = "cave_vacuum.geo";
	TString litGeom   = "simple_geo.geo";

	TStopwatch timer;
	timer.Start();

	gSystem->Load("/home/soft/tbb/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Lenny64/libtbb");
	gSystem->Load("/u/andrey/soft/tbb/Etch32/libtbb");

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3");             // Transport engine
	fRun->SetOutputFile(outFile);          // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo");       // Materials

	if ( caveGeom != "" ) {
	FairModule* cave = new CbmCave("CAVE");
	cave->SetGeometryFileName(caveGeom);
	fRun->AddModule(cave);
	cout << "    --- " << caveGeom << endl;
	}

	if ( litGeom != "" ) {
	FairDetector* lit = new CbmLitDet("LITDET", kTRUE);
	lit->SetGeometryFileName(litGeom);
	fRun->AddModule(lit);
	cout << "    --- " << litGeom << endl;
	}

//	CbmFieldMap* magField = NULL;
//	fRun->SetField(magField);

	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	Double_t minMom = 10.; //minimum momentum
	Double_t maxMom = 10.; //maximum momentum

	FairBoxGenerator* boxGen1 = new FairBoxGenerator(-11, 1);
	boxGen1->SetPRange(minMom, maxMom);
	boxGen1->SetPhiRange(0.,360.);
	boxGen1->SetThetaRange(0.,0.);
	boxGen1->SetCosTheta();
	boxGen1->Init();
	primGen->AddGenerator(boxGen1);

	fRun->SetGenerator(primGen);

	fRun->Init();

	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
	//  FairParAsciiFileIo* parOut = new FairParAsciiFileIo();
	parOut->open(parFile.Data());
	rtdb->setOutput(parOut);
	rtdb->saveOutput();
	rtdb->print();

	fRun->Run(nEvents);

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Real time used: " << timer.RealTime() << "s " << endl;
	cout << "CPU time used : " << timer.CpuTime() << "s " << endl;
	cout << endl << endl;
	// ------------------------------------------------------------------------
}

