tof_Ana100p(Int_t nEvents = 1000, Int_t iout=1)
{

  TString  InputFile     ="data/auaumbias.mc.root";
  TString  ParFile       ="data/auaumbias.params.root";

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
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libCbmDisplay");
  gSystem->Load("libHadron");

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

//  fRun->SetInputFile(InputFile.Data());
  fRun->SetInputFile("data/prot.mc.root");
  fRun->AddFriend("data/prot.eds.root");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  //histograms 
  fRun->SetOutputFile("data/prot.ana.root");
  TFile *fHist = fRun->GetOutputFile();

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
