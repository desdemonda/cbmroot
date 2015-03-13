void run_CbmIonGenerator(Int_t nEvents = 1)
{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Input file
  //TString inPath = "/d/cbm03/urqmd/auau/25gev/centr/";
  //TString inFile = inPath + "urqmd.auau.25gev.centr.0000.ftn14";
  
  // Output file
  TString outFile = Form("sts.mc.root",nEvents);
  
  // Parameter file
  TString parFile = Form("params.root",nEvents);

  // Cave geometry
  TString caveGeom = "cave.geo";
  
  // Target geometry
  TString targetGeom = "target_au_250mu.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe_standard.geo";

  // Magnet geometry and field map
  TString magnetGeom = "passive/magnet_v09e.geo";
  TString fieldMap    = "field_v10e";
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // MVD geometry
  TString mvdGeom    = "mvd/mvd_v07a.geo";

  // STS geometry
  TString stsGeom    = "sts/sts_v11a.geo";
  
  //STS geometry for the same z position of all sensors
  //TString stsGeom = "sts_same_z.geo";
  
  targetGeom = "";
  magnetGeom = "";
  stsGeom  = "";
  mvdGeom  = "";
  
  // In general, the following parts need not be touched
  // ========================================================================

  
  

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libCbmGenerators"); // for CbmIonGenerator
  // ------------------------------------------------------------------------

   
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName("TGeant3");              // Transport engine
  run->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  // ------------------------------------------------------------------------
  
  

  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------

    
  
  // -----   Create geometry   ----------------------------------------------
  FairModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);
  run->AddModule(cave);

  FairModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  run->AddModule(pipe);
  
  if( targetGeom != "") {
    FairModule* target= new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    run->AddModule(target);
  }
  if( magnetGeom != "") {
    FairModule* magnet= new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    run->AddModule(magnet);
  }
  if( mvdGeom != ""){
    FairDetector* mvd= new CbmMvd("MVD", kTRUE);
    mvd->SetGeometryFileName(mvdGeom); 
    run->AddModule(mvd);
  }
  if( stsGeom != "") {
    FairDetector* sts= new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom); 
    run->AddModule(sts);
  }
  // ------------------------------------------------------------------------

  
  
  // -----   Create magnetic field   ----------------------------------------
  if(magnetGeom!="") {
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
    magField->SetPosition(0., 0., fieldZ);
    magField->SetScale(fieldScale);
    run->SetField(magField);
  }
  // ------------------------------------------------------------------------

  
  
  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  //FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  //primGen->AddGenerator(urqmdGen);
  
  // CbmIonGenerator - check in Load libraries: gSystem->Load("libCbmGenerators");
  Int_t nions=10; // number of ions/event
  cout<<"@@@@@> CbmIonGenerator is ON with "<<nions<<" ion(s)/event !!!"<<endl;
  Int_t z=79, a=197, q=79; // Au-ion
  Double_t p=8.; // in AGeV/c
  Double_t vz=-0.0126; // in cm - z-pos. of vertex
  Double_t meanX=0.3;  // in cm - spatial distr. (XOY)
  Double_t meanY=-0.2; // in cm
  Double_t sigmaX = 0.078; // in cm - spatial distr. (XOY)
  Double_t sigmaY = 0.032; // in cm
  Double_t sigmatX = 0.00117885; // in rad (Px/P) - angular distr. (mean=0)
  Double_t sigmatY = 0.00094955; // in rad (Py/P)
  // Parameters of the trapezoid are set with respect to mean of Gaussian, NOT necesseraly in absolute coordinates.
  // x1 < x2 < 0 < x3 < x4
  Double_t x1=-0.10452, x2=-0.06942, x3=0.06942, x4=0.10452; // in cm - trapezoid distr.
  Double_t y1=-0.04448, y2=-0.02688, y3=0.02688, y4=0.04448; // in cm
  Double_t tX1=-0.00157966, tX2=-0.001049177, tX3=0.001049177, tX4=0.00157966; // in rad
  Double_t tY1=-0.00131987, tY2=-0.000797622, tY3=0.000797622, tY4=0.00131987; // in rad

//  CbmIonGenerator *IonGen = new CbmIonGenerator(z, a, q, nions, p, sigmaX, sigmaY, sigmatX, sigmatY);
  
//  CbmIonGenerator *IonGen = new CbmIonGenerator(z, a, q, nions, p, sigmaX, sigmaY, sigmatX, sigmatY, meanX, meanY, vz);
  
  CbmIonGenerator *IonGen = new CbmIonGenerator(z, a, q, nions, p, sigmaX, sigmaY, sigmatX, sigmatY, meanX, meanY, vz,
                                                x1, x2, x3, x4, y1, y2, y3, y4, tX1, tX2, tX3, tX4, tY1, tY2, tY3, tY4);
  
  primGen->AddGenerator(IonGen);
  
  run->SetGenerator(primGen);
  // ------------------------------------------------------------------------

//   run->SetStoreTraj(kTRUE);
  
  // -----   Initialize simulation run   ------------------------------------
  run->Init();
  // ------------------------------------------------------------------------

  
  
  // -----   Runtime database   ---------------------------------------------
  if(magnetGeom!="") {
    CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    fieldPar->setInputVersion(run->GetRunId(),1);
  }
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
  //run->CreateGeometryFile("data/geofile_full.root");
  
  
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;

  exit(0);
}
