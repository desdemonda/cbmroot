// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
run_pluto(Int_t pluto_example = 1)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;




  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString magnetGeom = "passive/magnet_v09e.geo";
  TString stsGeom    = "sts/sts_v11a.geo";
  TString richGeom   = "rich/rich_v08a.geo";
  TString trdGeom    = "trd/trd_v11c.geo";
  TString tofGeom    = "tof/tof_v07a.geo";
//  TString ecalGeom   = "ecal/ecal_v08a.geo";

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load all needed libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libGen");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libSts");
  gSystem->Load("libLittrack");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");

  FairLogger *logger = FairLogger::GetLogger();
  logger->SetLogFileName("MyLog.log");
  logger->SetLogToScreen(kTRUE);
  logger->SetLogToFile(kTRUE);
  logger->SetLogVerbosityLevel("HIGH");
  logger->SetLogFileLevel("DEBUG4");
  logger->SetLogScreenLevel("INFO");


  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile("data/auaumbias.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media.geo");
  
  // Create and add passive materials and detectors
  //-------------------------
  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName(caveGeom);
  fRun->AddModule(Cave);

  FairModule *Pipe= new CbmPipe("PIPE");
  Pipe->SetGeometryFileName(pipeGeom);
  fRun->AddModule(Pipe);
  
  FairModule *Target= new CbmTarget("Target");
  Target->SetGeometryFileName(targetGeom);
  fRun->AddModule(Target);		

  FairModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName(magnetGeom);
  fRun->AddModule(Magnet);
  
  FairDetector *Sts= new CbmSts("STS", kTRUE);
  Sts->SetGeometryFileName(stsGeom);
  fRun->AddModule(Sts);

  FairDetector *Trd= new CbmTrd("TRD",kTRUE );
  Trd->SetGeometryFileName(trdGeom);
  fRun->AddModule(Trd);

  FairDetector *Rich= new CbmRich("RICH", kTRUE);
  Rich->SetGeometryFileName(richGeom);
  fRun->AddModule(Rich);
  
  FairDetector *Tof= new CbmTof("TOF", kTRUE );
  Tof->SetGeometryFileName(tofGeom);
  fRun->AddModule(Tof);
	
 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  if (1 == pluto_example) {
    //*********** Example 1: Inline style Pluto input (elementary reaction)
    //This is just an example, please replace the reaction ctor as you like
    PReaction *reaction = new PReaction(8.0,"p","p","p p eta [pi+ pi- pi0]");
  } else if (2 == pluto_example) {
    //*********** Example 2: Inline style Pluto input (fireball "omega Dalitz")
    //This example is taken from the macro path of Pluto (thermal_w_dalitz.C)
    //test thermal source of w's
    //  (R.H. 28/7/2000)
    // + added decay of the omegas into di-electrons
    PFireball *source=new PFireball("w",2.,0.1,0,1,0,0.5,0,0,-0.2);
    source->Print();
    PParticle *omeg1=new PParticle("w");
    PParticle *s[]={source,omeg1};
    PChannel  *c=new PChannel(s,1,1);
    PParticle *ep6 = new PParticle("e+");
    PParticle *em6 = new PParticle("e-");
    PParticle *omegdecayb[] = {omeg1,ep6,em6};
    PChannel  *c15 = new PChannel(omegdecayb,2,1,1);
    PChannel  *cc[]={c,c15};
    PReaction *reaction=new PReaction(cc,2); //array, number of channels
  } else if (3 == pluto_example) {
    //*********** Example 3: Inline style Pluto input (fireball "J/Psi")
    //This example was provided by A. Kotynia
    Float_t Eb   = 25; 	    // to jest AGeV - ile GeV na nukleon
    Float_t T1    = 0.150;   // temperature in GeV // powinno byc 150
    Float_t T2    = 0.;      // temperature in GeV
    Float_t blast = 0.;      // radial expansion velocity
    
    //
    // Thermal source with temperature(s) T1 (T2), frac*f(T1) + (1-frac)*f(T2),
    // optional blast, optional polar anisotropies (A2,A4), 
    // optional flow (v1,v2).
    //

    PFireball *source_JPsi=new PFireball("J/Psi",Eb,T1,T2,1.,blast,0.,0.,0.,0.);
    source_JPsi->setSigma(0.23); 
    source_JPsi->Print();
    
    PParticle *JPsi = new PParticle("J/Psi");
    PParticle *mumJPsi = new PParticle("mu-");
    PParticle *mupJPsi = new PParticle("mu+");
    
    PParticle* s_JPsi[]    = {source_JPsi,JPsi};
    PChannel* c_sJPsi   = new PChannel(s_JPsi,1,1);
    
    PParticle *s_JPsidimu[]  ={JPsi,mumJPsi,mupJPsi};
    PChannel  *c_JPsidimu  = new PChannel(s_JPsidimu,2,1);
    PChannel  *cc_JPsi[]   = {c_sJPsi,c_JPsidimu};
    
    PReaction *reaction=new PReaction(cc_JPsi,2);
   } else if (4 == pluto_example) {
    //*********** Example 4: Adds embedded particles (e.g. for detector studies)
    // in this case we embed the particles into an eta dalitz
    //This example is taken from the macro path of Pluto (embeddedParticles.C)
    // + added decay of the di-electrons
    PReaction *reaction = new PReaction(8.0,"p","p","p p eta [dilepton [e+ e-] g]");
    
    //Construct the bulk container:
    PEmbeddedParticles * embedded = new PEmbeddedParticles();
    
    //We add a "white" dilepton, which we emit in a small cone:
    PParticle * dilepton = new PParticle("dilepton");
    embedded->AddParticle(dilepton);
    embedded->SetSampling(0, 1.,   //pmin and pmax in lab frame 
			  TMath::Pi()/1000., //opening angle
			  TMath::Pi()/2.,    //Theta of pointing vect.
			  TMath::Pi()/2.,    //Phi of pointing vect.
			  0.2, 1.5  //Mass sampling (optional)
			  ); 
    
    //Add our container to the reaction:
    reaction.AddBulk(embedded);
    
    reaction->setDecayAll(1.);   // decay all particles with tau < 1 ns (this decays our embedded dilepton)
  } else {
    cout << "There are only 4 (1-4) examples how to use the pluto reaction generator. You tried to run the non existing example" << pluto_example << endl;
  }

  reaction->Print();
  //feed the reaction into FAIR-ROOT:
  FairPlutoReactionGenerator *plutoGen = new FairPlutoReactionGenerator(reaction);
  primGen->AddGenerator(plutoGen);


  // Field Map Definition
 // --------------------
  // Constant Field
  CbmFieldConst *fMagField1=new CbmFieldConst();
  fMagField1->SetField(0, 14 ,0 ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField1->SetFieldRegion(-74, -39 ,-22 , 74, 39 , 160 );
  
  fRun->SetField(fMagField1);
  
  // Store the partile trajectories

  fRun->SetStoreTraj(kTRUE);
  
  // Initialize the simulation

  fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 // Set cuts for storing the trajectories
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);
 
  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("data/testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  Int_t nEvents = 3;
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "Realtime: " << rtime << " seconds" << endl;
  cout << " Cputime: " << ctime << " seconds" << endl; 

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}  
  
