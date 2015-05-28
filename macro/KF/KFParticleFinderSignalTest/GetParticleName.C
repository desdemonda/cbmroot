// #include "$VMCWORKDIR/KF/KFParticleParformance/KFPartEfficiencies.h"

void GetParticleName(int iParticle = 0) {

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
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMinuit2"); // Needs for rich ellipse fitter
  // ------------------------------------------------------------------------


  KFPartEfficiencies fEfficiency;
  std::cout << fEfficiency.partName[iParticle].Data() << std::endl;
}
