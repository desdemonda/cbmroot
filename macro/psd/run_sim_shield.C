
// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with MVD, STS, RICH, TRD, TOF and ECAL
//
// V. Friese   22/02/2007
//
// --------------------------------------------------------------------------

void run_sim_shield(Int_t nfile=1)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // ----- Paths and file names  --------------------------------------------
  // Number of events
  Int_t   nEvents = 1;
  TString numFile = "";
  numFile +=nfile;
  
  // TString outDir   = "/d/cbm04/alla/zdc10gev/Nov12/g3/";
  TString inFile =Form("/home/alla/cbm/shield/evt_%i_10gev.dat",nfile);
  //  TString outFile = outDir + "shield" + numFile +".mc.10m.calo.root";
  //  TString parFile = outDir + "shield" + numFile +".param.10m.calo.root";
  TString outFile = "shield" + numFile +".mc.10m.calo.root";
  TString parFile = "shield" + numFile +".param.10m.calo.root";
  
  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString pipeGeom   = "pipe_alla.geo";
  TString magnetGeom = "passive/magnet_v09e.geo";
  TString mvdGeom    = "mvd/mvd_v07a.geo";
  TString stsGeom    = "sts/sts_v11a.geo";
  TString richGeom   = "rich/rich_v08a.geo";
  TString trdGeom    = "trd/trd_v11c.geo";
  TString tofGeom    = "tof/tof_v07a.geo";
//  TString ecalGeom   = "ecal/ecal_v08a.geo";
 TString PsdGeom    = "Psd/Psd.geo";
  
  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "field_v10e";   // name of field map
  Double_t fieldZ     = 50.;             // field centre z position
  Double_t fieldScale =  1.;             // field scaling factor
  
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
  gSystem->Load("libCbmGenerators");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libLittrack");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libPsd");

  // -----------------------------------------------------------------------

 
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  //fRun->SetName("TGeant4");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


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
    //  fRun->AddModule(mvd);
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
  }

  if ( richGeom != "" ) {
    FairDetector* rich = new CbmRich("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    //   fRun->AddModule(rich);
  }
  

  if ( trdGeom != "" ) {
    FairDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    //   fRun->AddModule(trd);
  }

  if ( tofGeom != "" ) {
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    //   fRun->AddModule(tof);
  }
  
/*
  if ( ecalGeom != "" ) {
    FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data()); 
    fRun->AddModule(ecal);
  }
*/  
  CbmPsdv1* psd= new CbmPsdv1("PSD", kTRUE);
  // FairDetector* psd= new CbmPsdv1("PSD", kTRUE);
    psd->SetGeometryFileName(PsdGeom);
  psd->SetXshift(15.);
  psd->SetZposition(1060.);
  fRun->AddModule(psd);
  // ------------------------------------------------------------------------



  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------

  // Use theexperiment specific MC Event header instead of the default one
  // This one stores additional information about the reaction plane
  CbmMCEventHeader* mcHeader = new CbmMCEventHeader();
  fRun->SetMCEventHeader(mcHeader);

  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  // Use the CbmUrqmdGenrator which calculates a reaction plane and
  // rotate all particles accordingly
   CbmShieldGenerator* shieldGen = new CbmShieldGenerator(inFile);
   primGen->AddGenerator(shieldGen);
   //  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
   // urqmdGen->SetEventPlane(0. , 360.);
   // primGen->AddGenerator(urqmdGen);
  fRun->SetGenerator(primGen);       
  // ------------------------------------------------------------------------

 
  // -Trajectories Visualization (TGeoManager Only )
  // Switch this on if you want to visualize tracks in the
  // eventdisplay.
  // This is normally switch off, because of the huge files created
  // when it is switched on. 
  // fRun->SetStoreTraj(kTRUE);

  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------
  
  // Set cuts for storing the trajectories.
  // Switch this on only if trajectories are stored.
  // Choose this cuts according to your needs, but be aware
  // that the file size of the output file depends on these cuts

  // FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // trajFilter->SetStepSizeCut(0.01); // 1 cm
  // trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  // trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  // trajFilter->SetStorePrimaries(kTRUE);
  // trajFilter->SetStoreSecondaries(kTRUE);

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
  fRun->CreateGeometryFile("data/geofile_full.root");


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

