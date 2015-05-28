

void mvd_analyse(TString input = "auau.25gev", TString system = "centr", 
		 Int_t  nEvents = 100, Int_t  iVerbose = 0, 
		 const char* setup = "sis300_electron", 
		 bool PileUp = true, bool littrack = false)
{

  // ========================================================================
  //          Adjust this part according to your requirements

   
  // Input file (reco data)
  TString mcFile = "data/mvd.mc.unigen." + input + "." + system + ".root";
  // Output file
if(!PileUp)
	{
	if(littrack)
	  TString recoFile = "data/mvd.reco.unigen." + input + "." + system + ".littrack.root";
	else 
	  TString recoFile = "data/mvd.reco.unigen." + input + "." + system + ".l1.root";
	}
else if(littrack)
  TString recoFile = "data/mvd.reco.unigen." + input + "." + system + ".PileUp.littrack.root";
else 
  TString recoFile = "data/mvd.reco.unigen." + input + "." + system + ".PileUp.l1.root";

  // Output file
if(!PileUp)
	{
	if(littrack)
	  TString analysisFile = "data/mvd.ana.unigen." + input + "." + system + ".littrack.root";
	else 
	  TString analysisFile = "data/mvd.ana.unigen." + input + "." + system + ".l1.root";
	}
else if(littrack)
  TString analysisFile = "data/mvd.ana.unigen." + input + "." + system + ".PileUp.littrack.root";
else 
  TString analysisFile = "data/mvd.ana.unigen." + input + "." + system + ".PileUp.l1.root";

  // Parameter file

  TString parFile = "data/paramsunigen.";
  parFile = parFile + input;
  parFile = parFile + ".";
  parFile = parFile + system;
  parFile = parFile + ".root";

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly 
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString inDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = inDir + "/parameters/";

  TString setupFile = inDir + "/geometry/setup/" + setup + "_setup.C";
  TString setupFunct = setup;
  setupFunct += "_setup()";

  gROOT->LoadMacro(setupFile);
  gInterpreter->ProcessLine(setupFunct);

  TObjString stsDigiFile = paramDir + stsDigi;
  parFileList->Add(&stsDigiFile);
  cout << "macro/run/run_reco.C using: " << stsDigi << endl;

  TObjString tofDigiFile = paramDir + tofDigi;
  parFileList->Add(&tofDigiFile);
  cout << "macro/run/run_reco.C using: " << tofDigi << endl;
  // ------------------------------------------------------------------------
 
  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
   FairRunAna* run = new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(analysisFile);
   run->AddFriend(recoFile);
  // ------------------------------------------------------------------------

  Int_t minimalMvdHitsInStsTracks = 2;

  // -----   Mvd Qa and Analyse  --------------------------------------------
  CbmMvdQa* mvdQa = new CbmMvdQa();
  mvdQa->SetMinHitReq(minimalMvdHitsInStsTracks);
  run->AddTask(mvdQa);
  // ------------------------------------------------------------------------

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

  // -----   Run initialisation   -------------------------------------------
  run->Init();
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
     
  // -----   Start run   ----------------------------------------------------
  run->Run(0,nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << analysisFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
