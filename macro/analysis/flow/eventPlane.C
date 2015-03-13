// -------------------------------------------------------------------------
// ----- This task calculates the event plane for each collision event from STS and PSD information (input: reco. files)
// ----- It also provides comparision with real reaction plane (input: MC -transport output- files)
// ----- It outputs a TTree containing event planes and sub-event planes
// ----- Sub-event plane definition: 
// ----- STS: random sharing of tracks between 2 sub-events
// ----- PSD: sub-event 1 = central modules, sub-event 2 = 1st corona of modules around it, sub-event 3 = peripheral modules
// ----- It also provides event plane flattening techniques to correct for detection setup azimuthal asymmetry: Q-recentering (A. M. Poskanzer and S. A. Voloshin, Phys. Rev. C58, 1671 (1998) [arXiv:nucl-ex/9805001]) and Barrette method
// ----- Flattening techniques applied for each event and sub-event planes and in bins of detector multiplicity (detector multiplicity := number of reco. particles in STS or deposited energy in PSD - full or in sub-events)
// ----- Flattening techniques can be applied without binning if wanted (then use of fit functions - see TEST in the code)
// -------------------------------------------------------------------------

// IMPORTANT NOTE:
// Requires to run on collision events at least twice, e.g.:
// step 1: First run calculating event plane
//         (if run on many event files in parallel, merge event plane task output files, for at least > 10 000 events)
// step 2: Second run calculating event plane and accounting for event plane flattening correction
//         - correction based on the event plane sample calculated in step 1 -
//         (if run on many event files in parallel, merge event plane task output files, for at least > 10 000 events
//         -> to estimate correction factors to apply to the measured flow vn)

void eventPlane(Double_t En = 10, Int_t nEvents = 2, Int_t fQcorr = 0, Int_t fBarcorr = 0)
{
    Int_t gen = 0;             // model
    int mcreco = 1;            // STS EP method: use reco. tracks or MC tracks with >= 4 hits in STS
    Double_t cutY_sub = 0.8;   // STS EP method: use (reco or MC) tracks with y < cutY_sub to decrease STS/PSD overlap
    int doFixStat = 0;         // Binning method for EP flattening: fixed multiplicity intervals or fixed collision statistics intervals

  TString numEvt = "";
  if(nEvents<10) numEvt="000";
  if(nEvents>=10 && nEvents<100) numEvt="00";
  if(nEvents>=100 && nEvents<1000) numEvt="0";
  if(nEvents>=1000 && nEvents<10000) numEvt="";
  numEvt += nEvents;  

  TString sEn = "";
  sEn += En;

  // === reco or MC
  TString smcreco = "";
  if (mcreco == 1) smcreco = "_RECOtracks";
  else smcreco = "_MCtracks_ge4hitsSTS";

  // === STS Y cut to reduce STS/PSD overlap
  TString scutY_sub = "";
  if (cutY_sub == 0.8) scutY_sub = "_STScutY_lt0.8";

  // === Flattening
  TString sQcorr = "";
  if (fQcorr == 1) sQcorr = "_Qcorr";

  TString sBarcorr = "";
  if (fBarcorr == 1) sBarcorr = "_Barcorr_n8";

  TString sFlatBin = "";
  if (doFixStat == 0) sFlatBin = "_FlatBin_FixStep";
  else sFlatBin = "_FlatBin_FixStat";
  if (fQcorr != 1 && fBarcorr != 1 ) sFlatBin = "";

  // === Input & ouptput files
  TString dir = gSystem->Getenv("VMCWORKDIR");
  dir += "/macro/run/data/";

  // Input file (reco events)
  TString infilename = dir + "reco_" + numEvt +  "evt.root";
  // Input file (MC events)
  TString infilename2 = dir + "mc_" + numEvt +  "evt.root";
  // Parameter file
  TString paramfilename = dir + "params_" + numEvt +  "evt.root";
  // PSD coordinates (from Psdv1.cxx)
  TString PsdModCoord = dir + "psd_geo_xy_" + numEvt + "evt.txt";
  // Correction file
  TString corrfilename = dir + "ana_1Mevt" + smcreco + scutY_sub + ".root";
  TString corrfilename_Qcorr = dir + "ana_1Mevt" + smcreco + scutY_sub + "_Qcorr" + sFlatBin + ".root";
  // Output file
  TString outfilename = dir + "ana_" + numEvt +  "evt_" + smcreco + scutY_sub + sQcorr + sBarcorr + sFlatBin + ".root";

  //==================================
  FairRunAna* fRun = new FairRunAna();
  fRun->SetInputFile(infilename);
  fRun->AddFriend(infilename2);
  fRun->SetOutputFile(outfilename);
  
  //=== important for extrapolation to PV in B-field
  CbmKF *KF = new CbmKF();
  fRun->AddTask(KF);
  
  CbmL1* l1 = new CbmL1("CbmL1",1, 3);
  fRun->AddTask(l1);
  
  // Init Simulation Parameters from Root File
  FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
  FairParRootFileIo* input=new FairParRootFileIo();
  input->open(paramfilename);
  rtdb->setFirstInput(input);
  //===

  // --------- Event Plane --------------------------------
  eventPlane* eventPlaneTask = new eventPlane("EventPlane", 1, En);
  eventPlaneTask->setFileName_PsdModCoord(PsdModCoord);

  if (mcreco == 0) eventPlaneTask->STSuseReco(kFALSE);
  eventPlaneTask->STSsetYcut_PSDsub(cutY_sub);
  if (doFixStat == 1) eventPlaneTask->doFlatBinFixStat();

  if ( fQcorr == 1 && fBarcorr == 0 )
  {
      eventPlaneTask->PSDsetFlat(kTRUE, kFALSE); // (Q-recentering, Barrette method)
      eventPlaneTask->STSsetFlat(kTRUE, kFALSE);
      eventPlaneTask->setFileName_flat_Qcorr(corrfilename);
  }
  
  if ( fQcorr == 0 && fBarcorr == 1 )
  {
      eventPlaneTask->PSDsetFlat(kFALSE, kTRUE); // (Q-recentering, Barrette method)
      eventPlaneTask->STSsetFlat(kFALSE, kTRUE);
      eventPlaneTask->setFileName_flat_Barr(corrfilename);
  }

  if ( fQcorr == 1 && fBarcorr == 1 )
  {
      eventPlaneTask->PSDsetFlat(kTRUE, kTRUE); // (Q-recentering, Barrette method)
      eventPlaneTask->STSsetFlat(kTRUE, kTRUE);
      eventPlaneTask->setFileName_flat_Qcorr(corrfilename);
      eventPlaneTask->setFileName_flat_Barr(corrfilename_Qcorr);
  }

  fRun->AddTask(eventPlaneTask);
  // ---------------------------------------------------------

  fRun->Init();

  TStopwatch timer;
  timer.Start();
  fRun->Run(0,nEvents);
  timer.Stop();
  cout << "CPU time  " << timer.CpuTime() << "s" << endl;

}
