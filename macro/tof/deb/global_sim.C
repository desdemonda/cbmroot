/**
 * \file global_sim.C
 * \brief Macro runs simulation for "electron" or "muon" setups of CBM.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#include <iostream>
using std::cout;
using std::endl;

void global_sim(Int_t nEvents = 10, Int_t seed = 555)
{
	gRandom->SetSeed(seed);

	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

	// Files
	//	TString urqmdFile  = "/Users/andrey/Development/cbm/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14"; // input UrQMD file
	TString urqmdFile  = "../../input/urqmd.auau.25gev.centr.0000.ftn14"; // input UrQMD file
	TString dir = "data/"; // Directory for output simulation files
	TString mcFile = dir + "mc.0000.root"; //MC file name
	TString parFile = dir + "param.0000.root"; //Parameter file name

	// Geometry
	TString caveGeom = "cave.geo";
	TString	targetGeom = "target_au_250mu.geo";
	TString	pipeGeom = "pipe_standard.geo";
	TString	stsGeom = "sts/sts_v12b.geo.root";
        TString richGeom = "rich/rich_v08a.geo";
	TString	tofGeom  = "tof/tof_V13b.geo";
	TString	fieldMap = "field_v12a";
	TString	magnetGeom = "passive/magnet_v12a.geo";

	// If SCRIPT environment variable is set to "yes", i.e. macro is run via script
	if (script == "yes") {
		urqmdFile  = TString(gSystem->Getenv("LIT_URQMD_FILE"));
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
	}
	// -----   Magnetic field   -----------------------------------------------
	Double_t fieldZ = 50.; // field center z position
	Double_t fieldScale = 1.; // field scaling factor

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	FairRunSim* fRun = new FairRunSim();
	fRun->SetName("TGeant3"); // Transport engine
	fRun->SetOutputFile(mcFile); // Output file
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	fRun->SetMaterials("media.geo"); // Materials
//	fRun->SetStoreTraj(kTRUE);

	if ( caveGeom != "" ) {
		FairModule* cave = new CbmCave("CAVE");
		cave->SetGeometryFileName(caveGeom);
		fRun->AddModule(cave);
		cout << "    --- " << caveGeom << endl;
	}

	if ( pipeGeom != "" ) {
		FairModule* pipe = new CbmPipe("PIPE");
		pipe->SetGeometryFileName(pipeGeom);
		fRun->AddModule(pipe);
		cout << "    --- " << pipeGeom << endl;
	}

	if ( targetGeom != "" ) {
		FairModule* target = new CbmTarget("Target");
		target->SetGeometryFileName(targetGeom);
		fRun->AddModule(target);
		cout << "    --- " << targetGeom << endl;
	}

	if ( magnetGeom != "" ) {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		fRun->AddModule(magnet);
		cout << "    --- " << magnetGeom << endl;
	}

	if ( stsGeom != "" ) {
		FairDetector* sts = new CbmSts("STS", kTRUE);
		sts->SetGeometryFileName(stsGeom);
		fRun->AddModule(sts);
		cout << "    --- " << stsGeom << endl;
	}

        if ( richGeom != "" ) {
		FairDetector* rich = new CbmRich("RICH", kTRUE);
		rich->SetGeometryFileName(richGeom);
		fRun->AddModule(rich);
	}

	if ( tofGeom != "" ) {
		FairDetector* tof = new CbmTof("TOF", kTRUE);
		tof->SetGeometryFileName(tofGeom);
		fRun->AddModule(tof);
		cout << "    --- " << tofGeom << endl;
	}
	// ------------------------------------------------------------------------

   // -----   Create magnetic field   ----------------------------------------
   CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
   magField->SetPosition(0., 0., fieldZ);
   magField->SetScale(fieldScale);
   fRun->SetField(magField);
   // ------------------------------------------------------------------------

   CbmMCEventHeader* mcHeader = new CbmMCEventHeader();
   fRun->SetMCEventHeader(mcHeader);

	// ------------------------------------------------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	//  CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(urqmdFile);
	CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(urqmdFile);
	primGen->AddGenerator(urqmdGen);
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
	// ------------------------------------------------------------------------

	// -----   Start run   ----------------------------------------------------
	fRun->Run(nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Test passed"<< endl;
   cout << " All ok " << endl;
	cout << "Output file is " << mcFile << endl;
	cout << "Real time used: " << rtime << "s " << endl;
	cout << "CPU time used : " << ctime << "s " << endl << endl << endl;
	// ------------------------------------------------------------------------
}
