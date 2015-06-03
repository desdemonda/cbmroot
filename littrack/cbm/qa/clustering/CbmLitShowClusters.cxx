/*
 * CbmLitShowClusters.cxx
 *
 *  Created on: 18.05.2015
 *      Author: jonathan
 */


#include "CbmHistManager.h"
//#include "CbmHit.h"
//#include "CbmBaseHit.h"
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
#include "TH2Poly.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TMarker.h"
#include "TBox.h"
#include "TGaxis.h"

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

   Int_t nofClusters = fTrdClusters->GetEntries();
   for(Int_t iCluster=0; iCluster < nofClusters; iCluster++ ){
       CbmCluster *cluster = (CbmCluster*) fTrdClusters->At(iCluster);
       printf("Found Cluster [%u] consist of %u Digis.\n", iCluster, cluster->GetNofDigis());
   }
/*   TString fileName = TString("test") + TString(".csv");

   std::ofstream file(fileName);
   if(!file){
      cerr << "Uh oh, test.csv could not be opened for writing!" << std::endl;
      return;
   }
   file << "eventNo, iDigi, moduleId, secCol, secRow, charge, digiAddress" << std::endl;*/

   Int_t nentries = fTrdDigis->GetEntries();
   map<UInt_t, map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*>*> markerMap;
   Int_t stepSize = 100;

   for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
      CbmTrdDigi *digi = (CbmTrdDigi*) fTrdDigis->At(iDigi);
      Double_t charge = digi->GetCharge();
      Double_t digiTime = digi->GetTime();

      Int_t digiAddress  = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
      Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
      Int_t layerId = CbmTrdAddress::GetLayerId(digiAddress);
      if(layerId != 0)
        continue;
      Int_t sectorAddr = CbmTrdAddress::GetSectorAddress(digiAddress);
      Int_t sectorId = CbmTrdAddress::GetSectorId(digiAddress);
      Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
      Int_t secCol = CbmTrdAddress::GetColumnId(digi->GetAddress());

      cout << layerId << ", " << moduleAddress << ", " << moduleId << ", " << secCol << ", " << secRow << ", " << charge << std::endl;
      stringstream histoRow;
      histoRow << "hhh_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_col_H2";

      TVector3 posHit;
      TVector3 padSize;
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      fModuleInfo->GetPosition(moduleAddress, sectorId, secCol, secRow, posHit, padSize);

      padSize *= 1/TMath::Sqrt(12.);
      Double_t nofRows = fModuleInfo->GetNofRowsInSector(1);
      Double_t nofCols = fModuleInfo->GetNofColumnsInSector(1);
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

      cout << "Module Info: " << midX << ", " << midY << endl;

      if( moduleId < 5 ){
	if( ! fHM->Exists(histoRow.str()) ){
	  fHM->Create2<TH2D>(histoRow.str().c_str(), "Clustering Visualisation;Column;Row", nofCols, 0., nofCols, nofRows, 0., nofRows);
	}
	TH2 *pad2 = fHM->H2(histoRow.str());
	pad2->Fill(secCol, secRow, charge);
      }

      if(markerMap[moduleAddress] == nullptr)
	markerMap[moduleAddress] = new map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*>;
      map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*> *moduleVector = markerMap[moduleAddress];
      if( !moduleVector->count(sectorId) )
	moduleVector->insert(pair<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*>(sectorId, new map<pair<Int_t, Int_t>, CbmTrdDigi*>));
      map<pair<Int_t, Int_t>, CbmTrdDigi*>* sectorMap = moduleVector->at(sectorId);

      cout << "inserting Digi (" << sectorId << ", " << secCol << ", " << secRow << ")" << endl;
      sectorMap->insert(pair<pair<Int_t, Int_t>, CbmTrdDigi*>(pair<Int_t, Int_t>(secCol, secRow), digi));

   }

   // Histogram for the Modules
   cout << "Getting Module Informations" << std::endl;

   for(const auto& v : markerMap){
     UInt_t moduleAddress = v.first;
     Int_t layerId = CbmTrdAddress::GetLayerId(moduleAddress);
     UInt_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);
     fModuleInfo = fDigiPar->GetModule(moduleAddress);
     if( fModuleInfo == nullptr )
       continue;

     Double_t sizeX = fModuleInfo->GetSizeX();
     Double_t sizeY = fModuleInfo->GetSizeY();
     Double_t midX = fModuleInfo->GetX();
     Double_t midY = fModuleInfo->GetY();

     stringstream histo, histo0, box, box2, histoRow, histoRow2;
     histo << "hhh_Layer1_Module" << moduleId << "_Clustering_visualisation_col_H2";
     histo0 << "hhh_Layer1_Module" << moduleId << "_Clustering_visualisation0_col_H2";
     box << "hhh_Layer1_Module" << moduleId << "_Box_visualisation";
     box2 << "hhh_Layer1_Module" << moduleId << "_Box_time_visualisation";
     histoRow2 << "hhh_Layer1_Module" << moduleId << "_Time_Clustering_visualisation_col_H2";

     if( ! fHM->Exists(histo.str()) ){
	TH2Poly *pad = new TH2Poly(histo.str().c_str(), "Clustering Visualisation;X [cm];Y [cm]", 0., 2*sizeX, 0., 2*sizeY);
	fHM->Add(histo.str().c_str(), pad);
     }
     if( ! fHM->Exists(histoRow2.str()) ){
	 TH2Poly *tmpPad = new TH2Poly(histoRow2.str().c_str(), "Time Clustering Visualisation;X [cm];Y [cm]", 0., 2*sizeX, 0., 2*sizeY);
	 fHM->Add(histoRow2.str().c_str(), tmpPad);
     }

     TH2Poly *pad = (TH2Poly*)fHM->H2(histo.str());
     TH2Poly *pad3 = (TH2Poly*)fHM->H2(histoRow2.str());

     map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*> *moduleMap = v.second;


     TCanvas *c1 = new TCanvas(box.str().c_str(), box.str().c_str());
     c1->SetGrid(true, true);
     c1->Range(-5,-5,2*sizeX+5,2*sizeY+5);
     TGaxis *axis1 = new TGaxis(0,0,2*sizeX,0,0,2*sizeX,510,"", 1); // X-Axis
     axis1->SetName("axis1");
     axis1->SetTitle("X [cm]");
     axis1->Draw();
     TGaxis *axis2 = new TGaxis(0,0,0,2*sizeY,0,2*sizeY,510,"", 1); // Y-Axis
     axis2->SetName("axis2");
     axis2->SetTitle("Y [cm]");
     axis2->Draw();

/*     TCanvas *c2 = new TCanvas(box2.str().c_str(), box2.str().c_str());
     c2->SetGrid(true, true);
     c2->Range(-5,-5,2*sizeX+5,2*sizeY+5);
     TGaxis *axis3 = new TGaxis(0,0,2*sizeX,0,0,2*sizeX,510,"", 1); // X-Axis
     axis3->SetName("axis3");
     axis3->SetTitle("X [cm]");
     axis3->Draw();
     TGaxis *axis4 = new TGaxis(0,0,0,2*sizeY,0,2*sizeY,510,"", 1); // Y-Axis
     axis4->SetName("axis4");
     axis4->SetTitle("Y [cm]");
     axis4->Draw();*/
     for(const auto& module : *moduleMap ){
	Int_t sectorId = module.first;
	map<pair<Int_t, Int_t>, CbmTrdDigi*> *sectorMap = module.second;
	Double_t nofRows = fModuleInfo->GetNofRowsInSector(sectorId);
	Double_t nofCols = fModuleInfo->GetNofColumnsInSector(sectorId);


	for(Int_t curCol=0; curCol < nofCols; ++curCol){
	  for(Int_t curRow=0; curRow < nofRows; ++curRow){
	     Int_t digiAddress = CbmTrdAddress::GetAddress(layerId, moduleId, sectorId, curRow, curCol);
	     TVector3 posHit;
	     TVector3 padSize;
	     fModuleInfo->GetPosition(moduleAddress, sectorId, curCol, curRow, posHit, padSize);

	     padSize *= 1/TMath::Sqrt(12.);
	     Double_t cornX = midX-sizeX;
	     Double_t cornY = midY-sizeY;
	     Double_t x = posHit.X() - cornX;
	     Double_t y = posHit.Y() - cornY;
	     Double_t padx = padSize.X();
	     Double_t pady = padSize.Y();


	     Double_t digiCharge=1e-9;
	     Double_t digiTime=0;
	     if( sectorMap->count(pair<Int_t, Int_t>(curCol, curRow)) != 0 ){
		pad->AddBin(x-padx, y-pady, x+padx, y+pady);
		pad3->AddBin(x-padx, y-pady, x+padx, y+pady);
		CbmTrdDigi *digi = sectorMap->at(pair<Int_t,Int_t>(curCol, curRow));
		digiCharge = digi->GetCharge();
		digiTime = digi->GetTime();
		TBox *digiMarker = new TBox(x-padx, y-pady, x+padx, y+pady);
		digiMarker->SetFillColor(kViolet-(digiCharge*1e7));
		digiMarker->SetFillStyle((digiCharge>1e7)?3002:3001);
	        digiMarker->Draw();
	        pad->Fill(x, y, digiCharge);
	        pad3->Fill(x, y, digiTime);
	     }else{
		TBox *digiMarker = new TBox(x-padx, y-pady, x+padx, y+pady);
		digiMarker->SetFillColor(kBlue);
		digiMarker->SetFillStyle(3003);
		digiMarker->Draw();
	     }
	  } // end for rows
	} // end for cols
     } // end for module : *moduleMap
     stringstream boxPng, box2Png;
     boxPng << fOutputDir << "png/" << box.str().c_str() << ".png";
//     box2Png << fOutputDir << "png/" << box2.str().c_str() << ".png";
     c1->Print(boxPng.str().c_str());
//     c2->Print(box2Png.str().c_str());
   } // end for v : markerMap
   eventNo++;

   for(const auto& v : markerMap)
     delete v.second;
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


