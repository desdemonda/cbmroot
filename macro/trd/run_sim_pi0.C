// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with MVD, STS, RICH, TRD, TOF and ECAL
//
// V. Friese   22/02/2007
//
// --------------------------------------------------------------------------

void run_sim_pi0(Int_t nEvents = 1, Int_t urqmd = 0)
{
  if (nEvents > 500) nEvents = 500;
  // ========================================================================
  // geometry selection for sim + reco  by Cyrano
  // ========================================================================
  ifstream whichTrdGeo;
  whichTrdGeo.open("whichTrdGeo",ios::in);
  TString digipar;
  if (whichTrdGeo) whichTrdGeo >> digipar;
  cout << "selected geometry : >> " << digipar << " << (to select a different geometry, edit macro/trd/whichTrdGeo file)" << endl;
  whichTrdGeo.close();
  if (digipar.Length() == 0) digipar = "trd_standard";

  // ========================================================================
  //          Adjust this part according to your requirements

  // ----- Paths and file names  --------------------------------------------
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  //TString inFile  = inDir + "/input/urqmd.ftn14";
  TString urqmdFile;
  urqmdFile.Form("urqmd.auau.25gev.centr.%04i.ftn14",urqmd);
  TString inFile  = inDir + "/input/" + urqmdFile;
  TString outDir  = "data";
  TString outFile;
  outFile.Form("%s/test.mc.%04i.root",outDir.Data(),urqmd);
  TString parFile = outDir + "/params.root";
  TString parFile;// = "data/params.root";
  parFile.Form("%s/params.%04i.root",outDir.Data(),urqmd);
  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString magnetGeom = "passive/magnet_v09e.geo";
  TString mvdGeom    = "mvd/mvd_v07a.geo";
  TString stsGeom    = "sts/sts_v11a.geo";
  TString richGeom   = "rich/rich_v08a.geo";
  //  TString trdGeom    = "../macro/trd/trd_squared_modules.geo";
  //  TString trdGeom    = "trd/trd_v10b.geo";
  TString trdGeom    = "trd/" + digipar + ".geo";
  TString tofGeom    = "tof/tof_v07a.geo";
  //  TString ecalGeom   = "ecal/ecal_v08a.geo";
 
  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "field_v10e";    // name of field map
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
  /*
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
    gSystem->Load("libCbmData");
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
  */
  // -----------------------------------------------------------------------

 
 
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
  /*
    if ( ecalGeom != "" ) {
    FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data()); 
    fRun->AddModule(ecal);
    }
  */
  // ------------------------------------------------------------------------

  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------


  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);  
  primGen->AddGenerator(urqmdGen);
    
  FairBoxGenerator *pi0 = new FairBoxGenerator();
  pi0->SetPDGType(111);
  pi0->SetMultiplicity(1e3);
  pi0->SetBoxXYZ(-0.1,-0.1,0.1,0.1,0.);
  //pi0->SetPRange(0.,4.);
  pi0->SetPhiRange(0,360);
  pi0->SetThetaRange(2.5,25);
  pi0->SetPtRange(0.,3.);
  pi0->Init();
  primGen->AddGenerator(pi0);
  /*
    FairBoxGenerator *electron = new FairBoxGenerator();
    electron->SetPDGType(11);
    electron->SetMultiplicity(1e2);
    electron->SetBoxXYZ(-0.1,-0.1,0.1,0.1,0.);
    //electron->SetPRange(0.,4.);
    electron->SetPhiRange(0,360);
    electron->SetThetaRange(2.5,25);
    electron->SetPtRange(0.,3.);
    //electron->SetThetaRange(0.,3.);
    electron->Init();
    primGen->AddGenerator(electron);
  
    FairBoxGenerator *positron = new FairBoxGenerator();
    positron->SetPDGType(-11);
    positron->SetMultiplicity(1e2);
    positron->SetBoxXYZ(-0.1,-0.1,0.1,0.1,0.);
    //positron->SetPRange(0.,4.);
    positron->SetPhiRange(0,360);
    positron->SetThetaRange(2.5,25);
    positron->SetPtRange(0.,3.);
    //positron->SetThetaRange(0.,3.);
    positron->Init();
    primGen->AddGenerator(positron);
  
    FairBoxGenerator *gamma = new FairBoxGenerator();
    gamma->SetPDGType(22);
    gamma->SetMultiplicity(1e2);
    gamma->SetBoxXYZ(-0.1,-0.1,0.1,0.1,0.);
    gamma->SetPRange(0.,4.);
    //gamma->SetPtRange(0.,3.);
    gamma->SetPhiRange(0,360);
    gamma->SetThetaRange(2.5,25);
    //gamma->SetThetaRange(0.,3.);
    gamma->Init();
    primGen->AddGenerator(gamma);
  */
  fRun->SetGenerator(primGen);       
  // ------------------------------------------------------------------------

  // -Trajectories Visualization (TGeoManager Only )
  fRun->SetStoreTraj(kTRUE);

 
  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------

  // Set cuts for storing the trajectpries
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

