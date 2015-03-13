// 20130605 - checked by DE

eventDisplay()
{

  // ========================================================================
  // geometry selection for sim + reco  by Cyrano                            
  // ========================================================================
  ifstream whichTrdGeo;
  whichTrdGeo.open("whichTrdGeo",ios::in);
  TString selectGeo;
  if (whichTrdGeo) whichTrdGeo >> selectGeo;
  TString digipar = selectGeo(0,11);
  digipar.ReplaceAll(".ge","");
  cout << "selected geometry : >> " << selectGeo << " << (to select a different geometry, edit macro/trd/whichTrdGeo file)" << endl;
  cout << "selected digipar  : >> " << digipar << " << " << endl;
  whichTrdGeo.close();
  if (digipar.Length() == 0) digipar = "trd_standard";

  TString  InputFile     ="data/test.mc.root";
  TString  InputFile1    ="data/test.eds.root";
  TString  ParFile       ="data/params.root";
  TString  OutFile       ="data/test.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(InputFile.Data());
  fRun->AddFriend(InputFile1.Data());
  fRun->SetOutputFile(OutFile.Data());

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";
  TString trdDigiFile = paramDir + "/trd/" + digipar + ".digi.par";

  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  cout << "-I- CbmTrdDigiPar: Using file " << trdDigiFile << endl;
  parInput2->open(trdDigiFile, "in");
  rtdb->setSecondInput(parInput2);
  // ------------------------------------------------------------------------

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks       *Track     = new FairMCTracks       ("Monte-Carlo Tracks");
  FairMCPointDraw    *TrdPoints = new FairMCPointDraw    ("TrdPoint",kBlue , kFullSquare);
  CbmPixelHitSetDraw *TrdHits   = new CbmPixelHitSetDraw ("TrdHit"  ,kRed  , kFullSquare);
  //  CbmTrdDigiDraw     *TrdDigis  = new CbmTrdDigiDraw     ("TrdDigi" ,kGreen, kFullSquare);

  //  TrdDigis->SetLayerStation1(kTRUE,kTRUE,kTRUE,kTRUE);
  //  TrdDigis->SetLayerStation2(kTRUE,kTRUE,kTRUE,kTRUE);
  //  TrdDigis->SetLayerStation3(kTRUE,kTRUE,kTRUE,kTRUE);

  fMan->AddTask(Track);
  fMan->AddTask(TrdPoints);
  fMan->AddTask(TrdHits);
  //  fMan->AddTask(TrdDigis);

  fMan->Init(1,7,10000);  
}
