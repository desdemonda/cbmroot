// -------------------------------------------------------------------------
// -----                  CbmTrdTestEventsWithMom source file          -----
// -----                  Created 25/09/06  by M. Krauze               -----
// -------------------------------------------------------------------------

#include "CbmTrdTestEventsWithMom.h"

#include "CbmTrdPoint.h"
#include "FairRootManager.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrdTrackFitterKF_CA.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmVertex.h"
#include "FairTrackParam.h"
#include "CbmGeoTrdPar.h"
#include "FairGeoNode.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TVector3.h"

#include <iostream>
#include <cmath>
using namespace std;

// -----   Default constructor   -------------------------------------------
CbmTrdTestEventsWithMom::CbmTrdTestEventsWithMom()
  : FairTask("TRD track match"),
    fArr(),
    fArrB(),
    fRejInvMass(NULL),
    fNormRejInvMass(NULL),
    fRejNew(NULL),
    fEffNew(NULL),
    fRejInvMass2d(NULL),
    TrdPar(NULL),
    fTrueJPsiEvents(0),
    fPassedJpsiEvents(0),
    fTrd11_Z(0.),
    fTrd13_Z(0.),     
    fTrd14_Z(0.),
    fTrd21_Z(0.),
    fTrd24_Z(0.),
    fTrd31_Z(0.),
    fTrd34_Z(0.),
    fMomRes0(NULL),
    fMomRes05(NULL),
    fMomRes10(NULL),
    fMomRes15(NULL),
    fMomRes20(NULL),
    fMomRes25(NULL),
    fMomRes30(NULL),
    fMomRes35(NULL),
    fMomRes40(NULL),
    fMomRes45(NULL),
    fMomRes50(NULL),
    fInvMassRes(NULL),
    fBydlRes(NULL),
    fBydl(NULL),
    fPointBydlRes(NULL),
    fPointBydl(NULL),
    invMass(NULL),
    invMassMC(NULL),
    invMassAfter(NULL),
    invMassSmeared(NULL),
    fRvsCentr(NULL),
    fAllvsCentr(NULL),
    fMCDistY0(NULL),
    fRecoDistY0(NULL),
    fMCDistX1(NULL),
    fCombDistY0(NULL),
    fCombDistX1(NULL),
    fPtVsY(NULL),
    fPtAngle(NULL),
    fOpeningAngle(NULL),
    fdPx(NULL),
    fdPy(NULL),
    fdPz(NULL),
    fTxDist(NULL),
    fTxPos(NULL),
    fTxNeg(NULL),
    fMomDist(NULL),
    fPull(NULL),
    fPRecoMc(NULL),
    fPMc(NULL),
    fPMC(NULL),
    fPtMC(NULL),
    fPReco(NULL),
    fPtReco(NULL),
    fPassedPMC(NULL),
    fPassedPtMC(NULL),
    fPassedPReco(NULL),
    fPassedPtReco(NULL),
    fRes1(NULL),
    fPull1(NULL),
    fRes2(NULL),
    fPull2(NULL),
    fRes3(NULL),
    fPull3(NULL),
    fRes4(NULL),
    fPull4(NULL),
    fPtRes(NULL),
    fPtVsY_Before(NULL),
    fPtVsY_Pt(NULL),
    fPtVsY_XZ(NULL),
    fPtVsY_YZ(NULL),
    fPtVsY_Angle(NULL),
    fQvsQ(NULL),
    fHitsInLayer1(NULL),
    fNoElecPerEvent(NULL),
    fNoPositPerEvent(NULL),
    fNoPPionPerEvent(NULL),
    fNoNPionPerEvent(NULL),
    fNoProtonPerEvent(NULL),
    fNoPosTracks(NULL),
    fNoNegTracks(NULL),
    fNoAllPions(NULL),
    fNoPassedProton(NULL),
    fNoPassedPPion(NULL),
    fNoPassedNPion(NULL),
    fNoPassedPosTracks(NULL),
    fNoPassedNegTracks(NULL),
    fNoPassedAllPions(NULL),
    fNoPassedPosButPositrons(NULL),
    fNoPassedNegButElectrons(NULL),
    fPdgCodes(NULL),
    fChiDist(NULL),
    fChi2Dist(NULL),
    noPosPart(0),
    noNegPart(0),
    fNoTotCrossedX(0),
    fNoCrossedX(0),
    totParticlesSum(),
    totPassedParticlesSum(),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMCTracks(NULL),
    fArrayKFTrdHit(new TClonesArray("CbmKFTrdHit")),
    fMatchMap(),
    trdTrackFitter(new CbmTrdTrackFitterKF_CA(1,11)),
    trdTrackFitterKF(NULL),
    fVerbose(1),
    fCurrentEvent(0),
    fPassedEvents(0),
    fRF(0.),
    fChi(100.),
    fChi2(100.),
    fPtThr(0.),
    fLowerMassThr(0.),
    fUpperMassThr(10.),
    fNevents(0),
    fPrimOnly(0)
{
  for(int i = 0; i < 100; i++){
    fArr[i] = 0;
    fArrB[i] = 0;
  }
}
// -------------------------------------------------------------------------



// -----   Constructor with verbosity level   ------------------------------
CbmTrdTestEventsWithMom::CbmTrdTestEventsWithMom(Int_t verbose)
  : FairTask("TRD track match"),
    fArr(),
    fArrB(),
    fRejInvMass(NULL),
    fNormRejInvMass(NULL),
    fRejNew(NULL),
    fEffNew(NULL),
    fRejInvMass2d(NULL),
    TrdPar(NULL),
    fTrueJPsiEvents(0),
    fPassedJpsiEvents(0),
    fTrd11_Z(0.),
    fTrd13_Z(0.),     
    fTrd14_Z(0.),
    fTrd21_Z(0.),
    fTrd24_Z(0.),
    fTrd31_Z(0.),
    fTrd34_Z(0.),
    fMomRes0(NULL),
    fMomRes05(NULL),
    fMomRes10(NULL),
    fMomRes15(NULL),
    fMomRes20(NULL),
    fMomRes25(NULL),
    fMomRes30(NULL),
    fMomRes35(NULL),
    fMomRes40(NULL),
    fMomRes45(NULL),
    fMomRes50(NULL),
    fInvMassRes(NULL),
    fBydlRes(NULL),
    fBydl(NULL),
    fPointBydlRes(NULL),
    fPointBydl(NULL),
    invMass(NULL),
    invMassMC(NULL),
    invMassAfter(NULL),
    invMassSmeared(NULL),
    fRvsCentr(NULL),
    fAllvsCentr(NULL),
    fMCDistY0(NULL),
    fRecoDistY0(NULL),
    fMCDistX1(NULL),
    fCombDistY0(NULL),
    fCombDistX1(NULL),
    fPtVsY(NULL),
    fPtAngle(NULL),
    fOpeningAngle(NULL),
    fdPx(NULL),
    fdPy(NULL),
    fdPz(NULL),
    fTxDist(NULL),
    fTxPos(NULL),
    fTxNeg(NULL),
    fMomDist(NULL),
    fPull(NULL),
    fPRecoMc(NULL),
    fPMc(NULL),
    fPMC(NULL),
    fPtMC(NULL),
    fPReco(NULL),
    fPtReco(NULL),
    fPassedPMC(NULL),
    fPassedPtMC(NULL),
    fPassedPReco(NULL),
    fPassedPtReco(NULL),
    fRes1(NULL),
    fPull1(NULL),
    fRes2(NULL),
    fPull2(NULL),
    fRes3(NULL),
    fPull3(NULL),
    fRes4(NULL),
    fPull4(NULL),
    fPtRes(NULL),
    fPtVsY_Before(NULL),
    fPtVsY_Pt(NULL),
    fPtVsY_XZ(NULL),
    fPtVsY_YZ(NULL),
    fPtVsY_Angle(NULL),
    fQvsQ(NULL),
    fHitsInLayer1(NULL),
    fNoElecPerEvent(NULL),
    fNoPositPerEvent(NULL),
    fNoPPionPerEvent(NULL),
    fNoNPionPerEvent(NULL),
    fNoProtonPerEvent(NULL),
    fNoPosTracks(NULL),
    fNoNegTracks(NULL),
    fNoAllPions(NULL),
    fNoPassedProton(NULL),
    fNoPassedPPion(NULL),
    fNoPassedNPion(NULL),
    fNoPassedPosTracks(NULL),
    fNoPassedNegTracks(NULL),
    fNoPassedAllPions(NULL),
    fNoPassedPosButPositrons(NULL),
    fNoPassedNegButElectrons(NULL),
    fPdgCodes(NULL),
    fChiDist(NULL),
    fChi2Dist(NULL),
    noPosPart(0),
    noNegPart(0),
    fNoTotCrossedX(0),
    fNoCrossedX(0),
    totParticlesSum(),
    totPassedParticlesSum(),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMCTracks(NULL),
    fArrayKFTrdHit(new TClonesArray("CbmKFTrdHit")),
    fMatchMap(),
    trdTrackFitter(new CbmTrdTrackFitterKF_CA(1,11)),
    trdTrackFitterKF(NULL),
    fVerbose(verbose),
    fCurrentEvent(0),
    fPassedEvents(0),
    fRF(0.),
    fChi(100.),
    fChi2(100.),
    fPtThr(0.),
    fLowerMassThr(0.),
    fUpperMassThr(10.),
    fNevents(0),
    fPrimOnly(0)
{
  for(int i = 0; i < 100; i++){
    fArr[i] = 0;
    fArrB[i] = 0;
  }
}
// -------------------------------------------------------------------------



// -----   Constructor with name, title and verbosity  ---------------------
CbmTrdTestEventsWithMom::CbmTrdTestEventsWithMom(const char* name, const char* title,
						 Int_t verbose, Double_t RF,
						 Double_t chi, Double_t PtThr)
  : FairTask(name),
    fArr(),
    fArrB(),
    fRejInvMass(NULL),
    fNormRejInvMass(NULL),
    fRejNew(NULL),
    fEffNew(NULL),
    fRejInvMass2d(NULL),
    TrdPar(NULL),
    fTrueJPsiEvents(0),
    fPassedJpsiEvents(0),
    fTrd11_Z(0.),
    fTrd13_Z(0.),     
    fTrd14_Z(0.),
    fTrd21_Z(0.),
    fTrd24_Z(0.),
    fTrd31_Z(0.),
    fTrd34_Z(0.),
    fMomRes0(NULL),
    fMomRes05(NULL),
    fMomRes10(NULL),
    fMomRes15(NULL),
    fMomRes20(NULL),
    fMomRes25(NULL),
    fMomRes30(NULL),
    fMomRes35(NULL),
    fMomRes40(NULL),
    fMomRes45(NULL),
    fMomRes50(NULL),
    fInvMassRes(NULL),
    fBydlRes(NULL),
    fBydl(NULL),
    fPointBydlRes(NULL),
    fPointBydl(NULL),
    invMass(NULL),
    invMassMC(NULL),
    invMassAfter(NULL),
    invMassSmeared(NULL),
    fRvsCentr(NULL),
    fAllvsCentr(NULL),
    fMCDistY0(NULL),
    fRecoDistY0(NULL),
    fMCDistX1(NULL),
    fCombDistY0(NULL),
    fCombDistX1(NULL),
    fPtVsY(NULL),
    fPtAngle(NULL),
    fOpeningAngle(NULL),
    fdPx(NULL),
    fdPy(NULL),
    fdPz(NULL),
    fTxDist(NULL),
    fTxPos(NULL),
    fTxNeg(NULL),
    fMomDist(NULL),
    fPull(NULL),
    fPRecoMc(NULL),
    fPMc(NULL),
    fPMC(NULL),
    fPtMC(NULL),
    fPReco(NULL),
    fPtReco(NULL),
    fPassedPMC(NULL),
    fPassedPtMC(NULL),
    fPassedPReco(NULL),
    fPassedPtReco(NULL),
    fRes1(NULL),
    fPull1(NULL),
    fRes2(NULL),
    fPull2(NULL),
    fRes3(NULL),
    fPull3(NULL),
    fRes4(NULL),
    fPull4(NULL),
    fPtRes(NULL),
    fPtVsY_Before(NULL),
    fPtVsY_Pt(NULL),
    fPtVsY_XZ(NULL),
    fPtVsY_YZ(NULL),
    fPtVsY_Angle(NULL),
    fQvsQ(NULL),
    fHitsInLayer1(NULL),
    fNoElecPerEvent(NULL),
    fNoPositPerEvent(NULL),
    fNoPPionPerEvent(NULL),
    fNoNPionPerEvent(NULL),
    fNoProtonPerEvent(NULL),
    fNoPosTracks(NULL),
    fNoNegTracks(NULL),
    fNoAllPions(NULL),
    fNoPassedProton(NULL),
    fNoPassedPPion(NULL),
    fNoPassedNPion(NULL),
    fNoPassedPosTracks(NULL),
    fNoPassedNegTracks(NULL),
    fNoPassedAllPions(NULL),
    fNoPassedPosButPositrons(NULL),
    fNoPassedNegButElectrons(NULL),
    fPdgCodes(NULL),
    fChiDist(NULL),
    fChi2Dist(NULL),
    noPosPart(0),
    noNegPart(0),
    fNoTotCrossedX(0),
    fNoCrossedX(0),
    totParticlesSum(),
    totPassedParticlesSum(),
    fTracks(NULL),
    fPoints(NULL),
    fHits(NULL),
    fMatches(NULL),
    fMCTracks(NULL),
    fArrayKFTrdHit(new TClonesArray("CbmKFTrdHit")),
    fMatchMap(),
    trdTrackFitter(new CbmTrdTrackFitterKF_CA(1,11)),
    trdTrackFitterKF(NULL),
    fVerbose(verbose),
    fCurrentEvent(0),
    fPassedEvents(0),
    fRF(RF),
    fChi(chi),
    fChi2(100.),
    fPtThr(PtThr),
    fLowerMassThr(2.),
    fUpperMassThr(4.),
    fNevents(0),
    fPrimOnly(0)
{
  cout
    <<"-I- CbmTrdTestEventsWithMom::CbmTrdTestEventsWithMom: constructing...\n";
  
  for(int i = 0; i < 100; i++) {
    fArr[i] = 0;
    fArrB[i] = 0;
  }
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdTestEventsWithMom::~CbmTrdTestEventsWithMom() { }
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus CbmTrdTestEventsWithMom::Init() {


  fPassedEvents = 0;




  cout
    <<"-I- CbmTrdTestEventsWithMom::Init: starting...\n";


  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdTestEventsWithMom::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }


  // Get TRD hit Array
  fHits  = (TClonesArray*) ioman->GetObject("TrdHit");
  if ( ! fHits) {
    cout << "-W- CbmTrdTestEventsWithMom::Init: No TRDHit array!"
	 << endl;
    return kERROR;
  }

  cout
    <<"-I- CbmTrdTestEventsWithMom::Init: TrdHit OK...\n";

  // Get TrdTrack Array
  fTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
  if ( ! fTracks ) {
    cout << "-E- CbmTrdTestEventsWithMom::Init: No TrdTrack array!" << endl;
    return kERROR;
  }

  cout
    <<"-I- CbmTrdTestEventsWithMom::Init: TrdTrack OK...\n";

  // Get TRDPoint array
  fPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
  if ( ! fPoints ) {
    cout << "-E- CbmTrdTestEventsWithMom::Init: No TRDPoint array!" << endl;
    return kERROR;
  }

  cout
    <<"-I- CbmTrdTestEventsWithMom::Init: TrdPoint OK...\n";

  // Get MCTrack Array
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-E- CbmTrdMatchTracksMK::Init: No MCTrack array!" << endl;
    return kERROR;
  }

// Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
         <<" no FairRunAna object!" << endl;
    return kERROR;
  }


  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- CbmL1CATrdTrackFinderSA::TrdLayout :"
         <<" no runtime database!" << endl;
    return kERROR;
  }


  TrdPar = (CbmGeoTrdPar*) (rtdb->findContainer("CbmGeoTrdPar"));
  TObjArray *Nodes = TrdPar->GetGeoPassiveNodes();

  for( Int_t i=0;i<Nodes->GetEntries(); i++) {
    FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
    if ( !node ) continue;

    TString name = node->getName();
    //TString Sname = node->getShapePointer()->GetName();
    FairGeoVector nodeV = node->getLabTransform()->getTranslation(); //in cm

    Int_t id = node->getMCid();
    cout <<"name: "<< name <<"\tid: "<< id << endl;
    cout <<" name: "<< name
	 <<" value: " << nodeV.Z() << endl;
    //� � if(name.Contains("trd13")) fTrd13_Z = nodeV.Z();
    //� � if(name.Contains("trd14")) fTrd14_Z = nodeV.Z();
    //� � if(name.Contains("trd21")) fTrd21_Z = nodeV.Z();
    //� � if(name.Contains("trd24")) fTrd24_Z = nodeV.Z();
    //� � if(name.Contains("trd31")) fTrd31_Z = nodeV.Z();

    if(name == "trd11") fTrd11_Z = nodeV.Z();
    if(name == "trd13") fTrd13_Z = nodeV.Z();
    if(name == "trd14") fTrd14_Z = nodeV.Z();
    if(name == "trd21") fTrd21_Z = nodeV.Z();
    if(name == "trd24") fTrd24_Z = nodeV.Z();
    if(name == "trd31") fTrd31_Z = nodeV.Z();
    if(name == "trd34") fTrd34_Z = nodeV.Z();
  }


  // Create and register TrdTrackMatch array
  //   fMatches = new TClonesArray("CbmTrackMatch",100);
  //   ioman->Register("TrdTrackMatch", "TRD", fMatches, kTRUE);

  cout << "-I- CbmTrdTestEventsWithMom::Init: Creating Histogramms...";
  CreateHistogramms();

  fPassedEvents = 0;
  fCurrentEvent = 0;

  fNoTotCrossedX = 0;
  fNoCrossedX = 0;

  totParticlesSum.clear();
  totPassedParticlesSum.clear();


  trdTrackFitter->Init(); //mk

  cout << "-I- CbmTrdTestEventsWithMom::Init: trdTrackFitter OK" << endl;
  //  trdTrackFitterKF->Init(); //mk
  cout << "-I- CbmTrdTestEventsWithMom::Init: trdTrackFitterKF OK" << endl;


  noPosPart = 0;
  noNegPart = 0;

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdTestEventsWithMom::Exec(Option_t* opt) { //CbmTrdTestEventsWithMom::Exec()

  fCurrentEvent++;

  cout
    << "-I- CbmTrdTestEventsWithMom::Exec: starting..." << endl
    << "===========> Currently event no " << fCurrentEvent << " is being processed." << endl;


  // Clear output array

  //  fMatches->Clear();



  // Create some pointers and variables
  //   CbmTrdTrack*      track = NULL;
  //   CbmTrdHit*        hit   = NULL;
  //   FairMCPoint*       point = NULL;
  //   Int_t nHits       = 0;
  //   Int_t nMCTracks   = 0;
  //   Int_t iPoint      = 0;
  //   Int_t iMCTrack    = 0;
  //   Int_t nAll        = 0;
  //   Int_t nTrue       = 0;
  //   Int_t nWrong      = 0;
  //   Int_t nFake       = 0;
  //   Int_t nHitSum     = 0;
  //   Int_t nTrueSum    = 0;
  //   Int_t nWrongSum   = 0;
  //   Int_t nFakeSum    = 0;
  //   Int_t nMCTrackSum = 0;

  //  map<Int_t, Int_t>::iterator it = 0;

  CbmTrdTrack* trdTr1;
  //  CbmTrdTrack* trdTr2;

  //   CbmTrdHit* trdHit1;
  //  CbmTrdHit* trdHit12;
  //   CbmTrdPoint* trdPoint1;

  CbmTrdPoint* trdPt;
  //   CbmMCTrack* mcTr;

  Double_t
    //    Pt = 0,
    PtMC = 0,
    PtReco = 0,
    PMC = 0,
    PReco = 0;
  //y  = 0;

  //  Int_t recoInd;
  map<Int_t, Int_t>recoToMc; //delivers a number of the reconstructed track with a given ID
  //  map<Int_t, Int_t>::iterator irecoToMc;

  map<Int_t, Int_t> MC_nOfReferenceTracks; //how many MC tracks of that sort is in the event
  //  map<Int_t, Int_t>::iterator i_MC_nOfReferenceTracks;

  map<Int_t,Int_t> noTrackHits;
  //   map<Int_t,Int_t>::iterator iiIterator;
  //   Int_t hitsum = 0;
  //   Double_t maxPercent = .70;
  //  Bool_t goodTrack = true;

  Int_t noTrcks = Int_t(fTracks->GetEntries());
  //  Int_t noHts;

  //  Int_t noTRDPoints = fPoints->GetEntriesFast();

  //   Int_t
  //     nOfRefTracks = 0,
  //     nOfRefOther = 0,
  //     nOfAllTracks = 0;

  //   Int_t
  //     noGhosts = 0,
  //     noGoodTracks = 0,
  //     noTempGoodTracks = 0;

  Int_t
    //     iTemp,
    //     pdg,
    //     charge,
    max_RF = 100;


  //  Double_t
  //    invMassVal,
  //    pion_RF = 95.;
  //     invMassThr = 2.5,
  //     ptThr = 0.;

  Bool_t
    accept;

  //   FairTrackParam
  //     *paramFirst,
  //     *paramLast;

  //   vector<Int_t> posTracks; //vector with track indexes representing positive particles
  //   vector<Int_t>::iterator iPosTracks;

  //   vector<Int_t> negTracks; //vector with track indexes representing negative particles
  //   vector<Int_t>::iterator iNegTracks;

  vector<TLorentzVector> posTracks; //vector with track indexes representing positive particles
  vector<TLorentzVector>::iterator iPosTracks;

  vector<TLorentzVector> negTracks; //vector with track indexes representing negative particles
  vector<TLorentzVector>::iterator iNegTracks;

  vector<TLorentzVector> MCPosTracks;
  vector<TLorentzVector> MCNegTracks;

  vector<TLorentzVector> SmearPosTracks;
  vector<TLorentzVector> SmearNegTracks;


  posTracks.clear();
  negTracks.clear();
  MCPosTracks.clear();
  MCNegTracks.clear();


  //  Int_t
  //	      iTrdHit1,
  //iTrdHit12;

//   FairTrackParam
//     *paramFirst;

  //    *paramLast;


  //  if(fVerbose > 3) cout << "Marchewka 1" << endl;
  ShowMCDistance();

  //  if(fVerbose > 3) cout << "Marchewka 2" << endl;


  cout.precision(6);


  Int_t
    noAllPos  = 0,
    noAllNeg  = 0,
    noRecoPos = 0,
    noRecoNeg = 0,
    noTrueRecoPos = 0,
    noTrueRecoNeg = 0;

  map <Int_t,Int_t> trackIndexes;
  map <Int_t,Int_t>::iterator iTrackIndexes;



  Int_t hitMax = 0;


  Int_t
    noElecPerEvent = 0,
    noPositPerEvent = 0,
    noPPionPerEvent = 0,
    noNPionPerEvent = 0,
    noProtonPerEvent = 0,
    noPosTracks = 0,
    noNegTracks = 0,


    noPassedProton = 0,
    noPassedPPion = 0,
    noPassedNPion = 0,
    noPassedPosTracks = 0,
    noPassedNegTracks = 0;

  map <Int_t, Int_t> noParticles;
  map <Int_t, Int_t> noPassedParticles;

  map <Int_t, Int_t>::iterator iNoParticles;

  noParticles.clear();
  noPassedParticles.clear();

  Double_t me = 511e-6;

  Bool_t
    signature = false;

  //  Double_t
  //    invMassVal = 0;


  Double_t
    chi = 1000,
    chi2 = 1000;;

  //----------------------------------------------

  //  Int_t noPoints = fPoints->GetEntries();
  TVector3 mom;
  /*

  for(int k=0;k<noPoints;k++) {
    //loop over trd points
    trdPt = (CbmTrdPoint*)fPoints->At(k);
    if(trdPt == NULL) continue;
    if(Int_t(trdPt->GetZ()) == 403) {
      //	const Double_t zMag = 50; //in centimeters
      trdPt->Momentum(mom);
      Double_t pMC = mom.Mag();



      Double_t
	x  = trdPt->GetX(),
	y  = trdPt->GetY(),
	z  = trdPt->GetZ(),
	tx = mom.X()/mom.Z(),
	ty = mom.Y()/mom.Z();

      fHitsInLayer1->Fill(x,y);

      //      if((x > 50 && x < 100 ) && ( y > 50 && y < 100))
      {

	Int_t trackID = trdPt->GetTrackID();
	CbmMCTrack *mcTrack = (CbmMCTrack*)fMCTracks->At(trackID);

	Int_t charge = pdgCharge(mcTrack->GetPdgCode());

	if(mcTrack->GetMotherID() == -1) {

	  Double_t counter = (x - z*tx)*sqrt(1. + pow(tx,2));
	  Double_t denom   = (sqrt(1. + pow(tx,2) + pow(ty,2)));

	  Double_t Bydl = abs((pMC)*(counter/denom));
	  fPointBydl->Fill(Bydl);
	  Double_t pReco = -5.57011*charge*denom/counter;
	  fPointBydlRes->Fill((pReco-pMC)/pMC);
	} else continue;
      }

    }

  }
  */



  //-----------------------------------------------


    CbmMCTrack *mcTr = 0;




    //  if(fVerbose > 3) cout << "Marchewka 3" << endl;


  for(int i=0; i<noTrcks;i++) { //start od loop over reco tracks
    if(fVerbose > 1)cout << endl << "Processing track no. " << i << endl;
    trackIndexes.clear(); //clear the map

    trdTr1 = (CbmTrdTrack*)fTracks->At(i);
    if(trdTr1 == NULL) continue;
    //    trdTr1->SortHits();
    trdTrackFitter->DoFit(trdTr1); //mk
    //    if(trdTr1->GetNofTrdHits() < 12) continue;


    //  if(fVerbose > 3) cout << "Marchewka 4" << endl;

    if(fVerbose > 2)
      cout << "TRD Track refitted." << endl;


    //control parameter display
    FairTrackParam testParam;
    testParam = *(trdTr1->GetParamLast());


    if(fVerbose > 2)
      cout
	<< "Control display: " << endl
	<< "x= " << testParam.GetX() << ", y= " << testParam.GetY() << ", z=" << testParam.GetZ() << endl;
    //


    //-------------------------------
    /*
      FairTrackParam tempParam;
      tempParam = *(trdTr1->GetParamFirst());
      tempParam.SetQp(0.5);
      trdTr1->SetParamFirst(tempParam);
    */

//     trdTrackFitter->DFoit(trdTr1); //mk
//     cout << "TRD Track refitted." << endl;

    //-------------------------------

    //----------- put the ideal parameters into track before fitting -------


    //    if(fVerbose > 3) cout << "Marchewka 5" << endl;


    Int_t noHits = trdTr1->GetNofHits();
    if( noHits < 12 ) continue;
    //    cout << "NofTrdHits = " << noHits << endl;

    //    if(fVerbose > 3) cout << "Marchewka 6" << endl;


    int iMCTrack = 0;
    //    CbmTrdHit* trdHit = 0;
    trdPt = 0;
    //    Int_t foundPoint = 0;


    CbmVertex vtx;



    Int_t MCorReco = 2; //0= Reco, 1 - MC, 2-neither





    if( MCorReco == 2 ) { //2 - normal J/Psi trigger
      //      cout << "Marchewka" << endl;


      TVector3 bla2(0,0,0);
      vtx.Position(bla2);
      FairTrackParam v_track2;

      //      if(fVerbose > 3) cout << "Marchewka 7" << endl;



      for( int k=0;k<noHits;k++ ) {
	Int_t hitIndex = trdTr1->GetHitIndex(k);
	CbmTrdHit *Hit = (CbmTrdHit*)fHits->At(hitIndex);
	Int_t refMCIndex = Hit->GetRefId();
	trdPt = (CbmTrdPoint*)fPoints->At(refMCIndex);
	iMCTrack = trdPt->GetTrackID();
	trackIndexes[iMCTrack]++;
      }


      //    if(fVerbose > 3) cout << "Marchewka 8" << endl;


      for( iTrackIndexes  = trackIndexes.begin();
	   iTrackIndexes != trackIndexes.end();
	   iTrackIndexes++ ) {

	//	cout << "iTrackIndexes = " << iTrackIndexes->second << endl;
	if( iTrackIndexes->second > hitMax ) {
	  hitMax   = iTrackIndexes->second;
	  iMCTrack = iTrackIndexes->first;
	}
      }

      //    if(fVerbose > 3) cout << "Marchewka 9" << endl;


      mcTr = (CbmMCTrack*)fMCTracks->At(iMCTrack);
      //      if(fPrimOnly == 1)
	//	if(mcTr->GetMotherId() < 0)
	//	if(abs(mcTr->GetPdgCode()) != 11) continue;

      //    if(fVerbose > 3) cout << "Marchewka 10" << endl;


      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//      if(mcTr->GetMotherId() != -1) continue;
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


      Int_t pdgCode = mcTr->GetPdgCode();
      Int_t charge = pdgCharge(pdgCode);
      Int_t recoCharge = 0;



      fPdgCodes->Fill(pdgCode);







      //    if(fVerbose > 3) cout << "Marchewka 11" << endl;

      //!

    //    trdTrackFitter->SetPid(11);

    Double_t chi_2 = trdTrackFitterKF->FitToVertex( trdTr1, &vtx, &v_track2 );
    (const_cast<FairTrackParam*>(trdTr1->GetParamFirst()))->SetQp(v_track2.GetQp());
    (const_cast<FairTrackParam*>(trdTr1->GetParamLast()))->SetQp(v_track2.GetQp());
    trdTrackFitter->DoFit(trdTr1);

    chi_2 = trdTrackFitter->FitToVertex(trdTr1, &vtx, &v_track2);
    (const_cast<FairTrackParam*>(trdTr1->GetParamFirst()))->SetQp(v_track2.GetQp());
    (const_cast<FairTrackParam*>(trdTr1->GetParamLast()))->SetQp(v_track2.GetQp());
    trdTrackFitter->DoFit(trdTr1);

    chi_2 = trdTrackFitter->FitToVertex(trdTr1, &vtx, &v_track2);
    (const_cast<FairTrackParam*>(trdTr1->GetParamFirst()))->SetQp(v_track2.GetQp());
    (const_cast<FairTrackParam*>(trdTr1->GetParamLast()))->SetQp(v_track2.GetQp());
    trdTrackFitter->DoFit(trdTr1);


    //    if(fVerbose > 3) cout << "Marchewka 12" << endl;


      Double_t qp;
      qp = v_track2.GetQp();
      if(qp != 0) {
	recoCharge = (Int_t)(qp/fabs(qp));
      } else recoCharge = 0;
      charge = recoCharge;

      //!
      TVector3
	mom2,tempMom;

      v_track2.Momentum(mom2); //test

      //      if(fVerbose > 3) cout << "Marchewka 13" << endl;


      mcTr->GetMomentum(tempMom);
      PMC = tempMom.Mag();
      TLorentzVector lv;
      mcTr->Get4Momentum(lv);
      if(fVerbose > 3)
	cout
	  << "[px,py,pz,E]=[" << tempMom[0] << "," << tempMom[1] << "," << tempMom[2] << "," << mcTr->GetEnergy()
	  << endl;

      if(fVerbose > 3)
	cout
	  << "[px,py,pz,E]=[" << lv[0] << "," << lv[1] << "," << lv[2] << "," << lv[3]
	  << endl;


      //PtMC = mcTr->GetMomentum().Pt();
      //PtReco = sqrt(mom2.X()*mom2.X() + mom2.Y()*mom2.Y());
      PReco = mom2.Mag();





      //      fRes4->Fill(PReco);
      //      cout << "Test: PReco=" << PReco << endl;



      //cheching if it is a ghost track, i.e. lesz than 70% of correct hits
      if( hitMax < Double_t(noHits)*0.7 ) { //if it is a ghost
	if(Rejection(fRF, accept, max_RF, pdgCode) == false) { //if a ghost is rejected
	  continue;
	}
      }
      //end of rejecting ghosts part


      TVector3 bla(0,0,0);
      vtx.Position(bla);
      FairTrackParam v_track;
      FairTrackParam momParam;
      //      momParam.SetQp(0.5);
      //      trdTr1->SetParamFirst(momParam);


      //       cout
      // 	<< "Before fit: "
      // 	<< "tx=" << trdTr1->GetParamFirst()->GetTx()
      // 	<< ", x=" << trdTr1->GetParamFirst()->GetX()
      // 	<< ", z=" << trdTr1->GetParamFirst()->GetZ()
      // 	<< endl;




      if(fVerbose > 1)
	cout << "Initial chi2 of a track = " << trdTr1->GetChiSq() << endl;

      //chi = trdTrackFitterKF->FitToVertex( trdTr1, &vtx, &v_track );
      v_track = v_track2;
      chi = chi_2;

      if(fVerbose > 1)
	cout << "After fit chi2 of a track = " << chi << endl;

      if(trdTr1->GetNDF() != 0) {
	chi2 = trdTr1->GetChiSq()/Double_t((trdTr1->GetNDF()));
      } else chi2 = 9; //mk




      fChi2Dist->Fill(chi);
      fChiDist->Fill(chi2);

      TVector3
	mom1(0,0,0),
	pos1,
	smearedMom1;





      v_track.Momentum(mom1); //1st P calculation!!!
      v_track.Position(pos1);


      //Byld = -5.71598;
      //Double_t bydlRecoMom = charge/EstimateMomentum(trdTr1,-5.69542);
      //GetBydl(trdTr1, PMC, charge);

      //cout << "Ideal momentum: " << PMC << ", reconstructed: " << bydlRecoMom << endl;

      //      fBydlRes->Fill((bydlRecoMom - PMC));




      //		     fBydlRes->Fill((bydlRecoMom - PMC));


      //use mom1 as MCMom!!

      //	mom1 = trdTr1->GetParamFitrst()
      //mom1 = mcTr->GetMomentum();
      // 	cout << "mom before: " << mom1[0] << ", "<< mom1[1] << ", "<< mom1[2] << endl;
      // 	Double_t
      // 	  P = mcTr->GetMomentum().Mag(),
      // 	  px = mom1[0],
      // 	  py = mom1[1],
      // 	  pz = mom1[2];
      // 	Double_t
      // 	  ttx = px/pz,
      // 	  tty = py/pz;
      // 	Double_t Ps = gRandom->Gaus(P,P*0.5);
      // 	//Double_t Ps = P;
      // 	Double_t
      // 	  pzs = Ps/sqrt(ttx*ttx+tty*tty+1),
      // 	  pxs = ttx*pzs,
      // 	  pys = tty*sonlyprimspzs;
      // 	smearedMom1.SetXYZ(pxs,pys,pzs);

      //	cout << "initial momentum: " <<





      //       for(int i=0;i<3;i++) {
      // 	mom1[i]=mom1[i]+gRandom->Gaus(0,mom1[i]*0.15);
      // 	//mom1[i]=gRandom->Gaus(mom1[i],mom1[i]);
      //       }
      //cout << "mom after: " << mom1[0] << ", "<< mom1[1] << ", "<< mom1[2] << endl;


      //end of use MCMom

      //      TVector3 tempMom;
      mcTr->GetMomentum(tempMom);
      PMC = tempMom.Mag();
      mcTr->GetMomentum(tempMom);
      PtMC = tempMom.Pt();
      PtReco = sqrt(mom1.X()*mom1.X() + mom1.Y()*mom1.Y());
      PReco = mom1.Mag();





      //PtMC = (mcTr->GetMomentum()).Pt();


      if(PReco != 0)
	fRes3->Fill((PReco-PMC)/PReco);
      else fRes3->Fill(0);



      Double_t pRes3 = (PReco-PMC)/PReco;

      if(fVerbose > 2)
	cout << "#MOM#: Reco= " << PReco << ", MC= " << PMC << ", pRes= " << pRes3 << endl;


      if(PMC > 5) fMomRes50->Fill(pRes3);
      else if(PMC > 4.5) fMomRes45->Fill(pRes3);
      else if(PMC > 4)   fMomRes40->Fill(pRes3);
      else if(PMC > 3.5) fMomRes35->Fill(pRes3);
      else if(PMC > 3)   fMomRes30->Fill(pRes3);
      else if(PMC > 2.5) fMomRes25->Fill(pRes3);
      else if(PMC > 2)   fMomRes20->Fill(pRes3);
      else if(PMC > 1.5) fMomRes15->Fill(pRes3);
      else if(PMC > 1)   fMomRes10->Fill(pRes3);
      else if(PMC > 0.5) fMomRes05->Fill(pRes3);
      else if(PMC > 0)   fMomRes0->Fill(pRes3);



      //      fRes3->Fill(((PReco-PMC)/PReco));

      // *******************
      //	  Double_t Psmear = smearedMom1.Mag();

      //      fRes4->Fill((Psmear-PReco)/Psmear);
      //fRes4->Fill((Psmear-PMC)/PMC);

      // *******************

      //fPtRes->Fill((PtReco-PtMC)/PtReco);


      //Double_t
      //PtMC = (mcTr->GetMomentum()).Pt();



      //      Double_t PtRecoKick = GetKickPt(trdTr1);
      //      fPtRes->Fill((PtRecoKick-PtMC)/PtRecoKick);



      if(fVerbose > 2)
	cout << "PReco = " << PReco << ", PtReco = " << PtReco << endl;
      if(fVerbose > 2)
	cout << "PMC = " << PMC << ", PtMC = " << PtMC << endl;

      fPMC->Fill(PMC);
      fPtMC->Fill(PtMC);
      fPtReco->Fill(PtReco);
      fPReco->Fill(PReco);

      //       TVector3 v3 = backtrack(trdTr1->GetParamFirst(), trdTr1->GetParamLast(), 0);

      //       cout
      // 	<< "tx=" << trdTr1->GetParamFirst()->GetTx()
      // 	<< ", x=" << trdTr1->GetParamFirst()->GetX()
      // 	<< ", z=" << trdTr1->GetParamFirst()->GetZ()
      // 	<< ", x0=" << v3[0]
      // 	<< endl;



      noParticles[pdgCode]++;
      totParticlesSum[pdgCode]++;

      if(charge > 0) { //pos tracks
	noPosTracks++;

	//positron
	if(pdgCode == -11) noPositPerEvent++;

	//pion 211
	if(pdgCode == 211) noPPionPerEvent++;

	//proton 2212
	if(pdgCode == 2212) noProtonPerEvent++;

      }//end of pos tracks
      else if(charge < 0) { //neg tracks
	noNegTracks++;

	//electron
	if(pdgCode == 11) noElecPerEvent++;

	//pion
	if(pdgCode == -211) noNPionPerEvent++;

      }//end of neg tracks



      //      Double_t x1 = trdTr1->GetParamFirst()->GetX();
      Double_t y1 = trdTr1->GetParamFirst()->GetY();
      Double_t z1 = trdTr1->GetParamFirst()->GetZ();
      //Double_t tx = trdTr1->GetParamFirst()->GetTx();
      Double_t ty = trdTr1->GetParamFirst()->GetTy();

      //      Double_t x0 = x1 - tx*z1;
      Double_t y0 = y1 - ty*z1;

      //      cout << "y0 = " << y0 << endl;

      fRecoDistY0->Fill(y0);


//       Int_t
// 	recoCharge = Int_t(-(x0)/abs(x0));

      if(recoCharge > 0) noRecoPos++;
      else if(recoCharge < 0) noRecoNeg++;

      if(charge > 0) {
	noAllPos++;
	if(recoCharge > 0) noTrueRecoPos++;

      }
      else if(charge < 0) {
	noAllNeg++;
	if(recoCharge < 0) noTrueRecoNeg++;

      }


      //Rejection(Double_t Procent, Bool_t &accept, Int_t num, Int_t pdg)
      if(Rejection(fRF, accept, max_RF, pdgCode) == true) {

	//	if(abs(y0) < 0.6) {//the track points to the target
      	if(chi < fChi) {//the chi value lesser than 2 sigma - from FitToVertex

	  //if(PtReco > fPtThr)
	  if(chi2 < fChi2)
	  {
	    //	    if(PtMC > 0.8) {
	    //if(chi2 < fChi2)
	    if(PtReco > fPtThr)
	      { //chi2 fit quality cut

		noPassedParticles[pdgCode]++;
		totPassedParticlesSum[pdgCode]++;

		TLorentzVector partVect4;
		partVect4.SetXYZM(mom1.X(), mom1.Y(), mom1.Z(), me);
		TLorentzVector partVect4MC;
		TVector3 vmomMC;
		mcTr->GetMomentum(vmomMC);
		partVect4MC.SetXYZM(vmomMC[0],vmomMC[1],vmomMC[2],me);

		TLorentzVector partVect4Smear;
		partVect4Smear.SetXYZM(smearedMom1[0],smearedMom1[1],smearedMom1[2],me);

		fPassedPMC->Fill(PMC);
		fPassedPtMC->Fill(PtMC);
		fPassedPtReco->Fill(PtReco);
		fPassedPReco->Fill(PReco);
		if(recoCharge > 0) { //pos tracks, reco charge
		//if(charge > 0) { //pos tracks, ideal charge
		  noPassedPosTracks++;

		  posTracks.push_back(partVect4);
		  MCPosTracks.push_back(partVect4MC);
		  SmearPosTracks.push_back(partVect4Smear);

		  //pion 211
		  if(pdgCode == 211) noPassedPPion++;

		  //proton 2212
		  if(pdgCode == 2212) noPassedProton++;

		}//end of pos tracks
		else if(recoCharge < 0) { //neg tracks, reco charge
		  //else if(charge < 0) { //neg tracks, ideal charge
		  noPassedNegTracks++;

		  negTracks.push_back(partVect4);
		  MCNegTracks.push_back(partVect4MC);
		  SmearNegTracks.push_back(partVect4Smear);

		  //pion
		  if(pdgCode == -211) noPassedNPion++;

		}//end of neg tracks

	      }//end of chi2 quality cut

	  }//end of Pt test

	}//end of y0/chi cut

      }//end of if rejection part
      //      else cout << "***Particle rejected!" << endl;


    } //end of MCorReco == 2


    //---------------------------------------------------





  } //loop over reco tracks (?)



  //meszik
  //main loop for calculating invariant mass


  fAllvsCentr->Fill(noTrcks);


  if(posTracks.size() > 0 && negTracks.size() > 0) { //if there are pos and neg particles


    //the end-of-event combinatorics

    if(fVerbose > 1)
      cout
	<< "-----------> We have at least something to combine..." << endl
	<< posTracks.size() << " positive, " << negTracks.size() << ", negative. " << endl;


    const int size = 100;
    Int_t
      iIncBins[size],
      iIncBinsB[size];

    for(int i=0;i<size;i++) {
      iIncBins[i] = 0;
      iIncBinsB[i] = 0;
    }

    Double_t idealMass = 3.096;
    //    Double_t massRes = 0.4;

    for( iPosTracks  = posTracks.begin();
	 iPosTracks != posTracks.end();
	 iPosTracks++ ) { //for positive


      for( iNegTracks  = negTracks.begin();
	   iNegTracks != negTracks.end();
	   iNegTracks++ ) { //for negative

	Double_t iMassVal = ((*iPosTracks)+(*iNegTracks)).M();
	fInvMassRes->Fill((iMassVal-idealMass)/idealMass);
	if(fVerbose > 2)
	  cout << "===> invMass = " << iMassVal << " <=== " << endl;
	//cout << "InvMass = " << iMassVal << endl;
	if(iMassVal > fLowerMassThr && iMassVal < fUpperMassThr) {
	//if(iMassVal > idealMass*(1-massRes) && iMassVal < idealMass*(1+massRes)) {
	//	if(true) {
	  signature = true;

	  invMassAfter->Fill(iMassVal);

	  Double_t pVal = 0;
	  for(Int_t i = 0; i < 50; i++) {
	    pVal += 0.06;
	    if(iMassVal > fLowerMassThr+pVal && iMassVal < fUpperMassThr-pVal) {
	      if(iIncBins[i] == 0) {
		//		cout << "Increasing value for bin no. " << i << endl;
		fArr[i]++;
		iIncBins[i]++;
	      }
	    }
	  }
	  pVal = 0;
	  Double_t middle = (fUpperMassThr+fLowerMassThr)/2;
	  Double_t step = middle/25.;

	  if(fVerbose > 2) cout << "middle: "<< middle << ", step: " << step << endl;

	  for(int i=0;i<25;i++) {
	    if(iMassVal < middle+pVal && iMassVal > middle-pVal) {
	      if(iIncBinsB[i] == 0) {
		fArrB[i]++;
		iIncBinsB[i]++;
	      }
	    }
	    pVal += step;
	  }



	  //	      cout << "-->We have M > 2 !!!" << endl;
	}
	invMass->Fill(iMassVal);

      }

    }

    if( fVerbose > 3)
      for(int i=0;i<25;i++) {
	cout <<" fArrB[" << i << "] = " << fArrB[i] << endl;
      }

    if( fVerbose > 3)
      for(int i=0;i<25;i++) {
	cout <<" iIncBinsB[" << i << "] = " << iIncBinsB[i] << endl;
      }



      if(fVerbose > 3)
	for(int i=0;i<size;i++) {
	  cout <<" iIncBins[" << i << "] = " << iIncBins[i] << endl;
	}


      if(fVerbose > 3)
	for(int i=0;i<size;i++) {
	  cout <<" fArr[" << i << "] = " << fArr[i] << endl;
	}


    if( signature == true ) {
      fRvsCentr->Fill(noTrcks);
    }


    for( iPosTracks  = MCPosTracks.begin();
	 iPosTracks != MCPosTracks.end();
	 iPosTracks++ ) {


      for( iNegTracks  = MCNegTracks.begin();
	   iNegTracks != MCNegTracks.end();
	   iNegTracks++ ) {

	Double_t iMassVal = ((*iPosTracks)+(*iNegTracks)).M();
	// 	    if(iMassVal > 2) {
	// 	      signature = true;
	// 	    }
	invMassMC->Fill(iMassVal);

      }

    }
    /*
    for( iPosTracks  = SmearPosTracks.begin();
	 iPosTracks != SmearPosTracks.end();
	 iPosTracks++ ) {


      for( iNegTracks  = SmearNegTracks.begin();
	   iNegTracks != SmearNegTracks.end();
	   iNegTracks++ ) {

	Double_t iMassVal = ((*iPosTracks)+(*iNegTracks)).M();
	invMassSmeared->Fill(iMassVal);

      }

    }
    */

  }//end if

  MCNegTracks.clear();
  MCPosTracks.clear();
  posTracks.clear();
  negTracks.clear();

  map<Int_t, Int_t> pointIndices;
  map<Int_t, Int_t>::iterator iPointIndices;

  pointIndices.clear();


  Int_t noMCTracks = fMCTracks->GetEntries();
  for(int j=0;j<noMCTracks;j++) {
    mcTr = (CbmMCTrack*)fMCTracks->At(j);
    if(mcTr == NULL) continue;

    pointIndices[mcTr->GetPdgCode()]++;

  }

  /*
  for(iPointIndices  = pointIndices.begin();
      iPointIndices != pointIndices.end();
      iPointIndices++) {
    cout << "PdgCode: " << iPointIndices->first << ", entries: " << iPointIndices->second << endl;
  }
  */

  pointIndices.clear();
  Int_t trackID;
  Int_t noTrdPoints = fPoints->GetEntries();
  for(int i=0;i<noTrdPoints;i++) {
    trdPt = (CbmTrdPoint*)fPoints->At(i);
    if(trdPt == NULL) continue;
    trackID = trdPt->GetTrackID();
    //    cout << "TrackID = " << trackID << endl;
    pointIndices[trackID]++;
  }

  TLorentzVector partVect4;

  cout << "Gathered number of entries: " << pointIndices.size() << endl;

  for(iPointIndices  = pointIndices.begin();
      iPointIndices != pointIndices.end();
      iPointIndices++) {

    if(fVerbose > 2)
      cout << "No of hits indexed: " << iPointIndices->second << endl;
    //    cout <<"Index of a MCTrack: " <<  iPointIndices->first << endl;
    if(iPointIndices->second < 12) continue;
    mcTr = (CbmMCTrack*)fMCTracks->At(iPointIndices->first);
    if( mcTr == NULL ) continue;
    //    cout << "Pdg of a candidate: " << mcTr->GetPdgCode() <<endl;
    if(abs(mcTr->GetPdgCode()) != 11) continue;
    if(mcTr->GetMotherId() >= 0) continue;
    mcTr->Get4Momentum(partVect4);
    if(pdgCharge(mcTr->GetPdgCode()) > 0)
      posTracks.push_back(partVect4);
    else if(pdgCharge(mcTr->GetPdgCode()) < 0)
      negTracks.push_back(partVect4);

  }
  /*
  cout
    << "-----------> Monte Carlo check:" << endl
    << posTracks.size() << " positive, " << negTracks.size() << ", negative. " << endl;
  */

  Bool_t
    trueSignature = false;

  for( iPosTracks  = posTracks.begin();
       iPosTracks != posTracks.end();
       iPosTracks++ ) {


    for( iNegTracks  = negTracks.begin();
	 iNegTracks != negTracks.end();
	 iNegTracks++ ) {


      Double_t iMassVal = ((*iPosTracks)+(*iNegTracks)).M();
      Int_t val = Int_t(iMassVal*1000);
      cout <<"MCInvMassVal = " << iMassVal << endl;
      //      if(iMassVal > 3.05 && iMassVal < 3.1) {
      if(val == 3096) {
	fTrueJPsiEvents++;
	trueSignature = true;
      }
    }
  }

  if(signature == true)
    if(trueSignature == true)
      fPassedJpsiEvents++;



  //   for(iNoParticles  = noParticles.begin();
  //       iNoParticles != noParticles.end();
  //       iNoParticles++) {

  //     cout << "Particle pdg: " << iNoParticles->first << ", quantity: " << iNoParticles->second << endl;

  //   }

  //   for(iNoParticles  = noPassedParticles.begin();
  //       iNoParticles != noPassedParticles.end();
  //       iNoParticles++) {

  //     cout << "Particle pdg: " << iNoParticles->first << ", quantity: " << iNoParticles->second << endl;

  //   }








  //--------------------------------------------------------------------------




  if(signature == true) fPassedEvents++;

      if(fVerbose > 0)
	cout << "--> Passed " << fPassedEvents << " events per " << fCurrentEvent << endl;




  Int_t
    noPassedPart = 0,
    noAllPart = 0;

  for(iNoParticles  = totParticlesSum.begin();
      iNoParticles != totParticlesSum.end();
      iNoParticles++) {
    noAllPart += iNoParticles->second;

      if(fVerbose > 2)
	cout << "Total: Particle pdg: " << iNoParticles->first << ", quantity: " << iNoParticles->second << endl;

  }

  for(iNoParticles  = totPassedParticlesSum.begin();
      iNoParticles != totPassedParticlesSum.end();
      iNoParticles++) {

    noPassedPart += iNoParticles->second;
    //      if(fVerbose > 2)
	//cout << "Total Passed: Particle pdg: " << iNoParticles->first << ", quantity: " << iNoParticles->second << endl;

	}


  if(fVerbose > 0)
    cout << noAllPart << " particles processed so far, " << noPassedPart << " of them accepted by a trigger." << endl;




  if(fVerbose > 0)
    cout
      << "################################################################" << endl
      << "noAllPos:\t" << noAllPos << ",\tnoAllNeg:\t" << noAllNeg << endl
      << "noRecoPos:\t" << noRecoPos << ",\tnoRecoNeg:\t" << noRecoNeg << endl
      << "noTrueRecoPos:\t" << noTrueRecoPos << ",\tnoTrueRecoNeg:\t" << noTrueRecoNeg << endl
      << "################################################################" << endl
      << endl;





  //      noPassedParticles
  //	noParticles


}



// -------------------------------------------------------------------------

Double_t CbmTrdTestEventsWithMom::GetInvMass(TLorentzVector &pos, TLorentzVector &neg) {
  return (pos + neg).M();
}


Bool_t  CbmTrdTestEventsWithMom::Rejection(Double_t Procent, Bool_t &accept, Int_t num, Int_t pdg)
{
  //Akceptukemy czastke jesli jest w wieksza niz Procent
  //Akceptujemy := true
  //num - maksimum wylosowanej liczby

  if(abs(pdg) == 11) { //|| abs(pdg) == 13) {
    accept = true;
  }
  else {

    Int_t random;
    accept = false;

    random = (rand()%num);
    if(random >= Procent) accept = true;
  }
  return accept;
}


Int_t CbmTrdTestEventsWithMom::pdgCharge(Int_t pdgCode) {//pdgCharge
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

Double_t CbmTrdTestEventsWithMom::prosta(TVector3 v0, TVector3 v1, int w0, int w1)  //wo, w1 - numery wspolrzednych wektora ktore chcemy uzyc
{
  Double_t
    a,b,
    x1,y1,
    x2,y2;

  x1 = v0[w0];
  y1 = v0[w1];
  x2 = v1[w0];
  y2 = v1[w1];

  //  a = (y1 - b)/x1;
  //  b = y1/x1 + (y2 - x2*y1/x1)/(x1 - x2);
  a = (y2-y1)/(x2-x1);
  b = y1 - x1*a;

  //  if(verb)cout << "Rownanie prostej: y=" << a <<"*z + " << b << endl;
  return b;
}

TVector3 CbmTrdTestEventsWithMom::backtrack(CbmTrdPoint* trd0,CbmTrdPoint* trd8, Double_t z)
{
  //  Double_t
  //    T[6],
  //    V[3],
  //    result = 0;
  /*
    for(int i=0;i<3;i++)
    {
    V[i] = 0;
    }
  */
  //  TVector3 v0, v1;
  //  TVector3 v0;
  //  if(verb)trd0->Print();
  //  if(verb)trd8->Print();
  TVector3 v1(trd0->GetX(),trd0->GetY(),trd0->GetZ());
  TVector3 v0(trd8->GetX(),trd8->GetY(),trd8->GetZ());

  if(v1 == v0) return TVector3(-1000,-1000,-1000);

  /*
    if(trd0 == trd8)
    {
    cout << "Wektory rowne!!\n";
    //      v1(0) = 0;//(0,0,0);//zakladamy ze drugim punktem jest tarcza
    //      v1(1) = 0;
    //      v1(2) = 0;
    //znaleziono tylko jeden punkt w TRD
    }

    else
    {
    TVector3 v0(trd8->GetX(),trd8->GetY(),trd8->GetZ());
    }

  */
  //    cout << "**** backtrack ****\n";
  //  v0.Print();
  //  v1.Print();
  /*
    T[0] = v0.X();
    T[1] = v0.Y();
    T[2] = (v1.X() - v0.X())/(v1.Z() - v0.Z());
    T[3] = (v1.Y() - v0.Y())/(v1.Z() - v0.Z());
    T[4] = result;
    T[5] = v0.Z();
  */
  //  cout << "x,y,z=" << v1.X() << ", " << v1.Y() << ", " << v1.Z() << endl;
  //  cout << "px,py,pz=" << trd8->GetPx() << ", " << trd8->GetPy() << ", " << trd8->GetPz() << endl;
  //  TVector3 mom(trd0->GetPx(),trd0->GetPy(),trd0->GetPz());

  //  cout << "Mamy wynik q/p=" << AnalyticQP(T, V, mf) << ", p=" << mom.Mag() << endl;

  //  v0.Print();
  Double_t
    x,y,
    x0,y0,z0,
    x1,y1,z1,
    t,l,m;

  x0=v0.X();
  y0=v0.Y();
  z0=v0.Z();

  x1=v1.X();
  y1=v1.Y();
  z1=v1.Z();


  l = (z-z0);
  m = (z1-z0);

  //  cout << "(z-z0)="<< l << "(z1-z0)=" << m << endl;
  t = (z - z0)/(z1 - z0);



  //  if(verb)cout << "t=" << t << endl;

  x = x0 + t*(x1-x0);
  y = y0 + t*(y1-y0);

  //  cout << "backtrack: x,y,z = " << x << ", " << y << ", " << z << endl;

  TVector3 ret(x,y,z);
  return ret;
}

TVector3 CbmTrdTestEventsWithMom::backtrack(FairTrackParam* paramFirst, FairTrackParam* paramLast, Double_t z)
{
  TVector3 v1(paramFirst->GetX(),paramFirst->GetY(),paramFirst->GetZ());
  TVector3 v0(paramLast->GetX(), paramLast->GetY(), paramLast->GetZ());

  if(v1 == v0) return TVector3(-1000,-1000,-1000);

  Double_t
    x,y,
    x0,y0,z0,
    x1,y1,z1,
    t,l,m;

  x0=v0.X();
  y0=v0.Y();
  z0=v0.Z();

  x1=v1.X();
  y1=v1.Y();
  z1=v1.Z();


  l = (z-z0);
  m = (z1-z0);

  //  cout << "(z-z0)="<< l << "(z1-z0)=" << m << endl;
  t = (z - z0)/(z1 - z0);



  //  if(verb)cout << "t=" << t << endl;

  x = x0 + t*(x1-x0);
  y = y0 + t*(y1-y0);

  //  cout << "backtrack: x,y,z = " << x << ", " << y << ", " << z << endl;

  TVector3 ret(x,y,z);
  return ret;
}


Double_t CbmTrdTestEventsWithMom::AngleBetweenVectors(TVector3 v1, TVector3 v2)
{

  Double_t angDeg = v1.Angle(v2)*180/TMath::Pi();
  return angDeg;
}

Double_t CbmTrdTestEventsWithMom::AngleBetweenVectors(TLorentzVector* vPos, TLorentzVector* vNeg)
{
  Double_t sizeV1, sizeV2, sumV, total;

  sizeV1 = sqrt(pow(vPos->Px(),2) + pow(vPos->Py(),2) + pow(vPos->Pz(),2));
  sizeV2 = sqrt(pow(vNeg->Px(),2) + pow(vNeg->Py(),2) + pow(vNeg->Pz(),2));
  sumV = vPos->Px()*vNeg->Px() + vPos->Py()*vNeg->Py() + vPos->Pz()*vNeg->Pz();

  total = acos(sumV/(sizeV1*sizeV2))*180/TMath::Pi();
  //cout << "Kat wynosi: " << Total << " stopni.";

  return total;
}


void CbmTrdTestEventsWithMom::CreateHistogramms() {



  Int_t pRng = 3;

  fRejNew = new TH1F("RejNew" ,"RejNew", 25,0,3.25);
  fEffNew = new TH1F("EffNew" ,"EffNew", 25,0,3.25);
  fNormRejInvMass = new TH1F("normRejInvMass" ,"normRejInvMass", 50,0,50);
  fRejInvMass     = new TH1F("rejInvMass"     ,"rejInvMass",     50,0,50);
  fRejInvMass2d   = new TH2F("rejInvMass2d"   ,"rejInvMass2d",   50,0,50, 11, 0, 1.1);


  fMomRes0  = new TH1F("MomRes0", "MomRes0" ,100,-pRng,pRng);
  fMomRes05 = new TH1F("MomRes05","MomRes05",100,-pRng,pRng);
  fMomRes10 = new TH1F("MomRes10","MomRes10",100,-pRng,pRng);
  fMomRes15 = new TH1F("MomRes15","MomRes15",100,-pRng,pRng);
  fMomRes20 = new TH1F("MomRes20","MomRes20",100,-pRng,pRng);
  fMomRes25 = new TH1F("MomRes25","MomRes25",100,-pRng,pRng);
  fMomRes30 = new TH1F("MomRes30","MomRes30",100,-pRng,pRng);
  fMomRes35 = new TH1F("MomRes35","MomRes35",100,-pRng,pRng);
  fMomRes40 = new TH1F("MomRes40","MomRes40",100,-pRng,pRng);
  fMomRes45 = new TH1F("MomRes45","MomRes45",100,-pRng,pRng);
  fMomRes50 = new TH1F("MomRes50","MomRes50",100,-pRng,pRng);



  fInvMassRes = new TH1F("InvMassRes","InvMassRes",100,-1,1);

  fHitsInLayer1 = new TH2F("HitsInLayer1","HitsInLayer1",120,-300,300,120,-300,300);

  fBydlRes = new TH1F("BydlRes","BydlRes",100,-5,5);
  fPointBydlRes = new TH1F("PointBydlRes","PointBydlRes",100,-2,2);

  fBydl = new TH1F("Bydl","Bydl",100,-30,20);
  fPointBydl = new TH1F("PointBydl","PointBydl",100,-10,20);


  invMass = new TH1F("invMass","InvMass",100,0,5);
  invMassMC = new TH1F("invMassMC","InvMassMC",100,0,10);
  invMassAfter = new TH1F("invMassAfter","InvMassAfter",100,0,10);
  invMassSmeared = new TH1F("invMassSmeared","InvMassSmeared",100,0,10);


  Double_t y0Val = 50;
  Double_t z1Val = 30;

  fRvsCentr   = new TH1F("RFvsCentr","RFvsCentr",12,0,600);
  fAllvsCentr = new TH1F("AllvsCentr","AllvsCentr",12,0,600);

  fMCDistY0 = new TH1F("MCDistY0","MCDistY0",1000,-y0Val,y0Val);
  fMCDistX1 = new TH1F("MCDistZ1","MCDistZ1",1000,-z1Val,z1Val);

  fRecoDistY0 = new TH1F("recoDistY0","recoDistY0",100,-2,2);


  fCombDistY0 = new TH1F("CombDistY0","CombDistY0",1000,-y0Val,y0Val);
  fCombDistX1 = new TH1F("CombDistZ1","CombDistZ1",1000,-z1Val,z1Val);

  fPtAngle = new TH1F("PtAngle","PtAngle",360,0,359);
  fOpeningAngle = new TH1F("OpeningAngle","OpeningAngle",360,-179,180);

  fPtVsY = new TH2F("PtVsY","PtVsY",50,0,4,50,0,3);

  fdPx = new TH1F("dPx","dPx",50,-10,10);
  fdPy = new TH1F("dPy","dPy",50,-10,10);
  fdPz = new TH1F("dPz","dPz",50,-10,10);


  fPtVsY_Before = new TH2F("PtVsY_Before","PtVsY_Before",50,-5,5,50,-0,5);
  fPtVsY_Pt     = new TH2F("PtVsY_Pt","PtVsY_Pt",50,-5,5,50,0,5);
  fPtVsY_XZ     = new TH2F("PtVsY_XZ","PtVsY_XZ",50,-5,5,50,0,5);
  fPtVsY_YZ     = new TH2F("PtVsY_YZ","PtVsY_YZ",50,-5,5,50,0,5);
  fPtVsY_Angle  = new TH2F("PtVsY_Angle","PtVsY_Angle",50,-5,5,50,0,5);

  fTxDist = new TH1F("txDist","txDist",1000,-1,1);
  fTxPos  = new TH1F("txPos","txPos",1000,-1,1);
  fTxNeg  = new TH1F("txNeg","txNeg",1000,-1,1);

  fMomDist = new TH1F("momDist","momDist",100,0,15);
  fPRecoMc = new TH1F("pRecoMc","pRecoMc",100,-1,1);
  fPull    = new TH1F("pull","pull",100,-10,10);
  fPMc     = new TH1F("pMc","pMc",100,0,15);

  Double_t range12 = 0.4;

  fRes1  = new TH1F("Res1","Res1",100,-range12,range12);
  fPull1 = new TH1F("Pull1","Pull1",100,-10,10);
  fRes2  = new TH1F("Res2","Res2",100,-range12,range12);
  fPull2 = new TH1F("Pull2","Pull2",100,-10,10);
  fRes3  = new TH1F("Res3","Res3",100,-2,2);
  fPull3 = new TH1F("Pull3","Pull3",100,-10,10);
  fRes4  = new TH1F("Res4","Res4",100,-2,2);
  fPull4 = new TH1F("Pull4","Pull4",100,-10,10);


  fPtRes = new TH1F("ptRes","ptRes",100,-2,2);

  /***
  fNoElecPerEvent   = new TH1F("noElecPerEvent","noElecPerEvent",600,1,600);
  fNoPositPerEvent  = new TH1F("noPositPerEvent","noPositPerEvent",600,1,600);
  fNoPPionPerEvent  = new TH1F("noPPionPerEvent","noPPionPerEvent",600,1,600);
  fNoNPionPerEvent  = new TH1F("noNPionPerEvent","noNPionPerEvent",600,1,600);
  fNoProtonPerEvent = new TH1F("noProtonPerEvent","noProtonPerEvent",600,1,600);
  fNoPosTracks      = new TH1F("noPosTracks","noPosTracks",600,1,600);
  fNoNegTracks      = new TH1F("noNegTracks","noNegTracks",600,1,600);
  fNoAllPions       = new TH1F("noAllPions","noAllPions",600,1,600);
  ***/
  /***
  fNoPassedProton    = new TH1F("noPassedProton","noPassedProton",600,1,600);
  fNoPassedPPion     = new TH1F("noPassedPPion","noPassedPPion",600,1,600);
  fNoPassedNPion     = new TH1F("noPassedNPion","noPassedNPion",600,1,600);
  fNoPassedPosTracks = new TH1F("noPassedPosTracks","noPassedPosTracks",600,1,600);
  fNoPassedNegTracks = new TH1F("noPassedNegTracks","noPassedNegTracks",600,1,600);
  fNoPassedAllPions  = new TH1F("noPassedAllPions","noPassedAllPions",600,1,600);
  fNoPassedPosButPositrons = new TH1F("noPassedPosButPositrons","noPassedPosButPositrons",600,1,600);
  fNoPassedNegButElectrons = new TH1F("noPassedNegButElectrons","noPassedNegButElectrons",600,1,600);
  ***/

  fPdgCodes = new TH1F("pdgCodes","pdgCodes",700,-350,350);

  fPMC = new TH1F("pMC","pMC",100,0,15);
  fPtMC = new TH1F("pPtMC","pPtMC",100,0,15);
  fPReco = new TH1F("pReco","pReco",100,0,15);
  fPtReco = new TH1F("pPtReco","pPtReco",100,0,15);

  fPassedPMC = new TH1F("pPassedMC","pPassedMC",100,0,15);
  fPassedPtMC = new TH1F("pPassedPtMC","pPassedPtMC",100,0,15);
  fPassedPReco = new TH1F("pPassedReco","pPassedReco",100,0,15);
  fPassedPtReco = new TH1F("pPassedPtReco","pPassedPtReco",100,0,15);

  fChi2Dist = new TH1F("chi2Dist","chi2Dist",100,0,10);//ok
  fChiDist = new TH1F("chiDist","chiDist",100,0,10); //ok

}

void CbmTrdTestEventsWithMom::WriteHistogramms() {

  fRejNew->Write();
  fEffNew->Write();
  fNormRejInvMass->Write();
  fRejInvMass->Write();
  fRejInvMass2d->Write();


  fMomRes0->Write();
  fMomRes05->Write();
  fMomRes10->Write();
  fMomRes15->Write();
  fMomRes20->Write();
  fMomRes25->Write();
  fMomRes30->Write();
  fMomRes35->Write();
  fMomRes40->Write();
  fMomRes45->Write();
  fMomRes50->Write();



  fInvMassRes->Write();

  fHitsInLayer1->Write();

  fBydlRes->Write();
  fBydl->Write();


  fPointBydl->Write();
  fPointBydlRes->Write();

  invMass->Write();
  invMassMC->Write();
  invMassAfter->Write();
  invMassSmeared->Write();

  fMCDistY0->Write();
  fMCDistX1->Write();

  fRecoDistY0->Write();

  fRvsCentr->Write();
  fAllvsCentr->Write();

  fCombDistY0->Write();
  fCombDistX1->Write();

  fPtAngle->Write();
  fOpeningAngle->Write();

  fPtVsY->Write();

  fdPx->Write();
  fdPy->Write();
  fdPz->Write();


  fPtVsY_Before->Write();
  fPtVsY_Pt->Write();
  fPtVsY_XZ->Write();
  fPtVsY_YZ->Write();
  fPtVsY_Angle->Write();

  fTxDist->Write();
  fTxPos->Write();
  fTxNeg->Write();

  fMomDist->Write();
  fPRecoMc->Write();
  fPull->Write();
  fPMc->Write();

  fRes1->Write();
  fPull1->Write();
  fRes2->Write();
  fPull2->Write();
  fRes3->Write();
  fPull3->Write();
  fRes4->Write();
  fPull4->Write();

  fPtRes->Write();
  /**
  fNoElecPerEvent->Write();
  fNoPositPerEvent->Write();
  fNoPPionPerEvent->Write();
  fNoNPionPerEvent->Write();
  fNoProtonPerEvent->Write();
  fNoPosTracks->Write();
  fNoNegTracks->Write();
  fNoAllPions->Write();
  **/

  /**
  fNoPassedProton->Write();
  fNoPassedPPion->Write();
  fNoPassedNPion->Write();
  fNoPassedPosTracks->Write();
  fNoPassedNegTracks->Write();
  fNoPassedAllPions->Write();
  fNoPassedPosButPositrons->Write();
  fNoPassedNegButElectrons->Write();
  **/
  fPdgCodes->Write();

  fPMC->Write();
  fPtMC->Write();
  fPReco->Write();
  fPtReco->Write();

  fPassedPMC->Write();
  fPassedPtMC->Write();
  fPassedPReco->Write();
  fPassedPtReco->Write();

  fChi2Dist->Write();
  fChiDist->Write();

}


struct pointPair {
  Int_t p1;
  Int_t p12;

};


void CbmTrdTestEventsWithMom::ShowMCDistance() {
  //fills a histogram with backtracked values, axis = 0 -> X, axis = 1 -> Y

  //  Int_t trackSize = fMCTracks->GetEntries();
  Int_t pointSize = fPoints->GetEntries();

  //  CbmMCTrack *mcTr;
  CbmTrdPoint
    *trdPt1,
    *trdPt12;

  Int_t trackID1 = -42;
  Int_t trackID12 = -42;

  vector <Int_t> v1;
  vector <Int_t>::iterator iv1;
  vector <Int_t> v12;
  vector <Int_t>::iterator iv12;
  vector <pointPair> vPair;

  for(int i=0;i<pointSize;i++) {
    trdPt1 = (CbmTrdPoint*)fPoints->At(i);

    if(Int_t(trdPt1->GetZ()) == fTrd11_Z ) {
      v1.push_back(i);
      if(fVerbose > 2)cout <<  fTrd11_Z << ": i=" << i << endl;
    }

    if(Int_t(trdPt1->GetZ()) == fTrd34_Z) {
      v12.push_back(i);
      if(fVerbose > 2)cout << fTrd34_Z << ": i=" << i << endl;
    }
  }

  pointPair pPair;

  if(fVerbose > 1)cout << "Collected size: v1 = " << v1.size() << ", v12 = " << v12.size() << endl;

  CbmMCTrack
    *mcTr1,
    *mcTr12;

  for(iv1 = v1.begin();iv1 != v1.end();iv1++) {

    trackID1 = ((CbmTrdPoint*)fPoints->At(*iv1))->GetTrackID();
    for(iv12 = v12.begin();iv12 != v12.end();iv12++) {

      trackID12 = ((CbmTrdPoint*)fPoints->At(*iv12))->GetTrackID();
      if(trackID1 == trackID12) {


	pPair.p1  = *iv1;
	pPair.p12 = *iv12;

	vPair.push_back(pPair);
      }
    }
  }

  if(fVerbose > 1)cout << "Total " << vPair.size() << " point pairs detected." << endl;

  if(fVerbose > 1)cout << "Total " << pointSize << " points in array." << endl;

  for(vector <pointPair>::iterator ivPair = vPair.begin();
      ivPair != vPair.end();
      ivPair ++) {

    //     cout << "Index 1:  " << (*ivPair).p1 << endl;
    //     cout << "Index 12: " << (*ivPair).p12 << endl;

    trdPt1  = (CbmTrdPoint*)fPoints->At((*ivPair).p1);
    trdPt12 = (CbmTrdPoint*)fPoints->At((*ivPair).p12);

    mcTr1  = (CbmMCTrack*)fMCTracks->At(trackID1);
    mcTr12 = (CbmMCTrack*)fMCTracks->At(trackID12);

    if(mcTr1->GetMotherId() == -1 && mcTr12->GetMotherId() == -1) {

      TVector3 vY0 = backtrack(trdPt1, trdPt12, 0.);
      TVector3 vX1 = backtrack(trdPt1, trdPt12, 1.);

      if(trdPt12->GetX()*vX1.X() < 0) fNoCrossedX++;

      fMCDistY0->Fill(vY0.Y());
      fMCDistX1->Fill(vX1.X());
    }
  }


  //cout << "No of wrong particles: " << fNoCrossedX << endl;

  fNoTotCrossedX += fNoCrossedX;

  //   for(int i=0;i<trackSize;i++) {
  //     mcTr = (CbmMCTrack*)fMCTracks->At(i);
  //   }
}


// -----   Public method Finish   ------------------------------------------
void CbmTrdTestEventsWithMom::Finish() {

  cout
    << " -I- CbmTrdTestEventsWithMom::Finish:" << endl
    << "noPosTotal = " << noPosPart << ", noNegTotal = " << noNegPart << endl
    << "fNoTotalCrossedX = " << fNoTotCrossedX << endl;


  cout
    << " -I- CbmTrdTestEventsWithStsRev::Finish:" << endl
    << " -I- was executed with the following parameters: " << endl
    << "fPtThr = " << fPtThr << ", fRF = " << fRF << ", fChi = " << fChi << endl
    << "fLowerMassThr = " << fLowerMassThr << ", fUpperMassThr = " << fUpperMassThr << endl
    << "fPassedEvents = " << fPassedEvents << ", fNevents = " << fNevents;



  if(fNevents > 0)
    cout
      << ", " << Double_t(fPassedEvents)/Double_t(fNevents)*100. << "%";


  cout << endl;



  Double_t percDet,percCorr;
  if(fTrueJPsiEvents > 0)
    percDet = Double_t(fPassedJpsiEvents*100)/Double_t(fTrueJPsiEvents);
  else
    percDet = 0;

  if(fPassedEvents > 0)
    percCorr = Double_t(fPassedJpsiEvents*100)/Double_t(fPassedEvents);
  else
    percCorr = 0;

  cout
    << "No of detectable J/Psi: " << fTrueJPsiEvents
    << ", no of detected J/Psi: " << fPassedJpsiEvents
    << " which is " << percDet << "%" << endl
    << "Correctly triggered events: " << fTrueJPsiEvents//fPassedEvents
    << ", which is " << percCorr << "% correct." << endl;


  if(fTrueJPsiEvents > 0 && fPassedEvents > 0) {
    Int_t maxVal = 50;
    for(Int_t i = 0; i < maxVal; i++) {
      //    for(Int_t w = 0; w < fArr[i]; w++) {
      //      fRejInvMass->Fill(i);
      fRejInvMass->SetBinContent(i+1,Double_t((fNevents-fArr[i])*100)/Double_t(fNevents));
      if(fTrueJPsiEvents > 0)
	fNormRejInvMass->SetBinContent(i+1,Double_t((fNevents-fArr[i])*100)/Double_t(fTrueJPsiEvents));
      //  }
    }
  }



  for(Int_t i = 1; i < 25; i++) {
    if(fArrB[i] != 0) {
      fRejNew->SetBinContent(i,Double_t(100./((Double_t(fArrB[i]*100))/(Double_t(fNevents)))));


      if(fPassedJpsiEvents > 0)
	fEffNew->SetBinContent(i,Double_t(fArrB[i]*100)/Double_t(fTrueJPsiEvents));

    }
    else fRejNew->SetBinContent(i,Double_t(fNevents));
  }


  //------------------------------------------------------------



}
// -------------------------------------------------------------------------

TVector3 CbmTrdTestEventsWithMom::GetMCMomentum(Int_t trdRecoTrackID) {

  CbmTrdTrack *trdTr = (CbmTrdTrack*)fTracks->At(trdRecoTrackID);
//  trdTr->SortHits();
  Int_t hitIndex;
  CbmTrdHit *trdHit;
  Int_t MCTrackIndex;
  CbmTrdPoint *trdPt;

  //  for(int i=0;i<12;i++) {
  //    hitIndex = trdTr->GetTrdHitIndex(i);
  //     cout << i << ": " << hitIndex << ", ";

  //   }
  //   cout << endl;

  //  for(int i=0;i<12;i++) {
  hitIndex = trdTr->GetHitIndex(0);
  trdHit = (CbmTrdHit*)fHits->At(hitIndex);
  trdPt = (CbmTrdPoint*)fPoints->At(trdHit->GetRefId());
  MCTrackIndex = trdPt->GetTrackID();
  //     cout << "MCTrackID: " << MCTrackIndex << ", ";
  //   }


  //  cout << endl;
  CbmMCTrack *mcTrack = (CbmMCTrack*)fMCTracks->At(MCTrackIndex);
  TVector3 mom;
  mcTrack->GetMomentum(mom);
  //  TLorentzVector mom4 = mcTrack->Get4Momentum();
  //   cout
  //     << "-I- CbmTrdTestEventsWithMom::GetMCMomentum: output:" << endl
  //     << "MCTrackID: " << MCTrackIndex <<"GetNofTrdHits: " << trdTr->GetNofTrdHits() << endl
  //     << "[x,y,z]=["<<mom.X() <<","<<mom.Y()<<","<<mom.Z()<<"]" << endl
  //     << "[px,py,pz]=["<<mom4.X() <<","<<mom4.Y()<<","<<mom4.Z()<<"]" << endl;
  return mom;

}

Double_t CbmTrdTestEventsWithMom::GetBydl(CbmTrdTrack *trdTrack, Double_t pMC, Int_t charge) {
  //method calculates mean field constant Byld using MC info
  if ( trdTrack == NULL ) return 0;
  Double_t Bydl = 0;
  const FairTrackParam *param;
  //  const Double_t zMag = 50; //in centimeters

  param = trdTrack->GetParamFirst();


  Double_t
    tx = param->GetTx(),
    ty = param->GetTy(),
    x  = param->GetX(),
    z  = param->GetZ();

  Double_t counter = (x - z*tx)*sqrt(1. + pow(tx,2));
  Double_t denom   = (/*zMag**/sqrt(1. + pow(tx,2) + pow(ty,2)));

  Bydl = (pMC/charge)*(counter/denom);

  cout
    << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl
    << " x = " << x << ",\t\t z = " << z << endl
    << "tx = " << tx << ",\t\tty = " << ty << endl
    << "counter = " << counter << ",\t denominator = " << denom << endl
    << "pMC = " << pMC << ",\t\t charge = " << charge << endl
    << "Bydl = " << Bydl << endl
    << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

  fBydl->Fill(Bydl);
  return Bydl;

}

Double_t CbmTrdTestEventsWithMom::EstimateMomentum(CbmTrdTrack *trdTrack, Double_t Bydl) {
  //method calculates q/p using given mean field constant
  if(trdTrack == NULL) return 0;
  Double_t qp = 0;
  const FairTrackParam *param;
  //  const Double_t zMag = 50; //in centimeters

  param = trdTrack->GetParamFirst();

  Double_t
    tx = param->GetTx(),
    ty = param->GetTy(),
    x  = param->GetX(),
    z  = param->GetZ();

  Double_t counter = (x - z*tx)*sqrt(1 + pow(tx,2));
  Double_t denom   = (/*zMag**/sqrt(1 + pow(tx,2) + pow(ty,2)))*Bydl;

  qp = counter/(denom);
  //((x - z*tx)*sqrt(1 + tx*tx))/( zMag*sqrt(1 + tx*tx + ty*ty)*Bydl);



  return qp;

}

Double_t CbmTrdTestEventsWithMom::GetKickPt(CbmTrdTrack *trdTrack) {

  const Double_t e = 0.2998; //charge of the electron, [GeV/c/(T*m)]
  const Double_t B = 6;

  const FairTrackParam *param;
  FairTrackParam
    extParam2,
    extParam1;


  param = trdTrack->GetParamFirst();

//   Double_t
//     tx = param->GetTx(),
//     ty = param->GetTy(),
//     x  = param->GetX(),
//     z  = param->GetZ();

  Double_t
    z0 = 0,
    z2 = 100, //end of a magnet
    z1 = (z2 - z0)/2; //center of the magnet

  trdTrackFitter->Extrapolate(trdTrack, z2, &extParam2);
  trdTrackFitter->Extrapolate(trdTrack, z1, &extParam1);


  Double_t
    x0 = 0,
    x2 = extParam2.GetX(),
    x1 = extParam1.GetX();

  Double_t
    a = sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1)),
    b = sqrt(x1*x1 + z1*z1);


  Double_t L = (a+b)/100;
  Double_t Pt = e*B*L;

  if(fVerbose > 2)
    cout
      << "x0 = " << x0 << ", x1 = " << x1 << ", x2 = " << x2 << endl
      << "z0 = " << z0 << ", z1 = " << z1 << ", z2 = " << z2 << endl
      << "a = " << a << ", b = " << b << ", L=" << L
      << endl;

  return Pt;
}

Double_t CbmTrdTestEventsWithMom::GetKickPt(CbmTrdPoint *trdPoint) {

  const Double_t e = 0.2998; //charge of the electron, [GeV/c/(T*m)]
  const Double_t B = 6;

//   FairTrackParam *param;
//   FairTrackParam
//     extParam2,
//     extParam1;


//  param = trdTrack->GetParamFirst();

//   Double_t
//     tx = trdPoint->GetTx(),
//     ty = trdPoint->GetTy(),
//     x  = trdPoint->GetX(),
//     z  = trdPoint->GetZ();

  Double_t
    z0 = 0,
    z2 = 100, //end of a magnet
    z1 = (z2 - z0)/2; //center of the magnet

  //  trdTrackFitter->Extrapolate(trdTrack, z2, &extParam2);
  //  trdTrackFitter->Extrapolate(trdTrack, z1, &extParam1);


  Double_t
    x0 = 0,
    x2 = 7, //extParam2.GetX(),
    x1 = 5;//extParam1.GetX();

  Double_t
    a = sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1)),
    b = sqrt(x1*x1 + z1*z1);


  Double_t L = (a+b)/100;
  Double_t Pt = e*B*L;


  cout
    << "x0 = " << x0 << ", x1 = " << x1 << ", x2 = " << x2 << endl
    << "z0 = " << z0 << ", z1 = " << z1 << ", z2 = " << z2 << endl
    << "a = " << a << ", b = " << b << ", L=" << L
    << endl;

  return Pt;
}

ClassImp(CbmTrdTestEventsWithMom);

