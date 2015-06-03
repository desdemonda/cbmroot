// --------------------------------------------------------------------------
//
// Macro for local MVD reconstruction from MC data
//
// Tasks:  CbmMvdDigitiser
//         CbmMvdClusterfinder
//         CbmMvdHitfinder
// 
//
// V. Friese   23/04/2009
// Update: P. Sitzmann Juli 2014
// --------------------------------------------------------------------------


void mvd_reco(Int_t nEvents = 10, Int_t iVerbose = 0)
{

  // ========================================================================
  //          Adjust this part according to your requirements

   
  // Input file (MC events)
  TString inFile = "data/mvd.mc.nini.17gev.minbias.10000.root";
  
  // Background file (MC events, for pile-up)
  TString bgFile = "data/mvd.mc.nini.17gev.minbias.10000.root";

  // Output file
  TString outFile = "data/mvd.reco.nini.17gev.minbias.10000.root";

  // Parameter file
  TString parFile = "data/params.nini.17gev.minbias.10000.root";
 
  TString mvdGeom = "data/fullgeometry.root";

//  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly 
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString inDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = inDir + "/parameters/";

  TString setupFile = inDir + "/geometry/setup/sis300_electron_setup.C";
  TString setupFunct = "sis300_electron";
  setupFunct += "_setup()";

  gROOT->LoadMacro(setupFile);
  gInterpreter->ProcessLine(setupFunct);

  TObjString stsDigiFile = paramDir + stsDigi;
  parFileList->Add(&stsDigiFile);
  cout << "macro/run/run_reco.C using: " << stsDigi << endl;
 
  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  run->SetGeomFile(mvdGeom);

  // ------------------------------------------------------------------------
 
  // -----   MVD Digitiser   ----------------------------------------------
  CbmMvdDigitizer* mvdDigitise = new CbmMvdDigitizer("MVD Digitiser", 0, iVerbose); 

bool PileUp = false;

if(PileUp)
	{
 	 Int_t pileUpInMVD=3; 
  	mvdDigitise->SetBgFileName(bgFile);
  	mvdDigitise->SetBgBufferSize(200); 
  	mvdDigitise->SetPileUp(pileUpInMVD-1);
  	//--- Delta electrons -------
 	 mvdDigitise->SetDeltaName(deltaFile);
 	 mvdDigitise->SetDeltaBufferSize(pileUpInMVD*200); 
  	mvdDigitise->SetDeltaEvents(pileUpInMVD*100);
	}


  run->AddTask(mvdDigitise);
  // ----------------------------------------------------------------------

  // -----   MVD Clusterfinder   --------------------------------------------
  CbmMvdClusterfinder* mvdCluster = new CbmMvdClusterfinder("MVD Clusterfinder", 0, iVerbose); 
  run->AddTask(mvdCluster);
  // ----------------------------------------------------------------------

  // -----   MVD Hit Finder   ---------------------------------------------
  CbmMvdHitfinder* mvdHitfinder = new CbmMvdHitfinder("MVD Hit Finder", 0, iVerbose);
  mvdHitfinder->UseClusterfinder(kTRUE);
  run->AddTask(mvdHitfinder);
  // ----------------------------------------------------------------------
 
  // -----   STS digitizer   -------------------------------------------------
  // --- The following settings correspond to the settings for the old
  // --- digitizer in run_reco.C
  Double_t dynRange       =   40960.;  // Dynamic range [e]
  Double_t threshold      =    4000.;  // Digitisation threshold [e]
  Int_t nAdc              =    4096;   // Number of ADC channels (12 bit)
  Double_t timeResolution =       5.;  // time resolution [ns]
  Double_t deadTime       = 9999999.;  // infinite dead time (integrate entire event)
  Double_t noise          =       0.;  // ENC [e]
  Int_t digiModel         = 1;  // Model: 1 = uniform charge distribution along track

  CbmStsDigitize* stsDigi = new CbmStsDigitize(digiModel);
  stsDigi->SetParameters(dynRange, threshold, nAdc, timeResolution,
  		                   deadTime, noise);
  run->AddTask(stsDigi);
  // -------------------------------------------------------------------------

  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsFindClusters();
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits();
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------

  // -----   Matching   ---------------------------------------------
  CbmMatchRecoToMC* matcher = new CbmMatchRecoToMC();
  run->AddTask(matcher);
  // ----------------------------------------------------------------------

  // -----   STS track finding   --------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  run->AddTask(l1);

  Bool_t useMvdInL1Tracking = kTRUE;
  CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder, useMvdInL1Tracking);
  run->AddTask(stsFindTracks);
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
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
 

}
