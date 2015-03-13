/**
 * Performs standard transport simulation with GEANT3
 * in the CBM MuCh setup: STS + MUCH.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param inputSignal    Input file containing signal
 * @param inputBgr       Input file containing background
 * @param outFile        Output file for transport data
 * @param nEvents        Number of events to process
 */

#include <iostream>
using namespace std;

void much_sim(TString inputSignal = "",
              TString inputBgr = "",
              TString outFile = "",
              Int_t nEvents = 2)
{
  // ========================================================================
  //          Adjust this part according to your requirements

  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  if (inputSignal == "") {
    inputSignal = inputdir + "/macro/much/data/jpsi.root";
  }
  if (inputBgr == "") {
    inputBgr = inputdir + "/input/urqmd.ftn14";
  }
  if (outFile == "") {
    outFile = "data/mc.root";
  }
  TString parFile = "data/params.root";

  // -----   Confirm input parameters    ------------------------------------
  cout << endl;
  cout << "========  CBMROOT Macro much_sim  =================" << endl;
  cout << "First input file  is " << inputSignal  << endl;
  cout << "Second input file is " << inputBgr << endl;
  cout << "Output file       is " << outFile << endl;
  cout << "Events to process:   " << nEvents << endl;
  cout << "===================================================" << endl;
  cout << endl;

  // -----   Specify MUCH related geometry   --------------------------------
  // Use pipe_much.geo for the beam pipe in both cases.
  // In case you want the additional W shielding around the pipe,
  // use shield_standard.geo or shield_compact.geo, respective to the
  // MUCH geometry. Otherwise, define an empty string.
//  TString muchGeom   = "much/much_v11a.geo"; // default rectangular geometry
  TString muchGeom   = "much/much_v12b.geo"; // default sector geometry
  TString pipeGeom   = "pipe/pipe_much.geo";
  TString shieldGeom = "shield_standard.geo";

  // -----   Other geometries   ---------------------------------------------
  TString caveGeom   = "cave.geo";
  CbmTarget* target = new CbmTarget("Gold", 0.025);
  TString magnetGeom = "magnet/magnet_v12b.geo.root";
  TString stsGeom    = "sts/sts_v13d.geo.root";

  // -----   Magnetic field   -----------------------------------------------
  TString  fieldMap   = "field_v12b";   // name of field map
  Double_t fieldZ     = 50.;                 // field center z position
  Double_t fieldScale =  1.;                 // field scaling factor

  // In general, the following parts need not be touched
  // ========================================================================


  // -----   Set unique random generator seed   -----------------------------
  // Comment this out if you want to have a defined seed for reproducibility.
  gRandom->SetSeed(1);
  // ------------------------------------------------------------------------


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Create simulation run   ----------------------------------------
  cout << endl << "=== much_sim.C : Creating run and database ..." << endl;
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------

  // -----   Create media   -------------------------------------------------
  cout << endl << "=== much_sim.C : Set materials ..." << endl;
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------

  // -----   Create detectors and passive volumes   -------------------------
  cout << endl << "=== much_sim.C : Create geometry ..." << endl;
  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
    cout << "    --- " << caveGeom << endl;
  }

  if ( pipeGeom != "" ) {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    cout << "    --- " << pipeGeom << endl;
    fRun->AddModule(pipe);
  }

  if ( target ) fRun->AddModule(target);

  if ( shieldGeom != "" ) {
    FairModule* shield = new CbmShield("SHIELD");
    shield->SetGeometryFileName(shieldGeom);
    cout << "    --- " << shieldGeom << endl;
    fRun->AddModule(shield);
  }

  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    cout << "    --- " << magnetGeom << endl;
    fRun->AddModule(magnet);
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmStsMC(kTRUE);
    sts->SetGeometryFileName(stsGeom);
    cout << "    --- " << stsGeom << endl;
    fRun->AddModule(sts);
  }

  if ( muchGeom != "" ) {
    FairDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    cout << "    --- " << muchGeom << endl;
    fRun->AddModule(much);
  }
  // ------------------------------------------------------------------------

  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------

  // -----   Create PrimaryGenerator   --------------------------------------
  cout << endl << "=== much_sim.C : Create generators ..." << endl;
//  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  if ( inputSignal != "" ) {
  	CbmPlutoGenerator *plutoGen= new CbmPlutoGenerator(inputSignal);
  	primGen->AddGenerator(plutoGen);
  }
  if ( inputBgr != "" ) {
  	FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inputBgr);
  	primGen->AddGenerator(urqmdGen);
  }
  fRun->SetGenerator(primGen);
  // ------------------------------------------------------------------------


  // -----   Run initialization   -------------------------------------------
  cout << endl << "=== much_sim.C : Initialize run ..." << endl;
  fRun->Init();
  // ------------------------------------------------------------------------


  // -----   Runtime database   ---------------------------------------------
  cout << endl << "=== much_sim.C : Set up database ..." << endl;
  cout << "                Parameters will be saved to output file" << endl;
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  cout << endl << "=== much_sim.C : Starting timer ..." << endl;
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Start run   ----------------------------------------------------
  cout << endl << "=== much_sim.C : Start run ..." << endl;
  fRun->Run(nEvents);
  // Write geometry
  fRun->CreateGeometryFile("geometry.root");
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "=== much_sim.C : Macro finished successfully." << endl;
  cout << "=== much_sim.C : Output file is " << outFile << endl;
  cout << "=== much_sim.C : Real time used: " << rtime << "s " << endl;
  cout << "=== much_sim.C : CPU time used : " << ctime << "s " << endl;
  cout << endl << endl;
    // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}

