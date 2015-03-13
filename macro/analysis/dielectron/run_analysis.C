/** run_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 1.0
 **/

void run_analysis(Int_t nEvents = 1000)
{
   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   //gRandom->SetSeed(10);
	TString dir = "/hera/cbm/users/slebedev/mc/dielectron/sep13/25gev/trd/1.0field/nomvd/rho0/";
	TString mcFile = dir + "mc.auau.25gev.centr.00001.root";
	TString parFile = dir + "/params.auau.25gev.centr.00001.root";
	TString recoFile = dir + "/reco.auau.25gev.centr.00001.root";
	TString analysisFile = dir + "/test.analysis.test.auau.25gev.centr.00001.root";

  // TString parFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.param.root";
  // TString mcFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.mc.root";
  // TString recoFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.reco.root";
  // TString analysisFile = "/Users/slebedev/Development/cbm/data/simulations/lmvm/test.analysis.root";

	TString energy = "25gev";
	TString plutoParticle = "rho0";
	Double_t pionMisidLevel = -1.;
   Double_t trdAnnCut = 0.85;

   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v12b_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13g.digi.par"; // TRD digi file
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par"; // TRD digi file

   TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("MC_FILE"));
      recoFile = TString(gSystem->Getenv("RECO_FILE"));
      parFile = TString(gSystem->Getenv("PAR_FILE"));
      analysisFile = TString(gSystem->Getenv("ANALYSIS_FILE"));
      pionMisidLevel = TString(gSystem->Getenv("PION_MISIDENTIFICATION_LEVEL")).Atof();
      energy = TString(gSystem->Getenv("ENERGY"));
      plutoParticle = TString(gSystem->Getenv("PLUTO_PARTICLE"));
      trdAnnCut = TString(gSystem->Getenv("TRD_ANN_CUT")).Atof();
      stsMatBudgetFileName = TString(gSystem->Getenv("STS_MATERIAL_BUDGET_FILE"));
      stsDigiFile = TString(gSystem->Getenv("STS_DIGI"));
      trdDigiFile = TString(gSystem->Getenv("TRD_DIGI"));
      tofDigiFile = TString(gSystem->Getenv("TOF_DIGI"));
   }

    parFileList->Add(&stsDigiFile);
   if (trdDigiFile.GetString() != "") parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);

   // load libraries
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();
   gSystem->Load("libAnalysis");
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

   TStopwatch timer;
   timer.Start();

   FairRunAna* fRun = new FairRunAna();
   fRun->SetName("TGeant3");
   fRun->SetInputFile(mcFile);
   fRun->SetOutputFile(analysisFile);
   fRun->AddFriend(recoFile);

   //CbmKF is needed for Extrapolation
   CbmKF* kf = new CbmKF();
   fRun->AddTask(kf);

   CbmL1* l1 = new CbmL1();
    l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
   fRun->AddTask(l1);

   CbmAnaDielectronTask *task = new CbmAnaDielectronTask();
   if (energy == "8gev" || energy == "10gev") {
      // weight rho0 = Multiplicity * Branching Ratio = 9 * 4.7e-5 for 10 AGeV beam energy
      if (plutoParticle == "rho0") task->SetWeight(9 * 4.7e-5);
      // weight omega = Multiplicity * Branching Ratio = 19 * 7.28e-5 for 10 AGeV beam energy
      if (plutoParticle == "omegaepem" ) task->SetWeight(19 * 7.28e-5);
      // weight omega = Multiplicity * Branching Ratio = 19 * 7.7e-4 for 10 AGeV beam energy
      if (plutoParticle == "omegadalitz") task->SetWeight(19 * 7.7e-4);
      // weight phi = Multipli0city * Branching Ratio = 0.12 * 2.97e-4 for 10 AGeV beam energy
      if (plutoParticle == "phi") task->SetWeight(0.12 * 2.97e-4);
   } else if (energy == "25gev") {
      // weight rho0 = Multiplicity * Branching Ratio = 23 * 4.7e-5 for 25 AGeV beam energy
      if (plutoParticle == "rho0") task->SetWeight(23 * 4.7e-5);
      // weight omega = Multiplicity * Branching Ratio = 38 * 7.28e-5 for 25 AGeV beam energy
      if (plutoParticle == "omegaepem" ) task->SetWeight(38 * 7.28e-5);
      // weight omega = Multiplicity * Branching Ratio = 38 * 7.7e-4 for 25 AGeV beam energy
      if (plutoParticle == "omegadalitz") task->SetWeight(38 * 7.7e-4);
      // weight phi = Multiplicity * Branching Ratio = 1.28 * 2.97e-4 for 25 AGeV beam energy
      if (plutoParticle == "phi") task->SetWeight(1.28 * 2.97e-4);
   } else if (energy == "3.5gev") {
      // weight rho0 = Multiplicity * Branching Ratio = 1.0 * 4.7e-5 for 25 AGeV beam energy
      if (plutoParticle == "rho0") task->SetWeight(1.0 * 4.7e-5);
      // weight omega = Multiplicity * Branching Ratio = 1.2 * 7.28e-5 for 25 AGeV beam energy
      if (plutoParticle == "omegaepem" ) task->SetWeight(1.2 * 7.28e-5);
      // weight omega = Multiplicity * Branching Ratio = 1.2 * 5.9e-4 for 25 AGeV beam energy
      if (plutoParticle == "omegadalitz") task->SetWeight(1.2 * 7.7e-5);
      // weight phi = Multiplicity * Branching Ratio = 0.1 * 2.97e-4 for 25 AGeV beam energy
      if (plutoParticle == "phi") task->SetWeight(0.1 * 2.97e-4);
   }
   //task->SetChiPrimCut(2.);
   //task->SetMomentumCut(momentumCut); // if cut < 0 then it is not used
   task->SetUseMvd(IsMvd(parFile));
   task->SetUseRich(true);
   task->SetUseTrd(IsTrd(parFile));
   task->SetUseTof(true);
   task->SetPionMisidLevel(pionMisidLevel);
   task->SetTrdAnnCut(trdAnnCut);

   fRun->AddTask(task);


  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

   fRun->Init();
   fRun->Run(0, nEvents);

   timer.Stop();
   std::cout << "Macro finished succesfully." << std::endl;
   std::cout << "Output file is " << analysisFile << std::endl;
   std::cout << "Parameter file is " << parFile << std::endl;
   std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
   std::cout << " Test passed" << std::endl;
   std::cout << " All ok " << std::endl;
}
