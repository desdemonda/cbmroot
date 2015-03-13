/*
 * CbmClusteringA1.cxx
 *
 *  Created on: Apr 4, 2012
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
#include "CbmClusteringA1.h"

#include "FairRootManager.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringA1::CbmClusteringA1():
		fA1(),
		fA2(),
		fS(),
		fClusters(),
		fNumbersOfPads()
{
	fNofPads = 0;
	fNofActivePads = 0;
	fNofClusters = 0;
}

CbmClusteringA1::CbmClusteringA1(CbmClusteringGeometry* moduleGeo):
		fClusters()
{
   //Initialization
	fNofPads = moduleGeo->GetNPads();
	fNofActivePads = moduleGeo->GetAPadsNom();
	fNofClusters = fNofActivePads;
	fA1 = new UInt_t [fNofPads];
	fA2 = new UInt_t [fNofPads];
	fS = new Bool_t [fNofPads];
	fNumbersOfPads = new Int_t [fNofPads];
	Int_t nom = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		fA1[iPad] = moduleGeo->GetPadCharge(iPad);  //Filling primary array of charges
		fA2[iPad] = 0;
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

CbmClusteringA1::~CbmClusteringA1()
{
	delete [] fA1;
	delete [] fA2;
	delete [] fS;
	delete [] fNumbersOfPads;
	delete [] fClusters;
}

void CbmClusteringA1::MainClusteringA1(CbmClusteringGeometry* moduleGeo, Int_t algVersion)
{
	//algVersion == 1 -> all neighbors
	//algVersion == 2 -> only good neighbors
	Int_t localMaximum;
	Int_t temp;
	UInt_t temp1;
	//First step of clustering algorithm: Creating relationships between objects
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if(fA1[iPad] > 0)
		{
			localMaximum = iPad;
			if(algVersion == 1){
				for(Int_t nPad = 0; nPad < moduleGeo->GetNeighborsNum(iPad); nPad++)
				{
					if(fA1[moduleGeo->GetNeighbor(iPad, nPad)] > fA1[localMaximum])
					{
						localMaximum = moduleGeo->GetNeighbor(iPad, nPad);
					}
				}
			}
			if(algVersion == 2){
				for(Int_t nPad = 0; nPad < moduleGeo->GetGoodNeighborsNum(iPad); nPad++)
				{
					if(fA1[moduleGeo->GetNeighbor(iPad, nPad)] > fA1[localMaximum])
					{
						localMaximum = moduleGeo->GetNeighbor(iPad, nPad);
					}
				}
			}
			if((algVersion != 1)&&(algVersion != 2))
			{
				std::cout<<"Error! Unsupported version of the algorithm.\n";
			}
			fA2[localMaximum] += fA1[iPad];         //Filling secondary array of charges
			//fA2[iPad] -= fA1[iPad];				//For a special cases
			if(iPad != localMaximum)
			{
				fA2[iPad] = 0;
				fNofActivePads--;
				ChangeClusters(moduleGeo, iPad, fNumbersOfPads[iPad], fNumbersOfPads[localMaximum], algVersion);        //Changing relationships
			}
		}
	}
	//Second step of clustering algorithm: Creating clusters by relationships
	fClusters = new Cluster[(Int_t)(fNofActivePads)];
	for(Int_t iPad = 0; iPad < (Int_t)(fNofActivePads); iPad++)     //All clusters are empty
	{
		fClusters[iPad].fNCluster = 0;
		fClusters[iPad].fNofPads = 0;
		fClusters[iPad].fCharge = 0;
		fClusters[iPad].fX = 0;
		fClusters[iPad].fY = 0;
		fClusters[iPad].fPadsInCluster.clear();
		fClusters[iPad].fNPadsInCluster.clear();
		fClusters[iPad].fPadsCharges.clear();
	}

	Int_t nomCl = 0;
	Int_t Replase = 0;
	for(Int_t iPad = 0; iPad < fNofPads; iPad++)
	{
		if((fA2[iPad] != 0) && (fS[iPad] == 1))
		{
			Replase = fNumbersOfPads[iPad];
			nomCl++;
			Int_t padInCluster = 0;
			for(Int_t nPad = 0; nPad < fNofPads; nPad++)
			{
				if((fNumbersOfPads[nPad] == Replase) &&
				(moduleGeo->GetPadCharge(nPad) > 0) && (fS[nPad] == 1))
				{
					//Filling clusters
					fNumbersOfPads[nPad] = nomCl;
					fS[nPad] = 0;
					fClusters[nomCl - 1].fNCluster = nomCl;
					fClusters[nomCl - 1].fX += (moduleGeo->GetX0(nPad) * moduleGeo->GetPadCharge(nPad));
					fClusters[nomCl - 1].fY += (moduleGeo->GetY0(nPad) * moduleGeo->GetPadCharge(nPad));
					fClusters[nomCl - 1].fCharge += moduleGeo->GetPadCharge(nPad);
					fClusters[nomCl - 1].fPadsInCluster.push_back(moduleGeo->GetDigiNum(nPad));
					fClusters[nomCl - 1].fNPadsInCluster.push_back(nPad);
					fClusters[nomCl - 1].fPadsCharges.push_back(fA1[nPad]);
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

void CbmClusteringA1::ChangeClusters(CbmClusteringGeometry* moduleGeo, Int_t nPad, Int_t Cl0, Int_t Cl1, Int_t vers)
{
	fNumbersOfPads[nPad] = Cl1;
	Int_t nofNeighbors = 0;
	if(vers == 1)nofNeighbors = moduleGeo->GetNeighborsNum(nPad);
	if(vers == 2)nofNeighbors = moduleGeo->GetGoodNeighborsNum(nPad);
	for(Int_t iPad = 0; iPad < nofNeighbors; iPad++)
	{
		if(fNumbersOfPads[moduleGeo->GetNeighbor(nPad, iPad)] == Cl0)
		{
			ChangeClusters(moduleGeo, moduleGeo->GetNeighbor(nPad, iPad), Cl0, Cl1, vers);
		}
	}
}

Int_t CbmClusteringA1::GetCluster(Int_t iCluster)
{
	return fClusters[iCluster].fNCluster;
}
Float_t CbmClusteringA1::GetX0(Int_t iCluster)
{
	return fClusters[iCluster].fX;
}
Float_t CbmClusteringA1::GetY0(Int_t iCluster)
{
	return fClusters[iCluster].fY;
}
UInt_t CbmClusteringA1::GetClCharge(Int_t iCluster)
{
	return fClusters[iCluster].fCharge;
}
Int_t CbmClusteringA1::GetNofPads(Int_t iCluster)
{
	return fClusters[iCluster].fNofPads;
}
Int_t CbmClusteringA1::GetPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fPadsInCluster[iPad];
}

Int_t CbmClusteringA1::GetNPadInCluster(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fNPadsInCluster[iPad];
}

vector<Int_t> CbmClusteringA1::GetPads(Int_t iCluster)
{
	return fClusters[iCluster].fPadsInCluster;
}

UInt_t CbmClusteringA1::GetPadCharge(Int_t iCluster, Int_t iPad)
{
	return fClusters[iCluster].fPadsCharges[iPad];
}
