void run_sim(Int_t nEvents = 1000)
{
	TTree::SetMaxTreeSize(90000000000);

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   TString urqmdFile = "/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14";
   TString parFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.param.root";
   TString mcFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.mc.root";


	TString caveGeom = "cave.geo";
	TString pipeGeom = "pipe/pipe_standard.geo";
	TString mvdGeom = "";//"mvd/mvd_v08a.geo";
	TString stsGeom = "sts/sts_v13c.geo.root";
	TString richGeom = "rich/rich_v08a.geo";
	TString trdGeom = "trd/trd_v13p_3e.geo.root";
	TString tofGeom = "tof/tof_v13b.geo.root";
	TString fieldMap = "field_v12a";
	TString magnetGeom = "magnet/magnet_v12a.geo";

	Int_t nofElectrons = 5;
	Int_t nofPositrons = 5;
	TString urqmd = "yes";
	TString pluto = "no";
	TString plutoFile = "";
	TString plutoParticle = "";

	// Magnetic field
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale =  1.0; // field scaling factor

	if (script == "yes") {
		urqmdFile = TString(gSystem->Getenv("URQMD_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		mcFile = TString(gSystem->Getenv("MC_FILE"));

		caveGeom = TString(gSystem->Getenv("CAVE_GEOM"));
		//targetGeom = TString(gSystem->Getenv("TARGET_GEOM"));
		pipeGeom = TString(gSystem->Getenv("PIPE_GEOM"));
		mvdGeom = TString(gSystem->Getenv("MVD_GEOM"));
		stsGeom = TString(gSystem->Getenv("STS_GEOM"));
		richGeom = TString(gSystem->Getenv("RICH_GEOM"));
		trdGeom = TString(gSystem->Getenv("TRD_GEOM"));
		tofGeom = TString(gSystem->Getenv("TOF_GEOM"));
		fieldMap = TString(gSystem->Getenv("FIELD_MAP"));
		magnetGeom = TString(gSystem->Getenv("MAGNET_GEOM"));

		nofElectrons = TString(gSystem->Getenv("NOF_ELECTRONS")).Atoi();
		nofPositrons = TString(gSystem->Getenv("NOF_POSITRONS")).Atoi();
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
	fRun->SetName("TGeant3");              // Transport engine
	fRun->SetOutputFile(mcFile);          // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo");       // Materials

	// -----   Create detectors and passive volumes   -------------------------
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

   CbmTarget* target = new CbmTarget("Gold", 0.0025); // 25 mum
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
   CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);

	// -----   Create PrimaryGenerator   --------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	if (urqmd == "yes"){
	       // FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(urqmdFile);
	       // primGen->AddGenerator(urqmdGen);
       CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(urqmdFile);
	    primGen->AddGenerator(urqmdGen);
	}

	// Add electrons
	if (nofElectrons > 0 ) {
		FairBoxGenerator* boxGen1 = new FairBoxGenerator(-11, nofElectrons);
		boxGen1->SetPtRange(0.,3.);
		boxGen1->SetPhiRange(0.,360.);
		boxGen1->SetThetaRange(2.5,25.);
		boxGen1->SetCosTheta();
		boxGen1->Init();
		primGen->AddGenerator(boxGen1);
	}
	if (nofPositrons > 0) {
		FairBoxGenerator* boxGen2 = new FairBoxGenerator(11, nofPositrons);
		boxGen2->SetPtRange(0.,3.);
		boxGen2->SetPhiRange(0.,360.);
		boxGen2->SetThetaRange(2.5,25.);
		boxGen2->SetCosTheta();
		boxGen2->Init();
		primGen->AddGenerator(boxGen2);
	}

	if (pluto == "yes") {
		FairPlutoGenerator *plutoGen= new FairPlutoGenerator(plutoFile);
		primGen->AddGenerator(plutoGen);
	}

	fRun->SetGenerator(primGen);
//	fRun->SetStoreTraj(kTRUE);
	fRun->Init();

	if (pluto == "yes" && urqmd == "yes") {
		Float_t bratioEta[6];
		Int_t modeEta[6];

		TGeant3* gMC3 = (TGeant3*) gMC;

		for (Int_t kz = 0; kz < 6; ++kz) {
			bratioEta[kz] = 0.;
			modeEta[kz]   = 0;
		}

		Int_t ipa    = 17;
		bratioEta[0] = 39.38;  //2gamma
		bratioEta[1] = 32.20;  //3pi0
		bratioEta[2] = 22.70;  //pi+pi-pi0
		bratioEta[3] = 4.69;   //pi+pi-gamma
		bratioEta[4] = 0.60;   //e+e-gamma
		bratioEta[5] = 4.4e-2; //pi02gamma

		modeEta[0] = 101;    //2gamma
		modeEta[1] = 70707;  //3pi0
		modeEta[2] = 80907;  //pi+pi-pi0
		modeEta[3] = 80901;  //pi+pi-gamma
		modeEta[4] = 30201;  //e+e-gamma
		modeEta[5] = 10107;  //pi02gamma
		gMC3->Gsdk(ipa, bratioEta, modeEta);

		Float_t bratioPi0[6];
		Int_t modePi0[6];

		for (Int_t kz = 0; kz < 6; ++kz) {
			bratioPi0[kz] = 0.;
			modePi0[kz] = 0;
		}

		ipa = 7;
		bratioPi0[0] = 98.798;
		bratioPi0[1] = 1.198;

		modePi0[0] = 101;
		modePi0[1] = 30201;

		gMC3->Gsdk(ipa, bratioPi0, modePi0);

		Int_t t = time(NULL);
		TRandom *rnd = new TRandom(t);
		gMC->SetRandom(rnd);
	}

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

	// -----   Start run   ----------------------------------------------------
	fRun->Run(nEvents);

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	std::cout << "Macro finished succesfully." << std::endl;
	std::cout << "Output file is "    << mcFile << std::endl;
	std::cout << "Parameter file is " << parFile << std::endl;
	std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << "s" << std::endl;
	std::cout << " Test passed" << std::endl;
	std::cout << " All ok " << std::endl;
}

