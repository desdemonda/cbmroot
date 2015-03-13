/*
 * CbmClusteringSL.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kozlov
 */

#ifndef CBMCLUSTERINGSL_H_
#define CBMCLUSTERINGSL_H_

#include "FairTask.h"
#include "CbmClusteringGeometry.h"

class CbmClusteringSL
{
public:
	CbmClusteringSL();
	CbmClusteringSL(CbmClusteringGeometry* moduleGeo);
   virtual ~CbmClusteringSL();

   void SLRec1(CbmClusteringGeometry* moduleGeo, Int_t activePad);              //Recursive function for all neighbours
   void SLRec2(CbmClusteringGeometry* moduleGeo, Int_t activePad);              //Recursive function for vertical and horizontal neighbours
   void MainClusteringSL(CbmClusteringGeometry* moduleGeo, Int_t algVersion);   //Clustering function

   Int_t GetNofClusters()	const { return fNofClusters;}
   Int_t GetNofPads()		const { return fNofPads;}
   Int_t GetNofActivePads()	const { return fNofActivePads;}

   //Int_t GetCluster(Int_t iCluster);
   Float_t GetX0(Int_t iCluster);
   Float_t GetY0(Int_t iCluster);
   UInt_t GetClCharge(Int_t iCluster);
   Int_t GetNofPads(Int_t iCluster);
   Int_t GetPadInCluster(Int_t iCluster, Int_t iPad);

private:

   Int_t fNofPads;                  //Number of pads in module
   Int_t fNofActivePads;            //Number of charged pads in module

   UInt_t* fA1;                     //Primary array of charges
   Bool_t* fS;                      //Array of states
   Int_t* fNumbersOfPads;           //Array of relations Pad/Cluster

   Int_t fNofClusters;              //Number of found clusters in module
   struct Cluster{                  //Description of clusters
	   //Int_t fNofCluster;
	   Float_t fX;
	   Float_t fY;
	   UInt_t fCharge;
	   Int_t fNofPads;
	   vector<Int_t> fPadsInCluster;
   };
   Cluster* fClusters;

   CbmClusteringSL(const CbmClusteringSL&);
   CbmClusteringSL& operator=(const CbmClusteringSL&);
   
};

#endif /* CBMCLUSTERINGSL_H_ */
