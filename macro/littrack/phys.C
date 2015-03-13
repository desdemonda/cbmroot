
void phys(Int_t nEvents = 100) {
   TStopwatch timer;
   timer.Start();

   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   // Input and output data
   TString dir = "/Users/andrey/Development/cbm/d/events/nu/25gev/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameters file
   TString globalRecoFile = dir + "global.reco.0000.root"; // Input file with reconstructed tracks and hits
   TString analysisFile = dir + "analysis.0000.root"; // Output file with analysis

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // STS digi file

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
   	  parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
      globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
      analysisFile = TString(gSystem->Getenv("LIT_ANALYSIS_FILE"));
      stsDigiFile = TString(gSystem->Getenv("LIT_STS_DIGI"));
   }

   parFileList->Add(&stsDigiFile);

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->AddFriend(globalRecoFile);
   run->SetOutputFile(analysisFile);
   // ------------------------------------------------------------------------

   CbmKF *KF = new CbmKF();
   run->AddTask(KF);

   CbmL1* l1 = new CbmL1("CbmL1", 1, 3);
   run->AddTask(l1);

   Float_t cuts[2][3] = {{3.,3.,-100.},{3.,3.,-100.}};
   CbmKFParticlesFinder* kfPartFinder = new CbmKFParticlesFinder(cuts, 2);
   run->AddTask(kfPartFinder);

   CbmKFParticlesFinderQA* kfPartFinderQA = new CbmKFParticlesFinderQA(kfPartFinder,1,3);
   kfPartFinderQA->SetOutFile("");
   run->AddTask(kfPartFinderQA);

   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(parFileList, "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   run->Init();
   run->Run(0, nEvents);

   timer.Stop();
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is "    << analysisFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time=" << timer.RealTime() << " seconds, CPU time=" << timer.CpuTime() << " seconds"<< endl;
}
