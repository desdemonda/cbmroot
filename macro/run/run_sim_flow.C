// --------------------------------------------------------------------------
//
// Macro for transport simulation for the sake of collective flow studies, using UrQMD or SHIELD input and GEANT4
// CBM setup with (MVD,) STS, (RICH, TRD, TOF and ECAL), PSD (detector in brackets not included by default)
//
// 3 output files, with: 1) transported events 2) run parameters 3) PSD module XY coordinates in LAB (target) system (ASCII)
//
// Selim, 22/10/14
//
// --------------------------------------------------------------------------

// TO CHECK in the code: IMPORTANT NOTE

void run_sim_flow(Int_t nEvents = 2, Int_t En=25, const char* setup = "sis300_electron")
{
  // ========================================================================
  //          Adjust this part according to your requirements

  // if kbeam==kTRUE, transport the beam for estimating required PSD x-shift
  // if kbeam==kFALSE, transport particles from input models, 
  // gen=0 : UrQMD
  bool kbeam = kFALSE;
  Int_t gen = 0;

  cout << "gen = " << gen << endl;

//  Float_t psdZpos = 800.;   // (in cm); default: 8m at SIS100, 15m at SIS300

  TString numEvt = "";
  numEvt.Form("%04i", nEvents);

  // ----- Paths and file names  --------------------------------------------

  TString inputDir = gSystem->Getenv("VMCWORKDIR");

  TString outDir = "./data/";
  TString outFile = outDir + "mc_" + numEvt +  "evt.root";
  TString parFile = outDir + "params_" + numEvt +  "evt.root";

  TString setupFile = inputDir + "/geometry/setup/" + setup + "_setup.C";
  TString setupFunct = setup;
  setupFunct += "_setup()";
  
  gROOT->LoadMacro(setupFile);
  gInterpreter->ProcessLine(setupFunct);

  // UNIGEN data format for UrQMD
  TString inFile;
  inFile.Form("%s/input/urqmd.auau.%igev.mbias.root", inputDir.Data(), En);

  cout << "inFile: " << inFile << endl;
  cout << "outFile: " << outFile << endl;

  // -----  Overwrite geometries defined in setup ----------------------
  pipeGeom   = "pipe/pipe_v14c.root";
  mvdGeom    = "";
  richGeom   = "";
  trdGeom    = "";
  tofGeom    = "";

  CbmTarget* target = new CbmTarget("Gold", 0.025);

  cout << "STS geo : " << stsGeom << endl;

  // -----   Magnetic field   -----------------------------------------------
  // field scaling factor
  if (En == 35) fieldScale = 1.;
  if (En == 25) fieldScale = 1.;
  if (En == 15) fieldScale = 1.;
  if (En == 10) fieldScale = 1.;             
  if (En == 8)  fieldScale = 0.818;
  if (En == 6)  fieldScale = 0.632;
  if (En == 4)  fieldScale = 0.632;    
  if (En == 2)  fieldScale = 0.5; 

  cout << "Field scaling factor: " << fieldScale << endl;

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
  fRun->SetName("TGeant4");              // Transport engine
                                         // IMPORTANT NOTE: need G4 for hadronic calorimetry in PSD including projectile fragments (produced in SHIELD)
                                         // IMPORTANT NOTE: change physics list (in gconfig/g4Config.C) to either FTFP_BERT or QGSP_BIC_HP (both tested)
  fRun->SetOutputFile(outFile);          // Output file
  fRun->SetGenerateRunInfo(kTRUE);       // Create FairRunInfo file
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
    mvd->SetMotherVolume("pipevac1");
    fRun->AddModule(mvd);
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmStsMC(kTRUE);
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

  // ======== PSD part

  // PSD X-shift (in cm) vs evergy (2, 4, 6, 10, 15, 25, 35) AGeV vs Z-position (6, 8, 10, 15)m
  Float_t psdXshiftvsZ[7][4];
  Float_t psdXshift;
  int energy_ind;
  int distance_ind;

  // WARNING: PSD X-shift below given for field v10e !!

  // 2 agev
  psdXshiftvsZ[0][0] = 21.4;   // 8m
  psdXshiftvsZ[0][1] = 27.3;   // 10m
  psdXshiftvsZ[0][2] = 33.1;   // 12m
  psdXshiftvsZ[0][3] = 41.9;   // 15m

  // 4 agev
  psdXshiftvsZ[1][0] = 15.5;
  psdXshiftvsZ[1][1] = 19.8;
  psdXshiftvsZ[1][2] = 24;
  psdXshiftvsZ[1][3] = 30.4;

  // 6 agev
  psdXshiftvsZ[2][0] = 10.9;
  psdXshiftvsZ[2][1] = 13.9;
  psdXshiftvsZ[2][2] = 16.9;
  psdXshiftvsZ[2][3] = 21.4;

  // 10 agev
  psdXshiftvsZ[3][0] = 10.9;
  psdXshiftvsZ[3][1] = 13.9;
  psdXshiftvsZ[3][2] = 16.9;
  psdXshiftvsZ[3][3] = 21.4;

  // 15 agev
  psdXshiftvsZ[4][0] = 7.5;
  psdXshiftvsZ[4][1] = 9.5;
  psdXshiftvsZ[4][2] = 11.6;
  psdXshiftvsZ[4][3] = 14.7;

  // 25 agev
  psdXshiftvsZ[5][0] = 4.6;
  psdXshiftvsZ[5][1] = 5.8;
  psdXshiftvsZ[5][2] = 7.1;
  psdXshiftvsZ[5][3] = 9;

  // 35 agev
  psdXshiftvsZ[6][0] = 3.3;
  psdXshiftvsZ[6][1] = 4.2;
  psdXshiftvsZ[6][2] = 5.1;
  psdXshiftvsZ[6][3] = 6.5;

  

  if (En == 2) energy_ind = 0;
  if (En == 4) energy_ind = 1;
  if (En == 6) energy_ind = 2;
  if (En == 10) energy_ind = 3;
  if (En == 15) energy_ind = 4;
  if (En == 25) energy_ind = 5;
  if (En == 35) energy_ind = 6;

  if (psdZpos == 800.) distance_ind = 0;
  if (psdZpos == 1000.) distance_ind = 1;
  if (psdZpos == 1200.) distance_ind = 2;
  if (psdZpos == 1500.) distance_ind = 3;

  psdXshift = psdXshiftvsZ[energy_ind][distance_ind];

  CbmPsdv1_44mods_hole6cm* psd= new CbmPsdv1_44mods_hole6cm("PSD", kTRUE);

  // ========= Acceptance PSD & FD method
  //CbmPsdWoutv1_for44mods_full_holes* psd= new CbmPsdWoutv1_for44mods_full_holes("PSD", kTRUE);
  //CbmPsdWoutv1_for44mods_sub1* psd= new CbmPsdWoutv1_for44mods_sub1("PSD", kTRUE);

  psd->SetZposition(psdZpos); // in cm
  psd->SetXshift(psdXshift);  // in cm    
  TString geoFileNamePsd = outDir + "psd_geo_xy_" + numEvt + "evt.txt";
  psd->SetGeoFile(geoFileNamePsd);
  fRun->AddModule(psd);

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

  if (kbeam == kFALSE) {
    
    CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(inFile);
    urqmdGen->SetEventPlane(-TMath::Pi(), TMath::Pi());
    // IMPORTANT NOTE: event plane angle in [-pi, pi] by convention
    // TO DO: rotation should be done in FairPrimaryGenerator, not from UnigenGenerator -> same for all event generators
    
    primGen->AddGenerator(urqmdGen);
  } else {
    
    Double_t bMom;
    if (En == 35) bMom = 35.926;
    if (En == 25) bMom = 25.92;
    if (En == 15) bMom = 15.91;
    if (En == 10) bMom = 10.898; 
    if (En == 8) bMom = 8.88889;
    if (En == 6) bMom = 6.87454;
    if (En == 4) bMom = 4.84832;
    if (En == 2) bMom = 2.78444;
    
    int Nion;
    int pileup = 100;
    Nion = nEvents*pileup;
    
    FairIonGenerator *fIongen= new FairIonGenerator(79, 197, 79, Nion, 0., 0., bMom, 0., 0., -1.); 
    primGen->AddGenerator(fIongen);   
    
    nEvents = 1;
  }

   // Include beam emittance
   //primGen->SmearVertexZ(kTRUE);
   //primGen->SmearVertexXY(kTRUE);
   //primGen->SetBeam(0., 0., 0.15, 0.06, 2.2e-3, 2e-3); // emittance (SIS100) @ 10 AGeV ~ 2.2 mm.mrad (X) -> deltaX = +/- 1 mm && thetaX = +/- 2.2 mrad

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

  CbmStack* stack = (CbmStack*) FairMCApplication::Instance()->GetStack();
  //stack->SetMinPoints(0);                                                    //======== BTW, ask Volker if SetMinPoints & StoreSecondaries can be set indep. for diff. detector
  //stack->StoreSecondaries(kFALSE);

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
  //TString geoFileName = outDir + "geo/geofile_" + numEvt + "evt_" + sfileNum + ".root"; 
  //fRun->CreateGeometryFile(geoFileName);

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

  //gApplication->Terminate();
}

