void much_histo(TString anaFile, TString histoFile, int nEvents=1000){
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(anaFile);
  fRun->SetOutputFile(histoFile);

  CbmAnaDimuonHisto* anaHisto = new CbmAnaDimuonHisto("DimuonHisto",histoFile,nEvents);
//  anaHisto->SetMbinning(0.3, 1.1,80);
  anaHisto->SetMbinning(0.7,0.9,100);
  anaHisto->SetSignalPairs(10);
  anaHisto->SetMuchHitsCut(13);
  anaHisto->SetStsHitsCut(8);
  anaHisto->SetChiToVertexCut(3);

  fRun->AddTask(anaHisto);
  fRun->Init();
  fRun->Run(0,nEvents);
}
