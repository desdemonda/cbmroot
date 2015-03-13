/** CbmAnaDimuonAnalysis.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
 **/

#include "CbmAnaDimuonAnalysis.h"
#include "CbmAnaMuch.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmMuchPoint.h"
#include "CbmTrackMatch.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchStrawHit.h"
#include "CbmAnaMuonCandidate.h"
#include "CbmGlobalTrack.h"
#include "CbmBaseHit.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchPixelHit.h"
#include "CbmAnaDimuonCandidate.h"
#include "CbmKFTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigiMatch.h"
#include "CbmVertex.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"

// -----   Default constructor   -------------------------------------------
CbmAnaDimuonAnalysis::CbmAnaDimuonAnalysis(TString digiFileName, Int_t nSignalPairs)
  : FairTask("AnaDimuonAnalysis"),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fMuchPixelHits(NULL),
    fMuchStrawHits(NULL),
    fMuchTracks(NULL),
    fMuchTrackMatches(NULL),
    fStsTrackMatches(NULL),
    fMuCandidates(new TClonesArray("CbmAnaMuonCandidate",1)),
    fDimuonCandidates(new TClonesArray("CbmAnaDimuonCandidate",1)),
    fGlobalTracks(NULL),
    fPixelDigiMatches(NULL),
    fStrawDigiMatches(NULL),
    fClusters(NULL),
    fFitter(NULL),
    fVertex(NULL),
    fStsPointsAccQuota(4),
    fStsTrueHitQuota(0.7),
    fMuchPointsAccQuota(10),
    fMuchTrueHitQuota(0.7),
    fHistoFileName("histo.root"),
    fDigiFileName(digiFileName),
    fTriggerStationIndex(4),
    fNLayers(0),
    mapRecSts(),
    mapRecMuch(),
    mapAllMuPlus(),
    mapAccMuPlus(),
    mapRecMuPlus(),
    mapAllMuMinus(),
    mapAccMuMinus(),
    mapRecMuMinus(),
    fGeoScheme(NULL),
    fSignalPairs(nSignalPairs),
    fIsTriggerEnabled(kTRUE)
{
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaDimuonAnalysis::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();

  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaDimuonAnalysis::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks         = (TClonesArray*) fManager->GetObject("MCTrack");
  fStsPoints        = (TClonesArray*) fManager->GetObject("StsPoint");
  fMuchPoints       = (TClonesArray*) fManager->GetObject("MuchPoint");
  fMuchPixelHits    = (TClonesArray*) fManager->GetObject("MuchPixelHit");
  fMuchStrawHits    = (TClonesArray*) fManager->GetObject("MuchStrawHit");
  fStsTracks        = (TClonesArray*) fManager->GetObject("StsTrack");
  fMuchTracks       = (TClonesArray*) fManager->GetObject("MuchTrack");
  fMuchTrackMatches = (TClonesArray*) fManager->GetObject("MuchTrackMatch");
  fStsTrackMatches  = (TClonesArray*) fManager->GetObject("StsTrackMatch");
  fGlobalTracks     = (TClonesArray*) fManager->GetObject("GlobalTrack");
  fPixelDigiMatches = (TClonesArray*) fManager->GetObject("MuchDigiMatch");
  fStrawDigiMatches = (TClonesArray*) fManager->GetObject("MuchStrawDigiMatch");
  fClusters         = (TClonesArray*) fManager->GetObject("MuchCluster");
  fVertex           = (CbmVertex*)    fManager->GetObject("PrimaryVertex");
  fEvent=0;
  if (!(fMCTracks&&fStsPoints&&fMuchPoints&&fMuchPixelHits&&fStsTracks&&fMuchTracks&&fMuchTrackMatches&&fStsTrackMatches)){
    printf(" %p",fStsPoints);
    printf(" %p",fMuchPoints);
    printf(" %p",fStsTracks);
    printf(" %p",fMuchPixelHits);
    printf(" %p",fMuchTracks);
    printf(" %p",fMuchTrackMatches);
    printf(" %p",fMuchPixelHits);
    printf(" %p",fMuchStrawHits);
    printf("\n");
    Fatal("Init","One of TCloneArrays not available");
  }  
  
  fManager->Register("DimuonCandidates" ,"Much",fDimuonCandidates,kTRUE);
  fManager->Register("MuCandidates" ,"Much",fMuCandidates,kTRUE);
  
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fGeoScheme->Init(fDigiFileName);
//  fLastStationIndex = fGeoScheme->GetNStations()-1;
//  fNLayers = 0;
//  for (Int_t i=0;i<=fLastStationIndex;i++){
//    fNLayers+=fGeoScheme->GetLayerSides(i).size()/2;
//  }
  fFitter = new CbmStsKFTrackFitter();
  fFitter->Init();

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaDimuonAnalysis::Exec(Option_t* opt){
  Int_t nMCTracks      = fMCTracks->GetEntriesFast();
  Int_t nStsTracks     = fStsTracks->GetEntriesFast();
  Int_t nMuchTracks    = fMuchTracks->GetEntriesFast();
  Int_t nStsPoints     = fStsPoints->GetEntriesFast();
  Int_t nMuchPoints    = fMuchPoints->GetEntriesFast();
  Int_t nMuchPixelHits = fMuchPixelHits ? fMuchPixelHits->GetEntriesFast() : 0;
  Int_t nMuchStrawHits = fMuchStrawHits ? fMuchStrawHits->GetEntriesFast() : 0;
  Int_t nGlobalTracks  = fGlobalTracks->GetEntriesFast();
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" GlobalTracks: %4i",nGlobalTracks);
  if (fVerbose>0) printf(" StsTracks: %4i",nStsTracks);
  if (fVerbose>0) printf(" MuchTracks: %4i",nMuchTracks);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" MuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" MuchPixelHits: %4i",nMuchPixelHits);
  if (fVerbose>0) printf(" MuchStrawHits: %4i",nMuchStrawHits);
  if (fVerbose>-1) printf("\n");

  fMuCandidates->Clear();
  fDimuonCandidates->Clear();
  Int_t iMuCandidates = 0;
  
  // Create array of signal dimuons
  for (Int_t iDimuon=0;iDimuon<fSignalPairs;iDimuon++){
    new((*fDimuonCandidates)[iDimuon]) CbmAnaDimuonCandidate();
  }
  
  // Set MC momentum and MC track reference
  for (Int_t iMCTrack=0;iMCTrack<2*fSignalPairs;iMCTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
    // check mother Id for safety
    if (mcTrack->GetMotherId()>=0) { Error("Exec","motherId for signal track >=0"); continue; }
    // get info
    Int_t nAccStsPoints  = mcTrack->GetNPoints(kSTS);
    Int_t nAccMuchPoints = mcTrack->GetNPoints(kMUCH);
    Int_t pdgCode = mcTrack->GetPdgCode();
    TLorentzVector pMC;
    mcTrack->Get4Momentum(pMC);
    // set muon momentum and MC track reference
    CbmAnaMuonCandidate* mu = GetMu(iMCTrack);
    mu->SetMomentumMC(pMC);
    mu->SetMCTrackId(iMCTrack);
    // TODO add other info
  }
  
  // Set arrays of MC points in much (planeId vs point reference)
  for (Int_t iPoint=0;iPoint<nMuchPoints;iPoint++){
    CbmMuchPoint* point = (CbmMuchPoint*) fMuchPoints->At(iPoint);
    Int_t trackId = point->GetTrackID();
    if (trackId>=2*fSignalPairs) continue;
    CbmAnaMuonCandidate* mu = GetMu(trackId);
    Int_t planeId = fGeoScheme->GetLayerSideNr(point->GetDetectorId());
    mu->SetMuchPoint(planeId,iPoint);
  }

  
  for (Int_t iTrack=0;iTrack<nGlobalTracks;iTrack++){
    CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
    Int_t iMuchTrack = globalTrack->GetMuchTrackIndex();
    Int_t iStsTrack  = globalTrack->GetStsTrackIndex();
    //printf("iSts=%4i iMuch=%4i\n", iStsTrack, iMuchTrack);
    // Check much track
    if (iMuchTrack<0) continue;
    CbmMuchTrack*  muchTrack = (CbmMuchTrack*)  fMuchTracks->At(iMuchTrack);
    //printf("\n");
    Int_t nTriggerHits=0;
    Int_t nHits=0;
    for (Int_t i=0;i<muchTrack->GetNofHits();i++){
      Int_t hitIndex = muchTrack->GetHitIndex(i);
      Int_t hitType = muchTrack->GetHitType(i);
      CbmBaseHit* hit;
      if      (hitType==kMUCHPIXELHIT) hit = (CbmBaseHit*) fMuchPixelHits->At(hitIndex);
      else if (hitType==kMUCHSTRAWHIT) hit = (CbmBaseHit*) fMuchStrawHits->At(hitIndex);
      else Fatal("Exec","%i - wrong hit type, must be %i for pixel and %i for straw",hitType,kMUCHPIXELHIT,kMUCHSTRAWHIT);
      Int_t stationIndex = fGeoScheme->GetStationIndex(hit->GetAddress());
      if (stationIndex==fTriggerStationIndex) nTriggerHits++;
      nHits++;
    }
    
    if (nTriggerHits>=3) {
//      printf(":%i %i ",nHits,nTriggerHits);
//      printf("%i ",muchTrackMatch->GetNofTrueHits());
//      printf("%i\n",muchTrackMatch->GetMCTrackId());
    }

    // Take only tracks with at least 3 trigger hits
    if (fIsTriggerEnabled) if (nTriggerHits<3) continue;
    
    CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchTrackMatches->At(iMuchTrack);
//    Int_t mcMuchTrackId = CbmAnaMuch::GetTrackId(muchTrackMatch,fMuchTrueHitQuota);
    Int_t mcMuchTrackId = GetMCTrackId(iMuchTrack);
    
    CbmAnaMuonCandidate* mu;
    if (mcMuchTrackId<0 || mcMuchTrackId>=2*fSignalPairs) {
      new((*fMuCandidates)[iMuCandidates++]) CbmAnaMuonCandidate();
      mu = (CbmAnaMuonCandidate*) (*fMuCandidates)[iMuCandidates-1];
    } else {
      mu = GetMu(mcMuchTrackId); 
    }
    mu->SetNTriggerHits(nTriggerHits);
    mu->SetNMuchHits(muchTrack->GetNofHits());
    printf("%4i  ",muchTrack->GetNofHits());
    printf("%4i  ",mcMuchTrackId);
//    printf("%4i  ",muchTrackMatch->GetNofTrueHits());
    printf("chi2/ndf=%6.2f\n",muchTrack->GetChiSq()/muchTrack->GetNDF());
    
    if (iStsTrack<0) continue;
    CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iStsTrack);
    Int_t nStsHits = stsTrack->GetNofHits();
//    printf("nStsHits=%i\n",nStsHits);
    mu->SetNStsHits(nStsHits);
    Double_t chi = fFitter->GetChiToVertex(stsTrack);
    mu->SetChiToVertex(chi);
    FairTrackParam par;
    fFitter->Extrapolate(stsTrack,fVertex->GetZ(),&par);
    TLorentzVector mom;
    TVector3 p;
    par.Momentum(p);
    mom.SetVectM(p,0.105658);
    mu->SetMomentumRC(mom);
    mu->SetSign(par.GetQp()>0 ? 1 : -1);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaDimuonAnalysis::Finish(){
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaMuonCandidate* CbmAnaDimuonAnalysis::GetMu(Int_t trackId){
  if (trackId<0) Fatal("GetMu","MCtrackId for signal muon < 0");
  if (trackId>=2*fSignalPairs) Fatal("GetMu","MCtrackId for signal muon > 2*dimuons");
  Int_t iDimuon = trackId/2;
  Int_t sign = trackId%2; // 0 - muPlus, 1 - muMinus
  return ((CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon))->GetMu(sign); 
}
// -------------------------------------------------------------------------


Int_t CbmAnaDimuonAnalysis::GetMCTrackId(Int_t iMuchTrack){
  CbmMuchTrack*  muchTrack = (CbmMuchTrack*)  fMuchTracks->At(iMuchTrack);
  std::map<Int_t, Int_t> matchMap;
  for (Int_t i=0;i<muchTrack->GetNofHits();i++){
    Int_t hitIndex = muchTrack->GetHitIndex(i);
    Int_t hitType = muchTrack->GetHitType(i);
    CbmBaseHit* hit;
    if      (hitType==kMUCHPIXELHIT) hit = (CbmBaseHit*) fMuchPixelHits->At(hitIndex);
    else if (hitType==kMUCHSTRAWHIT) hit = (CbmBaseHit*) fMuchStrawHits->At(hitIndex);
    else Fatal("Exec","%i - wrong hit type, must be 6 for pixel and 7 for straw",hitType);
    Int_t stationIndex = fGeoScheme->GetStationIndex(hit->GetAddress());
    if (stationIndex!=fTriggerStationIndex) continue;
    if (hitType==kMUCHPIXELHIT) {
      CbmMuchPixelHit* phit = (CbmMuchPixelHit*) hit;
      Int_t clusterId = phit->GetRefId();
      CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
      for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++){
              Int_t digiIndex = cluster->GetDigi(iDigi);
              DigiToTrackMatch(fPixelDigiMatches, digiIndex, matchMap);
      }
    } else{
      CbmMuchStrawHit* phit = (CbmMuchStrawHit*) hit;
      Int_t digiIndex = phit->GetRefId();
      DigiToTrackMatch(fStrawDigiMatches, digiIndex, matchMap);
    }
  }
  Int_t bestMcTrackId = -1;
  Int_t nofTrue = 0;
      for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it != matchMap.end(); it++) {
              if (it->first != -1 && it->second > nofTrue) {
                      bestMcTrackId = it->first;
                      nofTrue = it->second;
              }
      }
  printf("%i\n",nofTrue);
  return bestMcTrackId;
}

void CbmAnaDimuonAnalysis::DigiToTrackMatch(
                const TClonesArray* digiMatches,
                Int_t digiIndex,
                std::map<Int_t, Int_t> &matchMap)
{
        CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) digiMatches->At(digiIndex);
        if (digiMatch == NULL) return;
        for (Int_t iPoint = 0; iPoint < digiMatch->GetNofLinks(); iPoint++) {
                Int_t pointIndex = digiMatch->GetLink(iPoint).GetIndex();
                if (pointIndex < 0) { // Fake or background hit
                        matchMap[-1]++;
                        continue;
                }
                FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(pointIndex);
                if (point == NULL) continue;
                matchMap[point->GetTrackID()]++;
        }
}


ClassImp(CbmAnaDimuonAnalysis);
