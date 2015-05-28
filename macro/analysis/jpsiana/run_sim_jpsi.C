void run_sim_jpsi(Int_t nEvents = 2)
{
   TTree::SetMaxTreeSize(90000000000);
	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	//gRandom->SetSeed(10);

	//TString inFile = "/data/CbmRoot/cbmroot/trunk/input/urqmd.auau.25gev.centr.root";
	//TString parFile = "/data/CbmRoot/sim_data/param.0001.root";
	//TString geoFile = "/data/CbmRoot/sim_data/geofilefull.0001.root";
	//TString outFile = "/data/CbmRoot/sim_data/mc.0001.root";

	TString inFile = "/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.00001.root";
	TString parFile = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/param.0001.root";
	TString geoFile = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/geofilefull.0001.root";
	TString outFile = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/mc.0001.root";

	TString caveGeom = "cave.geo";
	TString pipeGeom = "pipe/pipe_v14h.root";
	TString magnetGeom = "magnet/magnet_v12a.geo";
	TString mvdGeom = "";
	TString stsGeom = "sts/sts_v13d.geo.root";
	TString richGeom= "rich/rich_v14a_3e.root";
	TString trdGeom = "trd/trd_v14a_3e.geo.root";
	TString tofGeom = "tof/tof_v13b.geo.root";
	TString ecalGeom = "";
	TString fieldMap = "field_v12a";

	TString electrons = "no"; // If "yes" than primary electrons will be generated
	Int_t NELECTRONS = 5; // number of e- to be generated
	Int_t NPOSITRONS = 5; // number of e+ to be generated
	TString urqmd = "yes"; // If "yes" than UrQMD will be used as background
	TString pluto = "yes"; // If "yes" PLUTO particles will be embedded
	//TString plutoFile = "/data/CbmRoot/cbmroot/trunk/input/pluto.auau.25gev.jpsi.epem.0001.root";
	TString plutoFile = "/Users/slebedev/Development/cbm/data/jpsi/pluto.auau.25gev.jpsi.epem.0001.root";
	TString plutoParticle = "";
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale =  1.0; // field scaling factor

	if (script == "yes") {
		inFile = TString(gSystem->Getenv("IN_FILE"));
		outFile = TString(gSystem->Getenv("MC_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));

		caveGeom = TString(gSystem->Getenv("CAVE_GEOM"));
		pipeGeom = TString(gSystem->Getenv("PIPE_GEOM"));
		mvdGeom = TString(gSystem->Getenv("MVD_GEOM"));
		stsGeom = TString(gSystem->Getenv("STS_GEOM"));
		richGeom = TString(gSystem->Getenv("RICH_GEOM"));
		trdGeom = TString(gSystem->Getenv("TRD_GEOM"));
		tofGeom = TString(gSystem->Getenv("TOF_GEOM"));
		ecalGeom = TString(gSystem->Getenv("ECAL_GEOM"));
		fieldMap = TString(gSystem->Getenv("FIELD_MAP"));
		magnetGeom = TString(gSystem->Getenv("MAGNET_GEOM"));

		NELECTRONS = TString(gSystem->Getenv("NELECTRONS")).Atoi();
		NPOSITRONS = TString(gSystem->Getenv("NPOSITRONS")).Atoi();
		electrons = TString(gSystem->Getenv("ELECTRONS"));
		urqmd = TString(gSystem->Getenv("URQMD"));
		pluto = TString(gSystem->Getenv("PLUTO"));
		plutoFile = TString(gSystem->Getenv("PLUTO_FILE"));
		plutoParticle = TString(gSystem->Getenv("PLUTO_PARTICLE"));
		fieldScale = TString(gSystem->Getenv("FIELD_MAP_SCALE")).Atof();
	}

	gDebug = 0;
	TStopwatch timer;
	timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3"); // Transport engine
	fRun->SetOutputFile(outFile);
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	//fRun->SetStoreTraj(kTRUE);

	fRun->SetMaterials("media.geo"); // Materials

	if ( caveGeom != "" ) {
		FairModule* cave = new CbmCave("CAVE");
		cave->SetGeometryFileName(caveGeom);
		fRun->AddModule(cave);
	}

	if ( pipeGeom != "" ) {
		FairModule* pipe = new CbmPipe("PIPE");
		pipe->SetGeometryFileName(pipeGeom);
		fRun->AddModule(pipe);
	}

	CbmTarget* target = new CbmTarget("Gold", 0.025); // 250 mum
	fRun->AddModule(target);

	if ( magnetGeom != "" ) {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		fRun->AddModule(magnet);
	}

	if ( mvdGeom != "" ) {
		FairDetector* mvd = new CbmMvd("MVD", kTRUE);
		mvd->SetGeometryFileName(mvdGeom);
		fRun->AddModule(mvd);
	}

	if ( stsGeom != "" ) {
		FairDetector* sts = new CbmStsMC(kTRUE);
		sts->SetGeometryFileName(stsGeom);
		fRun->AddModule(sts);
	}

	if ( richGeom != "" ) {
		FairDetector* rich = new CbmRich("RICH", kTRUE);
		rich->SetGeometryFileName(richGeom);
		fRun->AddModule(rich);
	}

	if ( trdGeom != "" ) {
		FairDetector* trd = new CbmTrd("TRD",kTRUE );
		trd->SetGeometryFileName(trdGeom);
		fRun->AddModule(trd);
	}

	if ( tofGeom != "" ) {
		FairDetector* tof = new CbmTof("TOF", kTRUE);
		tof->SetGeometryFileName(tofGeom);
		fRun->AddModule(tof);
	}

	// -----   Create magnetic field   ----------------------------------------
	CbmFieldMap* magField = NULL;
	magField = new CbmFieldMapSym2(fieldMap);
	magField->SetPosition(0., 0., fieldZ);
	magField->SetScale(fieldScale);
	fRun->SetField(magField);

	// -----   Create PrimaryGenerator   --------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	if (urqmd == "yes"){
		//CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
	    CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(inFile);
		urqmdGen->SetEventPlane(0. , 360.);
		primGen->AddGenerator(urqmdGen);
	}

	//add electrons
	if (electrons == "yes"){
		FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, NPOSITRONS);
		boxGen1->SetPtRange(0.,3.);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5,25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);

		FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, NELECTRONS);
		boxGen2->SetPtRange(0.,3.);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5,25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);

//      CbmLitPolarizedGenerator *polGen;
//      polGen = new CbmLitPolarizedGenerator(443, NELECTRONS);
//      polGen->SetDistributionPt(0.176);        // 25 GeV
//      polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
//      polGen->SetRangePt(0.,3.);
//      polGen->SetRangeY(1.,3.);
//      polGen->SetBox(0);
//      polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
//      polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiElectron);
//      polGen->SetAlpha(0);
//      polGen->Init();
//      primGen->AddGenerator(polGen);
   }

	if (pluto == "yes") {
		CbmPlutoGenerator *plutoGen= new CbmPlutoGenerator(plutoFile);
		primGen->AddGenerator(plutoGen);
	}

	fRun->SetGenerator(primGen);

	fRun->Init();

	// -----   Runtime database   ---------------------------------------------
	CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
	fieldPar->SetParameters(magField);
	fieldPar->setChanged();
	fieldPar->setInputVersion(fRun->GetRunId(),1);
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
	parOut->open(parFile.Data());
	rtdb->setOutput(parOut);
	rtdb->saveOutput();
	rtdb->print();

	fRun->Run(nEvents);

    fRun->CreateGeometryFile(geoFile);

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime
       << "s" << endl << endl;

	cout << " Test passed" << endl;
	cout << " All ok " << endl;
}

