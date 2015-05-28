/// \file ConfigTemplate_user_task.C
/// \brief A template task configuration macro with example and explanations
///
/// This macro should represent a starting point for the usage of the
///
///         PairAnalysis PAckage (PAPA) -- written by Julian Book
///
///
/// It configures a dummy multi-task with 4 configurations.
///
/// UPDATES, NOTES:
/// - this is a beta version of PAPA, therefore backward compatiblity is not yet granted (27.05.15)
///
///
///
/// \author Julian Book <jbook@ikf.uni-frankfurt.de>, Goethe-University Frankfurt
/// \date May 27, 2015
///

PairAnalysis* Config_PairAnalysis(Int_t cutDefinition);

/// names of the tasks
TString names=("Acc;TRD;RICH;TRD+RICH");
enum { kAcc,                 /// acceptance study
       kTRDcfg,              /// TRD reconstruction
       kRICHcfg,             /// RICH reconstruction
       kTRDRICHcfg           /// TRD+RICH reconstruction
 };

////// OUTPUT
void InitHistograms(   PairAnalysis *papa, Int_t cutDefinition);
void InitHF(           PairAnalysis* papa, Int_t cutDefinition);
////// CUTS
void SetupEventCuts(   PairAnalysis *papa, Int_t cutDefinition);
void SetupTrackCuts(   PairAnalysis *papa, Int_t cutDefinition);
// void SetupV0Cuts( PairAnalysis *papa,  Int_t cutDefinition);
void SetupPairCuts(    PairAnalysis *papa,  Int_t cutDefinition);
void SetupTrackCutsMC( PairAnalysis *papa, Int_t cutDefinition);
////// SETTINGS
void ConfigBgrd(       PairAnalysis *papa,  Int_t cutDefinition);
void AddMCSignals(     PairAnalysis *papa,  Int_t cutDefinition);
////// MISC
TObjArray *arrNames=names.Tokenize(";");
const Int_t nPapa=arrNames->GetEntries();

//______________________________________________________________________________________
AnalysisTaskMultiPairAnalysis *ConfigTemplate_user_task(const char *taskname)
{
  ///
  /// creation of one multi task
  ///
  AnalysisTaskMultiPairAnalysis *task = new AnalysisTaskMultiPairAnalysis(taskname);
  task->SetBeamEnergy(8.); //TODO: get internally from FairBaseParSet::GetBeamMom()

  /// add PAPA analysis with different cuts to the task
  for (Int_t i=0; i<nPapa; ++i) {

    /// load configuration
    PairAnalysis *papa=Config_PairAnalysis(i);
    if(!papa) continue;

    /// add PAPA to the task
    task->AddPairAnalysis(papa);

    printf(" %s added\n",papa->GetName());
  }
  return task;

}

//______________________________________________________________________________________
PairAnalysis* Config_PairAnalysis(Int_t cutDefinition)
{
  ///
  /// Setup the instance of PairAnalysis
  ///

  /// task name
  TString name=arrNames->At(cutDefinition)->GetName();
  printf(" Adding config %s \n",name.Data());

  /// init PairAnalysis
  PairAnalysis *papa = new PairAnalysis(Form("%s",name.Data()), Form("%s",name.Data()));
  papa->SetHasMC(kTRUE);  /// TODO: set automatically
  /// ~ type of analysis (leptonic, hadronic or semi-leptonic 2-particle decays are supported)
  papa->SetLegPdg(-11,+11); /// default: dielectron

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  SetupEventCuts(papa,cutDefinition);
  SetupTrackCuts(papa,cutDefinition);
  // TODO: SetupV0Cuts(papa,cutDefinition);
  SetupPairCuts(papa,cutDefinition);
  SetupTrackCutsMC(papa,cutDefinition);

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv MISC vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// Monte Carlo Signals
  AddMCSignals(papa, cutDefinition);

  /// background estimators
  ConfigBgrd(papa,cutDefinition);

  /// pairing settings
  //  papa->SetNoPairing();   /// single particle analysis
  papa->SetProcessLS(kFALSE); /// skip LS calculation

  /// TODO: prefilter settings
  //  papa->SetPreFilterAllSigns();
  //  papa->SetPreFilterUnlikeOnly();

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv OUTPUT vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// histogram setup (single TH1,2,3, TProfile,2,3, THn)
  InitHistograms(papa,cutDefinition);
  /// histogram grid setup (a grid of TH1,2,3, TProfile,2,3, THn)
  InitHF(papa,cutDefinition);

  /// some simple cut QA for events, tracks, OS-pairs
  papa->SetCutQA();

  return papa;
}

//______________________________________________________________________________________
void SetupEventCuts(PairAnalysis *papa, Int_t cutDefinition)
{
  ///
  /// Setup the event cuts
  ///

  /// Cut using (a formula based on) variables which are defined and described in PairAnalysisVarManager.h
  /// Cuts can be added by either excluding or including criteria (see PairAnalysisVarCuts.h)
  /// formula function strings are listed here: http://root.cern.ch/root/html/TFormula.html#TFormula:Analyze
  PairAnalysisVarCuts *eventCuts=new PairAnalysisVarCuts("vertex","vertex");
  eventCuts->AddCut(PairAnalysisVarManager::kNVtxContrib, 0.0, 800.);  /// inclusion cut
  eventCuts->AddCut("VtxChi/VtxNDF", 6., 1.e3, kTRUE);                 /// 'kTRUE': exclusion cut based on formula
  eventCuts->AddCut("abs(ZvPrim)", 0., 10.);                           /// example of TMath in formula-cuts
  eventCuts->Print();                                                  /// for debug purpose (recommended)

  /// add cuts to the event filter
  papa->GetEventFilter().AddCuts(eventCuts);

}

//______________________________________________________________________________________
void SetupTrackCuts(PairAnalysis *papa, Int_t cutDefinition)
{
  ///
  /// Setup the track cuts
  ///
  Bool_t hasMC=papa->GetHasMC();

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv TRACK QUALITY CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// setup a cut group, in which cut logic can be set (kCompAND, kCompOR)
  PairAnalysisCutGroup  *grpQualCuts = new PairAnalysisCutGroup("quality","quality",PairAnalysisCutGroup::kCompAND);

  /// example for adding different acceptance cuts to the first cut object
  PairAnalysisVarCuts   *accCuts = new PairAnalysisVarCuts("acceptance","acceptance");
  accCuts->SetCutType(PairAnalysisVarCuts::kAll); /// wheter 'kAll' or 'kAny' cut has to be fullfilled
  /// 'switch' for config specific cut settings
  switch(cutDefinition) {
    //  case kAcc:
  case kTRDcfg:
  case kRICHcfg:
  case kTRDRICHcfg:
    accCuts->AddCut(PairAnalysisVarManager::kPt,           0.05, 1e30);
    accCuts->AddCut(PairAnalysisVarManager::kTRDHitsMC,      0.,   0.5, kTRUE); /// TRD acceptance
    break;
  default:
    accCuts->AddCut(PairAnalysisVarManager::kPt,           0.4, 1e30);
    break;
  }

  /// example of second cut object - standard reconstruction cuts
  PairAnalysisVarCuts   *recSTS = new PairAnalysisVarCuts("recSTS","recSTS");
  recSTS->SetCutType(PairAnalysisVarCuts::kAll);
  recSTS->AddCut(PairAnalysisVarManager::kSTSHits,         4.,   10.);        /// min+max requieremnt for hits
  recSTS->AddCut(PairAnalysisVarManager::kSTSChi2NDFtoVtx, 0.,    3.);        /// keep tracks pointing to the primary vertex
  recSTS->AddCut(PairAnalysisVarManager::kSTSFirstHitPosZ, 0.,   35.);        /// require a hit in first STS layer
  recSTS->AddCut("STSPin-STSPout",                         0.1, 100., kTRUE); /// exclude momentum large differences

  /// example of third cut object - standard reconstruction cuts
  PairAnalysisVarCuts   *recRICH = new PairAnalysisVarCuts("recRICH","recRICH");
  recRICH->SetCutType(PairAnalysisVarCuts::kAll);
  recRICH->AddCut(PairAnalysisVarManager::kRICHHits,       6.,   100.);       /// min+max  exclusion for hits

  /// example of fourth cut object - standard reconstruction cuts
  PairAnalysisVarCuts   *recTRD = new PairAnalysisVarCuts("recTRD","recTRD");
  recTRD->SetCutType(PairAnalysisVarCuts::kAll);
  // recTRD->AddCut("TRDTrueHits/TRDHits",                   0.7,  1.);         /// remove 'ghost' tracks
  recTRD->AddCut(PairAnalysisVarManager::kTRDHits,         3.,   4.);         /// min+max requieremnt for hits


  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv TRACK PID CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  PairAnalysisCutGroup *grpPIDCut  = new PairAnalysisCutGroup("PID","PID",PairAnalysisCutGroup::kCompOR);

  PairAnalysisVarCuts  *pidTRD     = new PairAnalysisVarCuts("pidTRD","pidTRD");
  pidTRD->AddCut(PairAnalysisVarManager::kTRDPidANN,     0.85,   10.);

  PairAnalysisVarCuts  *pidRICH    = new PairAnalysisVarCuts("pidRICH","pidRICH");
  pidRICH->AddCut(PairAnalysisVarManager::kRICHPidANN,     -0.8,   10.);

  /// example of a var cut combination:
  /// cuts can be applied e.g. for a certain range momentum range only, used for 2-dimensional
  /// PID cuts or for a cut in a pre-selected range (see possible 'AddCut'-arguments in PairAnalysisVarCutsCombi.h)
  PairAnalysisVarCutsCombi *pidCutsCombi = new PairAnalysisVarCutsCombi("PIDCuts","PIDCuts");
  //  pidCutsCombi->SetCutType(PairAnalysisVarCutCombi::kAny);

  switch(cutDefinition) {
  case kTRDRICHcfg:
    pidCutsCombi->AddCut(PairAnalysisVarManager::kTRDHits,     3.,     4., kFALSE,
			 PairAnalysisVarManager::kTRDPidANN,   0.85,  10., kFALSE); /// apply TRD Pid only for tracks with 3-4 hits
    pidCutsCombi->AddCut(PairAnalysisVarManager::kRICHHits,    6.,   100., kFALSE,
			 PairAnalysisVarManager::kRICHPidANN, -0.8,   10., kFALSE); /// apply RICH Pid only for rings with >=6 hits
    break;
  }

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv ACTIVATE CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// activate the cut sets (order might be CPU timewise important)
  switch(cutDefinition) {
  case kAcc:
    papa->           GetTrackFilter().AddCuts(accCuts);       accCuts   ->Print();
    papa->           GetTrackFilter().AddCuts(recSTS);        recSTS    ->Print();
    break;
  case kTRDcfg:
    papa->           GetTrackFilter().AddCuts(accCuts);       accCuts   ->Print();
    papa->           GetTrackFilter().AddCuts(recSTS);        recSTS    ->Print();
    papa->           GetTrackFilter().AddCuts(recTRD);        recTRD    ->Print();
    papa->           GetTrackFilter().AddCuts(pidTRD);        pidTRD    ->Print();
    break;
  case kRICHcfg:
    papa->           GetTrackFilter().AddCuts(accCuts);       accCuts   ->Print();
    papa->           GetTrackFilter().AddCuts(recSTS);        recSTS    ->Print();
    papa->           GetTrackFilter().AddCuts(recRICH);       recRICH   ->Print();
    papa->           GetTrackFilter().AddCuts(pidRICH);       pidRICH   ->Print();
    break;
  case kTRDRICHcfg:
    papa->           GetTrackFilter().AddCuts(accCuts);       accCuts   ->Print();
    papa->           GetTrackFilter().AddCuts(recSTS);        recSTS    ->Print();
    // grpPIDCut ->AddCut(pidTRD);
    // grpPIDCut ->AddCut(pidRICH);
    // papa->           GetTrackFilter().AddCuts(grpPIDCut);     grpPIDCut ->Print();
    papa->           GetTrackFilter().AddCuts(pidCutsCombi);  pidCutsCombi->Print();
    break;
  }

}

//______________________________________________________________________________________
void SetupPairCuts(PairAnalysis *papa, Int_t cutDefinition)
{
  ///
  /// Setup the pair cuts
  ///

  /// skip pair cuts if no pairing is done
  if(papa->IsNoPairing()) return;

  /// MC
  Bool_t hasMC=papa->GetHasMC();

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv PAIR PREFILTER CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// TODO: needs to be implemented
  /// example of gamma rejection cuts as prefilter in order to remove tracks from the final
  /// pairing based on pair informations
  // PairAnalysisVarCuts *gammaCuts = new PairAnalysisVarCuts("GammaCuts","GammaCuts");
  // gammaCuts->AddCut(PairAnalysisVarManager::kM,             0.,   0.05);
  // gammaCuts->AddCut(PairAnalysisVarManager::kOpeningAngle,  0.,   TMath::Pi()/8);
  // papa->GetPairPreFilter().AddCuts(gammaCuts);

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv PAIR  CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// example of rapidity selection
  PairAnalysisVarCuts *rapCut=new PairAnalysisVarCuts("RapidityCut","RapdityCut");
  rapCut->AddCut(PairAnalysisVarManager::kY,0.0,1.0);
  papa->GetPairFilter().AddCuts(rapCut);

}

//______________________________________________________________________________________
void SetupTrackCutsMC(PairAnalysis *papa, Int_t cutDefinition)
{
  ///
  /// Setup the track cuts based on MC information only
  ///
  if(!papa->GetHasMC()) return;

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv TRACK CUTS ON MCtruth vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  /// example of adding acceptance cuts
  PairAnalysisVarCuts   *varAccCutsMC = new PairAnalysisVarCuts("accCutMCtruth","accCutMCtruth");
  varAccCutsMC->SetCutType(PairAnalysisVarCuts::kAll); /// wheter kAll or kAny cut has to be fullfilled

  /// example for config specific cuts
  switch(cutDefinition) {
  case kAcc:
  case kTRDcfg:
  case kRICHcfg:
  case kTRDRICHcfg:
    varAccCutsMC->AddCut(PairAnalysisVarManager::kPtMC,           0.05, 1e30);
    // varAccCutsMC->AddCut(PairAnalysisVarManager::kEtaMC,         -0.8,   0.8);
    break;
  default:
    varAccCutsMC->AddCut(PairAnalysisVarManager::kPtMC,           0.4, 1e30);
    break;
  }

  /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv ACTIVATE CUTS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
  switch(cutDefinition) {
  default:
    papa->           GetTrackFilterMC().AddCuts(varAccCutsMC);    varAccCutsMC->Print();
  }

}

//______________________________________________________________________________________
void ConfigBgrd(PairAnalysis *papa, Int_t cutDefinition)
{
  ///
  /// Configurate the background estimators
  ///

  /// skip config
  switch(cutDefinition) {
    //  case kAcc:
  case kTRDcfg:
  case kRICHcfg:
    return;
  }

  /// Track Rotations
  /// Randomly rotate in phi one of the daughter tracks in-side a 'ConeAngle' for a given
  /// 'StartAngle'
  PairAnalysisTrackRotator *rot=new PairAnalysisTrackRotator;
  rot->SetIterations(2);               /// how often you want to rotate a track
  rot->SetConeAnglePhi(TMath::Pi());   /// cone angle
  rot->SetStartAnglePhi(TMath::Pi());  /// starting angle
  rot->SetRotationType(PairAnalysisTrackRotator::kRotateBothRandom); /// kRotatePositive, kRotateNegative or kRotateBothRandom (default)
  papa->SetTrackRotator(rot);

  /*
  /// TODO: implement Event Mixing
  /// add mixed events
  PairAnalysisMixingHandler *mix=new PairAnalysisMixingHandler;
  mix->AddVariable(PairAnalysisVarManager::kZvPrim,      "-10.,-5.,-4.,-3.,-2.,-1.,0.,1.,2.,3.,4.,5.,10.");
  mix->AddVariable(PairAnalysisVarManager::kCentrality,  9,  0.,90.);
  mix->AddVariable(PairAnalysisVarManager::kTPCrpH2,     10, TMath::Pi()/-2, TMath::Pi()/2);
  mix->SetSkipFirstEvent(kTRUE);                      /// needed for flow analysis
  mix->SetMixType(PairAnalysisMixingHandler::kAll);   /// checkout PairAnalysisMixingHandler.h (LS ME, OS ME or both)
  mix->SetDepth(150);                                 /// pool depth
  papa->SetMixingHandler(mix);
  */

}

//______________________________________________________________________________________
void InitHistograms(PairAnalysis *papa, Int_t cutDefinition)
{
  ///
  /// Initialise the histograms
  ///
  /// NOTE: Histograms are added to a specific class type such as 'event,pair,track'.
  ///       Several different objects of x-dimensions can be added via 'AddHisogram', 'AddProfile', 'AddSparse'.
  ///       Please have a look at PairAnalysisHistos.h to understand all possible input arguments.
  ///       For histograms and profiles you can use formulas with full 'TMath'-support to calculate new variables.
  ///       The type of binning is provided by some 'PairAnalysisHelper' functions.
  ///       Some examples are given below....

  Bool_t hasMC=papa->GetHasMC();

  ///Setup histogram Manager
  PairAnalysisHistos *histos=new PairAnalysisHistos(papa->GetName(),papa->GetTitle());
  papa->SetHistogramManager(histos);

  ///Initialise superior histogram classes
  histos->SetReservedWords("Hit;Track;Pair");

  ////// EVENT HISTOS /////
  histos->AddClass("Event"); /// add histogram class
  /// define output objects
  histos->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(200,-1.,1.),  PairAnalysisVarManager::kXvPrim);
  histos->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(100,0.,400.), PairAnalysisVarManager::kNVtxContrib);
  histos->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(200,0.0,10.0), "VtxChi/VtxNDF");
  histos->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(100,0.,500.), PairAnalysisVarManager::kNTrk);

  ////// PAIR HISTOS /////
  if(!papa->IsNoPairing()) {
    /// add histogram classes
    /// loop over all pair types and add classes (pair types in PairAnalysis.h EPairType)
    /// automatically skip pair types w.r.t. configured bgrd estimators
    for (Int_t i=0; i<PairAnalysis::kPairTypes; i++){
      if(!papa->DoProcessLS()      && (i==PairAnalysis::kSEPP || i==PairAnalysis::kSEMM) ) continue; /// check SE LS
      if(!papa->GetMixingHandler() && (i>=PairAnalysis::kMEPP && i<=PairAnalysis::kMEMM) ) continue; /// check ME OS,LS
      if(!papa->GetTrackRotator()  && (i==PairAnalysis::kSEPMRot)          ) continue; /// check SE ROT
      histos->AddClass(Form("Pair_%s",PairAnalysis::PairClassName(i)));
    }

    /// add MC signal histograms (if any) to pair class
    if(papa->GetMCSignals()) {
      for (Int_t i=0; i<papa->GetMCSignals()->GetEntriesFast(); ++i) {
	TString sigName = papa->GetMCSignals()->At(i)->GetName();
	if(sigName.Contains("Single")) continue; /// skip single particle signals (no pairs)
	if(i<3) continue;
	histos->AddClass(Form("Pair_%s",        sigName.Data()));
	histos->AddClass(Form("Pair_%s_MCtruth",sigName.Data()));
      }
    }

    ///// define output objects /////
    histos->AddHistogram("Pair",PairAnalysisHelper::MakeLinBinning(250,0.0,250*0.02), PairAnalysisVarManager::kM); /// 20MeV bins, 5 GeV/c2
    histos->AddHistogram("Pair", PairAnalysisHelper::MakeLinBinning(100,1.,4.5),      PairAnalysisVarManager::kYlab);
    histos->AddHistogram("Pair", PairAnalysisHelper::MakeLinBinning(250,0,5.),       PairAnalysisVarManager::kPt);
    histos->AddHistogram("Pair", PairAnalysisHelper::MakeLinBinning(125,.0,125*0.04), PairAnalysisVarManager::kM,
			 PairAnalysisHelper::MakeLinBinning(250,0,5.), PairAnalysisVarManager::kPt);
    histos->AddHistogram("Pair", PairAnalysisHelper::MakeLinBinning(100,0.,TMath::Pi()/2), PairAnalysisVarManager::kOpeningAngle);
    histos->AddHistogram("Pair", PairAnalysisHelper::MakePdgBinning(), PairAnalysisVarManager::kPdgCode);
    histos->AddHistogram("Pair", PairAnalysisHelper::MakePdgBinning(), PairAnalysisVarManager::kPdgCodeMother);
  } // pairs


  ///// TRACK HISTOS /////
  /// Track classes - legs of the pairs
  if(!papa->IsNoPairing()) {
    /// loop over all pair types and add classes (pair types in PairAnalysis.h EPairType)
    /// automatically skip pair types w.r.t. configured bgrd estimators
    for (Int_t i=0; i<PairAnalysis::kPairTypes; i++){
      if(!papa->DoProcessLS()      && (i==PairAnalysis::kSEPP || i==PairAnalysis::kSEMM) ) continue; /// check SE LS
      if(!papa->GetMixingHandler() && (i>=PairAnalysis::kMEPP && i<=PairAnalysis::kMEMM) ) continue; /// check ME OS,LS
      if(!papa->GetTrackRotator()  && (i==PairAnalysis::kSEPMRot)          ) continue; /// check SE ROT
      ///      histos->AddClass(Form("Track_Legs_%s",PairAnalysis::PairClassName(i)));
    }
  }

  /// Track classes - single tracks used any pairing
  /// loop over all leg types and add classes (leg types in PairAnalysis.h ELegType)
  for (Int_t i=0; i<PairAnalysis::kLegTypes; ++i)
    histos->AddClass(Form("Track_%s", PairAnalysis::TrackClassName(i)));
  /// OR/AND add merged leg histograms (e.g. e+-)
  histos->AddClass(Form("Track_%s",     PairAnalysis::PairClassName(PairAnalysis::kSEPM)));

  /// add MC signal (if any) histograms to pair class
  if(papa->GetMCSignals()) {
    for (Int_t i=0; i<papa->GetMCSignals()->GetEntriesFast(); ++i) {
      TString sigMCname = papa->GetMCSignals()->At(i)->GetName();
      if(i>3) continue; /// skip pair particle signals (no pairs)
      /// mc truth - pair leg class
      ///      histos->AddClass(Form("Track_Legs_%s_MCtruth",sigMCname.Data()));
      /// mc reconstructed - pair leg class
      ///      histos->AddClass(Form("Track_Legs_%s",        sigMCname.Data()));
      /// single tracks (merged +-)
      histos->AddClass(Form("Track_%s_%s",PairAnalysis::PairClassName(PairAnalysis::kSEPM),sigMCname.Data()));
      histos->AddClass(Form("Track_%s_%s_MCtruth",PairAnalysis::PairClassName(PairAnalysis::kSEPM),sigMCname.Data()));
    }
  }

  ///// define output objects /////
  histos->AddHistogram("Track", PairAnalysisHelper::MakePdgBinning(), PairAnalysisVarManager::kPdgCode);
  histos->AddHistogram("Track", PairAnalysisHelper::MakePdgBinning(), PairAnalysisVarManager::kPdgCodeMother);
  histos->AddHistogram("Track", PairAnalysisHelper::MakePdgBinning(), PairAnalysisVarManager::kPdgCodeGrandMother);
  /// MC matching
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(49,-0.5,48.5), PairAnalysisVarManager::kGeantId);
  histos->AddProfile(  "Track", PairAnalysisHelper::MakeLogBinning(23,1.,500.), PairAnalysisVarManager::kNTrk, PairAnalysisVarManager::kSTSisMC,"I");
  /// Reconstuction Quality
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(100,0.,50.), PairAnalysisVarManager::kTrackChi2NDF);
  /// Kinematics
  /// checkout http://root.cern.ch/root/html/TFormula.html#TFormula:Analyze for formula function strings
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLogBinning(400,1.e-3.,4.),  "sqrt(Xv*Xv+Yv*Yv+Zv*Zv)");
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLogBinning(400,1.e-3,4.),  "sqrt(XvMC*XvMC+YvMC*YvMC+ZvMC*ZvMC)");
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(400,0.,4.),  "Pt/PtMC");
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(400,0,4.),  PairAnalysisVarManager::kPt);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(400,0,10.),  PairAnalysisVarManager::kPout);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(316,0.,TMath::TwoPi()),  PairAnalysisVarManager::kPhi);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(100,0.,TMath::Pi()/4),  PairAnalysisVarManager::kTheta);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(100,-3.0,3.0),  PairAnalysisVarManager::kY,
		       PairAnalysisHelper::MakeLinBinning(200,0,TMath::TwoPi()),PairAnalysisVarManager::kPhi);
  //// TRD
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(6,-0.5, 5.5), PairAnalysisVarManager::kTRDHits);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(6,-0.5, 5.5), PairAnalysisVarManager::kTRDHitsMC);
  histos->AddProfile(  "Track", PairAnalysisHelper::MakeLinBinning(100,0.,10.), PairAnalysisVarManager::kPout,"TRDFakeHits/TRDHits","I;-0.5;2."); ///S
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(100,0., 5.e-4), PairAnalysisVarManager::kTRDSignal);
  /// STS
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(16,-0.5, 15.5), PairAnalysisVarManager::kSTSHits);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(101,-0.5, 50.5), PairAnalysisVarManager::kSTSChi2NDF);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(101,-0.05,10.05),  PairAnalysisVarManager::kSTSChi2NDFtoVtx);
  /// MVD
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(101,-0.5, 5.5), PairAnalysisVarManager::kMVDHits);
  /// RICH
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(46,4.5, 50.5), PairAnalysisVarManager::kRICHHits);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(101,-0.5, 50.5), PairAnalysisVarManager::kRICHChi2NDF);
  histos->AddHistogram("Track", PairAnalysisHelper::MakeLinBinning(150,-1.5, 1.5), PairAnalysisVarManager::kRICHPidANN);

  //// Example of THnSparse ////
  /// NOTE: these objects might get very, very large be carefull with the binning and number of dimensions
  /// array of bin limits
  TObjArray *limits  = new TObjArray();
  limits->Add(PairAnalysisHelper::MakeLogBinning( 10,  1.,  400.) );
  limits->Add(PairAnalysisHelper::MakeLinBinning(  8,  0.,TMath::TwoPi()) );
  limits->Add(PairAnalysisHelper::MakeLinBinning(100,  0.,    5.) );
  limits->Add(PairAnalysisHelper::MakeLinBinning(100,  0.,    5.) );
  UInt_t *vars = PairAnalysisVarManager::GetArray(PairAnalysisVarManager::kNVtxContrib,
						 PairAnalysisVarManager::kPhi,
						 PairAnalysisVarManager::kPt,
						 PairAnalysisVarManager::kPtMC
						 );
  /// add track sparse, NOTE: only for SE tracks
  //  histos->AddSparse(Form("Track_%s",PairAnalysis::PairClassName(PairAnalysis::kSEPM)), limits->GetEntriesFast(), limits, vars);

  ////// HIT HISTOS /////
  /// add histogram class
  histos->AddClass("Hit_STS");
  /// add MC signal (if any) histograms to hit class
  if(papa->GetMCSignals()) {
    for (Int_t i=0; i<papa->GetMCSignals()->GetEntriesFast(); ++i) {
      TString sigMCname = papa->GetMCSignals()->At(i)->GetName();
      if(i>3) continue; /// skip pair particle signals (only single particles)
      /// single tracks (merged +-)
      histos->AddClass(Form("Hit_TRD_%s",sigMCname.Data()));
    }
  }
  /// define output objects
  histos->AddHistogram("Hit",PairAnalysisHelper::MakeLinBinning(200,-400.,400.),  PairAnalysisVarManager::kPosX,
		       PairAnalysisHelper::MakeLinBinning(200,-400.,400.),  PairAnalysisVarManager::kPosY);
  histos->AddHistogram("Hit", PairAnalysisHelper::MakeLinBinning(100,  0.,5.e-5),  PairAnalysisVarManager::kEloss);



  ////// DEBUG //////
  TIter nextClass(histos->GetHistogramList());
  THashList *l=0;
  while ( (l=static_cast<THashList*>(nextClass())) ) {
    printf(" [D] HistogramManger: Class %s: Histograms: %04d \n", l->GetName(), l->GetEntries());
  }

} //end: init histograms

//______________________________________________________________________________________
void InitHF(PairAnalysis* papa, Int_t cutDefinition)
{
  ///
  /// Setup the Histogram Framework (a matrix of histograms)
  ///

  /// container
  PairAnalysisHF *hf=new PairAnalysisHF(papa->GetName(),papa->GetTitle());
  papa->SetHistogramArray(hf);

  ////// Add Variables and Dimensions /////
  hf->AddCutVariable(PairAnalysisVarManager::kNTrk,       PairAnalysisHelper::MakeLinBinning(5,0.,500));
  hf->AddCutVariable(PairAnalysisVarManager::kTRDMatches, PairAnalysisHelper::MakeArbitraryBinning("0.,1.,40.,50.,500."));

  ///Initialise superior histogram classes
  hf->SetReservedWords("Track;Pair");

  ////// EVENT HISTOS /////
  hf->AddClass("Event"); /// add histogram class
  /// define output objects
  hf->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(200,-1.,1.),  PairAnalysisVarManager::kXvPrim);
  hf->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(200,-1.,1.),  PairAnalysisVarManager::kYvPrim);
  hf->AddHistogram("Event",PairAnalysisHelper::MakeLinBinning(200,-1.,1.),  PairAnalysisVarManager::kZvPrim);

}

//______________________________________________________________________________________
void AddMCSignals(PairAnalysis *papa, Int_t cutDefinition){
  /// Do we have an MC handler?
  if (!papa->GetHasMC()) return;

  /// Example usage of predefined signals (see PairAnalysisSignalMC::EDefinedSignal)
  /// pair signals
  PairAnalysisSignalMC* conv = new PairAnalysisSignalMC(PairAnalysisSignalMC::kConversion);
  conv->SetFillPureMCStep(kTRUE);
  PairAnalysisSignalMC* rho0 = new PairAnalysisSignalMC(PairAnalysisSignalMC::kRho0);
  rho0->SetFillPureMCStep(kTRUE);      /// whether to fill MC truth (generator level)output (attention on CPU consumption)
  rho0->SetWeight(9 * 4.72e-05);       /// apply a weighting (fills PairAnalysisVarManager::kWeight & can be used for histogram weighting)

  /// single particle signals
  PairAnalysisSignalMC* ele = new PairAnalysisSignalMC(PairAnalysisSignalMC::kSingleInclEle);
  ele->SetFillPureMCStep(kTRUE);

  /// Example of a MC signal configuration (see PairAnalysisSignalMC.h for full functionality)
  PairAnalysisSignalMC* all = new PairAnalysisSignalMC("EPEM","EPEM");
  all->SetLegPDGs(11,-11);
  //  all->SetMotherPDGs(22,22,kTRUE,kTRUE);            /// requierement on the mother pdg code
  all->SetMothersRelation(PairAnalysisSignalMC::kSame); /// relation of the daughters
  all->SetFillPureMCStep(kTRUE);                        /// whether to fill MC truth (generator level) output
  //  all->SetCheckBothChargesLegs(kTRUE,kTRUE);        /// check both daughter charge combinations
  //  all->SetCheckBothChargesMothers(kTRUE,kTRUE);     /// check both mother charge combinations
  //  all->SetGEANTProcess(kPPrimary);                  /// set an requirement on the Geant process (see root: TMCProcess.h)

  /// activate mc signal
  switch(cutDefinition) {
  default:
    papa->AddSignalMC(ele);
    papa->AddSignalMC(conv);
    papa->AddSignalMC(rho0);
  }

}
//______________________________________________________________________________________

