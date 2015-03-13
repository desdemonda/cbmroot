//---------------------------------------------------------------------------------
// @author  M. Zyzak
// @version 1.0
// @since   15.08.14
// 
// macro to reconstruct particles from signal events by KFParticleFinder
//_________________________________________________________________________________

void physSignal(int nEventsToRun = 10000;) {

  TStopwatch timer;
  timer.Start();

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
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------

  const int firstEventToRun = 0;
  const int lastEventToRun = firstEventToRun + nEventsToRun - 1;

  TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
  TString name = "signal";

  TString inFile  = name + ".mc.root";
  TString parFile = name + ".params.root";
  TString bgFile  =  name + ".mc.root";
  TString recFile =  name + ".reco.root";

  TString outFile = "out.root";

  TList *parFileList = new TList();

  TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par";
  parFileList->Add(&stsDigiFile);


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(recFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

 //          Adjust this part according to your requirements
  CbmKF *KF = new CbmKF();
  run->AddTask(KF);

  CbmL1* l1 = new CbmL1("CbmL1",1, 3);
  const TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v13d.root";
  l1->SetMaterialBudgetFileName(stsMatBudgetFileName.Data());
  run->AddTask(l1);

    // ----- PID for KF Particle Finder --------------------------------------------
  CbmKFParticleFinderPID* kfParticleFinderPID = new CbmKFParticleFinderPID();
  kfParticleFinderPID->SetPIDMode(1);
  run->AddTask(kfParticleFinderPID);
  
  // ----- KF Particle Finder --------------------------------------------
  CbmKFParticleFinder* kfParticleFinder = new CbmKFParticleFinder();
  kfParticleFinder->SetPIDInformation(kfParticleFinderPID);
  kfParticleFinder->SetPVToZero();
  run->AddTask(kfParticleFinder);

  // ----- KF Particle Finder QA --------------------------------------------
  CbmKFParticleFinderQA* kfParticleFinderQA = new CbmKFParticleFinderQA("CbmKFParticleFinderQA", 0, kfParticleFinder->GetTopoReconstructor());
  kfParticleFinderQA->SetPrintEffFrequency(100);
  run->AddTask(kfParticleFinderQA);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
//  run->LoadGeometry();
  run->Init();
  cout << "Starting run" << endl;
  run->Run(firstEventToRun,lastEventToRun+1);
  // ------------------------------------------------------------------------

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
