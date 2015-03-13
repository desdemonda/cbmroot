// -----------------------------------------------------------------------
// -----                    CbmL1CATrdTrackFinderSA                  -----
// -----            Created 2/12/2006  by A. Bubak & M. Krauze       -----
// -----------------------------------------------------------------------

#include "CbmL1CATrdTrackFinderSA.h"

#include "CbmL1TrdTracklet.h"
#include "CbmL1TrdTracklet4.h"

#include "CbmKFTrdHit.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairBaseParSet.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmMCTrack.h"
#include "CbmGeoTrdPar.h"
#include "FairGeoNode.h"
#include "CbmVertex.h"
#include "CbmTrdTrackFitterKF.h"
#include "CbmKFTrack.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TLinearFitter.h"

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::map;
using std::set;
using std::fabs;

ClassImp(CbmL1CATrdTrackFinderSA);


CbmL1CATrdTrackFinderSA *CbmL1CATrdTrackFinderSA::fInstance = 0;

// ----------------------- Default constructor ---------------------------
CbmL1CATrdTrackFinderSA::CbmL1CATrdTrackFinderSA()
  : CbmTrdTrackFinder(),
    geoLayer(),
    fNTrdHits(-1),
    fNoTrdStations(-1),
    fNoTrdPerStation(-1),
    planeHits(),
    fvTrdHitArr(),
    fvTrdPointArr(),
    fivTrdHitArr(),
    fArrayTrdHit(new TClonesArray("CbmTrdHit")),
    fArrayTrdTrack(new TClonesArray("CbmTrdTrack")),
    TrdPar(NULL),
    fEvents(0),
    fNofEvents(0),
    fMCTrackArray(NULL),
    fMCPointArray(NULL),
    trdTrackFitterKF(new CbmTrdTrackFitterKF(1,11)),
    fVerbose(1),
    fNoTrdHits(),
    fTrdHitIndex(),
    fvTempArray(),
    fvFoundTracks(),
    tempTrack(),
    iStation1(),
    iStation2(),
    fImapSt1(),
    fImapSt2(),
    itTrackletsLeft(),
    itTrackletsRight(),
    iHitMap1(0),
    iHitMap2(0),
    iHitMapY1(0.),
    iHitMapY2(0.),
    fTrd13_Z(0.),
    fTrd14_Z(0.),
    fTrd21_Z(0.),
    fTrd24_Z(0.),
    fTrd31_Z(0.),
    fKfTrack(NULL),
    createSegments(),
    findNeighbour(),
    createSPs(),
    createSPs_SL(),
    sortSPs(),
    doFind(),
    sortHits(),
    tagTracks(),
    createTracks(),
    selectTracks(),
    delTime(),
    secondLoopTime(),
    refittingKF(),
    thirdLoopTime(),
    totCreateSegments(0.),
    totFindNeighbour(0.),
    totCreateSPs(0.),
    totCreateSPs_SL(0.),
    totSortSPs(0.),
    totDoFind(0.),
    totSortHits(0.),
    totTagTracks(0.),
    totCreateTracks(0.),
    totDelTime(0.),
    totSecondLoopTime(0.),
    totThirdLoopTime(0.),
    totSelectTracks(0.),
    totRefittingKF(0.),
    fh_chi2hit(NULL),
    fh_chi2hit_plane(NULL),
    fDistLongX(NULL),
    fDistLongY(NULL),
    fDistShortX(NULL),
    fDistShortY(NULL),
    fDistLongBX(NULL),
    fDistLongBY(NULL),
    fDistShortBX(NULL),
    fDistShortBY(NULL),
    fDistY12(NULL),
    fMomDistLongPrimaryX(NULL),
    fMomDistLongPrimaryY(NULL),
    fMomDistLongExtraX(NULL),
    fMomDistLongExtraY(NULL),
    fMomDistExtrapolPrimaryX(NULL),
    fMomDistExtrapolPrimaryY(NULL),
    fMomDistExtrapolExtraX(NULL),
    fMomDistExtrapolExtraY(NULL),
    fMomDistShortPrimaryX(NULL),
    fMomDistShortPrimaryY(NULL),
    fMomDistShortExtraX(NULL),
    fMomDistShortExtraY(NULL),
    fDistY(NULL),
    fDistX(NULL),
    fPlane1Ydens(NULL),
    fPlane5Ydens(NULL),
    fPlane9Ydens(NULL),
    fSPlength(NULL),
    fSPlengthMC(NULL),
    fYat0(NULL),
    fYat0MC(NULL),
    fNoEvTime(NULL),
    fh_SP_xDiff_MC(NULL),
    fh_SP_yDiff_MC(NULL),
    fh_SP_xDiff_nMC(NULL),
    fh_SP_yDiff_nMC(NULL),
    fUsedHitsPerPlane(NULL),
    fUnUsedHitsPerPlane(NULL),
    fRUsedHits(),
    fRUnUsedHits(),
    fTotHits()
{
  if( !fInstance ) fInstance = this;
}
// -----------------------------------------------------------------------

// --------------------------- Destructor --------------------------------
CbmL1CATrdTrackFinderSA::~CbmL1CATrdTrackFinderSA()
{
  delete fArrayTrdHit;
  delete fArrayTrdTrack;
  delete trdTrackFitterKF;
  //  delete fKfTrack;
  if( fInstance==this ) fInstance = 0;
}
// -----------------------------------------------------------------------



// ------------------------- Initialisation ------------------------------
void CbmL1CATrdTrackFinderSA::Init()
{
  // Create histogramms
  CreateHistogramms();
  // Activate data branches
  DataBranches();

  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmL1CATrdTrackFinderSA::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) {
    cout << "-E- CbmL1CATrdTrackFinderSA::Init: No MCTrack array!"
	 << endl;
    return;
  }

  // Get MCPoint array
  fMCPointArray  = (TClonesArray*) ioman->GetObject("TrdPoint");
  if ( ! fMCPointArray) {
    cout << "-E- CbmL1CATrdTrackFinderSA::Init: No MCPoint array!"
	 << endl;
    return;
  }

  // Determine the TRD layout
  TrdLayout();

  // Init KF Track fitter
  trdTrackFitterKF->Init();
}
// -----------------------------------------------------------------------



// -------------------- Algorithm implementation -------------------------
Int_t CbmL1CATrdTrackFinderSA::DoFind(TClonesArray *hitArray,
				      TClonesArray *trackArray)
{

#ifdef DRAW
  InitL1Draw();
#endif

  fArrayTrdHit = hitArray;
  fArrayTrdTrack = trackArray;

  fNofEvents++;

  doFind.Start();

  // Initialize control counters
  //  Int_t nNoTrdPoint  = 0;
  Int_t nNoTrdHit  = 0;
  Int_t trdPlaneID = 0;
  Int_t ptIndex    = 0;
  Int_t noHitsPerTracklet = 4;

  // Create pointers to TRD hit and TrdPoint
  CbmTrdHit*   pHit  = NULL;
  CbmTrdPoint* pMCpt = NULL;
  CbmMCTrack*  pMCtr = NULL;

  //--- Function: CreateSpacePoint --------------------------------------
  Double_t           //    b
    sigmaA_FL = 2, //3 3 3 2.5 2.5
    sigmaB_FL = 2, //3 3 3 2.5 2.5
    sigmaA_SL = 3,   //4 4 4 3   3
    sigmaB_SL = 3,   //4 4 4 3   3
    sigmaA_TL = 4,   //4 4 4 4   4
    sigmaB_TL = 4;   //4 4 4 4   4
  //--- Function: CreateSegments; Data from MC, look inside function ----
  Double_t
    dY_FL = 0.3, //0.5 0.5 0.5 0.4 0.4
    dX_FL = 0.3, //0.5 0.5 0.5 0.4 0.4
    dY_SL = 0.5, //0.7 0.6 0.8 0.6 0.6
    dX_SL = 0.5, //0.8 0.8 0.8 0.6 0.6
    dY_TL = 0.7, //0.7 0.6 0.8 0.7 0.8
    dX_TL = 0.7; //0.8 0.8 0.8 0.7 0.8

  //--- Function: FindNeighbour ------------------------------------------
  //distance from Y-propagated point, around which we look for neighbours
  Double_t
    distPropLongY_FL = 2.5, //3 3 3 3 2
    distPropLongX_FL = 2.5, //3 3 3 3 2
    distPropLongY_SL = 3,   //4 4 4 3 3
    distPropLongX_SL = 3,   //8 6 8 3 3
    distPropLongY_TL = 4,   //4 4 4 3 4
    distPropLongX_TL = 4;   //8 6 8 3 4

  //----------------------------------------------------------------------
  Bool_t competition    = true;
  Bool_t removeUsedHits = true;
  Bool_t secondLoop     = true;
  Bool_t thirdLoop      = true;
  Int_t  segment        = 0; //0 = long tracks, 1 = segments only
  //Int_t  nrLoop         = 1;

  //---- Simulate detector inefficiency -----------------------------------
  Int_t accept = 100;
  set<Int_t> globalSetRejectedHits;
  globalSetRejectedHits.clear();
  //----------------------------------------------------------------------


  sortHits.Start();
  //--- hit sorting procedure ------------------------------------------------
  Int_t nTrdHit = hitArray->GetEntriesFast();
  for (Int_t iHit = 0; iHit<nTrdHit; iHit++) {
    // Loop over hits. Get corresponding MCPoint and MCTrack index
    pHit = (CbmTrdHit*) hitArray->At(iHit);

    if(!Rejection(accept)) {
      // Simulate detector inefficiency
      globalSetRejectedHits.insert(iHit);
      continue;
    }
    if(!pHit) {
      cout << "-E- CbmL1CATrdTrackFinderSA::DoFind: Empty slot "
	   << "in HitArray at position " << iHit << endl;
      nNoTrdHit++;
      continue;
    }

    ptIndex = pHit->GetRefId();
    //if (ptIndex < 0) continue;           // fake or background hit
    pMCpt = (CbmTrdPoint*) (fMCPointArray->At(ptIndex));
    //if (!pMCpt) {
    //  nNoTrdPoint++;
    //  continue;
    //}
    pMCtr = (CbmMCTrack*) fMCTrackArray->At(pMCpt->GetTrackID());
    //if ( ! pMCtr ) continue;

    trdPlaneID = pHit->GetPlaneId();
    Int_t trdPlaneIDN = trdPlaneID-1;

    planeHits.mcTrackID = pMCpt->GetTrackID();
    planeHits.hitIndex = iHit;
    planeHits.X  = pHit->GetX();
    planeHits.Y  = pHit->GetY();
    planeHits.Z  = pHit->GetZ();
    planeHits.DX = pHit->GetDx();
    planeHits.DY = pHit->GetDy();
    planeHits.planeID = trdPlaneID;
    fvTrdHitArr[trdPlaneIDN].push_back(planeHits);

    //    cout << "geoLayer: " <<geoLayer.Y[trdPlaneID] <<", "<< geoLayer.Z[trdPlaneID] << endl;
    //ptIndex = pHit->GetRefIndex();
    //pMCpt = (CbmTrdPoint*) (fMCPointArray->At(ptIndex));

    //planeHits.hitIndex = ptIndex;
    //planeHits.X  = pMCpt->GetX();
    //planeHits.Y  = pMCpt->GetY();
    //planeHits.Z  = pMCpt->GetZ();
    //planeHits.DX = 0;
    //planeHits.DY = 0;;
    //fvTrdPointArr[trdPlaneIDN].push_back(planeHits);
  }

  for(Int_t i = 0; i < 12; i++){
    sort(fvTrdHitArr[i].begin(),  fvTrdHitArr[i].end(),  CompareY);
  }

  /*
  //-----------------------------------------------------------
  vector <LayerWithHits>::iterator ivT;
  LayerWithHits pl;
  for(ivT = fvTrdHitArr[0].begin();
  ivT != fvTrdHitArr[0].end();
  ivT++){
  pl = (*ivT);
  cout <<"(X:Y) "<<pl.X<<"\t"<< pl.Y << endl;
  fPlane1Ydens->Fill(pl.Y);
  }

  for(ivT = fvTrdHitArr[5].begin();
  ivT != fvTrdHitArr[5].end();
  ivT++){
  pl = (*ivT);
  //    cout << pl.Y << endl;
  fPlane5Ydens->Fill(pl.Y);
  }
  for(ivT = fvTrdHitArr[9].begin();
  ivT != fvTrdHitArr[9].end();
  ivT++){
  pl = (*ivT);
  //    cout << pl.Y << endl;
  fPlane9Ydens->Fill(pl.Y);
  }
  //-----------------------------------------------------------
  */
  sortHits.Stop();

  if(fVerbose > 2) {
    cout <<"-I- CbmL1CATrdTrackFinderSA::DoFind " << endl;
    for(Int_t i = 0; i < 12; i++){
      cout <<" Size of vector "<< i <<": "<< fvTrdHitArr[i].size() << endl;
    }
  }

  //create vectors that will hold the tracklet (4-hit) objects
  vector<CbmL1TrdTracklet4*> clTracklets[3];
  vector<CbmL1TrdTracklet4*> clTrackletsNew[3];

  //create vectors that will hold the mini-tracklet (2-hit) objects - Space Points
  vector<CbmL1TrdTracklet*> clSpacePoints[6];

  if(noHitsPerTracklet == 4) { //noHitsPerTracklet == 4
    //------ creation of mini tracklet 1-2 -------------------------------------
    cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
	 << "--------------------------------------------------" << endl
	 << "     ### Start creation of Space Points" <<endl
	 << "--------------------------------------------------" << endl;

    createSPs.Start();
    for(Int_t i = 0, j = 0; i < 6; i++, j=j+2) {
      CreateSpacePoints(fvTrdHitArr[j], fvTrdHitArr[j+1],
			clSpacePoints[i], sigmaA_FL, sigmaB_FL);
    }
    createSPs.Stop();

    //------- Sorting spacepoints -------------------------------------------------
    sortSPs.Start();
    for(Int_t i = 0; i < 6; i++){
      sort(clSpacePoints[i].begin(), clSpacePoints[i].end(), CbmL1TrdTracklet::Compare1);
    }
    sortSPs.Stop();

    // --------------- Creation tracklet from 4 space points ----------------
    cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
	 << "--------------------------------------------------" << endl
	 << "     ### Start creation tracklets" <<endl
	 << "--------------------------------------------------" << endl;
    createSegments.Start();
    //joining SP together to create 4-hit tracklets
    for(Int_t i = 0, j = 0; i < 3; i++, j=j+2) {
      CreateSegments(clSpacePoints[j], clSpacePoints[j+1],
		     clTracklets[i], dX_FL, dY_FL);
    }
    createSegments.Stop();

    //--- TESTING -------------------------------
    /* How many segments have 4 hits belonging to the same track (has the same trackID)*/
    /*
      vector<CbmL1TrdTracklet4*>::iterator itclTrackletsRight;
      vector<CbmL1TrdTracklet4*>::iterator iTrackletT;
      Int_t iSumSegment = 0;
      CbmTrdHit *trdHit;
      CbmTrdPoint* trdPoint;

      for(int i = 0; i < 3; i++){
      Double_t clSegmentSize = clTracklets[i].size();
      cout <<"Tracklet size: "<< clSegmentSize << endl;
      iSumSegment = 0;

      for(iTrackletT  = clTracklets[i].begin();
      iTrackletT != clTracklets[i].end();
      iTrackletT++){

      Int_t k = 0;
      Int_t Ind0 = (*iTrackletT)->GetInd(0);
      trdHit = (CbmTrdHit*) hitArray->At(Ind0);
      trdPoint = (CbmTrdPoint*) fMCPointArray->At(trdHit->GetRefIndex());
      Double_t trackID1 = trdPoint->GetTrackID();

      for(int iw = 1; iw < 4; iw++){
      Int_t Ind1 = (*iTrackletT)->GetInd(iw);
      trdHit = (CbmTrdHit*) hitArray->At(Ind1);
      trdPoint = (CbmTrdPoint*) fMCPointArray->At(trdHit->GetRefIndex());
      Double_t trackID2 = trdPoint->GetTrackID();

      if(trackID1 == trackID2) k++;
      }
      if(k == 3){
      iSumSegment++;
      }
      }

      Double_t sumSegmentWith4 = (iSumSegment*100)/clSegmentSize;
      //printf("(size %f, %i, %f)\n", clSegmentSize, iSumSegment, sumSegmentWith4);
      }
      //---------------------------------------------
      */

    //---------- BEGIN: Find friend ------------------------------------------------
    findNeighbour.Start();

    cout << "--- Finding neighbour 14-58" << endl;
    FindNeighbour(clTracklets[0],clTracklets[1],
		  distPropLongX_FL, distPropLongY_FL);

    cout << "--- Finding neighbour 58-912" << endl;
    FindNeighbour(clTracklets[1],clTracklets[2],
		  distPropLongX_FL, distPropLongY_FL);

    findNeighbour.Stop();

  }//end of noHitsPerTracklet == 4

  //###################################################################################################


  cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
       << "--------------------------------------------------" << endl
       << "     ### (FL) in Event No. " << fNofEvents << " ###" <<endl
       << "--------------------------------------------------" << endl;

  if(fVerbose > 2){
    cout    << "size of segment vector 14 = "
	    << clTracklets[0].size()
	    << endl
	    << "size of segment vector 58 = "
	    << clTracklets[1].size()
	    << endl
	    << "size of segment vector 912 = "
	    << clTracklets[2].size()
	    << endl;
  }

  if(fVerbose > 1)
    cout << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
	 << "Tracklet finding phase completed." << endl
	 << "Now constructing tracks from tracklets..."
	 << endl;


  tagTracks.Start();

  if(noHitsPerTracklet == 4) {//noHitsPerTracklet == 4
    //BEGIN: Tagging procedure ------------------------------------------------------
    cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
	 << "--------------------------------------------------" << endl
	 << "     ### Starting tagging procedure" <<endl
	 << "--------------------------------------------------" << endl;

    TagSegments(clTracklets[1], clTracklets[2], 1);
    cout << "--- Tagging 58-921 done." << endl;

    TagSegments(clTracklets[0], clTracklets[1], 0);
    cout << "--- Tagging 14-58 done." << endl;
  } //end of noHitsPerTracklet == 4

  tagTracks.Stop();
  //-------------------------------------------------------------------------



  //--------------------------------------------------------------------------------------
  //    Counting number of segments which have a value of 4, 3, 2, ... etc.
  CbmL1TrdTracklet4* clSegRight; //segment nearer to the target

  map<Int_t, Int_t> segValues14;
  map<Int_t, Int_t> segValues58;
  map<Int_t, Int_t> segValues912;
  Int_t segValue;

  vector<CbmL1TrdTracklet4*>::iterator itclTrackletsRight;
  //for tracklets in the 1st station
  for(itclTrackletsRight = clTracklets[0].begin();
      itclTrackletsRight != clTracklets[0].end();
      itclTrackletsRight++) {

    clSegRight = *itclTrackletsRight;
    segValue = clSegRight->GetVal();
    segValues14[segValue]++;
    if(segValue == 3) clTrackletsNew[0].push_back(clSegRight);
  }

  //for tracklets in the 2nd station
  for(itclTrackletsRight = clTracklets[1].begin();
      itclTrackletsRight != clTracklets[1].end();
      itclTrackletsRight++) {

    clSegRight = *itclTrackletsRight;
    segValue = clSegRight->GetVal();
    segValues58[segValue]++;
    if(segValue == 2) clTrackletsNew[1].push_back(clSegRight);

  }

  //for tracklets in the 3rd station
  for(itclTrackletsRight = clTracklets[2].begin();
      itclTrackletsRight != clTracklets[2].end();
      itclTrackletsRight++) {

    clSegRight = *itclTrackletsRight;
    segValue = clSegRight->GetVal();
    segValues912[segValue]++;
    if(segValue == 1) clTrackletsNew[2].push_back(clSegRight);
  }

  //--- For test only ----------------------------------------------------
  if(fVerbose > 0) {
    map<Int_t,Int_t>::iterator mIt;

    cout << "--- Map no. 14 has: " << endl;
    for(mIt = segValues14.begin();
	mIt != segValues14.end();
	mIt++) {
      cout << mIt->first << " segment has a count of "
	   << mIt->second << endl;
    }

    cout << "--- Map no. 58 has: " << endl;
    for(mIt = segValues58.begin();
	mIt != segValues58.end();
	mIt++) {
      cout << mIt->first << " segment has a count of "
	   << mIt->second << endl;
    }

    cout << "--- Map no. 912 has: " << endl;
    for(mIt = segValues912.begin();
	mIt != segValues912.end();
	mIt++) {
      cout << mIt->first << " segment has a count of "
	   << mIt->second << endl;
    }

    cout << endl;
  }
  //-------------------------------------------------------------------------

  //vector<CbmTrdTrack*> vTrdTrackCand;
  //vector<CbmTrdTrack*> vTempTrdTrackCand;
  //vector<CbmTrdTrack*>::iterator ivTempTrdTrackCand;
  //vector<CbmTrdTrack*> trackCand1;

  cout
    << "--------------------------------------------------" << endl
    << "   ### Starting creating the tracks               " << endl
    << "--------------------------------------------------" << endl;

  createTracks.Start();

  set<Int_t> globalSetUsedHits;
  globalSetUsedHits.clear();

  if(segment == 0) { //segment == 0 -> we create long tracks

    if(fVerbose > 2) {
      cout << "Outer area: " << endl
	   << "--- size of fArrayTrdTrack    = " << fArrayTrdTrack->GetEntriesFast() << endl
	   << "--- size of globalSetUsedHits = " << globalSetUsedHits.size() << endl;
    }

    //create long tracks from 3 sorts of tracklets - station 1,2,3
    CreateTracks(clTracklets[0], clTracklets[1],
		 clTracklets[2],
		 globalSetUsedHits,
		 removeUsedHits, competition, 1);

    createTracks.Stop();

    //clearing and removing objects that are not needed anymore
    for(Int_t i = 0; i < 3; i++){
      DeleteTracklets(clTracklets[i]);
      clTracklets[i].clear();
    }
    //----------------------------------------------------

    for(Int_t i = 0; i < 6; i++){
      DeleteTracklets(clSpacePoints[i]);
      clSpacePoints[i].clear();
    }

    //--- [Second Loop] ------------------------------------------------------
    secondLoopTime.Start();

    cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind (Second loop)" << endl
	 << "--------------------------------------------------" << endl
	 << "     ### (SL) in Event No. " << fNofEvents << " ###" <<endl
	 << "--------------------------------------------------" << endl;

    if(secondLoop) {

      for(Int_t i = 0; i < 12; i++) {
	fvTrdHitArr[i].clear();
      }

      set<Int_t>::iterator iglobalSetUsedHits;
      set<Int_t>::iterator iglobalSetRejectedHits;

      for (Int_t iHit = 0; iHit<nTrdHit; iHit++) {
	pHit = (CbmTrdHit*) hitArray->At(iHit);

	//	if(!Rejection(accept)) continue;

	if(!pHit) {
	  cout << "-E- CbmL1CATrdTrackFinderSA::DoFind: Empty slot "
	       << "in HitArray at position " << iHit << endl;
	  nNoTrdHit++;
	  continue;
	}

	//---- Simulate detector inefficiency -----------------------------------
	iglobalSetRejectedHits = globalSetRejectedHits.find(iHit);
	if(iglobalSetRejectedHits != globalSetRejectedHits.end()) continue;
	//----------------------------------------------------------------------

	iglobalSetUsedHits = globalSetUsedHits.find(iHit);
	if(iglobalSetUsedHits != globalSetUsedHits.end()) continue;

	trdPlaneID = pHit->GetPlaneId();
	Int_t trdPlaneIDN = trdPlaneID-1;

	planeHits.mcTrackID = pMCpt->GetTrackID();
	planeHits.hitIndex = iHit;
	planeHits.X  = pHit->GetX();
	planeHits.Y  = pHit->GetY();
	planeHits.Z  = pHit->GetZ();
	planeHits.DX = pHit->GetDx();
	planeHits.DY = pHit->GetDy();
	fvTrdHitArr[trdPlaneIDN].push_back(planeHits);
      }

      for(Int_t i = 0; i < 12; i++){
	sort(fvTrdHitArr[i].begin(),  fvTrdHitArr[i].end(),  CompareY);
      }

      //for(Int_t i = 0; i < 12; i++){
      // cout <<" Size of vector "<< i <<": "<< fvTrdHitArr[i].size() << endl;
      //}
      //--------------------------------------------------------------------------------------

      createSPs_SL.Start();

      //performing second step - make all previous tasks but with less restrictive conditions
      cout << "[Second Loop]::Creating space points"<< endl;

      for(Int_t i = 0, j = 0; i < 6; i++, j=j+2) {
	CreateSpacePoints(fvTrdHitArr[j], fvTrdHitArr[j+1],
			  clSpacePoints[i], sigmaA_SL, sigmaB_SL);
      }
      createSPs_SL.Stop();

      for(Int_t i = 0; i < 6; i++){
	sort(clSpacePoints[i].begin(), clSpacePoints[i].end(), CbmL1TrdTracklet::Compare1);
      }

      cout << "[Second Loop]::Creating tracklets" << endl;
      for(Int_t i = 0, j = 0; i < 3; i++, j=j+2) {
	CreateSegments(clSpacePoints[j], clSpacePoints[j+1],
		       clTracklets[i], dX_SL, dY_SL);
      }

      if(fVerbose > 2){
	cout << "--- size of segment vector 14 = "
	     << clTracklets[0].size()
	     << endl
	     << "--- size of segment vector 58 = "
	     << clTracklets[1].size()
	     << endl
	     << "--- size of segment vector 912 = "
	     << clTracklets[2].size()
	     << endl;
      }

      cout << "[Second Loop]::Finding friends 14-58" << endl;
      FindNeighbour(clTracklets[0],clTracklets[1],
		    distPropLongX_SL, distPropLongY_SL);

      cout << "[Second Loop]::Finding friends 58-912" << endl;
      FindNeighbour(clTracklets[1],clTracklets[2],
		    distPropLongX_SL, distPropLongY_SL);

      cout << "[Second Loop]::Tagging segments 58-912" << endl;
      TagSegments(clTracklets[1], clTracklets[2], 1);

      //--------------------------------------------------------------------------------------------
      cout << "[Second Loop]::Tagging segments 14-58" << endl;
      TagSegments(clTracklets[0], clTracklets[1], 0);

      //--------------------------------------------------------------------------------------------
      cout << "[Second Loop]::Creating tracks" << endl;
      CreateTracks(clTracklets[0], clTracklets[1],
		   clTracklets[2],
		   globalSetUsedHits,
		   removeUsedHits, competition, 2);
      //competition -> selects the best track candidate from each branch
    }

    for(Int_t i = 0; i < 3; i++){
      DeleteTracklets(clTracklets[i]);
      clTracklets[i].clear();
    }
    for(Int_t i = 0; i < 6; i++){
      DeleteTracklets(clSpacePoints[i]);
      clSpacePoints[i].clear();
    }

    secondLoopTime.Stop();

    thirdLoopTime.Start();

    cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind (Third loop)" << endl
	 << "--------------------------------------------------" << endl
	 << "     ### (TL) in Event No. " << fNofEvents << " ###" <<endl
	 << "--------------------------------------------------" << endl;

    if(thirdLoop) {

      for(Int_t i = 0; i < 12; i++) {
	fvTrdHitArr[i].clear();
      }

      set<Int_t>::iterator iglobalSetUsedHits;
      set<Int_t>::iterator iglobalSetRejectedHits;

      for (Int_t iHit = 0; iHit<nTrdHit; iHit++) {
	pHit = (CbmTrdHit*) hitArray->At(iHit);

	//	if(!Rejection(accept)) continue;

	if(!pHit) {
	  cout << "-E- CbmL1CATrdTrackFinderSA::DoFind: Empty slot "
	       << "in HitArray at position " << iHit << endl;
	  nNoTrdHit++;
	  continue;
	}

	//---- Simulate detector inefficiency -----------------------------------
	iglobalSetRejectedHits = globalSetRejectedHits.find(iHit);
	if(iglobalSetRejectedHits != globalSetRejectedHits.end()) continue;
	//----------------------------------------------------------------------

	iglobalSetUsedHits = globalSetUsedHits.find(iHit);
	if(iglobalSetUsedHits != globalSetUsedHits.end()) continue;

	trdPlaneID = pHit->GetPlaneId();
	Int_t trdPlaneIDN = trdPlaneID-1;

	planeHits.mcTrackID = pMCpt->GetTrackID();
	planeHits.hitIndex = iHit;
	planeHits.X  = pHit->GetX();
	planeHits.Y  = pHit->GetY();
	planeHits.Z  = pHit->GetZ();
	planeHits.DX = pHit->GetDx();
	planeHits.DY = pHit->GetDy();
	fvTrdHitArr[trdPlaneIDN].push_back(planeHits);
      }

      for(Int_t i = 0; i < 12; i++){
	sort(fvTrdHitArr[i].begin(),  fvTrdHitArr[i].end(),  CompareY);
      }

      for(Int_t i = 0, j = 0; i < 6; i++, j=j+2) {
	CreateSpacePoints(fvTrdHitArr[j], fvTrdHitArr[j+1],
			  clSpacePoints[i], sigmaA_TL, sigmaB_TL);
      }

      for(Int_t i = 0; i < 6; i++){
	sort(clSpacePoints[i].begin(), clSpacePoints[i].end(), CbmL1TrdTracklet::Compare1);
      }

      for(Int_t i = 0, j = 0; i < 3; i++, j=j+2) {
	CreateSegments(clSpacePoints[j], clSpacePoints[j+1],
		       clTracklets[i], dX_TL, dY_TL);
      }

      FindNeighbour(clTracklets[0],clTracklets[1],
		    distPropLongX_TL, distPropLongY_TL);
      FindNeighbour(clTracklets[1],clTracklets[2],
		    distPropLongX_TL, distPropLongY_TL);
      TagSegments(clTracklets[1], clTracklets[2], 1);

      TagSegments(clTracklets[0], clTracklets[1], 0);

      CreateTracks(clTracklets[0], clTracklets[1],
		   clTracklets[2],
		   globalSetUsedHits,
		   removeUsedHits, competition, 3);
    }

    thirdLoopTime.Stop();

  } //end segment == 0


  if(segment == 1) { //segment == 1 -> we create only 4-hit, 1-segment tracks!
    CreateAndManageSegments(clTracklets[0], clTracklets[1], clTracklets[2]);
  }



  //------ Used and Unused hists in the TRD's planes ------------------------
  //------ Histogramming
  set<Int_t>::iterator iglSetUsedHits;
  nTrdHit = hitArray->GetEntriesFast();

  for (Int_t iHit = 0; iHit<nTrdHit; iHit++) {
    pHit = (CbmTrdHit*) hitArray->At(iHit);

    trdPlaneID = pHit->GetPlaneId();
    fTotHits[trdPlaneID]++;

    iglSetUsedHits = globalSetUsedHits.find(iHit);
    if(iglSetUsedHits != globalSetUsedHits.end()){
      fRUsedHits[trdPlaneID]++;
      //fUsedHitsPerPlane->Fill(trdPlaneID-1);
    }else{
      fRUnUsedHits[trdPlaneID]++;
      //fUnUsedHitsPerPlane->Fill(trdPlaneID-1);
    }
  }
  //---------------------------------------------------------------------------


  cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
       << "--------------------------------------------------" << endl
       << "     ### Summary" << endl
       << "--------------------------------------------------" << endl
       << "--- Number of found tracks: " << fArrayTrdTrack->GetEntriesFast() << endl
       << endl;

  delTime.Start();

  cout << "\n-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
       << "--------------------------------------------------" << endl
       << "     ### Delete:Clear" <<endl
       << "--------------------------------------------------" << endl;

  for(Int_t i = 0; i < 12; i++){
    fvTrdHitArr[i].clear();
  }

  //----------------------------------------------------
  for(Int_t i = 0; i < 3; i++){
    DeleteTracklets(clTracklets[i]);
    clTracklets[i].clear();
  }

  //----------------------------------------------------
  for(Int_t i = 0; i < 6; i++){
    DeleteTracklets(clSpacePoints[i]);
    clSpacePoints[i].clear();
  }
  //----------------------------------------------------

  //segValues14.clear();
  //segValues58.clear();
  //segValues912.clear();

  delTime.Stop();
  doFind.Stop();



  //-------------------------------------------------------
#ifdef DRAW

  fNTrdHits = hitArray->GetEntriesFast();

  for (Int_t iHit = 0; iHit < fNTrdHits; iHit++) {
    pHit = (CbmTrdHit*) hitArray->At(iHit);

    ptIndex = pHit->GetRefIndex();
    pMCpt = (CbmTrdPoint*) (fMCPointArray->At(ptIndex));
    pMCtr = (CbmMCTrack*) fMCTrackArray->At(pMCpt->GetTrackID());

    trdPlaneID = pHit->GetPlaneID();
    Int_t trdPlaneIDN = trdPlaneID-1;

    planeHits.mcTrackID = pMCpt->GetTrackID();
    planeHits.hitIndex = iHit;
    planeHits.X  = pHit->GetX();
    planeHits.Y  = pHit->GetY();
    planeHits.Z  = pHit->GetZ();
    planeHits.DX = pHit->GetDx();
    planeHits.DY = pHit->GetDy();
    planeHits.planeID = trdPlaneID;
    fvTrdHitArr[trdPlaneIDN].push_back(planeHits);
  }

  Draw2();
  DrawAsk();

  for(Int_t i = 0; i < 12; i++){
    fvTrdHitArr[i].clear();
  }

#endif


  //-------------------------------------------------------



  //-----------------------------------------------------------------
  Double_t rtime;
  Double_t ctime;
  Double_t qtime = Double_t(fNofEvents);

  cout << endl <<"\n-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
       << "--------------------------------------------------" << endl
       << "     ### Time" <<endl
       << "--------------------------------------------------" << endl;
  cout << "         Do find: ";

  rtime = doFind.RealTime();
  ctime = doFind.CpuTime();
  totDoFind += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totDoFind,totDoFind/qtime);

  cout << "       Sort Hits: ";
  rtime = sortHits.RealTime();
  ctime = sortHits.CpuTime();
  totSortHits += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totSortHits,totSortHits/qtime);

  cout << "      Create SPs: ";
  rtime = createSPs.RealTime();
  ctime = createSPs.CpuTime();
  totCreateSPs += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totCreateSPs,totCreateSPs/qtime);

  cout << "        Sort SPs: ";
  rtime = sortSPs.RealTime();
  ctime = sortSPs.CpuTime();
  totSortSPs += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totSortSPs,totSortSPs/qtime);

  cout << " Create segments: ";
  rtime = createSegments.RealTime();
  ctime = createSegments.CpuTime();
  totCreateSegments += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totCreateSegments,totCreateSegments/qtime);

  cout << "     Find friend: ";
  rtime = findNeighbour.RealTime();
  ctime = findNeighbour.CpuTime();
  totFindNeighbour += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totFindNeighbour,totFindNeighbour/qtime);

  cout << "   Tag Tracklets: ";
  rtime = tagTracks.RealTime();
  ctime = tagTracks.CpuTime();
  totTagTracks += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totTagTracks,totTagTracks/qtime);

  cout << "   Create Tracks: ";
  rtime = createTracks.RealTime();
  ctime = createTracks.CpuTime();
  totCreateTracks += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totCreateTracks,totCreateTracks/qtime);

  cout << "Refitting Tracks: ";
  rtime = refittingKF.RealTime();
  ctime = refittingKF.CpuTime();
  totRefittingKF += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totRefittingKF,totRefittingKF/qtime);

  cout << "     Second Loop: ";
  rtime = secondLoopTime.RealTime();
  ctime = secondLoopTime.CpuTime();
  totSecondLoopTime += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totSecondLoopTime,totSecondLoopTime/qtime);

  cout << "      Third Loop: ";
  rtime = thirdLoopTime.RealTime();
  ctime = thirdLoopTime.CpuTime();
  totThirdLoopTime += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totThirdLoopTime,totThirdLoopTime/qtime);

  cout << "Deleting Objects: ";
  rtime = delTime.RealTime();
  ctime = delTime.CpuTime();
  totDelTime += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totDelTime,totDelTime/qtime);

  cout<<"[Second Loop]"<< endl;
  cout << "      Create SPs: ";
  rtime = createSPs_SL.RealTime();
  ctime = createSPs_SL.CpuTime();
  totCreateSPs_SL += ctime;
  printf("RealTime=%4.2f s, CpuTime=%4.2f s, TotCpu=%4.2f s, %4.2f/event\n",rtime,ctime,totCreateSPs_SL,totCreateSPs_SL/qtime);


  cout << endl << "-I- CbmL1CATrdTrackFinderSA::DoFind " << endl
       << "--------------------------------------------------" << endl
       << "     ### END: Track constructing phase completed" <<endl
       << "--------------------------------------------------" << endl;

  fNoEvTime->Fill(fArrayTrdTrack->GetEntriesFast(),doFind.RealTime());

  return 1;
}
// -----------------------------------------------------------------------

void  CbmL1CATrdTrackFinderSA::DeleteTracklets(vector<CbmL1TrdTracklet4*> vect) {

  vector<CbmL1TrdTracklet4*>::iterator it;
  for(it  = vect.begin();
      it != vect.end();
      it++) {
    delete (*it);
  }
}

// -----------------------------------------------------------------------

void  CbmL1CATrdTrackFinderSA::DeleteTracklets(vector<CbmL1TrdTracklet*> vect)
{
  vector<CbmL1TrdTracklet*>::iterator it;
  for(it  = vect.begin();
      it != vect.end();
      it++) {
    delete (*it);
  }
}


// ---------------------- Create histogramms -----------------------------
void CbmL1CATrdTrackFinderSA::CreateHistogramms() {

  fUsedHitsPerPlane   =  new TH1F("h_UsedHitsPerPlane",   "h_UsedHitsPerPlane",   13, 0, 13);
  fUnUsedHitsPerPlane =  new TH1F("h_UnUsedHitsPerPlane", "h_UnUsedHitsPerPlane", 13, 0, 13);


  // Normalized distance to hit
  fh_chi2hit = new TH1F("h_chi2hit",
			"Normalized distance to hit",
			500, 0., 50.);
  fh_chi2hit_plane = new TH2F("h_chi2hit_plane",
			      "Normalized distance to hit vs. plane number",
			      500, 0., 50.,
			      12, 0., 12.);

  fDistLongX   = new TH1F("DistLongX",  "DistLongX",  100,-200,200);
  fDistLongY   = new TH1F("DistLongY",  "DistLongY",  100,-200,200);
  fDistShortX  = new TH1F("DistShortX", "DistShortX", 100,-10,10);
  fDistShortY  = new TH1F("DistShortY", "DistShortY", 100,-10,10);

  fDistLongBX   = new TH1F("DistLongBX",  "DistLongBX",  100,-200,200);
  fDistLongBY   = new TH1F("DistLongBY",  "DistLongBY",  100,-200,200);
  fDistShortBX  = new TH1F("DistShortBX", "DistShortBX", 100,-200,200);
  fDistShortBY  = new TH1F("DistShortBY", "DistShortBY", 100,-200,200);

  fDistY12 = new TH1F("DistY12","DistY12",100,-1,1);

  fMomDistLongPrimaryX = new TH2F("MomDistLongPrimaryX",  "MomDistLongPrimaryX",  100,0,10,100,-30,30);
  fMomDistLongPrimaryY = new TH2F("MomDistLongPrimaryY",  "MomDistLongPrimaryY",  100,0,10,100,-30,30);
  fMomDistExtrapolPrimaryX = new TH2F("MomDistExtrapolPrimaryX",  "MomDistExtrapolPrimaryX",  100,0,10,200,-20,20);
  fMomDistExtrapolPrimaryY = new TH2F("MomDistExtrapolPrimaryY",  "MomDistExtrapolPrimaryY",  100,0,10,200,-20,20);

  fMomDistLongExtraX = new TH2F("MomDistLongExtraX",  "MomDistLongExtraX",  100,0,10,100,-30,30);
  fMomDistLongExtraY = new TH2F("MomDistLongExtraY",  "MomDistLongExtraY",  100,0,10,100,-30,30);
  fMomDistExtrapolExtraX = new TH2F("MomDistExtrapolExtraX",  "MomDistExtrapolExtraX",  100,0,10,200,-20,20);
  fMomDistExtrapolExtraY = new TH2F("MomDistExtrapolExtraY",  "MomDistExtrapolExtraY",  100,0,10,200,-20,20);

  fMomDistShortPrimaryX = new TH2F("MomDistShortPrimaryX",  "MomDistShortPrimaryX",  100,0,10,100,-5,5);
  fMomDistShortPrimaryY = new TH2F("MomDistShortPrimaryY",  "MomDistShortPrimaryY",  100,0,10,100,-5,5);
  fMomDistShortExtraX   = new TH2F("MomDistShortExtraX",    "MomDistShortExtraX",    100,0,10,100,-5,5);
  fMomDistShortExtraY   = new TH2F("MomDistShortExtraY",    "MomDistShortExtraY",    100,0,10,100,-5,5);

  fDistY = new TH1F("DistY","DistY",1000,-10,10);
  fDistX = new TH1F("DistX","DistX",1000,-10,10);

  fPlane1Ydens = new TH1F("Plane1Ydens","Plane1Ydens",1000,-1000,1000);
  fPlane5Ydens = new TH1F("Plane5Ydens","Plane5Ydens",1000,-1000,1000);
  fPlane9Ydens = new TH1F("Plane9Ydens","Plane9Ydens",1000,-1000,1000);

  fSPlengthMC = new TH1F("SPlengthMC","SPlengthMC",200,0,20);
  fSPlength   = new TH1F("SPlength",  "SPlength",  200,0,20);

  fYat0   = new TH1F("Yat0",   "Yat0",   500,-500,500);
  fYat0MC = new TH1F("Yat0MC", "Yat0MC", 500,-500,500);

  fNoEvTime = new TH2F("NoEvTime","NoEvTime",1000,0,1000,1000,0,5);

  fh_SP_xDiff_MC = new TH1F("fh_SP_xDiff_MC","fh_SP_xDiff_MC",400,-20,20);
  fh_SP_yDiff_MC = new TH1F("fh_SP_yDiff_MC","fh_SP_yDiff_MC",400,-20,20);

  fh_SP_xDiff_nMC = new TH1F("fh_SP_xDiff_nMC","fh_SP_xDiff_nMC",400,-20,20);
  fh_SP_yDiff_nMC = new TH1F("fh_SP_yDiff_nMC","fh_SP_yDiff_nMC",400,-20,20);

}
// -----------------------------------------------------------------------



// -------------------- Activates data branches --------------------------
void CbmL1CATrdTrackFinderSA::DataBranches() {

  // Get pointer to the ROOT manager
  FairRootManager* rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    cout << "-E- CbmL1CATrdTrackFinderSA::DataBranches : "
	 << "ROOT manager is not instantiated" << endl;
    return;
  }

  fArrayTrdHit  = (TClonesArray*) rootMgr->GetObject("TrdHit");
  if ( ! fArrayTrdHit) {
    cout << "-E- CbmL1CATrdTrackFinderSA::Init: No TrdHit array!"
	 << endl;
    return;
  }
}
// -----------------------------------------------------------------------



// ------------------- Determines the TRD layout -------------------------
void CbmL1CATrdTrackFinderSA::TrdLayout() {

  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 <<" no FairRunAna object!" << endl;
    return;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 <<" no runtime database!" << endl;
    return;
  }
  // Get the pointer to container of base parameters
  FairBaseParSet* baseParSet =
    (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
  if(NULL == baseParSet) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 <<" no container of base parameters!" << endl;
    return;
  }

  TrdPar = (CbmGeoTrdPar*) (rtdb->findContainer("CbmGeoTrdPar"));
  TObjArray *Nodes = TrdPar->GetGeoSensitiveNodes();
  for( Int_t i=0;i<Nodes->GetEntries(); i++) {
    FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
    //FairGeoNode *node = (FairGeoNode*) Nodes->At(i);
    if ( !node ) continue;

    TString name = node->getName();
    //TString Sname = node->getShapePointer()->GetName();
    FairGeoVector nodeV = node->getLabTransform()->getTranslation(); //in cm

    //    FairGeoBasicShape *gShapePointer = (FairGeoBasicShape *)node->getShapePointer();
    //gShapePointer->printParam();

    //    node->print();

    //Int_t id = node->getMCid();
    //cout <<"name: "<< name <<"\tid: "<< id << endl;
    cout <<" name: "<< name
     	 <<"\t(z): "
	 << nodeV.Z() <<endl;

    //     if(name.Contains("trd13")) fTrd13_Z = nodeV.Z();
    //     if(name.Contains("trd14")) fTrd14_Z = nodeV.Z();
    //     if(name.Contains("trd21")) fTrd21_Z = nodeV.Z();
    //     if(name.Contains("trd24")) fTrd24_Z = nodeV.Z();
    //     if(name.Contains("trd31")) fTrd31_Z = nodeV.Z();

    if(name == "trd1gas#3") fTrd13_Z = Int_t(nodeV.Z());
    if(name == "trd1gas#4") fTrd14_Z = Int_t(nodeV.Z());
    if(name == "trd2gas#1") fTrd21_Z = Int_t(nodeV.Z());
    if(name == "trd2gas#4") fTrd24_Z = Int_t(nodeV.Z());
    if(name == "trd3gas#1") fTrd31_Z = Int_t(nodeV.Z());

//     if(name == "trd13") fTrd13_Z = nodeV.Z();
//     if(name == "trd14") fTrd14_Z = nodeV.Z();
//     if(name == "trd21") fTrd21_Z = nodeV.Z();
//     if(name == "trd24") fTrd24_Z = nodeV.Z();
//     if(name == "trd31") fTrd31_Z = nodeV.Z();

    if(name == "trd1gas#1") geoLayer.Z[0]  = nodeV.Z();
    if(name == "trd1gas#2") geoLayer.Z[1]  = nodeV.Z();
    if(name == "trd1gas#3") geoLayer.Z[2]  = nodeV.Z();
    if(name == "trd1gas#4") geoLayer.Z[3]  = nodeV.Z();
    if(name == "trd2gas#1") geoLayer.Z[4]  = nodeV.Z();
    if(name == "trd2gas#2") geoLayer.Z[5]  = nodeV.Z();
    if(name == "trd2gas#3") geoLayer.Z[6]  = nodeV.Z();
    if(name == "trd2gas#4") geoLayer.Z[7]  = nodeV.Z();
    if(name == "trd3gas#1") geoLayer.Z[8]  = nodeV.Z();
    if(name == "trd3gas#2") geoLayer.Z[9]  = nodeV.Z();
    if(name == "trd3gas#3") geoLayer.Z[10] = nodeV.Z();
    if(name == "trd3gas#4") geoLayer.Z[11] = nodeV.Z();
  }

#ifdef DRAW
  for(int i = 0; i < 4; i++){
    geoLayer.Y[i] = 273.1;
  }
  for(int i = 4; i < 8; i++){
    geoLayer.Y[i] = 396.0;
  }
  for(int i = 8; i < 12; i++){
    geoLayer.Y[i] = 518.9;
  }

  Int_t scaleZ  = 300;
  Int_t scaleZA = 0;
  for(int i = 0; i < 12; i++){
    if(i == 4) scaleZA = 1000;
    if(i == 8) scaleZA *= 2;

    geoLayer.scale[i] += ((i+1)*scaleZ)+scaleZA;
    geoLayer.Z[i] += geoLayer.scale[i];
  }
#endif

  /*
    TrdPar = (CbmGeoTrdPar*) (rtdb->findContainer("CbmGeoTrdPar"));
    //TObjArray *Nodes = TrdPar->GetGeoSensitiveNodes();
    Nodes = TrdPar->GetGeoSensitiveNodes();
    for( Int_t i=0;i<Nodes->GetEntries(); i++) {
    FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
    if ( !node ) continue;

    TString name = node->getName();
    FairGeoVector nodeV = node->getLabTransform()->getTranslation(); //in cm

    Int_t uid = i+1;
    cout <<"name, uid, Z: "<< name <<", "<< uid <<", "<< nodeV.Z() << endl;

    if(uid == 3) fTrd13_Z = nodeV.Z();
    if(uid == 4) fTrd14_Z = nodeV.Z();
    if(uid == 5) fTrd21_Z = nodeV.Z();
    if(uid == 8) fTrd24_Z = nodeV.Z();
    if(uid == 9) fTrd31_Z = nodeV.Z();
    }
  */

  // Get the pointer to detector list
  TObjArray *detList = baseParSet->GetDetList();
  if(NULL == detList) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 << " no detector list!" << endl;
    return;
  }
  // Find TRD detector
  FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
  if(NULL == trd) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 << " no TRD detector!" << endl;
    return;
  }
  // Determine the geometry version
  TString name = trd->GetGeometryFileName();
  if(name.Contains("9")) {
    cout << "-I- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 << " TRD geometry : 3x3." << endl;
    fNoTrdStations = 3;
    fNoTrdPerStation = 3;
  } else if(name.Contains("12")) {
    cout << "-I- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 << " TRD geometry : 3x4." << endl;
    fNoTrdStations = 3;
    fNoTrdPerStation = 4;
  } else if(name.Contains("6x2")) {
    cout << "-I- CbmL1CATrdTrackFinderSA::TrdLayout :"
	 << " TRD geometry : 6x2." << endl;
    fNoTrdStations = 6;
    fNoTrdPerStation = 2;
  }
    fNoTrdStations = 3;
    fNoTrdPerStation = 4;
}
// -----------------------------------------------------------------------


// ------------------------- Write histogramms ---------------------------
void CbmL1CATrdTrackFinderSA::WriteHistogramms() {

  fh_chi2hit->Write();
  fh_chi2hit_plane->Write();

  fDistLongBX->Write();
  fDistLongBY->Write();
  fDistShortBX->Write();
  fDistShortBY->Write();

  fDistY12->Write();

  //----------------------------------------------------------
  fMomDistLongPrimaryX->Write();
  fMomDistLongPrimaryY->Write();
  fMomDistLongExtraX->Write();
  fMomDistLongExtraY->Write();

  fMomDistExtrapolPrimaryX->Write();
  fMomDistExtrapolPrimaryY->Write();
  fMomDistExtrapolExtraX->Write();
  fMomDistExtrapolExtraY->Write();

  fMomDistShortPrimaryX->Write();
  fMomDistShortPrimaryY->Write();
  fMomDistShortExtraX->Write();
  fMomDistShortExtraY->Write();

  fDistLongX->Write();
  fDistLongY->Write();
  fDistShortX->Write();
  fDistShortY->Write();

  fDistY->Write();
  fDistX->Write();

  fPlane1Ydens->Write();
  fPlane5Ydens->Write();
  fPlane9Ydens->Write();

  fSPlength->Write();
  fSPlengthMC->Write();

  fYat0->Write();
  fYat0MC->Write();

  fNoEvTime->Write();

  fh_SP_xDiff_MC->Write();
  fh_SP_yDiff_MC->Write();

  fh_SP_xDiff_nMC->Write();
  fh_SP_yDiff_nMC->Write();



  //---------------------------------------------------------------------
  map<Int_t, Int_t>::iterator iUsedHits;
  map<Int_t, Int_t>::iterator iUnUsedHits;
  map<Int_t, Int_t>::iterator iTotHits;
  Double_t nContent;

  for(iUsedHits  = fRUsedHits.begin();
      iUsedHits != fRUsedHits.end();
      iUsedHits++){

    iTotHits = fTotHits.find(iUsedHits->first);
    nContent = (iUsedHits->second*100)/iTotHits->second;
//     cout << iUsedHits->first  <<", "
// 	 << iUsedHits->second <<", "
// 	 << iTotHits->second <<", "
// 	 << nContent << endl;
    fUsedHitsPerPlane->SetBinContent((iUsedHits->first), nContent);
  }
  for(iUnUsedHits  = fRUnUsedHits.begin();
      iUnUsedHits != fRUnUsedHits.end();
      iUnUsedHits++){

    iTotHits = fTotHits.find(iUnUsedHits->first);
    nContent = (iUnUsedHits->second*100)/iTotHits->second;
    fUnUsedHitsPerPlane->SetBinContent((iUnUsedHits->first), nContent);
  }
  fUsedHitsPerPlane->Write();
  fUnUsedHitsPerPlane->Write();
  //---------------------------------------------------------------------



}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
Double_t CbmL1CATrdTrackFinderSA::FitLinear(CbmTrdTrack *tr,
					    Int_t var = 1) {
  //fit using a least square method


  //  cout << ">>> Trying to fit a track..." << endl;
  Int_t noHits = tr->GetNofHits();
  //  cout << "No Hits in the track : " << noHits << endl;
  Int_t iHit;
  CbmTrdHit *hit;

  Double_t C1[12];
  Double_t C2[12];
  Double_t  Z[12];

  Double_t n = 12;

  for(int i=0;i<12;i++) {
    C1[i] = 0;
    C2[i] = 0;
    Z [i] = 0;
  }

  //  Int_t ind = 0;

  for(int i=0;i<noHits;i++) {

    iHit = tr->GetHitIndex(i);
    hit = (CbmTrdHit*)fArrayTrdHit->At(iHit);

    C1[i] = hit->GetX();
    C2[i] = hit->GetY();
    Z[i]  = hit->GetZ();
  }

  Double_t
    sumXZ = 0,
    sumYZ = 0,
    sumX  = 0,
    sumY  = 0,
    sumZx = 0,
    sumZy = 0,
    sumX2 = 0,
    sumY2 = 0,
    sumZx2 = 0,
    sumZy2 = 0;

  for(int i=0;i<12;i+=2) {
    //    cout << "C1[" << i << "]=" << C1[i] << endl;
    //    cout << "C2[" << i << "]=" << C2[i] << endl;

    //    if(!(i % 2))
    {
      sumXZ += C1[i]*Z[ i];

      sumX  += C1[i];
      sumX2 += C1[i]*C1[i];

      sumZx  += Z[i];
      sumZx2 += Z[i]*Z[ i];


    }
  }

  for(int i=1;i<12;i+=2) {
    //    cout << "C1[" << i << "]=" << C1[i] << endl;
    //    cout << "C2[" << i << "]=" << C2[i] << endl;

    //    if(!((1i % 2))
    {
      sumYZ += C2[i]*Z[ i];

      sumY  += C2[i];
      sumY2 += C2[i]*C2[i];

      sumZy  += Z[ i];
      sumZy2 += Z[ i]*Z[ i];
    }
  }


  cout << "";
  //   Double_t a = (n*sumXZ - sumX*sumZ)/(n*sumX2 - sumX*sumX);
  //   Double_t b = (sumZ - a*sumX)/n;
  Double_t r1 = (n*sumXZ - sumX*sumZx)/sqrt((n*sumX2-sumX*sumX)*(n*sumZx2-sumZx*sumZx));
  //  Double_t Sa = sqrt((n*(sumY2 -a*sumXZ-b*sumY))/((n-2)*(n*sumX2-sumX*sumX)));
  //  Double_t Sb = Sa*sqrt(sumX2/n);


  Double_t r2 = (n*sumYZ - sumY*sumZy)/sqrt((n*sumY2-sumY*sumY)*(n*sumZy2-sumZy*sumZy));


  //   cout << "Linear coefficients: a= "
  //        << a << ", Sa= " << Sa << ", b= " << b << ", Sb= " << Sb << endl
  //        << ", r=" << r << ", chi2=" << tr->GetChi2() << endl;


  return sqrt(r1*r1+r2*r2);
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
Double_t CbmL1CATrdTrackFinderSA::DistTwoTrackletsY(Int_t iIndexFirst,
						    Int_t iIndexSecond,
						    Double_t zed) {
  //returns an extrapolated coordinate
  CbmTrdHit
    *pHitFirst,
    *pHitSecond;
  Double_t dist;

  Double_t Y1 = 0;
  Double_t Z1 = 0;

  // Get hits from first and second plane in station
  if(iIndexFirst != -1) {
    pHitFirst  = (CbmTrdHit*) fArrayTrdHit->At(iIndexFirst);
    Y1  = pHitFirst->GetY();
    Z1  = pHitFirst->GetZ();
  }

  pHitSecond = (CbmTrdHit*) fArrayTrdHit->At(iIndexSecond);

  // Get position Y & Z of hits

  Double_t Y4 = pHitSecond->GetY();
  Double_t Z4 = pHitSecond->GetZ();

  Double_t a = (Y4 - Y1)/(Z4 - Z1);
  Double_t b = Y1 - a*Z1;
  Double_t Y = a*zed + b ;// - Y1;
  dist = Y;
  //      cout << "From the coords (y1,z1) = ("
  // 	  << Y1 << ", " << Z1 << endl
  //           << "From the coords (y2,z2) = ("
  // 	  << Y4 << ", " << Z4 << endl
  //           << "     we get the value = "
  //           << dist
  //           << endl;
  return dist;
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------

Double_t CbmL1CATrdTrackFinderSA::DistTwoTrackletsX(Int_t iIndexFirst,
						    Int_t iIndexSecond,
						    Double_t zed) {
  CbmTrdHit
    *pHitFirst,
    *pHitSecond;
  Double_t dist;

  Double_t X1 = 0;
  Double_t Z1 = 0;

  // Get hits from first and second plane in station
  if(iIndexFirst != -1) {
    pHitFirst  = (CbmTrdHit*) fArrayTrdHit->At(iIndexFirst);
    X1  = pHitFirst->GetX();
    Z1  = pHitFirst->GetZ();
  }

  pHitSecond = (CbmTrdHit*) fArrayTrdHit->At(iIndexSecond);


  Double_t X4 = pHitSecond->GetX();
  Double_t Z4 = pHitSecond->GetZ();

  Double_t a = (X4 - X1)/(Z4 - Z1);
  Double_t b = X1 - a*Z1;
  //  Double_t X = (dz)/(dx)*(zed - Z1);
  Double_t X = a*zed + b;
  //    = dy/dz * (zed + Z1) - X1;
  dist = X;
  //       cout << "From the coords (x1,z1) = ("
  // 	   << X1 << ", " << Z1 << endl
  //            << "From the coords (x4,z4) = ("
  // 	   << X4 << ", " << Z4 << endl
  // 	   << "and at zed = " << zed << endl
  //            << "     we get the value = "
  //            << dist
  //            << endl;
  return dist;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
void CbmL1CATrdTrackFinderSA::FindNeighbour(vector<CbmL1TrdTracklet4*> &v1,
					    vector<CbmL1TrdTracklet4*> &v2,
					    Double_t dY, Double_t dX) {

  vector<CbmL1TrdTracklet4*>::iterator iv1;
  CbmL1TrdTracklet4 *tr1;

  Double_t
    extY = 0,extX = 0,  //two extrapolated coords from the first tracklet
    mesY = 0,mesX = 0,  //two measured coords from the second tracklet
    aminY, amaxY,
    aminX, amaxX;
  Int_t indexA, indexB;
  Int_t
    Left = 0,
    Right,
    Mid = 0;

  Int_t il = 0;
  for(iv1 = v1.begin(); iv1 != v1.end(); iv1++) {
    il++;
    tr1 = *iv1;
    extY = tr1->GetExt(0);
    extX = tr1->GetExt(1);
    indexA = tr1->GetIndex();
    amaxY = extY + dY;
    aminY = extY - dY;
    amaxX = extX + dX;
    aminX = extX - dX;

    Left = 0;
    Right = v2.size();
    Mid = 0;

    // ----------- looking by bisection ------------------
    while(Left < Right){
      Mid = (Left+Right)/2;
      mesY = v2[Mid]->GetCoord(0);

      if (amaxY < mesY){
	Left = Mid+1;
      }else{
	Right = Mid-1;
      }

      /*
	cout << il << ": Size: "<< v1.size()
	<<" mesY: "<< v2[Mid]->GetCoord(0)
	<<" extY: "<< extY
	<<" Mid: "<< Mid
	<< endl;
      */
    }
    //----------------------------------------------------

    Int_t size = v2.size();
    for(Int_t i = Mid; i < size; i++) {
      mesY = v2[i]->GetCoord(0);
      mesX = v2[i]->GetCoord(1);
      //cout <<"(mesX,MesY) "<< mesX <<"\t"<< mesY << endl;
      indexB = v2[i]->GetIndex();
      //cout <<"indexB "<< indexB << endl;

      if (mesY > amaxY) continue;
      if (mesY < aminY) break;

      //      cout <<"----------- Setting vAccost [Right|Left]"<< endl;

      //      cout << aminX <<", "<< mesX <<", "<< amaxX << endl;

      if(aminX < mesX && mesX < amaxX) {
	tr1->vAccostRight.push_back(indexB);
	v2[i]->vAccostLeft.push_back(indexA);
	//	cout << aminX <<", "<< mesX <<", "<< amaxX << endl;
      }
    }
  }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
Bool_t CbmL1CATrdTrackFinderSA::OverlapsHitsXY(Int_t posA,
					       Int_t posB)
{
  Bool_t overlap = false;

  CbmTrdHit
    *pHitA,
    *pHitB;

  Double_t
    hitPosA_X,
    hitPosA_Y,
    hitPosB_X,
    hitPosB_Y,
    dMul1 = 3, //sigma multiplier for calculating the range of overlapping hits
    dMul2 = 3; //sigma multiplier for calculating the range of overlapping hits

  Double_t
    hitPosA_DX,
    hitPosA_DY,
    hitPosB_DX,
    hitPosB_DY;

  pHitA = (CbmTrdHit*) fArrayTrdHit->At(posA);
  hitPosA_X  = pHitA->GetX();
  hitPosA_Y  = pHitA->GetY();
  hitPosA_DX = pHitA->GetDx();
  hitPosA_DY = pHitA->GetDy();

  pHitB = (CbmTrdHit*) fArrayTrdHit->At(posB);
  hitPosB_X  = pHitB->GetX();
  hitPosB_Y  = pHitB->GetY();
  hitPosB_DX = pHitB->GetDx();
  hitPosB_DY = pHitB->GetDy();

  //   cout
  //     << "Overlap function...\n"
  //     << "X1=" << hitPosA_X << ", dX1=" << hitPosA_DX << ", Y1=" << hitPosA_Y << ", dY1=" << hitPosA_DY
  //     << "X2=" << hitPosB_X << ", dX2=" << hitPosB_DX << ", Y2=" << hitPosB_Y << ", dY2=" << hitPosB_DY
  //     << endl;


  if(((hitPosA_X+dMul1*hitPosA_DX) >= (hitPosB_X-dMul2*hitPosB_DX)) &&
     ((hitPosA_X-dMul1*hitPosA_DX) <= (hitPosB_X+dMul2*hitPosB_DX)) &&
     ((hitPosA_Y+dMul2*hitPosA_DY) >= (hitPosB_Y-dMul1*hitPosB_DY)) &&
     ((hitPosA_Y-dMul2*hitPosA_DY) <= (hitPosB_Y+dMul1*hitPosB_DY))){
    overlap = true;
  }

  //   CbmTrdPoint *pt1 = (CbmTrdPoint*)fMCPointArray->At(posA);
  //   CbmTrdPoint *pt2 = (CbmTrdPoint*)fMCPointArray->At(posB);

  //   if(overlap && (pt1->GetTrackID() == pt2->GetTrackID())) return true;
  //   else return false;

  return overlap;
}


// -----------------------------------------------------------------------
void CbmL1CATrdTrackFinderSA::TagSegments(vector<CbmL1TrdTracklet4*>& clTrackletsA,
					  vector<CbmL1TrdTracklet4*>& clTrackletsB,
					  Int_t noCombSegments) {
  //asigning numbers to each segment, 4- highest, 0 - lowest

  //  cout << "TagSegments: engaging... " << endl;

  //  vector<CbmL1TrdTracklet4*>::iterator itclTrackletsLeft;
  vector<CbmL1TrdTracklet4*>::iterator itclTrackletsRight;

  CbmL1TrdTracklet4* clSegRight;

  vector<Int_t> vLeft;
  vector<Int_t> vRight;

  vector<Int_t>::iterator ivLeft;
  //  vector <Int_t>::iterator ivRight;

  Int_t
    valLeft  = 0,
    valRight = 0;

  //     CbmL1TrdTracklet4* clSegLeft; //segment farther from the target
  //     CbmL1TrdTracklet4* clSegRight; //segment nearer to the target


  //  cout << "Engaging Right loop " << endl;
  for(itclTrackletsRight = clTrackletsB.begin();
      itclTrackletsRight != clTrackletsB.end();
      itclTrackletsRight++) {

    clSegRight = *itclTrackletsRight;
    //cout << "---> X of tracklet = " << clSegRight->GetCoord(0) << endl;
    vLeft = clSegRight->vAccostLeft;
    //cout << "Size of Right vector = " << clSegRight->vAccostRight.size() << endl;
    //cout << "Size of Left vector  = " << clSegRight->vAccostLeft.size() << endl;
    //    iHitRightA = clSegRight->GetInd(0);
    //    iHitRightB = clSegRight->GetInd(1);

    //      cout << "Tracklets 912: iHitRight:   " << iHitRight << "\t" << endl;
    if(vLeft.size() > 0) {
      //      cout << "Processing Comrade vector of size " << vLeft.size() << endl;
      valRight = clSegRight->GetVal();
      if(valRight == 0) {
	clSegRight->SetVal(1);
	valRight++;
      }
      for(ivLeft = vLeft.begin(); ivLeft != vLeft.end(); ivLeft++) {
	valLeft  = (clTrackletsA[*ivLeft])->GetVal();
	if(valLeft <= valRight) clTrackletsA[*ivLeft]->SetVal(valRight+1);
      }
    }

    //    if(noCombSegments == 1){
    //  if((*itclTrackletsRight)->GetVal() != 2) clTrackletsB.erase(itclTrackletsRight);
    //}

    //cout << "clSegRight = " << clSegRight->GetVal() << endl;
    //if(isAlone == true) clSegRight->SetIsAlone(true);
  }
}
// -----------------------------------------------------------------------






// -----------------------------------------------------------------------
void CbmL1CATrdTrackFinderSA::CreateTracks(vector<CbmL1TrdTracklet4*> clTracklets14,
					   vector<CbmL1TrdTracklet4*> clTracklets58,
					   vector<CbmL1TrdTracklet4*> clTracklets912,
					   set<Int_t> &globalSetUsedHits,
					   Bool_t removeUsedHits,
					   Bool_t competition,
					   Int_t nrLoop){
  //create long tracks from previously selected segments

  if(fVerbose > 2) {
    cout << "Inner area: " << endl
	 << "--- size of fArrayTrdTrack    = " << fArrayTrdTrack->GetEntriesFast() << endl
	 << "--- size of globalSetUsedHits = " << globalSetUsedHits.size() << endl;
  }

  CbmL1TrdTracklet4
    *clTrdSeg1,
    *clTrdSeg2;

  vector<CbmL1TrdTracklet4*>::iterator itclSeg3;

  Bool_t isEmpty = true;

  Int_t
    licz = 0,
    counter = 0;

  CbmTrdTrack* tr1 = NULL;
  //  CbmTrdTrack* tr2 = NULL;
  CbmTrdHit* hit1 = NULL;
  //CbmTrdHit* hit2 = NULL;

  vector<CbmTrdTrack*> vTempTrdTrackCand;
  vector<CbmTrdTrack*>::iterator ivTempTrdTrackCand;

  set<Int_t>::iterator iglobalSetUsedHits;

  vector<CbmTrdTrack*>::iterator ivTrdTrackCand;

  Int_t iFakeTrack = 0;
  Int_t iHit = 0;
  Bool_t bTrueTrack = true;

  FairTrackParam *trParam;
  trParam = new FairTrackParam();
  trParam->SetQp(0.);

  CbmVertex vtx;
  TVector3 vzero(0,0,0);
  vtx.Position(vzero);

  Int_t iSecondLoop = 1;
  Int_t trMax = 1;          //number of tracks from one tree accepted as tracks candidates
  Int_t noHitsAccepted = 0; //number of used hits above which the tracks is recognized as fake; 0 = all tracks are fake.

  vector<TempTrackStruct> auxTrack;
  auxTrack.clear();

  Int_t trdInd;
  vector<TempTrackStruct>::iterator ikol;

  cout << "--- Engaging main loop..." << endl;

  if(fVerbose > 2) cout << "--- No of outer iterations to go: " << clTracklets14.size() << endl;
  //  Int_t vSize = Int_t(clTracklets14.size());
  //for(int i = 0; i < vSize; i++) {
  //  if( (clTracklets14[i])->GetVal() == 3) licz++;
  //   }
  //   cout << "* No of 3 in vector: " << licz << endl;

  Int_t trackArrayInd = fArrayTrdTrack->GetEntriesFast();
  Int_t trlsNo[] = {0,0,0};

  for(itclSeg3 = clTracklets14.begin();
      itclSeg3 != clTracklets14.end();
      itclSeg3++) { //loop over tracklets with value of 3
    if((*itclSeg3)->GetVal() != 3) {
      trlsNo[0]++;
      continue;
    }
    for(int i = 0; i < 12; i++) tempTrack.M[i] = 0;

    tempTrack.M[0] = (*itclSeg3)->GetInd(0);
    tempTrack.M[1] = (*itclSeg3)->GetInd(1);
    tempTrack.M[2] = (*itclSeg3)->GetInd(2);
    tempTrack.M[3] = (*itclSeg3)->GetInd(3);

    Int_t iInd2 = 0;
    Int_t size2 = Int_t((*itclSeg3)->vAccostRight.size());
    for(Int_t iSeg2 = 0; iSeg2 < size2; iSeg2++) {//loop over clTracklets with value of 2
      iInd2 = (*itclSeg3)->vAccostRight[iSeg2];
      clTrdSeg2 = clTracklets58[iInd2];
      if(clTrdSeg2->GetVal() != 2) continue;

      tempTrack.M[4] = clTrdSeg2->GetInd(0);
      tempTrack.M[5] = clTrdSeg2->GetInd(1);
      tempTrack.M[6] = clTrdSeg2->GetInd(2);
      tempTrack.M[7] = clTrdSeg2->GetInd(3);

      Int_t iInd1 = 0;
      Int_t size1 = Int_t(clTrdSeg2->vAccostRight.size());
      for(Int_t iSeg1 = 0; iSeg1 < size1; iSeg1++) {//loop over clTracklets with value of 1
	iInd1 = clTrdSeg2->vAccostRight[iSeg1];
	clTrdSeg1 = clTracklets912[iInd1];
	if(clTrdSeg1->GetVal() != 1) continue;

	tempTrack.M[8]  = clTrdSeg1->GetInd(0);
	tempTrack.M[9]  = clTrdSeg1->GetInd(1);
	tempTrack.M[10] = clTrdSeg1->GetInd(2);
	tempTrack.M[11] = clTrdSeg1->GetInd(3);

	counter++;
	isEmpty = false;

	/*
	//-------------------------------------
	tr2 = new CbmTrdTrack();
	tr2->SetParamFirst(*trParam);
	tr2->SetParamLast(*trParam);

	for(Int_t we = 0; we < 12; we++) {
	hit1 = (CbmTrdHit*)fArrayTrdHit->At(tempTrack.M[we]);
	tr2->AddHit(tempTrack.M[we], hit1);
	}
	tr2->SortHits();
	FitKF(tr2);
	//	  trdTrackFitterKF->DoFit(tr2);
	tempTrack.Chi2 = tr2->GetChi2();
	delete tr2;
	//-------------------------------------	  */

	//tempTrack.Chi2 = FitTLinearFitter(tempTrack.M);
	//tempTrack.Chi2 = FitLinear(tempTrack.M, 1);
	//tempTrack.Chi2 = FitLSM(tempTrack.M);
	tempTrack.Chi2 = Fit(tempTrack.M);

	auxTrack.push_back(tempTrack);

	//if((counter%1000) == 0) {
	//printf("\rTracks: %i",counter);
	//fflush(stdout);
	//}
      }//end::loop 1
    }//end::loop 2

    //if(iSecondLoop%2 == 0)
    {
      iSecondLoop = 0;
      sort(auxTrack.begin(), auxTrack.end(), CompareChi2);
      //if(nrLoop == 1) trMax = 1000;
      //if(nrLoop == 2) trMax = 1000;
      //	if(nrLoop == 1) cout <<"auxTrack.size(): "<< auxTrack.size() << endl;
      Int_t li = 0;
      //CbmTrdTrack *T3;
      //Double_t chiMax = 1000;

      for(ikol = auxTrack.begin();
	  ikol != auxTrack.end();
	  ikol++) {
	if(li >= trMax) break;

	tr1 = new CbmTrdTrack();

	for(Int_t we = 0; we < 12; we++) {
	  trdInd = (*ikol).M[we];
	  hit1 = (CbmTrdHit*)fArrayTrdHit->At(trdInd);

	  tr1->AddHit(trdInd, kTRDHIT);
	}
//	tr1->SortHits();
	//tr1->SetChi2((*ikol).Chi2);
	//cout << tempTrack.Chi2 << endl;

	/*// Set reliable Q/p value. Based on MC values ----------------
	  CbmTrdPoint *pMCpt;
	  CbmMCTrack *mcTr;
	  Int_t ptIndex;
	  Double_t mom;
	  trdInd = tr1->GetTrdHitIndex(0);
	  hit1 = (CbmTrdHit*)fArrayTrdHit->At(trdInd);
	  ptIndex = hit1->GetRefIndex();
	  pMCpt = (CbmTrdPoint*) (fMCPointArray->At(ptIndex));

	  mcTr = (CbmMCTrack*)fMCTrackArray->At(pMCpt->GetTrackID());
	  mom = mcTr->GetMomentum().Mag();
	  Int_t pdgCode = mcTr->GetPdgCode();
	  Int_t charge = PdgToCharge(pdgCode);
	  trParam->SetQp(charge/mom);
	  //trParam->SetQp(1.);
	  //cout <<"(pdg, Qp) "<< pdgCode <<", "<< charge/mom << endl;
	  //-------------------------------------------------------------
	  */

	tr1->SetParamFirst(trParam);
	tr1->SetParamLast(trParam);

	//FitKF(tr1);
	//  if(tr1->GetChi2() < chiMax){
	// T3 = tr1;
	//  chiMax = tr1->GetChi2();
	//}
	//if((*ikol).Chi2 < 3)
	vTempTrdTrackCand.push_back(tr1);
	li++;
      }
      //vTempTrdTrackCand.push_back(T3);
      auxTrack.clear();
      //      cout << "------------------"<< endl;
    }
    //cout << iSecondLoop << endl;
    iSecondLoop++;
  }//loop over 3s

  //cout << clTracklets14.size() <<"\t"
  //     << trlsNo[0] << endl;

  //  if(nrLoop == 2) competition = false;
  if(competition) {
    refittingKF.Start();
    cout << "\n--- Refiting by KF..."<< endl;
    /* Refit using KF fitter                                                     */
    //Int_t iKlops = 0;
    //cout << "vTempTrdTrackCand.size(): " << vTempTrdTrackCand.size() << endl;
    for(ivTempTrdTrackCand = vTempTrdTrackCand.begin();
	ivTempTrdTrackCand != vTempTrdTrackCand.end();
	ivTempTrdTrackCand++) {

      //FitKF(*ivTempTrdTrackCand);
      //trdTrackFitterKF->DoFit(*ivTempTrdTrackCand, &vtx);
      trdTrackFitterKF->DoFit(*ivTempTrdTrackCand);
      //       Double_t mX = (*ivTempTrdTrackCand)->GetParamLast()->GetX();
      //       Double_t mY = (*ivTempTrdTrackCand)->GetParamLast()->GetY();
      //       Double_t mXY = sqrt(pow(mX,2)+pow(mY,2));
      //       Double_t mChi2 = (*ivTempTrdTrackCand)->GetChi2();
      //       (*ivTempTrdTrackCand)->SetChi2(mChi2*mX2);
      //cout <<"Chi2: "<< (*ivTempTrdTrackCand)->GetChi2() << endl;
      //iKlops++;
      //cout << iKlops <<": GetNofTrdHits(): " << (*ivTempTrdTrackCand)->GetNofTrdHits() << endl;
    }
    refittingKF.Stop();

    sort(vTempTrdTrackCand.begin(),vTempTrdTrackCand.end(),CompareChi2TrdTrack);
  }

  CbmTrdTrack *trCand;
  Int_t firstHitSunk = 0;
  for(ivTrdTrackCand = vTempTrdTrackCand.begin();
      ivTrdTrackCand != vTempTrdTrackCand.end();
      ivTrdTrackCand++) { //Loop over track candidates; mark used hits
    trCand = (*ivTrdTrackCand);
    //cout << "Track no " << trackInd++
    //	 << " has " << (*ivTrdTrackCand)->GetNofTrdHits()
    //	 << " hits and chi2= " << (*ivTrdTrackCand)->GetChi2() << endl;

    // if(nrLoop == 2) removeUsedHits = false;

    if(removeUsedHits) {//RemoveHits
      Int_t noHitsA = trCand->GetNofHits();

      bTrueTrack = true;
      firstHitSunk = 0;

      for(int i = 0; i < noHitsA; i++) {
	iHit = trCand->GetHitIndex(i);
	iglobalSetUsedHits = globalSetUsedHits.find(iHit);

	if(iglobalSetUsedHits != globalSetUsedHits.end()) {
	  if(firstHitSunk == noHitsAccepted) {
	    bTrueTrack = false;
	    iFakeTrack++;
	    break;
	  }
	  firstHitSunk++;
	}
      }

      //       Int_t k = 0;
      //       for(int i = 0; i < noHitsA; i++) {
      // 	if(i != 11) k = SA[i]+SA[i+1];
      // 	//	if(i != 10) k = SA[i]+SA[i+1]+SA[i+2];
      // 	if(k == noHitsAccepted){
      // 	    bTrueTrack = false;
      // 	    iFakeTrack++;
      // 	    break;
      // 	  }
      //       }

      //if(firstHitSunk != 0) cout << firstHitSunk << endl;
      if(bTrueTrack) {
	new ((*fArrayTrdTrack)[trackArrayInd]) CbmTrdTrack(*trCand);
	trackArrayInd++;
	for(int i = 0; i < noHitsA; i++){
	  iHit = trCand->GetHitIndex(i);
	  globalSetUsedHits.insert(iHit);
	}
	delete trCand;
      }

    }else{ // end of RemoveHits
      new ((*fArrayTrdTrack)[trackArrayInd]) CbmTrdTrack(*trCand);
      trackArrayInd++;
      delete trCand;
    }
  }

  cout << "\n--- Finding tracks finished"<< endl;
  cout << ":::Track candidates: " << vTempTrdTrackCand.size() << endl
       << ":::Fake tracks:      " << iFakeTrack << endl
       << ":::fArrayTrdTrack:   " << fArrayTrdTrack->GetEntriesFast() << endl;

  vTempTrdTrackCand.clear();
  //  delete trParam;

  if(!isEmpty) { licz++; isEmpty = true; }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1CATrdTrackFinderSA::CreateSegments(vector<CbmL1TrdTracklet*> clSpacePointsAB,
					     vector<CbmL1TrdTracklet*> clSpacePointsCD,
					     vector<CbmL1TrdTracklet4*> &clTrackletsAD,
					     Double_t dX, Double_t dY) {
  //--- method to create 4-hit tracklets from Space Points (smaller 2-hit tracklets) ---
  vector<CbmL1TrdTracklet*>::iterator iSpacePtA;
  vector<CbmL1TrdTracklet*>::iterator iSpacePtB;
  vector<CbmL1TrdTracklet*>::iterator iSpacePtStart;

  Int_t
    iIndex = 0,       //index
    indSpacePtA,       //index of 1st miniTracklet
    indSpacePtB,       //index of 2nd miniTracklet
    indSpacePtC,       //index of 3rd miniTracklet
    indSpacePtD,       //index of 4th miniTracklet
    noXSurv = 0,      //number of tracklets that survived the X distance cut
    noYSurv = 0,      //number of tracklets that survived the Y distance cut
    noXYSurv = 0,     //number of tracklets that survived both X & Y distance cuts
    noAllPairs = 0,
    iSegAcc14 = 0;

  CbmL1TrdTracklet4 *clTr;

  CbmL1TrdTracklet
    *trLeft,
    *trRight;

  iSpacePtStart  = clSpacePointsCD.begin();

  //---------- BEGIN: Creation tracklet 12-34 ---------------------------------
  for(iSpacePtA  = clSpacePointsAB.begin();
      iSpacePtA != clSpacePointsAB.end();
      iSpacePtA++) {
    trLeft = *iSpacePtA;
    indSpacePtA = trLeft->GetIndLeft();
    indSpacePtB = trLeft->GetIndRight();

    //    cout << "GetCoord1: " << trLeft->GetCoord1() << endl;
    //cout << "--------------------" << clSpacePointsCD.size() << endl;


    //--- extrapolate --------------------------------------------
    Double_t
      y1  = trLeft->GetCoord(0),
      x1  = trLeft->GetCoord(1),
      y1z = trLeft->GetCoord(2),
      x1z = trLeft->GetCoord(3),
      x2  = 0,
      y2  = 0,
      distBetweenLayer = x1z-y1z,
      y2z = y1z+distBetweenLayer*2,
      x2z = x1z+distBetweenLayer*2;

    y2 = (y1/y1z)*y2z;
    x2 = (x1/x1z)*x2z;
    //--- end: extrapolate --------------------------------------

    Bool_t bFirst = true;

    //for(y = 0; y < trsize; y++){//y
    //trRight = clSpacePointsCD[y];
    //cout << trRight->GetCoord(0) << endl;
    //    Bool_t bFirst = true;
    for(iSpacePtB  = iSpacePtStart;
	iSpacePtB != clSpacePointsCD.end();
	iSpacePtB++) {
      trRight = *iSpacePtB;

      indSpacePtC = trRight->GetIndLeft();
      indSpacePtD = trRight->GetIndRight();
      noAllPairs++;

      Double_t
	yB  = trRight->GetCoord(0),
	xB  = trRight->GetCoord(1),
	yBz = trRight->GetCoord(2),
	xBz = trRight->GetCoord(3);

      /*
      //--- for estimation dx and dy ----------------------------
      trdHit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtA);
      Int_t refInd1 = trdHit1->GetRefIndex();
      trdHit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtB);
      Int_t refInd2 = trdHit1->GetRefIndex();

      trdHit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtC);
      Int_t refInd3 = trdHit1->GetRefIndex();
      trdHit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtD);
      Int_t refInd4 = trdHit1->GetRefIndex();

      CbmTrdPoint
      *pPointAy,
      *pPointBx,
      *pPointCy,
      *pPointDx;

      pPointAy = (CbmTrdPoint*) fMCPointArray->At(refInd1);
      pPointBx = (CbmTrdPoint*) fMCPointArray->At(refInd2);
      pPointCy = (CbmTrdPoint*) fMCPointArray->At(refInd3);
      pPointDx = (CbmTrdPoint*) fMCPointArray->At(refInd4);

      Int_t trIDAy = pPointAy->GetTrackID();
      Int_t trIDBx = pPointBx->GetTrackID();
      Int_t trIDCy = pPointCy->GetTrackID();
      Int_t trIDDx = pPointDx->GetTrackID();

      CbmMCTrack *pMCtrack;
      Int_t noTRDPoints;

      Double_t
      x1  = pPointBx->GetX(),
      y1  = pPointAy->GetY(),
      z1y = pPointAy->GetZ(),
      z1x = pPointBx->GetZ(),
      x2  = 0,
      y2  = 0,
      z2y = z1y+distBetweenLayer*2,
      z2x = z1x+distBetweenLayer*2;

      y2 = (y1/z1y)*z2y;
      x2 = (x1/z1x)*z2x;

      if(trIDAy == trIDCy){
      pMCtrack = (CbmMCTrack*) fMCTrackArray->At(trIDAy);
      noTRDPoints = pMCtrack->GetTrdPoints();

      if(noTRDPoints == 12){
      Double_t fPosY = pPointCy->GetY();
      fDistY->Fill(fabs(fPosY-y2));
      }
      }

      if(trIDBx == trIDDx){
      pMCtrack = (CbmMCTrack*) fMCTrackArray->At(trIDBx);
      noTRDPoints = pMCtrack->GetTrdPoints();

      if(noTRDPoints == 12){
      Double_t fPosX = pPointDx->GetX();
      fDistX->Fill(fabs(fPosX-x2));
      }
      }
      //--- end: for estimation dx and dy -------------------------------
      */

      if(trRight->GetCoord(0) > y2+dY){
	continue;
      }else{
	if(bFirst){
	  bFirst = false;
	  iSpacePtStart = iSpacePtB;
	}
      }
      if(trRight->GetCoord(0) < y2-dY) break;


      if((trRight->GetCoord(1) < x2+dX) &&
      	 (trRight->GetCoord(1) > x2-dX)){

	iSegAcc14++;  //we have one more tracklet
	//we create a new tracklet object to add it into a vector
	clTr = new CbmL1TrdTracklet4();

  	clTr->SetCoord(0, y1);
  	clTr->SetCoord(1, x1);
  	clTr->SetCoord(2, yB);
  	clTr->SetCoord(3, xB);

	clTr->M[0] = y1z;
	clTr->M[1] = x1z;
	clTr->M[2] = yBz;
	clTr->M[3] = xBz;

	//zed is a z-axis value to which we extrapolate
	Double_t zed = 0;

	//cout << trdHit1->GetZ() <<" : "<< trRight->GetCoord(3) << endl;
	if(Int_t(trRight->GetCoord(3)+1) == fTrd14_Z){
	  zed = fTrd21_Z;
	  //cout <<"fTrd21_Z: "<< fTrd21_Z << endl;
	}
	//cout << trRight->GetCoord(3) <<"\t"<< fTrd14_Z << endl;
	if(Int_t(trRight->GetCoord(3)+1) == fTrd24_Z){
	  zed = fTrd31_Z;
	  //cout <<"fTrd31_Z: "<< fTrd31_Z << endl;
	}
	//cout << trRight->GetCoord(3) <<"\t"<< fTrd24_Z << endl;

	//if(trRight->GetPlanesID(1) == 4) zed = fTrd21_Z;
	//if(trRight->GetPlanesID(1) == 8) zed = fTrd31_Z;
	//	if(trRight->GetPlanesID(1) == 4) zed = 970;
	//if(trRight->GetPlanesID(1) == 8) zed = fTrd31_Z;

	/* //Extrapolated by CbmTrdTrackFitterKF::Extrapolate insteas fo DistTwoTrackletsY
	   //Extrapolate && fo DistTwoTrackletsY give the same results
	   CbmTrdTrack *trdTrack = new CbmTrdTrack();
	   CbmTrdHit *hit1;

	   hit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtA);
	   trdTrack->AddHit(indSpacePtA, hit1);
	   hit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtB);
	   trdTrack->AddHit(indSpacePtB, hit1);
	   hit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtC);
	   trdTrack->AddHit(indSpacePtC, hit1);
	   hit1 = (CbmTrdHit*)fArrayTrdHit->At(indSpacePtD);
	   trdTrack->AddHit(indSpacePtD, hit1);

	   trdTrack->SortHits();

	   FairTrackParam *trParam;
	   trParam = new FairTrackParam();
	   trParam->SetQp(0.);
	   trdTrack->SetParamFirst(*trParam);
	   trdTrack->SetParamLast(*trParam);

	   trdTrackFitterKF->DoFit(trdTrack);

	   //FairTrackParam *e_track = new FairTrackParam();
	   trdTrackFitterKF->Extrapolate(trdTrack, zed, trParam);

	   clTr->SetExt(0, trParam->GetY());
	   clTr->SetExt(1, trParam->GetX());

	   cout <<"(MY, KF) "
	   << DistTwoTrackletsY(indSpacePtB, indSpacePtD, zed) <<", "
	   << trParam->GetX() << endl;
	*/

	//set the extrapolated coordinates - Y
	clTr->SetExt(0, DistTwoTrackletsY(indSpacePtA, indSpacePtC, zed));
	//set the extrapolated coordinates - X
	clTr->SetExt(1, DistTwoTrackletsX(indSpacePtB, indSpacePtD, zed+distBetweenLayer));
	//set the indices of all 4 point in tracklet
	clTr->SetInd(0, indSpacePtA);
	clTr->SetInd(1, indSpacePtB);
	clTr->SetInd(2, indSpacePtC);
	clTr->SetInd(3, indSpacePtD);
	//initial value is always 0; it is modified if necessary
	clTr->SetVal(0);

	//position of the tracklet in tracklets vector
	clTr->SetIndex(iIndex);
	iIndex++;

	//add a tracklet to a vector
	clTrackletsAD.push_back(clTr);
	noXYSurv++;
      }
    }
  }
  if(fVerbose > 2) {
    cout << " ----------- Tracklet 12-34 ------------------" << endl;
    cout << " Number of X survivors: " << noXSurv << endl;
    cout << " Number of Y survivors: " << noYSurv << endl;
    cout << "Number of XY survivors: " << noXYSurv << endl;
    cout << "   Number of all pairs: " << noAllPairs << endl;
  }
}
// -----------------------------------------------------------------------

void CbmL1CATrdTrackFinderSA::CreateSpacePoints(vector<LayerWithHits> vTrdHitArrayA,
						vector<LayerWithHits> vTrdHitArrayB,
						vector<CbmL1TrdTracklet*> &clSpacePointsAB,
						Double_t sigmaA, Double_t sigmaB) {
  //method to create Space Points (SP) from two individual detector hist

  vector<LayerWithHits>::iterator itA;
  vector<LayerWithHits>::iterator itB;
  vector<LayerWithHits>::iterator itStart;

  Int_t
    noOverlapsAB = 0,
    //    noMCOverlapsAB = 0,
    iInd = 0;

  CbmL1TrdTracklet *clSpacePt;

  Int_t
    indA, indB;

  Double_t
    A_X, A_Y, A_Z,
    A_DX, A_DY;

  Double_t
    B_X, B_Y, B_Z,
    B_DX, B_DY;

  Int_t
    A_planeID,
    B_planeID;

  Int_t
    A_mcTrID,
    B_mcTrID;

  //   Double_t SPlength;


  /*
    68,0%; <= 1sigma
    95,5%; <= 2sigma
    99,7%, <= 3sigma
  */

  itStart = vTrdHitArrayB.begin();

  for(itA  = vTrdHitArrayA.begin();
      itA != vTrdHitArrayA.end();
      itA++) {

    indA = (*itA).hitIndex;
    A_X  = (*itA).X;
    A_Y  = (*itA).Y;
    A_Z  = (*itA).Z;
    A_DX = (*itA).DX;
    A_DY = (*itA).DY;
    A_planeID = (*itA).planeID;
    A_mcTrID = (*itA).mcTrackID;

    /*//--- extrapolate --------------------------------------------
      Double_t distBetweenLayer = fTrd14_Z-fTrd13_Z;
      cout << "distBetweenLayer: " << distBetweenLayer << endl;

      Double_t
      x1  = A_X,
      y1  = A_Y,
      z1y = A_Z,
      z1x = A_Z,
      x2  = 0,
      y2  = 0,
      z2y = z1y+distBetweenLayer, // 12 is a distance beetwen plane in TRD
      z2x = z1x+distBetweenLayer; // 12 is a distance beetwen plane in TRD

      y2 = (y1/z1y)*z2y;
      x2 = (x1/z1x)*z2x;
      //--- end: extrapolate --------------------------------------*/

      Bool_t bFirst = true;
      for(itB  = itStart;
	  itB != vTrdHitArrayB.end();
	  itB++) {

	indB = (*itB).hitIndex;
	B_X  = (*itB).X;
	B_Y  = (*itB).Y;
	B_Z  = (*itB).Z;
	B_DX = (*itB).DX;
	B_DY = (*itB).DY;
	B_planeID = (*itB).planeID;
	B_mcTrID = (*itB).mcTrackID;

	if (B_Y + sigmaB*B_DY < A_Y - sigmaA*A_DY){
	  continue;
	}else{
	  /* Finding the bottom level to begin the second loop.
	     The value of bottom level is taken from previous "second loop".*/
	  if (bFirst){
	    itStart = itB;
	    bFirst = false;
	  }
	}

	if (B_Y - sigmaB*B_DY > A_Y + sigmaA*A_DY) break;

	// cout <<"(A_X, A_DX, sigmaA*A_DX) "<< A_X <<", "<< A_DX <<", "<< sigmaA*A_DX <<", "<< endl;
	// cout <<"(A_Y, A_DY, sigmaA*A_DY) "<< A_Y <<", "<< A_DY <<", "<< sigmaA*A_DY <<", "<< endl;
	// cout <<"(B_X, B_DX, sigmaB*B_DX) "<< B_X <<", "<< B_DX <<", "<< sigmaB*B_DX <<", "<< endl;
	// cout <<"(B_Y, B_DY, sigmaB*B_DY) "<< B_Y <<", "<< B_DY <<", "<< sigmaB*B_DY <<", "<< endl;
	// cout <<"-----------------------------------------"<< endl;
	//      if (B_Y < y2 - sigmaB*A_DY) continue;
	//      if (B_Y > y2 + sigmaB*A_DY) break;
	//      cout <<"[Extrapol, Real, Diff] "
	//   << y2 <<", "<< B_Y <<", "<< B_Y-y2 << endl;

	if((B_X - sigmaB*B_DX < A_X + sigmaA*A_DX) &&
	   (B_X + sigmaB*B_DX > A_X - sigmaA*A_DX)){

	  noOverlapsAB++;
	  clSpacePt = new CbmL1TrdTracklet();

	  clSpacePt->SetIndLeft(indA);
	  clSpacePt->SetIndRight(indB);
	  clSpacePt->SetVal(0);
	  clSpacePt->SetIndex(iInd);
	  clSpacePt->SetCoord(0, A_Y);
	  clSpacePt->SetCoord(1, B_X);
	  clSpacePt->SetCoord(2, A_Z);
	  clSpacePt->SetCoord(3, B_Z);
	  clSpacePt->SetPlanesID(A_planeID, B_planeID);

	  // 	//----------------------------------------
	  // 	TVector3 t1, t2, t3;
	  // 	t1.SetXYZ(A_X, A_Y, A_Z);
	  // 	t2.SetXYZ(B_X, B_Y, B_Z);
	  // 	t3 = t2 - t1;
	  // 	SPlength = t3.Mag();

	  // 	Double_t
	  // 	  x = 0,
	  // 	  y = 0,
	  // 	  z = 0,
	  // 	  t = 0;

	  // 	 t = (z - A_Z)/(B_Z - A_Z);
	  // 	 x = A_X + t*(B_X-A_X);
	  // 	 y = A_Y + t*(B_Y-A_Y);

	  // 	//SPlength = sqrt(pow(B_Y-A_Y,2)+pow(B_Y-A_Y,2));
	  // 	 if(Station == 1){
	  // 	   if(A_mcTrID == B_mcTrID){
	  // 	     noMCOverlapsAB++;
	  // 	     fSPlengthMC->Fill(SPlength);
	  // 	     fYat0MC->Fill(x);
	  // 	   }else{
	  // 	     fSPlength->Fill(SPlength);
	  // 	     fYat0->Fill(x);
	  // 	   }
	  // 	 }
	  iInd++;

	  clSpacePointsAB.push_back(clSpacePt);

	  if(A_mcTrID == B_mcTrID){
	    fh_SP_xDiff_MC->Fill(A_X-B_X);
	    fh_SP_yDiff_MC->Fill(A_Y-B_Y);
	  }else{
	    fh_SP_xDiff_nMC->Fill(A_X-B_X);
	    fh_SP_yDiff_nMC->Fill(A_Y-B_Y);
	  }
	}
      }
  }
  iInd = 0;
  if(fVerbose > 1) cout <<"--- No Space Points: "<< noOverlapsAB << endl;
  //   cout <<"--- (No SP, MCSP) "
  //        << noOverlapsAB <<", "
  //        << noMCOverlapsAB <<endl;
}


// -----------------------------------------------------------------------
void CbmL1CATrdTrackFinderSA::CreateAndManageSegments(vector<CbmL1TrdTracklet4*> clTracklets14,
						      vector<CbmL1TrdTracklet4*> clTracklets58,
						      vector<CbmL1TrdTracklet4*> clTracklets912){
  //CreateAndManageSegments with no long track creation, 1 track = 1 segment

  vector<CbmL1TrdTracklet4*>::iterator itclSeg;

  CbmTrdTrack *tr1 = NULL;
  CbmTrdHit* hit1 = NULL;
  CbmTrdHit* hit2 = NULL;
  CbmTrdHit* hit3 = NULL;
  CbmTrdHit* hit4 = NULL;

  CbmL1TrdTracklet4
    *clTrdSeg;

  Int_t
    trackArrayInd = 0;

  Int_t
    indTrdHit1,
    indTrdHit2,
    indTrdHit3,
    indTrdHit4;

  for(itclSeg = clTracklets14.begin();
      itclSeg != clTracklets14.end();
      itclSeg++) {

    clTrdSeg = *itclSeg;
    tr1 = new CbmTrdTrack();

    indTrdHit1 = clTrdSeg->GetInd(0);
    hit1 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit1);
    tr1->AddHit(indTrdHit1, kTRDHIT);

    indTrdHit2 = clTrdSeg->GetInd(1);
    hit2 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit2);
    tr1->AddHit(indTrdHit2, kTRDHIT);

    indTrdHit3 = clTrdSeg->GetInd(2);
    hit3 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit3);
    tr1->AddHit(indTrdHit3, kTRDHIT);

    indTrdHit4 = clTrdSeg->GetInd(3);
    hit4 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit4);
    tr1->AddHit(indTrdHit4, kTRDHIT);

//    tr1->SortHits();

    new ((*fArrayTrdTrack)[trackArrayInd]) CbmTrdTrack(*tr1);

    delete tr1;
    trackArrayInd++;
  }

  for(itclSeg = clTracklets58.begin();
      itclSeg != clTracklets58.end();
      itclSeg++) {

    clTrdSeg = *itclSeg;
    tr1 = new CbmTrdTrack();

    indTrdHit1 = clTrdSeg->GetInd(0);
    hit1 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit1);
    tr1->AddHit(indTrdHit1, kTRDHIT);

    indTrdHit2 = clTrdSeg->GetInd(1);
    hit2 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit2);
    tr1->AddHit(indTrdHit2, kTRDHIT);

    indTrdHit3 = clTrdSeg->GetInd(2);
    hit3 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit3);
    tr1->AddHit(indTrdHit3, kTRDHIT);

    indTrdHit4 = clTrdSeg->GetInd(3);
    hit4 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit4);
    tr1->AddHit(indTrdHit4, kTRDHIT);

//    tr1->SortHits();

    new ((*fArrayTrdTrack)[trackArrayInd]) CbmTrdTrack(*tr1);

    delete tr1;
    trackArrayInd++;
  }

  for(itclSeg = clTracklets912.begin();
      itclSeg != clTracklets912.end();
      itclSeg++) {

    clTrdSeg = *itclSeg;
    tr1 = new CbmTrdTrack();

    indTrdHit1 = clTrdSeg->GetInd(0);
    hit1 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit1);
    tr1->AddHit(indTrdHit1, kTRDHIT);

    indTrdHit2 = clTrdSeg->GetInd(1);
    hit2 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit2);
    tr1->AddHit(indTrdHit2, kTRDHIT);

    indTrdHit3 = clTrdSeg->GetInd(2);
    hit3 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit3);
    tr1->AddHit(indTrdHit3, kTRDHIT);

    indTrdHit4 = clTrdSeg->GetInd(3);
    hit4 = (CbmTrdHit*)fArrayTrdHit->At(indTrdHit4);
    tr1->AddHit(indTrdHit4, kTRDHIT);

//    tr1->SortHits();

    new ((*fArrayTrdTrack)[trackArrayInd]) CbmTrdTrack(*tr1);

    delete tr1;
    trackArrayInd++;
  }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
Double_t CbmL1CATrdTrackFinderSA::Fit(CbmTrdTrack *tr)
{//fits a track with a straight line and caluculates each point's deviation

  Double_t chi2 = 0;

  Int_t iHit;
  CbmTrdHit *pHit[12];

  Double_t
    y1,y2,
    z1,z2,
    yA[12], zA[12],
    yB[12], zB[12];

  Double_t dist1 = 0;
  Double_t dist2 = 0;
  //  Double_t sum1 = 0;
  //  Double_t sum2 = 0;
  Double_t yS1 = 0;
  Double_t yS2 = 0;

  Int_t noHits = tr->GetNofHits();

  for(int i = 0; i < noHits; i++){
    iHit = tr->GetHitIndex(i);

    pHit[i]  = (CbmTrdHit*) fArrayTrdHit->At(iHit);
  }

  //first and last X(orY) pointa in the track
  yA[0]  = pHit[0]->GetY();
  zA[0]  = pHit[0]->GetZ();
  yB[10] = pHit[10]->GetY();
  zB[10] = pHit[10]->GetZ();

  //first and last Y(orX) pointa in the track
  yA[1]  = pHit[1]->GetX();
  zA[1]  = pHit[1]->GetZ();
  yB[11] = pHit[11]->GetX();
  zB[11] = pHit[11]->GetZ();

  Int_t t1 = 0;
  Int_t t2 = 1;

  for(int i = 0; i < 4; i++){

    //for X(orY) coordinates
    t1+=2;
    y1 = fabs(pHit[t1]->GetY());
    z1 = pHit[t1]->GetZ();

    yS1 = fabs((((yB[10]-yA[0])*(z1-zA[0]))/(zB[10]-zA[0]))+yA[0]);
    dist1 = fabs(yS1-y1);

    //for Y(orX) coordinates
    t2+=2;
    y2 = fabs(pHit[t2]->GetX());
    z2 = pHit[t2]->GetZ();

    yS2 = fabs((((yB[11]-yA[1])*(z2-zA[1]))/(zB[11]-zA[1]))+yA[1]);
    dist2 = fabs(yS2-y2);

    chi2 += (dist1+dist2)/2;
  }

  //  cout << "chi2: " << chi2 << endl;


  //   for(int i = 0; i < 4; i++){

  //     //for X(orY) coordinates
  //     t1+=2;
  //     y1 = fabs(pHit[t1]->Y());
  //     z1 = pHit[t1]->Z();

  //     yS1 = fabs((((yB[10]-yA[0])*(z1-zA[0]))/(zB[10]-zA[0]))+yA[0]);
  //     dist1 = fabs(yS1-y1);
  //     sum1+=dist1;

  //     //for Y(orX) coordinates
  //     t2+=2;
  //     y2 = fabs(pHit[t2]->X());
  //     z2 = pHit[t2]->Z();

  //     yS2 = fabs((((yB[11]-yA[1])*(z2-zA[1]))/(zB[11]-zA[1]))+yA[1]);
  //     dist2 = fabs(yS2-y2);
  //     sum2+=dist2;
  //   }
  //  chi2=sum1+sum2;



  // Calculate length of the track ---------------
  //   Double_t
  //     length0 = 0,
  //     length1 = 0,
  //     length2 = 0;

  //   t1 = 0;
  //   t2 = 1;

  //   Double_t
  //     x = 0,
  //     y = 0,
  //     z = 0;

  //   for(int i = 0; i < 4; i++){
  //     y = fabs(pHit[t1+2]->Y())-fabs(pHit[t1]->Y());
  //     z = (pHit[t1+2]->Z())-(pHit[t1]->Z());
  //     length1 += sqrt(pow(y,2) + pow(z,2));
  //     t1+=2;

  //     x = fabs(pHit[t2+2]->X())-fabs(pHit[t2]->X());
  //     z = (pHit[t2+2]->Z())-(pHit[t2]->Z());
  //     length2 += sqrt(pow(x,2) + pow(z,2));
  //     t2+=2;
  //   }

  //   width0 = width1 + width2;
  //   //  cout << width0 << endl;

  return chi2;
  //return length0;
}
// -----------------------------------------------------------------------

Double_t CbmL1CATrdTrackFinderSA::Fit(Int_t M[])
{//fits a track with a straight line and caluculates each point's deviation

  Double_t chi2 = 0;

  Int_t iHit;
  CbmTrdHit *pHit[12];

  Double_t
    y1,y2,
    z1,z2,
    yA[12], zA[12],
    yB[12], zB[12];

  Double_t dist1 = 0;
  Double_t dist2 = 0;
  Double_t yS1 = 0;
  Double_t yS2 = 0;

  Int_t noHits = 12;

  for(int i = 0; i < noHits; i++){
    iHit = M[i];
    pHit[i]  = (CbmTrdHit*) fArrayTrdHit->At(iHit);
  }

  //first and last X(orY) pointa in the track
  yA[0]  = pHit[0]->GetY();
  zA[0]  = pHit[0]->GetZ();
  yB[10] = pHit[10]->GetY();
  zB[10] = pHit[10]->GetZ();

  //first and last Y(orX) pointa in the track
  yA[1]  = pHit[1]->GetX();
  zA[1]  = pHit[1]->GetZ();
  yB[11] = pHit[11]->GetX();
  zB[11] = pHit[11]->GetZ();

  Int_t t1 = 0;
  Int_t t2 = 1;

  for(int i = 0; i < 4; i++){

    //for X(orY) coordinates
    t1+=2;
    y1 = fabs(pHit[t1]->GetY());
    z1 = pHit[t1]->GetZ();

    yS1 = fabs((((yB[10]-yA[0])*(z1-zA[0]))/(zB[10]-zA[0]))+yA[0]);
    dist1 = fabs(yS1-y1);

    //for Y(orX) coordinates
    t2+=2;
    y2 = fabs(pHit[t2]->GetX());
    z2 = pHit[t2]->GetZ();

    yS2 = fabs((((yB[11]-yA[1])*(z2-zA[1]))/(zB[11]-zA[1]))+yA[1]);
    dist2 = fabs(yS2-y2);

    //chi2 += (dist2+dist1)/2;
    chi2 += sqrt(pow(dist2,2)+pow(dist1,2));
  }
  return chi2;
}
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
Double_t CbmL1CATrdTrackFinderSA::FitLinear(Int_t M[],
					    Int_t var = 1) {
  //fit using a least square method


  Int_t noHits = 12;

  Int_t iHit;
  CbmTrdHit *hit;

  Double_t C1[12];
  Double_t C2[12];
  Double_t  Z[12];

  Double_t n = 12;

  for(int i=0;i<12;i++) {
    C1[i] = 0;
    C2[i] = 0;
    Z [i] = 0;
  }

  //  Int_t ind = 0;

  for(int i=0;i<noHits;i++) {

    iHit = M[i];
    hit = (CbmTrdHit*)fArrayTrdHit->At(iHit);

    C1[i] = hit->GetX();
    C2[i] = hit->GetY();
    Z[i]  = hit->GetZ();
  }

  Double_t
    sumXZ = 0,
    sumYZ = 0,
    sumX  = 0,
    sumY  = 0,
    sumZx = 0,
    sumZy = 0,
    sumX2 = 0,
    sumY2 = 0,
    sumZx2 = 0,
    sumZy2 = 0;

  for(int i=0;i<12;i+=2) {
    //    cout << "C1[" << i << "]=" << C1[i] << endl;
    //    cout << "C2[" << i << "]=" << C2[i] << endl;
    //    if(!(i % 2))
    {
      sumXZ += C1[i]*Z[ i];

      sumX  += C1[i];
      sumX2 += C1[i]*C1[i];

      sumZx  += Z[i];
      sumZx2 += Z[i]*Z[ i];
    }
  }

  for(int i=1;i<12;i+=2) {
    //    cout << "C1[" << i << "]=" << C1[i] << endl;
    //    cout << "C2[" << i << "]=" << C2[i] << endl;
    //    if(!((1i % 2))
    {
      sumYZ += C2[i]*Z[ i];

      sumY  += C2[i];
      sumY2 += C2[i]*C2[i];

      sumZy  += Z[ i];
      sumZy2 += Z[ i]*Z[ i];
    }
  }


  cout << "";
  //   Double_t a = (n*sumXZ - sumX*sumZ)/(n*sumX2 - sumX*sumX);
  //   Double_t b = (sumZ - a*sumX)/n;
  Double_t r1 = (n*sumXZ - sumX*sumZx)/sqrt((n*sumX2-sumX*sumX)*(n*sumZx2-sumZx*sumZx));
  //  Double_t Sa = sqrt((n*(sumY2 -a*sumXZ-b*sumY))/((n-2)*(n*sumX2-sumX*sumX)));
  //  Double_t Sb = Sa*sqrt(sumX2/n);


  Double_t r2 = (n*sumYZ - sumY*sumZy)/sqrt((n*sumY2-sumY*sumY)*(n*sumZy2-sumZy*sumZy));


  //   cout << "Linear coefficients: a= "
  //        << a << ", Sa= " << Sa << ", b= " << b << ", Sb= " << Sb << endl
  //        << ", r=" << r << ", chi2=" << tr->GetChi2() << endl;


  return sqrt(r1*r1+r2*r2);
}

Double_t CbmL1CATrdTrackFinderSA::FitLSM(Int_t M[]){

  Double_t chi2 = 0;
  Int_t iHit;
  CbmTrdHit *pHit[12];

  Int_t noHits = 12;
  for(int i = 0; i < noHits; i++){
    iHit = M[i];
    pHit[i]  = (CbmTrdHit*) fArrayTrdHit->At(iHit);
  }

  Double_t
    xAv = 0,
    yAv = 0,
    zAvx = 0,
    zAvy = 0,
    sumXiXav = 0,
    sumYiYav = 0,
    sumXiXav_ZiZAv = 0,
    sumYiYav_ZiZAv = 0,
    sumZiZav_x = 0,
    sumZiZav_y = 0,
    bY = 0,
    bX = 0;

  for(int i = 0; i <= 12; i+=2){
    yAv  += pHit[i]->GetY();
    zAvy += pHit[i]->GetZ();
  }
  for(int i = 1; i <= 12; i+=2){
    xAv  += pHit[i]->GetX();
    zAvx += pHit[i]->GetZ();
  }

  for(int i = 0; i < 12; i+=2){
    sumYiYav_ZiZAv += ((pHit[i]->GetY())-yAv)*((pHit[i]->GetZ())-zAvy);
    sumYiYav += pow((pHit[i]->GetY())-yAv,2);
    sumZiZav_y += pow((pHit[i]->GetZ())-zAvy,2);
  }
  for(int i = 1; i < 12; i+=2){
    sumXiXav_ZiZAv += ((pHit[i]->GetX())-xAv)*((pHit[i]->GetZ())-zAvx);
    sumXiXav += pow((pHit[i]->GetX())-xAv,2);
    sumZiZav_x += pow((pHit[i]->GetZ())-zAvx,2);
  }

  bY = sumYiYav_ZiZAv/sumYiYav;
  bX = sumXiXav_ZiZAv/sumXiXav;

  chi2 = sqrt((sumZiZav_y-bY*sumYiYav_ZiZAv)/(4))+sqrt((sumZiZav_x-bX*sumXiXav_ZiZAv)/(4));

  return chi2;
}


Double_t CbmL1CATrdTrackFinderSA::FitKF(CbmTrdTrack* pTrack) {

  // Declare variables outside the loop
  CbmTrdHit* pHit = NULL;
  CbmKFTrdHit* pKFHit = NULL;
  Int_t hitIndex = 0;
  Int_t materialIndex = 0;
  Double_t eLoss = 0.;

  // Loop over TRD hits of the track
  Int_t nTracks = pTrack->GetNofHits();
  for(Int_t iHit = 0; iHit < nTracks; iHit++) {
    // Get current hit index
    hitIndex = pTrack->GetHitIndex(iHit);
    //Get the pointer to the CbmTrdHit
    pHit = (CbmTrdHit*) fArrayTrdHit->At(hitIndex);
    // Accumulate TR energy loss
    eLoss += pHit->GetELoss();
    // Create CbmKFTrdHit
    pKFHit = new CbmKFTrdHit();
    pKFHit->Create(pHit);
    materialIndex = pKFHit->MaterialIndex;
    // Add to the KFTrack
    fKfTrack->fHits.push_back(pKFHit);
  } // Loop over TRD hits

  fKfTrack->GetRefChi2() = 0.;
  fKfTrack->GetRefNDF() = 0;
  fKfTrack->SetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamLast())));

  fKfTrack->Fit();
  // Store parameters at first layer
  fKfTrack->GetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamFirst())));
  // Store parameters at last layer
  fKfTrack->GetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamLast())));
  // Store chi2 of fit
  pTrack->SetChiSq(fKfTrack->GetRefChi2());
  pTrack->SetNDF(fKfTrack->GetRefNDF());

  // Store accumulated TR energy loss
  pTrack->SetELoss(eLoss);

  // Delete CbmKFTrdHit objects
  vector<CbmKFHit*>::iterator it;
  for(it = fKfTrack->fHits.begin(); it != fKfTrack->fHits.end(); it++) {
    pKFHit = (CbmKFTrdHit*) (*it);
    delete pKFHit;
  }
  fKfTrack->fHits.clear();

  return 0;
}

Double_t CbmL1CATrdTrackFinderSA::FitTLinearFitter(Int_t M[]) {

  Double_t chi2  = 0;
  Double_t chi2x = 0;
  Double_t chi2y = 0;
  const Int_t nrPnts = 12;
  Int_t k = 0;
  Int_t w = 0;
  Int_t j = 6;
  Double_t ax[6];
  Double_t ay[6];
  Double_t az1[6];
  Double_t az2[6];

  CbmTrdHit *trdHit;
  for(int i = 0; i < nrPnts; i++) {
    trdHit = (CbmTrdHit*) fArrayTrdHit->At(M[i]);
    k = i+1;
    if((k%2) == 0) {
      ax[w]  = trdHit->GetX();
      az1[w] = trdHit->GetZ();
      w++;
    }else{
      ay[w]  = trdHit->GetY();
      az2[w] = trdHit->GetZ();
    }
  }

  TLinearFitter *lf;
  lf = new TLinearFitter(2);
  lf->SetFormula("hyp2");
  lf->StoreData(0);

  TVectorD x; x.Use(j,ax);
  TVectorD z1; z1.Use(j,az1);
  lf->AssignData(j,1,ax,az1);
  lf->Eval();
  chi2x = lf->GetChisquare();

  delete lf;
  //--------------------------------
  lf = new TLinearFitter(2);
  lf->SetFormula("hyp2");
  lf->StoreData(0);

  TVectorD y; x.Use(j,ay);
  TVectorD z2; z2.Use(j,az1);
  lf->AssignData(j,1,ay,az1);
  lf->Eval();
  chi2y = lf->GetChisquare();

  chi2 = chi2x+chi2y;

  return chi2;
}

Int_t CbmL1CATrdTrackFinderSA::PdgToCharge(Int_t pdgCode) {
  Int_t value;
  switch (pdgCode) {//switch
  case 11: //electron
    {
      value = -1;
      break;
    }
  case -11: //positron
    {
      value = +1;
      break;
    }
  case 13: //muon-
    {
      value = -1;
      break;
    }
  case -13: //muon+
    {
      value = +1;
      break;
    }
  case 22:
    {
      value = 0; //gamma
      break;
    }
  case 211:
    {
      value = +1; //pi+
      break;
    }
  case -211:
    {
      value = -1; //pi-
      break;
    }
  case 111:
    {
      value = 0; //pi0
      break;
    }
  case 213:
    {
      value =  +1; //rho+
      break;
    }
  case -213:
    {
      value = -1; //rho-
      break;
    }
  case 113:
    {
      value = 0; //rho0
      break;
    }
  case 221:
    {
      value = 0; //eta
      break;
    }
  case 323:
    {
      value = 0; //omega
      break;
    }
  case 333:
    {
      value = 0; //phi
      break;
    }
  case 321:
    {
      value = +1; //kaon+
      break;
    }
  case -321:
    {
      value = -1; //kaon-
      break;
    }

  case 443:
    {
      value = 0; //jpsi
      break;
    }
  case 2112:
    {
      value = 0; //neutron
      break;
    }
  case 2212:
    {
      value = +1; //proton
      break;
    }
  case -2212:
    {
      value = 0; //anti-proton (?!)
      break;
    }
  case 3222:
    {
      value = +1; //sigma+
      break;
    }
  case 3112:
    {
      value = -1; //sigma-
      break;
    }
  case 3212:
    {
      value = 0; //sigma0
      break;
    }
  default:
    {
      value = -1000;
      break;
    }
  }
  return value;

}

Bool_t CbmL1CATrdTrackFinderSA::Rejection(Double_t Procent, Int_t num){

  Bool_t accept = true;
  Int_t random;

  random = (rand()%num);

  if(random >= Procent) accept = false;

  return accept;
}




