/*
 * CbmClusteringWard.cxx
 *
 *  Created on: Apr 10, 2012
 *      Author: kozlov
 */

#include "CbmClusteringGeometry.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchPoint.h"
#include "CbmMCTrack.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchDigi.h"
#include "CbmClusteringWard.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringWard::CbmClusteringWard():
		clustersInMethod(),
		padsInClusters(),
		fS(),
		fClustersInBlock(),
		clustersInMethod_2(0),
		wardActivePads(),
		fClusters(),
		firstBlockElement(),
		fModuleGeo(),
		BLOCK_SIZE(),
		fNumbersOfPads(),
		MAX_NEIGHBORS(),
		listValue()
{
	fNofPads = 0;
	fNofActivePads = 0;
	fNofClusters = 0;
}

CbmClusteringWard::CbmClusteringWard(CbmClusteringGeometry* moduleGeo, Int_t blockSize):
		fModuleGeo()
{
	MAX_NEIGHBORS = 50;
	BLOCK_SIZE = blockSize;

	fClusters = new Cluster [5000];	//1000 - временное значение. поработать над этим

	fClustersInBlock = new ClusterBlock [BLOCK_SIZE];
	for(Int_t iCl = 0; iCl < BLOCK_SIZE; iCl++)
	{
		fClustersInBlock[iCl].nofNeighbors = 0;
		fClustersInBlock[iCl].nofPads = 0;
		fClustersInBlock[iCl].xc = 0;
		fClustersInBlock[iCl].yc = 0;
		fClustersInBlock[iCl].clCharge = 0;
		fClustersInBlock[iCl].nofCluster = 0;
		fClustersInBlock[iCl].linkToDesignation = 0;
		for(Int_t iPad = 0; iPad < MAX_NEIGHBORS; iPad++)
		{
			fClustersInBlock[iCl].neighbors[iPad] = 0;
			fClustersInBlock[iCl].wardDistances[iPad] = 0;
			fClustersInBlock[iCl].S[iPad] = 0;
//---!!!---<>
			//fClustersInBlock[iCl].numbersOfPadsInCluster[j] = 0;
		}
	}

	clustersInMethod = 0;
	clustersInMethod_2 = 0;
	firstBlockElement = 0;	//Novii blok na4inaets9 so sleduushego elementa
	listValue = 0;

	fNofPads = moduleGeo->GetNPads();
	fNofActivePads = moduleGeo->GetAPadsNom();
	fNofClusters = fNofActivePads;
	fS = new Bool_t [fNofPads];
	fNumbersOfPads = new Int_t [fNofPads];	//---|
	Int_t nom = 0;
	padsInClusters = new Int_t [fNofPads];	//---|
	wardActivePads = new Bool_t [fNofPads];

	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		padsInClusters[iPad] = iPad;	//g_ClustersArray[iPad] - ???
		if(moduleGeo->GetPadCharge(iPad) > 0)
		{
			wardActivePads[iPad] = true;
		}
		else
		{
			wardActivePads[iPad] = false;
		}
	}

	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		fS[iPad] = 0;
		fNumbersOfPads[iPad] = 0;
		if(moduleGeo->GetPadCharge(iPad) > 0)
		{
			fS[iPad] = 1;
			nom++;
			fNumbersOfPads[iPad] = nom;
		}
	}

	//-----------------------------------
	/*for(Int_t iPad = 0; iPad < BLOCK_SIZE; iPad++)
	{
		if(fClustersInBlock[iPad].clCharge != 0){
		std::cout<<"Cl: "<<fClustersInBlock[iPad].nofCluster<<"; Charge: "<<fClustersInBlock[iPad].clCharge<<"\n";
		std::cout<<"activePads: "<<wardActivePads[iPad]<<"; fS: "<<fS[iPad]<<"\n";}
	}*/
}

CbmClusteringWard::~CbmClusteringWard()
{
	/*delete fS;
	delete fNumbersOfPads;
	delete fClustersInBlock;
	delete fClusters;*/
}

Float_t CbmClusteringWard::WardDistance(Int_t iPad1, Int_t iPad2)
{
	return (((fClustersInBlock[iPad1].clCharge * fClustersInBlock[iPad2].clCharge) * ((fClustersInBlock[iPad1].xc - fClustersInBlock[iPad2].xc) *
			(fClustersInBlock[iPad1].xc - fClustersInBlock[iPad2].xc) + (fClustersInBlock[iPad1].yc - fClustersInBlock[iPad2].yc) *
			(fClustersInBlock[iPad1].yc - fClustersInBlock[iPad2].yc))) / (fClustersInBlock[iPad1].clCharge + fClustersInBlock[iPad2].clCharge));
}

void CbmClusteringWard::WardCreate(CbmClusteringGeometry* moduleGeo)
{
	Int_t clusterNumber = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)	//0 <> 1 -???
	{
		//Initsiatsi9 aktivnih 94eek
		if(moduleGeo->GetPadCharge(iPad) > 0)
		{
			fClustersInBlock[clusterNumber].nofNeighbors = moduleGeo->GetGoodNeighborsNum(iPad);
			fClustersInBlock[clusterNumber].nofPads = 1;
			fClustersInBlock[clusterNumber].padsInCluster[0] = iPad;
//---!!!---<
			//fClustersInBlock[clusterNumber].numbersOfPadsInCluster[1] = iPad;
			fClustersInBlock[clusterNumber].nofCluster = iPad;
			fClustersInBlock[clusterNumber].clCharge = moduleGeo->GetPadCharge(iPad);
			fClustersInBlock[clusterNumber].xc = moduleGeo->GetX0(iPad);
			fClustersInBlock[clusterNumber].yc = moduleGeo->GetY0(iPad);
			clusterNumber++;	//Перенес из начала цикла - проверить
		}
	}
	clustersInMethod = clusterNumber;
	//clustersInMethod_2 = clustersInMethod; //- !!! ПРОВЕРИТЬ, может оно совсем не нужно
	//Opredelenie sosedei dl9 vseh 94eek-klasterov
	//Vipoln9ets9 posle opredeleni9 vseh nomerov klasterov v klasse
	Int_t neighbor = 0;
	for(Int_t iPad = 0; iPad < clustersInMethod; iPad++)	//0 <> 1 - ???
	{
		neighbor = 0;
		for(Int_t jPad = 0; jPad < clustersInMethod; jPad++)	//0 <> 1 - ???
		{
			for(Int_t kPad = 0; kPad < fClustersInBlock[iPad].nofNeighbors; kPad++)	//0 <> 1 - ???
			{
				if(moduleGeo->GetNeighbor(fClustersInBlock[iPad].nofCluster, kPad) == fClustersInBlock[jPad].nofCluster)
				{
					fClustersInBlock[iPad].neighbors[neighbor] = jPad;
					neighbor++;
				}
			}
		}
		fClustersInBlock[iPad].nofNeighbors = neighbor;	//добавил
	}
	//Vi4islenie rassto9nii mejdu sosed9mi
	for(Int_t iCl = 0; iCl < clustersInMethod; iCl++)	//0 <> 1 - ???
	{
		for(Int_t jCl = 0; jCl < fClustersInBlock[iCl].nofNeighbors; jCl++)	//0 <> 1 - ???
		{
			fClustersInBlock[iCl].wardDistances[jCl] = WardDistance(iCl, fClustersInBlock[iCl].neighbors[jCl]);
			//Aktivizatsi9 rassto9niz mejdu sosed9mi
			fClustersInBlock[iCl].S[jCl] = 1;
		}
    }
}

//функции загрузки в блок недоделаны. есть проблема с nofNeighbors
void CbmClusteringWard::AddClusterInBlock(CbmClusteringGeometry* moduleGeo, Int_t newCluster, Int_t addedCluster)
{
	fClustersInBlock[newCluster].nofNeighbors = moduleGeo->GetGoodNeighborsNum(addedCluster);
	fClustersInBlock[newCluster].nofPads = 1;
	fClustersInBlock[newCluster].nofCluster = addedCluster;
	fClustersInBlock[newCluster].linkToDesignation = padsInClusters[addedCluster];
	fClustersInBlock[newCluster].clCharge = moduleGeo->GetPadCharge(addedCluster);
	fClustersInBlock[newCluster].xc = moduleGeo->GetX0(addedCluster);
	fClustersInBlock[newCluster].yc = moduleGeo->GetY0(addedCluster);
	fClustersInBlock[newCluster].nofNeighbors = 0;
	fClustersInBlock[newCluster].padsInCluster[0] = moduleGeo->GetDigiNum(addedCluster);
	//std::cout<<"AddClusterInBlock: "<<addedCluster<<" -> "<<newCluster<<"\n";
	//std::cout<<"nofCluster: "<<fClustersInBlock[newCluster].nofCluster<<"; ";
	//std::cout<<"clCharge: "<<fClustersInBlock[newCluster].clCharge<<"; ";
	//std::cout<<"linkToDesignation: "<<fClustersInBlock[newCluster].linkToDesignation<<"\n";
	//std::cout<<"xc = "<<fClustersInBlock[newCluster].xc<<"; yc = "<<fClustersInBlock[newCluster].yc<<"\n";
	//std::cout<<"nofPads: "<<fClustersInBlock[newCluster].nofPads<<"; ";
	//std::cout<<"nofNeighbors: "<<fClustersInBlock[newCluster].nofNeighbors<<"\n";
}

Bool_t CbmClusteringWard::WardBlockCreateStep(CbmClusteringGeometry* moduleGeo, Int_t wardStep, Bool_t wardStepRec, Int_t addedPad)
{
	//std::cout<<"BlockCreateStep started!\n";
	Int_t tempPad = addedPad;
	//wardActivePads[tempPad] = false;
	//Proverka previsheni9 limita obrabotki za 1 shag
	if(wardStep >= BLOCK_SIZE)
	{
		//Aktivasti9 94eiki esli dobavlenie v spisok obrabotki nevozmojno
		wardActivePads[tempPad] = true;
		return false;
	}
	if(wardStepRec == false)
	{
		//Aktivasti9 94eiki esli dobavlenie v spisok obrabotki nevozmojno
		wardActivePads[tempPad] = true;
		return false;
	}
	if(tempPad > fNofPads)
	{
		std::cout<<"Error with tempPad in WardBlockCreateStep\n";
		return false;
	}
	Int_t lastPadInRecursion = tempPad;
	//Deaktivatsi9 94eiki
	wardActivePads[tempPad] = false;
	wardStep++;
	if(moduleGeo->GetGoodNeighborsNum(addedPad) > 0)
	{
		for(Int_t iPad = 0; iPad < moduleGeo->GetGoodNeighborsNum(addedPad); iPad++)	//0 <> 1 -???
		{
			if(wardActivePads[moduleGeo->GetNeighbor(addedPad, iPad)] == true)
			{
				//std::cout<<"\nEst' sosed!\n";
				wardStepRec = WardBlockCreateStep(moduleGeo, wardStep, wardStepRec, moduleGeo->GetNeighbor(addedPad, iPad));
				if(wardStepRec == 0)
				{
					//Aktivasti9 94eiki esli dobavlenie v spisok obrabotki nevozmojno
					wardActivePads[tempPad] = true;
					return false;
				}
				/*listValue++;
				clustersInMethod = listValue;
				std::cout<<"ADD CLUSTER - 1\n";	//--------------------
				AddClusterInBlock(moduleGeo, wardStep, tempPad);
				//-???- переставил
				wardActivePads[tempPad] = false;
				lastPadInRecursion = 0;*/
			}
		}
	}
	if(lastPadInRecursion != 0)
	{
		listValue++;
		clustersInMethod = listValue;
		//std::cout<<"ADD CLUSTER - 2\n";	//--------------------
		AddClusterInBlock(moduleGeo, wardStep, lastPadInRecursion);
		std::cout<<"Add cluster "<<lastPadInRecursion<<" in cluster "<<wardStep<<"\n";
		//std::cout<<"Cluster added!\n";	//--------------------
		//wardActivePads[lastPadInRecursion] = false;	//---???---
	}
	//Yda4noe zavershenie rekursivnoi funktsii
	return wardStepRec;
}

void CbmClusteringWard::WardBlockCreate(CbmClusteringGeometry* moduleGeo)
{
	//Koeffitsient dostato4nosti razmera spiska obrabotki
	Float_t listLimit = 0.9;
	//Flag zaversheni9 postroeni9 spiska obrabotki
	Bool_t workListFinished = false;
	//Poslednii element spiska 94eek, vnesennii v spisok obrabotki ---!!!---
	//int wardStepFinish = 0;
	//Nomer dobavl9emoi 94eiki
	Int_t nomActivePad = 0;
	//Nomer poslednei dobavlennoi v spisok obrabotki 94eiki
	Int_t lastActivePad = firstBlockElement - 1;
	//Koli4estvo ob'ektov v spiske obrabotki
	Int_t wardStep = 0;
	Bool_t wardStepRec = true;
		//Int_t AAA = 0;
	do{
		do{
			lastActivePad++;
			if(wardActivePads[lastActivePad] == true)
			{
				nomActivePad = lastActivePad;
			}
		}while((nomActivePad == 0)&&(lastActivePad < fNofPads));
		wardStep = listValue;
		if((nomActivePad != 0)&&(lastActivePad < fNofPads))
		{
			//std::cout<<"---nomActivePad: "<<nomActivePad<<"\n";
			AddClusterInBlock(moduleGeo, wardStep, nomActivePad);	//??? - dobavil 21.12.12 - ???
			wardStepRec = WardBlockCreateStep(moduleGeo, wardStep, wardStepRec, nomActivePad);
			//std::cout<<"BlockCreateStep finished!\n";	//---------------
		}
		nomActivePad = 0;
		//---!!!---!!!---!!!---
		if((lastActivePad >= fNofPads)||(wardStep >= (int)(listLimit * BLOCK_SIZE)))	//???
		{
			workListFinished = true;
		}
		//std::cout<<AAA<<"\n";
		//AAA++;
	}while((!workListFinished)/*&&(AAA < 35)*/);
	//Sostavlenie spiska sosedei
	//std::cout<<"clustersInMethod: "<<clustersInMethod<<"\n";
	Int_t neighbor = 0;
	for(Int_t iCl = 0; iCl < clustersInMethod; iCl++)	//0 <> 1 - ???
	{
		neighbor = 0;
		//std::cout<<"Neighbors: "<<moduleGeo->GetGoodNeighborsNum(fClustersInBlock[iCl].nofCluster)<<"\n";
		for(Int_t jCl = 0; jCl < clustersInMethod; jCl++)	//0 <> 1 - ???
		{
			for(Int_t kCl = 0; kCl < moduleGeo->GetGoodNeighborsNum(fClustersInBlock[iCl].nofCluster)/*fClustersInBlock[iCl].nofNeighbors*/; kCl++)	//0 <> 1 - ???
			{
				//std::cout<<"Cl1: "<<moduleGeo->GetNeighbor(fClustersInBlock[iCl].nofCluster, kCl);
				//std::cout<<"; Cl2: "<<fClustersInBlock[jCl].nofCluster<<"\n";
				if(moduleGeo->GetNeighbor(fClustersInBlock[iCl].nofCluster, kCl) == fClustersInBlock[jCl].nofCluster)
				{
					fClustersInBlock[iCl].neighbors[neighbor] = jCl;
					//std::cout<<"Get Neighbor "<<jCl<<" to Pad "<<iCl<<"\n";
					neighbor++;
				}
			}
		}
		fClustersInBlock[iCl].nofNeighbors = neighbor;
	}
	//Vi4islenie rassto9nii mejdu sosed9mi
	for(Int_t iCl = 0; iCl < clustersInMethod; iCl++)	//0 <> 1 - ???
	{
		for(Int_t jCl = 0; jCl < fClustersInBlock[iCl].nofNeighbors; jCl++)	//0 <> 1 - ???
		{
			fClustersInBlock[iCl].wardDistances[jCl] = WardDistance(iCl, fClustersInBlock[iCl].neighbors[jCl]);
			//Aktivizatsi9 rassto9ni9 mejdu sosed9mi
			fClustersInBlock[iCl].S[jCl] = 1;
		}
	}
	Int_t TEMP = firstBlockElement + listValue;
	for(Int_t iPad = firstBlockElement; iPad < TEMP; iPad++)	//0 <> 1 - ???
	{
		if(padsInClusters[iPad] == 0)
		{
			listValue++;
		}
	}
	firstBlockElement += listValue;
	for(Int_t i = 0; i < fNofPads+1; i++)
	{
		std::cout<<"->Pad "<<i<<"; Cl: "<<fClustersInBlock[i].nofCluster<<"; sPads: "<<fClustersInBlock[i].nofPads<<"; nofNeighbors: "<<fClustersInBlock[i].nofNeighbors<<"\n";
	}
	//std::cout<<"! listValue: "<<listValue<<"\n";
}

void CbmClusteringWard::DeleteCluaster(Int_t clusterNumber)
{
	//Ydalenie klastera iz spiskov sosedei
	Bool_t clusterWasDeleted = false;
	for(Int_t iCl = 0; iCl < fClustersInBlock[clusterNumber].nofNeighbors; iCl++)	//0 <> 1 - ???
	{
		clusterWasDeleted = false;
		for(Int_t jCl = 0; jCl < fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].nofNeighbors; jCl++)	//0 <> 1 - ???
		{
			//Sjatie spiskov sosedei klastera s zamesheniem ydal9emogo klastera
			if(clusterWasDeleted == true)
			{
				//Podn9t' nomer soseda v spiske
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].neighbors[jCl - 1]
				 = fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].neighbors[jCl];
				//Opustit' NULL v spiske
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].neighbors[jCl] = 0;
				//Podn9t' rassto9nie v spiske
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].wardDistances[jCl - 1]
				 = fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].wardDistances[jCl];
				//Opustit' NULL v spiske
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].wardDistances[jCl] = 0;
				//Podn9t' flag aktivnosti rassto9ni9 v spiske
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].S[jCl - 1]
				 = fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].S[jCl];
				//Opustit' NULL v spiske
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].S[jCl] = 0;
               }
			//Dostignut udal9emii klaster v spiske sosedei
			if(fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].neighbors[jCl] == clusterNumber)
			{
				clusterWasDeleted = true;
			}
		}
		//Ymen'shenie koli4estva sosedei dlz sosedei ydal9emogo klastera	-???-
		fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].nofNeighbors--;
	}
	//O4istka spiska sosedei ydal9emogo klastera
	for(Int_t iCl = 0; iCl < fClustersInBlock[clusterNumber].nofNeighbors; iCl++)	//0 <> 1 - ???
	{
		fClustersInBlock[clusterNumber].neighbors[iCl] = 0;
		fClustersInBlock[clusterNumber].wardDistances[iCl] = 0;
		fClustersInBlock[clusterNumber].S[iCl] = 0;
    }
	//Ydalenie klastera
	fClustersInBlock[clusterNumber].nofPads = 0;
	fClustersInBlock[clusterNumber].nofNeighbors = 0;
	fClustersInBlock[clusterNumber].nofCluster = 0;
	fClustersInBlock[clusterNumber].linkToDesignation = 0;
	fClustersInBlock[clusterNumber].clCharge = 0;
	fClustersInBlock[clusterNumber].xc = 0;
	fClustersInBlock[clusterNumber].yc = 0;
	fNofClusters--;
	/*for(Int_t iCl = clusterNumber; iCl < (fNofClusters - 1); iCl++)
	{
		fClustersInBlock[iCl].nofPads = fClustersInBlock[iCl + 1].nofPads;
		fClustersInBlock[iCl].nofNeighbors = fClustersInBlock[iCl + 1].nofNeighbors;
		fClustersInBlock[iCl].nofCluster = fClustersInBlock[iCl+1].nofCluster;
	}*/
}

void CbmClusteringWard::WardDistanceRecalculation(Int_t clusterNumber)
{
	Float_t newDistance;
	//Ydalenie klastera iz sobstvennogo spiska sosedei
	for(Int_t iCl = 0; iCl < fClustersInBlock[clusterNumber].nofNeighbors; iCl++)	//0 <> 1 - ???
	{
		if(fClustersInBlock[clusterNumber].neighbors[iCl] == clusterNumber)
		{
			for(Int_t jCl = iCl; jCl < fClustersInBlock[clusterNumber].nofNeighbors; jCl++)	//???
			{
				fClustersInBlock[clusterNumber].neighbors[jCl] = fClustersInBlock[clusterNumber].neighbors[jCl+1];
				fClustersInBlock[clusterNumber].S[jCl] = fClustersInBlock[clusterNumber].S[jCl+1];
			}
			//Yni4tojenie poslednego soseda posle sdviga massiva
			fClustersInBlock[clusterNumber].neighbors[fClustersInBlock[clusterNumber].nofNeighbors - 1] = 0;	// - 1 -???
			fClustersInBlock[clusterNumber].S[fClustersInBlock[clusterNumber].nofNeighbors - 1] = 0;
			fClustersInBlock[clusterNumber].nofNeighbors--;
		}
	}
	//Ras4et novih rassto9nii dl9 vseh sosedei
	for(Int_t iCl = 0; iCl < fClustersInBlock[clusterNumber].nofNeighbors; iCl++)	//0 <> 1 - ???
	{
		newDistance = WardDistance(clusterNumber, fClustersInBlock[clusterNumber].neighbors[iCl]);
		fClustersInBlock[clusterNumber].wardDistances[iCl] = newDistance;
		//Ystanovka rassto9nii sosed9m
		for(Int_t jCl = 0; jCl < fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].nofNeighbors; jCl++)	//0 <> 1 - ???
		{
			if(fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].neighbors[jCl] == clusterNumber)
			{
				fClustersInBlock[fClustersInBlock[clusterNumber].neighbors[iCl]].wardDistances[jCl] = newDistance;
			}
		}
	}
}


Float_t CbmClusteringWard::XCenterRecalculation(Int_t cl1, Int_t cl2)
{
	return (fClustersInBlock[cl1].xc * fClustersInBlock[cl1].clCharge
			+ fClustersInBlock[cl2].xc * fClustersInBlock[cl2].clCharge)
			/ (fClustersInBlock[cl1].clCharge + fClustersInBlock[cl2].clCharge);
}

Float_t CbmClusteringWard::YCenterRecalculation(Int_t cl1, Int_t cl2)
{
	return (fClustersInBlock[cl1].yc * fClustersInBlock[cl1].clCharge
			+ fClustersInBlock[cl2].yc * fClustersInBlock[cl2].clCharge)
			/ (fClustersInBlock[cl1].clCharge + fClustersInBlock[cl2].clCharge);
}

void CbmClusteringWard::WardProcessingData(Float_t maxDistance)
{
	Float_t minimalDistance = maxDistance;
	Int_t cluster1 = 0, cluster2 = 0;
	do{
		minimalDistance = maxDistance + 0.0001;
		//Proverka nali4i9 ob'edin9emih klasterov
		if(clustersInMethod == 1)
		{
			break;
		}
		//Nahojdenie minimal'nogo Wardovskogo rassto9ni9
		cluster1 = -1;
		for(Int_t iCl = 0; iCl < fNofActivePads; iCl++)	//0 <> 1 - ???
		{
			//std::cout<<iCl<<" - Neighbours: "<<fClustersInBlock[iCl].nofNeighbors<<"\n";
			for(Int_t jCl = 0; jCl < fClustersInBlock[iCl].nofNeighbors; jCl++)	//0 <> 1 - ???
			{
				/* Inogda popadauts9 W == 0
				 * Nedo4et v rabote metodov udaleni9 i peres4eta
				 * Ne vli9et na itogovii rezultat
				 */
				if((fClustersInBlock[iCl].wardDistances[jCl] < minimalDistance) && (fClustersInBlock[iCl].wardDistances[jCl] > 0))
				{
					minimalDistance = fClustersInBlock[iCl].wardDistances[jCl];
					cluster1 = iCl;
					cluster2 = fClustersInBlock[iCl].neighbors[jCl];
				}
			}
			//std::cout<<" - c1: "<<cluster1<<"; c2: "<<cluster2<<"\n";
		}
		if(cluster1 == -1)break;	//если нет кластеров для объединения. странная строчка
		//Prekreshenie raboti po predel'nomu rassto9niu
		if(minimalDistance >= maxDistance)break;
		//Vibor glavnogo klastera pri ob'edinenii
		if(cluster1 > cluster2)
		{
			Int_t k = cluster1;
			cluster1 = cluster2;
			cluster2 = k;
		}
//		std::cout<<">>>Connect Cl "<<cluster2<<" to Cl "<<cluster1<<"\n";
		//Sbros aktivnosti prisoedin9emogo klastera,
		//Esli oni dubliryut sv9zi glavnogo klastera
		for(Int_t iCl = 0; iCl < fClustersInBlock[cluster1].nofNeighbors; iCl++)	//0 <> 1 - ???
		{
			for(Int_t jCl = 0; jCl < fClustersInBlock[cluster2].nofNeighbors; jCl++)	//0 <> 1 - ???
			{
				if(fClustersInBlock[cluster1].neighbors[iCl] == fClustersInBlock[cluster2].neighbors[jCl])
				{
					fClustersInBlock[cluster2].S[jCl] = 0;
				}
			}
		}
		//Pereda4a sosedei s aktivnimi sv9z9mi glavnomy klasteru
		for(Int_t iCl = 0; iCl < fClustersInBlock[cluster2].nofNeighbors; iCl++)	//0 <> 1 - ???
		{
			if(fClustersInBlock[cluster2].S[iCl] == 1)
			{
				fClustersInBlock[cluster1].neighbors[fClustersInBlock[cluster1].nofNeighbors] = fClustersInBlock[cluster2].neighbors[iCl];
				fClustersInBlock[cluster1].S[fClustersInBlock[cluster1].nofNeighbors] = 1;
				fClustersInBlock[cluster1].nofNeighbors++;
			}
		}
		//Pereda4a 94eek glavnomu klasteru
		for(Int_t iCl = 0; iCl < fClustersInBlock[cluster2].nofPads; iCl++)	//0 <> 1 - ???
		{
			fClustersInBlock[cluster1].padsInCluster[fClustersInBlock[cluster1].nofPads] = fClustersInBlock[cluster2].padsInCluster[iCl];
			fClustersInBlock[cluster1].nofPads++;
//---!!!---<>
			//fClustersInBlock[cluster1].numbersOfPadsInCluster[fClustersInBlock[cluster1].nofPads] = ClustersInBlock[cluster2].numbersOfPadsInCluster[i];
		}
		for(Int_t iPad = 0; iPad < fNofPads; iPad++)	//0 <> 1 - ???	//g_max_pads <> fNofPads - ???
		{
			if(padsInClusters[iPad] == fClustersInBlock[cluster2].linkToDesignation)//!!!
			{
				padsInClusters[iPad] = fClustersInBlock[cluster1].linkToDesignation;//!!!
			}
		}
		//Vi4islenie stentra novogo klastera
		fClustersInBlock[cluster1].xc = XCenterRecalculation(cluster1, cluster2);
		fClustersInBlock[cluster1].yc = YCenterRecalculation(cluster1, cluster2);
		//Zamena prisoedin9emogo klastera na novii v spiskah sosedei drugih klasterov
		//Bool_t thereIsNewCluaster;
		for(Int_t iCl = 0; iCl < fClustersInBlock[cluster2].nofNeighbors; iCl++)	//0 <> 1 - ???
		{
			Bool_t thereIsNewCluaster = false;
			for(Int_t j = 0; j < fClustersInBlock[fClustersInBlock[cluster2].neighbors[iCl]].nofNeighbors; j++)	//0 <> 1 - ???
			{
				//Esli v spiskah sosedei yje est' novii klaster
				if(fClustersInBlock[fClustersInBlock[cluster2].neighbors[iCl]].neighbors[j] == cluster1)
				{
					thereIsNewCluaster = true;
				}
			}
			//Dobavlenie novogo klastera v spiski sosedei sosedei prisoedin9emogo klastera
			if(thereIsNewCluaster == false)
			{
				fClustersInBlock[fClustersInBlock[cluster2].neighbors[iCl]].neighbors[fClustersInBlock[fClustersInBlock[cluster2].neighbors[iCl]].nofNeighbors] = cluster1;
				fClustersInBlock[fClustersInBlock[cluster2].neighbors[iCl]].nofNeighbors++;
			}
		}
		//Vi4islenie amplitudi novogo klastera
		fClustersInBlock[cluster1].clCharge = fClustersInBlock[cluster1].clCharge+fClustersInBlock[cluster2].clCharge;
		//Ydalenie prisoedinennogo klastera
		DeleteCluaster(cluster2);
		//Peres4et rassto9nii dl9 novogo klastera
		WardDistanceRecalculation(cluster1);
		clustersInMethod--;
	}while(minimalDistance < maxDistance);
	//Yplotnenie massiva klasterov
	//Int_t clustersInMethodTEMP = clustersInMethod;
	Int_t d = 0;
	for(Int_t iCl = 0; iCl < fNofActivePads; iCl++)	//0 <> 1 - ??? - = ne ydal9l
	{
		d = 0;
		if(fClustersInBlock[iCl].clCharge == 0)
		{
			for(Int_t i = iCl; i < fNofActivePads; i++)
			{
				if(fClustersInBlock[i].clCharge == 0)
				{
					d++;
				}
				else break;
			}
			for(Int_t jCl = iCl; jCl < fNofActivePads; jCl++)
			{
				fClustersInBlock[jCl].clCharge = fClustersInBlock[jCl + d].clCharge;
				fClustersInBlock[jCl].nofPads = fClustersInBlock[jCl + d].nofPads;
				fClustersInBlock[jCl].nofCluster = fClustersInBlock[jCl + d].nofCluster;
				fClustersInBlock[jCl].nofNeighbors = fClustersInBlock[jCl + d].nofNeighbors;
				fClustersInBlock[jCl].xc = fClustersInBlock[jCl + d].xc;
				fClustersInBlock[jCl].yc = fClustersInBlock[jCl + d].yc;
				//Perenos nomerov sosedei
				for(Int_t kCl = 0; kCl < fClustersInBlock[jCl + d].nofNeighbors; kCl++)	//0 <> 1 - ???
				{
					fClustersInBlock[jCl].neighbors[kCl] = fClustersInBlock[jCl + d].neighbors[kCl];
					fClustersInBlock[jCl].wardDistances[kCl] = fClustersInBlock[jCl + d].wardDistances[kCl];
				}
				//Perenos nomerov 94eek v klastere
				for(Int_t iPad = 0; iPad < fClustersInBlock[jCl].nofPads; iPad++)
				{
					fClustersInBlock[jCl].padsInCluster[iPad] = fClustersInBlock[jCl + d].padsInCluster[iPad];
				}
//---!!!---
					/*for(int k = 1; k <= ClustersInBlock[j + 1].padsInCluster; k++)
				{
					ClustersInBlock[j].numbersOfPadsInCluster[k] = ClustersInBlock[j+1].numbersOfPadsInCluster[k];
				}*/
			}
		}
	}
	//clustersInMethod_2 += clustersInMethod;
	/* Dopolnitel'noe obnulenie udalennih elementov
	 * massiva klasterov ne vipoln9los', poskol'ky
	 * oni doljni bit' obnuleni pri udalenii
	 * prisoedin9emih klasterov
	 */
}

void CbmClusteringWard::GetClustersFromBlock()
{
//	std::cout<<"-===-clInMethod: "<<clustersInMethod<<"\n";
   Int_t nc = fNofClusters;
	for(Int_t iCl = 0; iCl < /*clustersInMethod*/fNofClusters; iCl++)	//0 <> 1 - ???
	{
	   if(fClustersInBlock[iCl].nofPads == 0){
	      nc--;
	      continue;
	   }
		fClusters[clustersInMethod_2 + iCl].sumClCharge = fClustersInBlock[iCl].clCharge;
		//fClusters[clustersInMethod_2 + iCl].linkToDesignation = fClustersInBlock[iCl].linkToDesignation;	//linkToDesignation - ???
		fClusters[clustersInMethod_2 + iCl].nofCluster = fClustersInBlock[iCl].nofCluster;
//		std::cout<<"-=-nofPads: "<<fClustersInBlock[iCl].nofPads<<"\n";
		fClusters[clustersInMethod_2 + iCl].nofPads = fClustersInBlock[iCl].nofPads;
		fClusters[clustersInMethod_2 + iCl].xc = fClustersInBlock[iCl].xc;
		fClusters[clustersInMethod_2 + iCl].yc = fClustersInBlock[iCl].yc;
		for(Int_t i = 0; i < fClusters[clustersInMethod_2 + iCl].nofPads; i++)
		{
			fClusters[clustersInMethod_2 + iCl].padsInCluster[i] = fClustersInBlock[iCl].padsInCluster[i];
		}
		//WARNING net zapolneni9 massiva sosedei - ?!
	}
	fNofClusters = nc;
	clustersInMethod_2 += clustersInMethod;
//	std::cout<<"-===-clInMethod_2: "<<clustersInMethod_2<<"\n";
	//std::cout<<"clustersInMethod: "<<clustersInMethod<<"\n";
}

void CbmClusteringWard::WardMainFunction(CbmClusteringGeometry* moduleGeo, Float_t maxDistance)
{
	WardCreate(moduleGeo);
	std::cout<<"---Ward block created\n";
	//WardBlockCreate(moduleGeo);
	WardProcessingData(maxDistance);
	std::cout<<"---Ward data processed\n";
	GetClustersFromBlock();
	std::cout<<"---Ward clustering finished\n";
	/*do{
		Int_t TEMP = firstBlockElement;
		WardBlockCreate(moduleGeo);
		if(TEMP == firstBlockElement)
		{
			cout<<"Error!";
			break;
		}
		WardProcessingData(maxDistance);
		GetClustersFromBlock();
	}while(firstBlockElement < fNofPads);*/
}

Int_t CbmClusteringWard::GetCluster(Int_t iCluster)
{
	return fClusters[iCluster].nofCluster;
}
Float_t CbmClusteringWard::GetX0(Int_t iCluster)
{
	return fClusters[iCluster].xc;
}
Float_t CbmClusteringWard::GetY0(Int_t iCluster)
{
	return fClusters[iCluster].yc;
}
UInt_t CbmClusteringWard::GetClCharge(Int_t iCluster)
{
	return fClusters[iCluster].sumClCharge;
}
Int_t CbmClusteringWard::GetNofPads(Int_t iCluster)
{
	return fClusters[iCluster].nofPads;
}
Int_t CbmClusteringWard::GetPadInCluster(Int_t iCluster, Int_t iPad)
{
	//std::cout<<"Cl: "<<iCluster;
	//std::cout<<"\nPad: "<<iPad;
	//std::cout<<"\nnPad: "<<fClusters[iCluster].padsInCluster[iPad]<<"\n";
	if(fClusters[iCluster].padsInCluster[iPad] > 10000)return 0;
	if(fClusters[iCluster].padsInCluster[iPad] < 0)return 0;
	return fClusters[iCluster].padsInCluster[iPad];
}
