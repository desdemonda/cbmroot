void run_sim_delta(Int_t nEvents = 20000)
{
	Int_t iVerbose = 0;

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	gRandom->SetSeed(10);

	TString parFile = "", outFile ="";
	TString caveGeom = "", targetGeom = "", pipeGeom   = "",
			magnetGeom = "", mvdGeom    = "", mvdGeom = "";
	TString fieldMap = "";

	// Magnetic field
	Double_t fieldZ = 50.; // field centre z position
	Double_t fieldScale = 0.7; // field scaling factor

	parFile = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/70field/deltasource/param.delta.root";
	outFile = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/70field/deltasource/mc.delta.root";

   caveGeom = "cave.geo";
   targetGeom = "target_au_025mu.geo";
   pipeGeom = "pipe_standard.geo";
   mvdGeom = "mvd/mvd_v08a.geo";
   fieldMap = "field_v10e";
   magnetGeom = "passive/magnet_v09e.geo";

	gDebug = 0;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();

	gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
	cbmlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3");              // Transport engine
	fRun->SetOutputFile(outFile);          // Output file
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

	if ( targetGeom != "" ) {
		FairModule* target = new CbmTarget("Target");
		target->SetGeometryFileName(targetGeom);
		fRun->AddModule(target);
	}

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
	CbmFieldMap* magField = NULL;
   // -----   Create magnetic field   ----------------------------------------
   if (fieldMap == "field_electron_standard" || fieldMap == "field_v10e")
      magField = new CbmFieldMapSym2(fieldMap);
   else if (fieldMap == "field_muon_standard" )
      magField = new CbmFieldMapSym2(fieldMap);
   else if (fieldMap == "FieldMuonMagnet" )
      magField = new CbmFieldMapSym3(fieldMap);
   else {
      cout << "===> ERROR: Unknown field map " << fieldMap << endl;
      exit;
   }
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);

	// -----   Create PrimaryGenerator   --------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
   FairIonGenerator*  fIonGen = new FairIonGenerator(79, 197, 79, 1, 0.,0., 25., 0., 0., -1.);
   primGen->AddGenerator(fIonGen);
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

	// -----   Start run   ----------------------------------------------------
	fRun->Run(nEvents);

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
	cout << " Test passed" << endl;
	cout << " All ok " << endl;
	exit(0);
}

