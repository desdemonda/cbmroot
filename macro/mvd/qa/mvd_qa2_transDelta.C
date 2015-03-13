 // --------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// CBM setup with MVD only
//
// V. Friese   06/02/2007
//
// --------------------------------------------------------------------------

Int_t mvd_qa2_transDelta()
{
  // ========================================================================
  //          Adjust this part according to your requirements



     // Input file
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  //  TString inFile  = inDir + "/input/urqmd.ftn14";


 
  // Number of events
  Int_t   nEvents = 50;


  // Output path
  TString outDir="./data/";   
  TString outpath ="./data/";

  //output file
  TString outfile = "mvd.mcDelta.root";

  //output filename
  TString outFile = outpath + outfile;

  // Parameter file name
  TString parFile = outpath + "params.root";

  // Cave geometry
  TString caveGeom = "cave.geo";

  // Target geometry
  CbmTarget* target = new CbmTarget("Gold", 0.025);

  // Beam pipe geometry
  TString pipeGeom = "pipe/pipe_v13a.geo.root";

  // Magnet geometry and field map
  TString magnetGeom  = "magnet/magnet_v12b.geo.root";
  TString fieldMap    = "field_v12b";
  Double_t fieldZ     = 40.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // MVD geometry
     TString mvdGeom = "mvd/mvd_v14a.geo.root";
//     TString stsGeom = "sts_Standard_s3055AAFK5.SecD.geo";

  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


  // -----   Create geometry   ----------------------------------------------
  FairModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);
  fRun->AddModule(cave);

  FairModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  fRun->AddModule(pipe);
  
  fRun->AddModule(target);

  FairModule* magnet= new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);
  fRun->AddModule(magnet);

  FairDetector* mvd= new CbmMvd("MVD", kTRUE);
  mvd->SetGeometryFileName(mvdGeom);
   mvd->SetMotherVolume("pipevac1");
  fRun->AddModule(mvd);


  //FairDetector* sts= new CbmSts("STS", kTRUE);
  //sts->SetGeometryFileName(stsGeom);
  //fRun->AddModule(sts);
  // ------------------------------------------------------------------------


  // -----   Create magnetic field   ----------------------------------------
  if ( fieldMap == "field_electron_standard")
    magField = new CbmFieldMapSym2(fieldMap);
  else if ( fieldMap == "field_v10e")
    magField = new CbmFieldMapSym2(fieldMap);
  else if ( fieldMap == "FieldAlligator" )
    magField = new CbmFieldMapSym2(fieldMap);
  else if ( fieldMap == "FieldMuonMagnet" )
    magField = new CbmFieldMapSym3(fieldMap);
  else if ( fieldMap == "field_v12b" )
    magField = new CbmFieldMapSym3(fieldMap);
  else {
    cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
    exit;
  }
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------



  FairIon *fIon = new FairIon("My_Au", 79, 197, 79, 25.,183.47324);
  fRun->AddNewIon(fIon);

  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen   = new FairPrimaryGenerator();
  //FairAsciiGenerator*   signalGen = new FairAsciiGenerator(D0asciiFile);


  FairIonGenerator*     fIongen   = new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);

  //primGen->AddGenerator(urqmdGen);
 // primGen->AddGenerator(signalGen);
   primGen->AddGenerator(fIongen);

  fRun->SetGenerator(primGen);


  // ------------------------------------------------------------------------


 
  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------

  
  
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
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;

}

