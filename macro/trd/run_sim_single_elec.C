// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with MVD, STS, RICH, TRD, TOF and ECAL
//
// V. Friese   22/02/2007
//
// 20130605 - checked by DE
// --------------------------------------------------------------------------

void run_sim_single_elec(Int_t nEvents = 1)
{

  // ========================================================================
  // geometry selection for sim + reco  by Cyrano                            
  // ========================================================================
  ifstream whichTrdGeo;
  whichTrdGeo.open("whichTrdGeo",ios::in);
  TString selectGeo;
  if (whichTrdGeo) whichTrdGeo >> selectGeo;
  TString digipar = selectGeo(0,9);
  digipar.ReplaceAll(".","");
  cout << "selected geometry : >> " << selectGeo << " << (to select a different geometry, edit macro/trd/whichTrdGeo file)" << endl;
  cout << "selected digipar  : >> " << digipar << " << " << endl;
  whichTrdGeo.close();
  if (digipar.Length() == 0) digipar = "trd_standard";

  // ========================================================================
  //          Adjust this part according to your requirements

  // ----- Paths and file names  --------------------------------------------
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  TString inFile  = inDir + "/input/urqmd.ftn14";
  //TString inFile  = inDir + "/input/urqmd.auau.25gev.centr.0000.ftn14";
  TString outDir  = "data";
  TString outFile = outDir + "/test.mc.root";
  TString parFile = outDir + "/params.root";
  
  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  CbmTarget* target = new CbmTarget("Gold", 0.025);
  TString pipeGeom   = "pipe/pipe_standard.geo";
  TString magnetGeom = "magnet/magnet_v12b.geo.root";
  TString mvdGeom    = "mvd/mvd_v07a.geo";
  TString stsGeom    = "sts/sts_v12b.geo.root";
  TString richGeom   = "rich/rich_v08a.geo";
  //  TString trdGeom    = "trd/trd_v13o.root";
  TString trdGeom    = "trd/" + selectGeo;
  //  TString tofGeom    = "tof/tof_v13b.root";
  TString tofGeom    = "";
  TString ecalGeom   = "";
//  TString ecalGeom   = "ecal/ecal_v08a.geo";
  
  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "field_v12b";    // name of field map
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

  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
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
  
  if ( target ) fRun->AddModule(target);

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

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
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

  if ( tofGeom != "" ) {
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
  }
  
  if ( ecalGeom != "" ) {
    FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data()); 
    fRun->AddModule(ecal);
  }
  
  // ------------------------------------------------------------------------



  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
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

  // Use the FairBoxGenerator which generates a soingle electron
  FairBoxGenerator *eminus = new FairBoxGenerator();
  eminus->SetPDGType(11);
  eminus->SetMultiplicity(1);
  //  eminus->SetBoxXYZ(-400.,-400.,400.,400.,405.);  // z=405 for SIS100 setup
  eminus->SetBoxXYZ(32.,-32.,32.,-32.,405.);  // shoot at corner of diagonal modules
  eminus->SetPRange(2.,2.);
  eminus->SetPhiRange(0.,360.);
  eminus->SetThetaRange(0.,0.);
  primGen->AddGenerator(eminus);

//  FairBoxGenerator *e2 = new FairBoxGenerator();
//  e2->SetPDGType(11);
//  e2->SetMultiplicity(1);
//  e2->SetBoxXYZ(-32.,32.,-32.,32.,405.);  // shoot at corner of diagonal modules
//  e2->SetPRange(2.,2.);
//  e2->SetPhiRange(0.,360.);
//  e2->SetThetaRange(0.,0.);
//  primGen->AddGenerator(e2);

  FairBoxGenerator *e3 = new FairBoxGenerator();
  e3->SetPDGType(11);
  e3->SetMultiplicity(1);
  e3->SetBoxXYZ(-82.,82.,-82.,82.,405.);  // shoot at corner of diagonal module
  e3->SetPRange(2.,2.);
  e3->SetPhiRange(0.,360.);
  e3->SetThetaRange(0.,0.);
  primGen->AddGenerator(e3);

//  // Use the CbmUrqmdGenrator which calculates a reaction plane and
//  // rotate all particles accordingly
//  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
//  urqmdGen->SetEventPlane(0. , 360.);
//  primGen->AddGenerator(urqmdGen);

  fRun->SetGenerator(primGen);       
  // ------------------------------------------------------------------------

 
  // -Trajectories Visualization (TGeoManager Only )
  // Switch this on if you want to visualize tracks in the
  // eventdisplay.
  // This is normally switch off, because of the huge files created
  // when it is switched on. 
  fRun->SetStoreTraj(kTRUE);

  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------
  
  // Set cuts for storing the trajectories.
  // Switch this on only if trajectories are stored.
  // Choose this cuts according to your needs, but be aware
  // that the file size of the output file depends on these cuts

   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
   trajFilter->SetStepSizeCut(0.01); // 1 cm
   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
   trajFilter->SetStorePrimaries(kTRUE);
   trajFilter->SetStoreSecondaries(kTRUE);

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

