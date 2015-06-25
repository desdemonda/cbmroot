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
#include "CbmTrdCluster.h"
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
#include "TH3.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TMarker.h"
#include "TBox.h"
#include "TMarker3DBox.h"
#include "TGaxis.h"
#include "TPaletteAxis.h"
#include "TLatex.h"

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
  Int_t nofModules = fDigiPar->GetNrOfModules();
  for(Int_t i=0; i<nofModules; i++){
    if(i>30) break;
    Int_t moduleAddr = fDigiPar->GetModuleId(i);
    Int_t moduleId = CbmTrdAddress::GetModuleId(moduleAddr);
    Int_t layerId = CbmTrdAddress::GetLayerId(moduleAddr);
    fModuleInfo = fDigiPar->GetModule(moduleAddr);

    Double_t sizeX = fModuleInfo->GetSizeX();
    Double_t sizeY = fModuleInfo->GetSizeY();

    // Creating Canvas
    stringstream box, boxPng;
    box << "bbb_Layer_" << layerId << "_Module_" << moduleId << "_box";
    boxPng << "test/png/" << box.str().c_str() << ".png";

    printf("Creating Canvas: %s\n", box.str().c_str());

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

    for(Int_t sectorId=0; sectorId < fModuleInfo->GetNofSectors(); sectorId++ ){
      Double_t nofRows = fModuleInfo->GetNofRowsInSector(sectorId);
      Double_t nofCols = fModuleInfo->GetNofColumnsInSector(sectorId);

      for(Int_t curCol=0; curCol < nofCols; ++curCol){
	for(Int_t curRow=0; curRow < nofRows; ++curRow){
	  TVector3 posHit;
	  TVector3 padSize;
	  fModuleInfo = fDigiPar->GetModule(moduleId);
	  fModuleInfo->GetPosition(moduleId, sectorId, curCol, curRow, posHit, padSize);

	  Double_t midX = fModuleInfo->GetX(); // module center
	  Double_t midY = fModuleInfo->GetY();
	  Double_t cornX = midX-sizeX;  // lower left corner of module
	  Double_t cornY = midY-sizeY;
	  Double_t x = posHit.X() - cornX; // x position of the pad
	  Double_t y = posHit.Y() - cornY;

	  Double_t padx = padSize.X()/2;
	  Double_t pady = padSize.Y()/2;


	  TBox *digiMarker = new TBox(x-padx, y-pady, x+padx, y+pady);
	  digiMarker->SetFillColor(kGray);
	  digiMarker->Draw();
	}
      }
    }
    c1->SaveAs(boxPng.str().c_str());
  }

  ProcessSectorHistosOld();
}

void CbmLitShowClusters::ProcessSectorHistosOld()
{
   static Int_t eventNo = 1;
   if(eventNo > 1)
     return;

   Int_t nofClusters = fTrdClusters->GetEntries();
   map<Int_t, vector<CbmTrdCluster*>> clusterMap;
   fHM->Create1<TH1F>("hno_NofObjects_digis_per_cluster_H1", "Digis per Cluster", 100, -.5, 99.5);
   for(Int_t iCluster=0; iCluster < nofClusters; iCluster++ ){
       CbmTrdCluster *cluster = (CbmTrdCluster*) fTrdClusters->At(iCluster);
       Int_t nofDigis = cluster->GetNofDigis();
       Int_t clusterAddress = cluster->GetAddress();
       Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(clusterAddress);
       Int_t moduleId = CbmTrdAddress::GetModuleId(clusterAddress);
       Int_t layerId = CbmTrdAddress::GetLayerId(clusterAddress);
       TH1F *h = (TH1F*)fHM->H1("hno_NofObjects_digis_per_cluster_H1");
       h->Fill(nofDigis);
       if(layerId != 1)
         continue;
       printf("Found Cluster [%u] consist of %u Digis.\n", iCluster, nofDigis);

/*       if( !clusterMap->count(moduleAddress) )
	 clusterMap->insert(pair<Int_t, vector<CbmTrdCluster*>*>(moduleAddress, new vector<CbmTrdCluster*>));*/
//       if( clusterMap[moduleAddress] == nullptr)
//	 clusterMap[moduleAddress] = new vector<CbmTrdCluster*>;
       //vector<CbmTrdCluster*> *clusterVector = ;
       clusterMap[moduleId].push_back(cluster);
   }
/*   TString fileName = TString("test") + TString(".csv");

   std::ofstream file(fileName);
   if(!file){
      cerr << "Uh oh, test.csv could not be opened for writing!" << std::endl;
      return;
   }
   file << "eventNo, iDigi, moduleId, secCol, secRow, charge, digiAddress" << std::endl;*/

   Int_t nentries = fTrdDigis->GetEntries();
   map<UInt_t, map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>>> markerMap;
   Int_t stepSize = 100;
   Int_t maxLayer = 0;
   Int_t maxModuleId = 0;
   Int_t maxSectorId = 0;

//   TCanvas *c = new TCanvas("had_AllDigis_3d", "All Digis", 1600, 900);

   for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
      CbmTrdDigi *digi = (CbmTrdDigi*) fTrdDigis->At(iDigi);
      Double_t charge = digi->GetCharge();
      Double_t digiTime = digi->GetTime();

      Int_t digiAddress  = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
      Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
      Int_t layerId = CbmTrdAddress::GetLayerId(digiAddress);
      Int_t sectorAddr = CbmTrdAddress::GetSectorAddress(digiAddress);
      Int_t sectorId = CbmTrdAddress::GetSectorId(digiAddress);
      Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
      Int_t secCol = CbmTrdAddress::GetColumnId(digi->GetAddress());

//      cout << layerId << ", " << moduleAddress << ", " << moduleId << ", " << secCol << ", " << secRow << ", " << charge << std::endl;
      stringstream histoRow, histoRowLog, histo0, histo1;
      histoRow << "hhs_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_col_H2_lin";
      histoRowLog << "hhs_Layer1_Module" << moduleId << "_Sector" << sectorId << "_Clustering_visualisation_col_H2_log";
      histo0 << "hhc_Layer1_Module" << moduleId << "_Clustering_visualisation0_col_H2";
      histo1 << "hhc_Layer1_Module" << moduleId << "_Clustering_visualisation1_col_H2";

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
      Double_t midZ = fModuleInfo->GetZ();
      Double_t cornX = midX-sizeX;
      Double_t cornY = midY-sizeY;
      Double_t x = posHit.X() - cornX;
      Double_t y = posHit.Y() - cornY;
      Double_t padx = padSize.X();
      Double_t pady = padSize.Y();
      Double_t padz = padSize.Z();

      // TMarker3DBox(Float_t x, Float_t y, Float_t z, Float_t dx, Float_t dy, Float_t dz, Float_t theta, Float_t phi)
//      Int_t weight = 0;
//      for(Double_t i=1e-4; i > charge; i/=10) weight++;
//      TMarker3DBox *m = new TMarker3DBox(posHit.X(),posHit.Y(),posHit.Z(), padx, pady, padz, 0, 0);
//      m->SetFillColor(kViolet - weight);
//      m->Draw();


      if(layerId > maxLayer)  maxLayer = layerId;
      if(moduleId > maxModuleId)  maxModuleId = moduleId;
      if(sectorId > maxSectorId)  maxSectorId = sectorId;
      if(layerId != 1) continue;

      if( moduleId < 5 ){
	if( ! fHM->Exists(histoRow.str()) ){
	  stringstream histTitle;
	  histTitle << "Sector " << sectorId << " Visualization;Column;Row";
	  fHM->Create2<TH2D>(histoRow.str().c_str(), histTitle.str().c_str(), nofCols, 0., nofCols, nofRows, 0., nofRows);
	  fHM->Create2<TH2D>(histoRowLog.str().c_str(), histTitle.str().c_str(), nofCols, 0., nofCols, nofRows, 0., nofRows);
	}
	TH2 *pad1 = fHM->H2(histoRow.str());
	TH2 *pad2 = fHM->H2(histoRowLog.str());
	pad1->Fill(secCol, secRow, charge);
	pad2->Fill(secCol, secRow, charge);
      }

/*      if(markerMap[moduleAddress] == nullptr)
	markerMap[moduleAddress] = new map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*>;
      map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*> *moduleVector = markerMap[moduleAddress];
      if( !moduleVector->count(sectorId) )
	moduleVector->insert(pair<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>*>(sectorId, new map<pair<Int_t, Int_t>, CbmTrdDigi*>));
      map<pair<Int_t, Int_t>, CbmTrdDigi*>* sectorMap = moduleVector->at(sectorId);*/

//      cout << "inserting Digi (" << sectorId << ", " << secCol << ", " << secRow << ")" << endl;
      markerMap[moduleAddress][sectorId].insert(pair<pair<Int_t, Int_t>, CbmTrdDigi*>(pair<Int_t, Int_t>(secCol, secRow), digi));

   } // end iDigi < nentries
//   c->SaveAs("test/png/had_AllDigis_3d.png");

   printf("MaxLayerId: %u, \tMaxModuleId: %u, \tMaxSectorId: %u\n", maxLayer, maxModuleId, maxSectorId);

   // Filling Histograms for each Module
   for(const auto& v : markerMap){
     UInt_t moduleAddress = v.first;
     Int_t layerId = CbmTrdAddress::GetLayerId(moduleAddress);
/*     stringstream warningMsg;
     warningMsg << "ModuleAddr: " << moduleAddress << ", layerId: " << layerId;
     if(layerId != 0)
       Warning("ProcessSectorHistos", warningMsg.str().c_str());*/
     assert(layerId == 0);
     UInt_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);
     fModuleInfo = fDigiPar->GetModule(moduleAddress);
     if( fModuleInfo == nullptr )
       continue;

     printf("Processing Module: %u, \tId: %u\n", moduleAddress, moduleId);

     Double_t sizeX = fModuleInfo->GetSizeX();
     Double_t sizeY = fModuleInfo->GetSizeY();
     Double_t sizeZ = fModuleInfo->GetSizeZ();
     Double_t midX = fModuleInfo->GetX();
     Double_t midY = fModuleInfo->GetY();
     Double_t midZ = fModuleInfo->GetZ();

     stringstream histo, histoLog, histo0, histo1, box, box2, histoRow2, histoRow2Log;
     histo << "hhh_Layer1_Module" << moduleId << "_Charge_visualisation_col_H2_lin";
     histoLog << "hhh_Layer1_Module" << moduleId << "_Charge_visualisation_col_H2_log";
     histo0 << "hhc_Layer1_Module" << moduleId << "_Clustering_visualisation0_col_H2";
     histo1 << "hhc_Layer1_Module" << moduleId << "_Clustering_visualisation1_col_H2";
     box << "hhh_Layer1_Module" << moduleId << "_Box_visualisation";
     box2 << "hhh_Layer1_Module" << moduleId << "_Cluster_Box_visualisation";
     histoRow2 << "hhh_Layer1_Module" << moduleId << "_Time_visualisation_col_H2_lin";
     histoRow2Log << "hhh_Layer1_Module" << moduleId << "_Time_visualisation_col_H2_log";

     if( ! fHM->Exists(histo.str()) ){
	TH2Poly *pad = new TH2Poly(histo.str().c_str(), "Charge Visualization;X [cm];Y [cm]", 0., 2*sizeX, 0., 2*sizeY);
	TH2Poly *pad2 = new TH2Poly(histoLog.str().c_str(), "Charge Visualization (logarithmic);X [cm];Y [cm]", 0., 2*sizeX, 0., 2*sizeY);
	fHM->Add(histo.str().c_str(), pad);
	fHM->Add(histoLog.str().c_str(), pad2);
     }
     if( ! fHM->Exists(histoRow2.str()) ){
	 TH2Poly *tmpPad = new TH2Poly(histoRow2.str().c_str(), "Time Visualization [ns];X [cm];Y [cm]", 0., 2*sizeX, 0., 2*sizeY);
	 TH2Poly *tmpPad2 = new TH2Poly(histoRow2Log.str().c_str(), "Time Visualization (logarithmic) [ns];X [cm];Y [cm]", 0., 2*sizeX, 0., 2*sizeY);
	 fHM->Add(histoRow2.str().c_str(), tmpPad);
	 fHM->Add(histoRow2Log.str().c_str(), tmpPad);
     }

     TH2Poly *pad = (TH2Poly*)fHM->H2(histo.str());
     TH2Poly *pad2 = (TH2Poly*)fHM->H2(histoLog.str());
     TH2Poly *pad3 = (TH2Poly*)fHM->H2(histoRow2.str());
     TH2Poly *pad4 = (TH2Poly*)fHM->H2(histoRow2Log.str());


     const map<Int_t, map<pair<Int_t, Int_t>, CbmTrdDigi*>>& moduleMap = v.second;


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

     Int_t maxWeight = 0;

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
     for(const auto& module : moduleMap ){
	Int_t sectorId = module.first;
	map<pair<Int_t, Int_t>, CbmTrdDigi*> sectorMap = module.second;
	Double_t nofRows = fModuleInfo->GetNofRowsInSector(sectorId);
	Double_t nofCols = fModuleInfo->GetNofColumnsInSector(sectorId);


	for(const auto &d : module.second){
	   CbmTrdDigi *digi = d.second;
	   Int_t digiAddress = digi->GetAddress();
	   Int_t moduleAddr = CbmTrdAddress::GetModuleAddress(digiAddress);
	   Int_t curCol = CbmTrdAddress::GetColumnId(digiAddress);
	   Int_t curRow = CbmTrdAddress::GetRowId(digiAddress);
	   TVector3 posHit;
	   TVector3 padSize;
	   fModuleInfo = fDigiPar->GetModule(moduleAddr);
	   fModuleInfo->GetPosition(moduleAddr, sectorId, curCol, curRow, posHit, padSize);

	   Double_t cornX = midX-sizeX;
	   Double_t cornY = midY-sizeY;
	   Double_t x = posHit.X() - cornX;
	   Double_t y = posHit.Y() - cornY;
	   Double_t padx = padSize.X()/2;
	   Double_t pady = padSize.Y()/2;

	   Double_t digiCharge = digi->GetCharge();
	   Double_t digiTime = digi->GetTime();
	   pad->AddBin(x-padx, y-pady, x+padx, y+pady);
	   pad2->AddBin(x-padx, y-pady, x+padx, y+pady);
	   pad3->AddBin(x-padx, y-pady, x+padx, y+pady);
	   pad4->AddBin(x-padx, y-pady, x+padx, y+pady);

	   TBox *digiMarker = new TBox(x-padx, y-pady, x+padx, y+pady);
	   Int_t weight = 0;
	   for(Double_t i=1e-4; i > digiCharge; i/=10) weight++;
	   if(weight > maxWeight) maxWeight = weight;
	   digiMarker->SetFillColor(kViolet-weight);
	   digiMarker->Draw();

	   pad->Fill(x, y, digiCharge);
	   pad2->Fill(x, y, digiCharge);
	   pad3->Fill(x, y, digiTime);
	   pad4->Fill(x, y, digiTime);
	} // end for v : module.second
     } // end for module : moduleMap

     // Drawing a Legend for Box visualization
     TLatex Tl;
     for(Int_t w=0; (w <= maxWeight or w>49); w++){
	 TBox *b = new TBox(2*sizeX+2, 2*sizeY+3-w, 2*sizeX+4.5, 2*sizeY+3-w-1);
	 b->SetFillColor(kViolet-w);
	 b->Draw();

	 if(w%5 != 0) continue;

	 stringstream txt;
	 txt << "10^{" << -4-w << "}";
	 Tl.DrawLatex(2*sizeX, 2*sizeY+3-w-.5, txt.str().c_str());
     }
     stringstream boxPng, box2Png;
     boxPng << fOutputDir << "png/" << box.str().c_str() << ".png";
     c1->Print(boxPng.str().c_str());


     // Creating Cluster Pad
/*     if(clusterMap[moduleId].size() == 0)
	continue;
     vector<CbmTrdCluster*> clusterVector = clusterMap[moduleId];

     TCanvas *c0 = new TCanvas(box2.str().c_str(), box2.str().c_str());
     c0->SetGrid(true, true);
     c0->Range(-5,-5,2*sizeX+5,2*sizeY+5);
     TGaxis *axis0 = new TGaxis(0,0,2*sizeX,0,0,2*sizeX,510,"", 1); // X-Axis
     axis0->SetName("axis0");
     axis0->SetTitle("X [cm]");
     axis0->Draw();
     TGaxis *axis0y = new TGaxis(0,0,0,2*sizeY,0,2*sizeY,510,"", 1); // Y-Axis
     axis0y->SetName("axis0y");
     axis0y->SetTitle("Y [cm]");
     axis0y->Draw();

     Int_t iColorCounter = 0;
     Int_t iShadeCounter = 0;
     Int_t clusterCounter = 0;
     vector<Color_t> colorVector = list_of(kOrange)(kPink)(kViolet)(kAzure)(kTeal)(kSpring);
     for(const auto& cluster : clusterVector){
	  stringstream hh0, hh1, hhT;
	  clusterCounter++;
	  hh0 << histo0.str() << "_cluster" << clusterCounter;
	  hh1 << histo1.str() << "_cluster" << clusterCounter;
	  hhT << "Cluster " << clusterCounter << " (" << cluster->GetNofDigis() << ");X [cm];Y [cm]";
	  if( ! fHM->Exists(hh0.str())){
	     TH2Poly *pad = new TH2Poly(hh0.str().c_str(), hhT.str().c_str(), 0., 2*sizeX, 0., 2*sizeY);
	     TH2Poly *pad1 = new TH2Poly(hh1.str().c_str(), hhT.str().c_str(), 0., 2*sizeX, 0., 2*sizeY);
	     //TH3F *pad = new TH3F(hh0.str().c_str(), hhT.str().c_str(), 2.5*sizeX, 0., 2*sizeX, 2.5*sizeY, 0., 2*sizeY, 10, 0., 10.);
	     //TH3F *pad1 = new TH3F(hh1.str().c_str(), hhT.str().c_str(), 2.5*sizeX, 0., 2*sizeX, 2.5*sizeY, 0., 2*sizeY, 10, 0., 10.);
	     fHM->Add(hh0.str().c_str(), pad);
	     fHM->Add(hh1.str().c_str(), pad1);
	  }else{
	     printf("Cluster Hist already exists: Module: %u, Cluster: %u", moduleId, clusterCounter);
	  }
	  TH2Poly *h0 = (TH2Poly*)fHM->H1(hh0.str());
	  TH2Poly *h1 = (TH2Poly*)fHM->H1(hh1.str());
	  h0->SetFillColor(colorVector[(iColorCounter % 6)]+(iShadeCounter % 10));
	  h1->SetFillColor(colorVector[(iColorCounter % 6)]+(iShadeCounter % 10));

	  for(const auto& idigi : cluster->GetDigis()){
	      CbmTrdDigi *newDigi = (CbmTrdDigi*)fTrdDigis->At(idigi);
	      Int_t digiAddress  = newDigi->GetAddress();
	      Double_t charge = newDigi->GetCharge();
	      Double_t digiTime = newDigi->GetTime();

	      Int_t moduleAddr = CbmTrdAddress::GetModuleAddress(digiAddress);
	      Int_t moduleId2 = CbmTrdAddress::GetModuleId(digiAddress);
	      Int_t sectorId = CbmTrdAddress::GetSectorId(digiAddress);
	      Int_t secRow = CbmTrdAddress::GetRowId(digiAddress);
	      Int_t secCol = CbmTrdAddress::GetColumnId(digiAddress);

	      TVector3 posHit;
	      TVector3 padSize;
	      fModuleInfo = fDigiPar->GetModule(moduleAddr);
	      fModuleInfo->GetPosition(moduleAddr, sectorId, secCol, secRow, posHit, padSize);

	      padSize *= 1/TMath::Sqrt(12.);
	      Double_t sizeX = fModuleInfo->GetSizeX();
	      Double_t sizeY = fModuleInfo->GetSizeY();
	      Double_t midX = fModuleInfo->GetX();
	      Double_t midY = fModuleInfo->GetY();
	      Double_t cornX = midX-sizeX;
	      Double_t cornY = midY-sizeY;
	      Double_t cornZ = midZ-sizeZ;
	      Double_t x = posHit.X() - cornX;
	      Double_t y = posHit.Y() - cornY;
	      Double_t z = posHit.Z() - cornZ;
	      Double_t padx = padSize.X();
	      Double_t pady = padSize.Y();
	      Double_t padz = padSize.Z();

	      h0->AddBin(x-padx, y-pady, x+padx, y+pady);
	      h1->AddBin(x-padx, y-pady, x+padx, y+pady);
	      h0->Fill(x, y, charge);
	      h1->Fill(x, y, digiTime);

	      TBox *b = new TBox(x-padx, y-pady, x+padx, y+pady);
	      b->SetFillColor(colorVector[(iColorCounter % 6)]+(iShadeCounter % 10));
	      stringstream tt;
	      tt << "Digi (" << x << ", " << y << ")" << endl <<
		    "Cluster (" << clusterCounter << ")";
	      b->SetToolTipText(tt.str().c_str());
//	      b->SetLineColor(kMagenta+2);
	      b->Draw();
	  } // end for idigi : cluster->GetDigis
	  iShadeCounter++;
	  if(iShadeCounter % 10 == 0)
	    iColorCounter++;
     } // end for cluster : *clusterVector
     box2Png << fOutputDir << "png/" << box2.str().c_str() << ".png";
     c0->Print(box2Png.str().c_str());*/
   } // end for v : markerMap

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


