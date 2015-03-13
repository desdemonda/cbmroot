//---------------------------------------------------------------------------------
// @author  M. Zyzak
// @version 1.0
// @since   15.08.14
// 
// macro to simulate signal events for KFParticleFinder
//_________________________________________________________________________________

void simSignal(Int_t iParticle = 0, Int_t nEvents = 10000)
{
  // ========================================================================
  //          Adjust this part according to your requirements
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
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  
  // Number of events
  const bool useMvd = 0; // 2maps
  const bool sameZ = 0;
  const bool usePipe = 1;
  const int  nElectrons = 0;
  
  // Output name
  TString outName = "signal";

  // Output file name
  TString outFile = outName + ".mc.root";

  // Parameter file name
  TString parFile = outName +".params.root";

    // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString pipeGeom   = "pipe/pipe_v13b.geo.root"; //===>>>
  TString magnetGeom = "magnet/magnet_v12b.geo.root";
  TString mvdGeom    = "mvd/mvd_v07a.geo";

  TString stsGeom;
  if (sameZ) stsGeom    = "sts_same_z.geo";
  else stsGeom    = "sts/sts_v13d.geo.root";

  if (!useMvd) mvdGeom = "";
  if (!usePipe) pipeGeom = "";
  TString richGeom = "";
  TString trdGeom  = "";
  TString tofGeom = "";
  TString ecalGeom = "";

  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "field_v12b";   // name of field map
  Double_t fieldZ     = 50.;             // field centre z position
  Double_t fieldScale =  1.;             // field scaling factor
  
  // -----   Input file name   ----------------------------------------------  
  TString inFile = "Signal.txt";

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

  CbmTarget* target = new CbmTarget("Gold", 0.025);
  fRun->AddModule(target);
  
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
  FairAsciiGenerator*  asciiGen = new FairAsciiGenerator(inFile);
  primGen->AddGenerator(asciiGen);

  fRun->SetGenerator(primGen);
  
  
  KFPartEfficiencies eff;
  
  Double_t lifetime = eff.partLifeTime[iParticle]; // lifetime
  Double_t mass = eff.partMass[iParticle];
  Int_t PDG = eff.partPDG[iParticle];
  Double_t charge = eff.partCharge[iParticle];
  
  if(iParticle == 48 || iParticle ==49)
  {
    for(int iPall=48; iPall<50; iPall++)
    {
      Double_t lifetime = eff.partLifeTime[iPall]; // lifetime
      Double_t mass = eff.partMass[iPall];
      Int_t PDG = eff.partPDG[iPall];
      Double_t charge = eff.partCharge[iPall];
    
      FairParticle* newPartice = new FairParticle(PDG, eff.partTitle[iPall], kPTHadron, mass, charge,
              lifetime, "hadron", 0.0, 1, 1, 0, 1, 1, 0, 0, 1, kFALSE);
      fRun->AddNewParticle(newPartice);
    }
    TString pythia6Config = "/u/mzyzak/cbmtrunk/macro/kf/KFParticleFinderSignalTest/Signal/DecayConfig.C()";
    fRun->SetPythiaDecayer(pythia6Config);
  }

  fRun->Init();

  if(!(iParticle == 48 || iParticle ==49))
  {
    gMC->DefineParticle(PDG, eff.partTitle[iParticle], kPTHadron, mass, charge,
            lifetime, "hadron", 0.0, 1, 1, 0, 1, 1, 0, 0, 1, kFALSE);
    
    Int_t mode[6][3];
    Float_t bratio[6];

    for (Int_t kz = 0; kz < 6; kz++) {
      bratio[kz] = 0.;
      mode[kz][0] = 0;
      mode[kz][1] = 0;
      mode[kz][2] = 0;
    }
    bratio[0] = 100.;
    for(int iD=0; iD<eff.GetNDaughters(iParticle); iD++)
    {
      if(iD>2)
        continue;
      mode[0][iD] = eff.GetDaughterPDG(iParticle, iD); //pi+
    }
    
    gMC->SetDecayMode(PDG,bratio,mode);
  }
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
}

