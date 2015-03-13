// -------------------------------------------------------------------------
// -----                  CbmTrdMatchTracksMK source file              -----
// -----                  Created 20/02/06  by M.Krauze                -----
// -----                  Destroyed 12.09.2007 by A. Bubak             -----
// -------------------------------------------------------------------------

#include "CbmTrdMatchTracksMK.h"

#include "FairMCPoint.h"
#include "CbmTrdPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
//#include "CbmTrackMatch.h"

#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TProfile.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmTrdMatchTracksMK::CbmTrdMatchTracksMK()
  : FairTask("TRD track match"), 
    MC_noPrimRefAll(),
    RECO_noPrimRefAll(),
    NO_TOTAL_MC_REF_TRACKS(0),
    NO_TOTAL_RECO_TRACKS(0),
    NO_TOTAL_CORREC_TRECO_TRACKS(0),
    NO_TOTAL_GHOST_TRACKS(0),
    NO_TOTAL_CLONE_TRACKS(0),
    NO_TOTAL_EVENTS(0),
    NO_TOTAL_CLONES_PRIM_FAST(0),
    NO_TOTAL_CLONES_PRIM_SLOW(0),
    NO_TOTAL_CLONES_FAST(0),
    NO_TOTAL_CLONES_SLOW(0),
    RECO_Tot_PrimaryRefFast(0),
    RECO_Tot_PrimaryRefSlow(0),
    RECO_Tot_PrimaryRefAll(0),
    RECO_Tot_AllRefFast(0),
    RECO_Tot_AllRefSlow(0),
    RECO_Tot_AllRefAll(0),
    RECO_Tot_ExtraRefFast(0),
    RECO_Tot_ExtraRefSlow(0),
    RECO_Tot_ExtraRefAll(0),
    MC_Tot_PrimaryRefFast(0),
    MC_Tot_PrimaryRefSlow(0),
    MC_Tot_PrimaryRefAll(0),
    MC_Tot_AllRefFast(0),
    MC_Tot_AllRefSlow(0),   
    MC_Tot_AllRefAll(0),
    MC_Tot_ExtraRefFast(0),
    MC_Tot_ExtraRefSlow(0),
    MC_Tot_ExtraRefAll(0),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMCTracks(NULL),
    fMatchMap(),
    fVerbose(1),
    fNoHitsPerTracklet(2),
    fEffVsMom(NULL),
    fh_EffVsMom(NULL),
    fh_mom_trdtrack_good(NULL),
    fh_mom_mctrack_reco(NULL),
    fNoHitsPerTrack(NULL),
    fNoTrueHits(NULL),
    fNoTracks(NULL),
    fNoPrimHighMom(NULL),
    fNoPrimLowMom(NULL),
    fPartTy(NULL),
    fGhostTracks_meanZ(NULL),
    fXDistShort14(NULL),
    fYDistShort14(NULL),
    fXDistShortGh14(NULL),
    fYDistShortGh14(NULL),
    fXDistLong45(NULL),
    fYDistLong45(NULL),
    fXDistLongGh45(NULL),
    fYDistLongGh45(NULL),
    fXDistShort58(NULL),
    fYDistShort58(NULL),
    fXDistShortGh58(NULL),
    fYDistShortGh58(NULL),
    fXDistLong89(NULL),
    fYDistLong89(NULL),
    fXDistLongGh89(NULL),
    fYDistLongGh89(NULL),
    fXDistShort912(NULL),
    fYDistShort912(NULL),
    fXDistShortGh912(NULL),
    fYDistShortGh912(NULL),
    fh_Chi2_Ghost(NULL),
    fh_Chi2_CorrectFast(NULL),
    fh_Chi2_CorrectSlow(NULL),
    fh_Tx_Ghost(NULL),
    fh_Tx_CorrectFast(NULL),
    fh_Tx_CorrectSlow(NULL),
    fh_Ty_Ghost(NULL),
    fh_Ty_CorrectFast(NULL),
    fh_Ty_CorrectSlow(NULL),
    fh_Y_Ghost(NULL),
    fh_Y_CorrectFast(NULL),
    fh_Y_CorrectSlow(NULL),
    fh_X_Ghost(NULL),
    fh_X_CorrectFast(NULL),
    fh_X_CorrectSlow(NULL)
{
  CreateHistogramms();
}
// -------------------------------------------------------------------------



// -----   Constructor with verbosity level   ------------------------------
CbmTrdMatchTracksMK::CbmTrdMatchTracksMK(Int_t verbose)
  : FairTask("TRD track match"),
    MC_noPrimRefAll(),
    RECO_noPrimRefAll(),
    NO_TOTAL_MC_REF_TRACKS(0),
    NO_TOTAL_RECO_TRACKS(0),
    NO_TOTAL_CORREC_TRECO_TRACKS(0),
    NO_TOTAL_GHOST_TRACKS(0),
    NO_TOTAL_CLONE_TRACKS(0),
    NO_TOTAL_EVENTS(0),
    NO_TOTAL_CLONES_PRIM_FAST(0),
    NO_TOTAL_CLONES_PRIM_SLOW(0),
    NO_TOTAL_CLONES_FAST(0),
    NO_TOTAL_CLONES_SLOW(0),
    RECO_Tot_PrimaryRefFast(0),
    RECO_Tot_PrimaryRefSlow(0),
    RECO_Tot_PrimaryRefAll(0),
    RECO_Tot_AllRefFast(0),
    RECO_Tot_AllRefSlow(0),
    RECO_Tot_AllRefAll(0),
    RECO_Tot_ExtraRefFast(0),
    RECO_Tot_ExtraRefSlow(0),
    RECO_Tot_ExtraRefAll(0),
    MC_Tot_PrimaryRefFast(0),
    MC_Tot_PrimaryRefSlow(0),
    MC_Tot_PrimaryRefAll(0),
    MC_Tot_AllRefFast(0),
    MC_Tot_AllRefSlow(0),   
    MC_Tot_AllRefAll(0),
    MC_Tot_ExtraRefFast(0),
    MC_Tot_ExtraRefSlow(0),
    MC_Tot_ExtraRefAll(0),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMCTracks(NULL),
    fMatchMap(),
    fVerbose(verbose),
    fNoHitsPerTracklet(2),
    fEffVsMom(NULL),
    fh_EffVsMom(NULL),
    fh_mom_trdtrack_good(NULL),
    fh_mom_mctrack_reco(NULL),
    fNoHitsPerTrack(NULL),
    fNoTrueHits(NULL),
    fNoTracks(NULL),
    fNoPrimHighMom(NULL),
    fNoPrimLowMom(NULL),
    fPartTy(NULL),
    fGhostTracks_meanZ(NULL),
    fXDistShort14(NULL),
    fYDistShort14(NULL),
    fXDistShortGh14(NULL),
    fYDistShortGh14(NULL),
    fXDistLong45(NULL),
    fYDistLong45(NULL),
    fXDistLongGh45(NULL),
    fYDistLongGh45(NULL),
    fXDistShort58(NULL),
    fYDistShort58(NULL),
    fXDistShortGh58(NULL),
    fYDistShortGh58(NULL),
    fXDistLong89(NULL),
    fYDistLong89(NULL),
    fXDistLongGh89(NULL),
    fYDistLongGh89(NULL),
    fXDistShort912(NULL),
    fYDistShort912(NULL),
    fXDistShortGh912(NULL),
    fYDistShortGh912(NULL),
    fh_Chi2_Ghost(NULL),
    fh_Chi2_CorrectFast(NULL),
    fh_Chi2_CorrectSlow(NULL),
    fh_Tx_Ghost(NULL),
    fh_Tx_CorrectFast(NULL),
    fh_Tx_CorrectSlow(NULL),
    fh_Ty_Ghost(NULL),
    fh_Ty_CorrectFast(NULL),
    fh_Ty_CorrectSlow(NULL),
    fh_Y_Ghost(NULL),
    fh_Y_CorrectFast(NULL),
    fh_Y_CorrectSlow(NULL),
    fh_X_Ghost(NULL),
    fh_X_CorrectFast(NULL),
    fh_X_CorrectSlow(NULL)
{
  CreateHistogramms();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdMatchTracksMK::~CbmTrdMatchTracksMK() { }
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus CbmTrdMatchTracksMK::Init() {

  cout << "CbmTrdMatchTracksMK::Init: initializing...\n";

  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdMatchTracksMK::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TRD hit Array
  fHits  = (TClonesArray*) ioman->GetObject("TrdHit");
  if ( ! fHits) {
    cout << "-W- CbmTrdMatchTracksMK::Init: No TRDHit array!"
	 << endl;
    return kERROR;
  }

  // Get TrdTrack Array
  fTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmTrdMatchTracksMK::Init: No TrdTrack array!" << endl;
    return kERROR;
  }

  // Get MCTrack Array
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-E- CbmTrdMatchTracksMK::Init: No MCTrack array!" << endl;
    return kERROR;
  }


  // Get TRDPoint array
  fPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
  if ( ! fPoints ) {
    cout << "-E- CbmTrdMatchTracksMK::Init: No TRDPoint array!" << endl;
    return kERROR;
  }

  // Create and register TrdTrackMatch array
  fMatches = new TClonesArray("CbmTrackMatch",100);
  ioman->Register("TrdTrackMatch", "TRD", fMatches, kTRUE);

  MC_Tot_PrimaryRefFast = 0;

  cout << "MC_Tot_PrimaryRefFast = " << MC_Tot_PrimaryRefFast << endl;

  MC_Tot_PrimaryRefSlow = 0;
  MC_Tot_PrimaryRefAll = 0;
  MC_Tot_AllRefFast = 0;
  MC_Tot_AllRefSlow = 0;
  MC_Tot_AllRefAll = 0;


  RECO_Tot_PrimaryRefFast = 0;
  RECO_Tot_PrimaryRefSlow = 0;
  RECO_Tot_PrimaryRefAll = 0;
  RECO_Tot_AllRefFast = 0;
  RECO_Tot_AllRefSlow = 0;
  RECO_Tot_AllRefAll = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdMatchTracksMK::Exec(Option_t* opt) {
  //cout << "CbmTrdMatchTracksMK: executing...\n";
  cout << "-I- CbmTrdMatchTracksMK::Exec " << endl;

  // Clear output array
  fMatches->Clear();

  // Create some pointers and variables
  //  CbmTrdTrack*      track = NULL;
  //  CbmTrdHit*        hit   = NULL;
  //  FairMCPoint*       point = NULL;
  Int_t nTrdHit     = fHits->GetEntriesFast();
  if(nTrdHit != 0) {
    Int_t nMCTracks   = fMCTracks->GetEntriesFast();;
    //  Int_t iPoint      = 0;
    //  Int_t iMCTrack    = 0;
    //  Int_t nAll        = 0;
    //  Int_t nTrue       = 0;
    //  Int_t nWrong      = 0;
    //  Int_t nFake       = 0;
    Int_t nHitSum     = 0;
    Int_t nTrueSum    = 0;
    //  Int_t nWrongSum   = 0;
    //  Int_t nFakeSum    = 0;
    //  Int_t nMCTrackSum = 0;
    Int_t nMCTrackPrim = 0;

    //  map<Int_t, Int_t>::iterator it = 0;
    map<Int_t, Int_t>::iterator it;;

    TStopwatch cArray;
    TStopwatch cMCRef;
    TStopwatch cHighMomPrims;
    TStopwatch cLowMomPrims;
    TStopwatch cMainLoop;
    TStopwatch cIndexing;
    TStopwatch cCloneCount;

    Bool_t dispTime = false;


    CbmMCTrack* mcTrack;

    CbmMCTrack* mcTr;
    CbmTrdPoint* trdPt;

    Int_t //values for counting the clones level subdivisions
      clonePrimaryFast = 0,
      clonePrimarySlow = 0,
      cloneExtraFast        = 0,
      cloneExtraSlow        = 0;

    //A:calculating the number of reference primaries, 12 hits in trd, mom > 1GeV
    map<Int_t, Int_t> MC_nOfPrimaryRefFastTracks;
    map<Int_t, Int_t> MC_nOfPrimaryRefSlowTracks;
    map<Int_t, Int_t> MC_nOfPrimaryRefAllTracks;

    map<Int_t, Int_t> MC_nOfAllRefFast;
    map<Int_t, Int_t> MC_nOfAllRefSlow;
    map<Int_t, Int_t> MC_nOfAllRefAll;

    map<Int_t, Int_t>::iterator iNOfTrdPoints;

    vector<Int_t> MC_highMomPrims;
    vector<Int_t> MC_lowMomPrims;
    vector<Int_t> MC_highMomAll;
    vector<Int_t> MC_lowMomAll;

    vector<Int_t>::iterator iMC_highMomPrims;

    Int_t
      MC_PrimaryRefFast = 0,
      MC_PrimaryRefSlow = 0,
      MC_PrimaryRefAll  = 0,
      MC_AllRefFast     = 0,
      MC_AllRefSlow     = 0,
      MC_AllRefAll      = 0;


    // Create STL map from MC track to number of TRD hits
    map<Int_t, Int_t> mapNoHit;



    //  map<Int_t, Int_t> MC_noPrimRefAll;
    //  map<Int_t, Int_t> RECO_noPrimRefAll;

    cArray.Start();
    TVector3 momentum;

    //creating an array of primary ref fast and slow MC tracks
    for(int j=0; j<fPoints->GetEntriesFast();j++) {  // loop over TRDPoints
      trdPt = (CbmTrdPoint*)fPoints->At(j);
      if( ! trdPt) continue;
      mcTr = (CbmMCTrack*)fMCTracks->At(trdPt->GetTrackID());
      if ( ! mcTr) continue;
      //    if(mcTr->GetTrdPoints() == 12) {
      //    cout <<"No points in TRD: "<< mcTr->GetNPoints(kTRD) << endl;

      if(mcTr->GetNPoints(kTRD) == 12) {
	if( mcTr->GetMotherId() < 0) {
	  MC_nOfPrimaryRefAllTracks[trdPt->GetTrackID()]++;
	  //	if(mcTr->GetMomentum().Mag() >= 1) {
	  mcTr->GetMomentum(momentum);
	  if(momentum.Mag() >= 1) {
	    MC_nOfPrimaryRefFastTracks[trdPt->GetTrackID()]++;  //high mom track indizes
	  }
	  else {
	    MC_nOfPrimaryRefSlowTracks[trdPt->GetTrackID()]++;  //low mom track indizes
	  }
	} else { //if particle is not a primary
	  MC_nOfAllRefAll[trdPt->GetTrackID()]++;
	  mcTr->GetMomentum(momentum);
	  if(momentum.Mag() >= 1) {
	    MC_nOfAllRefFast[trdPt->GetTrackID()]++;  //high mom track indizes
	  }
	  else {
	    MC_nOfAllRefSlow[trdPt->GetTrackID()]++;  //low mom track indizes
	  }
	}
      }
    }
    cArray.Stop();
    cMCRef.Start();
    //---------------------------------------------------
    //For MC Reference tracks

    for(iNOfTrdPoints = MC_nOfPrimaryRefFastTracks.begin();
	iNOfTrdPoints != MC_nOfPrimaryRefFastTracks.end();
	iNOfTrdPoints++) {
      MC_highMomPrims.push_back(iNOfTrdPoints->first);  //vector of high mom tracks
      MC_PrimaryRefFast++;

      //      nMCTrackPrim++;
    }

    for(iNOfTrdPoints = MC_nOfPrimaryRefSlowTracks.begin();
	iNOfTrdPoints != MC_nOfPrimaryRefSlowTracks.end();
	iNOfTrdPoints++) {
      MC_lowMomPrims.push_back(iNOfTrdPoints->first);  //vector of low mom tracks
      MC_PrimaryRefSlow++;

      //      nMcLowMomTrackPrim++;
    }

    //------------------------------------------------------
    //For MC All tracks

    for(iNOfTrdPoints = MC_nOfAllRefFast.begin();

	iNOfTrdPoints != MC_nOfAllRefFast.end();
	iNOfTrdPoints++) {
      MC_highMomAll.push_back(iNOfTrdPoints->first);  //vector of high mom tracks
      MC_AllRefFast++;

      //      nMCTrackPrim++;
    }

    for(iNOfTrdPoints = MC_nOfAllRefSlow.begin();
	iNOfTrdPoints != MC_nOfAllRefSlow.end();
	iNOfTrdPoints++) {
      MC_lowMomAll.push_back(iNOfTrdPoints->first);  //vector of low mom tracks
      MC_AllRefSlow++;

      //      nMcLowMomTrackPrim++;
    }

    cMCRef.Stop();

    MC_PrimaryRefAll = MC_PrimaryRefFast + MC_PrimaryRefSlow;

    MC_AllRefAll = MC_AllRefFast + MC_AllRefSlow;



    Int_t
      My_PrimaryRefFast =  MC_highMomPrims.size(),
      My_PrimaryRefSlow =  MC_lowMomPrims.size(),
      My_PrimaryRefAll  =  MC_nOfPrimaryRefAllTracks.size(),
      My_AllRefFast     =  MC_nOfAllRefFast.size() + My_PrimaryRefFast,
      My_AllRefSlow     =  MC_nOfAllRefSlow.size() + My_PrimaryRefSlow,
      My_AllRefAll      =  MC_nOfAllRefAll.size() + My_PrimaryRefAll,
      My_ExtraRefFast   =  MC_nOfAllRefFast.size(),
      My_ExtraRefSlow   =  MC_nOfAllRefSlow.size(),
      My_ExtraRefAll    =  My_ExtraRefFast + My_ExtraRefSlow;



    if(fVerbose > 2) {
      cout
	<< "My_PrimaryRefFast  = " << My_PrimaryRefFast << endl
	<< "My_PrimaryRefSlow  = " << My_PrimaryRefSlow << endl
	<< "My_PrimaryRefAll   = " << My_PrimaryRefAll  << endl
	<< "My_AllRefFast  = " << My_AllRefFast << endl
	<< "My_AllRefSlow  = " << My_AllRefSlow << endl
	<< "My_AllRefAll   = " << My_AllRefAll  << endl;


      cout
	<< "MC_PrimaryRefFast  = " << MC_PrimaryRefFast << endl
	<< "MC_PrimaryRefSlow  = " << MC_PrimaryRefSlow << endl
	<< "MC_PrimaryRefAll   = " << MC_PrimaryRefAll  << endl
	<< "MC_AllRefFast  = " << MC_AllRefFast << endl
	<< "MC_AllRefSlow  = " << MC_AllRefSlow << endl
	<< "MC_AllRefAll   = " << MC_AllRefAll  << endl;



    }

    //  cout << "\nMC_nOfPrimaryRefFastTracks = " << MC_nOfPrimaryRefFastTracks.size() << endl;


    //   for(int i=0;i<fMCTracks->GetEntries();i++) {
    //     mcTrack = (CbmMCTrack*)fMCTracks->At(i);
    //     if(mcTrack->GetMotherId() == -1) {
    //       if(mcTrack->GetTrdPoints() == 12) {
    // 	if(mcTrack->GetMomentum().Mag() < 1) {
    // 	  nMcLowMomTrackPrim++;
    // 	}
    //       }
    //     }
    //   }

    //A:end of calculating referenc primaries


    map<Int_t, Int_t>mcToReco;
    map<Int_t, Int_t>recoToMc; //delivers a number of reconstructed tracks with a given ID
    map<Int_t, Int_t>recoToMcGhost; // :ab:
    map<Int_t, Int_t> recoToLowMc;

    //  map<Int_t, Int_t>::iterator irecoToMcGhost; // :ab:
    map<Int_t, Int_t>::iterator irecoToMc;

    CbmTrdTrack* trdTr1;
    CbmTrdHit* trdHit1;
    CbmTrdHit* trdHit;
    CbmTrdPoint* trdPoint1;
    //  CbmTrdPoint* trdPoint2;
    //  CbmMCTrack* mcTrack1;

    Int_t mcInd;
    Int_t recoInd;

    if(fVerbose > 1)  cout << "Size of the fTracks = " << fTracks->GetEntries() << endl;

    map<Int_t, Bool_t> usedMCTracks;       //says if an MCTrack has a corresponding reco track
    map<Int_t, Bool_t> usedLowMomMCTracks; //says if an MCTrack has a corresponding low mom reco track
    map<Int_t, Bool_t> usedRecoTracks;     //says if a reco track has a corresponding MCTrack

    if(fVerbose > 1) cout << "Preparing the primaries vector...\n";

    map<Int_t, Int_t> usedLowMom;

    cHighMomPrims.Start();


    TVector3 momMcTrack;
    if(fVerbose > 1) cout << "--- making high mom prims...\n";
    for(iMC_highMomPrims = MC_highMomPrims.begin();
	iMC_highMomPrims != MC_highMomPrims.end();
	iMC_highMomPrims++) {
      //    usedMCTracks[*iMC_highMomPrims] = true; //says if an index belongs to a high mom primary

      mcInd = *iMC_highMomPrims;
      mcTrack = (CbmMCTrack*)fMCTracks->At(mcInd);
      mcTrack->GetMomentum(momMcTrack);
      Int_t iMapBin = Int_t(momMcTrack.Mag()*10);
      MC_noPrimRefAll[iMapBin]++;
    }


    cHighMomPrims.Stop();
    if(fVerbose > 1) cout << "--- making low mom prims...\n";
    cLowMomPrims.Start();
    for(iMC_highMomPrims = MC_lowMomPrims.begin();
	iMC_highMomPrims != MC_lowMomPrims.end();
	iMC_highMomPrims++) {
      //    usedLowMom[*iMC_highMomPrims]++;
      //    usedLowMomMCTracks[*iMC_highMomPrims] = true; //says if an index belongs to a low mom primary

      mcInd = *iMC_highMomPrims;
      mcTrack = (CbmMCTrack*)fMCTracks->At(mcInd);
      mcTrack->GetMomentum(momMcTrack);
      Int_t iMapBin = Int_t(momMcTrack.Mag()*10);
      MC_noPrimRefAll[iMapBin]++;
    }
    cLowMomPrims.Stop();

    Int_t noPrimaryRefSlow = 0;


    if(fVerbose > 1) cout << "High mom prims = " << MC_highMomPrims.size() << endl;

    if(fVerbose > 1) cout << " Low mom prims = " << MC_lowMomPrims.size() << endl;

    //  for(

    if(fVerbose > 1) cout << "Engaging loop over reco tracks..." << endl;
    Int_t noGhost = 0, noOther = 0;


    //  Double_t meanZ;
    Int_t
      total1 = 0,
      total2 = 0;

    Int_t
      noRefFast    = 0,  // 12, high GeV x
      noRefSlow    = 0,  // 12, low GeV x
      noRefAll     = 0,  // 12, any GeV x
      noAllTrack   = 0;  // any station, any GeV x
    //    noExtraTrack = 0;  // from reference, 12, any GeV x


    map<Int_t, Int_t> mapPrimRefAll;

    map<Int_t, Int_t> refSlow;
    map<Int_t, Int_t> refFast;

    map<Int_t, Int_t> allRefSlow;
    map<Int_t, Int_t> allRefFast;

    map<Int_t, Int_t> mapAllRefAll;

    // 2 maps used to calculate the eff vs mom plot
    // iMap = Int_t(momentum/100);


    Int_t
      RECO_PrimaryRefFast = 0,
      RECO_PrimaryRefSlow = 0,
      RECO_PrimaryRefAll  = 0,
      RECO_AllRefFast     = 0,
      RECO_AllRefSlow     = 0,
      RECO_AllRefAll      = 0,
      RECO_ExtraRefFast     = 0,
      RECO_ExtraRefSlow     = 0,
      RECO_ExtraRefAll      = 0;



    map<Int_t,Int_t> noTrackHits;
    Int_t hitsum = 0;
    Double_t maxPercent = 0.7;
    //Double_t maxPercent = 1.;
    Bool_t isGhost = true;

    //creating a map with a number of hits per track "noTrackHits"
    //

    if(fVerbose > 1) cout << "--- checking for ghosts..." << endl;

    cMainLoop.Start();

    Int_t
      //    noPrimaries = 0,
      noReferences = 0,
      noIter = 0;

    Int_t noTrcks = Int_t(fTracks->GetEntries());
    Int_t noHts;
    //  map<Int_t,Int_t>::iterator it1;
    if(fVerbose > 1) cout <<"--- Number of tracks: "<< noTrcks <<endl;

    for(int i=0;i<noTrcks;i++) { //main loop
      //  for(int i=0;i<fTracks->GetEntries();i++) { //main loop
      trdTr1 = (CbmTrdTrack*)fTracks->At(i);
      if(trdTr1 != NULL) { //if trdTrack != NULL
	hitsum = 0;
	noTrackHits.clear();
	if(fVerbose > 2) {
	  cout << "checking for good hits...\n";
	  cout << "Number of hits per track: " << trdTr1->GetNofHits() << endl;
	}

	noHts = trdTr1->GetNofHits();
	//if(fVerbose > 2) cout << "noHits= " << noHts << endl;
	for(int iHit=0;iHit < noHts;iHit++) { //checking for good hits
	  //a condition was here, probably by mistake
	  if(fVerbose > 2)
	    cout << "We have a TrdHit here." << endl;

	  trdHit1 = (CbmTrdHit*)fHits->At(trdTr1->GetHitIndex(iHit));
	  if( ! trdHit1) { cout << "!!! Empty pointer to TrdHit !!!" << endl; continue; }
	  trdPoint1 = (CbmTrdPoint*)fPoints->At(trdHit1->GetRefId());
	  if(fVerbose > 2) {
	    cout
	      << "TrdHitIndex= " << trdTr1->GetHitIndex(iHit)
	      << "TrdPointIndex= " << trdHit1->GetRefId() << endl;

	  }
	  noTrackHits[trdPoint1->GetTrackID()]++;
	  hitsum++;
	  noIter++;
	}

	//      recoInd = trdPoint1->GetTrackID();

	//if(recoInd == trdPoint2->GetTrackID()) { //if reco_tr_id == mc_tr_id
	isGhost = true;

	if(fVerbose > 2)
	  cout << "Hitsum = " << hitsum << endl;
	//      if(noTrackHits.size() <=2) {//if not a ghost track, size<=2 means at least good 4 hits, 70%
	//not exactly!!

	//      cout << "checking for ghosts...\n";

	for(it = noTrackHits.begin();
	    it != noTrackHits.end();
	    it++) {
	  if(Double_t(it->second)/Double_t(hitsum) >= maxPercent) {
	    isGhost = false;
	    recoInd = it->first;
	    break;
	  }
	}

	//Double_t chi2;
	//chi2 = trdTr1->GetChi2()/Double_t((trdTr1->GetNDF()));
	//chi2 = trdTr1->GetChi2();
	FairTrackParam *param;
	param = (FairTrackParam*)trdTr1->GetParamLast();

	Double_t tx = param->GetTx();
	Double_t ty = param->GetTy();
	Double_t Y  = param->GetY();
	Double_t X  = param->GetX();

	//cout <<"(tx, ty) "<< tx <<", "<< ty << endl;

	TVector3 momG;

	if(!isGhost) { //is not a ghost, 70% hits are ok
	  //	  cout << "it's not a ghost...\n";


	  mcTrack = (CbmMCTrack*)fMCTracks->At(recoInd);
	  //if(mcTrack->GetMotherId() == -1) { // noPrimaries++;
	  if(mcTrack->GetNPoints(kTRD) == 12) { //ref
	    if(fVerbose > 2)
	      cout << "it's a reference track...\n";
	    noReferences++;
	    mapAllRefAll[recoInd]++;
	    noRefAll++;

	    mcTrack->GetMomentum(momG);
	    Double_t mom = momG.Mag();
	    fh_mom_trdtrack_good->Fill(mom);

	    if(mcTrack->GetMotherId() == -1) { //primary
	      if(fVerbose > 2)
		cout << "it's a primary track...\n";
	      //	    Int_t
	      noAllTrack++;
	      mapPrimRefAll[recoInd]++;

	      mcTrack->GetMomentum(momG);
	      if(momG.Mag() >= 1) { //fast
		if(fVerbose > 2)
		  cout << "it's a fast track...\n";
		noRefFast++;
		//fh_Chi2_CorrectFast->Fill(chi2);
		fh_Tx_CorrectFast->Fill(tx);
		fh_Ty_CorrectFast->Fill(ty);
		fh_Y_CorrectFast->Fill(Y);
		fh_X_CorrectFast->Fill(X);
		//RECO_PrimaryRefFast++;//
		//	      refFast[recoInd]++;
	      } else { //slow
		if(fVerbose > 2)
		  cout << "it's a slow track...\n";
		noRefSlow++;
		//fh_Chi2_CorrectSlow->Fill(chi2);
		fh_Tx_CorrectSlow->Fill(tx);
		fh_Ty_CorrectSlow->Fill(ty);
		fh_Y_CorrectSlow->Fill(Y);
		fh_X_CorrectSlow->Fill(X);
		//	      refSlow[recoInd]++;
	      }
	      total1++;
	      if(fVerbose > 2)
		cout << "Let's see it it corresponds to an MC track...\n";

	      // ------------------------------------
	      if(MC_nOfPrimaryRefFastTracks.find(recoInd) != MC_nOfPrimaryRefFastTracks.end()) {
		recoToMc[recoInd]++;
		total2++;
		continue;
	      }
	      // ------------------------------------

	      //---------------------------------
	      if(MC_nOfPrimaryRefSlowTracks.find(recoInd) != MC_nOfPrimaryRefSlowTracks.end()) {
		recoToLowMc[recoInd]++;
		continue;
	      }
	      //---------------------------------

	    }else { //not primary
	      mcTrack->GetMomentum(momG);
	      if(momG.Mag() >= 1) { //fast

		// -----------------------------------
		if(MC_nOfAllRefFast.find(recoInd) != MC_nOfAllRefFast.end()) {
		  allRefFast[recoInd]++;
		  continue;
		}
		// ------------------------------------

	      } else { //slow

		// -----------------------------
		if(MC_nOfAllRefSlow.find(recoInd) != MC_nOfAllRefSlow.end()) {
		  allRefSlow[recoInd]++;
		  continue;
		}
	      }
	      // -----------------------------

	    }
	  }
	} else { // :ab:
	  noGhost++;
	  //fh_Chi2_Ghost->Fill(chi2);
	  fh_Tx_Ghost->Fill(tx);
	  fh_Ty_Ghost->Fill(ty);
	  fh_Y_Ghost->Fill(Y);
	  fh_X_Ghost->Fill(X);

	} //end of part :ab:
      }
    }





    Int_t pointIndex = 0;
    CbmTrdPoint* trdPoint;
    Int_t mcTrackIndex = 0;


    // Loop over TRD hits
    for(Int_t iHit = 0; iHit < nTrdHit; iHit++) {
      // Get pointer to the TRD hit
      trdHit = (CbmTrdHit*) fHits->At(iHit);
      if(NULL == trdHit) continue;
      // Get index of point
      pointIndex = trdHit->GetRefId();
      if(pointIndex < 0) continue;
      // Get pointer to the TRD point
      trdPoint = (CbmTrdPoint*) fPoints->At(pointIndex);
      if(NULL == trdPoint) continue;
      // Get index of MC track
      mcTrackIndex = trdPoint->GetTrackID();
      // Increment number of TRD hits
      mapNoHit[mcTrackIndex] += 1;
    } // Loop over TRD hits




    Double_t mom = 0;
    // Loop over MC tracks
    TVector3 momM;
    for(Int_t iTrack = 0; iTrack < nMCTracks; iTrack++) {
      // Get pointer to the MC track
      mcTrack = (CbmMCTrack*) fMCTracks->At(iTrack);
      if(NULL == mcTrack) continue;
      // Get momentum value
      mcTrack->GetMomentum(momM);
      mom = momM.Mag();
      // Skip if not reconstructable in TRD
      if(mapNoHit[iTrack] != 12) continue;
      // Fill histogramms
      fh_mom_mctrack_reco->Fill(mom);
    }// Loop over MC tracks







    cMainLoop.Stop();


    //    }
    //  }

    //  cout << "No primaries = " << noPrimaries << ", no references = " << noReferences << endl;
    if(fVerbose > 1) cout << "### noIter = " << noIter << endl;

    if(fVerbose > 1) cout << "--- the main loop finished. now indexing the results... " << endl;

    cIndexing.Start();

    TVector3 momN;
    for(irecoToMc  = recoToMc.begin();
	irecoToMc != recoToMc.end();
	irecoToMc++) {

      mcTrack = (CbmMCTrack*)fMCTracks->At(irecoToMc->first);

      mcTrack->GetMomentum(momN);
      Int_t iMapBin = Int_t(momN.Mag()*10);
      RECO_noPrimRefAll[iMapBin]++;
    }

    for(irecoToMc  = recoToLowMc.begin();
	irecoToMc != recoToLowMc.end();
	irecoToMc++) {

      mcTrack = (CbmMCTrack*)fMCTracks->At(irecoToMc->first);
      mcTrack->GetMomentum(momN);
      Int_t iMapBin = Int_t(momN.Mag()*10);
      RECO_noPrimRefAll[iMapBin]++;
    }

    cIndexing.Stop();

    RECO_PrimaryRefFast = recoToMc.size();
    RECO_PrimaryRefSlow = recoToLowMc.size();
    RECO_PrimaryRefAll = mapPrimRefAll.size();

    RECO_AllRefFast = allRefFast.size() + RECO_PrimaryRefFast;
    RECO_AllRefSlow = allRefSlow.size() + RECO_PrimaryRefSlow;
    RECO_AllRefAll = mapAllRefAll.size();

    RECO_ExtraRefFast  = allRefFast.size();
    RECO_ExtraRefSlow  = allRefSlow.size();
    RECO_ExtraRefAll   = RECO_ExtraRefFast + RECO_ExtraRefSlow;

    if(fVerbose > 1) {
      cout
	<< "RECO_PrimaryRefFast = " << RECO_PrimaryRefFast << endl
	<< "RECO_PrimaryRefSlow = " << RECO_PrimaryRefSlow << endl
	<< " RECO_PrimaryRefAll = " << RECO_PrimaryRefAll << endl
	<< "    RECO_AllRefFast = " << RECO_AllRefFast << endl
	<< "    RECO_AllRefSlow = " << RECO_AllRefSlow << endl
	<< "     RECO_AllRefAll = " << RECO_AllRefAll << endl
	<< endl;
    }


    //  cout << "noPrimaryRefSlow = " << noPrimaryRefSlow << endl;


    Int_t noClones = 0;
    Int_t noAllSeg = 0;
    Int_t noFours  = 0;

    //MC_ID -> no_reco_tracks
    Int_t MC_ind;

    cCloneCount.Start();

    for(irecoToMc  = recoToMc.begin();
	irecoToMc != recoToMc.end();
	irecoToMc++) {
      //    cout << "Map of clones = " <<  irecoToMc->second << endl;
      noClones += irecoToMc->second - 1;
      noAllSeg += irecoToMc->second;
      //    cout << "clone second Prim Fast=" << irecoToMc->second << endl;

      //    if(irecoToMc->second == 5) noFours++;
      //    MC_ind = irecoToMc->first;
      //    mcTr = (CbmMCTrack*)fMCTracks->At(MC_ind); //see the particle momentum
      //    cout << "ID = " << MC_ind << ", P=" << mcTr->GetMomentum().Mag() << endl;
      //    if(mcTr->GetMomentum().Mag() >= 1) {
      //      cloneExtraFast++;
      // if( mcTr->GetMotherId() < 0) {
      clonePrimaryFast += irecoToMc->second - 1;
      //    }
      //    }
    }

    for(irecoToMc  = recoToLowMc.begin();
	irecoToMc != recoToLowMc.end();
	irecoToMc++) {
      //    cout << "Map of clones = " <<  irecoToMc->second << endl;
      noClones += irecoToMc->second - 1;
      noAllSeg += irecoToMc->second;
      //    cout << "clone second Prim Slow=" << irecoToMc->second << endl;

      //    if(irecoToMc->second == 5) noFours++;
      MC_ind = irecoToMc->first;
      //    cout <<
      //    mcTr = (CbmMCTrack*)fMCTracks->At(MC_ind); //see the particle momentum
      //    cout << "ID = " << MC_ind << ", P=" << mcTr->GetMomentum().Mag() << endl;
      //    if(mcTr->GetMomentum().Mag() < 1) {
      //    cloneExtraSlow++;
      //    if( mcTr->GetMotherId() < 0) {
      clonePrimarySlow += irecoToMc->second - 1 ;
      //    }
      //  }
    }

    for(irecoToMc  = allRefFast.begin();
	irecoToMc != allRefFast.end();
	irecoToMc++) {
      noClones += irecoToMc->second - 1;
      noAllSeg += irecoToMc->second;
      //    cout << "clone second Fast=" << irecoToMc->second << endl;

      //    if(irecoToMc->second == 5) noFours++;
      //    MC_ind = irecoToMc->first;
      cloneExtraFast  += irecoToMc->second - 1 ;
    }

    for(irecoToMc  = allRefSlow.begin();
	irecoToMc != allRefSlow.end();
	irecoToMc++) {
      noClones += irecoToMc->second - 1;
      noAllSeg += irecoToMc->second;
      //    cout << "clone second Slow=" << irecoToMc->second << endl;

      //    if(irecoToMc->second == 5) noFours++;
      //    MC_ind = irecoToMc->first;
      cloneExtraSlow += irecoToMc->second - 1;
    }

    cCloneCount.Stop();

    if(fVerbose > 2) {
      cout << "We have " << noFours << " fours." << endl;
      cout << "We have " << noAllSeg << " segments." << endl;

      cout << "We have " << noClones << " clones." << endl;
    }

    // Count the number of ghost----------------------------
    //   for(irecoToMcGhost  = recoToMcGhost.begin();
    //       irecoToMcGhost != recoToMcGhost.end();
    //       irecoToMcGhost++) {

    //     //if(usedMCTracks[irecoToMcGhost->first]) {
    //     noGhost =+ irecoToMcGhost->second;
    //     //} else noOther =+ irecoToMcGhost->second;

    //   }
    //------------------------------------------------------

    if(fVerbose > 2)  cout << "We have " << noGhost << " ghosts." << endl
			   << "We have " << noOther << " others."
			   << endl;

    Int_t noPrimaryRefFast = 0;

    if(fVerbose > 2) cout << "\nComparing two maps...\n";

    if(fVerbose > 2) cout << "Number of a common matches = " << noPrimaryRefFast
			  << endl;



    //----------------------------------------------------------------


    // Event statistics
    Int_t nTracks = fTracks->GetEntries();


    RECO_Tot_PrimaryRefFast    += RECO_PrimaryRefFast;
    RECO_Tot_PrimaryRefSlow    += RECO_PrimaryRefSlow;
    RECO_Tot_PrimaryRefAll     += RECO_PrimaryRefAll;

    RECO_Tot_AllRefFast        += RECO_AllRefFast;
    RECO_Tot_AllRefSlow        += RECO_AllRefSlow;
    RECO_Tot_AllRefAll         += RECO_AllRefAll;

    RECO_Tot_ExtraRefFast      += RECO_ExtraRefFast;
    RECO_Tot_ExtraRefSlow      += RECO_ExtraRefSlow;
    RECO_Tot_ExtraRefAll       += RECO_ExtraRefAll;

    if(fVerbose > 1) {
      cout
	<< "MC_Tot_PrimaryRefFast = " << MC_Tot_PrimaryRefFast << endl
	<< "    MC_PrimaryRefFast = " << MC_PrimaryRefFast << endl;
    }


    MC_Tot_PrimaryRefFast      += MC_PrimaryRefFast;
    MC_Tot_PrimaryRefSlow      += MC_PrimaryRefSlow;
    MC_Tot_PrimaryRefAll       += MC_PrimaryRefAll;

    MC_Tot_AllRefFast          += My_AllRefFast;
    MC_Tot_AllRefSlow          += My_AllRefSlow;
    MC_Tot_AllRefAll           += My_AllRefAll;

    MC_Tot_ExtraRefFast        += My_ExtraRefFast;
    MC_Tot_ExtraRefSlow        += My_ExtraRefSlow;
    MC_Tot_ExtraRefAll         += My_ExtraRefAll;



    //Suspicious
    NO_TOTAL_MC_REF_TRACKS += nMCTrackPrim;
    NO_TOTAL_RECO_TRACKS += nTracks;
    NO_TOTAL_CORREC_TRECO_TRACKS += recoToMc.size();
    //


    NO_TOTAL_GHOST_TRACKS += noGhost;
    NO_TOTAL_CLONE_TRACKS += noClones;
    NO_TOTAL_EVENTS++;

    NO_TOTAL_CLONES_PRIM_FAST += clonePrimaryFast;
    NO_TOTAL_CLONES_PRIM_SLOW += clonePrimarySlow;
    NO_TOTAL_CLONES_FAST += cloneExtraFast;
    NO_TOTAL_CLONES_SLOW += cloneExtraSlow;


    Double_t qTrue = 0.;
    if ( nHitSum) qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 1.;
    if (fVerbose > 1) { //if verbose > 1 then print
      Double_t recoTrs   = Double_t(recoToMc.size());
      noPrimaryRefFast    = Int_t(recoTrs);

      Double_t qGhosts = -42;
      if(nTracks) qGhosts  = Double_t(noGhost)/Double_t(nTracks)*1.;

      Double_t qClones = -42.;
      if(My_AllRefFast+My_AllRefSlow) qClones  = Double_t(noClones)/Double_t(My_AllRefFast+My_AllRefSlow)*1.;

      Double_t qPrimaryRefFast = -42.;
      if(MC_PrimaryRefFast) qPrimaryRefFast = Double_t(RECO_PrimaryRefFast)/Double_t(MC_PrimaryRefFast)*1.;

      Double_t qPrimaryRefSlow = -42.;
      if(MC_PrimaryRefSlow) qPrimaryRefSlow = Double_t(RECO_PrimaryRefSlow)/Double_t(MC_PrimaryRefSlow)*1.;

      Double_t qPrimaryRefAll = -42.;
      if(MC_PrimaryRefAll) qPrimaryRefAll = Double_t(RECO_PrimaryRefAll)/Double_t(MC_PrimaryRefAll)*1.;

      Double_t qAllRefFast = -42.;
      if(My_AllRefFast) qAllRefFast = Double_t(RECO_AllRefFast)/Double_t(My_AllRefFast)*1.;

      Double_t qAllRefSlow = -42.;
      if(My_AllRefSlow) qAllRefSlow = Double_t(RECO_AllRefSlow)/Double_t(My_AllRefSlow)*1.;

      Double_t qAllRefAll = -42.;
      if(My_AllRefAll) qAllRefAll = Double_t(RECO_AllRefAll)/Double_t(My_AllRefAll)*1.;

      Double_t qExtraRefFast = -42.;
      if(My_ExtraRefFast) qExtraRefFast = Double_t(RECO_ExtraRefFast)/Double_t(My_ExtraRefFast)*1.;

      Double_t qExtraRefSlow = -42.;
      if(My_ExtraRefSlow) qExtraRefSlow = Double_t(RECO_ExtraRefSlow)/Double_t(My_ExtraRefSlow)*1.;

      Double_t qExtraRefAll = -42.;
      if(My_ExtraRefAll) qExtraRefAll = Double_t(RECO_ExtraRefAll)/Double_t(My_ExtraRefAll)*1.;

      Double_t qClonesPrimFast = -42.;
      if(MC_PrimaryRefFast) qClonesPrimFast = Double_t(clonePrimaryFast)/Double_t(MC_PrimaryRefFast)*1.;

      Double_t qClonesPrimSlow = -42.;
      if(MC_PrimaryRefSlow) qClonesPrimSlow = Double_t(clonePrimarySlow)/Double_t(MC_PrimaryRefSlow)*1.;

      Double_t qClonesFast = -42.;
      if(My_AllRefFast) qClonesFast = Double_t(cloneExtraFast+clonePrimaryFast)/Double_t(My_AllRefFast)*1.;

      Double_t qClonesSlow = -42.;
      if(My_AllRefSlow) qClonesSlow = Double_t(cloneExtraSlow+clonePrimarySlow)/Double_t(My_AllRefSlow)*1.;

      Double_t qExtraClonesFast = -42.;
      if(My_ExtraRefFast) qExtraClonesFast = Double_t(cloneExtraFast)/Double_t(My_ExtraRefFast)*1.;

      Double_t qExtraClonesSlow = -42.;
      if(My_ExtraRefSlow) qExtraClonesSlow = Double_t(cloneExtraSlow)/Double_t(My_ExtraRefSlow)*1.;


      cout.setf(ios::fixed, ios::floatfield);
      cout.setf(ios::showpoint);
      Int_t mW = 7;
      cout.precision(3);

      cout << endl;
      cout << "-------------------------------------------------------"
	   << endl;
      cout << "-I-                TRD Event #" << NO_TOTAL_EVENTS << " Summary              -I-"
	   << endl;
       cout << "        Reconstructed TrdTracks   : " << std::setw(mW+1) << nTracks << endl;

      cout// << fixed
	<< "    Eff. of primary reference all : " << std::setw(mW) << qPrimaryRefAll << "" << std::setw(mW) << RECO_PrimaryRefAll << endl
	<< "   Eff. of primary reference fast : " << std::setw(mW) << qPrimaryRefFast <<"" << std::setw(mW) <<RECO_PrimaryRefFast << endl
	<< "   Eff. of primary reference slow : " <<std::setw(mW) << qPrimaryRefSlow << "" << std::setw(mW) <<RECO_PrimaryRefSlow << endl
	<< "-------------------------------------------------------" << endl
	<< "      Eff. of extra reference all : " << std::setw(mW) <<  qExtraRefAll << "" << std::setw(mW) <<RECO_ExtraRefAll << endl
	<< "     Eff. of extra reference fast : " << std::setw(mW) <<  qExtraRefFast << "" << std::setw(mW) <<RECO_ExtraRefFast << endl
	<< "     Eff. of extra reference slow : " << std::setw(mW) <<  qExtraRefSlow << "" << std::setw(mW) <<RECO_ExtraRefSlow << endl
	<< "-------------------------------------------------------" << endl
	<< "        Eff. of all reference all : " << std::setw(mW) <<qAllRefAll << "" << std::setw(mW) <<RECO_AllRefAll << endl
	<< "       Eff. of all reference fast : " << std::setw(mW) <<qAllRefFast << "" << std::setw(mW) <<RECO_AllRefFast << endl
	<< "       Eff. of all reference slow : " << std::setw(mW) <<qAllRefSlow << "" << std::setw(mW) <<RECO_AllRefSlow << endl
	<< "-------------------------------------------------------" << endl
	<< "                      Ghost level : " << std::setw(mW) <<qGhosts << "" << std::setw(mW) <<noGhost << endl
	<< "                Total clone level : " << std::setw(mW) <<qClones << "" << std::setw(mW) <<noClones << endl
	<< "-------------------------------------------------------" << endl
	<< "         Primary fast clone level : " << std::setw(mW) <<qClonesPrimFast << "" << std::setw(mW) <<clonePrimaryFast << endl
	<< "         Primary slow clone level : " << std::setw(mW) <<qClonesPrimSlow << "" << std::setw(mW) <<clonePrimarySlow << endl
	<< "-------------------------------------------------------" << endl
	<< "           Extra fast clone level : " << std::setw(mW) <<qExtraClonesFast << "" << std::setw(mW) <<cloneExtraFast << endl
	<< "           Extra slow clone level : " << std::setw(mW) <<qExtraClonesSlow << "" << std::setw(mW) <<cloneExtraSlow << endl
	<< "-------------------------------------------------------" << endl
	<< "             All fast clone level : " << std::setw(mW) <<qClonesFast << "" << std::setw(mW) <<cloneExtraFast+clonePrimaryFast << endl
	<< "             All slow clone level : " << std::setw(mW) <<qClonesSlow << "" << std::setw(mW) <<cloneExtraSlow+clonePrimarySlow << endl
	<< "--------------------------------------------------------"
	<< endl
	<< "  Size of the MC_noPrimRefAll map = " << MC_noPrimRefAll.size() << endl
	<< "Size of the RECO_noPrimRefAll map = " << RECO_noPrimRefAll.size() << endl

	<< endl;

      Int_t max;
      if(MC_noPrimRefAll.size() > RECO_noPrimRefAll.size()) max=MC_noPrimRefAll.size();
      else max=RECO_noPrimRefAll.size();

      Int_t
	RECO_sum = 0,
	MC_sum = 0;

      for(int j=0;j<Int_t(RECO_noPrimRefAll.size());j++) {
	RECO_sum += RECO_noPrimRefAll[j];
      }

      for(int j=0;j<Int_t(MC_noPrimRefAll.size());j++) {
	MC_sum += MC_noPrimRefAll[j];
      }

      if(fVerbose > 1) {
	cout << "RECO sum = " << RECO_sum << endl
	     << "  MC_sum = " << MC_sum << endl;
      }

      map<Int_t, Int_t> effDiff;


      for(int m=0;m<100;m++) {
	//      Double_t denom;
	Double_t qEff;
	if(MC_noPrimRefAll[m] == 0) qEff = 0;
	else
	  qEff = 100*Double_t(RECO_noPrimRefAll[m])/Double_t(MC_noPrimRefAll[m]);

	Int_t
	  reco = RECO_noPrimRefAll[m],
	  mc = MC_noPrimRefAll[m];

	mom = Double_t(m);

	//      cout << "Filling the values: (" << mom << ", " << qEff << ")" << endl;
	for(int k=0;k<RECO_noPrimRefAll[m];k++) {
	  fEffVsMom->Fill(mom/10, 100);
	}
	for(int k=0;k<mc-reco;k++) {
	  fEffVsMom->Fill(mom/10, 0);
	}

      }

      if(fVerbose == 3) {
	noRefSlow = noPrimaryRefSlow;
	noRefFast = noPrimaryRefFast;
      }
    }

    //-----------------------------------------

    Double_t qTotPrimaryRefFast = Double_t(RECO_Tot_PrimaryRefFast)/Double_t(MC_Tot_PrimaryRefFast)*100;
    Double_t qTotPrimaryRefSlow = -42.;
    if(MC_Tot_PrimaryRefSlow) qTotPrimaryRefSlow = Double_t(RECO_Tot_PrimaryRefSlow)/Double_t(MC_Tot_PrimaryRefSlow)*100;

    Double_t qTotPrimaryRefAll = Double_t(RECO_Tot_PrimaryRefAll)/Double_t(MC_Tot_PrimaryRefAll)*100;

    Double_t qTotAllRefFast = Double_t(RECO_Tot_AllRefFast)/Double_t(MC_Tot_AllRefFast)*100;

    Double_t qTotAllRefSlow = -42.;
    if(MC_Tot_AllRefSlow) qTotAllRefSlow = Double_t(RECO_Tot_AllRefSlow)/Double_t(MC_Tot_AllRefSlow)*100;

    Double_t qTotAllRefAll = -42.;
    if(MC_Tot_AllRefAll) qTotAllRefAll = Double_t(RECO_Tot_AllRefAll)/Double_t(MC_Tot_AllRefAll)*100;

    Double_t qTotExtraRefFast = -42.;
    if(MC_Tot_ExtraRefFast) qTotExtraRefFast = Double_t(RECO_Tot_ExtraRefFast)/Double_t(MC_Tot_ExtraRefFast)*100;

    Double_t qTotExtraRefSlow = -42.;
    if(MC_Tot_ExtraRefSlow) qTotExtraRefSlow = Double_t(RECO_Tot_ExtraRefSlow)/Double_t(MC_Tot_ExtraRefSlow)*100;

    Double_t qTotExtraRefAll = -42.;
    if(MC_Tot_ExtraRefAll) qTotExtraRefAll = Double_t(RECO_Tot_ExtraRefAll)/Double_t(MC_Tot_ExtraRefAll)*100;

    Double_t qTotClones = -42.;
    if(MC_Tot_AllRefAll) qTotClones = Double_t(NO_TOTAL_CLONE_TRACKS)/Double_t(MC_Tot_AllRefAll)*100;

    Double_t qTotGhosts = -42;
    if(NO_TOTAL_RECO_TRACKS) qTotGhosts = Double_t(NO_TOTAL_GHOST_TRACKS)/Double_t(NO_TOTAL_RECO_TRACKS)*100;

    Double_t qTotClonesPrimFast = -42.;
    if(MC_Tot_PrimaryRefFast) qTotClonesPrimFast = Double_t(NO_TOTAL_CLONES_PRIM_FAST)/Double_t(MC_Tot_PrimaryRefFast)*100;


    Double_t qTotClonesPrimSlow = -42.;
    if(MC_Tot_PrimaryRefSlow) qTotClonesPrimSlow = Double_t(NO_TOTAL_CLONES_PRIM_SLOW)/Double_t(MC_Tot_PrimaryRefSlow)*100;

    Double_t qTotExtraClonesFast = -42.;
    if(MC_Tot_AllRefFast) qTotExtraClonesFast = Double_t(NO_TOTAL_CLONES_FAST)/Double_t(MC_Tot_AllRefFast)*100;

    Double_t qTotExtraClonesSlow = -42.;
    if(MC_Tot_AllRefSlow) qTotExtraClonesSlow = Double_t(NO_TOTAL_CLONES_SLOW)/Double_t(MC_Tot_AllRefSlow)*100;

    Double_t qTotClonesFast = -42.;
    if(MC_Tot_AllRefFast) qTotClonesFast = Double_t(NO_TOTAL_CLONES_FAST+NO_TOTAL_CLONES_PRIM_FAST)/Double_t(MC_Tot_AllRefFast)*100;


    Double_t qTotClonesSlow = -42.;
    if(MC_Tot_AllRefSlow) qTotClonesSlow = Double_t(NO_TOTAL_CLONES_SLOW+NO_TOTAL_CLONES_PRIM_SLOW)/Double_t(MC_Tot_AllRefSlow)*100;


    //-----------------------------------------


    Double_t qTotEvents = Double_t(NO_TOTAL_RECO_TRACKS)/Double_t(NO_TOTAL_EVENTS);

    Int_t verb = 1;

    if(verb > 1)
      {
	cout
	  << " RECO_Tot_PrimaryRefFast = " << RECO_Tot_PrimaryRefFast << endl
	  << "   MC_Tot_PrimaryRefFast = " << MC_Tot_PrimaryRefFast << endl
	  << "      qTotPrimaryRefFast = " << qTotPrimaryRefFast << endl
	  << endl
	  << " RECO_Tot_PrimaryRefSlow = " << RECO_Tot_PrimaryRefSlow << endl
	  << "   MC_Tot_PrimaryRefSlow = " << MC_Tot_PrimaryRefSlow << endl
	  << "      qTotPrimaryRefSlow = " << qTotPrimaryRefSlow << endl

	  << endl;
      }


    cout << endl;
    cout << "#######################################################"
	 << endl;
    //   cout << "-I-            TRD Track Matching Totals            -I-"
    //        << endl
    //        << "               Summary of event " << NO_TOTAL_EVENTS
    //        << endl;
    //   cout << "        Reconstructed TrdTracks   : " << qTotEvents << " /event" << endl;
    //   cout << "  Number of correct reco tracks   : " << qTotCommonMatches << " /event" << endl;
    //   cout << "  Number of MC reference tracks   : " << qTotMCTrackPrim << " /event" << endl;
    //   cout << "  Efficiency for ref. primaries   : " << qTotEff <<"% " << noTotCommonMatches << endl;
    //   cout << "                         Ghosts   : " << qTotGhosts << "% " << noTotGhost << endl;
    //   cout << "                         Clones   : " << qTotClones << "% " << noTotClones << endl;
    //   cout << "#######################################################"
    //        << endl;

    //    cout.width(10);

    Int_t mW = 7;
    cout.precision(1);


    cout << fixed << "Accumulated " << NO_TOTAL_EVENTS << " events: " << endl;

    cout << "        Reconstructed TrdTracks : " << std::setw(mW) << Int_t(qTotEvents) << " " << endl
	 << "  Eff. of primary reference all : " << std::setw(mW) <<  qTotPrimaryRefAll << "% " << std::setw(mW) << RECO_Tot_PrimaryRefAll << endl
	 << " Eff. of primary reference fast : " << std::setw(mW) <<  qTotPrimaryRefFast <<"% " << std::setw(mW) << RECO_Tot_PrimaryRefFast << " " << endl
	 << " Eff. of primary reference slow : " << std::setw(mW) <<  qTotPrimaryRefSlow << "% " << std::setw(mW) << RECO_Tot_PrimaryRefSlow << endl
	 << "-------------------------------------------------------" << endl

	 << "    Eff. of extra reference all : " << std::setw(mW) <<  qTotExtraRefAll << "% " << std::setw(mW) << RECO_Tot_ExtraRefAll << endl
	 << "   Eff. of extra reference fast : " << std::setw(mW) <<  qTotExtraRefFast << "% " << std::setw(mW) << RECO_Tot_ExtraRefFast << endl
	 << "   Eff. of extra reference slow : " << std::setw(mW) <<  qTotExtraRefSlow << "% " << std::setw(mW) << RECO_Tot_ExtraRefSlow << endl
	 << "-------------------------------------------------------" << endl

	 << "      Eff. of all reference all : " << std::setw(mW) <<  qTotAllRefAll << "% " << std::setw(mW) << RECO_Tot_AllRefAll << endl
	 << "     Eff. of all reference fast : " << std::setw(mW) <<  qTotAllRefFast << "% " << std::setw(mW) << RECO_Tot_AllRefFast << endl
	 << "     Eff. of all reference slow : " << std::setw(mW) <<  qTotAllRefSlow << "% " << std::setw(mW) << RECO_Tot_AllRefSlow << endl
	 << "-------------------------------------------------------" << endl

	 << "                    Ghost level : " << std::setw(mW) <<  qTotGhosts << "% " <<  std::setw(mW) << NO_TOTAL_GHOST_TRACKS << endl
	 << "                    Clone level : " << std::setw(mW) <<  qTotClones << "% " <<   std::setw(mW) << NO_TOTAL_CLONE_TRACKS << endl
	 << "-------------------------------------------------------" << endl

	 << "       Primary fast clone level : " << std::setw(mW) <<  qTotClonesPrimFast << "% " <<  std::setw(mW) << NO_TOTAL_CLONES_PRIM_FAST << endl
	 << "       Primary slow clone level : " << std::setw(mW) <<  qTotClonesPrimSlow << "% " <<  std::setw(mW) << NO_TOTAL_CLONES_PRIM_SLOW << endl
	 << "-------------------------------------------------------" << endl

	 << "         Extra fast clone level : " << std::setw(mW) <<  qTotExtraClonesFast << "% " <<  std::setw(mW) << NO_TOTAL_CLONES_FAST << endl
	 << "         Extra slow clone level : " << std::setw(mW) <<  qTotExtraClonesSlow << "% " <<  std::setw(mW) << NO_TOTAL_CLONES_SLOW << endl
	 << "-------------------------------------------------------" << endl

	 << "           All fast clone level : " << std::setw(mW) <<  qTotClonesFast << "% " <<  std::setw(mW) << NO_TOTAL_CLONES_PRIM_FAST+NO_TOTAL_CLONES_FAST << endl
	 << "           All slow clone level : " << std::setw(mW) <<  qTotClonesSlow << "% " <<  std::setw(mW) << NO_TOTAL_CLONES_PRIM_SLOW+NO_TOTAL_CLONES_SLOW << endl

	 << "######################################################"
	 <<endl;

    //     cout << "        Reconstructed TrdTracks : " << qTotEvents << " /event" << endl
    // 	 << " Eff. of primary reference fast : " << qTotPrimaryRefFast <<"% " << double(RECO_Tot_PrimaryRefFast)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << " Eff. of primary reference slow : " << qTotPrimaryRefSlow << "% " << double(RECO_Tot_PrimaryRefSlow)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "  Eff. of primary reference all : " << qTotPrimaryRefAll << "% " << double(RECO_Tot_PrimaryRefAll)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "-------------------------------------------------------" << endl

    // 	 << "   Eff. of extra reference fast : " << qTotExtraRefFast << "% " << double(RECO_Tot_ExtraRefFast)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "   Eff. of extra reference slow : " << qTotExtraRefSlow << "% " << double(RECO_Tot_ExtraRefSlow)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "    Eff. of extra reference all : " << qTotExtraRefAll << "% " << double(RECO_Tot_ExtraRefAll)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "-------------------------------------------------------" << endl

    // 	 << "         Eff. of reference fast : " << qTotAllRefFast << "% " << double(RECO_Tot_AllRefFast)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "         Eff. of reference slow : " << qTotAllRefSlow << "% " << double(RECO_Tot_AllRefSlow)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "          Eff. of reference all : " << qTotAllRefAll << "% " << double(RECO_Tot_AllRefAll)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "-------------------------------------------------------" << endl

    // 	 << "                    Ghost level : " << qTotGhosts << "% " << double(NO_TOTAL_GHOST_TRACKS)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "                    Clone level : " << qTotClones << "% " <<  double(NO_TOTAL_CLONE_TRACKS)/double(NO_TOTAL_EVENTS) << " /event" << endl
    // 	 << "-------------------------------------------------------" << endl

    // 	 << "       Primary fast clone level : " << qTotClonesPrimFast << "% " << Double_t(NO_TOTAL_CLONES_PRIM_FAST)/Double_t(NO_TOTAL_EVENTS) << endl
    // 	 << "       Primary slow clone level : " << qTotClonesPrimSlow << "% " << Double_t(NO_TOTAL_CLONES_PRIM_SLOW)/Double_t(NO_TOTAL_EVENTS) << endl
    // 	 << "-------------------------------------------------------" << endl

    // 	 << "         Extra fast clone level : " << qTotExtraClonesFast << "% " << Double_t(NO_TOTAL_CLONES_FAST)/Double_t(NO_TOTAL_EVENTS) << endl
    // 	 << "         Extra fast clone level : " << qTotExtraClonesSlow << "% " << Double_t(NO_TOTAL_CLONES_SLOW)/Double_t(NO_TOTAL_EVENTS) << endl
    // 	 << "-------------------------------------------------------" << endl

    // 	 << "           All fast clone level : " << qTotClonesFast << "% " << Double_t(NO_TOTAL_CLONES_PRIM_FAST+NO_TOTAL_CLONES_FAST)/Double_t(NO_TOTAL_EVENTS) << endl
    // 	 << "           All slow clone level : " << qTotClonesSlow << "% " << Double_t(NO_TOTAL_CLONES_PRIM_SLOW+NO_TOTAL_CLONES_SLOW)/Double_t(NO_TOTAL_EVENTS) << endl

    // 	 << "######################################################"
    // 	 <<endl;

    //   else cout << "-I- CbmTrdMatchTracksMK: rec. " << nTracks << ", quota "
    // 	    << qTrue << " % " << endl;

    //   cout << "----------------------------------------------------------" << endl
    //        << "-I- CbmTrdMatchTracksMK: " << endl
    //        << "       noHighMomTracks= " << noHighMomTracks << endl
    //        << "        noLowMomTracks= " << noLowMomTracks << endl
    //        << "          noMCTruePrim= " << noMCTruePrim << endl
    //        << "        noDoubleTracks= " << noDoubleTracks << endl
    //        << "         noGhostTracks= " << noGhostTracks << endl
    //        << "----------------------------------------------------------"
    //        << endl;
    //    WriteHistogramms();


    if(dispTime) {
      Double_t rtime;
      Double_t ctime;
      //      Double_t qtime = Double_t(fNofEvents);

      cout << "        cArray : ";
      rtime = cArray.RealTime();
      ctime = cArray.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);

      cout << "        cMCRef : ";
      rtime = cMCRef.RealTime();
      ctime = cMCRef.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);

      cout << " cHighMomPrims : ";
      rtime = cHighMomPrims.RealTime();
      ctime = cHighMomPrims.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);

      cout << "  cLowMomPrims : ";
      rtime = cLowMomPrims.RealTime();
      ctime = cLowMomPrims.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);

      cout << "     cMainLoop : ";
      rtime = cMainLoop.RealTime();
      ctime = cMainLoop.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);

      cout << "     cIndexing : ";
      rtime = cIndexing.RealTime();
      ctime = cIndexing.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);

      cout << "   cCloneCount : ";
      rtime = cCloneCount.RealTime();
      ctime = cCloneCount.CpuTime();
      printf("RealTime=%4.2f s, CpuTime=%4.2f s\n",rtime,ctime);
      cout << endl;
    }



    MC_nOfPrimaryRefFastTracks.clear();
    mcToReco.clear();
    recoToMc.clear();
    recoToMcGhost.clear();
    usedMCTracks.clear();
    usedRecoTracks.clear();
    MC_highMomPrims.clear();
  }
}

// -------------------------------------------------------------------------

void CbmTrdMatchTracksMK::CreateHistogramms()
{
  //Define some histogramms

  //fEffVsMom = new TProfile("EffVsMom","EffVsMom",100,0,100);
  //  fEffVsMom = new TH2F("EffVsMom","EffVsMom",100,0,100,100,0,100);
  fEffVsMom = new TProfile("EffVsMom","EffVsMom",100, 0.0, 10, 0.0, 100.0);
  fh_EffVsMom = new TH1F("h_EffVsMom","h_EffVsMom",100, 0,10);
  fh_mom_trdtrack_good = new TH1F("h_mom_trdtrack_good","h_mom_trdtrack_good",100, 0,10);
  fh_mom_mctrack_reco  = new TH1F("h_mom_mctrack_reco","h_mom_mctrack_reco",100, 0,10);


  //  cout << "\n!!!!Creating histogramms!!!!\n";
  fNoHitsPerTrack = new TH1F("noHitsPerTrack","noHitsPerTrack",12,1,13);
  fNoTrueHits = new TH1F("noTrueHits","noTrueHits",12,1,13);
  fNoTracks = new TH1F("noTracks","noTracks",100,0,5000);
  fNoPrimHighMom = new TH1F("NoPrimHighMom","fNoPrimHighMom",100,0,100);
  fNoPrimLowMom = new TH1F("NoPrimLowMom","fNoPrimLowMom",100,0,100);
  fPartTy = new TH1F("PartTy","PartTy",100,0,1);
  fGhostTracks_meanZ = new TH1F("GhostTracks_meanZ","GhostTracks_meanZ",100,300,900);



  //
  fXDistShort14 = new TH1F("XDistShort14","XDistShort14",100,0,6);
  fYDistShort14 = new TH1F("YDistShort14","YDistShort14",100,0,2);

  fXDistShortGh14 = new TH1F("XDistShortGh14","XDistShortGh14",100,0,6);
  fYDistShortGh14 = new TH1F("YDistShortGh14","YDistShortGh14",100,0,2);


  //

  fXDistLong45 = new TH1F("XDistLong45","XDistLong45",100,0,20);
  fYDistLong45 = new TH1F("YDistLong45","YDistLong45",100,0,35);


  fXDistLongGh45 = new TH1F("XDistLongGh45","XDistLongGh45",100,0,20);
  fYDistLongGh45 = new TH1F("YDistLongGh45","YDistLongGh45",100,0,35);

  //
  fXDistShort58 = new TH1F("XDistShort58","XDistShort58",100,0,4);
  fYDistShort58 = new TH1F("YDistShort58","YDistShort58",100,0,1);


  fXDistShortGh58 = new TH1F("XDistShortGh58","XDistShortGh58",100,0,4);
  fYDistShortGh58 = new TH1F("YDistShortGh58","YDistShortGh58",100,0,1);


  //

  fXDistLong89 = new TH1F("XDistLong89","XDistLong89",100,0,40);
  fYDistLong89 = new TH1F("YDistLong89","YDistLong89",100,0,12.5);


  fXDistLongGh89 = new TH1F("XDistLongGh89","XDistLongGh89",100,0,40);
  fYDistLongGh89 = new TH1F("YDistLongGh89","YDistLongGh89",100,0,12.4);

  //
  fXDistShort912 = new TH1F("XDistShort912","XDistShort912",100,0,4);
  fYDistShort912 = new TH1F("YDistShort912","YDistShort912",100,0,1);


  fXDistShortGh912 = new TH1F("XDistShortGh912","XDistShortGh912",100,0,4);
  fYDistShortGh912 = new TH1F("YDistShortGh912","YDistShortGh912",100,0,1);

  //
  fh_Chi2_Ghost       = new TH1F("fh_Chi2_Ghost",      "fh_Chi2_Ghost",      1000,0,10);
  fh_Chi2_CorrectFast = new TH1F("fh_Chi2_CorrectFast","fh_Chi2_CorrectFast",1000,0,10);
  fh_Chi2_CorrectSlow = new TH1F("fh_Chi2_CorrectSlow","fh_Chi2_CorrectSlow",1000,0,10);

  fh_Tx_Ghost       = new TH1F("fh_Tx_Ghost",      "fh_Tx_Ghost",      1000,0,1);
  fh_Tx_CorrectFast = new TH1F("fh_Tx_CorrectFast","fh_Tx_CorrectFast",1000,0,1);
  fh_Tx_CorrectSlow = new TH1F("fh_Tx_CorrectSlow","fh_Tx_CorrectSlow",1000,0,1);

  fh_Ty_Ghost       = new TH1F("fh_Ty_Ghost",      "fh_Ty_Ghost",      1000,0,1);
  fh_Ty_CorrectFast = new TH1F("fh_Ty_CorrectFast","fh_Ty_CorrectFast",1000,0,1);
  fh_Ty_CorrectSlow = new TH1F("fh_Ty_CorrectSlow","fh_Ty_CorrectSlow",1000,0,1);

  fh_Y_Ghost       = new TH1F("fh_Y_Ghost",      "fh_Y_Ghost",      400,0,400);
  fh_Y_CorrectFast = new TH1F("fh_Y_CorrectFast","fh_Y_CorrectFast",400,0,400);
  fh_Y_CorrectSlow = new TH1F("fh_Y_CorrectSlow","fh_Y_CorrectSlow",400,0,400);

  fh_X_Ghost       = new TH1F("fh_X_Ghost",      "fh_X_Ghost",      400,0,400);
  fh_X_CorrectFast = new TH1F("fh_X_CorrectFast","fh_X_CorrectFast",400,0,400);
  fh_X_CorrectSlow = new TH1F("fh_X_CorrectSlow","fh_X_CorrectSlow",400,0,400);

}

void CbmTrdMatchTracksMK::WriteHistogramms()
{
  //Define some histogramms

  fEffVsMom->Write();

  fh_mom_trdtrack_good->Write();
  fh_mom_mctrack_reco->Write();

  fh_EffVsMom->Add(fh_mom_trdtrack_good);
  fh_EffVsMom->Sumw2();
  fh_EffVsMom->Divide(fh_mom_mctrack_reco);
  fh_EffVsMom->Write();

  fNoHitsPerTrack->Write();
  fNoTrueHits->Write();
  fNoTracks->Write();
  fNoPrimHighMom->Write();
  fNoPrimLowMom->Write();
  fPartTy->Write();
  fGhostTracks_meanZ->Write();
  //
  fXDistShort14->Write();
  fYDistShort14->Write();


  fXDistShortGh14->Write();
  fYDistShortGh14->Write();


  //

  fXDistLong45->Write();
  fYDistLong45->Write();


  fXDistLongGh45->Write();
  fYDistLongGh45->Write();

  //
  fXDistShort58->Write();
  fYDistShort58->Write();


  fXDistShortGh58->Write();
  fYDistShortGh58->Write();


  //

  fXDistLong89->Write();
  fYDistLong89->Write();


  fXDistLongGh89->Write();
  fYDistLongGh89->Write();

  //
  fXDistShort912->Write();
  fYDistShort912->Write();


  fXDistShortGh912->Write();
  fYDistShortGh912->Write();

  //
  fh_Chi2_Ghost->Write();
  fh_Chi2_CorrectFast->Write();
  fh_Chi2_CorrectSlow->Write();

  //
  fh_Tx_Ghost->Write();
  fh_Tx_CorrectFast->Write();
  fh_Tx_CorrectSlow->Write();

  fh_Ty_Ghost->Write();
  fh_Ty_CorrectFast->Write();
  fh_Ty_CorrectSlow->Write();

  fh_Y_Ghost->Write();
  fh_Y_CorrectFast->Write();
  fh_Y_CorrectSlow->Write();

  fh_X_Ghost->Write();
  fh_X_CorrectFast->Write();
  fh_X_CorrectSlow->Write();
}

Int_t CbmTrdMatchTracksMK::GetMCIndex(CbmTrdTrack* trdTrack)
{
  Int_t
    nTrackEnries = trdTrack->GetNofHits(),
    trdPtInd = -1,
    mcIndex = -1,
    trueInd = -1,
    trueVal = -1,
    tempVal;

  CbmTrdHit* trdHit;
  FairMCPoint* mcPoint;

  map<Int_t, Int_t> indeksy;
  map<Int_t, Int_t>::iterator it;

  for(int i=0;i<nTrackEnries;i++)
    {
      trdHit = (CbmTrdHit*)fHits->At(trdTrack->GetHitIndex(i));
      trdPtInd = trdHit->GetRefId();
      mcPoint = (FairMCPoint*)fPoints->At(trdPtInd);
      mcIndex = mcPoint->GetTrackID();
      indeksy[mcIndex]+=1;
    }

  for(it=indeksy.begin();it != indeksy.end(); it++)
    {
      tempVal = it->second;
      if(tempVal > trueVal) {
	trueVal = tempVal;
	trueInd = it->first;
      }
    }
  indeksy.clear();
  //cout << trueVal << " hits from a track " << trueInd << endl;
  return trueInd;
}

Double_t CbmTrdMatchTracksMK::DistTracklet(TClonesArray* fHit,
					   Int_t iIndexFirst,
					   Int_t iIndexSecond)
{
  CbmTrdPoint
    *pHitFirst,
    *pHitSecond;
  Double_t dist;

  Double_t firstPosY = 0;
  Double_t firstPosZ = 0;


  // Get hits from first and second plane in station
  if(iIndexFirst != -1) {
    pHitFirst  = (CbmTrdPoint*) fHit->At(iIndexFirst);
    firstPosY  = fabs(pHitFirst->GetY());
    firstPosZ  = pHitFirst->GetZ();
  }
  pHitSecond = (CbmTrdPoint*) fHit->At(iIndexSecond);

  // Get position Y & Z of hits

  Double_t secondPosY = fabs(pHitSecond->GetY());
  Double_t secondPosZ = pHitSecond->GetZ();

  Double_t Y = (firstPosZ*secondPosY)/secondPosZ;
  dist = fabs(Y-firstPosY);

  //   cout << "From the coords (y,z) = ("
  //        << firstPosY << ", " << firstPosZ << endl
  //        << "From the coords (y,z) = ("
  //        << secondPosY << ", " << secondPosZ << endl
  //        << "     we get the value = "
  //        << dist
  //        << endl;


  return dist;
}

Double_t CbmTrdMatchTracksMK::DistTrackletCutX(TClonesArray* trdMCPoint,
					       Int_t iIndexFirst,
					       Int_t iIndexSecond)
{
  CbmTrdPoint
    *mcPointFirst,
    *mcPointSecond;
  Double_t dist;

  // Get hits from first and second plane in station
  mcPointFirst  = (CbmTrdPoint*) trdMCPoint->At(iIndexFirst);
  mcPointSecond = (CbmTrdPoint*) trdMCPoint->At(iIndexSecond);

  // Get position Y & Z of hits
  Double_t firstPosX  = fabs(mcPointFirst->GetX());
  Double_t firstPosZ  = mcPointFirst->GetZ();

  Double_t secondPosX = fabs(mcPointSecond->GetX());
  Double_t secondPosZ = mcPointSecond->GetZ();

  Double_t X = (firstPosZ*secondPosX)/secondPosZ;
  dist = fabs(X-firstPosX);

  return dist;
}

// -----   Public method Finish   ------------------------------------------
void CbmTrdMatchTracksMK::Finish() { }
// -------------------------------------------------------------------------



ClassImp(CbmTrdMatchTracksMK)








