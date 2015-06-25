/**
 * \file CbmLitClusteringQa.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "CbmLitClusteringQa.h"
#include "CbmLitClusteringQaReport.h"
#include "CbmHistManager.h"
#include "CbmHit.h"
#include "FairRootManager.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchPoint.h"
#include "CbmMuchCluster.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleGemRadial.h"
#include "CbmMuchSector.h"
#include "CbmMuchSectorRadial.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmMCTrack.h"
#include "CbmTrdAddress.h"
#include "CbmCluster.h"
#include "CbmStsDigi.h"
#include "CbmMatch.h"
#include "CbmStsAddress.h"

#include "TSystem.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <boost/assign/list_of.hpp>
#include <fstream>
#include <cmath>
#include <sstream>
using std::cout;
using std::vector;
using std::map;
using std::binary_search;
using boost::assign::list_of;

CbmLitClusteringQa::CbmLitClusteringQa():
   fHM(NULL),
   fOutputDir(""),
   fDet(),
   fMCTracks(NULL),
   fMvdPoints(NULL),
   fMvdDigis(NULL),
   fMvdClusters(NULL),
   fMvdHits(NULL),
   fStsPoints(NULL),
   fStsDigis(NULL),
   fStsClusters(NULL),
   fStsHits(NULL),
   fStsDigiMatches(NULL),
   fStsClusterMatches(NULL),
   fStsHitMatches(NULL),
   fRichHits(NULL),
   fRichPoints(NULL),
   fMuchPoints(NULL),
   fMuchDigis(NULL),
   fMuchClusters(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchDigiMatches(NULL),
   fMuchClusterMatches(NULL),
   fMuchPixelHitMatches(NULL),
   fMuchStrawHitMatches(NULL),
   fTrdPoints(NULL),
   fTrdDigis(NULL),
   fTrdClusters(NULL),
   fTrdHits(NULL),
   fTrdDigiMatches(NULL),
   fTrdClusterMatches(NULL),
   fTrdHitMatches(NULL),
   fTofPoints(NULL),
   fTofHits(NULL),
   fMuchDigiFileName("")
{

}

CbmLitClusteringQa::~CbmLitClusteringQa()
{
   if (fHM) delete fHM;
}

InitStatus CbmLitClusteringQa::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   fDet.DetermineSetup();
   ReadDataBranches();

   InitMuchGeoScheme(fMuchDigiFileName);

   CreateHistograms();
   return kSUCCESS;
}

void CbmLitClusteringQa::Exec(
    Option_t* opt)
{
   FillEventCounterHistograms();

   ProcessPoints(fStsPoints, "Sts", kSTS);
   ProcessPoints(fTrdPoints, "Trd", kTRD);
   ProcessPoints(fMuchPoints, "Much", kMUCH);

   ProcessDigis(fStsDigis, fStsDigiMatches, "Sts", kSTS);
   ProcessDigis(fTrdDigis, fTrdDigiMatches, "Trd", kTRD);
   ProcessDigis(fMuchDigis, fMuchDigiMatches, "Much", kMUCH);

   ProcessClusters(fStsClusters, fStsClusterMatches, "Sts", kSTS);
   ProcessClusters(fTrdClusters, fTrdClusterMatches, "Trd", kTRD);
   ProcessClusters(fMuchClusters, fMuchClusterMatches, "Much", kMUCH);

   ProcessHits(fStsHits, fStsHitMatches,"Sts", kSTS);
   ProcessHits(fTrdHits, fTrdHitMatches,"Trd", kTRD);
   ProcessHits(fMuchPixelHits, fMuchPixelHitMatches, "Much", kMUCH);

   FillResidualAndPullHistograms(fStsPoints, fStsHits, fStsHitMatches, "Sts", kSTS);
   FillResidualAndPullHistograms(fTrdPoints, fTrdHits, fTrdHitMatches, "Trd", kTRD);
   FillResidualAndPullHistograms(fMuchPoints, fMuchPixelHits, fMuchPixelHitMatches, "Much", kMUCH);

   FillHitEfficiencyHistograms(fStsPoints, fStsHits, fStsHitMatches, "Sts", kSTS);
   FillHitEfficiencyHistograms(fTrdPoints, fTrdHits, fTrdHitMatches, "Trd", kTRD);
   FillHitEfficiencyHistograms(fMuchPoints, fMuchPixelHits, fMuchPixelHitMatches, "Much", kMUCH);

   fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
   std::cout << "CbmLitClusteringQa::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;
}

void CbmLitClusteringQa::Finish()
{
   fHM->WriteToFile();
   CbmSimulationReport* report = new CbmLitClusteringQaReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitClusteringQa::InitMuchGeoScheme(
      const string& digiFileName)
{
   if (fDet.GetDet(kMUCH) && fMuchDigiFileName != "") {
      TFile* oldfile = gFile;
      TFile* file = new TFile(digiFileName.c_str());
      TObjArray* stations = (TObjArray*) file->Get("stations");
      file->Close();
      file->Delete();
      gFile=oldfile;
      CbmMuchGeoScheme::Instance()->Init(stations);
   }
}

void CbmLitClusteringQa::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

   fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
   fMvdDigis = (TClonesArray*) ioman->GetObject("MvdDigi");
   fMvdClusters = (TClonesArray*) ioman->GetObject("MvdCluster");
   fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");

   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   fStsDigis = (TClonesArray*) ioman->GetObject("StsDigi");
   fStsClusters = (TClonesArray*) ioman->GetObject("StsCluster");
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   fStsDigiMatches = (TClonesArray*) ioman->GetObject("StsDigiMatch");
   fStsClusterMatches = (TClonesArray*) ioman->GetObject("StsClusterMatch");
   fStsHitMatches = (TClonesArray*) ioman->GetObject("StsHitMatch");

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");

   fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
   fMuchDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
   fMuchClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
   fMuchDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
   fMuchClusterMatches = (TClonesArray*) ioman->GetObject("MuchClusterMatch");
   fMuchPixelHitMatches = (TClonesArray*) ioman->GetObject("MuchPixelHitMatch");
   fMuchStrawHitMatches = (TClonesArray*) ioman->GetObject("MuchStrawHitMatch");

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
   fTrdDigiMatches = (TClonesArray*) ioman->GetObject("TrdDigiMatch");
   fTrdClusterMatches = (TClonesArray*) ioman->GetObject("TrdClusterMatch");
   fTrdHitMatches = (TClonesArray*) ioman->GetObject("TrdHitMatch");

   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
}

Int_t CbmLitClusteringQa::GetStationId(
      Int_t address,
      DetectorId detId)
{
   assert(detId == kSTS || detId == kTRD || detId == kMUCH);
   if (detId == kSTS) return CbmStsAddress::GetElementId(address, kStsStation);
   else if (detId == kTRD) return CbmTrdAddress::GetLayerId(address);
   else if (detId == kMUCH) return (CbmMuchGeoScheme::Instance()->GetLayerSideNr(address) - 1) / 2;
   return 0;
}

void CbmLitClusteringQa::ProcessPoints(
      const TClonesArray* points,
      const string& detName,
      DetectorId detId)
{
   string histName = "hno_NofObjects_" + detName + "Points_Station";
   if (NULL == points || !fHM->Exists(histName)) return;
   for (Int_t i = 0; i < points->GetEntriesFast(); i++) {
      const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(i));
      fHM->H1(histName)->Fill(GetStationId(point->GetDetectorID(), detId));
   }
}

void CbmLitClusteringQa::ProcessDigis(
      const TClonesArray* digis,
      const TClonesArray* digiMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL == digis || !fHM->Exists("hno_NofObjects_" + detName + "Digis_Station")) return;
   for (Int_t i = 0; i < digis->GetEntriesFast(); i++) {
      const CbmDigi* digi = static_cast<const CbmDigi*>(digis->At(i));
      const CbmMatch* digiMatch = static_cast<const CbmMatch*>(digiMatches->At(i));
      Int_t stationId = GetStationId(digi->GetAddress(), detId);
      fHM->H1("hno_NofObjects_" + detName + "Digis_Station")->Fill(stationId);
      fHM->H1("hpa_" + detName + "Digi_NofPointsInDigi_H1")->Fill(digiMatch->GetNofLinks());
      fHM->H1("hpa_" + detName + "Digi_NofPointsInDigi_H2")->Fill(stationId, digiMatch->GetNofLinks());
   }
}

void CbmLitClusteringQa::ProcessClusters(
      const TClonesArray* clusters,
      const TClonesArray* clusterMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL != clusters && fHM->Exists("hno_NofObjects_" + detName + "Clusters_Station")) {
      for (Int_t i = 0; i < clusters->GetEntriesFast(); i++) {
         const CbmCluster* cluster = static_cast<const CbmCluster*>(clusters->At(i));
         const CbmMatch* clusterMatch = static_cast<const CbmMatch*>(clusterMatches->At(i));
         Int_t stationId = GetStationId(cluster->GetAddress(), detId);
         fHM->H1("hno_NofObjects_" + detName + "Clusters_Station")->Fill(stationId);
         fHM->H1("hpa_" + detName + "Cluster_NofDigisInCluster_H1")->Fill(cluster->GetNofDigis());
         fHM->H1("hpa_" + detName + "Cluster_NofDigisInCluster_H2")->Fill(stationId, cluster->GetNofDigis());
         fHM->H1("hpa_" + detName + "Cluster_NofPointsInCluster_H1")->Fill(clusterMatch->GetNofLinks());
         fHM->H1("hpa_" + detName + "Cluster_NofPointsInCluster_H2")->Fill(stationId, clusterMatch->GetNofLinks());
      }
   }
}

void CbmLitClusteringQa::ProcessHits(
      const TClonesArray* hits,
      const TClonesArray* hitMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL != hits && fHM->Exists("hno_NofObjects_" + detName + "Hits_Station")) {
      for (Int_t i = 0; i < hits->GetEntriesFast(); i++) {
         const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(i));
         const CbmMatch* hitMatch = static_cast<const CbmMatch*>(hitMatches->At(i));
         Int_t stationId = GetStationId(hit->GetAddress(), detId);
         fHM->H1("hno_NofObjects_" + detName + "Hits_Station")->Fill(stationId);
         fHM->H1("hpa_" + detName + "Hit_SigmaX_H1")->Fill(hit->GetDx());
         fHM->H1("hpa_" + detName + "Hit_SigmaX_H2")->Fill(stationId, hit->GetDx());
         fHM->H1("hpa_" + detName + "Hit_SigmaY_H1")->Fill(hit->GetDy());
         fHM->H1("hpa_" + detName + "Hit_SigmaY_H2")->Fill(stationId, hit->GetDy());
         fHM->H1("hpa_" + detName + "Hit_NofPointsInHit_H1")->Fill(hitMatch->GetNofLinks());
         fHM->H1("hpa_" + detName + "Hit_NofPointsInHit_H2")->Fill(stationId, hitMatch->GetNofLinks());
      }
   }
}

void CbmLitClusteringQa::FillEventCounterHistograms()
{
   if (NULL != fMvdPoints && fHM->Exists("hno_NofObjects_MvdPoints_Event")) fHM->H1("hno_NofObjects_MvdPoints_Event")->Fill(fMvdPoints->GetEntriesFast());
   if (NULL != fMvdDigis && fHM->Exists("hno_NofObjects_MvdDigis_Event")) fHM->H1("hno_NofObjects_MvdDigis_Event")->Fill(fMvdDigis->GetEntriesFast());
   if (NULL != fMvdClusters && fHM->Exists("hno_NofObjects_MvdClusters_Event")) fHM->H1("hno_NofObjects_MvdClusters_Event")->Fill(fMvdClusters->GetEntriesFast());
   if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits_Event")) fHM->H1("hno_NofObjects_MvdHits_Event")->Fill(fMvdHits->GetEntriesFast());

   if (NULL != fStsPoints && fHM->Exists("hno_NofObjects_StsPoints_Event")) fHM->H1("hno_NofObjects_StsPoints_Event")->Fill(fStsPoints->GetEntriesFast());
   if (NULL != fStsDigis && fHM->Exists("hno_NofObjects_StsDigis_Event")) fHM->H1("hno_NofObjects_StsDigis_Event")->Fill(fStsDigis->GetEntriesFast());
   if (NULL != fStsClusters && fHM->Exists("hno_NofObjects_StsClusters_Event")) fHM->H1("hno_NofObjects_StsClusters_Event")->Fill(fStsClusters->GetEntriesFast());
   if (NULL != fStsHits && fHM->Exists("hno_NofObjects_StsHits_Event")) fHM->H1("hno_NofObjects_StsHits_Event")->Fill(fStsHits->GetEntriesFast());

   if (NULL != fRichHits && fHM->Exists("hno_NofObjects_RichHits_Event")) fHM->H1("hno_NofObjects_RichHits_Event")->Fill(fRichHits->GetEntriesFast());
   if (NULL != fRichPoints && fHM->Exists("hno_NofObjects_RichPoints_Event")) fHM->H1("hno_NofObjects_RichPoints_Event")->Fill(fRichPoints->GetEntriesFast());

   if (NULL != fTrdPoints && fHM->Exists("hno_NofObjects_TrdPoints_Event")) fHM->H1("hno_NofObjects_TrdPoints_Event")->Fill(fTrdPoints->GetEntriesFast());
   if (NULL != fTrdDigis && fHM->Exists("hno_NofObjects_TrdDigis_Event")) fHM->H1("hno_NofObjects_TrdDigis_Event")->Fill(fTrdDigis->GetEntriesFast());
   if (NULL != fTrdClusters && fHM->Exists("hno_NofObjects_TrdClusters_Event")) fHM->H1("hno_NofObjects_TrdClusters_Event")->Fill(fTrdClusters->GetEntriesFast());
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits_Event")) fHM->H1("hno_NofObjects_TrdHits_Event")->Fill(fTrdHits->GetEntriesFast());

   if (NULL != fMuchPoints && fHM->Exists("hno_NofObjects_MuchPoints_Event")) fHM->H1("hno_NofObjects_MuchPoints_Event")->Fill(fMuchPoints->GetEntriesFast());
   if (NULL != fMuchDigis && fHM->Exists("hno_NofObjects_MuchDigis_Event")) fHM->H1("hno_NofObjects_MuchDigis_Event")->Fill(fMuchDigis->GetEntriesFast());
   if (NULL != fMuchClusters && fHM->Exists("hno_NofObjects_MuchClusters_Event")) fHM->H1("hno_NofObjects_MuchClusters_Event")->Fill(fMuchClusters->GetEntriesFast());
   if (NULL != fMuchPixelHits && fHM->Exists("hno_NofObjects_MuchPixelHits_Event")) fHM->H1("hno_NofObjects_MuchPixelHits_Event")->Fill(fMuchPixelHits->GetEntriesFast());
   if (NULL != fMuchStrawHits && fHM->Exists("hno_NofObjects_MuchStrawHits_Event")) fHM->H1("hno_NofObjects_MuchStrawHits_Event")->Fill(fMuchStrawHits->GetEntriesFast());

   if (NULL != fTofPoints && fHM->Exists("hno_NofObjects_TofPoints_Event")) fHM->H1("hno_NofObjects_TofPoints_Event")->Fill(fTofPoints->GetEntriesFast());
   if (NULL != fTofHits && fHM->Exists("hno_NofObjects_TofHits_Event")) fHM->H1("hno_NofObjects_TofHits_Event")->Fill(fTofHits->GetEntriesFast());
}

void CbmLitClusteringQa::FillResidualAndPullHistograms(
      const TClonesArray* points,
      const TClonesArray* hits,
      const TClonesArray* hitMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL == points || NULL == hits || NULL == hitMatches) return;
   string nameResidualX = "hrp_" + detName + "_ResidualX_H2";
   string nameResidualY = "hrp_" + detName + "_ResidualY_H2";
   string namePullX = "hrp_" + detName + "_PullX_H2";
   string namePullY = "hrp_" + detName + "_PullY_H2";
   if (!fHM->Exists(nameResidualX) || !fHM->Exists(nameResidualY)
         || !fHM->Exists(namePullX) || !fHM->Exists(namePullY)) return;

   Int_t nofHits = hits->GetEntriesFast();
	for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(iHit));
      const CbmMatch* match = static_cast<const CbmMatch*>(hitMatches->At(iHit));
      if (isnan(static_cast<Float_t>(hit->GetX())) || (isnan(static_cast<Float_t>(hit->GetY())))) continue;
      const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(match->GetMatchedLink().GetIndex()));
      if (point == NULL) continue;
      //Float_t xPoint = (muchPoint->GetXIn() + muchPoint->GetXOut()) / 2;
      //Float_t yPoint = (muchPoint->GetYIn() + muchPoint->GetYOut()) / 2;
      Float_t residualX =  point->GetX() - hit->GetX();
      Float_t residualY =  point->GetY() - hit->GetY();
      Int_t stationId = GetStationId(hit->GetAddress(), detId);
      fHM->H2(nameResidualX)->Fill(stationId, residualX);
      fHM->H2(nameResidualY)->Fill(stationId, residualY);
      fHM->H2(namePullX)->Fill(stationId, residualX / hit->GetDx());
      fHM->H2(namePullY)->Fill(stationId, residualY / hit->GetDy());
   }
}

void CbmLitClusteringQa::FillHitEfficiencyHistograms(
      const TClonesArray* points,
      const TClonesArray* hits,
      const TClonesArray* hitMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL == points || NULL == hits || NULL == hitMatches) return;
   string accName = "hhe_" + detName + "_All_Acc_Station";
   if (NULL == points || !fHM->Exists(accName)) return;
   for (Int_t i = 0; i < points->GetEntriesFast(); i++) {
      const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(i));
      fHM->H1(accName)->Fill(GetStationId(point->GetDetectorID(), detId));
   }

   string recName = "hhe_" + detName + "_All_Rec_Station";
   string cloneName = "hhe_" + detName + "_All_Clone_Station";
   set<Int_t> mcPointSet; // IDs of MC points
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(iHit));
      const CbmMatch* match = static_cast<const CbmMatch*>(hitMatches->At(iHit));
      if (mcPointSet.find(match->GetMatchedLink().GetIndex()) == mcPointSet.end()) {
         fHM->H1(recName)->Fill(GetStationId(hit->GetAddress(), detId));
         mcPointSet.insert(match->GetMatchedLink().GetIndex());
      } else {
         fHM->H1(cloneName)->Fill(GetStationId(hit->GetAddress(), detId));
      }
   }
}

void CbmLitClusteringQa::CreateHistograms()
{
   CreateNofObjectsHistograms(kMVD, "Mvd", "Station", "Station number");
   CreateNofObjectsHistograms(kSTS, "Sts", "Station", "Station number");
   CreateNofObjectsHistograms(kTRD, "Trd", "Station", "Station number");
   CreateNofObjectsHistograms(kMUCH, "Much", "Station", "Station number");

   CreateNofObjectsHistograms(kMVD, "Mvd");
   CreateNofObjectsHistograms(kSTS, "Sts");
   CreateNofObjectsHistograms(kTRD, "Trd");
   CreateNofObjectsHistograms(kMUCH, "Much");
   CreateNofObjectsHistograms(kTOF, "Tof");
   CreateNofObjectsHistograms(kRICH, "Rich");

   CreateClusterParametersHistograms(kSTS, "Sts");
   CreateClusterParametersHistograms(kTRD, "Trd");
   CreateClusterParametersHistograms(kMUCH, "Much");

   CreateHitEfficiencyHistograms(kSTS, "Sts", "Station", "Station number", 100, -0.5, 99.5);
   CreateHitEfficiencyHistograms(kMUCH, "Much", "Station", "Station number", 100, -0.5, 99.5);
   CreateHitEfficiencyHistograms(kTRD, "Trd", "Station", "Station number", 100, -0.5, 99.5);

   // Histogram stores number of events
   fHM->Create1<TH1F>("hen_EventNo_ClusteringQa", "hen_EventNo_ClusteringQa", 1, 0, 1.);
}

void CbmLitClusteringQa::CreateNofObjectsHistograms(
      DetectorId detId,
      const string& detName)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
   Int_t nofBins = 100000;
   Double_t minX = -0.5;
   Double_t maxX = 99999.5;
   string name = "hno_NofObjects_" + detName;
   fHM->Create1<TH1F>(name + "Points_Event", name + "Points_Event;Points per event;Counter", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Digis_Event", name + "Digis_Event;Digis per event;Counter", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Clusters_Event", name + "Clusters_Event;Clusters per event;Counter", nofBins, minX, maxX);
   if (detId == kMUCH) {
      fHM->Create1<TH1F>(name + "PixelHits_Event", name + "PixelHits_Event;Hits per event;Counter", nofBins, minX, maxX);
      fHM->Create1<TH1F>(name + "StrawHits_Event", name + "StrawHits_Event;Hits per event;Counter", nofBins, minX, maxX);
   } else {
      fHM->Create1<TH1F>(name + "Hits_Event", name + "Hits_Event;Hits per event;Counter", nofBins, minX, maxX);
   }
}

void CbmLitClusteringQa::CreateNofObjectsHistograms(
      DetectorId detId,
      const string& detName,
      const string& parameter,
      const string& xTitle)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
   Int_t nofBins = 100;
   Double_t minX = -0.5;
   Double_t maxX = 99.5;
   string name = "hno_NofObjects_" + detName;
   fHM->Create1<TH1F>(name + "Points_" + parameter, name + "Points_" + parameter + ";" + xTitle + ";Points per event", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Digis_" + parameter, name + "Digis_" + parameter + ";" + xTitle + ";Digis per event", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Clusters_" + parameter, name + "Clusters_" + parameter + ";" + xTitle + ";Clusters per event", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Hits_" + parameter, name + "Hits_" + parameter + ";" + xTitle + ";Hits per event", nofBins, minX, maxX);
}

void CbmLitClusteringQa::CreateClusterParametersHistograms(
      DetectorId detId,
      const string& detName)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kMVD || detId == kSTS || detId == kRICH || detId == kMUCH || detId == kTRD || detId == kTOF);
   Int_t nofBinsStation = 100;
   Double_t minStation = -0.5;
   Double_t maxStation = 99.5;
   Int_t nofBins = 100;
   Double_t min = -0.5;
   Double_t max = 99.5;
   Int_t nofBinsSigma = 100;
   Double_t minSigma = -0.5;
   Double_t maxSigma = 9.5;
   Int_t nofBinsResidual = 200;
   Double_t minResidual = -10.0;
   Double_t maxResidual = 10.0;
   Int_t nofBinsPull = 100;
   Double_t minPull = -5.0;
   Double_t maxPull = 5.0;

   string nameH1 = "hpa_" + detName + "Cluster_NofDigisInCluster_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of digis;Yield", nofBins, min, max);
   string nameH2 = "hpa_" + detName + "Cluster_NofDigisInCluster_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of digis;Yield", nofBinsStation, minStation, max, nofBins, min, max);
   nameH1 = "hpa_" + detName + "Cluster_NofPointsInCluster_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
   nameH2 = "hpa_" + detName + "Cluster_NofPointsInCluster_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, max, nofBins, min, max);
   nameH1 = "hpa_" + detName + "Digi_NofPointsInDigi_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
   nameH2 = "hpa_" + detName + "Digi_NofPointsInDigi_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, maxStation, nofBins, min, max);
   nameH1 = "hpa_" + detName + "Hit_NofPointsInHit_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
   nameH2 = "hpa_" + detName + "Hit_NofPointsInHit_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, max, nofBins, min, max);
   nameH1 = "hpa_" + detName + "Hit_SigmaX_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";#sigma_{X} [cm];Yield", nofBinsSigma, minSigma, maxSigma);
   nameH2 = "hpa_" + detName + "Hit_SigmaX_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;#sigma_{X} [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsSigma, minSigma, maxSigma);
   nameH1 = "hpa_" + detName + "Hit_SigmaY_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";#sigma_{Y} [cm];Yield", nofBinsSigma, minSigma, maxSigma);
   nameH2 = "hpa_" + detName + "Hit_SigmaY_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;#sigma_{Y} [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsSigma, minSigma, maxSigma);

   // Residual and pull histograms
   nameH2 = "hrp_" + detName + "_ResidualX_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Residual X [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsResidual, minResidual, maxResidual);
   nameH2 = "hrp_" + detName + "_ResidualY_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Residual Y [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsResidual, minResidual, maxResidual);
   nameH2 = "hrp_" + detName + "_PullX_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Pull X;Yield", nofBinsStation, minStation, maxStation, nofBinsPull, minPull, maxPull);
   nameH2 = "hrp_" + detName + "_PullY_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Pull Y;Yield", nofBinsStation, minStation, maxStation, nofBinsPull, minPull, maxPull);
}

void CbmLitClusteringQa::CreateHitEfficiencyHistograms(
      DetectorId detId,
      const string& detName,
      const string& parameter,
      const string& xTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   if (!fDet.GetDet(detId)) return;
   vector<string> types = list_of("Acc")("Rec")("Eff")("Clone")("CloneProb");
   vector<string> cat = list_of("All");
   for (Int_t iCat = 0; iCat < cat.size(); iCat++) {
      for (Int_t iType = 0; iType < types.size(); iType++) {
         string yTitle = (types[iType] == "Eff") ? "Efficiency [%]" : (types[iType] == "CloneProb") ? "Probability [%]" : "Counter";
         string histName = "hhe_" + detName + "_" + cat[iCat] + "_" + types[iType] + "_" + parameter;
         string histTitle = histName + ";" + xTitle + ";" + yTitle;
         fHM->Add(histName, new TH1F(histName.c_str(), histTitle.c_str(), nofBins, minBin, maxBin));
      }
   }
}

ClassImp(CbmLitClusteringQa);
