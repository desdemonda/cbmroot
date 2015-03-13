// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with MVD, STS, RICH, TRD, TOF and ECAL
//
// V. Friese   22/02/2007
//
// --------------------------------------------------------------------------

void run_sim_with_destructors(Int_t nEvents = 4,
                   TString inDir="$VMCWORKDIR/input", 
                   TString inFile="urqmd.ftn14",
                   TString outDir="data/",
                   TString outFile="test.mc.root", 
                   TString parFile="params.root")
{

  // ----- setup path and file names  ------------------------------------
  // If any part of the directory path contains a environment
  // variable this part is replaced by the content of the variable

   TString inDirFinal;
   if (inDir.BeginsWith("/")) inDirFinal+="/";

   TStringToken* subPath = new TStringToken(inDir,"/");  
   while (subPath->NextToken()) {
     if (subPath->Contains("$")) {
       TString envVariable=subPath->ReplaceAll("$","");
       inDirFinal +=gSystem->Getenv(envVariable);
       inDirFinal +="/";
     } else {
       inDirFinal +=*subPath;
       inDirFinal +="/";
     }
   }
   inDir=inDirFinal;


   TString outDirFinal;
   subPath = new TStringToken(outDir,"/");  
   if (outDir.BeginsWith("/")) outDirFinal+="/";
   while (subPath->NextToken()) {
     if (subPath->Contains("$")) {
       TString envVariable=subPath->ReplaceAll("$","");
       outDirFinal +=gSystem->Getenv(envVariable);
       outDirFinal +="/";
     } else {
       outDirFinal +=*subPath;
       outDirFinal +="/";
     }
   }
   outDir=outDirFinal;
  
  inFile = inDir + inFile;
  outFile = outDir + outFile;
  parFile = outDir + parFile;
  TString geoFile = outDir + "geofile_full.root";

  // Writes file with the memory consumption of the macro
  // If you need this information uncomment the following block
   Int_t Interval=5;
   Int_t PID=gSystem->GetPid();
   cout<<"PID: "<<PID<<endl;
   TString cmdline="$VMCWORKDIR/scripts/check_memory.sh ";
   cmdline+= PID;
   cmdline+= " ";
   cmdline+= Interval;
   cmdline+= " ";
   cmdline+= outFile;
   cmdline+= "  &";
   cout<<cmdline<<endl;
   gSystem->Exec(cmdline);
  
  // ========================================================================
  //          Adjust this part according to your requirements

  // -----  Geometries  -----------------------------------------------------
  // If a part of the setup should be switched of comment the line with
  // the definition of the geometry file.

  TString caveGeom   = "cave.geo";
  TString targetGeom = "";
  TString pipeGeom   = "";
  TString magnetGeom = "";

  TString mvdGeom    = "";
  TString stsGeom    = "";
  TString richGeom   = "";
  TString trdGeom    = "";
  TString tofGeom    = "";
  TString ecalGeom   = "";

  targetGeom = "target_au_250mu.geo";
  pipeGeom   = "pipe_standard.geo";
  magnetGeom = "magnet_electron_standard.geo";
  mvdGeom    = "mvd_standard.geo";
  stsGeom    = "sts_standard.geo";
  richGeom   = "rich_standard.geo";
  trdGeom    = "trd_standard.geo";
  tofGeom    = "tof_standard.geo";
  ecalGeom   = "ecal_FastMC.geo";

  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "field_electron_standard";   // name of field map
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
  gSystem->Load("libFairTools");
  gSystem->Load("libFairDB");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libCbmGenerators");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
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
    cave=NULL;
    delete cave;
  }

  if ( pipeGeom != "" ) {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
    pipe=NULL;
    delete pipe;
  }
  
  if ( targetGeom != "" ) {
    FairModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
    target=NULL;
    delete target;
  }

  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
    magnet=NULL;
    delete magnet;
  }
  
  if ( mvdGeom != "" ) {
    FairDetector* mvd = new CbmMvd("MVD", kTRUE);
    mvd->SetGeometryFileName(mvdGeom);
    fRun->AddModule(mvd);
    mvd=NULL;
    delete mvd;
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
    sts=NULL;
    delete sts;
  }

  if ( richGeom != "" ) {
    FairDetector* rich = new CbmRich("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    fRun->AddModule(rich);
    rich=NULL;
    delete rich;
  }
  

  if ( trdGeom != "" ) {
    FairDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    fRun->AddModule(trd);
    trd=NULL;
    delete trd;
  }

  if ( tofGeom != "" ) {
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
    tof=NULL;
    delete tof;
  }
  
  if ( ecalGeom != "" ) {
    FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data()); 
    fRun->AddModule(ecal);
    ecal=NULL;
    delete ecal;
  }
  
  // ------------------------------------------------------------------------



  // -----   Create magnetic field   ----------------------------------------
  if (fieldMap == "field_electron_standard" ) 
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_muon_standard" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "FieldMuonMagnet" )
    CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
  else {
    cout << "===> ERROR: Unknown field map " << fieldMap << endl;
    exit;
  }
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------

  
  CbmMCEventHeader* mcHeader = new CbmMCEventHeader();
  fRun->SetMCEventHeader(mcHeader);
  mcHeader=NULL;
  delete mcHeader;

  /*
  CbmEventHeader* evHeader = new CbmEventHeader();
  evHeader->SetCbmRunId(-111);
  fRun->SetEventHeader(evHeader);
  evHeader=NULL;
  delete evHeader;
  */

  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
  urqmdGen->SetEventPlane(0., 360.);
  //FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  //primGen->AddGenerator(urqmdGen);
  fRun->SetGenerator(primGen);
  urqmdGen=NULL;
  delete urqmdGen;
  primGen=NULL;
  delete primGen;
       
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

  fieldPar=NULL;
  delete fieldPar;
  parOut=NULL;
  delete parOut;
  // has to be done after magField was handed to the parameter container
  magField=NULL;
  delete magField; 
  rtdb=NULL;
  delete rtdb;  
  // ------------------------------------------------------------------------

 
  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
  // ------------------------------------------------------------------------
  fRun->CreateGeometryFile("data/geofile_full.root");

  delete fRun;

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
  //  gSystem->Sleep(30000);// sleep 30 seconds
  exit(0);
}

