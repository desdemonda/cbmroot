/*
 * CbmMuchClustering.cxx
 *
 *  Created on: Mar 5, 2012
 *      Author: kozlov
 */

#include "CbmMuchClustering.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchPoint.h"
#include "CbmMCTrack.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchDigi.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmClusteringGeometry.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleGemRadial.h"
#include "CbmClusteringA1.h"
#include "CbmClusteringSL.h"
#include "CbmClusteringWard.h"
#include "CbmMuchStation.h"
#include "CbmMuchCluster.h"
#include "CbmMuchStrawHit.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchAddress.h"
#include "TStopwatch.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMath.h"

#include "draw/CbmDrawHist.h"
#include "TCanvas.h"
#include "TH1F.h"

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

CbmMuchClustering::CbmMuchClustering(const char* digiFileName):
		FairTask(),
		fModulesGeometryArray(),
		fDigiFile(digiFileName),
		fScheme(CbmMuchGeoScheme::Instance()),
		fCluster(),
		fMuchDigi(),
		fHit(),
		fNofClusters()
{
	fAlgorithmVersion = 2;
	fNofModules = 0;
	fNofEvents = 0;
}

CbmMuchClustering::~CbmMuchClustering()
{
	if (fHit)
	{
		fHit->Delete();
		delete fHit;
	}
	if (fCluster)
	{
		fCluster->Delete();
		delete fCluster;
	}
}

InitStatus CbmMuchClustering::Init()
{
   std::cout << "CbmMuchClustering::Init" << std::endl;

   // Initialize GeoScheme
   TFile* oldfile=gFile;
   TFile* file=new TFile(fDigiFile);
   TObjArray* stations = (TObjArray*) file->Get("stations");
   file->Close();
   file->Delete();
   gFile=oldfile;
   fScheme->Init(stations);
   // Initialize arrays of objects
   ReadDataBranches();
   // Create special geometry for every module
   CreateModulesGeometryArray();

   return kSUCCESS;
}

void CbmMuchClustering::Exec(Option_t* opt)
{
	//static Int_t eventNo = 0;
   TStopwatch timer;
   timer.Start();
	//std::cout << "CbmMuchClustering::Exec: event No=" << fNofEvents << std::endl;
	fNofEvents++;
	if( fMuchDigi->GetEntriesFast() == 0 ){
		cout << "-I- " << GetName() << "::Exec: No digis found, event skipped. " << endl;
		fNofEvents--;
		return;
	}
	fNofClusters = 0;
	// Clear output array
	if (fHit) fHit->Clear();
	if (fCluster) fCluster->Delete();
	// Initialize digi charges for special geometry in one event
	SetDigiCharges();
	// Find clusters and hits
	ClusteringMainFunction();
	// Clear digi charges array
	ClearDigiCharges();
	timer.Stop();
	std::cout<<"CbmMuchClustering: time "<<timer.RealTime()<<"; clusters:   "<<fNofClusters<<"\n";
}

void CbmMuchClustering::Finish()
{
   std::cout << "CbmMuchClustering::Finish" << std::endl;
}

void CbmMuchClustering::ReadDataBranches()
{
	FairRootManager* ioman = FairRootManager::Instance();
	assert(ioman != NULL);
	fMuchDigi = (TClonesArray*) ioman->GetObject("MuchDigi");
	fCluster = new TClonesArray("CbmMuchCluster", 1000);
	ioman->Register("MuchCluster", "Cluster in MUCH", fCluster, kTRUE);
	fHit = new TClonesArray("CbmMuchPixelHit", 1000);
	ioman->Register("MuchPixelHit", "Hit in MUCH", fHit, kTRUE);
}

void CbmMuchClustering::CreateModulesGeometryArray()
{
	fNofModules = 0;
	fModulesGeometryArray.clear();
	Int_t iModule = 0;
	Int_t nModules = 0;
	// Create special geometry for all modules
	for(Int_t iSt = 0; iSt < fScheme->GetNStations(); iSt++)
	{
		CbmMuchStation* station = (CbmMuchStation*)fScheme->GetStation(iSt);
		Int_t nLayers = station->GetNLayers();
		for(Int_t iL = 0; iL < nLayers; iL++)
		{
			CbmMuchLayer* layer = (CbmMuchLayer*)station->GetLayer(iL);
			CbmMuchLayerSide* lside = (CbmMuchLayerSide*)layer->GetSideF();
			nModules = lside->GetNModules();
			fNofModules += nModules;
			for(Int_t iMod = 0; iMod < nModules; iMod++)
			{
			   // Add special module geometry in array
			   fModulesGeometryArray.push_back(new CbmClusteringGeometry(iSt, iL, 0, iMod, fScheme));
			   // Add module index in map
			   fModulesByDetId[fModulesGeometryArray[iModule]->GetDetId()] = iModule;
			   iModule++;
			}
			lside = (CbmMuchLayerSide*)layer->GetSideB();
			nModules = lside->GetNModules();
			fNofModules += nModules;
			for(Int_t iMod = 0; iMod < nModules; iMod++)
			{
			   // Add special module geometry in array
			   fModulesGeometryArray.push_back(new CbmClusteringGeometry(iSt, iL, 1, iMod, fScheme));
			   // Add module index in map
			   fModulesByDetId[fModulesGeometryArray[iModule]->GetDetId()] = iModule;
			   iModule++;
			}
			std::cout<<"Layer "<<iL<<" geometry created successful.\n";
		}
		std::cout<<"Station "<<iSt<<" geometry created successful.\n";
	}
}

void CbmMuchClustering::SetDigiCharges()
{
	for(Int_t iDigi = 0; iDigi < fMuchDigi->GetEntriesFast(); iDigi++)
	{
		const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
		Long64_t chId = muchDigi->GetAddress();
		Int_t detID = CbmMuchAddress::GetElementAddress(chId, kMuchModule);
		Int_t iPad = fModulesGeometryArray[fModulesByDetId[detID]]->GetPadByChannelId(chId);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetPadCharge(iPad, muchDigi->GetAdc());
		fModulesGeometryArray[fModulesByDetId[detID]]->SetDigiNum(iPad, iDigi);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetAPadsPlusOne();
	}
}

void CbmMuchClustering::ClearDigiCharges()
{
	for(Int_t iDigi = 0; iDigi < fMuchDigi->GetEntriesFast(); iDigi++)
	{
		const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
		Long64_t chId = muchDigi->GetChannelId();
		Int_t detID = CbmMuchAddress::GetElementAddress(chId, kMuchModule);
		Int_t iPad = fModulesGeometryArray[fModulesByDetId[detID]]->GetPadByChannelId(chId);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetPadCharge(iPad, 0);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetDigiNum(iPad, 0);
		fModulesGeometryArray[fModulesByDetId[detID]]->SetAPadsNom(0);
	}
}

void CbmMuchClustering::ClusteringMainFunction()
{
	for(Int_t iMod = 0; iMod < fNofModules; iMod++)
	{
		CbmMuchModuleGem* module = (CbmMuchModuleGem*)fScheme->GetModuleByDetId(
				fModulesGeometryArray[iMod]->GetDetId());
		// Switching clustering algorithm by function SetAlgorithmVersion
		switch(fAlgorithmVersion){
		case 1:
		{// 1 - Developed algorithm, using all neighbors
			ClusteringA1(fModulesGeometryArray[iMod], module, 1);
			break;
		}
		case 2:
		{// 2 - Developed algorithm, do not using diagonal neighbors
			ClusteringA1(fModulesGeometryArray[iMod], module, 2);
			break;
		}
		case 3:
		{// 3 - Simple Single Linkage method, using all neighbors
			ClusteringSL(fModulesGeometryArray[iMod], module, 1);
			break;
		}
		case 4:
		{// 4 - Simple Single Linkage method, do not using diagonal neighbors;
			ClusteringSL(fModulesGeometryArray[iMod], module, 2);
			break;
		}
		case 5:
		{// 5 - Ward's method (!) not tested
			//ClusteringWard(fModulesGeometryArray[iMod], module);
			std::cout<<"CbmMuchClustering: Error! Algorithm not tested\n";
			break;
		}
		default: std::cout<<"CbmMuchClustering: Error! Wrong version of the algorithm.\n"; break;
		}
	}
}

void CbmMuchClustering::ClusteringA1(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver)
{
   // Create developed algorithm class for module m1
   CbmClusteringA1* clustersA1 = new CbmClusteringA1(m1);
   // Start algorithm
   clustersA1->MainClusteringA1(m1, Ver);
   // Increase number of clusters
   fNofClusters += clustersA1->GetNofClusters();
   // Write Clusters and Hits in output arrays
   vector<Int_t> digiIndices;
	for(Int_t iCl = 0; iCl < clustersA1->GetNofClusters(); iCl++)
	{
		digiIndices.clear();
		Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
		Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
		Double_t tmin = -1;
		x = clustersA1->GetX0(iCl);
		y = clustersA1->GetY0(iCl);
		z = m2->GetPosition()[2];
		CbmMuchPad* pad = NULL;
		Int_t address = 0;
		Int_t planeId = 0;
		for(Int_t iPad = 0; iPad < clustersA1->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = clustersA1->GetPadInCluster(iCl, iPad);
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
			if(iPad == 0){
			   address = CbmMuchAddress::GetElementAddress(muchDigi->GetAddress(),kMuchModule);
			   planeId = fScheme->GetLayerSideNr(address);
			}
			digiIndices.push_back(iDigi);
			Long64_t channelId = muchDigi->GetChannelId();
			pad = m2->GetPad(channelId);
			t   = muchDigi->GetTime();
			if(tmin < 0) tmin = t;
			if(tmin < t) tmin = t;
			q   = muchDigi->GetAdc();
			dx  = pad->GetDx();
			dy  = pad->GetDy();
			dxy = pad->GetDxy();
			dt = muchDigi->GetDTime();
			sumq    += q;
			sumt    += q*t;
			sumdx2  += q*q*dx*dx;
			sumdy2  += q*q*dy*dy;
			sumdxy2 += q*q*dxy*dxy;
			sumdt2  += q*q*dt*dt;
		}
		t   = tmin;
		dx  = sqrt(sumdx2/12)/sumq;
		dy  = sqrt(sumdy2/12)/sumq;
		dxy = sqrt(sumdxy2/12)/sumq;
		dt = sqrt(sumdt2)/sumq;
		Int_t nCluster = fCluster->GetEntriesFast();
		CbmMuchCluster* cluster = new ((*fCluster)[nCluster]) CbmMuchCluster();
		cluster->AddDigis(digiIndices);
		Int_t nHit = fHit->GetEntriesFast();
		new ((*fHit)[nHit]) CbmMuchPixelHit(address, x, y, z, dx, dy, 0, dxy, nCluster, planeId, t, dt);
	}
	delete clustersA1;
}

void CbmMuchClustering::ClusteringSL(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2, Int_t Ver)
{
   // Create SL algorithm class for module m1
   CbmClusteringSL* clustersSL = new CbmClusteringSL(m1);
   // Start algorithm
   clustersSL->MainClusteringSL(m1, Ver);
   // Increase number of clusters
   fNofClusters += clustersSL->GetNofClusters();
   // Write Clusters and Hits in output arrays
   vector<Int_t> digiIndices;
	for(Int_t iCl = 0; iCl < clustersSL->GetNofClusters(); iCl++)
	{
		digiIndices.clear();
		Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
		Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
		Double_t tmin = -1;
		x = clustersSL->GetX0(iCl);
		y = clustersSL->GetY0(iCl);
		z = m2->GetPosition()[2];
		CbmMuchPad* pad = NULL;
		Int_t address = 0;
		Int_t planeId = 0;
		for(Int_t iPad = 0; iPad < clustersSL->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = clustersSL->GetPadInCluster(iCl, iPad);
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
			if(iPad == 0){
               address = CbmMuchAddress::GetElementAddress(muchDigi->GetAddress(),kMuchModule);
               planeId = fScheme->GetLayerSideNr(address);
            }
			digiIndices.push_back(iDigi);
			Long64_t channelId = muchDigi->GetChannelId();
			pad = m2->GetPad(channelId);
			t   = muchDigi->GetTime();
			if(tmin < 0) tmin = t;
            if(tmin < t) tmin = t;
			q   = muchDigi->GetAdc();
			dx  = pad->GetDx();
			dy  = pad->GetDy();
			dxy = pad->GetDxy();
			dt = muchDigi->GetDTime();
			sumq    += q;
			sumt    += q*t;
			sumdx2  += q*q*dx*dx;
			sumdy2  += q*q*dy*dy;
			sumdxy2 += q*q*dxy*dxy;
			sumdt2  += q*q*dt*dt;
		}
		t   = tmin;
		dx  = sqrt(sumdx2/12)/sumq;
		dy  = sqrt(sumdy2/12)/sumq;
		dxy = sqrt(sumdxy2/12)/sumq;
		dt = sqrt(sumdt2)/sumq;
		Int_t nCluster = fCluster->GetEntriesFast();
      CbmMuchCluster* cluster = new ((*fCluster)[nCluster]) CbmMuchCluster();
      cluster->AddDigis(digiIndices);
		Int_t nHit = fHit->GetEntriesFast();
		new ((*fHit)[nHit]) CbmMuchPixelHit(address, x, y, z, dx, dy, 0, dxy, nCluster, planeId, t, dt);
	}
	delete clustersSL;
}

void CbmMuchClustering::ClusteringWard(CbmClusteringGeometry* m1, CbmMuchModuleGem* m2)
{
   CbmClusteringWard* clustersWard = new CbmClusteringWard(m1, 1000);
	clustersWard->WardMainFunction(m1, 100000);
	fNofClusters += clustersWard->GetNofClusters();
	for(Int_t iCl = 0; iCl < clustersWard->GetNofClusters(); iCl++)
	{
//	   std::cout<<">Start "<<iCl<<"\n";
		vector<Int_t> digiIndices;
		//---
		//std::cout<<"=>Cluster "<<iCl<<"\n";
		Int_t detId = m2->GetDetectorId();
		Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
		Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
//		std::cout<<">Values created\n";
		x = clustersWard->GetX0(iCl);
		y = clustersWard->GetY0(iCl);
		z = m2->GetPosition()[2];
//		std::cout<<">Center - ok; x: "<<x<<"; y: "<<y<<"; z: "<<z<<"\n";
		CbmMuchPad* pad = NULL;
//		std::cout<<">Start calculations... \n";
		for(Int_t iPad = 0; iPad < clustersWard->GetNofPads(iCl); iPad++)
		{
			Int_t iDigi = clustersWard->GetPadInCluster(iCl, iPad);
//			std::cout<<">>iDigi: "<<iDigi;
			const CbmMuchDigi* muchDigi = static_cast<const CbmMuchDigi*>(fMuchDigi->At(iDigi));
//			std::cout<<" - ok";
			digiIndices.push_back(iDigi);
//			std::cout<<" - ok\n";
			Long64_t channelId = muchDigi->GetChannelId();
//			std::cout<<">>channelId: "<<channelId<<"\n";
			pad = m2->GetPad(channelId);
//			std::cout<<">>Pad - ok\n";
			t   = muchDigi->GetTime();
			q   = muchDigi->GetADCCharge();
//			std::cout<<">>t, q - ok\n";
			dx  = 1;//pad->GetDx();
//			std::cout<<">>dx - ok\n";
			dy  = 1;//pad->GetDy();
//			std::cout<<">>dy - ok\n";
			dxy = pad->GetDxy();
//			std::cout<<">>dxy - ok\n";
			dt = muchDigi->GetDTime();
//			std::cout<<">>dt - ok\n";
			sumq    += q;
			sumt    += q*t;
			sumdx2  += q*q*dx*dx;
			sumdy2  += q*q*dy*dy;
			sumdxy2 += q*q*dxy*dxy;
			sumdt2  += q*q*dt*dt;
//			std::cout<<">>sum - ok\n";
		}
//		std::cout<<" ...finished\n";
		t   = sumt/sumq;
		dx  = sqrt(sumdx2/12)/sumq;
		dy  = sqrt(sumdy2/12)/sumq;
		dxy = sqrt(sumdxy2/12)/sumq;
		dt = sqrt(sumdt2)/sumq;
//		std::cout<<"-----Data calculated for Cl "<<iCl<<"\n";
		Int_t nCluster = fCluster->GetEntriesFast();
//		std::cout<<"-------Add cluster "<<nCluster<<"; nDigis: "<<digiIndices.size();
		CbmMuchCluster* cluster = new ((*fCluster)[nCluster]) CbmMuchCluster();
		cluster->AddDigis(digiIndices);
//		std::cout<<" - ok\n";
		//---
		Int_t planeId = fScheme->GetLayerSideNr(detId);
		Int_t nHit = fHit->GetEntriesFast();
		std::cout<<"\nCluster: "<<nHit<<"; detId: "<<detId<<"; NofPads: "<<clustersWard->GetNofPads(iCl)<<"\n";
		std::cout<<"x: "<<x<<"; y: "<<y<<"; z: "<<z<<"\n";
		std::cout<<"dx: "<<dx<<"; dy: "<<dy<<"; dxy: "<<dxy<<"\n";
		std::cout<<"planeId: "<<planeId<<"; t: "<<t<<"; dt: "<<dt<<"\n";
		std::cout<<"-------\n";
//		std::cout<<"-------Add Hit "<<nHit;
		new ((*fHit)[nHit]) CbmMuchPixelHit(detId, x, y, z, dx, dy, 0, dxy, nCluster, planeId, t, dt);
//		std::cout<<" - ok\n";
	}
	delete clustersWard;
}
ClassImp(CbmMuchClustering);
