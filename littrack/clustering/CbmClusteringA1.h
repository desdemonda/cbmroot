/*
 * CbmClusteringA1.h
 *
 *  Created on: Apr 4, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGA1_H_
#define CBMCLUSTERINGA1_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"

class CbmClusteringA1
{
public:
	CbmClusteringA1();
	CbmClusteringA1(CbmClusteringGeometry* moduleGeo);
   virtual ~CbmClusteringA1();

   void MainClusteringA1(CbmClusteringGeometry* moduleGeo, Int_t algVersion);                   //Clustering function
   void ChangeClusters(CbmClusteringGeometry* moduleGeo, Int_t nPad, Int_t Cl0, Int_t Cl1, Int_t algVersion);

   Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofPads()		const { return fNofPads;}
   Int_t GetNofActivePads()	const { return fNofActivePads;}

   Int_t GetCluster(Int_t iCluster);
   Float_t GetX0(Int_t iCluster);
   Float_t GetY0(Int_t iCluster);
   UInt_t GetClCharge(Int_t iCluster);
   Int_t GetNofPads(Int_t iCluster);
   Int_t GetPadInCluster(Int_t iCluster, Int_t iPad);
   Int_t GetNPadInCluster(Int_t iCluster, Int_t iPad);
   UInt_t GetPadCharge(Int_t iCluster, Int_t iPad);
   vector<Int_t> GetPads(Int_t iCluster);

private:

   Int_t fNofPads;          //Number of pads in module
   Int_t fNofActivePads;    //Number of charged pads in module

   UInt_t* fA1;             //Primary array of charges
   UInt_t* fA2;             //Secondary array of charges
   Bool_t* fS;              //Array of states
   Int_t* fNumbersOfPads;   //Array of relations Pad/Cluster

   Int_t fNofClusters;      //Number of found clusters in module
   struct Cluster{          //Description of clusters
	   Int_t fNCluster;
	   Float_t fX;
	   Float_t fY;
	   UInt_t fCharge;
	   Int_t fNofPads;
	   vector<Int_t> fPadsInCluster;
	   vector<Int_t> fNPadsInCluster;
	   vector<Int_t> fPadsCharges;
   };
   Cluster* fClusters;

   CbmClusteringA1(const CbmClusteringA1&);
   CbmClusteringA1& operator=(const CbmClusteringA1&);
  
};

#endif
