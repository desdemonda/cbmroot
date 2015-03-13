tof_Ana300(Int_t nEvents = 1000, Int_t iout=0)
{

  TString  InputFile     ="data/mc.0000.root";
  TString  ParFile       ="data/param.0000.root";


  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
  loadlibs();
  gSystem->Load("libHadron");

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

//  fRun->SetInputFile(InputFile.Data());
  fRun->SetInputFile("data/mc.0000.root");
  fRun->AddFriend("data/global.reco.0000.root");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  //histograms 
  fRun->SetOutputFile("data/global.ana.0000.root");

  CbmHadronAnalysis *HadronAna = new CbmHadronAnalysis(); // in hadron
  fRun->AddTask(HadronAna);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  cout << "Starting run" << endl;
  fRun->Run(0, nEvents);
  // ------------------------------------------------------------------------

// save histos to file
// TFile *fHist = new TFile("data/auaumbias.hst.root","RECREATE");
 cout << "Processing done, outflag =" <<iout << endl;
 if (iout==1){
 TFile *fHist = fRun->GetOutputFile();
 fHist->Write();
 if(0){ // explicit writing 
  TIter next(gDirectory->GetList());
  TH1 *h;
  TObject* obj;
  while(obj= (TObject*)next()){
      if(obj->InheritsFrom(TH1::Class())){
         h = (TH1*)obj;
         cout << "Write histo " << h->GetTitle() << endl;
         h->Write();
      }
  }
 }
 fHist->ls();
 fHist->Close();
 }
}
