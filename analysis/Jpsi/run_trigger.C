void run_trigger(Int_t nfile=0)
{

 Int_t iVerbose = 0;
 Int_t nEvents = 1000;
  TString system  = "auau";

// Beam momentum
  TString beam    = "25gev";
 // Trigger (centrality)
  TString trigger = "pure";
  TString numFile = "00";
  if(nfile<10) numFile="000";
  if(nfile>99) numFile="0";
  numFile +=nfile;

  // Input file (M
  // Output file name
  TString outDir  = "/d/cbm04/alla/Feb09/fullEcal" ;

   TString inFileMC = outDir + "/jpsi."+beam + "." + trigger + "." + numFile +".mc.root";
   cout<<inFileMC<<endl;
  TString inFile = outDir + "/global.reco.0005.root";
   TString parFile = outDir +"/jpsi."+beam + "."  + trigger + "." + numFile + ".params.root";

    TString outputFileName  = "jpsitrigger."+beam  +"."+ trigger +"." + numFile  +".root";
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
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libLittrack");
  gSystem->Load("libDilept");
  gSystem->Load("libJpsi.so");
  // ------------------------------------------------------------------------    // Create CBM Run
    FairRunAna* fRun = new FairRunAna();

    // Set the MC version used
    //    fRun->SetName("TGeant3");

    // Add input/output files to run
    fRun->SetInputFile( inFile );
    fRun->AddFile( inFileMC );
    fRun->SetOutputFile( outputFileName );
     
     // Create and add users tasks

            
    // Electron id assignment 
    CbmJpsiTrigger* taskTrig = new CbmJpsiTrigger();

    
    taskTrig->SetCuts(1); //sMom,     
    fRun->AddTask(taskTrig);
   
  
    // Init and Run
    fRun->Init();
    fRun->Run(0,nEvents);

}
