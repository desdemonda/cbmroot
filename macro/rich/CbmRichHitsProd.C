/* $Id: CbmRichHitsProd.C,v 1.8 2006/09/15 12:50:52 turany Exp $ */


{


// this macro read an input Tree structure
// from simulation and generate an output
// tree containing collections of RichHits

// ========================================================================
  //          Adjust this part according to your requirements
  
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 10;

  // Input file (MC events)
  TString inFile = "../run/test.mc.root";

  // Parameter file
  TString parFile = "../run/params.root";
  
  // Output file
  TString outFile = "test.richhits.root";

  // In general, the following parts need not be touched
  // ========================================================================

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
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------

  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo *io1 = new FairParRootFileIo();
  io1->open(parFile.Data());
  rtdb->setFirstInput(io1);

  // ------- Parameters for photodetector -------------------------
  Double_t pmt_rad = 0.4;            // PMT radius (cm)
  Double_t pmt_dist = 0.;            // distance between PMTs (cm)
  Int_t  det_type = 4;    // detector type (choose: 1=Protvino, 2=Hamamatsu, 3=CsI)
  Int_t  noise = 220;    // number of noise points to be added
                          //  (note: excluding geom. eff. (~0.9))

  // define RichHitProducer to run
  CbmRichHitProducer *hp= new CbmRichHitProducer(pmt_rad,pmt_dist,det_type,noise,iVerbose);
  fRun->AddTask(hp);
  
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);


}
