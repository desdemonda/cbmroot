void eff(Int_t file_nr=0, Int_t nEvents=1, Int_t pdg=3122){
  Char_t filenr[4];
  sprintf(filenr,"%04d",file_nr);
  printf("Filenr: %s\n", filenr);

  TString signal;
  if      (pdg==3122) signal = "la";
  else if (pdg==3312) signal = "xi";
  else if (pdg==3334) signal = "om";
  else exit(1);

  TString inDir = TString(filenr);
  inDir+="/";
  TString mcFile   = inDir+signal+".mc.root";
  TString parFile  = inDir+signal+".par.root";
  TString rcFile   = inDir+signal+".rc.root";
  TString efFile   = inDir+signal+".ef.root";

  Int_t iVerbose = 0;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();

  FairRunAna* run= new FairRunAna();
  run->SetInputFile(mcFile);
  run->AddFriend(rcFile);
  run->SetOutputFile(efFile);

  CbmAnaHypRecEff* eff = new CbmAnaHypRecEff(pdg,iVerbose);
  run->AddTask(eff);

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  run->Init();
  run->Run(0, nEvents);
}
