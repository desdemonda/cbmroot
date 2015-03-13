void run_sim(Int_t nEvents = 100000)
{
   TString script = TString(gSystem->Getenv("SCRIPT"));

   TString asciiInput = "/Users/slebedev/Development/cbm/data/simulations/richprototype/epi.ascii.dat";
   TString outDir = "/Users/slebedev/Development/cbm/data/simulations/richprototype/";
   TString outFile = outDir + "/mc.root";
   TString parFile = outDir + "/params.root";

   TString caveGeom = "cave.geo";
   TString richGeom = "rich/prototype/RichPrototype-1Mir-Standard.geo";

   if (script == "yes") {
      asciiInput = TString(gSystem->Getenv("IN_ASCII_FILE"));
      outFile = TString(gSystem->Getenv("MC_FILE"));
      parFile = TString(gSystem->Getenv("PAR_FILE"));
      caveGeom = TString(gSystem->Getenv("CAVE_GEOM"));
      richGeom = TString(gSystem->Getenv("RICH_GEOM"));
   }

   gDebug = 0;

   TStopwatch timer;
   timer.Start();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   // Create simulation run
   FairRunSim* fRun = new FairRunSim();
   fRun->SetName("TGeant3");// Transport engine
   fRun->SetOutputFile(outFile); // Output file
   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

   // Create media
   fRun->SetMaterials("media.geo");

   // Create detectors and passive volumes
   if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
   }
   if ( richGeom != "" ) {
    FairDetector* rich = new CbmRichProt("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    fRun->AddModule(rich);
   }

   // Create PrimaryGenerator
   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

   FairAsciiGenerator* asciiGen = new FairAsciiGenerator(asciiInput);
   primGen->AddGenerator(asciiGen);

   fRun->SetGenerator(primGen);
  // fRun->SetStoreTraj(kTRUE);
   fRun->Init();


   //((CbmStack*)FairMCApplication::Instance()->GetStack())->SetMinPoints(0);

   /*
   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
   trajFilter->SetStepSizeCut(0.01); // 1 cm
   trajFilter->SetVertexCut(-2000., -2000., -2000., 2000., 2000., 2000.);
   trajFilter->SetMomentumCutP(0.); // p_lab > 0
   trajFilter->SetEnergyCut(0., 10.); // 0 < Etot < 10 GeV
   trajFilter->SetStorePrimaries(kTRUE);
   trajFilter->SetStoreSecondaries(kTRUE);
   */

   Bool_t kParameterMerged = kTRUE;
   FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
   parOut->open(parFile.Data());
   rtdb->setOutput(parOut);
   rtdb->saveOutput();
   rtdb->print();

   fRun->Run(nEvents);
   fRun->CreateGeometryFile("geofile.rich.prot.standard.root");

   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}

