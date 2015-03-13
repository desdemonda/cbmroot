#include "CbmTrdQa.h"
#include "CbmTrdDigiPar.h"
#include "CbmMCTrack.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TMath.h"
#include "TLegend.h"
#include "TF1.h"
#include "TH2.h"
#include "TH1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"
#include "TBox.h"
#include "TPaveText.h"
#include "TColor.h"
#include "TPolyLine.h"
#include "TFile.h"
#include <iostream>
#include <cmath>

#include "CbmTrdDigitizer.h"
#include "CbmTrdDigitizerPRF.h"
#include "CbmTrdClusterFinderFast.h"
#include "CbmTrdHitProducerCluster.h"
#include "CbmTrdUtils.h"
#include "CbmTrdRadiator.h"

using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;

CbmTrdQa::CbmTrdQa(CbmTrdRadiator *radiator) 
  : FairTask("TrdQa"),
    fMCTracks(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fClusters(NULL),
    fHits(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fLayerMap(),
    fLayerMapIt(),
    fLayerPointMap(),
    fLayerPointMapIt(),
    fLayerHitMap(),
    fLayerHitMapIt(),
    fModulePointMap(),
    fModulePointMapIt(), 
    fModuleDigiMap(),
    fModuleDigiMapIt(), 
    fModuleClusterMap(),
    fModuleClusterMapIt(), 
    fModuleHitMap(),
    fModuleHitMapIt(), 
    fModuleAveragePointsMap(),
    fModuleAveragePointsMapIt(),
    fModuleTrackableMap2(),
    fModuleTrackableMap2It(), 
    fModuleTrackableMap(),
    fModuleTrackableMapIt(), 
    fModuleClusterSizeMap(),
    fModuleClusterSizeMapIt(), 
    fModuledEdxMap(),
    fModuledEdxMapIt(), 
    fModuleTracklengthMap(),
    fModuleTracklengthMapIt(),
    fModuleMultiPointMap(),
    fModuleMultiPointMapIt(),
    fModuleDeltaEMap(), 
    fModuleDeltaEMapIt(), 
    fModuleGhostMap(),
    fModuleGhostMapIt(), 
    fModuleLostMap(),
    fModuleLostMapIt(),
    fModuleEfficiencyMap(),
    fModuleEfficiencyMapIt(),
    fTriggerThreshold(1e-6),
    fGeo(""),
    fD(true),
    fT(true),
    fP(true),
    fC(true),
    fH(true),
    fTrianglePads(false),
    fLayerDummy(NULL),
    fStsTrdPoints(NULL),
    fStsMuchPoints(NULL),
    fStsTofPoints(NULL),
    fMuchTrdPoints(),
    fMuchTofPoints(),
    fTrdTofPoints(),
    fStsTrdPointsTrackable(NULL),
    fTrdPointsPerMcTrack_PID(NULL),
    fTrdPointsPerMcTrack_PT(NULL),
    fTrdPointsPerMcTrack_P(NULL),
    fTrdTrackCrossedRadiator(NULL),
    fMultiHitSamePadPerMcTrack(NULL),
    fMultiHitSamePadPerMcTrack_angle(NULL),
    fMultiHitSamePadPerMcTrack_zBirth(NULL),
    fMultiHitSamePadPerMcTrack_PID(NULL),
    fMultiHitSamePadPerMcTrack_motherPID(NULL),
    fMultiHitSamePadPerMcTrack_motherzBirth(NULL),
    fMultiHitAdjacentPadPerMcTrack(NULL),
    fMultiHitAdjacentPadPerMcTrack_angle(NULL),
    fMultiHitAdjacentPadPerMcTrack_zBirth(NULL),
    fMultiHitAdjacentPadPerMcTrack_PID(NULL),
    fMultiHitAdjacentPadPerMcTrack_motherzBirth(NULL),
    fMultiHitAdjacentPadPerMcTrack_motherPID(NULL),
    fDistanceMcToHit(NULL),
    fDistanceMcToHitAll(NULL),
    fPositionResolutionShort(NULL),
    fPositionResolutionLong(NULL),  
    fdEdxPoint(NULL),
    fdEdxDigi(NULL),
    fdEdxCluster(NULL),
    fdEdxHit(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxPionGhost(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fdEdxElectronGhost(NULL),
    fClusterSize(NULL),  
    fPointsPerDigi(NULL),
    fDigiPerCluster(NULL),
    fClusterPerHit(NULL),
    fMultiPointssVsR(NULL),
    fGhostPointsVsR(NULL),
    fMultiHitsVsR(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL),
    fHitToPointEfficiencyVsR(NULL),
    fMultiHitsVsAlpha(NULL),
    fGhostHitVsAlpha(NULL),
    fLostPointVsAlpha(NULL),
    fHitToPointEfficiencyVsAlpha(NULL),
    fPRF_1D(NULL),
  fPRF_2D(NULL),
    fRadiator(radiator)
{
}

CbmTrdQa::CbmTrdQa(const char *name, const char *title, const char *geo, Double_t triggerThreshold, CbmTrdRadiator *radiator) 
  : FairTask("TrdQa"),
    fMCTracks(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fClusters(NULL),
    fHits(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fLayerMap(),
    fLayerMapIt(),
    fLayerPointMap(),
    fLayerPointMapIt(),
    fLayerHitMap(),
    fLayerHitMapIt(),
    fModulePointMap(),
    fModulePointMapIt(), 
    fModuleDigiMap(),
    fModuleDigiMapIt(), 
    fModuleClusterMap(),
    fModuleClusterMapIt(), 
    fModuleHitMap(),
    fModuleHitMapIt(), 
    fModuleAveragePointsMap(),
    fModuleAveragePointsMapIt(),
    fModuleTrackableMap2(),
    fModuleTrackableMap2It(), 
    fModuleTrackableMap(),
    fModuleTrackableMapIt(), 
    fModuleClusterSizeMap(),
    fModuleClusterSizeMapIt(), 
    fModuledEdxMap(),
    fModuledEdxMapIt(), 
    fModuleTracklengthMap(),
    fModuleTracklengthMapIt(), 
    fModuleMultiPointMap(),
    fModuleMultiPointMapIt(),
    fModuleDeltaEMap(), 
    fModuleDeltaEMapIt(), 
    fModuleGhostMap(),
    fModuleGhostMapIt(), 
    fModuleLostMap(),
    fModuleLostMapIt(),
    fModuleEfficiencyMap(),
    fModuleEfficiencyMapIt(),
    fTriggerThreshold(triggerThreshold),
    fGeo(geo),
    fD(true),
    fT(true),
    fP(true),
    fC(true),
    fH(true),
    fTrianglePads(false),
    fLayerDummy(NULL),
    fStsTrdPoints(NULL),
    fStsMuchPoints(NULL),
    fStsTofPoints(NULL),
    fStsTrdPointsTrackable(NULL),
    fTrdPointsPerMcTrack_PID(NULL),
    fTrdPointsPerMcTrack_PT(NULL),
    fTrdPointsPerMcTrack_P(NULL),
    fTrdTrackCrossedRadiator(NULL),
    fMultiHitSamePadPerMcTrack(NULL),
    fMultiHitSamePadPerMcTrack_angle(NULL),
    fMultiHitSamePadPerMcTrack_zBirth(NULL),
    fMultiHitSamePadPerMcTrack_PID(NULL),
    fMultiHitSamePadPerMcTrack_motherPID(NULL),
    fMultiHitSamePadPerMcTrack_motherzBirth(NULL),
    fMultiHitAdjacentPadPerMcTrack(NULL),
    fMultiHitAdjacentPadPerMcTrack_angle(NULL),
    fMultiHitAdjacentPadPerMcTrack_zBirth(NULL),
    fMultiHitAdjacentPadPerMcTrack_PID(NULL),
    fMultiHitAdjacentPadPerMcTrack_motherzBirth(NULL),
    fMultiHitAdjacentPadPerMcTrack_motherPID(NULL),
    fDistanceMcToHit(NULL),
    fDistanceMcToHitAll(NULL),
    fPositionResolutionShort(NULL),
    fPositionResolutionLong(NULL),  
    fdEdxPoint(NULL),
    fdEdxDigi(NULL),
    fdEdxCluster(NULL),
    fdEdxHit(NULL),
    fdEdxPionMc(NULL),
    fdEdxPionHit(NULL),
    fdEdxPionGhost(NULL),
    fdEdxElectronMc(NULL),
    fdEdxElectronHit(NULL),
    fdEdxElectronGhost(NULL),
    fClusterSize(NULL),  
    fPointsPerDigi(NULL),
    fDigiPerCluster(NULL),
    fClusterPerHit(NULL),
    fMultiPointssVsR(NULL),
    fGhostPointsVsR(NULL),
    fMultiHitsVsR(NULL),
    fGhostHitVsR(NULL),
    fLostPointVsR(NULL),
    fHitToPointEfficiencyVsR(NULL),
    fMultiHitsVsAlpha(NULL),
    fGhostHitVsAlpha(NULL),
    fLostPointVsAlpha(NULL),
    fHitToPointEfficiencyVsAlpha(NULL),
    fPRF_1D(NULL),
  fPRF_2D(NULL),
    fRadiator(radiator)
{
}

CbmTrdQa::~CbmTrdQa()
{  
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  if(fMCTracks){
    fMCTracks->Clear("C");
    fMCTracks->Delete();
    delete fMCTracks;
  }
  if(fPoints){
    fPoints->Delete();
    delete fPoints;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }
  if(fHits){
    fHits->Delete();
    delete fHits;
  }
  if(fDigiPar)
    delete fDigiPar;
  
  for (fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    delete fLayerMapIt->second;
  } 
  fLayerMap.clear();
  for (fLayerPointMapIt = fLayerPointMap.begin();
       fLayerPointMapIt != fLayerPointMap.end(); fLayerPointMapIt++) {
    delete fLayerPointMapIt->second;
  } 
  fLayerPointMap.clear();
  for (fLayerHitMapIt = fLayerHitMap.begin();
       fLayerHitMapIt != fLayerHitMap.end(); fLayerHitMapIt++) {
    delete fLayerHitMapIt->second;
  } 
  fLayerHitMap.clear();
  for (fModulePointMapIt = fModulePointMap.begin();
       fModulePointMapIt != fModulePointMap.end(); fModulePointMapIt++) {
    fModulePointMapIt->second.clear();
  } 
  fModulePointMap.clear();

  for (fModuleDigiMapIt = fModuleDigiMap.begin();
       fModuleDigiMapIt != fModuleDigiMap.end(); fModuleDigiMapIt++) {
    fModuleDigiMapIt->second.clear();
  } 
  fModuleDigiMap.clear();

  for (fModuleClusterMapIt = fModuleClusterMap.begin();
       fModuleClusterMapIt != fModuleClusterMap.end(); fModuleClusterMapIt++) {
    fModuleClusterMapIt->second.clear();
  } 
  fModuleClusterMap.clear();

  for (fModuleHitMapIt = fModuleHitMap.begin();
       fModuleHitMapIt != fModuleHitMap.end(); fModuleHitMapIt++) {
    fModuleHitMapIt->second.clear();
  } 
  fModuleHitMap.clear();

  for (fModuleGhostMapIt = fModuleGhostMap.begin();
       fModuleGhostMapIt != fModuleGhostMap.end(); fModuleGhostMapIt++) {
    delete fModuleGhostMapIt->second;
  } 
  fModuleGhostMap.clear();

  for (fModuleLostMapIt = fModuleLostMap.begin();
       fModuleLostMapIt != fModuleLostMap.end(); fModuleLostMapIt++) {
    delete fModuleLostMapIt->second;
  } 
  fModuleLostMap.clear();

  for (fModuleEfficiencyMapIt = fModuleEfficiencyMap.begin();
       fModuleEfficiencyMapIt != fModuleEfficiencyMap.end(); fModuleEfficiencyMapIt++) {
    delete fModuleEfficiencyMapIt->second;
  } 
  fModuleEfficiencyMap.clear();

  for (fModuleMultiPointMapIt = fModuleMultiPointMap.begin();
       fModuleMultiPointMapIt != fModuleMultiPointMap.end(); fModuleMultiPointMapIt++) {
    delete fModuleMultiPointMapIt->second;
  } 
  fModuleMultiPointMap.clear();


  for (fModuleDeltaEMapIt = fModuleDeltaEMap.begin();
       fModuleDeltaEMapIt != fModuleDeltaEMap.end(); fModuleDeltaEMapIt++) {
    delete fModuleDeltaEMapIt->second;
  } 
  fModuleDeltaEMap.clear();

  for (fModuleTracklengthMapIt = fModuleTracklengthMap.begin();
       fModuleTracklengthMapIt != fModuleTracklengthMap.end(); fModuleTracklengthMapIt++) {
    delete fModuleTracklengthMapIt->second;
  } 
  fModuleTracklengthMap.clear();

  for (fModuledEdxMapIt = fModuledEdxMap.begin();
       fModuledEdxMapIt != fModuledEdxMap.end(); fModuledEdxMapIt++) {
    delete fModuledEdxMapIt->second;
  } 
  fModuledEdxMap.clear();

  for (fModuleClusterSizeMapIt = fModuleClusterSizeMap.begin();
       fModuleClusterSizeMapIt != fModuleClusterSizeMap.end(); fModuleClusterSizeMapIt++) {
    delete fModuleClusterSizeMapIt->second;
  } 
  fModuleClusterSizeMap.clear();


  delete fTrdPointsPerMcTrack_PID;
  delete fTrdPointsPerMcTrack_PT;
  delete fTrdPointsPerMcTrack_P;
  delete fTrdTrackCrossedRadiator;

  delete fMultiHitSamePadPerMcTrack;
  delete fMultiHitSamePadPerMcTrack_angle;
  delete fMultiHitSamePadPerMcTrack_zBirth;
  delete fMultiHitSamePadPerMcTrack_PID;
  delete fMultiHitSamePadPerMcTrack_motherzBirth;
  delete fMultiHitSamePadPerMcTrack_motherPID;
  delete fMultiHitAdjacentPadPerMcTrack;
  delete fMultiHitAdjacentPadPerMcTrack_angle;
  delete fMultiHitAdjacentPadPerMcTrack_zBirth;
  delete fMultiHitAdjacentPadPerMcTrack_PID;
  delete fMultiHitAdjacentPadPerMcTrack_motherzBirth;
  delete fMultiHitAdjacentPadPerMcTrack_motherPID;

  delete fDistanceMcToHit;
  delete fDistanceMcToHitAll;
  delete fdEdxPionMc;
  delete fdEdxPionHit;
  delete fdEdxPionGhost;
  delete fdEdxElectronMc;
  delete fdEdxElectronHit;
  delete fdEdxElectronGhost;
  delete fdEdxPoint;
  delete fdEdxDigi;
  delete fdEdxCluster;
  delete fdEdxHit;
  delete fMultiHitsVsR;
  delete fGhostHitVsR;
  delete fLostPointVsR;
  delete fPositionResolutionShort;
  delete fPositionResolutionLong;
  delete fHitToPointEfficiencyVsR;
  delete fMultiHitsVsAlpha;
  delete fGhostHitVsAlpha;
  delete fLostPointVsAlpha;
  delete fHitToPointEfficiencyVsAlpha;
  delete fClusterSize;
  delete fPointsPerDigi;
  delete fDigiPerCluster;
  delete fClusterPerHit;
  delete fPRF_1D;
  delete fPRF_2D;
  //delete fLayerDummy;
  
}
void CbmTrdQa::SetParContainers()
{
    cout<<" * CbmTrdQa * :: SetParContainers() "<<endl;
    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));
}
InitStatus CbmTrdQa::ReInit()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar")); 
  return kSUCCESS;
}
InitStatus CbmTrdQa::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();  
  fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-W CbmTrdQa::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fT = false;
  }
  fPoints =(TClonesArray *)  ioman->GetObject("TrdPoint");
  if ( ! fPoints ) {
    cout << "-W CbmTrdQa::Init: No TrdPoint array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fP = false;
  }
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdQa::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fD = false;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdQa::Init: No TrdCluster array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fC = false;
  }
  fHits =(TClonesArray *)  ioman->GetObject("TrdHit");
  if ( ! fHits ) {
    cout << "-W CbmTrdQa::Init: No TrdHits array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
    fH = false;
  }
  fGeoHandler->Init();

  fLayerDummy = new TH2I("LayerDummy","",1,-600,600,1,-500,500);
  fLayerDummy->SetXTitle("x-coordinate [cm]");
  fLayerDummy->SetYTitle("y-coordinate [cm]");
  fLayerDummy->GetXaxis()->SetLabelSize(0.02);
  fLayerDummy->GetYaxis()->SetLabelSize(0.02);
  fLayerDummy->GetZaxis()->SetLabelSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleSize(0.02);
  fLayerDummy->GetXaxis()->SetTitleOffset(1.5);
  fLayerDummy->GetYaxis()->SetTitleSize(0.02);
  fLayerDummy->GetYaxis()->SetTitleOffset(2);
  fLayerDummy->GetZaxis()->SetTitleSize(0.02);
  fLayerDummy->GetZaxis()->SetTitleOffset(-2);
  fLayerDummy->SetStats(kFALSE);
  fLayerDummy->SetContour(99);
  fLayerDummy->Fill(0.,0.,0.);

  fStsTrdPoints = new TH2F("fStsTrdPoints","fStsTrdPoints",12,-0.5,11.5,21,-0.5,20.5);
  fStsTrdPoints->SetStats(kFALSE);
  fStsTrdPoints->SetXTitle("TRD points / track");
  fStsTrdPoints->SetYTitle("STS points / track");
  fStsTrdPoints->SetContour(99);

  fStsMuchPoints = new TH2F("fStsMuchPoints","fStsMuchPoints",12,-0.5,11.5,21,-0.5,20.5);
  fStsMuchPoints->SetStats(kFALSE);
  fStsMuchPoints->SetXTitle("MUCH points / track");
  fStsMuchPoints->SetYTitle("STS points / track");
  fStsMuchPoints->SetContour(99);

  fStsTofPoints = new TH2F("fStsTofPoints","fStsTofPoints",12,-0.5,11.5,21,-0.5,20.5);
  fStsTofPoints->SetStats(kFALSE);
  fStsTofPoints->SetXTitle("TOF points / track");
  fStsTofPoints->SetYTitle("STS points / track");
  fStsTofPoints->SetContour(99);

  TString name;
  for (Int_t iStsPoints = 0; iStsPoints < 11; iStsPoints++){
    name.Form("_%02iStsPoints", iStsPoints);

    fMuchTrdPoints[iStsPoints] = new TH2F("fMuchTrdPoints"+name,"fMuchTrdPoints"+name,12,-0.5,11.5,21,-0.5,20.5);
    fMuchTrdPoints[iStsPoints]->SetStats(kFALSE);
    fMuchTrdPoints[iStsPoints]->SetXTitle("TRD points / track");
    fMuchTrdPoints[iStsPoints]->SetYTitle("MUCH points / track");
    fMuchTrdPoints[iStsPoints]->SetContour(99);

    fMuchTofPoints[iStsPoints] = new TH2F("fMuchTofPoints"+name,"fMuchTofPoints"+name,12,-0.5,11.5,21,-0.5,20.5);
    fMuchTofPoints[iStsPoints]->SetStats(kFALSE);
    fMuchTofPoints[iStsPoints]->SetXTitle("TOF points / track");
    fMuchTofPoints[iStsPoints]->SetYTitle("MUCH points / track");
    fMuchTofPoints[iStsPoints]->SetContour(99);

    fTrdTofPoints[iStsPoints] = new TH2F("fTrdTofPoints"+name,"fTrdTofPoints"+name,12,-0.5,11.5,21,-0.5,20.5);
    fTrdTofPoints[iStsPoints]->SetStats(kFALSE);
    fTrdTofPoints[iStsPoints]->SetXTitle("TOF points / track");
    fTrdTofPoints[iStsPoints]->SetYTitle("TRD points / track");
    fTrdTofPoints[iStsPoints]->SetContour(99);
  }

  fStsTrdPointsTrackable = new TH2F("fStsTrdPointsTrackable","fStsTrdPointsTrackable",12,-0.5,11.5,21,-0.5,20.5);
  fStsTrdPointsTrackable->SetStats(kFALSE);
  fStsTrdPointsTrackable->SetXTitle("TRD points / track");
  fStsTrdPointsTrackable->SetYTitle("STS points / track");
  fStsTrdPointsTrackable->SetContour(99);


  fTrdPointsPerMcTrack_PID = new TH2F("fTrdPointsPerMcTrack_PID","fTrdPointsPerMcTrack_PID",12,-0.5,11.5,49,0.5,49.5);
  fTrdPointsPerMcTrack_PID->SetContour(99);
  fTrdPointsPerMcTrack_PID->SetXTitle("n TRD Points");
  fTrdPointsPerMcTrack_PID->SetYTitle("");
  for (Int_t bin = 1; bin <= 49; bin++) {
    fTrdPointsPerMcTrack_PID->GetYaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
  }
  fTrdPointsPerMcTrack_PT = new TH2F("fTrdPointsPerMcTrack_PT","fTrdPointsPerMcTrack_PT",12,-0.5,11.5,30,0,3);
  fTrdPointsPerMcTrack_PT->SetContour(99);
  fTrdPointsPerMcTrack_PT->SetXTitle("n TRD Points");
  fTrdPointsPerMcTrack_PT->SetYTitle("p_{T} (GeV/c)");

  fTrdPointsPerMcTrack_P = new TH2F("fTrdPointsPerMcTrack_P","fTrdPointsPerMcTrack_P",12,-0.5,11.5,100,0,10);
  fTrdPointsPerMcTrack_P->SetContour(99);
  fTrdPointsPerMcTrack_P->SetXTitle("n TRD Points");
  fTrdPointsPerMcTrack_P->SetYTitle("p (GeV/c)");

  fTrdTrackCrossedRadiator = new TH2F("fTrdTrackCrossedRadiator","fTrdTrackCrossedRadiator",12,-0.5,11.5,15,-3.5,11.5);
  fTrdTrackCrossedRadiator->SetStats(kFALSE);
  fTrdTrackCrossedRadiator->SetContour(99);
  fTrdTrackCrossedRadiator->SetXTitle("n TRD Points");
  fTrdTrackCrossedRadiator->SetYTitle("n Radiator crossings");

  fDistanceMcToHit = new TH1I("DistanceMcToHit", "DistanceMcToHit", 1500, 0, 150);
  fDistanceMcToHit->SetStats(kFALSE);
  fDistanceMcToHit->SetXTitle("#Delta r (MC-Hit) pairs [cm]");
  fDistanceMcToHit->SetYTitle("#");
  fDistanceMcToHit->SetMarkerStyle(24);
  fDistanceMcToHitAll = new TH1I("DistanceMcToHitAll", "DistanceMcToHitAll", 1500, 0, 150);
  fDistanceMcToHitAll->SetStats(kFALSE);
  fDistanceMcToHitAll->SetXTitle("#Delta r (MC-Hit) all [cm]");
  fDistanceMcToHitAll->SetYTitle("#");
  fDistanceMcToHitAll->SetMarkerStyle(24);
  fdEdxPionMc = new TH1F("dEdxPionMc", "dEdxPionMc", 250, 0, 30.0e-6);
  fdEdxPionMc->SetStats(kFALSE);
  fdEdxPionMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxPionMc->SetYTitle("#");
  fdEdxPionMc->SetLineColor(1);
  fdEdxPionMc->SetLineStyle(1);
  fdEdxPionHit = new TH1F("dEdxPionHit", "dEdxPionHit", 250, 0, 30.0e-6);
  fdEdxPionHit->SetStats(kFALSE);
  fdEdxPionHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxPionHit->SetYTitle("#");
  fdEdxPionHit->SetLineColor(1);
  fdEdxPionHit->SetLineStyle(2);
  fdEdxPionGhost = new TH1F("dEdxPionGhost", "dEdxPionGhost", 250, 0, 30.0e-6);
  fdEdxPionGhost->SetStats(kFALSE);
  fdEdxPionGhost->SetXTitle("dE/dx Ghost [GeV]");
  fdEdxPionGhost->SetYTitle("#");
  fdEdxPionGhost->SetLineColor(1);
  fdEdxPionGhost->SetLineStyle(3);
  fdEdxElectronMc = new TH1F("dEdxElectronMc", "dEdxElectronMc", 250, 0, 30.0e-6);
  fdEdxElectronMc->SetStats(kFALSE);
  fdEdxElectronMc->SetXTitle("dE/dx MC [GeV]");
  fdEdxElectronMc->SetYTitle("#");
  fdEdxElectronMc->SetLineColor(2);
  fdEdxElectronMc->SetLineStyle(1);
  fdEdxElectronHit = new TH1F("dEdxElectronHit", "dEdxElectronHit", 250, 0, 30.0e-6);
  fdEdxElectronHit->SetStats(kFALSE);
  fdEdxElectronHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxElectronHit->SetYTitle("#");
  fdEdxElectronHit->SetLineColor(2);
  fdEdxElectronHit->SetLineStyle(2);
  fdEdxElectronGhost = new TH1F("dEdxElectronGhost", "dEdxElectronGhost", 250, 0, 30.0e-6);
  fdEdxElectronGhost->SetStats(kFALSE);
  fdEdxElectronGhost->SetXTitle("dE/dx Ghost [GeV]");
  fdEdxElectronGhost->SetYTitle("#");
  fdEdxElectronGhost->SetLineColor(2);
  fdEdxElectronGhost->SetLineStyle(3);
  fdEdxPoint = new TH1F("dEdxPoint", "dEdxPoint", 1.0e3, 0, 30.0e-6);
  fdEdxPoint->SetStats(kFALSE);
  fdEdxPoint->SetXTitle("dE/dx Hit [GeV]");
  fdEdxPoint->SetYTitle("#");
  fdEdxDigi = new TH1F("dEdxDigi", "dEdxDigi", 1.0e3, 0, 30.0e-6);
  fdEdxDigi->SetStats(kFALSE);
  fdEdxDigi->SetXTitle("dE/dx Hit [GeV]");
  fdEdxDigi->SetYTitle("#");
  fdEdxCluster = new TH1F("dEdxCluster", "dEdxCluster", 1.0e3, 0, 30.0e-6);
  fdEdxCluster->SetStats(kFALSE);
  fdEdxCluster->SetXTitle("dE/dx Hit [GeV]");
  fdEdxCluster->SetYTitle("#");
  fdEdxHit = new TH1F("dEdxHit", "dEdxHit", 1.0e3, 0, 30.0e-6);
  fdEdxHit->SetStats(kFALSE);
  fdEdxHit->SetXTitle("dE/dx Hit [GeV]");
  fdEdxHit->SetYTitle("#");
  fMultiHitsVsR = new TProfile("MultiHitVsR", "MultiHitVsR", 785, 0, 785);
  fMultiHitsVsR->SetStats(kFALSE);
  fMultiHitsVsR->SetXTitle("r [cm]");
  fMultiHitsVsR->SetYTitle("hits per point on same pad/ all points per module [%]");
  fMultiHitsVsR->SetMarkerStyle(24);
  fMultiHitsVsR->SetMarkerColor(4);
  fMultiHitsVsR->SetLineColor(4);
  fGhostHitVsR = new TProfile("GhostHitVsR", "GhostHitVsR", 785, 0, 785);
  fGhostHitVsR->SetStats(kFALSE);
  fGhostHitVsR->SetXTitle("r [cm]");
  fGhostHitVsR->SetYTitle("left over hits / all points per module [%]");
  fGhostHitVsR->SetMarkerStyle(24);
  fGhostHitVsR->SetMarkerColor(2);
  fGhostHitVsR->SetLineColor(2);
  fLostPointVsR = new TProfile("LostPointVsR", "LostPointVsR", 785, 0, 785);
  fLostPointVsR->SetStats(kFALSE);
  fLostPointVsR->SetXTitle("r [cm]");
  fLostPointVsR->SetYTitle("left over points / all points per module [%]");
  fLostPointVsR->SetMarkerStyle(24);
  fLostPointVsR->SetMarkerColor(3);
  fLostPointVsR->SetLineColor(3);
  fHitToPointEfficiencyVsR = new TProfile("fHitToPointEfficiencyVsR", "fHitToPointEfficiencyVsR", 785, 0, 785);
  fHitToPointEfficiencyVsR->SetStats(kFALSE);
  fHitToPointEfficiencyVsR->SetXTitle("r [cm]");
  fHitToPointEfficiencyVsR->SetYTitle("point hit pairs / all points per module [%]");
  fHitToPointEfficiencyVsR->SetMarkerStyle(24);
  fHitToPointEfficiencyVsR->SetMarkerColor(1);
  fHitToPointEfficiencyVsR->SetLineColor(1);
  fMultiHitsVsAlpha = new TProfile("MultiHitsVsAlpha", "MultiHitsVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fMultiHitsVsAlpha->SetStats(kFALSE);
  fMultiHitsVsAlpha->SetXTitle("#alpha [mrad]");
  fMultiHitsVsAlpha->SetYTitle("hits per point on same pad/ all points per module [%]");
  fMultiHitsVsAlpha->SetMarkerStyle(24);
  fMultiHitsVsAlpha->SetMarkerColor(4);
  fMultiHitsVsAlpha->SetLineColor(4);
  fGhostHitVsAlpha = new TProfile("GhostHitVsAlpha", "GhostHitVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fGhostHitVsAlpha->SetStats(kFALSE);
  fGhostHitVsAlpha->SetXTitle("#alpha [mrad]");
  fGhostHitVsAlpha->SetYTitle("left over hits / all points per module [%]");
  fGhostHitVsAlpha->SetMarkerStyle(24);
  fGhostHitVsAlpha->SetMarkerColor(2);
  fGhostHitVsAlpha->SetLineColor(2);
  fLostPointVsAlpha = new TProfile("LostPointVsAlpha", "LostPointVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fLostPointVsAlpha->SetStats(kFALSE);
  fLostPointVsAlpha->SetXTitle("#alpha [mrad]");
  fLostPointVsAlpha->SetYTitle("left over points / all points per module [%]");
  fLostPointVsAlpha->SetMarkerStyle(24);
  fLostPointVsAlpha->SetMarkerColor(3);
  fLostPointVsAlpha->SetLineColor(3);
  fHitToPointEfficiencyVsAlpha = new TProfile("fHitToPointEfficiencyVsAlpha", "fHitToPointEfficiencyVsAlpha",TMath::Pi()/3.*100,0,TMath::Pi()/3.*1000);
  fHitToPointEfficiencyVsAlpha->SetStats(kFALSE);
  fHitToPointEfficiencyVsAlpha->SetXTitle("#alpha [mrad]");
  fHitToPointEfficiencyVsAlpha->SetYTitle("point hit pairs / all points per module [%]");
  fHitToPointEfficiencyVsAlpha->SetMarkerStyle(24);
  fHitToPointEfficiencyVsAlpha->SetMarkerColor(1);
  fHitToPointEfficiencyVsAlpha->SetLineColor(1);
  fPositionResolutionShort = new TH1I("fPositionResolutionShort", "fPositionResolutionShort", 20000, -10, 10);
  fPositionResolutionShort->SetStats(kFALSE);
  fPositionResolutionShort->SetXTitle("position deviation along short pad side [cm]");
  fPositionResolutionShort->SetYTitle("#");
  fPositionResolutionLong = new TH1I("fPositionResolutionLong", "fPositionResolutionLong", 20000, -10, 10);
  fPositionResolutionLong->SetStats(kFALSE);
  fPositionResolutionLong->SetXTitle("position deviation along long pad side  [cm]");
  fPositionResolutionLong->SetYTitle("#");
  fClusterSize = new TH1I("fClusterSize","fClusterSize",21,-0.5,20.5);
  fClusterSize->SetStats(kFALSE);
  fClusterSize->SetXTitle("cluster size [pads]");
  fClusterSize->SetYTitle("#");
  fPointsPerDigi = new TH1I("fPointsPerDigi","fPointsPerDigi",101,-0.5,100.5);
  fPointsPerDigi->SetStats(kFALSE);
  fPointsPerDigi->SetXTitle("point per digi");
  fPointsPerDigi->SetYTitle("#");
  fDigiPerCluster = new TH1I("fDigiPerCluster","fDigiPerCluster",101,-0.5,100.5);
  fDigiPerCluster->SetStats(kFALSE);
  fDigiPerCluster->SetXTitle("digi per cluster");
  fDigiPerCluster->SetYTitle("#");
  fClusterPerHit = new TH1I("fClusterPerHit","fClusterPerHit",101,-0.5,100.5);
  fClusterPerHit->SetStats(kFALSE);
  fClusterPerHit->SetXTitle("cluster per hit");
  fClusterPerHit->SetYTitle("#");
  fPRF_1D = new TProfile("fPRF_1D","fPRF_1D",30,-1.5,1.5);
  fPRF_2D = new TH2I("fPRF_2D","fPRF_2D",30,-1.5,1.5,10,0,1);
  fMultiHitSamePadPerMcTrack = new TH1I("fMultiHitSamePadPerMcTrack","fMultiHitSamePadPerMcTrack",21,-0.5,20.5);
  fMultiHitSamePadPerMcTrack->SetStats(kFALSE);
  fMultiHitSamePadPerMcTrack->SetXTitle("Multihits per MC-track");
  fMultiHitSamePadPerMcTrack->SetYTitle("#");
  fMultiHitSamePadPerMcTrack_angle = new TH1I("fMultiHitSamePadPerMcTrack_angle","fMultiHitSamePadPerMcTrack_angle",3600,0,360);
  fMultiHitSamePadPerMcTrack_angle->SetStats(kFALSE);
  fMultiHitSamePadPerMcTrack_angle->SetXTitle("angle [deg]");
  fMultiHitSamePadPerMcTrack_angle->SetYTitle("#");
  fMultiHitSamePadPerMcTrack_zBirth = new TH2I("fMultiHitSamePadPerMcTrack_zBirth","fMultiHitSamePadPerMcTrack_zBirth",10000,-50,1000,10000,-50,1000);
  fMultiHitSamePadPerMcTrack_zBirth->SetStats(kFALSE);
  fMultiHitSamePadPerMcTrack_zBirth->SetContour(99);
  fMultiHitSamePadPerMcTrack_zBirth->SetXTitle("z_{Birth} multihits track a [cm]");
  fMultiHitSamePadPerMcTrack_zBirth->SetYTitle("z_{Birth} multihits track b [cm]");
  fMultiHitSamePadPerMcTrack_PID = new TH2I("fMultiHitSamePadPerMcTrack_PID","fMultiHitSamePadPerMcTrack_PID",49,0.5,49.5,49,0.5,49.5);
  fMultiHitSamePadPerMcTrack_PID->SetStats(kFALSE);
  for (Int_t bin = 1; bin <= 49; bin++) {
    fMultiHitSamePadPerMcTrack_PID->GetYaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
    fMultiHitSamePadPerMcTrack_PID->GetXaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
  }
  fMultiHitSamePadPerMcTrack_PID->GetXaxis()->SetRangeUser(0,15);
  fMultiHitSamePadPerMcTrack_PID->GetYaxis()->SetRangeUser(0,15);
  fMultiHitSamePadPerMcTrack_PID->SetContour(99);
  fMultiHitSamePadPerMcTrack_PID->SetXTitle("MC PID track a");
  fMultiHitSamePadPerMcTrack_PID->SetYTitle("MC PID track b");
  fMultiHitSamePadPerMcTrack_motherzBirth = new TH2I("fMultiHitSamePadPerMcTrack_motherzBirth","fMultiHitSamePadPerMcTrack_motherzBirth",10000,-50,1000,10000,-50,1000);
  fMultiHitSamePadPerMcTrack_motherzBirth->SetStats(kFALSE);
  fMultiHitSamePadPerMcTrack_motherzBirth->SetContour(99);
  fMultiHitSamePadPerMcTrack_motherzBirth->SetXTitle("z_{Birth} multihits track a [cm]");
  fMultiHitSamePadPerMcTrack_motherzBirth->SetYTitle("z_{Birth} multihits track b [cm]");
  fMultiHitSamePadPerMcTrack_motherPID = new TH2I("fMultiHitSamePadPerMcTrack_motherPID","fMultiHitSamePadPerMcTrack_motherPID",49,0.5,49.5,49,0.5,49.5);
  fMultiHitSamePadPerMcTrack_motherPID->SetStats(kFALSE);
  for (Int_t bin = 1; bin <= 49; bin++) {
    fMultiHitSamePadPerMcTrack_motherPID->GetYaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
    fMultiHitSamePadPerMcTrack_motherPID->GetXaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
  }
  fMultiHitSamePadPerMcTrack_motherPID->GetXaxis()->SetRangeUser(0,15);
  fMultiHitSamePadPerMcTrack_motherPID->GetYaxis()->SetRangeUser(0,15);
  fMultiHitSamePadPerMcTrack_motherPID->SetContour(99);
  fMultiHitSamePadPerMcTrack_motherPID->SetXTitle("MC PID track a");
  fMultiHitSamePadPerMcTrack_motherPID->SetYTitle("MC PID track b");

  fMultiHitAdjacentPadPerMcTrack = new TH1I("fMultiHitAdjacentPadPerMcTrack","fMultiHitAdjacentPadPerMcTrack",21,-0.5,20.5);
  fMultiHitAdjacentPadPerMcTrack->SetStats(kFALSE);
  fMultiHitAdjacentPadPerMcTrack->SetXTitle("Multihits per MC-track");
  fMultiHitAdjacentPadPerMcTrack->SetYTitle("#");
  fMultiHitAdjacentPadPerMcTrack_angle = new TH1I("fMultiHitAdjacentPadPerMcTrack_angle","fMultiHitAdjacentPadPerMcTrack_angle",3600,0,360);
  fMultiHitAdjacentPadPerMcTrack_angle->SetStats(kFALSE);
  fMultiHitAdjacentPadPerMcTrack_angle->SetXTitle("angle [deg]");
  fMultiHitAdjacentPadPerMcTrack_angle->SetYTitle("#");
  fMultiHitAdjacentPadPerMcTrack_zBirth = new TH2I("fMultiHitAdjacentPadPerMcTrack_zBirth","fMultiHitAdjacentPadPerMcTrack_zBirth",10000,-50,1000,10000,-50,1000);
  fMultiHitAdjacentPadPerMcTrack_zBirth->SetStats(kFALSE);
  fMultiHitAdjacentPadPerMcTrack_zBirth->SetContour(99);
  fMultiHitAdjacentPadPerMcTrack_zBirth->SetXTitle("z_{Birth} multihits track a [cm]");
  fMultiHitAdjacentPadPerMcTrack_zBirth->SetYTitle("z_{Birth} multihits track b [cm]");
  fMultiHitAdjacentPadPerMcTrack_PID = new TH2I("fMultiHitAdjacentPadPerMcTrack_PID","fMultiHitAdjacentPadPerMcTrack_PID",49,0.5,49.5,49,0.5,49.5);
  fMultiHitAdjacentPadPerMcTrack_PID->SetStats(kFALSE);
  for (Int_t bin = 1; bin <= 49; bin++) {
    fMultiHitAdjacentPadPerMcTrack_PID->GetYaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
    fMultiHitAdjacentPadPerMcTrack_PID->GetXaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
  }
  fMultiHitAdjacentPadPerMcTrack_PID->GetXaxis()->SetRangeUser(0,15);
  fMultiHitAdjacentPadPerMcTrack_PID->GetYaxis()->SetRangeUser(0,15);
  fMultiHitAdjacentPadPerMcTrack_PID->SetContour(99);
  fMultiHitAdjacentPadPerMcTrack_PID->SetXTitle("MC PID track a");
  fMultiHitAdjacentPadPerMcTrack_PID->SetYTitle("MC PID track b");
  fMultiHitAdjacentPadPerMcTrack_motherzBirth = new TH2I("fMultiHitAdjacentPadPerMcTrack_motherzBirth","fMultiHitAdjacentPadPerMcTrack_motherzBirth",10000,-50,1000,10000,-50,1000);
  fMultiHitAdjacentPadPerMcTrack_motherzBirth->SetStats(kFALSE);
  fMultiHitAdjacentPadPerMcTrack_motherzBirth->SetContour(99);
  fMultiHitAdjacentPadPerMcTrack_motherzBirth->SetXTitle("z_{Birth} multihits track a [cm]");
  fMultiHitAdjacentPadPerMcTrack_motherzBirth->SetYTitle("z_{Birth} multihits track b [cm]");
  fMultiHitAdjacentPadPerMcTrack_motherPID = new TH2I("fMultiHitAdjacentPadPerMcTrack_motherPID","fMultiHitAdjacentPadPerMcTrack_motherPID",49,0.5,49.5,49,0.5,49.5);
  fMultiHitAdjacentPadPerMcTrack_motherPID->SetStats(kFALSE);
  for (Int_t bin = 1; bin <= 49; bin++) {
    fMultiHitAdjacentPadPerMcTrack_motherPID->GetYaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
    fMultiHitAdjacentPadPerMcTrack_motherPID->GetXaxis()->SetBinLabel(bin,CbmTrdUtils::GetGeantName(bin));
  }
  fMultiHitAdjacentPadPerMcTrack_motherPID->GetXaxis()->SetRangeUser(0,15);
  fMultiHitAdjacentPadPerMcTrack_motherPID->GetYaxis()->SetRangeUser(0,15);
  fMultiHitAdjacentPadPerMcTrack_motherPID->SetContour(99);
  fMultiHitAdjacentPadPerMcTrack_motherPID->SetXTitle("MC PID track a");
  fMultiHitAdjacentPadPerMcTrack_motherPID->SetYTitle("MC PID track b");


  return kSUCCESS;
}
Double_t CbmTrdQa::GetTrackLength(CbmTrdPoint* point){
  return TMath::Sqrt(
		     (point->GetXOut() - point->GetXIn()) * (point->GetXOut() - point->GetXIn()) + 
		     (point->GetYOut() - point->GetYIn()) * (point->GetYOut() - point->GetYIn()) + 
		     (point->GetZOut() - point->GetZIn()) * (point->GetZOut() - point->GetZIn())
		     );
}

void CbmTrdQa::SetTriggerThreshold(Double_t triggerthreshold){
  fTriggerThreshold = triggerthreshold;
}

TPolyLine *CbmTrdQa::CreateTriangularPad(Int_t column, Int_t row, Double_t content){
  const Int_t nCoordinates = 4;
  Double_t x[nCoordinates] = {column-0.5,column+0.5,column+0.5,column-0.5};
  Double_t y[nCoordinates] = {row-0.5,   row-0.5,   row+0.5,   row-0.5   };
  if (row%2 != 0){
    y[1] = row+0.5;
    y[2] = row+0.5;
  }
  TPolyLine *pad = new TPolyLine(nCoordinates,x,y);
  pad->SetFillColor(content);
  return pad;
}

void CbmTrdQa::Exec(Option_t * option)
{
  Bool_t samePadMerge = false;//true;
  //fTriggerThreshold = CbmTrdClusterFinderFast::GetTriggerThreshold();
  TStopwatch timer;
  timer.Start();
  cout << endl << "==================CbmTrdQa===================" << endl;
  Int_t nEntries(0), iTrack(-1), mother_iTrack(-1), moduleAddress(-1), Station(-1), Layer(-1), combiId(-1), dEdx(0), rPos(0), PDG(-1), ghostCount(0), lostCount(0), nStsPoints(0);
  TString title;
  Bool_t debug = false;
  CbmMCTrack *track = NULL;
  CbmMCTrack *track2 = NULL;
  CbmMCTrack *track_mother = NULL;
  CbmMCTrack *track2_mother = NULL;
  CbmTrdPoint *point = NULL;
  CbmTrdPoint *point2 = NULL;
  CbmTrdDigi *digi = NULL;
  CbmTrdCluster *cluster = NULL;
  CbmTrdHit *hit = NULL;
  CbmTrdPoint *point_temp = NULL;
  std::map<Int_t, Int_t> deltaEMap;
  std::map<Int_t, Int_t> TrackableMap;
  std::map<Int_t, std::map<Int_t, Int_t> > doubleHitSinglePadMap; // digiAddress, std::map[MC_track_ID]
  std::map<Int_t, Int_t> channelAddressMap;
  std::map<Int_t, Int_t> McTrackDoubleHitMap;
  std::map<Int_t, Int_t> neighbourChannelAddressMap;
  std::map<Int_t, Int_t> McTrackNeighbourHitMap;
  // MC-points
  if (fP && fT) {
    nEntries = fPoints->GetEntries();
    printf("%02i Points\n",nEntries);
   
    std::map<Int_t, Int_t> hasBeenUsedTrack;
    for (Int_t iPoint=0; iPoint < nEntries; iPoint++ ) {
      point = (CbmTrdPoint*) fPoints->At(iPoint);
      fdEdxPoint->Fill(point->GetEnergyLoss());
      moduleAddress = point->GetDetectorID();
      Station  = CbmTrdAddress::GetLayerId(moduleAddress) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Layer    = CbmTrdAddress::GetLayerId(moduleAddress) % 4 + 1;//fGeoHandler->GetLayer(moduleId);
      //Station  = 0;//fGeoHandler->GetStation(moduleId);
      //Layer    = CbmTrdAddress::GetLayerId(moduleAddress);//fGeoHandler->GetLayer(moduleId);
      //printf ("P det: %i S%i L%i  \n",moduleId,Station,Layer);//GetDetId()); 
      combiId = 10 * Station + Layer;
      iTrack = point->GetTrackID();
      track = (CbmMCTrack*) fMCTracks->At(iTrack);
      //PDG = track->GetPdgCode();
      nStsPoints = track->GetNPoints(kSTS);
      //fStsTrdPoints->Fill(track->GetNPoints(kTRD),track->GetNPoints(kSTS));
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      if (fModuleGhostMap.find(moduleAddress) == fModuleGhostMap.end()){
	title.Form("G%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleGhostMap[moduleAddress] = new TH1I(title,title,200,0,200);
	fModuleGhostMap[moduleAddress]->SetXTitle("left over points / all points [%]");
	fModuleGhostMap[moduleAddress]->SetYTitle("#");
	title.Form("L%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleLostMap[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleLostMap[moduleAddress]->SetXTitle("left over hits / all points [%]");
	fModuleLostMap[moduleAddress]->SetYTitle("#");
	title.Form("E%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleEfficiencyMap[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleEfficiencyMap[moduleAddress]->SetXTitle("found point hit pairs / all points [%]");
	fModuleEfficiencyMap[moduleAddress]->SetYTitle("#");
	title.Form("M%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleMultiPointMap[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleMultiPointMap[moduleAddress]->SetXTitle("multi point per channel / all points [%]");
	fModuleMultiPointMap[moduleAddress]->SetYTitle("#");
	title.Form("D%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleDeltaEMap[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleDeltaEMap[moduleAddress]->SetXTitle("#delta-electrons [%]");
	fModuleDeltaEMap[moduleAddress]->SetYTitle("#");
	title.Form("TL%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleTracklengthMap[moduleAddress]  = new TH1I(title,title,200,0,10);
	fModuleTracklengthMap[moduleAddress]->SetXTitle("track length [cm]");
	fModuleTracklengthMap[moduleAddress]->SetYTitle("#");
	title.Form("dE%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuledEdxMap[moduleAddress]  = new TH1I(title,title,200,0,50);
	fModuledEdxMap[moduleAddress]->SetXTitle("#LTdEdx#GT [keV]");
	fModuledEdxMap[moduleAddress]->SetYTitle("#");
	title.Form("CS%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleClusterSizeMap[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleClusterSizeMap[moduleAddress]->SetXTitle("cluster size [channel]");
	fModuleClusterSizeMap[moduleAddress]->SetYTitle("#");
	title.Form("TA%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleTrackableMap[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleTrackableMap[moduleAddress]->SetXTitle("min. 5 STS points [%]");
	fModuleTrackableMap[moduleAddress]->SetYTitle("#");
	title.Form("TA2%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleTrackableMap2[moduleAddress]  = new TH1I(title,title,200,0,200);
	fModuleTrackableMap2[moduleAddress]->SetXTitle("min. 5 STS points [#]");
	fModuleTrackableMap2[moduleAddress]->SetYTitle("#");
	title.Form("AP%i_S%i_L%i_(%.2f, %.2f, %.2f)",moduleAddress,Station,Layer,fModuleInfo->GetX(),fModuleInfo->GetY(),fModuleInfo->GetZ());
	fModuleAveragePointsMap[moduleAddress]  = new TH1I(title,title,500,0,500);
	fModuleAveragePointsMap[moduleAddress]->SetXTitle("average points per event");
	fModuleAveragePointsMap[moduleAddress]->SetYTitle("#");

	deltaEMap[moduleAddress] = 0;
	TrackableMap[moduleAddress] = 0;
      }
  
      Double_t p_global[3] = {0.5 * (point->GetXOut() + point->GetXIn()),
			      0.5 * (point->GetYOut() + point->GetYIn()),
			      0.5 * (point->GetZOut() + point->GetZIn())};
	
      gGeoManager->FindNode(p_global[0], p_global[1], p_global[2]);
      if (!TString(gGeoManager->GetPath()).Contains("gas")){
	cout << gGeoManager->GetPath() << endl;
	continue;
      }
      iTrack = point->GetTrackID();
      track = (CbmMCTrack*) fMCTracks->At(iTrack);

      Int_t Pdg_code = track->GetPdgCode();
      if (hasBeenUsedTrack.find(iTrack) != hasBeenUsedTrack.end()) {
	//hasBeenUsedTrack[iTrack] += 1;//continue;
	if (NULL != fRadiator)
	  if (fRadiator->LatticeHit(point))
	    hasBeenUsedTrack[iTrack] -= 1;
      } else {
	hasBeenUsedTrack[iTrack] = track->GetNPoints(kTRD);
	fTrdPointsPerMcTrack_PID->Fill(track->GetNPoints(kTRD), CbmTrdUtils::PdgToGeant(Pdg_code));
	fTrdPointsPerMcTrack_PT->Fill(track->GetNPoints(kTRD), track->GetPt());
	fTrdPointsPerMcTrack_P->Fill(track->GetNPoints(kTRD), track->GetP());
      }


      Double_t trackLength = GetTrackLength(point);
      /*
	TMath::Sqrt(
	(point->GetXOut() - point->GetXIn()) * (point->GetXOut() - point->GetXIn()) + 
	(point->GetYOut() - point->GetYIn()) * (point->GetYOut() - point->GetYIn()) + 
	(point->GetZOut() - point->GetZIn()) * (point->GetZOut() - point->GetZIn())
	);
      */
      if (Int_t(trackLength / 1.0 + 0.9) < 1){// Track length threshold of minimum 0.1cm track length in gas volume
	deltaEMap[moduleAddress] += 1;	
	continue;
      }
      if (nStsPoints >= 5)
	TrackableMap[moduleAddress] += 1;

      //track = (CbmMCTrack*) fMCTracks->At(point->GetTrackID());
      Int_t sectorId(0), columnId(0), rowId(0);
      fModuleInfo->GetPadInfo(point, sectorId, columnId, rowId);
      //printf("L:%i, MA:%i, S:%i R:%i, C:%i\n",CbmTrdAddress::GetLayerId(moduleAddress),CbmTrdAddress::GetModuleId(moduleAddress),sectorId,rowId,columnId);
      Int_t channelAddress = CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(moduleAddress),CbmTrdAddress::GetModuleId(moduleAddress),sectorId,rowId,columnId);

      if (channelAddressMap.find(channelAddress) == channelAddressMap.end()){
	channelAddressMap[channelAddress] = 0;
	neighbourChannelAddressMap[channelAddress] = 0;
      }
      channelAddressMap[channelAddress] += 1;
      //neighbourChannelAddressMap[channelAddress] += 1;

      if (McTrackDoubleHitMap.find(point->GetTrackID()) == McTrackDoubleHitMap.end()){
	McTrackDoubleHitMap[point->GetTrackID()] = 0;
	McTrackNeighbourHitMap[point->GetTrackID()] = 0;
      }
      if (channelAddressMap[channelAddress] > 1){
	McTrackDoubleHitMap[point->GetTrackID()] += 1;
      }
      
      Int_t neighbourRow(-1), neighbourColumn(-1), neighbourSector(-1), neighbourChannelAddress(0);
      //printf("\nS:%3i C:%3i R:%3i\n",sectorId,columnId,rowId);
      for (Int_t r = -1; r <= 1; r++){
	for (Int_t c = -1; c <= 1; c++){
	  if (r == 0 && c == 0) continue;
	  if (r != 0 && c != 0) continue;
	  if (rowId < fModuleInfo->GetNofRowsInSector(sectorId)-1 && rowId > 0){
	    neighbourSector = sectorId;
	    neighbourRow = rowId+r;
	  } else if (rowId+r < 0 && sectorId > 0){
	    neighbourSector = sectorId-1;
	    neighbourRow = fModuleInfo->GetNofRowsInSector(neighbourSector)-1;
	  } else if (rowId == fModuleInfo->GetNofRowsInSector(sectorId)-1 && sectorId < fModuleInfo->GetNofSectors()-1){
	    neighbourSector = sectorId+1;
	    neighbourRow = 0;
	  }
	  if (columnId+c >= 0 && columnId+c <= fModuleInfo->GetNofColumnsInSector(sectorId)-1)
	    neighbourColumn = columnId + c;
	  //printf ("          S:%3i C:%3i R:%3i\n",neighbourSector,neighbourColumn,neighbourRow);
	  if (neighbourSector > -1 && neighbourRow > -1 && neighbourColumn > -1){
	    neighbourChannelAddress = CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(moduleAddress),CbmTrdAddress::GetModuleId(moduleAddress),neighbourSector,neighbourRow,neighbourColumn);
	    if (channelAddressMap.find(neighbourChannelAddress) != channelAddressMap.end()){
	      //if (neighbourChannelAddressMap.find(neighbourChannelAddress) != neighbourChannelAddressMap.end()){
	      neighbourChannelAddressMap[channelAddress] += 1;
	      neighbourChannelAddressMap[neighbourChannelAddress] += 1;	    
	      //if (neighbourChannelAddressMap[neighbourChannelAddress] > 1){
	      //if (McTrackNeighbourHitMap.find(neighbourChannelAddress) == McTrackNeighbourHitMap.end())
	      //McTrackNeighbourHitMap[point->GetTrackID()] = 0;
	      McTrackNeighbourHitMap[point->GetTrackID()] += 1;
	      //}
	    }
	  }
	}
      }
      Double_t ELoss = point->GetEnergyLoss();
      //if (ELoss < 1E-9){
      //continue;
      //}
      fModulePointMap[moduleAddress].push_back(iPoint); 
      fModuledEdxMap[moduleAddress]->Fill(ELoss*1E6);
      fModuleTracklengthMap[moduleAddress]->Fill(trackLength);

      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] =  new TCanvas(title,title,1200,1000);
	fLayerDummy->Draw("colz");
      }
    }
    for (std::map<Int_t,Int_t>::iterator it = hasBeenUsedTrack.begin(); it != hasBeenUsedTrack.end(); it++){
      track = (CbmMCTrack*) fMCTracks->At(it->first);
      fTrdTrackCrossedRadiator->Fill(track->GetNPoints(kTRD),it->second);
    }
  }

 


  // Digis
  if (fD) {
    nEntries = fDigis->GetEntries();
    printf("%i Digis \n",nEntries);
    for (Int_t iDigi=0; iDigi < nEntries; iDigi++ ) {
      digi = (CbmTrdDigi*) fDigis->At(iDigi);
      //fPointsPerDigi->Fill((Int_t)digi->GetMCIndex().size());
      fdEdxDigi->Fill(digi->GetCharge());
      moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());//digi->GetDetId();
      Station  = CbmTrdAddress::GetLayerId(moduleAddress) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Layer    = CbmTrdAddress::GetLayerId(moduleAddress) % 4 + 1;//fGeoHandler->GetLayer(moduleId);
      //printf ("D det: %i S%i L%i  \n",moduleAddress,Station,Layer);//GetDetId()); 
      combiId = 10 * Station + Layer;
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      if (fModuleDigiMap.find(moduleAddress) == fModuleDigiMap.end()){
      }
      fModuleDigiMap[moduleAddress].push_back(iDigi);
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] =  new TCanvas(title,title,1200,1000);
	fLayerDummy->Draw("colz");
      }
    }
  }
  
  //Clusters
  if (fC) {
    nEntries = fClusters->GetEntries();
    printf("%i\n",nEntries);
    for (Int_t iCluster=0; iCluster < nEntries; iCluster++ ) {
      cluster = (CbmTrdCluster*) fClusters->At(iCluster);
      Double_t charge = 0;
      fModuleClusterSizeMap[CbmTrdAddress::GetModuleAddress(((CbmTrdDigi*)fDigis->At(cluster->GetDigi(0)))->GetAddress())]->Fill(Int_t(cluster->GetNofDigis()));
      for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++){
	digi = (CbmTrdDigi*)fDigis->At(cluster->GetDigi(iDigi));
	charge += digi->GetCharge();
      }
      fdEdxCluster->Fill(charge);
      fClusterSize->Fill(cluster->GetNofDigis());
      fDigiPerCluster->Fill(cluster->GetNofDigis());
      if (cluster->GetNofDigis() >= 3) {
	Double_t qLeft(0.0), qCenter(.0), qRight(0.0);
	for (Int_t i = 0; i < cluster->GetNofDigis(); i++) {
	  digi = (CbmTrdDigi*) fDigis->At(cluster->GetDigi(i));
	 
	}
	//fPRF_2D->Fill();
	//fPRF_1D->Fill();
      }
      //moduleAddress = cluster->GetDetId();
      //Station  = fGeoHandler->GetStation(moduleAddress);
      //Layer    = fGeoHandler->GetLayer(moduleAddress);
      //printf ("det: %i S%i L%i  \n",moduleAddress,Station,Layer);//GetDetId()); 
      //combiId = 10 * Station + Layer;   
      //fModuleInfo = fDigiPar->GetModule(moduleAddress);
      //if (fModuleClusterMap.find(moduleAddress) == fModuleClusterMap.end()){
      //fModuleClusterMap[moduleAddress].push_back(iCluster);
      //}
    }
  }
  
  //Hits
  if (fH) {
    nEntries = fHits->GetEntries();
    printf("%i Hits\n",nEntries);
    for (Int_t iHit=0; iHit < nEntries; iHit++ ) {
      hit = (CbmTrdHit*) fHits->At(iHit);
      fdEdxHit->Fill(hit->GetELoss());
      moduleAddress = hit->GetAddress();//hit->GetDetId();
      Station  = CbmTrdAddress::GetLayerId(moduleAddress) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Layer    = CbmTrdAddress::GetLayerId(moduleAddress) % 4 + 1;//fGeoHandler->GetLayer(moduleId);
      combiId = 10 * Station + Layer;
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      if (fModuleHitMap.find(moduleAddress) == fModuleHitMap.end()){
      }
      fModuleHitMap[moduleAddress].push_back(iHit);
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] = new TCanvas(title,title,1200,1000);
	fLayerMap[combiId]->cd();
	fLayerDummy->Draw("colz");
      }
    }
  }
  for (std::map<Int_t, Int_t>::iterator McTrackDoubleHitMapIt = McTrackDoubleHitMap.begin(); // Loop for all module ids which have mc points
       McTrackDoubleHitMapIt != McTrackDoubleHitMap.end(); McTrackDoubleHitMapIt++) {
    fMultiHitSamePadPerMcTrack->Fill(McTrackDoubleHitMapIt->second);
  }

  for (std::map<Int_t, Int_t>::iterator McTrackNeighbourHitMapIt = McTrackNeighbourHitMap.begin();
       McTrackNeighbourHitMapIt != McTrackNeighbourHitMap.end(); McTrackNeighbourHitMapIt++) {
    fMultiHitAdjacentPadPerMcTrack->Fill(McTrackNeighbourHitMapIt->second);
  }
  //printf(Int_t(fModulePointMapIt->second.size()),Int_t(->second.size()),Int_t(->second.size()));
  std::map<Int_t, Int_t> notMultiPointTRDPoints;
  Int_t allPoints(0), allHits(0);//, notMultiPointTRDPoints(0);
  for (fModulePointMapIt = fModulePointMap.begin(); // Loop for all module ids which have mc points
       fModulePointMapIt != fModulePointMap.end(); fModulePointMapIt++) {
    if (fModuleHitMap.find(fModulePointMapIt->first) == fModuleHitMap.end()) {
      printf("module %i not found in hit map\n",fModulePointMapIt->first);
    } else {
      std::map<Int_t, Int_t> hasBeenUsed;
      moduleAddress = fModulePointMapIt->first;
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      allPoints = Int_t(fModulePointMapIt->second.size());
      fModuleAveragePointsMap[moduleAddress]->Fill(allPoints);
      allHits = Int_t(fModuleHitMap[moduleAddress].size());
      fModuleDeltaEMap[moduleAddress]->Fill(100. * deltaEMap[moduleAddress] / allPoints);
      fModuleTrackableMap[moduleAddress]->Fill(100. * TrackableMap[moduleAddress] / allPoints);
      fModuleTrackableMap2[moduleAddress]->Fill(TrackableMap[moduleAddress]);
      //fModuleHitMapIt = fModuleHitMap[fModulePointMapIt->first];
      //cout << "Module: " << moduleAddress << "   Points: "<< fModulePointMapIt->second.size() << endl;
     
      Double_t multiHitCounterModule = 0;
      Double_t r = sqrt(fModuleInfo->GetX() * fModuleInfo->GetX() + fModuleInfo->GetY() * fModuleInfo->GetY()); // radial position of the module center
      Double_t alpha = atan(r / fModuleInfo->GetZ()) * 1000.; //[mrad]
      //printf("%.2f %.2f %.2f %.2f %.2f\n",fModuleInfo->GetX(), fModuleInfo->GetY(), fModuleInfo->GetZ(), r, alpha);
      //for (Int_t iPoint = 0; iPoint < Int_t(fModulePointMapIt->second.size()); iPoint++) {
      for (Int_t iPoint = Int_t(fModulePointMapIt->second.size())-1; iPoint >= 0; iPoint--) { // its better to go backwards since found point and hit pairs are erased -> vector sice is dynamic
	//hasBeenUsed[fModulePointMapIt->second[iPoint]] = 0;
	point = (CbmTrdPoint*) fPoints->At(fModulePointMapIt->second[iPoint]);
	iTrack = point->GetTrackID();


	track = (CbmMCTrack*) fMCTracks->At(iTrack);
	mother_iTrack = track->GetMotherId();
	if (mother_iTrack >= 0)
	  track_mother = (CbmMCTrack*) fMCTracks->At(mother_iTrack);
	else
	  track_mother = NULL;
	if (notMultiPointTRDPoints.find(iTrack) == notMultiPointTRDPoints.end()){
	  //if (track->GetNPoints(kSTS) > 0)
	  {	
	    Int_t nStsPoint = track->GetNPoints(kSTS);
	    fStsTrdPoints->Fill(track->GetNPoints(kTRD),track->GetNPoints(kSTS));
	    fStsMuchPoints->Fill(track->GetNPoints(kMUCH),track->GetNPoints(kSTS));
	    fStsTofPoints->Fill(track->GetNPoints(kTOF),track->GetNPoints(kSTS));
	    if (nStsPoint < 11){
	      fMuchTrdPoints[nStsPoint]->Fill(track->GetNPoints(kTRD),track->GetNPoints(kMUCH));
	      fMuchTofPoints[nStsPoint]->Fill(track->GetNPoints(kTOF),track->GetNPoints(kMUCH));
	      fTrdTofPoints[nStsPoint]->Fill(track->GetNPoints(kTOF),track->GetNPoints(kTRD));
	    }
	    notMultiPointTRDPoints[iTrack] = track->GetNPoints(kTRD);       
	  }
	}
	Int_t Pdg_code = track->GetPdgCode();

	/*
	  Double_t p_global[3] = {0.5 * (point->GetXIn() + point->GetXOut()),
	  0.5 * (point->GetYIn() + point->GetYOut()),
	  0.5 * (point->GetZIn() + point->GetZOut())};
	*/
	
	Double_t p_global[3] = {0.5 * (point->GetXOut() + point->GetXIn()),
				0.5 * (point->GetYOut() + point->GetYIn()),
				0.5 * (point->GetZOut() + point->GetZIn())};
	
	gGeoManager->FindNode(p_global[0], p_global[1], p_global[2]);
	const Double_t *global_point = gGeoManager->GetCurrentPoint();
	Double_t p_local[3] = {0.0};
	gGeoManager->MasterToLocal(p_global, p_local);
	//p_local[0] = p_global[0] - fModuleInfo->GetX();
	//p_local[1] = p_global[1] - fModuleInfo->GetY();

	Int_t xPpad(0), yPpad(0), pSectorId(-1); // correspoondig digi from point point of view

	fModuleInfo->GetPadInfo(p_local, pSectorId, xPpad, yPpad);
	yPpad = fModuleInfo->GetModuleRow(pSectorId, yPpad);

	for (Int_t jPoint = 0; jPoint < iPoint; jPoint++) {
	  point2 = (CbmTrdPoint*) fPoints->At(fModulePointMapIt->second[jPoint]);
	  Int_t jTrack = point2->GetTrackID();
	  track2 = (CbmMCTrack*) fMCTracks->At(jTrack);
	  Int_t mother_jTrack = track2->GetMotherId();
	  if (mother_jTrack >= 0)
	    track2_mother = (CbmMCTrack*) fMCTracks->At(mother_jTrack);
	  else
	    track2_mother = NULL;
	  if (notMultiPointTRDPoints.find(jTrack) == notMultiPointTRDPoints.end()){
	    //if (track2->GetNPoints(kSTS) > 0)
	    {
	      Int_t nStsPoint = track2->GetNPoints(kSTS);
	      fStsTrdPoints->Fill(track2->GetNPoints(kTRD),track2->GetNPoints(kSTS));
	      fStsMuchPoints->Fill(track2->GetNPoints(kMUCH),track2->GetNPoints(kSTS));
	      fStsTofPoints->Fill(track2->GetNPoints(kTOF),track2->GetNPoints(kSTS));
	      if (nStsPoint < 11){
		fMuchTrdPoints[nStsPoint]->Fill(track2->GetNPoints(kTRD),track2->GetNPoints(kMUCH));
		fMuchTofPoints[nStsPoint]->Fill(track2->GetNPoints(kTOF),track2->GetNPoints(kMUCH));
		fTrdTofPoints[nStsPoint]->Fill(track2->GetNPoints(kTOF),track2->GetNPoints(kTRD));
	      }
	      notMultiPointTRDPoints[jTrack] = track2->GetNPoints(kTRD);
	    }
	  }

	  Double_t p_global2[3] = {0.5 * (point2->GetXOut() + point2->GetXIn()),
				   0.5 * (point2->GetYOut() + point2->GetYIn()),
				   0.5 * (point2->GetZOut() + point2->GetZIn())};
	
	  gGeoManager->FindNode(p_global2[0], p_global2[1], p_global2[2]);
	  const Double_t *global_point2 = gGeoManager->GetCurrentPoint();
	  Double_t p_local2[3] = {0.0};
	  gGeoManager->MasterToLocal(p_global2, p_local2);
	  Int_t xPpad2(0), yPpad2(0), pSectorId2(-1); // correspoondig digi from point point of view

	  fModuleInfo->GetPadInfo(p_local2, pSectorId2, xPpad2, yPpad2);
	  yPpad2 = fModuleInfo->GetModuleRow(pSectorId2, yPpad2);
	 
	  if ((xPpad2 == xPpad) && (yPpad2 == yPpad)){ // identical pad
	    //Double_t angle = CalcAngle(point, point2);
	    //cout << angle*1000. << "mrad   "  <<  angle / TMath::Pi() * 180. << "deg" << endl;
	    fMultiHitSamePadPerMcTrack_angle->Fill(CalcAngle(point, point2));
	    fMultiHitSamePadPerMcTrack_zBirth->Fill(track->GetStartZ(), track2->GetStartZ());
	    fMultiHitSamePadPerMcTrack_PID->Fill(CbmTrdUtils::PdgToGeant(track->GetPdgCode()), CbmTrdUtils::PdgToGeant(track2->GetPdgCode()));
	    if (NULL != track_mother && NULL != track2_mother){
	      fMultiHitSamePadPerMcTrack_motherzBirth->Fill(track_mother->GetStartZ(), track2_mother->GetStartZ());
	      fMultiHitSamePadPerMcTrack_motherPID->Fill(CbmTrdUtils::PdgToGeant(track_mother->GetPdgCode()), CbmTrdUtils::PdgToGeant(track2_mother->GetPdgCode()));
	    }
	    if (hasBeenUsed.find(fModulePointMapIt->second[iPoint]) == hasBeenUsed.end()){
	      hasBeenUsed[fModulePointMapIt->second[iPoint]] = 1;
	      multiHitCounterModule++;
	      if (notMultiPointTRDPoints.find(iTrack) != notMultiPointTRDPoints.end())
		if (notMultiPointTRDPoints[iTrack] > 0)
		  notMultiPointTRDPoints[iTrack]--;
	    }
	    if (hasBeenUsed.find(fModulePointMapIt->second[jPoint]) == hasBeenUsed.end()){
	      hasBeenUsed[fModulePointMapIt->second[jPoint]] += 1;
	      multiHitCounterModule++;
	      if (notMultiPointTRDPoints.find(jTrack) != notMultiPointTRDPoints.end())
		if (notMultiPointTRDPoints[jTrack] > 0)
		  notMultiPointTRDPoints[jTrack]--;
	    }
	  } else  if ((fabs(xPpad - xPpad2) <= 1 && fabs(yPpad - yPpad2) <= 1))  { // surrounding neighbour pads
	    if ((xPpad2 == xPpad) || (yPpad2 == yPpad)){ // exclude diagonal
	      fMultiHitAdjacentPadPerMcTrack_angle->Fill(CalcAngle(point, point2));
	      fMultiHitAdjacentPadPerMcTrack_zBirth->Fill(track->GetStartZ(), track2->GetStartZ());
	      fMultiHitAdjacentPadPerMcTrack_PID->Fill(CbmTrdUtils::PdgToGeant(track->GetPdgCode()), CbmTrdUtils::PdgToGeant(track2->GetPdgCode()));
	      if (NULL != track_mother && NULL != track2_mother){
		fMultiHitAdjacentPadPerMcTrack_motherzBirth->Fill(track_mother->GetStartZ(), track2_mother->GetStartZ());
		fMultiHitAdjacentPadPerMcTrack_motherPID->Fill(CbmTrdUtils::PdgToGeant(track_mother->GetPdgCode()), CbmTrdUtils::PdgToGeant(track2_mother->GetPdgCode()));
	      }
	      if (hasBeenUsed.find(fModulePointMapIt->second[iPoint]) == hasBeenUsed.end()){
		hasBeenUsed[fModulePointMapIt->second[iPoint]] = 1;
		multiHitCounterModule++;
	      }
	      if (hasBeenUsed.find(fModulePointMapIt->second[jPoint]) == hasBeenUsed.end()){
		hasBeenUsed[fModulePointMapIt->second[jPoint]] += 1;
		multiHitCounterModule++;
	      } 
	    } else {
	      //fStsTrdPointsTrackable->Fill(track->GetNPoints(kTRD),track->GetNPoints(kSTS));
	    }
	  } else {
	    //fStsTrdPointsTrackable->Fill(track->GetNPoints(kTRD),track->GetNPoints(kSTS));
	  }
	}
	Double_t xPPadSize(fModuleInfo->GetPadSizeX(pSectorId)), yPPadSize(fModuleInfo->GetPadSizeY(pSectorId)); // correspoondig pad sizes from point point of view 
	//GetPadInfos( moduleAddress, p_local[0], p_local[1], xPpad, yPpad, xPPadSize, yPPadSize);
	if (point->GetEnergyLoss() > 0.0) {
	  if (Pdg_code == 211 || Pdg_code == -211)
	    fdEdxPionMc->Fill(point->GetEnergyLoss());
	  if (Pdg_code == 11 || Pdg_code == -11 )
	    fdEdxElectronMc->Fill(point->GetEnergyLoss());
	}
	Double_t minimumDistance = sqrt(xPPadSize * xPPadSize + yPPadSize * yPPadSize);
	Double_t xDiviation(0), yDiviation(0);
	Int_t minDHitId = -1;
	Double_t mergedELoss = 0.0;
	Double_t hitELoss = 0.0;
	Int_t multiHitCounter = 0;
	for (Int_t iHit = 0; iHit < Int_t(fModuleHitMap[moduleAddress].size()); iHit++) {
	  hit = (CbmTrdHit*) fHits->At(fModuleHitMap[moduleAddress][iHit]);
	  Double_t h_global[3] = {hit->GetX(),
				  hit->GetY(),
				  hit->GetZ()};
	  gGeoManager->FindNode(h_global[0], h_global[1], h_global[2]);
	  const Double_t *global_hit = gGeoManager->GetCurrentPoint();
	  Double_t h_local[3] = {0.0};
	  gGeoManager->MasterToLocal(h_global, h_local);
	  //h_local[0] = h_global[0] - fModuleInfo->GetX();
	  //h_local[1] = h_global[1] - fModuleInfo->GetY();
	  Int_t xHpad(0), yHpad(0), hSectorId(-1); // correspoondig digi from hit point of view
	  fModuleInfo->GetPadInfo(h_local, hSectorId, xHpad, yHpad);
	  yHpad = fModuleInfo->GetModuleRow(hSectorId, yHpad);
	  Double_t xHPadSize(fModuleInfo->GetPadSizeX(hSectorId)), yHPadSize(fModuleInfo->GetPadSizeY(hSectorId)); // correspoondig pad sizes from point point of view 
	  //Double_t xHPadSize(0), yHPadSize(0); // correspoondig pad sizes from hit point of view
	  //GetPadInfos( moduleAddress, h_local[0], h_local[1], xHpad, yHpad, xHPadSize, yHPadSize);
	  //point_temp = new CbmTrdPoint(iTrack, hit->GetDetId(), );
	  //r = sqrt(h_global[0] * h_global[0] + h_global[1] * h_global[1]);
	  //r = sqrt(hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY());
	  Double_t distance = sqrt(
				   (p_local[0]-h_local[0]) * (p_local[0]-h_local[0]) 
				   + 
				   (p_local[1]-h_local[1]) * (p_local[1]-h_local[1]));
	  fDistanceMcToHitAll->Fill(distance);
	  //if (xHpad == xPpad && yHpad == yPpad)	  {
	  if (distance < minimumDistance || (xHpad == xPpad && yHpad == yPpad)) { // distance between point and hit is smaller one pad diagonal or on the same pad
	    //if (distance < minimumDistance){
	    minimumDistance = distance;
	    minDHitId = iHit;
	    xDiviation = p_local[0]-h_local[0];
	    yDiviation = p_local[1]-h_local[1];
	    hitELoss = hit->GetELoss();
	    /*
	      if (xHpad == xPpad && yHpad == yPpad){
	      multiHitCounter++;
	      if (samePadMerge)
	      mergedELoss += hit->GetELoss();
	      }
	    */
	  }
	}
	
	if (minDHitId > -1){ // If minimumDistance < pad diagonal -> delete point and hit from map
	  //multiHitCounterModule += multiHitCounter / Float_t(fModulePointMapIt->second.size());
	  fModuleHitMap[moduleAddress].erase(fModuleHitMap[moduleAddress].begin() + minDHitId);
	  fModulePointMapIt->second.erase(fModulePointMapIt->second.begin() + iPoint);
	  if (samePadMerge){
	    //if (mergedELoss == 0.0)
	    //printf("(%02i, %02i) (%02i, %02i) %.2e  %.2e\n",xPpad, yPpad, fModuleInfo->GetnCol(), fModuleInfo->GetnRow(), mergedELoss, point->GetEnergyLoss());
	    if (Pdg_code == 211 || Pdg_code == -211) // pdg code from associated point is used
	      fdEdxPionHit->Fill(mergedELoss);
	    if (Pdg_code == 11 || Pdg_code == -11 )
	      fdEdxElectronHit->Fill(mergedELoss);
	  }
	  else {
	    if (Pdg_code == 211 || Pdg_code == -211) // pdg code from associated point is used
	      fdEdxPionHit->Fill(hitELoss);
	    if (Pdg_code == 11 || Pdg_code == -11 )
	      fdEdxElectronHit->Fill(hitELoss);
	  }
	  fDistanceMcToHit->Fill(minimumDistance);
	  if (xPPadSize > yPPadSize) { // ok to use here only the pad size of the point because the sapect ration does not change within one module
	    fPositionResolutionShort->Fill(yDiviation);
	    fPositionResolutionLong->Fill(xDiviation);
	  }
	  else {
	    fPositionResolutionShort->Fill(xDiviation);
	    fPositionResolutionLong->Fill(yDiviation);
	  }
	}		
      }
    
      fMultiHitsVsAlpha->Fill(alpha, 100. * multiHitCounterModule / Float_t(allPoints));
      fMultiHitsVsR->Fill(r ,100. * multiHitCounterModule / Float_t(allPoints));
      // left over points == lost
      fModuleLostMap[moduleAddress]->Fill( 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      fLostPointVsR->Fill(r, 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      fLostPointVsAlpha->Fill(alpha, 100. * Float_t(fModulePointMapIt->second.size()) / Float_t(allPoints));
      // left over hits == ghosts
      fModuleGhostMap[moduleAddress]->Fill( 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      fGhostHitVsR->Fill(r, 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      fGhostHitVsAlpha->Fill(alpha, 100. * Float_t(fModuleHitMap[fModulePointMapIt->first].size()) / Float_t(allPoints));
      // found point hit pairs / all points == detection efficiency
      fModuleEfficiencyMap[moduleAddress]->Fill(100. * ( allPoints - Float_t(fModulePointMapIt->second.size() )) / Float_t(allPoints));
      fHitToPointEfficiencyVsR->Fill(r, 100. * ( allPoints - Float_t(fModulePointMapIt->second.size() )) / Float_t(allPoints));
      fHitToPointEfficiencyVsAlpha->Fill(alpha, 100. * ( allPoints - Float_t(fModulePointMapIt->second.size() )) / Float_t(allPoints));

      fModuleMultiPointMap[moduleAddress]->Fill(100. * (Float_t(multiHitCounterModule)) / Float_t(allPoints));
    }
  }
  for (std::map<Int_t, Int_t>::iterator notMultiPointTRDPointsIt = notMultiPointTRDPoints.begin(); notMultiPointTRDPointsIt != notMultiPointTRDPoints.end(); notMultiPointTRDPointsIt++){
    track = (CbmMCTrack*)(fMCTracks->At(notMultiPointTRDPointsIt->first));     
    fStsTrdPointsTrackable->Fill(notMultiPointTRDPointsIt->second, track->GetNPoints(kSTS));
  }
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");
}
void CbmTrdQa::SetTriangularPads(Bool_t triangles)
{
  fTrianglePads = triangles;
}


void CbmTrdQa::SaveHistos()
{
  TString origpath = gDirectory->GetPath();
  printf ("\n%s\n",origpath.Data());
  TString newpath = origpath;
  newpath.ReplaceAll("eds","trd_qa");
  newpath.ReplaceAll(":/","");
  TFile *tempFile = new TFile(newpath,"recreate");
  gDirectory = tempFile->CurrentDirectory();
  gDirectory->pwd();

  TString title;
  title.Form("_TH_%.2EGeV_",fTriggerThreshold);
  gDirectory->pwd();
  if (!gDirectory->Cd("TrdQa")) 
    gDirectory->mkdir("TrdQa");
  gDirectory->Cd("TrdQa");
  TCanvas *c = new TCanvas("c","c",800,600);
  c->cd();
  TLegend *l = new TLegend(0.65,0.65,0.85,0.85);
  l->SetTextSize(0.025);
  l->SetTextColor(1);
  l->SetFillStyle(0);
  l->SetLineStyle(0);
  l->SetLineColor(0);
  fdEdxPionMc->DrawCopy();
  l->AddEntry(fdEdxElectronMc,"MC electron","l");
  fdEdxElectronMc->DrawCopy("same");
  l->AddEntry(fdEdxElectronHit,"Hit electron","l");
  fdEdxElectronHit->DrawCopy("same");
  l->AddEntry(fdEdxPionMc,"MC pion","l");
  fdEdxPionMc->DrawCopy("same");
  l->AddEntry(fdEdxPionHit,"Hit pion","l");
  fdEdxPionHit->DrawCopy("same");
  l->Draw("same");
  c->SaveAs("pics/TrdQadEdx"+title+".pdf");
  c->SaveAs("pics/TrdQadEdx"+title+".png");
  /*
    delete l;
    TLegend *l = new TLegend(0.65,0.65,0.85,0.85);
    l->SetTextSize(0.025);
    l->SetTextColor(1);
    l->SetFillStyle(0);
    l->SetLineStyle(0);
    l->SetLineColor(0);
  */
  l->Clear();
  l->AddEntry(fHitToPointEfficiencyVsR,"Hit to point efficiency","p");
  fHitToPointEfficiencyVsR->DrawCopy();
  l->AddEntry(fGhostHitVsR,"Ghost hits","p");
  fGhostHitVsR->DrawCopy("same");
  l->AddEntry(fLostPointVsR,"Lost points","p");
  fLostPointVsR->DrawCopy("same");
  l->AddEntry(fMultiHitsVsR,"Multi points","p");
  fMultiHitsVsR->DrawCopy("same");
  l->Draw("same");
  c->SaveAs("pics/TrdQaStatisticVsR"+title+".pdf");
  c->SaveAs("pics/TrdQaStatisticVsR"+title+".png");
  fHitToPointEfficiencyVsAlpha->DrawCopy();
  fGhostHitVsAlpha->DrawCopy("same");
  fLostPointVsAlpha->DrawCopy("same");
  fMultiHitsVsAlpha->DrawCopy("same");
  l->Draw("same");
  c->SaveAs("pics/TrdQaStatisticVsAlpha"+title+".pdf");
  c->SaveAs("pics/TrdQaStatisticVsAlpha"+title+".png");
  gDirectory->pwd();
  fdEdxPoint->Write("", TObject::kOverwrite);
  fdEdxDigi->Write("", TObject::kOverwrite);
  c->cd()->SetLogx(1);
  c->cd()->SetLogy(1);
  fdEdxDigi->Draw();
  c->SaveAs("pics/TrdQadEdxDigi.pdf");
  c->SaveAs("pics/TrdQadEdxDigi.png");
  fdEdxCluster->Write("", TObject::kOverwrite);
  fdEdxCluster->Draw();
  c->SaveAs("pics/TrdQadEdxCluster"+title+".pdf");
  c->SaveAs("pics/TrdQadEdxCluster"+title+".png");
  c->cd()->SetLogx(0);
  c->cd()->SetLogy(0);
  fdEdxHit->Write("", TObject::kOverwrite);
  fdEdxPionMc->Write("", TObject::kOverwrite);
  fdEdxPionHit->Write("", TObject::kOverwrite);
  fdEdxPionGhost->Write("", TObject::kOverwrite);
  fdEdxElectronMc->Write("", TObject::kOverwrite);
  fdEdxElectronHit->Write("", TObject::kOverwrite);
  fdEdxElectronGhost->Write("", TObject::kOverwrite);
  fDistanceMcToHit->Write("", TObject::kOverwrite);
  fDistanceMcToHitAll->Write("", TObject::kOverwrite);
  fMultiHitsVsR->Write("", TObject::kOverwrite);
  fGhostHitVsR->Write("", TObject::kOverwrite);
  fLostPointVsR->Write("", TObject::kOverwrite);
  fHitToPointEfficiencyVsR->Write("", TObject::kOverwrite);
  fMultiHitsVsAlpha->Write("", TObject::kOverwrite);
  fGhostHitVsAlpha->Write("", TObject::kOverwrite);
  fLostPointVsAlpha->Write("", TObject::kOverwrite);
  fHitToPointEfficiencyVsAlpha->Write("", TObject::kOverwrite);
  fPositionResolutionShort->Write("", TObject::kOverwrite);
  fPositionResolutionShort->DrawCopy();
  c->SaveAs("pics/TrdQaPositionResolutionShort"+title+".pdf");
  c->SaveAs("pics/TrdQaPositionResolutionShort"+title+".png");
  fPositionResolutionLong->Write("", TObject::kOverwrite);
  fPositionResolutionLong->DrawCopy();
  c->SaveAs("pics/TrdQaPositionResolutionLong"+title+".pdf");
  c->SaveAs("pics/TrdQaPositionResolutionLong"+title+".png");
  fClusterSize->Write("", TObject::kOverwrite);
  fPointsPerDigi->Write("", TObject::kOverwrite);
  fDigiPerCluster->Write("", TObject::kOverwrite);
  fClusterPerHit->Write("", TObject::kOverwrite);
  fPRF_1D->Write("", TObject::kOverwrite);
  fPRF_2D->Write("", TObject::kOverwrite);
  c->SetLogy(1);
  fMultiHitSamePadPerMcTrack->DrawCopy();
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack"+title+".png");
  fMultiHitSamePadPerMcTrack->Write("", TObject::kOverwrite);
  c->SetLogx(1);
  fMultiHitSamePadPerMcTrack_angle->DrawCopy();
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_angle"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_angle"+title+".png");
  fMultiHitSamePadPerMcTrack_angle->Write("", TObject::kOverwrite);
  c->SetLogx(0);
  c->SetLogy(0);
  c->SetLogz(1);
  fMultiHitSamePadPerMcTrack_zBirth->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_zBirth"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_zBirth"+title+".png");
  fMultiHitSamePadPerMcTrack_zBirth->Write("", TObject::kOverwrite);
  fMultiHitSamePadPerMcTrack_motherzBirth->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_motherzBirth"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_motherzBirth"+title+".png");
  fMultiHitSamePadPerMcTrack_motherzBirth->Write("", TObject::kOverwrite);
  c->SetLogz(0);
  fMultiHitSamePadPerMcTrack_PID->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_PID"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_PID"+title+".png");
  fMultiHitSamePadPerMcTrack_PID->Write("", TObject::kOverwrite);
  fMultiHitSamePadPerMcTrack_motherPID->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_motherPID"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitSamePadPerMcTrack_motherPID"+title+".png");
  fMultiHitSamePadPerMcTrack_motherPID->Write("", TObject::kOverwrite);
  c->SetLogz(1);
  c->SetLogy(1);
  fMultiHitAdjacentPadPerMcTrack->DrawCopy();
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack"+title+".png");
  fMultiHitAdjacentPadPerMcTrack->Write("", TObject::kOverwrite);
  c->SetLogx(1);
  fMultiHitAdjacentPadPerMcTrack_angle->DrawCopy();
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_angle"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_angle"+title+".png");
  fMultiHitAdjacentPadPerMcTrack_angle->Write("", TObject::kOverwrite);
  c->SetLogx(0);
  c->SetLogy(0);
  c->SetLogz(1);
  fMultiHitAdjacentPadPerMcTrack_zBirth->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_zBirth"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_zBirth"+title+".png");
  fMultiHitAdjacentPadPerMcTrack_zBirth->Write("", TObject::kOverwrite);
  fMultiHitAdjacentPadPerMcTrack_motherzBirth->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_motherzBirth"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_motherzBirth"+title+".png");
  fMultiHitAdjacentPadPerMcTrack_motherzBirth->Write("", TObject::kOverwrite);
  c->SetLogz(0);
  fMultiHitAdjacentPadPerMcTrack_motherPID->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_motherPID"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_motherPID"+title+".png");
  fMultiHitAdjacentPadPerMcTrack_motherPID->Write("", TObject::kOverwrite);
  fMultiHitAdjacentPadPerMcTrack_PID->DrawCopy("colz");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_PID"+title+".pdf");
  c->SaveAs("pics/TrdQaMultiHitAdjacentPadPerMcTrack_PID"+title+".png");
  fMultiHitAdjacentPadPerMcTrack_PID->Write("", TObject::kOverwrite);
  c->SetLogz(1);
  printf("------------->>>>>>     TRD tracks:%i               TRD tracks trackable:%i     <<<<<<-------------\n",(Int_t)fStsTrdPoints->GetEntries(),(Int_t)fStsTrdPointsTrackable->GetEntries());
  fStsTrdPoints->Scale(100./fStsTrdPoints->GetEntries());
  fStsTrdPoints->GetZaxis()->SetRangeUser(0,100);//100./fStsTrdPoints->GetEntries(),fStsTrdPoints->GetBinContent(fStsTrdPoints->GetMaximumBin()));
  fStsTrdPoints->DrawCopy("colz,text");
  c->SaveAs("pics/TrdQaStsTrdPoints"+title+".pdf");
  c->SaveAs("pics/TrdQaStsTrdPoints"+title+".png");
  fStsTrdPoints->Write("", TObject::kOverwrite);

  fStsMuchPoints->Scale(100./fStsMuchPoints->GetEntries());
  fStsMuchPoints->GetZaxis()->SetRangeUser(0,100);//100./fStsMuchPoints->GetEntries(),fStsMuchPoints->GetBinContent(fStsMuchPoints->GetMaximumBin()));
  fStsMuchPoints->DrawCopy("colz,text");
  c->SaveAs("pics/TrdQaStsMuchPoints"+title+".pdf");
  c->SaveAs("pics/TrdQaStsMuchPoints"+title+".png");
  fStsMuchPoints->Write("", TObject::kOverwrite);

  fStsTofPoints->Scale(100./fStsTofPoints->GetEntries());
  fStsTofPoints->GetZaxis()->SetRangeUser(0,100);//100./fStsTofPoints->GetEntries(),fStsTofPoints->GetBinContent(fStsTofPoints->GetMaximumBin()));
  fStsTofPoints->DrawCopy("colz,text");
  c->SaveAs("pics/TrdQaStsTofPoints"+title+".pdf");
  c->SaveAs("pics/TrdQaStsTofPoints"+title+".png");
  fStsTofPoints->Write("", TObject::kOverwrite);
  TString name;
  for (Int_t iStsPoints = 0; iStsPoints < 11; iStsPoints++){
    name.Form("_%02iStsPoints",iStsPoints);
    fMuchTrdPoints[iStsPoints]->Scale(100./fMuchTrdPoints[iStsPoints]->GetEntries());
    fMuchTrdPoints[iStsPoints]->GetZaxis()->SetRangeUser(0,100);//100./fMuchTrdPoints[iStsPoints]->GetEntries(),
    fMuchTrdPoints[iStsPoints]->DrawCopy("colz,text");
    c->SaveAs("pics/TrdQaMuchTrdPoints"+name+title+".pdf");
    c->SaveAs("pics/TrdQaMuchTrdPoints"+name+title+".png");
    fMuchTrdPoints[iStsPoints]->Write("", TObject::kOverwrite);

    name.Form("_%iStsPoints",iStsPoints);
    fMuchTofPoints[iStsPoints]->Scale(100./fMuchTofPoints[iStsPoints]->GetEntries());
    fMuchTofPoints[iStsPoints]->GetZaxis()->SetRangeUser(0,100);//100./fMuchTofPoints[iStsPoints]->GetEntries(),
    fMuchTofPoints[iStsPoints]->DrawCopy("colz,text");
    c->SaveAs("pics/TrdQaMuchTofPoints"+name+title+".pdf");
    c->SaveAs("pics/TrdQaMuchTofPoints"+name+title+".png");
    fMuchTofPoints[iStsPoints]->Write("", TObject::kOverwrite);

    name.Form("_%iStsPoints",iStsPoints);
    fTrdTofPoints[iStsPoints]->Scale(100./fTrdTofPoints[iStsPoints]->GetEntries());
    fTrdTofPoints[iStsPoints]->GetZaxis()->SetRangeUser(0,100);//100./fTrdTofPoints[iStsPoints]->GetEntries(),
    fTrdTofPoints[iStsPoints]->DrawCopy("colz,text");
    c->SaveAs("pics/TrdQaTrdTofPoints"+name+title+".pdf");
    c->SaveAs("pics/TrdQaTrdTofPoints"+name+title+".png");
    fTrdTofPoints[iStsPoints]->Write("", TObject::kOverwrite);
  }


  fStsTrdPointsTrackable->Scale(100./fStsTrdPointsTrackable->GetEntries());
  fStsTrdPointsTrackable->GetZaxis()->SetRangeUser(100./fStsTrdPoints->GetEntries(),fStsTrdPoints->GetBinContent(fStsTrdPoints->GetMaximumBin())); // same scale for both histos
  fStsTrdPointsTrackable->DrawCopy("colz,text");
  c->SaveAs("pics/TrdQaStsTrdPointsTrackable"+title+".pdf");
  c->SaveAs("pics/TrdQaStsTrdPointsTrackable"+title+".png");
  fStsTrdPointsTrackable->Write("", TObject::kOverwrite);

  fTrdPointsPerMcTrack_PID->Write("", TObject::kOverwrite);
  fTrdPointsPerMcTrack_PID->DrawCopy("colz");
  c->SaveAs("pics/TrdQaPointsPerMcTrack_PID"+title+".pdf");
  c->SaveAs("pics/TrdQaPointsPerMcTrack_PID"+title+".png");
  fTrdPointsPerMcTrack_PT->Write("", TObject::kOverwrite);
  fTrdPointsPerMcTrack_PT->DrawCopy("colz");
  c->SaveAs("pics/TrdQaPointsPerMcTrack_PT"+title+".pdf");
  c->SaveAs("pics/TrdQaPointsPerMcTrack_PT"+title+".png");
  fTrdPointsPerMcTrack_P->Write("", TObject::kOverwrite);
  fTrdPointsPerMcTrack_P->DrawCopy("colz");
  c->SaveAs("pics/TrdQaPointsPerMcTrack_P"+title+".pdf");
  c->SaveAs("pics/TrdQaPointsPerMcTrack_P"+title+".png");
  fTrdTrackCrossedRadiator->Scale(100./fTrdTrackCrossedRadiator->GetEntries());
  fTrdTrackCrossedRadiator->Write("", TObject::kOverwrite);
  fTrdTrackCrossedRadiator->DrawCopy("colz,text");
  c->SaveAs("pics/TrdQaTrackCrossedRadiator"+title+".pdf");
  c->SaveAs("pics/TrdQaTrackCrossedRadiator"+title+".png");
  c->SetLogz(0);
  delete l;
  gDirectory->Cd("..");
  tempFile->Close();
  gDirectory->Cd(origpath);
  gDirectory->pwd();
}
void CbmTrdQa::FinishEvent()
{ 
  if (fT) {
    fMCTracks->Clear();
    fMCTracks->Delete();
  }
  if (fP) {
    fPoints->Clear();
    fPoints->Delete();
    for (fModulePointMapIt = fModulePointMap.begin();
	 fModulePointMapIt != fModulePointMap.end(); fModulePointMapIt++) {
      fModulePointMapIt->second.clear();
    } 
    fModulePointMap.clear();
  }
  if (fD) {
    fDigis->Clear();
    fDigis->Delete();
    for (fModuleDigiMapIt = fModuleDigiMap.begin();
	 fModuleDigiMapIt != fModuleDigiMap.end(); fModuleDigiMapIt++) {
      fModuleDigiMapIt->second.clear();
    } 
    fModuleDigiMap.clear();
  }
  if (fC) {
    fClusters->Clear();
    fClusters->Delete();
    for (fModuleClusterMapIt = fModuleClusterMap.begin();
	 fModuleClusterMapIt != fModuleClusterMap.end(); fModuleClusterMapIt++) {
      fModuleClusterMapIt->second.clear();
    } 
    fModuleClusterMap.clear();
  }
  if (fH) {
    fHits->Clear();
    fHits->Delete();
    for (fModuleHitMapIt = fModuleHitMap.begin();
	 fModuleHitMapIt != fModuleHitMap.end(); fModuleHitMapIt++) {
      fModuleHitMapIt->second.clear();
    } 
    fModuleHitMap.clear();
  }
}
  void CbmTrdQa::GetPadInfos(Int_t moduleAddress, Double_t x, Double_t y, Int_t &iCol, Int_t &iRow, Double_t &padSizeX, Double_t &padSizeY)
  {
    x += fModuleInfo->GetSizeX(); // move origin from module center to lower right corner
    y += fModuleInfo->GetSizeY(); // move origin from module center to lower right corner
    fModuleInfo = fDigiPar->GetModule(moduleAddress);
    Int_t nSector = fModuleInfo->GetNofSectors();
    Int_t iSectorX(0), iSectorY(0);
    Double_t iSectorSizeX = fModuleInfo->GetSectorSizeX(iSectorX);
    Double_t iSectorSizeY = fModuleInfo->GetSectorSizeY(iSectorY);
    Double_t iPadSizeX = fModuleInfo->GetPadSizeX(iSectorX);
    Double_t iPadSizeY = fModuleInfo->GetPadSizeY(iSectorY);
    Int_t iPadX = 0;
    Int_t iPadY = 0;
    if (x > 2 * fModuleInfo->GetSizeX() || y > 2 * fModuleInfo->GetSizeY() || x < 0 || y < 0) {
      printf("point out of module::\n   module %i (%.2f, %.2f) (%.2f, %.2f)\n",moduleAddress,x,y,2 * fModuleInfo->GetSizeX(),2 * fModuleInfo->GetSizeY());
    } else {   
      while (x > iSectorSizeX && iSectorX < nSector) {    
	x -= iSectorSizeX;
	iPadX += iSectorSizeX / iPadSizeX;
	iSectorX++;
	iSectorSizeX = fModuleInfo->GetSectorSizeX(iSectorX);
	iPadSizeX = fModuleInfo->GetPadSizeX(iSectorX);          
      }   
      padSizeX = iPadSizeX;
      iPadX += Int_t((x / padSizeX) + 0.5);
      iCol = iPadX;
      while (y > iSectorSizeY && iSectorY < nSector) {  
	y -= iSectorSizeY;
	iPadY += iSectorSizeY / iPadSizeY;
	iSectorY++;
	iSectorSizeY = fModuleInfo->GetSectorSizeY(iSectorY);
	iPadSizeY = fModuleInfo->GetPadSizeY(iSectorY);      
      }
      padSizeY = iPadSizeY;
      iPadY += Int_t((x / padSizeX) + 0.5);
      iRow = iPadY;
    }
  }
void CbmTrdQa::CreateLayerView(std::map<Int_t, TH1*>& Map, TString folder, TString pics, TString zAxisTitle, Double_t fmax, Double_t fmin, Bool_t logScale){
TString origpath = gDirectory->GetPath();
  printf ("\n%s\n",origpath.Data());
  TString newpath = origpath;
  newpath.ReplaceAll("eds","trd_qa");
  newpath.ReplaceAll(":/","");
  TFile *tempFile = new TFile(newpath,"recreate");
  gDirectory = tempFile->CurrentDirectory();
  gDirectory->pwd();
  TString title(""), name("");
  name.Form("_TH_%.2EGeV_",fTriggerThreshold);
  TPaveText *text=NULL;
  std::map<Int_t, TH1*>::iterator MapIt;
  std::vector<Int_t> fColors;
  std::vector<Double_t> fZLevel;
  //Double_t fmax(20), fmin(0);
  for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
    fColors.push_back(TColor::GetColorPalette(i));
    if (logScale)
      fZLevel.push_back(fmin + TMath::Power(10, TMath::Log10(fmax) / TColor::GetNumberOfColors() * i));// log scale
    else
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
  }

  gDirectory->pwd();
  if (!gDirectory->Cd(folder)) 
    gDirectory->mkdir(folder);
  gDirectory->Cd(folder);
   
  for (MapIt = Map.begin(); MapIt != Map.end(); MapIt++) {
    Double_t value = MapIt->second->GetMean(1);
    Double_t valueE = MapIt->second->GetRMS(1);
    fModuleInfo = fDigiPar->GetModule(MapIt->first);
    Int_t Station  = CbmTrdAddress::GetLayerId(MapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
    Int_t Layer    = CbmTrdAddress::GetLayerId(MapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);
    Int_t combiId = 10 * Station + Layer;
    fLayerMap[combiId]->cd();
    if (MapIt == Map.begin()){
      fLayerDummy->SetZTitle(zAxisTitle);
      fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
    }
    fLayerMap[combiId]->cd()->Update();
    text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
				    fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
				    fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
				    fModuleInfo->GetY()+fModuleInfo->GetSizeY());
    text->SetFillStyle(1001);
    text->SetLineColor(1);

    Int_t j = 0;
    while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
      //printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
      j++;
    }
    text->SetFillColor(fColors[j]);
    if (j >= (Int_t)fZLevel.size())
      text->SetFillColor(2);

    if (fColors[j]<65)
      text->SetTextColor(kWhite);
    title.Form("%.1f#pm%.1f",value,valueE);
    text->AddText(title);
    text->Draw("same");
  }
  for (fLayerMapIt = fLayerMap.begin(); fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQa%s_S%i_L%i_%s%s.pdf",pics.Data(),fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (MapIt = Map.begin(); MapIt != Map.end(); MapIt++) {
    MapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  tempFile->Close();
  gDirectory->Cd(origpath);
  gDirectory->pwd();
}

void CbmTrdQa::CreateLayerView()
{
  TString origpath = gDirectory->GetPath();
  printf ("\n%s\n",origpath.Data());
  TString newpath = origpath;
  newpath.ReplaceAll("eds","trd_qa");
  newpath.ReplaceAll(":/","");
  TFile *tempFile = new TFile(newpath,"recreate");
  gDirectory = tempFile->CurrentDirectory();
  gDirectory->pwd();
  TString title(""), name("");
  TPaveText *text =NULL;
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(20), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    gDirectory->pwd();
    if (!gDirectory->Cd("TrdQa")) 
      gDirectory->mkdir("TrdQa");
    gDirectory->Cd("TrdQa");
    /*
      if (fLayerMap.find(combiId) == fLayerMap.end()){
      title.Form("Station%i_Layer%i",Station,Layer);
      fLayerMap[combiId] =  new TCanvas(title,title,1200,1000);
      fLayerDummy->Draw("colz");
      }
    */
    printf("fModuleGhostMap: %i\n",(Int_t)fModuleGhostMap.size());
    for (fModuleGhostMapIt = fModuleGhostMap.begin();
	 fModuleGhostMapIt != fModuleGhostMap.end(); fModuleGhostMapIt++) {
      Double_t value = fModuleGhostMapIt->second->GetMean(1);
      Double_t valueE = fModuleGhostMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleGhostMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleGhostMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleGhostMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleGhostMapIt == fModuleGhostMap.begin()){
	//fLayerDummy->SetZTitle("left over hits / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);

      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);
      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(20), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }
    gDirectory->pwd();
    if (!gDirectory->Cd("Ghost")) 
      gDirectory->mkdir("Ghost");
    gDirectory->Cd("Ghost");
    for (
	 fLayerMapIt = fLayerMap.begin();
	 fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
      fLayerMapIt->second->Write("", TObject::kOverwrite);
      title.Form("pics/TrdQaGhost_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
      fLayerMapIt->second->SaveAs(title);
      title.ReplaceAll("pdf","png");
      fLayerMapIt->second->SaveAs(title);
    }
    for (fModuleGhostMapIt = fModuleGhostMap.begin();
	 fModuleGhostMapIt != fModuleGhostMap.end(); fModuleGhostMapIt++) {
      fModuleGhostMapIt->second->Write("", TObject::kOverwrite);
    }
    gDirectory->Cd("..");
    printf("fModuleLostMap: %i\n",(Int_t)fModuleLostMap.size());
    for (fModuleLostMapIt = fModuleLostMap.begin();
	 fModuleLostMapIt != fModuleLostMap.end(); fModuleLostMapIt++) {
      Double_t value = fModuleLostMapIt->second->GetMean(1);
      Double_t valueE = fModuleLostMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleLostMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleLostMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleLostMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);    
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleLostMapIt == fModuleLostMap.begin()){
	//fLayerDummy->SetZTitle("left over points / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);
      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }
  
  {
    gDirectory->pwd();
    if (!gDirectory->Cd("Lost")) 
      gDirectory->mkdir("Lost");
    gDirectory->Cd("Lost");

    for (
	 fLayerMapIt = fLayerMap.begin();
	 fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
      fLayerMapIt->second->Write("", TObject::kOverwrite);
      title.Form("pics/TrdQaLost_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
      fLayerMapIt->second->SaveAs(title);
      title.ReplaceAll("pdf","png");
      fLayerMapIt->second->SaveAs(title);
    }
    for (fModuleLostMapIt = fModuleLostMap.begin();
	 fModuleLostMapIt != fModuleLostMap.end(); fModuleLostMapIt++) {
      fModuleLostMapIt->second->Write("", TObject::kOverwrite);
    }
    gDirectory->Cd("..");

    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(100), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }
    printf("fModuleEfficiencyMap: %i\n",(Int_t)fModuleEfficiencyMap.size());
    for (fModuleEfficiencyMapIt = fModuleEfficiencyMap.begin();
	 fModuleEfficiencyMapIt != fModuleEfficiencyMap.end(); fModuleEfficiencyMapIt++) {
      Double_t value = fModuleEfficiencyMapIt->second->GetMean(1);
      Double_t valueE = fModuleEfficiencyMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleEfficiencyMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleEfficiencyMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleEfficiencyMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleEfficiencyMapIt == fModuleEfficiencyMap.begin()){
	//fLayerDummy->SetZTitle("found point hit pairs / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], value, fZLevel[j]);
	j++;
      }
      //printf ("              %2i<%2i %2i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j]-50, value, fZLevel[j]);
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }
 
  gDirectory->pwd();
  if (!gDirectory->Cd("Efficiency")) 
    gDirectory->mkdir("Efficiency");
  gDirectory->Cd("Efficiency");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaEfficiency_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleEfficiencyMapIt = fModuleEfficiencyMap.begin();
       fModuleEfficiencyMapIt != fModuleEfficiencyMap.end(); fModuleEfficiencyMapIt++) {
    fModuleEfficiencyMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(20), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleMultiPointMap: %i\n",(Int_t)fModuleMultiPointMap.size());
    for (fModuleMultiPointMapIt = fModuleMultiPointMap.begin();
	 fModuleMultiPointMapIt != fModuleMultiPointMap.end(); fModuleMultiPointMapIt++) {
      Double_t value = fModuleMultiPointMapIt->second->GetMean(1);
      Double_t valueE = fModuleMultiPointMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleMultiPointMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleMultiPointMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleMultiPointMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleMultiPointMapIt == fModuleMultiPointMap.begin()){
	//fLayerDummy->SetZTitle("multi points per channel / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("MultiPoint")) 
    gDirectory->mkdir("MultiPoint");
  gDirectory->Cd("MultiPoint");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaMultiPoint_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleMultiPointMapIt = fModuleMultiPointMap.begin();
       fModuleMultiPointMapIt != fModuleMultiPointMap.end(); fModuleMultiPointMapIt++) {
    fModuleMultiPointMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(10), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuledEdxMap: %i\n",(Int_t)fModuledEdxMap.size());
    for (fModuledEdxMapIt = fModuledEdxMap.begin();
	 fModuledEdxMapIt != fModuledEdxMap.end(); fModuledEdxMapIt++) {
      Double_t value = fModuledEdxMapIt->second->GetMean(1);
      Double_t valueE = fModuledEdxMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuledEdxMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuledEdxMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuledEdxMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuledEdxMapIt == fModuledEdxMap.begin()){
	//fLayerDummy->SetZTitle("dE/dx [keV]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("dEdx")) 
    gDirectory->mkdir("dEdx");
  gDirectory->Cd("dEdx");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQadEdx_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuledEdxMapIt = fModuledEdxMap.begin();
       fModuledEdxMapIt != fModuledEdxMap.end(); fModuledEdxMapIt++) {
    fModuledEdxMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(2), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleTracklengthMap: %i\n",(Int_t)fModuleTracklengthMap.size());
    for (fModuleTracklengthMapIt = fModuleTracklengthMap.begin();
	 fModuleTracklengthMapIt != fModuleTracklengthMap.end(); fModuleTracklengthMapIt++) {
      Double_t value = fModuleTracklengthMapIt->second->GetMean(1);
      Double_t valueE = fModuleTracklengthMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleTracklengthMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleTracklengthMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleTracklengthMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleTracklengthMapIt == fModuleTracklengthMap.begin()){
	//fLayerDummy->SetZTitle("track length [cm]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("Tracklength")) 
    gDirectory->mkdir("Tracklength");
  gDirectory->Cd("Tracklength");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaTracklength_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleTracklengthMapIt = fModuleTracklengthMap.begin();
       fModuleTracklengthMapIt != fModuleTracklengthMap.end(); fModuleTracklengthMapIt++) {
    fModuleTracklengthMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(1), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleDeltaEMap: %i\n",(Int_t)fModuleDeltaEMap.size());
    for (fModuleDeltaEMapIt = fModuleDeltaEMap.begin();
	 fModuleDeltaEMapIt != fModuleDeltaEMap.end(); fModuleDeltaEMapIt++) {
      Double_t value = fModuleDeltaEMapIt->second->GetMean(1);
      Double_t valueE = fModuleDeltaEMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleDeltaEMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleDeltaEMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleDeltaEMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleDeltaEMapIt == fModuleDeltaEMap.begin()){
	//fLayerDummy->SetZTitle("#delta-electrons / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("DeltaE")) 
    gDirectory->mkdir("DeltaE");
  gDirectory->Cd("DeltaE");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaDeltaE_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleDeltaEMapIt = fModuleDeltaEMap.begin();
       fModuleDeltaEMapIt != fModuleDeltaEMap.end(); fModuleDeltaEMapIt++) {
    fModuleDeltaEMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(15), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleClusterSizeMap: %i\n",(Int_t)fModuleClusterSizeMap.size());
    for (fModuleClusterSizeMapIt = fModuleClusterSizeMap.begin();
	 fModuleClusterSizeMapIt != fModuleClusterSizeMap.end(); fModuleClusterSizeMapIt++) {
      Double_t value = fModuleClusterSizeMapIt->second->GetMean(1);
      Double_t valueE = fModuleClusterSizeMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleClusterSizeMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleClusterSizeMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleClusterSizeMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleClusterSizeMapIt == fModuleClusterSizeMap.begin()){
	//fLayerDummy->SetZTitle("#delta-electrons / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
      //printf("ModuleAddress:%8i  cluster size: %5.2f+-%5.2f\n",fModuleClusterSizeMapIt->first,value,valueE);
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("ClusterSize")) 
    gDirectory->mkdir("ClusterSize");
  gDirectory->Cd("ClusterSize");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaClusterSize_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleClusterSizeMapIt = fModuleClusterSizeMap.begin();
       fModuleClusterSizeMapIt != fModuleClusterSizeMap.end(); fModuleClusterSizeMapIt++) {
    fModuleClusterSizeMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(100), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleTrackableMap: %i\n",(Int_t)fModuleTrackableMap.size());
    for (fModuleTrackableMapIt = fModuleTrackableMap.begin();
	 fModuleTrackableMapIt != fModuleTrackableMap.end(); fModuleTrackableMapIt++) {
      Double_t value = fModuleTrackableMapIt->second->GetMean(1);
      Double_t valueE = fModuleTrackableMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleTrackableMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleTrackableMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleTrackableMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleTrackableMapIt == fModuleTrackableMap.begin()){
	//fLayerDummy->SetZTitle("#delta-electrons / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("TrackableRatio")) 
    gDirectory->mkdir("TrackableRatio");
  gDirectory->Cd("TrackableRatio");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaTrackableRatio_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleTrackableMapIt = fModuleTrackableMap.begin();
       fModuleTrackableMapIt != fModuleTrackableMap.end(); fModuleTrackableMapIt++) {
    fModuleTrackableMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(50), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleTrackableMap2: %i\n",(Int_t)fModuleTrackableMap2.size());
    for (fModuleTrackableMap2It = fModuleTrackableMap2.begin();
	 fModuleTrackableMap2It != fModuleTrackableMap2.end(); fModuleTrackableMap2It++) {
      Double_t value = fModuleTrackableMap2It->second->GetMean(1);
      Double_t valueE = fModuleTrackableMap2It->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleTrackableMap2It->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleTrackableMap2It->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleTrackableMap2It->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleTrackableMap2It == fModuleTrackableMap2.begin()){
	//fLayerDummy->SetZTitle("#delta-electrons / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("Trackable")) 
    gDirectory->mkdir("Trackable");
  gDirectory->Cd("Trackable");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaTrackable_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleTrackableMap2It = fModuleTrackableMap2.begin();
       fModuleTrackableMap2It != fModuleTrackableMap2.end(); fModuleTrackableMap2It++) {
    fModuleTrackableMap2It->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  {
    std::vector<Int_t> fColors;
    std::vector<Double_t> fZLevel;
    Double_t fmax(100), fmin(0);
    for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
      fColors.push_back(TColor::GetColorPalette(i));
      //fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
      fZLevel.push_back(fmin + (fmax / TColor::GetNumberOfColors() * i)); // lin scale
    }

    printf("fModuleAveragePointsMap: %i\n",(Int_t)fModuleAveragePointsMap.size());
    for (fModuleAveragePointsMapIt = fModuleAveragePointsMap.begin();
	 fModuleAveragePointsMapIt != fModuleAveragePointsMap.end(); fModuleAveragePointsMapIt++) {
      Double_t value = fModuleAveragePointsMapIt->second->GetMean(1);
      Double_t valueE = fModuleAveragePointsMapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(fModuleAveragePointsMapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(fModuleAveragePointsMapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(fModuleAveragePointsMapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);  
      Int_t combiId = 10 * Station + Layer;
      fLayerMap[combiId]->cd();
      if (fModuleAveragePointsMapIt == fModuleAveragePointsMap.begin()){
	//fLayerDummy->SetZTitle("#delta-electrons / all points [%]");
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	//fLayerDummy->DrawCopy("colz");
      }
      fLayerMap[combiId]->cd()->Update();
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);    
      Int_t j = 0;
      while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	//printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	j++;
      }
      text->SetFillColor(fColors[j]);
      if (j >= (Int_t)fZLevel.size())
	text->SetFillColor(2);

      if (fColors[j]<65)
	text->SetTextColor(kWhite);

      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
  }

  gDirectory->pwd();
  if (!gDirectory->Cd("Trackable")) 
    gDirectory->mkdir("Trackable");
  gDirectory->Cd("Trackable");

  for (
       fLayerMapIt = fLayerMap.begin();
       fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
    fLayerMapIt->second->Write("", TObject::kOverwrite);
    title.Form("pics/TrdQaAveragePoints_S%i_L%i_%s%s.pdf",fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,fGeo.Data(),name.Data());//(fLayerMapIt->first)/10,(fLayerMapIt->first)-(fLayerMapIt->first)/10*10,fGeo.Data());
    fLayerMapIt->second->SaveAs(title);
    title.ReplaceAll("pdf","png");
    fLayerMapIt->second->SaveAs(title);
  }
  for (fModuleAveragePointsMapIt = fModuleAveragePointsMap.begin();
       fModuleAveragePointsMapIt != fModuleAveragePointsMap.end(); fModuleAveragePointsMapIt++) {
    fModuleAveragePointsMapIt->second->Write("", TObject::kOverwrite);
  }
  gDirectory->Cd("..");
  gDirectory->Cd("..");

  tempFile->Close();
  gDirectory->Cd(origpath);
  gDirectory->pwd();

}
Double_t CbmTrdQa::CalcAngle(const CbmTrdPoint* pointA, const CbmTrdPoint* pointB){
  return TMath::ACos(
		     ((pointA->GetXOut() - pointA->GetXIn()) * (pointB->GetXOut() - pointB->GetXIn()) + 
		      (pointA->GetYOut() - pointA->GetYIn()) * (pointB->GetYOut() - pointB->GetYIn()) + 
		      (pointA->GetZOut() - pointA->GetZIn()) * (pointB->GetZOut() - pointB->GetZIn())) / (
													  TMath::Sqrt(
														      (pointA->GetXOut() - pointA->GetXIn()) * (pointA->GetXOut() - pointA->GetXIn()) + 
														      (pointA->GetYOut() - pointA->GetYIn()) * (pointA->GetYOut() - pointA->GetYIn()) + 
														      (pointA->GetZOut() - pointA->GetZIn()) * (pointA->GetZOut() - pointA->GetZIn())) *
													  TMath::Sqrt(
														      (pointB->GetXOut() - pointB->GetXIn()) * (pointB->GetXOut() - pointB->GetXIn()) + 
														      (pointB->GetYOut() - pointB->GetYIn()) * (pointB->GetYOut() - pointB->GetYIn()) + 
														      (pointB->GetZOut() - pointB->GetZIn()) * (pointB->GetZOut() - pointB->GetZIn()))
		   
													  )
		     );
}
    void CbmTrdQa::NormalizeHistos()
    {
      Double_t Entries = fdEdxPoint->GetEntries();
      fdEdxPoint->Scale(1./Entries);
      Entries = fdEdxDigi->GetEntries();
      fdEdxDigi->Scale(1./Entries);
      Entries = fdEdxCluster->GetEntries();
      fdEdxCluster->Scale(1./Entries);
      Entries = fdEdxHit->GetEntries();
      fdEdxHit->Scale(1./Entries);
      Entries = fdEdxPionMc->GetEntries();
      fdEdxPionMc->Scale(1./Entries);
      Entries = fdEdxPionHit->GetEntries();
      fdEdxPionHit->Scale(1./Entries);
      Entries = fdEdxElectronMc->GetEntries();
      fdEdxElectronMc->Scale(1./Entries);
      Entries = fdEdxElectronHit->GetEntries();
      fdEdxElectronHit->Scale(1./Entries);
    }
  void CbmTrdQa::FinishTask()
  {
    NormalizeHistos();
    SaveHistos();
    CreateLayerView();  
  }
  void CbmTrdQa::Register()
  {
    //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  ClassImp(CbmTrdQa)
