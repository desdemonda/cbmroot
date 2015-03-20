/*
 * \file CbmLitClusteringQaTrd.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Dec, 2014
 */

#include "CbmHistManager.h"
#include "CbmHit.h"
#include "CbmBaseHit.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"
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
#include "CbmMatch.h"

#include "TSystem.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <boost/assign/list_of.hpp>
#include <cmath>		// for math functions
#include <iostream>		// for cli output
#include <fstream>
#include <sstream>
#include <string>

#include "CbmLitClusteringQaTrd.h"
#include "CbmLitClusteringQaTrdReport.h"
using std::cout;
using std::vector;
using boost::assign::list_of;


CbmLitClusteringQaTrd::CbmLitClusteringQaTrd():
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
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fMuchDigiFileName("")
{

}

CbmLitClusteringQaTrd::~CbmLitClusteringQaTrd()
{
   if (fHM) delete fHM;
}

/**
 * \brief Initialization (Creation of histograms)
 */
InitStatus CbmLitClusteringQaTrd::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   fDet.DetermineSetup();
   ReadDataBranches();

//   fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));

   CreateHistograms();
   return kSUCCESS;
}

/**
 * \brief Execution (Filling of histograms)
 */
void CbmLitClusteringQaTrd::Exec(
    Option_t* opt)
{
   if (NULL != fTrdPoints && fHM->Exists("hno_NofObjects_TrdPoints_Event")) fHM->H1("hno_NofObjects_TrdPoints_Event")->Fill(fTrdPoints->GetEntriesFast());
   if (NULL != fTrdDigis && fHM->Exists("hno_NofObjects_TrdDigis_Event")) fHM->H1("hno_NofObjects_TrdDigis_Event")->Fill(fTrdDigis->GetEntriesFast());
   if (NULL != fTrdClusters && fHM->Exists("hno_NofObjects_TrdClusters_Event")) fHM->H1("hno_NofObjects_TrdClusters_Event")->Fill(fTrdClusters->GetEntriesFast());
   if (NULL != fTrdHits && fHM->Exists("hno_NofObjects_TrdHits_Event")) fHM->H1("hno_NofObjects_TrdHits_Event")->Fill(fTrdHits->GetEntriesFast());

   ProcessPoints(fTrdPoints, "Trd", kTRD);
   ProcessDigis(fTrdDigis, fTrdDigiMatches, "Trd", kTRD);
   ProcessClusters(fTrdClusters, fTrdClusterMatches, "Trd", kTRD);
   ProcessHits(fTrdHits, fTrdHitMatches,"Trd", kTRD);
   FillResidualAndPullHistograms(fTrdPoints, fTrdHits, fTrdHitMatches, "Trd", kTRD);
   FillHitEfficiencyHistograms(fTrdPoints, fTrdHits, fTrdHitMatches, "Trd", kTRD);

   ProcessSectorHistos();

   // Increment and output the event counter
   fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
   std::cout << "CbmLitClusteringQaTrd::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;
}

/**
 * Finish Execution (Write results to file)
 */
void CbmLitClusteringQaTrd::Finish()
{
   fHM->WriteToFile();
   CbmSimulationReport* report = new CbmLitClusteringQaTrdReport();
   report->Create(fHM, fOutputDir);
   delete report;
}

void CbmLitClusteringQaTrd::ProcessSectorHistos()
{
   static Int_t sModule = 0;
   static Int_t sModule2 = 0;
   static Int_t eventNo = 1;

   TString fileName = TString("Event") + TString(eventNo) + TString(".csv");

   std::ofstream file(fileName);
   if(!file){
      cerr << "Uh oh, Event" << eventNo << ".csv could not be opened for writing!" << std::endl;
      return;
   }
   file << "eventNo, iDigi, moduleId, secCol, secRow, digiAddress" << std::endl;

   Int_t nentries = fTrdDigis->GetEntries();

   for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
     CbmTrdDigi *digi = (CbmTrdDigi*) fTrdDigis->At(iDigi);
     Double_t charge = digi->GetCharge();

     Int_t digiAddress  = digi->GetAddress();
     Int_t layerId    = CbmTrdAddress::GetLayerId(digiAddress);
     Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
     Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
     Int_t iSector = CbmTrdAddress::GetSectorId(digi->GetAddress());
     Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
     Int_t secCol = CbmTrdAddress::GetColumnId(digi->GetAddress());

     fModuleInfo = fDigiPar->GetModule(moduleAddress);

//     printf("digi: %i; layer: %i; module: %i, sect: %i, col: %i, row: %i; digiAddr: %i\n",//; nofCols:%i\n",
//     	      iDigi, layerId, moduleId, iSector, secCol, secRow, digiAddress);//,
     Int_t nofCols = fModuleInfo->GetNofColumns();
     Int_t nofRows = fModuleInfo->GetNofRows();

     if(moduleAddress == 19349){
	 FillModuleDigis("19349", secCol, secRow, charge);
	//fHM->H2("hhh_Section1_Clustering_visualisation_H2")->SetMaximum(nofRows);
//	fHM->H2("hhh_Module1_Clustering_visualisation_col_H2")->Fill(secCol, secRow, charge);
//	fHM->H2("hhh_Module1_Clustering_visualisation_cont_H2")->Fill(secCol, secRow, charge);
     }
     if(moduleAddress == 901){
	 FillModuleDigis("901", secCol, secRow, charge);

	 //fHM->H2("hhh_Section1_Clustering_visualisation_H2")->;
//	fHM->H2("hhh_Module0_Clustering_visualisation_col_H2")->Fill(secCol, secRow, charge);
//	fHM->H2("hhh_Module0_Clustering_visualisation_cont_H2")->Fill(secCol, secRow, charge);
	file << eventNo <<  iDigi << "," << moduleId << "," << secCol << "," << secRow << "," << digiAddress << std::endl;
     }
     if(moduleAddress == 5){
	//fHM->H2("hhh_Section1_Clustering_visualisation_H2")->;
	 FillModuleDigis("5", secCol, secRow, charge);
     }
   }
   eventNo++;
}

void CbmLitClusteringQaTrd::FillModuleDigis(const string nr, Int_t col, Int_t row, Int_t charge)
{
  fHM->H2("hhh_Module" + nr + "_Clustering_visualisation_col_H2")->Fill(col, row, charge);
  fHM->H2("hhh_Module" + nr + "_Clustering_visualisation_cont_H2")->Fill(col, row, charge);
}

void CbmLitClusteringQaTrd::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
   fTrdDigiMatches = (TClonesArray*) ioman->GetObject("TrdDigiMatch");
   fTrdClusterMatches = (TClonesArray*) ioman->GetObject("TrdClusterMatch");
   fTrdHitMatches = (TClonesArray*) ioman->GetObject("TrdHitMatch");
}

void CbmLitClusteringQaTrd::ProcessPoints(
      const TClonesArray* points,
      const string& detName,
      DetectorId detId)
{
   string histName = "hno_NofObjects_" + detName + "Points_Station";
   if (NULL == points || !fHM->Exists(histName)) return;
   for (Int_t i = 0; i < points->GetEntriesFast(); i++) {
      const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(i));
      fHM->H1(histName)->Fill(CbmTrdAddress::GetLayerId(point->GetDetectorID()));
   }
}

void CbmLitClusteringQaTrd::ProcessDigis(
      const TClonesArray* digis,
      const TClonesArray* digiMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL == digis || !fHM->Exists("hno_NofObjects_" + detName + "Digis_Station")) return;
   for (Int_t i = 0; i < digis->GetEntriesFast(); i++) {
      const CbmDigi* digi = static_cast<const CbmDigi*>(digis->At(i));
      const CbmMatch* digiMatch = static_cast<const CbmMatch*>(digiMatches->At(i));
      Int_t stationId = CbmTrdAddress::GetLayerId(digi->GetAddress());
      fHM->H1("hno_NofObjects_" + detName + "Digis_Station")->Fill(stationId);
      fHM->H1("hpa_" + detName + "Digi_NofPointsInDigi_H1")->Fill(digiMatch->GetNofLinks());
      fHM->H1("hpa_" + detName + "Digi_NofPointsInDigi_H2")->Fill(stationId, digiMatch->GetNofLinks());
   }
}

void CbmLitClusteringQaTrd::ProcessClusters(
      const TClonesArray* clusters,
      const TClonesArray* clusterMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL != clusters && fHM->Exists("hno_NofObjects_" + detName + "Clusters_Station")) {
      for (Int_t i = 0; i < clusters->GetEntriesFast(); i++) {
         const CbmCluster* cluster = static_cast<const CbmCluster*>(clusters->At(i));
         const CbmMatch* clusterMatch = static_cast<const CbmMatch*>(clusterMatches->At(i));
         Int_t stationId = CbmTrdAddress::GetLayerId(cluster->GetAddress());
         fHM->H1("hno_NofObjects_" + detName + "Clusters_Station")->Fill(stationId);
         fHM->H1("hpa_" + detName + "Cluster_NofDigisInCluster_H1")->Fill(cluster->GetNofDigis());
         fHM->H1("hpa_" + detName + "Cluster_NofDigisInCluster_H2")->Fill(stationId, cluster->GetNofDigis());
         fHM->H1("hpa_" + detName + "Cluster_NofPointsInCluster_H1")->Fill(clusterMatch->GetNofLinks());
         fHM->H1("hpa_" + detName + "Cluster_NofPointsInCluster_H2")->Fill(stationId, clusterMatch->GetNofLinks());
      }
   }
}

void CbmLitClusteringQaTrd::ProcessHits(
      const TClonesArray* hits,
      const TClonesArray* hitMatches,
      const string& detName,
      DetectorId detId)
{
   if (NULL != hits && fHM->Exists("hno_NofObjects_" + detName + "Hits_Station")) {
      for (Int_t i = 0; i < hits->GetEntriesFast(); i++) {
         const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(i));
         const CbmMatch* hitMatch = static_cast<const CbmMatch*>(hitMatches->At(i));
         Int_t stationId = CbmTrdAddress::GetLayerId(hit->GetAddress());
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

void CbmLitClusteringQaTrd::FillResidualAndPullHistograms(
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
      Int_t stationId = CbmTrdAddress::GetLayerId(hit->GetAddress());
      fHM->H2(nameResidualX)->Fill(stationId, residualX);
      fHM->H2(nameResidualY)->Fill(stationId, residualY);
      fHM->H2(namePullX)->Fill(stationId, residualX / hit->GetDx());
      fHM->H2(namePullY)->Fill(stationId, residualY / hit->GetDy());
   }
}

void CbmLitClusteringQaTrd::FillHitEfficiencyHistograms(
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
      fHM->H1(accName)->Fill(CbmTrdAddress::GetLayerId(point->GetDetectorID()));
   }

   string recName = "hhe_" + detName + "_All_Rec_Station";
   string cloneName = "hhe_" + detName + "_All_Clone_Station";
   set<Int_t> mcPointSet; // IDs of MC points
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(iHit));
      const CbmMatch* match = static_cast<const CbmMatch*>(hitMatches->At(iHit));
      if (mcPointSet.find(match->GetMatchedLink().GetIndex()) == mcPointSet.end()) {
         fHM->H1(recName)->Fill(CbmTrdAddress::GetLayerId(hit->GetAddress()));
         mcPointSet.insert(match->GetMatchedLink().GetIndex());
      } else {
         fHM->H1(cloneName)->Fill(CbmTrdAddress::GetLayerId(hit->GetAddress()));
      }
   }
}

void CbmLitClusteringQaTrd::CreateHistograms()
{
   CreateNofObjectsHistograms(kTRD, "Trd", "Station", "Station number");
   CreateNofObjectsHistograms(kTRD, "Trd");
   CreateClusterParametersHistograms(kTRD, "Trd");
   CreateHitEfficiencyHistograms(kTRD, "Trd", "Station", "Station number", 100, -0.5, 99.5);

   // Histogram for the Sections (Cols and Rows)
   fModuleInfo = fDigiPar->GetModule(19349);
   Int_t rows = fModuleInfo->GetNofRows();
   Int_t cols = fModuleInfo->GetNofColumns();
   fHM->Create2<TH2F>("hhh_Module19349_Clustering_visualisation_cont_H2", "hhh_Module19349_Clustering_visualisation_cont_H2", cols, 0, cols, rows, 0, rows);
   fHM->Create2<TH2F>("hhh_Module19349_Clustering_visualisation_col_H2", "hhh_Module19349_Clustering_visualisation_col_H2", cols, 0, cols, rows, 0, rows);

   fModuleInfo = fDigiPar->GetModule(901);
   rows = fModuleInfo->GetNofRows();
   cols = fModuleInfo->GetNofColumns();
   fHM->Create2<TH2F>("hhh_Module901_Clustering_visualisation_cont_H2", "hhh_Module901_Clustering_visualisation_cont_H2", cols, 0, cols, rows, 0, rows);
   fHM->Create2<TH2F>("hhh_Module901_Clustering_visualisation_col_H2", "hhh_Module901_Clustering_visualisation_col_H2", cols, 0, cols, rows, 0, rows);

   fModuleInfo = fDigiPar->GetModule(5);
   rows = fModuleInfo->GetNofRows();
   cols = fModuleInfo->GetNofColumns();
   fHM->Create2<TH2F>("hhh_Module5_Clustering_visualisation_cont_H2", "hhh_Module5_Clustering_visualisation_cont_H2", cols, 0, cols, rows, 0, rows);
   fHM->Create2<TH2F>("hhh_Module5_Clustering_visualisation_col_H2", "hhh_Module5_Clustering_visualisation_col_H2", cols, 0, cols, rows, 0, rows);

   // Histogram stores number of events
   fHM->Create1<TH1F>("hen_EventNo_ClusteringQa", "hen_EventNo_ClusteringQa", 1, 0, 1.);
}

void CbmLitClusteringQaTrd::CreateNofObjectsHistograms(
		DetectorId detId,
		const string& detName)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kTRD);
   Int_t nofBins = 100000;
   Double_t minX = -0.5;
   Double_t maxX = 99999.5;
   string name = "hno_NofObjects_" + detName;
   fHM->Create1<TH1F>(name + "Points_Event", name + "Points_Event;Points per event;Counter", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Digis_Event", name + "Digis_Event;Digis per event;Counter", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Clusters_Event", name + "Clusters_Event;Clusters per event;Counter", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Hits_Event", name + "Hits_Event;Hits per event;Counter", nofBins, minX, maxX);
}

void CbmLitClusteringQaTrd::CreateNofObjectsHistograms(
      DetectorId detId,
      const string& detName,
      const string& parameter,
      const string& xTitle)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kTRD);
   Int_t nofBins = 100;
   Double_t minX = -0.5;
   Double_t maxX = 99.5;
   string name = "hno_NofObjects_" + detName;
   fHM->Create1<TH1F>(name + "Points_" + parameter, name + "Points_" + parameter + ";" + xTitle + ";Points per event", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Digis_" + parameter, name + "Digis_" + parameter + ";" + xTitle + ";Digis per event", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Clusters_" + parameter, name + "Clusters_" + parameter + ";" + xTitle + ";Clusters per event", nofBins, minX, maxX);
   fHM->Create1<TH1F>(name + "Hits_" + parameter, name + "Hits_" + parameter + ";" + xTitle + ";Hits per event", nofBins, minX, maxX);
}

void CbmLitClusteringQaTrd::CreateClusterParametersHistograms(
      DetectorId detId,
      const string& detName)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kTRD);
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

   // Number of (Digis|Points) in Cluster
   string nameH1 = "hpa_" + detName + "Cluster_NofDigisInCluster_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of digis;Yield", nofBins, min, max);
   string nameH2 = "hpa_" + detName + "Cluster_NofDigisInCluster_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of digis;Yield", nofBinsStation, minStation, max, nofBins, min, max);
   nameH1 = "hpa_" + detName + "Cluster_NofPointsInCluster_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
   nameH2 = "hpa_" + detName + "Cluster_NofPointsInCluster_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, max, nofBins, min, max);

   // Number of Points in Digi
   nameH1 = "hpa_" + detName + "Digi_NofPointsInDigi_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
   nameH2 = "hpa_" + detName + "Digi_NofPointsInDigi_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, maxStation, nofBins, min, max);

   // Number of Points in Hit
   nameH1 = "hpa_" + detName + "Hit_NofPointsInHit_H1";
   fHM->Create1<TH1F>(nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
   nameH2 = "hpa_" + detName + "Hit_NofPointsInHit_H2";
   fHM->Create2<TH2F>(nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, max, nofBins, min, max);

   // Sigma(X|Y)
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

void CbmLitClusteringQaTrd::CreateHitEfficiencyHistograms(
      DetectorId detId,
      const string& detName,
      const string& parameter,
      const string& xTitle,
      Int_t nofBins,
      Double_t minBin,
      Double_t maxBin)
{
   if (!fDet.GetDet(detId)) return;
   assert(detId == kTRD);
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

ClassImp(CbmLitClusteringQaTrd)
