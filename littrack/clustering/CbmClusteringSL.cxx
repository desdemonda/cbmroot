/*
 * CbmClusteringSL.cxx
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
#include "CbmClusteringSL.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringSL::CbmClusteringSL():
		fS(NULL),
		fNumbersOfPads(NULL),
		fA1(NULL),
		fClusters(NULL)
{
	fNofPads = 0;
	fNofActivePads = 0;
	fNofClusters = 0;
}

CbmClusteringSL::CbmClusteringSL(CbmClusteringGeometry* moduleGeo):
      fS(NULL),
      fNumbersOfPads(NULL),
      fA1(NULL),
      fClusters(NULL)
{
   //Initialization
	fNofPads = moduleGeo->GetNPads();
	fNofActivePads = moduleGeo->GetAPadsNom();
	fNofClusters = fNofActivePads;
	fA1 = new UInt_t [fNofPads];
	fS = new Bool_t [fNofPads];
	fClusters = new Cluster [fNofActivePads];
	fNumbersOfPads = new Int_t [fNofPads];
	Int_t nom = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		fA1[iPad] = moduleGeo->GetPadCharge(iPad);  //Filling primary array of charges
		fS[iPad] = 0;
		fNumbersOfPads[iPad] = 0;
		if(fA1[iPad] > 0)
		{
			fS[iPad] = 1;                           //Filling array of states
			nom++;
			fNumbersOfPads[iPad] = nom;             //Filling array of relations Pad/Cluster
		}
	}
}

CbmClusteringSL::~CbmClusteringSL()
{
	delete [] fA1;
	delete [] fS;
	delete [] fNumbersOfPads;
	delete [] fClusters;
}

void CbmClusteringSL::SLRec1(CbmClusteringGeometry* moduleGeo, Int_t activePad)
{
	for(Int_t iPad = 0; iPad < moduleGeo->GetNeighborsNum(activePad); iPad++)
	{
		if(fS[moduleGeo->GetNeighbor(activePad, iPad)] == 1)
		{
			fNumbersOfPads[moduleGeo->GetNeighbor(activePad, iPad)] = fNumbersOfPads[activePad];
			fS[moduleGeo->GetNeighbor(activePad, iPad)] = 0;
			SLRec1(moduleGeo, moduleGeo->GetNeighbor(activePad, iPad));
        }
	}
}

void CbmClusteringSL::SLRec2(CbmClusteringGeometry* moduleGeo, Int_t activePad)
{
	for(Int_t iPad = 0; iPad < moduleGeo->GetGoodNeighborsNum(activePad); iPad++)
	{
		if(fS[moduleGeo->GetNeighbor(activePad, iPad)] == 1)
		{
			fNumbersOfPads[moduleGeo->GetNeighbor(activePad, iPad)] = fNumbersOfPads[activePad];
			fS[moduleGeo->GetNeighbor(activePad, iPad)] = 0;
			SLRec2(moduleGeo, moduleGeo->GetNeighbor(activePad, iPad));
        }
	}
}

void CbmClusteringSL::MainClusteringSL(CbmClusteringGeometry* moduleGeo, Int_t algVersion)
{
	//algVersion == 1 -> all neighbors
	//algVersion == 2 -> only good neighbors
   //First step of clustering algorithm: Creating relationships between objects
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if((fA1[iPad] > 0) && (fS[iPad] == 1))
		{
			if(algVersion == 1)
			{
				SLRec1(moduleGeo, iPad);
			}
			if(algVersion == 2)
			{
				SLRec2(moduleGeo, iPad);
			}
			if((algVersion != 1)&&(algVersion != 2))
			{
				std::cout<<" - CbmClusteringSL: Error! Unsupported version of the algorithm.\n";
			}
		}
	}
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if(fA1[iPad] > 0)
		{
			fS[iPad] = 1;
		}
	}
	for(Int_t iPad = 0; iPad < fNofActivePads; iPad++)          //All clusters are empty
	{
		//fClusters[iPad].nofCluster = 0;
		fClusters[iPad].fNofPads = 0;
		fClusters[iPad].fCharge = 0;
		fClusters[iPad].fX = 0;
		fClusters[iPad].fY = 0;
	}

	//Second step of clustering algorithm: Creating clusters by relationships
	Int_t nomCl = 0;
	Int_t Replase = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if((fA1[iPad] != 0) && (fS[iPad] == 1))
		//if(activePad1[iPad] != 0)
		{
			Replase = fNumbersOfPads[iPad];
			nomCl++;
			Int_t padInCluster = 0;
			for(Int_t nPad = 0; nPad < fNofPads; nPad++)
			{

				if((fNumbersOfPads[nPad] == Replase) &&
				(moduleGeo->GetPadCharge(nPad) > 0) && (fS[nPad] == 1))
				{
					fNumbersOfPads[nPad] = nomCl;
					fS[nPad] = 0;
					//Filling clusters
					fClusters[nomCl - 1].fX += (moduleGeo->GetX0(nPad) * moduleGeo->GetPadCharge(nPad));
					fClusters[nomCl - 1].fY += (moduleGeo->GetY0(nPad) * moduleGeo->GetPadCharge(nPad));
					fClusters[nomCl - 1].fCharge += moduleGeo->GetPadCharge(nPad);
					fClusters[nomCl - 1].fPadsInCluster.push_back(moduleGeo->GetDigiNum(nPad));
					padInCluster++;
					fClusters[nomCl - 1].fNofPads = padInCluster;
				}
			}
		}
	}
	//Hits calculation
	for(Int_t iCl = 0; iCl < nomCl; iCl++)
	{
		if(fClusters[iCl].fCharge == 0)
		{
		   cout<<" - MainClusteringA1: Warning! DIVISION ON ZERO!";
		   break;
		}
		fClusters[iCl].fX = fClusters[iCl].fX / fClusters[iCl].fCharge;
		fClusters[iCl].fY = fClusters[iCl].fY / fClusters[iCl].fCharge;
	}
	fNofClusters = nomCl;
}

/*Int_t CbmClusteringSL::GetCluster(Int_t iCluster)
{
	return fClusters[iCluster].nofCluster;
}*/
Float_t CbmClusteringSL::GetX0(Int_t iCluster)
{
	return fClusters[iCluster].fX;
}
Float_t CbmClusteringSL::GetY0(Int_t iCluster)
{
	return fClusters[iCluster].fY;
}
UInt_t CbmClusteringSL::GetClCharge(Int_t iCluster)
{
	return fClusters[iCluster].fCharge;
}
Int_t CbmClusteringSL::GetNofPads(Int_t iCluster)
{
	return fClusters[iCluster].fNofPads;
}
Int_t CbmClusteringSL::GetPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fPadsInCluster[iPad];
}
