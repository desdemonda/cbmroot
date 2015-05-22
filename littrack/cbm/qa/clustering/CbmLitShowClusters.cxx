/*
 * CbmLitShowClusters.cxx
 *
 *  Created on: 18.05.2015
 *      Author: jonathan
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
#include "CbmLitShowClustersReport.h"

#include "TSystem.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TMarker.h"

#include <boost/assign/list_of.hpp>
#include <cmath>		// for math functions
#include <iostream>		// for cli output
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

#include "CbmLitShowClusters.h"
#include "CbmLitClusteringQaTrdReport.h"
using std::cout;
using std::endl;
using std::vector;
using boost::assign::list_of;


CbmLitShowClusters::CbmLitShowClusters():
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
//   fMarker(),
   fMuchDigiFileName("")
{

}

CbmLitShowClusters::~CbmLitShowClusters()
{
   if (fHM) delete fHM;
}

/**
 * \brief Initialization (Creation of histograms)
 */
InitStatus CbmLitShowClusters::Init()
{
   // Create histogram manager which is used throughout the program
   fHM = new CbmHistManager();

   fDet.DetermineSetup();
   ReadDataBranches();

   fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
   assert( fDigiPar != nullptr );

   CreateHistograms();

   return kSUCCESS;
}

/**
 * \brief Execution (Filling of histograms)
 */
void CbmLitShowClusters::Exec(
    Option_t* opt)
{
  ProcessSectorHistos();

  // Increment and output the event counter
  fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
  std::cout << "CbmLitShowClusters::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;
}

/**
 * Finish Execution (Write results to file)
 */
void CbmLitShowClusters::Finish()
{
  fHM->WriteToFile();
  CbmSimulationReport* report = new CbmLitShowClustersReport();
  report->Create(fHM, fOutputDir);
  delete report;
}

void CbmLitShowClusters::ProcessSectorHistos()
{
   static Int_t eventNo = 1;
   if(eventNo > 1)
     return;

/*   TString fileName = TString("test") + TString(".csv");

   std::ofstream file(fileName);
   if(!file){
      cerr << "Uh oh, test.csv could not be opened for writing!" << std::endl;
      return;
   }
   file << "eventNo, iDigi, moduleId, secCol, secRow, charge, digiAddress" << std::endl;*/

   Int_t nentries = fTrdDigis->GetEntries();
   map<UInt_t, vector<TEllipse*>*> markerMap;
   Int_t stepSize = 100;

   for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
     CbmTrdDigi *digi = (CbmTrdDigi*) fTrdDigis->At(iDigi);
     Double_t charge = digi->GetCharge();

     Int_t digiAddress  = digi->GetAddress();
     Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
     Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
     Int_t layerId = CbmTrdAddress::GetLayerId(digiAddress);
     Int_t sectorAddr = CbmTrdAddress::GetSectorAddress(digiAddress);
     Int_t sectorId = CbmTrdAddress::GetSectorId(digiAddress);
     Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
     Int_t secCol = CbmTrdAddress::GetColumnId(digi->GetAddress());

     if(layerId == 0){
	  cout << layerId << ", " << moduleAddress << ", " << moduleId << ", " << secCol << ", " << secRow << ", " << charge << std::endl;
	  stringstream histo, histoRow, histoRow2;
	  histo << "hhh_Layer1_Module" << moduleId << "_Clustering_visualisation_col_H2";

	  TVector3 posHit;
	  TVector3 padSize;
	  fModuleInfo = fDigiPar->GetModule(moduleAddress);
	  fModuleInfo->GetPosition(moduleAddress, sectorId, secCol, secRow, posHit, padSize);

	  Double_t nofRows = fModuleInfo->GetNofRows();
	  Double_t nofCols = fModuleInfo->GetNofColumns();
	  Double_t sizeX = fModuleInfo->GetSizeX();
	  Double_t sizeY = fModuleInfo->GetSizeY();
	  Double_t midX = fModuleInfo->GetX();
	  Double_t midY = fModuleInfo->GetY();
	  Double_t cornX = midX-sizeX;
	  Double_t cornY = midY-sizeY;

	  Double_t x = posHit.X() - cornX;
	  Double_t y = posHit.Y() - cornY;
	  Double_t padx = padSize.X();
	  Double_t pady = padSize.Y();

	  if( ! fHM->Exists(histo.str()) ){
	     fHM->Create2<TH2D>(histo.str().c_str(), "Clustering Visualisation;X [cm];Y [cm]", stepSize, 0., 2*sizeX, stepSize, 0., 2*sizeY);
	  }
	  if( sectorId == 1 ){
	    histoRow << "hhh_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_col_H2";
	    histoRow2 << "hhh_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_cont_H2";
	    if( ! fHM->Exists(histoRow.str()) ){
	      fHM->Create2<TH2D>(histoRow.str().c_str(), "Clustering Visualisation;Column;Row", nofCols, 0., nofCols, nofRows, 0., nofRows);
	      fHM->Create2<TH2D>(histoRow2.str().c_str(), "Clustering Visualisation;Column;Row", nofCols, 0., nofCols, nofRows, 0., nofRows);
	    }
	    TH2 *pad2 = fHM->H2(histoRow.str());
	    TH2 *pad3 = fHM->H2(histoRow2.str());

	    pad2->Fill(secCol, secRow, charge);
	    pad3->Fill(secCol, secRow, charge);

	  }
	  TH2 *pad = fHM->H2(histo.str());
	  pad->Fill(x, y, charge);
	  TEllipse *m = new TEllipse(x, y, padx, pady);  // Define the Marker

	  vector<TEllipse*> *moduleVector = markerMap[moduleAddress];
	  moduleVector->push_back(m);

     }
   }

   // Histogram for the Modules
   cout << "Getting Module Informations" << std::endl;
   std::map<Int_t, CbmTrdModule*> map = fDigiPar->GetModuleMap();
   std::cout << map.size() << std::endl;
   for(const auto& v : map) {
       std::cout << v.first << " --> " << v.second->GetSizeX() << std::endl;
   }

   for(const auto& v : markerMap){
     UInt_t moduleAddress = v.first;
     UInt_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);
     Int_t sectorId = 1;
     fModuleInfo = fDigiPar->GetModule(moduleAddress);
     if( fModuleInfo == nullptr )
       continue;

     stringstream histo, box, histoRow, histoRow2;
     histo << "hhh_Layer1_Module" << moduleId << "_Clustering_visualisation_col_H2";
     box << "hhh_Layer1_Module" << moduleId << "_Box_visualisation";
/*     if( sectorId == 1 ){
       histoRow << "hhh_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_col_H2";
       histoRow2 << "hhh_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_cont_H2";
       TH2 *pad2 = fHM->H2(histoRow.str());
       TH2 *pad3 = fHM->H2(histoRow2.str());
       TCanvas *c2 = new TCanvas(histoRow.str().c_str(), histoRow.str().c_str());
       TCanvas *c3 = new TCanvas(histoRow2.str().c_str(), histoRow2.str().c_str());
       pad2->Draw("colz");
       pad3->Draw("contz");
     }*/
     Double_t maxX = fModuleInfo->GetSizeX();
     Double_t maxY = fModuleInfo->GetSizeY();
     cout << "Module [" << moduleId << "]: " <<  maxX << ", " << maxY << std::endl;

     TCanvas *c1 = new TCanvas(box.str().c_str(), box.str().c_str());
     c1->SetGrid(true, true);
     gPad->Range(0., 0., 2*maxX, 2*maxY);
//     pad->SetStats(false);
     vector<TEllipse*> *moduleVector = v.second;
     for(const auto& m : *moduleVector){
//	TBox *m = moduleVector.back();
 	cout << "Drawing Module [" << moduleId << "]: " << m->GetX1() << ", " << m->GetY1() << ", " << m->GetR1() << ", " << m->GetR2() << endl;
 	m->Draw();
//	pad->Fill(m->GetX1(), m->GetY1());
     }
/*     TCanvas *c = new TCanvas(histo.str().c_str(), histo.str().c_str());
     TH2 *pad = fHM->H2(histo.str());
     pad->Draw("colz");
*/
   }
   eventNo++;
}

void CbmLitShowClusters::ReadDataBranches()
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

void CbmLitShowClusters::CreateHistograms()
{
   // Histogram stores number of events
   fHM->Create1<TH1F>("hen_EventNo_ClusteringQa", "hen_EventNo_ClusteringQa", 1, 0, 1.);
}


