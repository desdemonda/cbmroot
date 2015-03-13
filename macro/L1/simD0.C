// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libBase");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libTof");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libEcal");
  gSystem->Load("libGen");
  
  char *outFile = "/d/cbm02/ikisel/data/default/D0/sim_1000_0.root";
  char *parFile = "/d/cbm02/ikisel/data/default/D0/sim_1000_0.params.root";
  char *urqmdFile  ="/d/cbm01/cbmsim/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14";
  char *asciiFile  ="/d/cbm02/vassiliev/d0_1000_0.dat";

  //char *urqmdFile  =("/d/cbm01/cbmsim/urqmd/auau/25gev/mbias/urqmd.auau.25gev.mbias.0000.ftn14");

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile(outFile);

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media.geo");
 
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new CbmPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);
  
  
  FairModule *Magnet= new CbmMagnet("MAGNET");
  // 1- Active shielding Geometry
  Magnet->SetGeometryFileName("magnet_active.geo");
  // 2- Iron Magnet
  // Magnet->SetGeometryFileName("magnet_iron.geo");
  fRun->AddModule(Magnet);
  
  FairDetector *Sts= new CbmSts("STS", kTRUE);
  Sts->SetGeometryFileName("sts.geo");
  fRun->AddModule(Sts);

  
  FairModule *Target= new CbmTarget("Target");
  Target->SetGeometryFileName("target.geo");
  fRun->AddModule(Target);		

  //FairDetector *Tof= new CbmTof("TOF", kTRUE );
  //Tof->SetGeometryFileName("tof.geo");
  //fRun->AddModule(Tof);
	
  //FairDetector *Trd= new CbmTrd("TRD",kTRUE );
  //Trd->SetGeometryFileName("trd_9.geo");
  //fRun->AddModule(Trd);

  //FairDetector *Rich= new CbmRich("RICH", kTRUE);
  //Rich->SetGeometryFileName("rich.geo");
  //fRun->AddModule(Rich);

  //FairDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName("ecal.geo");
  //fRun->AddModule(Ecal);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Ascii generator

  FairAsciiGenerator *D0gen = new FairAsciiGenerator(asciiFile);
  primGen->AddGenerator(D0gen);

  // Urqmd  Generator
  FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(urqmdFile);

  primGen->AddGenerator(urqmdGen);

 // Particle Generator
 //  FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1., 1.,
 //  2);
 //  primGen->AddGenerator(partGen);
 
 // Ion Generator
 //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);

  // smear the beam spot 
  primGen->SetBeam(0.0, 0.0, 100.e-4, 100.e-4);//cm
  
  primGen->SetTarget(0.0, 0.025);//cm
  
  Bool_t smear = false;
  
  primGen->SmearVertexZ(smear);
  primGen->SmearVertexXY(smear);

 // Field Map Definition
 // --------------------
  // 1- Reading the new field map in the old format

     // CbmFieldMap *fMagField= new CbmFieldMap("FIELD.v04_pavel.map");
   // Constant Field
     //  CbmConstField *fMagField=new CbmConstField();
     //  fMagField->SetFieldXYZ(0, 14 ,0 ); // values are in kG
     // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
     //  fMagField->SetFieldRegions(-74, -39 ,-22 , 74, 39 , 160 );

  // 2- Reading the new field map in the new format

  CbmField *fMagField=new CbmField("Magnet Active Shielding map");
  // Active Shielding
     fMagField->readRootfile("$VMCWORKDIR/input/FieldActive.root", "NewMap");
  // Iron Dipole
  //  fMagField->readRootfile("$VMCWORKDIR/input/FieldIron.root", "Field");
   fRun->SetField(fMagField);

   fRun->Init();


 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 //  fRun->SetStoreTraj(kTRUE);
 // Set cuts for storing the trajectpries
  /* FairTrajFilter* trajFilter = FairTrajFilter::Instance();
     trajFilter->SetStepSizeCut(0.01); // 1 cm
     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);
   */


  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  Int_t nEvents = 1000;
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
