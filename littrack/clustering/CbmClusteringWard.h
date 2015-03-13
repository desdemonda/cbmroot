/*
 * CbmClusteringWard.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGWARD_H_
#define CBMCLUSTERINGWARD_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"

//class CbmMuchGeoScheme;
//class CbmMuchCluster;
//class CbmMuchPad;
//class TClonesArray;

class CbmClusteringWard//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmClusteringWard();

	CbmClusteringWard(CbmClusteringGeometry* moduleGeo, Int_t blockSize);
   /**
    * \brief Destructor.
    */
   virtual ~CbmClusteringWard();

   Float_t WardDistance(Int_t iPad1, Int_t iPad2);
   void WardCreate(CbmClusteringGeometry* moduleGeo);
   void AddClusterInBlock(CbmClusteringGeometry* moduleGeo, Int_t newCluster, Int_t addedCluster);
   Bool_t WardBlockCreateStep(CbmClusteringGeometry* moduleGeo, Int_t wardStep, Bool_t wardStepRec, Int_t addedPad);
   void WardBlockCreate(CbmClusteringGeometry* moduleGeo);
   void DeleteCluaster(Int_t clusterNumber);
   void WardDistanceRecalculation(Int_t clusterNumber);
   Float_t XCenterRecalculation(Int_t cl1, Int_t cl2);
   Float_t YCenterRecalculation(Int_t cl1, Int_t cl2);
   void WardProcessingData(Float_t maxDistance);
   void GetClustersFromBlock();
   void WardMainFunction(CbmClusteringGeometry* moduleGeo, Float_t maxDistance);

   Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofAPads()	const { return fNofActivePads;}
   Int_t GetNofPads()	const { return fNofPads;}

   //void MainClusteringA1(CbmMuchGeoCl* moduleGeo, Int_t algVersion);

   /*Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofPads()		const { return fNofPads;}
   Int_t GetNofActivePads()	const { return fNofActivePads;}*/

   Int_t GetCluster(Int_t iCluster);
   Float_t GetX0(Int_t iCluster);
   Float_t GetY0(Int_t iCluster);
   UInt_t GetClCharge(Int_t iCluster);
   Int_t GetNofPads(Int_t iCluster);
   Int_t GetPadInCluster(Int_t iCluster, Int_t iPad);

private:

   Int_t BLOCK_SIZE;
   Int_t MAX_NEIGHBORS;

   Int_t fNofPads;
   Int_t fNofActivePads;

   Bool_t* fS; //g_MAX_PADS
   Int_t* fNumbersOfPads; //g_MAX_PADS

   CbmClusteringGeometry* fModuleGeo;

   Int_t fNofClusters;

   struct ClusterBlock
   	{
   		//Nomera sisedei klastera
   		Int_t neighbors[50];	//numbersOfNeighbors
   		//Koli4estvo sosedei klastera
   		Int_t nofNeighbors;	//maxNumberOfNeighbors
   		//Nomer klastera
   		Int_t nofCluster;	//numberOfCluster
   		//Obozna4enie klastera v massive nomerov klasterov
   		Int_t linkToDesignation;
   		//Amplituda klastera
   		Float_t clCharge;	//A
   		//Koordinati tsentra klastera
   		Float_t xc, yc;
   		//Rassto9ni9 mejdu klasterami
   		Float_t wardDistances[50];	//W
   		Bool_t S[50];
   		//Nomera 94eek v klastere
   //---!!!---
   		//int numbersOfPadsInCluster[g_ward_MAX_NEIGHBORS];
   		//Koli4estvo 94eek v klastere
   		Int_t nofPads;	//padsInCluster
   		Int_t padsInCluster[30];
   	};
   ClusterBlock* fClustersInBlock;

   //Massiv sootvetstvi9 94eek i klasterov
   	Int_t* padsInClusters;	//g_MAX_PADS
   	Int_t clustersInMethod;
   	Int_t clustersInMethod_2;
   	//Massiv neobrabotannih 94eek
   	Bool_t* wardActivePads;	//g_MAX_PADS
   	//Pervii element bloka obrabativaemih dannih
   	Int_t firstBlockElement;
   	//Koli4estvo ob'ektov v spiske obrabitki
   	Int_t listValue;

   struct Cluster{
	   Int_t nofCluster;
	   Float_t xc;
	   Float_t yc;
	   UInt_t sumClCharge;
	   Int_t nofPads;
	   Int_t padsInCluster[30];
   };
   Cluster* fClusters;

   CbmClusteringWard(const CbmClusteringWard&);
   CbmClusteringWard& operator=(const CbmClusteringWard&);
   
};

#endif /* CBMCLUSTERINGWARD_H_ */
