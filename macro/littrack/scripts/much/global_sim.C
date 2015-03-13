/**
 * \file global_sim.C
 * \brief Macro runs simulation for "electron" or "muon" setups of CBM.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#include <iostream>
using std::cout;
using std::endl;

void global_sim(Int_t nEvents = 100, Int_t seed = 555)
{
	gRandom->SetSeed(seed);

   TTree::SetMaxTreeSize(90000000000);
	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

	// Files
	TString urqmdFile  = "/Users/andrey/Development/cbm/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14"; // input UrQMD file
	TString dir = "events/much_anna_omega_8gev_10k/"; // Directory for output simulation files
	TString mcFile = dir + "mc.0000.root"; //MC file name
	TString parFile = dir + "param.0000.root"; //Parameter file name
	TString plutoFile = "/Users/andrey/Development/cbm/d/pluto/omega.8gev.1M.root";

	// Geometries
   TString caveGeom = "cave.geo";
	TString targetGeom = "target/target_au_250mu.geo";
	TString pipeGeom = "pipe/pipe_much.geo";
	TString shieldGeom = "shield_standard.geo";
	TString stsGeom = "sts/sts_v12b.geo.root";
	TString muchGeom = "much/much_v12c.geo";
	TString trdGeom = "";//"trd_muon_setup_new.geo";
	TString tofGeom = "tof/tof_v13b.geo.root";
	TString fieldMap = "field_v12a";
	TString magnetGeom = "magnet/magnet_v12a.geo";


	// If SCRIPT environment variable is set to "yes", i.e. macro is run via script
	if (script == "yes") {
	   urqmdFile  = TString(gSystem->Getenv("LIT_URQMD_FILE"));
	   mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
	   parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
	   plutoFile = TString(gSystem->Getenv("LIT_PLUTO_FILE"));

      caveGeom = TString(gSystem->Getenv("LIT_CAVE_GEOM"));
      targetGeom = TString(gSystem->Getenv("LIT_TARGET_GEOM"));
      pipeGeom = TString(gSystem->Getenv("LIT_PIPE_GEOM"));
      shieldGeom = TString(gSystem->Getenv("LIT_SHIELD_GEOM"));
      stsGeom = TString(gSystem->Getenv("LIT_STS_GEOM"));
      muchGeom = TString(gSystem->Getenv("LIT_MUCH_GEOM"));
      trdGeom = TString(gSystem->Getenv("LIT_TRD_GEOM"));
      tofGeom = TString(gSystem->Getenv("LIT_TOF_GEOM"));
      fieldMap = TString(gSystem->Getenv("LIT_FIELD_MAP"));
      magnetGeom = TString(gSystem->Getenv("LIT_MAGNET_GEOM"));
	}
	// -----   Magnetic field   -----------------------------------------------
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale = 1.; // field scaling factor

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunSim* run = new FairRunSim();
	run->SetName("TGeant3"); // Transport engine
	run->SetOutputFile(mcFile); // Output file
	FairRuntimeDb* rtdb = run->GetRuntimeDb();

	run->SetMaterials("media.geo"); // Materials
	//run->SetStoreTraj(kTRUE);

	if ( caveGeom != "" ) {
		FairModule* cave = new CbmCave("CAVE");
		cave->SetGeometryFileName(caveGeom);
		run->AddModule(cave);
	}

	if ( pipeGeom != "" ) {
		FairModule* pipe = new CbmPipe("PIPE");
		pipe->SetGeometryFileName(pipeGeom);
		run->AddModule(pipe);
	}

	if ( shieldGeom != "" ) {
		FairModule* shield = new CbmShield("SHIELD");
		shield->SetGeometryFileName(shieldGeom);
		run->AddModule(shield);
	}

	if ( targetGeom != "" ) {
		FairModule* target = new CbmTarget("Target");
		target->SetGeometryFileName(targetGeom);
		run->AddModule(target);
	}

	if ( magnetGeom != "" ) {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		run->AddModule(magnet);
	}

	if ( stsGeom != "" ) {
		FairDetector* sts = new CbmSts("STS", kTRUE);
		sts->SetGeometryFileName(stsGeom);
		run->AddModule(sts);
	}

	if ( muchGeom != "" ) {
		FairDetector* much = new CbmMuch("MUCH", kTRUE);
		much->SetGeometryFileName(muchGeom);
		run->AddModule(much);
	}

	if ( trdGeom != "" ) {
		FairDetector* trd = new CbmTrd("TRD",kTRUE );
		trd->SetGeometryFileName(trdGeom);
		run->AddModule(trd);
	}

	if ( tofGeom != "" ) {
		FairDetector* tof = new CbmTof("TOF", kTRUE);
		tof->SetGeometryFileName(tofGeom);
		run->AddModule(tof);
	}
	// ------------------------------------------------------------------------

   // -----   Create magnetic field   ----------------------------------------
   CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   run->SetField(magField);
   // ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	if (urqmdFile != "") {
	   CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(urqmdFile);
	   primGen->AddGenerator(urqmdGen);
	}

	if (plutoFile != "") {
	   FairPlutoGenerator* plutoGen = new FairPlutoGenerator(plutoFile);
      primGen->AddGenerator(plutoGen);
	}

	run->SetGenerator(primGen);
	run->Init();

	// -----   Runtime database   ---------------------------------------------
	CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
	fieldPar->SetParameters(magField);
	fieldPar->setChanged();
	fieldPar->setInputVersion(run->GetRunId(),1);
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
	parOut->open(parFile.Data());
	rtdb->setOutput(parOut);
	rtdb->saveOutput();
	rtdb->print();
	// ------------------------------------------------------------------------

	// -----   Start run   ----------------------------------------------------
	run->Run(nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << "Macro finished successfully." << endl;
	cout << "Test passed"<< endl;
   cout << " All ok " << endl;
	cout << "Output file is " << mcFile << endl;
	cout << "Real time used: " << timer.RealTime() << "s " << endl;
	cout << "CPU time used : " << timer.CpuTime() << "s " << endl;
	// ------------------------------------------------------------------------
}
