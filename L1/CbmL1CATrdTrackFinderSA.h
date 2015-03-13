// -----------------------------------------------------------------------
// -----                    CbmL1CATrdTrackFinderSA                  -----
// -----            Created 2/12/2006 by A. Bubak & M. Krauze        -----
// -----------------------------------------------------------------------

/** CbmL1CATrdTrackFinderSA_Hits
 *@author A.Bubak<abubak@nuph.us.edu.pl> & M.Krauze<meszik@nuph.us.edu.pl>
 ** Algorithm for track finding in TRD, based on Cellular Automathon algorithm
 **/


#ifndef CBML1CATRDTRACKFINDERSA_HITS
#define CBML1CATRDTRACKFINDERSA_HITS

#include "CbmTrdTrackFinder.h"

#include "CbmTrdTrack.h"

#include "TStopwatch.h"
#include <vector>
#include <map>
#include <set>

class TClonesArray;
class CbmTrdHit;
class CbmKF;
class CbmKFTrack;
class TH1F;
class TH2F;
class CbmGeoTrdPar;
class CbmTrdTrackFitterKF;
class CbmL1TrdTracklet;
class CbmL1TrdTracklet4;

class CbmL1CATrdTrackFinderSA : public CbmTrdTrackFinder {
  private:
    CbmL1CATrdTrackFinderSA(const CbmL1CATrdTrackFinderSA&);
    CbmL1CATrdTrackFinderSA operator=(const CbmL1CATrdTrackFinderSA&);
  
 public:

  /** Default constructor **/
  CbmL1CATrdTrackFinderSA();

  /** Destructor **/
  virtual ~CbmL1CATrdTrackFinderSA();


  static CbmL1CATrdTrackFinderSA *Instance(){ return fInstance; }
  static CbmL1CATrdTrackFinderSA *fInstance;

  /** Initialisation **/
  void Init();

  /** Track finding algorithm **/
  Int_t DoFind(TClonesArray* hitArray,
	       TClonesArray* trackArray);

  struct Layer
  {
    Layer(){}
    Double_t X[12];
    Double_t Y[12];
    Double_t Z[12];
    Double_t scale[12];
  } geoLayer;

  Int_t fNTrdHits;

  /** TRD layout **/
  Int_t fNoTrdStations;
  Int_t fNoTrdPerStation;

  /**  Structure contains trd hits divided per given station **/
  struct LayerWithHits
  {
    LayerWithHits():
hitIndex(0),
mcTrackID(0),
 X(0),
 Y(0),
 Z(0),
 DX(0),
 DY(0),
planeID(0)
    {}
    Int_t    hitIndex;
    Int_t    mcTrackID;
    Double_t X;
    Double_t Y;
    Double_t Z;
    Double_t DX;
    Double_t DY;
    Int_t    planeID;
  } planeHits;

  //--- vector with hits which belong to given trd plane
  std::vector<LayerWithHits> fvTrdHitArr[12];
  std::vector<LayerWithHits> fvTrdPointArr[12];

  std::vector<LayerWithHits>::iterator fivTrdHitArr;

  /** Data branches **/
  TClonesArray* fArrayTrdHit;
  TClonesArray* fArrayTrdTrack;

 private:

  CbmGeoTrdPar *TrdPar;

  /** Event counter **/
  Int_t fEvents;
  Int_t fNofEvents;

  /** Arrays of MC information **/
  TClonesArray* fMCTrackArray;
  TClonesArray* fMCPointArray;

  /** Traxk fitter **/
  CbmTrdTrackFitterKF* trdTrackFitterKF;

  /** Verbosity level **/
  Int_t fVerbose;

  /** Array of sorted TRD hits **/
  Int_t fNoTrdHits[12];
  Int_t fTrdHitIndex[12][10000];

  /** Arrays for TRD tracks **/
  std::vector<CbmTrdTrack*> fvTempArray;
  std::vector<CbmTrdTrack*> fvFoundTracks;

  inline static Bool_t CompareY(LayerWithHits A, LayerWithHits B){
    return (A.Y < B.Y);
  };

  /** Structure contains temporary values of track candidates **/
  struct TempTrackStruct
  {
    TempTrackStruct():Chi2(0){};
    Double_t Chi2;
    Int_t M[12];
  } tempTrack;

  inline static Bool_t CompareChi2(TempTrackStruct A, TempTrackStruct B){
    return (A.Chi2 < B.Chi2);
  };


  // iterators to vectors
  std::vector<Int_t>::iterator iStation1;
  std::vector<Int_t>::iterator iStation2;

  std::vector<Int_t>::iterator fImapSt1;
  std::vector<Int_t>::iterator fImapSt2;

  std::vector<CbmL1TrdTracklet4*>::iterator itTrackletsLeft;
  std::vector<CbmL1TrdTracklet4*>::iterator itTrackletsRight;

  Int_t
    iHitMap1,
    iHitMap2;
  Double_t
    iHitMapY1,
    iHitMapY2;

  /** Z position selected trd layers **/
  Double_t
    fTrd13_Z,
    fTrd14_Z,
    fTrd21_Z,
    fTrd24_Z,
    fTrd31_Z;

  CbmKFTrack   *fKfTrack;       // KF track

  /** To check time comsumption of procedures **/
  TStopwatch
    createSegments,
    findNeighbour,
    createSPs,
    createSPs_SL,
    sortSPs,
    doFind,
    sortHits,
    tagTracks,
    createTracks,
    selectTracks,
    delTime,
    secondLoopTime,
    refittingKF,
    thirdLoopTime;

  Double_t
    totCreateSegments,
    totFindNeighbour,
    totCreateSPs,
    totCreateSPs_SL,
    totSortSPs,
    totDoFind,
    totSortHits,
    totTagTracks,
    totCreateTracks,
    totDelTime,
    totSecondLoopTime,
    totThirdLoopTime,
    totSelectTracks,
    totRefittingKF;

  void  DeleteTracklets(std::vector<CbmL1TrdTracklet4*> vect);
  void  DeleteTracklets(std::vector<CbmL1TrdTracklet*> vect);

  Double_t DistTwoTrackletsX(Int_t iIndexFirst,
			     Int_t iIndexSecond,
			     Double_t zed);

  Double_t DistTwoTrackletsY(Int_t iIndexFirst,
			     Int_t iIndexSecond,
			     Double_t zed);

  void CreateSpacePoints(std::vector<LayerWithHits> vTrdHitArrayA,
			 std::vector<LayerWithHits> vTrdHitArrayB,
			 std::vector<CbmL1TrdTracklet*> &clSpacePointsAB,
			 Double_t  sigmaA, Double_t sigmaB);

  void CreateSegments(std::vector<CbmL1TrdTracklet*> clSpacePointsAB,
		      std::vector<CbmL1TrdTracklet*> clSpacePointsCD,
		      std::vector<CbmL1TrdTracklet4*> &clTrackletsAD,
		      Double_t dX, Double_t dY);

  void TagSegments(std::vector<CbmL1TrdTracklet4*> &clTrackletsA,
		   std::vector<CbmL1TrdTracklet4*> &clTrackletsB,
		   Int_t noCombSegments = 0);

  void CreateTracks(std::vector<CbmL1TrdTracklet4*> clTracklets14,
		    std::vector<CbmL1TrdTracklet4*> clTracklets58,
		    std::vector<CbmL1TrdTracklet4*> clTracklets912,
		    std::set <Int_t> &setUsedHits,
		    Bool_t removeUsedHits,
		    Bool_t competition = true,
		    Int_t nrLoop = 0);

  void CreateAndManageSegments(std::vector<CbmL1TrdTracklet4*> clTracklets14,
			       std::vector<CbmL1TrdTracklet4*> clTracklets58,
			       std::vector<CbmL1TrdTracklet4*> clTracklets912);

  Bool_t OverlapsHitsXY(Int_t posA, Int_t posB);

  void FindNeighbour(std::vector<CbmL1TrdTracklet4*> &v1,
		     std::vector<CbmL1TrdTracklet4*> &v2,
		     Double_t dY, Double_t dX);

  /** Fitting procedures **/
  Double_t FitLinear(CbmTrdTrack *tr, Int_t var); //var = {1,2} => which coordinate to use
  Double_t FitLinear(Int_t M[], Int_t var); //var = {1,2} => which coordinate to use
  Double_t Fit(CbmTrdTrack *tr);
  Double_t Fit(Int_t M[]);
  Double_t FitKF(CbmTrdTrack* pTrack);
  Double_t FitTLinearFitter(Int_t M[]);
  Double_t FitLSM(Int_t M[]);


  /** Rejection procedure **/
  Bool_t Rejection(Double_t Procent, Int_t num = 100);

  /** Pdg to charge conversion **/
  Int_t PdgToCharge(Int_t pdgCode);

  /** Create histogramms **/
  void CreateHistogramms();

  /** Activates data branches **/
  void DataBranches();

  /** Determine detector layout **/
  void TrdLayout();

 public:

  /** Write histogramms **/
  void WriteHistogramms();

  /** Public accessors **/
  inline const Int_t& GetVerbose() const {return fVerbose;};

  /** Public modifiers **/
  inline void SetVerbose(const Int_t& verbose) {fVerbose = verbose;};

  /** Histogramms **/
  TH1F* fh_chi2hit;
  TH2F* fh_chi2hit_plane;

  // Histogramms for the distance cuts
  TH1F* fDistLongX;
  TH1F* fDistLongY;
  TH1F* fDistShortX;
  TH1F* fDistShortY;

  TH1F* fDistLongBX;
  TH1F* fDistLongBY;
  TH1F* fDistShortBX;
  TH1F* fDistShortBY;

  TH1F* fDistY12;

  TH2F* fMomDistLongPrimaryX;
  TH2F* fMomDistLongPrimaryY;
  TH2F* fMomDistLongExtraX;
  TH2F* fMomDistLongExtraY;

  TH2F* fMomDistExtrapolPrimaryX;
  TH2F* fMomDistExtrapolPrimaryY;
  TH2F* fMomDistExtrapolExtraX;
  TH2F* fMomDistExtrapolExtraY;

  TH2F* fMomDistShortPrimaryX;
  TH2F* fMomDistShortPrimaryY;
  TH2F* fMomDistShortExtraX;
  TH2F* fMomDistShortExtraY;

  TH1F *fDistY;
  TH1F *fDistX;

  TH1F *fPlane1Ydens;
  TH1F *fPlane5Ydens;
  TH1F *fPlane9Ydens;

  TH1F *fSPlength;
  TH1F *fSPlengthMC;

  TH1F *fYat0;
  TH1F *fYat0MC;

  TH2F* fNoEvTime;

  TH1F *fh_SP_xDiff_MC;
  TH1F *fh_SP_yDiff_MC;

  TH1F *fh_SP_xDiff_nMC;
  TH1F *fh_SP_yDiff_nMC;

  TH1F
    *fUsedHitsPerPlane,
    *fUnUsedHitsPerPlane;


  std::map<Int_t, Int_t> fRUsedHits;
  std::map<Int_t, Int_t> fRUnUsedHits;
  std::map<Int_t, Int_t> fTotHits;

  /** Function for sorting **/
    static Bool_t CompareChi2TrdTrack(const CbmTrdTrack* a, const CbmTrdTrack* b)
    {
      return ( a->GetChiSq()/static_cast<Double_t>(a->GetNDF()) <
          b->GetChiSq()/static_cast<Double_t>(b->GetNDF()) );
    };

  ClassDef(CbmL1CATrdTrackFinderSA, 1);

};


#endif

