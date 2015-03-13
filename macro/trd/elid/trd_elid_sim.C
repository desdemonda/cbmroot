void trd_elid_sim(Int_t nEvents = 200)
{
   Double_t minMomentum = 1.5; //minimum momentum
   Double_t maxMomentum = 8.0; //maximum momentum
   Double_t thetaMin = 2.5;
   Double_t thetaMax = 25.;

	TString outFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.mc.root";
	TString parFile = "/Users/slebedev/Development/cbm/data/simulations/trd/elid/piel.0000.params.root";

	// -----  Geometries  -----------------------------------------------------
   TString caveGeom = "cave.geo";
   TString pipeGeom = "pipe/pipe_standard.geo";
   TString stsGeom = "";//"sts/sts_v12b.geo.root";
   TString richGeom = "";// "rich/rich_v08a.geo";
   TString trdGeom = "trd/trd_v14a_3e.geo.root";
   TString fieldMap = "field_v12a";
   TString magnetGeom = "magnet/magnet_v12a.geo";
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale = 1.; // field scaling factor

   gDebug = 0;

	TStopwatch timer;
	timer.Start();

   // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();

   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunSim* fRun = new FairRunSim();
   fRun->SetName("TGeant3"); // Transport engine
   fRun->SetOutputFile(outFile);
   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

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

   if ( stsGeom != "" ) {
      FairDetector* sts = new CbmStsMC("STS", kTRUE);
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

   // -----   Create magnetic field   ----------------------------------------
   CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);

   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	Int_t kfCode1 = 11; // electrons
	Int_t kfCode2 = -11; // positrons

	FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 200);
	boxGen1->SetPRange(minMomentum, maxMomentum);
//	boxGen1->SetXYZ(50., 50., 450.);
	boxGen1->SetPhiRange(0., 360.);
	boxGen1->SetThetaRange(thetaMin, thetaMax);
	boxGen1->SetCosTheta();
	boxGen1->Init();
	primGen->AddGenerator(boxGen1);

	FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, 200);
	boxGen2->SetPRange(minMomentum, maxMomentum);
//	boxGen2->SetXYZ(50., 50., 450.);
	boxGen2->SetPhiRange(0., 360.);
	boxGen2->SetThetaRange(thetaMin, thetaMax);
	boxGen2->SetCosTheta();
	boxGen2->Init();
	primGen->AddGenerator(boxGen2);

	FairBoxGenerator* boxGen3 = new FairBoxGenerator(211, 200);
	boxGen3->SetPRange(minMomentum, maxMomentum);
//	boxGen3->SetXYZ(50., 50., 450.);
	boxGen3->SetPhiRange(0., 360.);
	boxGen3->SetThetaRange(thetaMin, thetaMax);
	boxGen3->SetCosTheta();
	boxGen3->Init();
	primGen->AddGenerator(boxGen3);

	FairBoxGenerator* boxGen4 = new FairBoxGenerator(-211, 200);
	boxGen4->SetPRange(minMomentum, maxMomentum);
//	boxGen4->SetXYZ(50., 50., 450.);
	boxGen4->SetPhiRange(0., 360.);
	boxGen4->SetThetaRange(thetaMin, thetaMax);
	boxGen4->SetCosTheta();
	boxGen4->Init();
	primGen->AddGenerator(boxGen4);

	fRun->SetGenerator(primGen);

	fRun->Init();

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

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
}

