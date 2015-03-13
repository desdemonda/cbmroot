// -------------------------------------------------------------------------
// ----- This task reconstructs the particle flow (vn, n = 1 & 2) out of measured particle azimuthal distributions (input: reco. files or KFParticle Finder output)
// ----- Particle azimuthal distributions can be taken w.r.t. the true reaction plane (input: MC -transport output- file)
// ----- or w.r.t. the event plane (input: output files from eventPlane task) - WARNING, need files with CORRECTED event planes (flattening)
// ----- Several choices for the event plane (using STS or PSD - see eventPlane task)
// ----- It can calculate the correction factor which account for the reaction plane resolution of the detectors & apply it to the measures vn
// ----- (input: output file from eventPlane task, containg a large number of collisions, Nevent > 10 000)
// ----- Particle flow drawn and correction factor calculated for a given centrality interval. Centrality intervals now defined based on STS reco. track multiplicy
// ----- It can correct for inhomogeneous detector acceptance and reconstruction efficiency (input: from collision models, data format: UNIGEN)
// ----- Final drawing: pt- and y-dependences of vn & integrated vn.
// ----- Suggested use: the user can draw the reconstructed flow versus input flow from models
// -------------------------------------------------------------------------

//========== Sequence for simulations:   ( '+' means sequential processing)
//========== 1) mode = 0 or 1 or 2: 
//==========    Creation of generic TTree (containing particle azimuth w.r.t. reaction plane or event plane),
//==========    either from model-generated events (mode 0) or reconstructed events (mode 1) or KF events (mode 2)
//==========            +
//========== 2) mode = 3 or 4 or 5+6+7: 
//==========    Calculate the particle flow and draw rapidity-, pt-dependence & integrated value
//==========    - Simple drawing from TTree (mode 3)
//==========    - or Drawing including detection efficiency correction (mode 4) - Slow -> restriction: Nevent<100K and Nbin_y*Nbin_pt < 100's
//==========    - or Drawing including detection efficiency correction (modes 5+6+7) - fast -> no restriction

//========== NOTE on modes 5+6+7: Detection efficiency correction requires flow computation in narrow (pt, y) bins
//==========           in mode 5: TTree ordering by increasing pt & y to loop only once over TTree
//==========         then mode 6: merging of several files with 2D maps (vn, etc) produced in mode 5; needed if mode 5 is run on parallel on several event sub-samples
//==========         then mode 7: Detection efficiency map calculation (requires to run mode 5 and 6 on reco. AND model-generated events) and final drawing


void anaFlow(Int_t mode = 7, Double_t En = 10., Int_t nEvents = 200, Int_t fileNum = 10, int iharmo = 2, int genreco = 1, Double_t bMIN = 20., Double_t bMAX = 30.)
{
    Int_t gen = 0;
    TString harmo = "";
    if (iharmo==1) harmo = "1";
    if (iharmo==2) harmo = "2";
    Int_t particle = 211;
    Int_t mcreco = 1;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libCbmGenerators");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libTrd");
  gSystem->Load("libL1");
  //gSystem->Load("libTof");
  gSystem->Load("libPsd");
  gSystem->Load("libAnalysis");

  TString sGen = "";
  if (gen == 0) sGen = "urqmd";
  if (gen == 1) sGen = "shield";

  TString sEn = "";
  sEn += En;

  TString numEvt = "";
  if(nEvents<10) numEvt="000";
  if(nEvents>=10 && nEvents<100) numEvt="00";
  if(nEvents>=100 && nEvents<1000) numEvt="0";
  if(nEvents>=1000 && nEvents<10000) numEvt="";
  numEvt += nEvents;
  
  TString sfileNumL = "";
  if(fileNum<10) sfileNumL="0000";
  if(fileNum>=10 && fileNum<100) sfileNumL="000";
  if(fileNum>=100 && fileNum<1000) sfileNumL="00";
  if(fileNum>=1000 && fileNum<10000) sfileNumL="0";
  sfileNumL += fileNum;
  
  TString sfileNum = "";
  if(fileNum<10) sfileNum="000";
  if(fileNum>=10 && fileNum<100) sfileNum="00";
  if(fileNum>=100 && fileNum<1000) sfileNum="0";
  if(fileNum>=1000 && fileNum<10000) sfileNum="";
  sfileNum += fileNum;

  TString sbMIN = "";
  TString sbMAX = "";
  sbMIN += bMIN;
  sbMAX += bMAX;
  //TString Bcut = "Bsel_" + sbMIN + "fm_" + sbMAX + "fm";  //HERE
  TString Bcut = "Bsel_" + sbMIN + "perc_" + sbMAX + "perc_mostCentr";
  if (mode == 0 || mode == 1 || mode == 2) Bcut = "";

  TString dir = "/hera/cbm/users/sseddiki/test/";

  //====================================================
  CbmAnaFlow* anaFlow = new CbmAnaFlow("anaflow", 1, En);
  anaFlow->setMode(mode);

  // Mode 0, 1, 2: generic TTree creation; contains e.g. particle phi w.r.t. reaction plane and event planes.

  if (mode == 0)
  {
      // Input file (MC events)
      TString infilename2 = dir + "sim/mc_" + numEvt +  "evt_" + sfileNum +  ".root";
      // Input file (reco events)
      TString infilename = dir + "reco/reco_" + numEvt +  "evt_" + sfileNum +  ".root";
      //param
      TString parFile = dir + "sim/params_" + numEvt +  "evt_" + sfileNum +  ".root";

      TString outfilename = dir + "ana/anaFlow_gen_" + numEvt +  "evt_" + Bcut + sfileNum + ".root";

      FairRunAna* fRun = new FairRunAna();
      fRun->SetInputFile(infilename);
      fRun->AddFriend(infilename2);
      fRun->SetOutputFile(outfilename);

      // Input file (gen events)
      TString infilename_gen = "/hera/cbm/prod/gen/" + sGen + "/auau/" + sEn + "gev/mbias/urqmd.auau." + sEn + "gev.mbias." + sfileNumL +  ".root";
      anaFlow->setFileName_gen(infilename_gen);

      //=== important for reco info -> STS mult
      CbmKF *KF = new CbmKF();
      fRun->AddTask(KF);

      CbmL1* l1 = new CbmL1("CbmL1",1, 3);
      fRun->AddTask(l1);

      // Init Simulation Parameters from Root File
      FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
      FairParRootFileIo* input=new FairParRootFileIo();
      input->open(parFile);
      rtdb->setFirstInput(input);
  }

  if (mode == 1)
  {
    
    // Input file (MC events)
    TString infilename2 = dir + "sim/mc_" + numEvt +  "evt_" + sfileNum +  ".root";
    // Input file (reco events)
    TString infilename = dir + "reco/reco_" + numEvt +  "evt_" + sfileNum +  ".root";
    //param
    TString parFile = dir + "sim/params_" + numEvt +  "evt_" + sfileNum +  ".root";
    
    TString EPfilename_Qcorr = dir + "ana/ana_" + numEvt +  "evt_" + sfileNum + "_RECOtracks_STScutY_lt0.8_Qcorr_FlatBin_FixStep.root";
    cout << "EPfilename_Qcorr: " << EPfilename_Qcorr << endl;

    // Output file
    if (mcreco == 0) TString outfilename = dir + "ana/anaFlow_reco_assMC_" + numEvt +  "evt_" + Bcut + sfileNum + ".root";
    if (mcreco == 1) TString outfilename = dir + "ana/anaFlow_reco_track_" + numEvt +  "evt_" + Bcut + sfileNum + ".root";

    FairRunAna* fRun = new FairRunAna();
    fRun->SetInputFile(infilename);
    fRun->AddFriend(infilename2);
    fRun->AddFriend(EPfilename_Qcorr);    
    fRun->SetOutputFile(outfilename);
    
    //=== important for extrapolation to PV -> B-field 
    CbmKF *KF = new CbmKF();
    fRun->AddTask(KF);
    
    CbmL1* l1 = new CbmL1("CbmL1",1, 3);
    fRun->AddTask(l1);
    
    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* input=new FairParRootFileIo();
    input->open(parFile);
    rtdb->setFirstInput(input);
  }
  
  if (mode == 2)
  {
    // Input file (reco events)
    TString infilename = dir + "reco/kfparticlefinder_" + numEvt +  "evt_" + sfileNum +  ".root";
    // Input file (MC events)
    TString infilename2 = dir + "sim/mc_" + numEvt +  "evt_" + sfileNum +  ".root";  
    //param
    TString parFile = dir + "sim/params_" + numEvt +  "evt_" + sfileNum +  ".root";

    TString EPfilename_Qcorr = dir + "ana/ana_" + numEvt +  "evt_" + sfileNum + "_Qcorr.root";
    cout << "EPfilename_Qcorr: " << EPfilename_Qcorr << endl;

    // Output file
    TString outfilename = dir + "ana/anaFlow_kfpart_" + numEvt +  "evt_" + Bcut + sfileNum + ".root";
      
    FairRunAna* fRun = new FairRunAna();
    fRun->SetInputFile(infilename);
    fRun->AddFriend(infilename2);
    fRun->AddFriend(EPfilename_Qcorr);
    fRun->SetOutputFile(outfilename);
  }

  // stored in TTree either reco. tracks, or MC tracks associated with reco. tracks. No effect if mode == 0
  if (mode == 0 || mode == 1 || mode == 2) anaFlow->setMCorRECOTrack(mcreco);

  // ================= mode = 3 (use 1 TTree from 1 gen. file) or 4 (several TTree merged into 1)
  if (mode == 3 || mode == 4 || mode == 5 || mode == 6 || mode == 7)
  {
    TString infilename2 = dir + "sim/mc_" + numEvt +  "evt_0010.root";
    TString outfilename2 = dir + "ana/dummyOut_" + numEvt +  "evt_" + sfileNum +  ".root";

    FairRunAna* fRun = new FairRunAna();
    fRun->SetInputFile(infilename2);
    fRun->SetOutputFile(outfilename2);
    
    nEvents = 1;
  }

  // Mode 3, 4, 5+6+7: Calculate flow and draw results

  if (mode == 3 || mode == 4)
  {
    if (mcreco == 0) TString file_tree = dir + "ana/anaFlow_reco_assMC_1000evt_"+ sfileNum +".root";                 // reco. or KFParticle finder output
    if (mcreco == 1) TString file_tree = dir + "ana/anaFlow_reco_track_1000evt_"+ sfileNum +".root";             // reco. or KFParticle finder output
    TString file_tree_gen = dir + "ana/anaFlow_gen_1000evt_"+ sfileNum +".root";                                     // particles at gen. level
    TString EPcorrFactor_Qcorr = dir + "ana/ana_1Mevt_RECOtracks_STScutY_lt0.8_Qcorr_FlatBin_FixStep.root";           // correction for reaction plane resolution

    cout << "file_tree: " << file_tree << endl;
    cout << "file_tree_gen: " << file_tree_gen << endl;
    cout << "EPcorrFactor_Qcorr: " << EPcorrFactor_Qcorr << endl;
    anaFlow->setFileName_tree(file_tree);
    anaFlow->setFileName_tree_gen(file_tree_gen);
    anaFlow->setFileName_EPcorrFactor(EPcorrFactor_Qcorr);

    if (genreco == 0) TString sGenReco = "gen";
    if (genreco == 1) TString sGenReco = "reco_track";

    TString outfilename = dir + "ana/pion/v"+harmo+"_pion_"+sGenReco+"_N_eff_vspty_"+Bcut+"_mode4_EP_PSD2_effCorr.root";
    if (mode == 4) anaFlow->setFileName_out(outfilename);
    cout << "outfilename: " << outfilename << endl;

    TString sEP = "";
    if (genreco == 1 && mode == 3) sEP = "_mode3_EP_PSD2_EPcorr_sub";
    if (genreco == 1 && mode == 4) sEP = "_mode4_EP_PSD2_EPcorr_sub_effCorr";
    TString result_y = "result/au" + sEn + "au/pion/result_pion_mode4_cut6_"+sGenReco+"_v"+harmo+"_y_"+Bcut+sEP+"_30bins.txt";
    TString result_pt = "result/au" + sEn + "au/pion/result_pion_mode4_cut6_"+sGenReco+"_v"+harmo+"_pt_"+Bcut+sEP+"_30bins.txt";
    anaFlow->SetFileName_ascii(result_y, result_pt);

    anaFlow->setBrange(bMIN, bMAX);
    anaFlow->setflowHarmoToAnalyse(harmo);
    anaFlow->setParticleToAnalyse(particle);      // comment if all particles needed
    anaFlow->setQcut("");                         // particle charge: "positive"/"negative"/"neutral"/"charged", no cut if not set
    anaFlow->setS_BG("signal");                   // or "bg". Both included when not set (only effective with kfparticle output files for now).
    //anaFlow->setCalcFlowCoef_meth("phifit");    // method: sdt: "cosnphi", alternative: "phifit"
    if (genreco == 0) anaFlow->useTrackatGen();   // use generated events directly: in which case file_tree = file_tree_gen -> file_tree not needed

    // 2 lines below: no effect if useTrackatGen()
    anaFlow->doPrimary();
    if (mode == 4) anaFlow->doEffCorr();

    // == EP corr
    anaFlow->setEPmeth("EP_PSD2");                // "RP" automatically if useTrackatGen()
    // below: no effect if "RP"
    anaFlow->setdoEPCorr(kTRUE);
    anaFlow->setEPsubevent(kTRUE);

    // Final Draw
    anaFlow->setdoInt_diffvn(2);                  // 0: int. vn, 1: diff. vn (both f(y) & f(pt)), 2: both int. & diff. vn
    anaFlow->setPTrange(0., 2., 30);
    anaFlow->setYrange(-1.5, 1.5, 30);

    // cut6
    anaFlow->setPTcut(0.1, 0.3);
    if (harmo=="1") anaFlow->setYcut(0.4, 0.6);
    if (harmo=="2") anaFlow->setYcut(-0.2, 0.2);
  }

  // sort TTree + create 2Dmap
  if (mode == 5)
  {
      // in mode 5: MC tracks are tracks at generation level (from event generator output - converted to UNIGEN data format)
      if (genreco == 0) TString infilename = dir + "ana/anaFlow_gen_" + numEvt +  "evt_" + sfileNum;
      if (genreco == 1) TString infilename = dir + "ana/anaFlow_reco_track_" + numEvt +  "evt_" + sfileNum;

      if (genreco == 0) TString outfilename =  dir + "ana/pion/anaFlow_pion_v"+harmo+"_gen_" + numEvt +  "evt_"+Bcut+"_" + sfileNum;
      if (genreco == 1) TString outfilename =  dir + "ana/pion/anaFlow_pion_v"+harmo+"_reco_track_" + numEvt +  "evt_"+Bcut+"_" + sfileNum;

      infilename += ".root";
      if (genreco == 0) outfilename += "_2Dhistos_mode567_bin300-400.root";
      if (genreco == 1) outfilename += "_2Dhistos_mode567_bin300-400_EP_PSD2.root";

      cout << "infilenme: " << infilename << endl;
      cout << "outfilename: " << outfilename << endl;

      // STS centrality
      TString EPcorrFactor_Qcorr = dir + "ana/ana_1Mevt_RECOtracks_STScutY_lt0.8_Qcorr_FlatBin_FixStep.root";

      anaFlow->setFileName_tree(infilename);
      anaFlow->setFileName_EPcorrFactor(EPcorrFactor_Qcorr);
      anaFlow->setFileName_out(outfilename);

      anaFlow->setBrange(bMIN, bMAX);
      anaFlow->setflowHarmoToAnalyse(harmo);
      anaFlow->setParticleToAnalyse(particle); 
      anaFlow->doPrimary();
      if (genreco == 0) anaFlow->setEPmeth("RP");
      if (genreco == 1) anaFlow->setEPmeth("EP_PSD2");              
      //anaFlow->setCalcFlowCoef_meth("phifit");  // method: sdt: "cosnphi", alternative: "phifit"
      //anaFlow->setS_BG("signal");               // or "bg". Both included when not set (only effective with kfparticle output files for now).
      //anaFlow->setQcut("");                     // particle charge: "positive"/"negative"/"neutral"/"charged", no cut if not set
  }

  // Merge 2Dmap from several files
  if (mode == 6)
  {
      TString dirname_2Dhistos = dir + "ana/pion";

      const char *in;
      if (iharmo==1)
      {
	  if (genreco == 0) in = "anaFlow_pion_v1_gen";
	  if (genreco == 1) in = "anaFlow_pion_v1_reco_track";
      }
      else
      {
	  if (genreco == 0) in = "anaFlow_pion_v2_gen";
	  if (genreco == 1) in = "anaFlow_pion_v2_reco_track";
      }
      
      const char *inside = Bcut;

      const char *ex;
      if (genreco == 0) ex = "2Dhistos_mode567_bin300-400.root";
      if (genreco == 1) ex = "2Dhistos_mode567_bin300-400_EP_PSD2.root";

      // in mode 6: MC tracks are tracks at generation level (from event generator output - converted to UNIGEN data format)
      if (genreco == 0) TString outfilename = dir + "ana/pion/anaFlow_pion_v"+harmo+"_gen_1Mevt_"+Bcut+ "_2Dhistos_mode567_bin300-400.root";
      if (genreco == 1) TString outfilename = dir + "ana/pion/anaFlow_pion_v"+harmo+"_reco_track_1Mevt_"+Bcut+ "_2Dhistos_mode567_bin300-400_EP_PSD2.root";

      cout << "outfilename: " << outfilename << endl;

      anaFlow->setDirName(dirname_2Dhistos, in, ex, inside);
      anaFlow->setFileName_out(outfilename);
      
  }

  // Draw final graphs
  if (mode == 7)
  {
    if (genreco == 0) TString sGenReco = "gen";
    if (genreco == 1) TString sGenReco = "reco";

    TString file_2Dmap     = dir + "ana/pion/anaFlow_pion_v"+harmo+"_reco_track_1Mevt_"+Bcut+ "_2Dhistos_mode567_bin300-400_EP_PSD2.root";      
    TString file_2Dmap_gen = dir + "ana/pion/anaFlow_pion_v"+harmo+"_gen_1Mevt_"+Bcut+ "_2Dhistos_mode567_bin300-400.root";
    TString EPcorrFactor_Qcorr = dir + "ana/ana_1Mevt_RECOtracks_STScutY_lt0.8_Qcorr_FlatBin_FixStep.root";
    TString outfilename = dir + "ana/pion/eff_pion_v"+harmo+"_pion_"+sGenReco+"_vspty_1Mevt_"+Bcut+ "_mode567_bin300-400.root";

    cout << "file_2Dmap: " << file_2Dmap << endl;
    cout << "file_2Dmap (part. at gen.): " << file_2Dmap_gen << endl;
    cout << "EPcorrFactor_Qcorr: " << EPcorrFactor_Qcorr << endl;
    cout << "outfilename: " << outfilename << endl;

    anaFlow->setFileName_tree(file_2Dmap);
    anaFlow->setFileName_tree_gen(file_2Dmap_gen);
    anaFlow->setFileName_EPcorrFactor(EPcorrFactor_Qcorr);
    anaFlow->setFileName_out(outfilename);

    TString sEP = "";
    if (genreco == 1) sEP = "_EP_PSD2_EPcorr_sub_effCorr";
    TString result_y = "result/au" + sEn + "au/pion/result_pion_mode567_bin300-400_cut9_"+sGenReco+"_v"+harmo+"_y_1Mevt_" + Bcut +sEP+"_30bins.txt";
    TString result_pt = "result/au" + sEn + "au/pion/result_pion_mode567_bin300-400_cut9_"+sGenReco+"_v"+harmo+"_pt_1Mevt_" + Bcut +sEP+"_30bins.txt";
    //TString result_y = "result/au" + sEn + "au/pion/result_pion_mode567_bin300-400_cut6_"+sGenReco+"_N_y_1Mevt_" + Bcut +"_30bins.txt";
    //TString result_pt = "result/au" + sEn + "au/pion/result_pion_mode567_bin300-400_cut6_"+sGenReco+"_N_pt_1Mevt_" + Bcut +"_30bins.txt";
    anaFlow->SetFileName_ascii(result_y, result_pt);
    cout << "-------- Ascii output file: " << result_y << ", " << result_pt << endl;

    anaFlow->setBrange(bMIN, bMAX);
    anaFlow->setflowHarmoToAnalyse(harmo);
    anaFlow->setParticleToAnalyse(particle);     
    if (genreco == 0) anaFlow->useTrackatGen();  // use generated events directly: in which case file_tree = file_tree_gen -> file_tree not needed
    anaFlow->doEffCorr();                        // no effect if useTrackatGen() 

    // == EP corr
    anaFlow->setEPmeth("EP_PSD2");               // if useTrackatGen(): only RP; if "RP", 2 lines below have no effect
    anaFlow->setdoEPCorr(kTRUE);
    anaFlow->setEPsubevent(kTRUE);

    // == Final Draw
    anaFlow->setdoInt_diffvn(1);                 // 0: int. vn, 1: diff. vn (both f(y) & f(pt)), 2: both int. & diff. vn
    anaFlow->setPTrange(0., 2., 30);             // bin_min, bin_man, number of bins
    anaFlow->setYrange(-1.5, 1.5, 30);           // bin_min, bin_man, number of bins

    // cut
    if (harmo=="1") anaFlow->setPTcut(0.8, 1.);
    if (harmo=="2") anaFlow->setPTcut(0.8, 1.);

    if (harmo=="1") anaFlow->setYcut(0.4, 0.6);
    if (harmo=="2") anaFlow->setYcut(-0.2, 0.2);
  }

  fRun->AddTask(anaFlow);
  // ---------------------------------------------------------

  fRun->Init();  
  
  TStopwatch timer;
  timer.Start();
  fRun->Run(0,nEvents);
  timer.Stop();
  cout << "CPU time  " << timer.CpuTime() << "s" << endl;

  //gApplication->Terminate();

}