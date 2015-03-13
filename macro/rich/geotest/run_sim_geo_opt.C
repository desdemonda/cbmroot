void run_sim_geo_opt(Int_t nEvents = 200)
{
   TTree::SetMaxTreeSize(90000000000);

   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   //TString inFile = "/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.00001.root";
   TString outDir = "/Users/slebedev/Development/cbm/data/simulations/rich/urqmdtest/";
   TString parFile =  outDir + "25gev.centr.param.0000.root";
   TString outFile = outDir + "25gev.centr.mc.0000.root";

   TString caveGeom = "cave.geo";
   TString pipeGeom   = "pipe/pipe_standard.geo";
   TString magnetGeom = "magnet/magnet_v12a.geo";
   TString stsGeom = "";//"sts/sts_v13d.geo.root";
   TString richGeom= "rich/rich_v08a.geo";
   TString fieldMap = "field_v12a";
   Double_t fieldZ = 50.; // field center z position
   Double_t fieldScale =  1.0; // field scaling factor

	if (script == "yes") {
		outFile = TString(gSystem->Getenv("MC_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
		caveGeom = TString(gSystem->Getenv("CAVE_GEOM"));
		pipeGeom = TString(gSystem->Getenv("PIPE_GEOM"));
		stsGeom = TString(gSystem->Getenv("STS_GEOM"));
		richGeom = TString(gSystem->Getenv("RICH_GEOM"));
		fieldMap = TString(gSystem->Getenv("FIELD_MAP"));
		magnetGeom = TString(gSystem->Getenv("MAGNET_GEOM"));
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

   fRun->SetMaterials("media.geo"); // Materials

   if ( caveGeom != "" ) {
      FairModule* cave = new CbmCave("CAVE");
      cave->SetGeometryFileName(caveGeom);
      fRun->AddModule(cave);
   }

   if ( pipeGeom != "") {
      FairModule* pipe = new CbmPipe("PIPE");
      pipe->SetGeometryFileName(pipeGeom);
      fRun->AddModule(pipe);
   }

	CbmTarget* target = new CbmTarget("Gold", 0.025); // 250 mum
	fRun->AddModule(target);

   if ( magnetGeom != "") {
      FairModule* magnet = new CbmMagnet("MAGNET");
      magnet->SetGeometryFileName(magnetGeom);
      fRun->AddModule(magnet);
   }

   if ( stsGeom != "") {
      FairDetector* sts = new CbmStsMC(kTRUE);
      sts->SetGeometryFileName(stsGeom);
      fRun->AddModule(sts);
   }

   if ( richGeom != "") {
      FairDetector* rich = new CbmRich("RICH", kTRUE);
      rich->SetGeometryFileName(richGeom);
      fRun->AddModule(rich);
   }

   CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);

   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

    // e+/-
	FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 1);
	boxGen1->SetPtRange(0.,3.);
	boxGen1->SetPhiRange(0.,360.);
	boxGen1->SetThetaRange(2.5,25.);
	boxGen1->SetCosTheta();
	boxGen1->Init();
	primGen->AddGenerator(boxGen1);

	FairBoxGenerator* boxGen2 = new FairBoxGenerator(-11, 1);
	boxGen2->SetPtRange(0.,3.);
	boxGen2->SetPhiRange(0.,360.);
	boxGen2->SetThetaRange(2.5,25.);
	boxGen2->SetCosTheta();
	boxGen2->Init();
	primGen->AddGenerator(boxGen2);


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
   cout << "Output file is "    << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}
