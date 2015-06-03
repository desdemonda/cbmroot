/**
 * \file global_sim.C
 * \brief Macro runs simulation for "electron" or "muon" setups of CBM.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#include <iostream>
using std::cout;
using std::endl;

void global_sim(Int_t nEvents = 5)
{
   TTree::SetMaxTreeSize(90000000000);
	TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

	// Specify "electron" or "muon" setup of CBM
	//TString setup = "muon";
	TString setup = "electron";

	// Event parameters
	Int_t nofMuonsPlus = 0; // number of embedded muons from FairBoxGenerator
	Int_t nofMuonsMinus = 0; // number of embedded muons from FairBoxGenerator
	Int_t nofElectrons = 0; // number of embedded electrons from FairBoxGenerator
	Int_t nofPositrons = 0; // number of embedded positrons from FairBoxGenerator
	Int_t nofPionsPlus = 0; // number of embedded pions from FairBoxGenerator
	Int_t nofPionsMinus = 0; // number of embedded pions from FairBoxGenerator
	Int_t nofJPsiToMuons = 0; // number of embedded J/Psi particles decaying to mu+ and mu-
	Int_t nofJPsiToElectrons = 10; // number of embedded J/Psi particles decaying to e+ and e-
	Int_t nofAuIons = 0; // number of generated Au ions
	TString urqmd = "yes"; // If "yes" than UrQMD will be used as background
    TString unigen = "no"; // If "yes" than CbmUnigenGenerator will be used instead of FairUrqmdGenerator
    TString pluto = "no"; // If "yes" PLUTO generator will be used

	// Files
	// TString urqmdFile  = "/Users/andrey/Development/cbm/prod/gen/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.00001.root"; // input UrQMD file
	TString urqmdFile = TString(gSystem->Getenv("VMCWORKDIR")) +TString("/input/urqmd.ftn14");
	TString dir = "events/trd_v13p_3e/"; // Directory for output simulation files
	TString mcFile = dir + "mc.0000.root"; //MC file name
	TString parFile = dir + "param.0000.root"; //Parameter file name
	TString plutoFile = "/Users/andrey/Development/cbm/d/pluto/omega.25gev.1M.root";

	// Geometries
	TString caveGeom = "", //targetGeom = "",
			pipeGeom = "", shieldGeom = "",
			mvdGeom = "", stsGeom = "", muchGeom = "", richGeom = "", trdGeom = "",
			tofGeom = "", ecalGeom = "", fieldMap = "", magnetGeom = "";
	if (setup == "muon") {
		caveGeom   = "cave.geo";
		//targetGeom = "target/target_au_250mu.geo";
		pipeGeom   = "pipe/pipe_v13c.geo.root";
		shieldGeom = "shield_standard.geo";
		mvdGeom    = "";//"mvd/mvd_v07a.geo";
		stsGeom    = "sts/sts_v13d.geo.root";
		muchGeom   = "much/much_v13e.geo"; // v13f
		trdGeom    = "";//"trd_muon_setup_new.geo";
		tofGeom    = "";//"tof/tof_v13b.geo.root";
		fieldMap   = "field_v12a";
		magnetGeom = "magnet/magnet_v12b.geo.root";
	} else if (setup == "electron") {
		caveGeom   = "cave.geo";
		//targetGeom = "target/target_au_250mu.geo";
		pipeGeom   = "pipe/pipe_v13a.geo.root";
		mvdGeom    = "";//"mvd/mvd_v07a.geo.root";
		stsGeom    = "sts/sts_v13d.geo.root";
		richGeom   = "rich/rich_v08a.geo";
		trdGeom    = "trd/trd_v13p_3e.geo.root";
		tofGeom    = "tof/tof_v13b.geo.root";
		ecalGeom   = "";//"ecal_FastMC.geo";
		fieldMap   = "field_v12a";
		magnetGeom = "magnet/magnet_v12a.geo";
	}

	// If SCRIPT environment variable is set to "yes", i.e. macro is run via script
	if (script == "yes") {
		urqmdFile  = TString(gSystem->Getenv("LIT_URQMD_FILE"));
		mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
		parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
		plutoFile = TString(gSystem->Getenv("LIT_PLUTO_FILE"));

		nofMuonsPlus = TString(gSystem->Getenv("LIT_NOF_MUONS_PLUS")).Atoi();
		nofMuonsMinus = TString(gSystem->Getenv("LIT_NOF_MUONS_MINUS")).Atoi();
		nofElectrons = TString(gSystem->Getenv("LIT_NOF_ELECTRONS")).Atoi();
		nofPositrons = TString(gSystem->Getenv("LIT_NOF_POSITRONS")).Atoi();
		nofPionsPlus = TString(gSystem->Getenv("LIT_NOF_PIONS_PLUS")).Atoi();
		nofPionsMinus = TString(gSystem->Getenv("LIT_NOF_PIONS_MINUS")).Atoi();
		nofJPsiToMuons = TString(gSystem->Getenv("LIT_NOF_JPSI_TO_MUONS")).Atoi();
		nofJPsiToElectrons = TString(gSystem->Getenv("LIT_NOF_JPSI_TO_ELECTRONS")).Atoi();
		nofAuIons = TString(gSystem->Getenv("LIT_NOF_AU_IONS")).Atoi();
		urqmd = TString(gSystem->Getenv("LIT_URQMD"));
        unigen = TString(gSystem->Getenv("LIT_UNIGEN"));
        pluto = TString(gSystem->Getenv("LIT_PLUTO"));

		caveGeom = TString(gSystem->Getenv("LIT_CAVE_GEOM"));
		//targetGeom = TString(gSystem->Getenv("LIT_TARGET_GEOM"));
		pipeGeom = TString(gSystem->Getenv("LIT_PIPE_GEOM"));
		shieldGeom = TString(gSystem->Getenv("LIT_SHIELD_GEOM"));
		mvdGeom = TString(gSystem->Getenv("LIT_MVD_GEOM"));
		stsGeom = TString(gSystem->Getenv("LIT_STS_GEOM"));
		muchGeom = TString(gSystem->Getenv("LIT_MUCH_GEOM"));
		richGeom = TString(gSystem->Getenv("LIT_RICH_GEOM"));
		trdGeom = TString(gSystem->Getenv("LIT_TRD_GEOM"));
		tofGeom = TString(gSystem->Getenv("LIT_TOF_GEOM"));
		ecalGeom = TString(gSystem->Getenv("LIT_ECAL_GEOM"));
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

	//if ( targetGeom != "" ) {
	//	FairModule* target = new CbmTarget("Target");
	//	target->SetGeometryFileName(targetGeom);
		CbmTarget* target = new CbmTarget("Gold", 0.025);
		run->AddModule(target);
	//}

	if ( magnetGeom != "" ) {
		FairModule* magnet = new CbmMagnet("MAGNET");
		magnet->SetGeometryFileName(magnetGeom);
		run->AddModule(magnet);
	}

	if ( mvdGeom != "" ) {
		FairDetector* mvd = new CbmMvd("MVD", kTRUE);
		mvd->SetGeometryFileName(mvdGeom);
                mvd->SetMotherVolume("pipevac1");
		run->AddModule(mvd);
	}

	if ( stsGeom != "" ) {
		FairDetector* sts = new CbmStsMC(kTRUE);
		sts->SetGeometryFileName(stsGeom);
		run->AddModule(sts);
	}

	if ( richGeom != "" ) {
		FairDetector* rich = new CbmRich("RICH", kTRUE);
		rich->SetGeometryFileName(richGeom);
		run->AddModule(rich);
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

	if ( ecalGeom != "" ) {
		FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
		run->AddModule(ecal);
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

	if (urqmd == "yes" && unigen == "yes") {
		CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(urqmdFile);
		primGen->AddGenerator(urqmdGen);
	}

	if (urqmd == "yes" && unigen != "yes") {
	   FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(urqmdFile);
       primGen->AddGenerator(urqmdGen);
	}

	if (pluto == "yes") {
	   FairPlutoGenerator* plutoGen = new FairPlutoGenerator(plutoFile);
       primGen->AddGenerator(plutoGen);
	}

	if (nofJPsiToMuons > 0) {
      CbmLitPolarizedGenerator *polGen;
      polGen = new CbmLitPolarizedGenerator(443, nofJPsiToMuons);
      polGen->SetDistributionPt(0.176);        // 25 GeV
      polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
      polGen->SetRangePt(0.,3.);
      polGen->SetRangeY(1.,3.);
      polGen->SetBox(0);
      polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
      polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiMuon);
      polGen->SetAlpha(0);
      polGen->Init();
      primGen->AddGenerator(polGen);
	}

   if (nofJPsiToElectrons > 0) {
      CbmLitPolarizedGenerator *polGen;
      polGen = new CbmLitPolarizedGenerator(443, nofJPsiToElectrons);
      polGen->SetDistributionPt(0.176);        // 25 GeV
      polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
      polGen->SetRangePt(0.,3.);
      polGen->SetRangeY(1.,3.);
      polGen->SetBox(0);
      polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
      polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiElectron);
      polGen->SetAlpha(0);
      polGen->Init();
      primGen->AddGenerator(polGen);
   }

   if (nofMuonsPlus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(-13, nofMuonsPlus);
		boxGen->SetPRange(1.5, 16.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5, 25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
   }

   if (nofMuonsMinus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(13, nofMuonsMinus);
		boxGen->SetPRange(1.5, 16.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5, 25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofElectrons > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(11, nofElectrons);
		boxGen->SetPtRange(0.,3.);
//		boxGen->SetPRange(30, 30);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofPositrons > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(-11, nofPositrons);
		boxGen->SetPtRange(0.,3.);
//		boxGen->SetPRange(30, 30);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofPionsPlus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(211, nofPionsPlus);
//		boxGen1->SetPtRange(0.,3.);
		boxGen->SetPRange(1., 2.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofPionsMinus > 0) {
		FairBoxGenerator* boxGen = new FairBoxGenerator(-211, nofPionsMinus);
//		boxGen->SetPtRange(0.,3.);
		boxGen->SetPRange(1, 2.);
		boxGen->SetPhiRange(0.,360.);
		boxGen->SetThetaRange(2.5,25.);
		boxGen->SetCosTheta();
		boxGen->Init();
		primGen->AddGenerator(boxGen);
	}

	if (nofAuIons >0) {
		FairIonGenerator *AuGen = new FairIonGenerator(79, 197, 79, nofAuIons, 0., 0., 25., 0., 0., -0.1);
		primGen->AddGenerator(AuGen);
	}


	run->SetGenerator(primGen);
	run->Init();

	cout << "creating CbmFieldPar" << endl;

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
