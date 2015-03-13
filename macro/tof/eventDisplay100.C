eventDisplay100()
{

  TString  InputFile     ="data/auaumbias.root";
  TString  ParFile       ="data/params.root";


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
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

//  fRun->SetInputFile(InputFile.Data());
  fRun->SetInputFile("data/test.mc.root");
//  fRun->AddFriend("data/auaumbiasHits.root");
  fRun->SetOutputFile("data/test.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  //  FairMCPointDraw *RichPoint =   new FairMCPointDraw ("RichPoint",kOrange,  kFullSquare);
  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kBlue,  kFullSquare);
//  FairMCPointDraw *TrdPoint= new FairMCPointDraw ("TrdPoint",kTeal,  kFullSquare);
  //FairMCPointDraw *EcalPoint =    new FairMCPointDraw ("EcalPoint",kYellow,  kFullSquare);
  //FairMCPointDraw *RefPlanePoint = new FairMCPointDraw ("RefPlanePoint",kPink,  kFullSquare);
  //FairMCPointDraw *StsPoint = new FairMCPointDraw ("StsPoint",kCyan,  kFullSquare);
   
  //CbmPixelHitSetDraw *TofHits =   new CbmPixelHitSetDraw ("TofHit",kRed,  kFullSquare);
                                                              
  fMan->AddTask(Track);
  
  //  fMan->AddTask(RichPoint);
  //fMan->AddTask(EcalPoint);   
  fMan->AddTask(TofPoint);   
  //fMan->AddTask(TofHits);   
  //fMan->AddTask(TrdPoint);
  //fMan->AddTask(RefPlanePoint);
  //fMan->AddTask(StsPoint);
 
    
  fMan->Init();                     
  
  //  gGeoManager->SetVisLevel(1);


}
