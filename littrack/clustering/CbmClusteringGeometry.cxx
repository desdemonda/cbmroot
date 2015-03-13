/*
 * CbmClusteringGeometry.cxx
 *
 *  Created on: Feb 22, 2012
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
#include "CbmMuchModuleGemRadial.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmMuchDigi.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchSectorRadial.h"
#include "CbmMuchPadRectangular.h"
#include "CbmMuchAddress.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmClusteringGeometry::CbmClusteringGeometry():
		fDetId(),
		fPadList()
{
	fNofPads = 0;
	fNofActivePads = 0;
}

CbmClusteringGeometry::CbmClusteringGeometry(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	SetMuchModuleGeometryRadialFast(nStation, nLayer, nSide, nModule, scheme);
//	switch(geoVersion){
//	case 1:
//	{
//		SetMuchModuleGeometryRectangular(nStation, nLayer, nSide, nModule, scheme);     //For old rectangular geometry
//		break;
//	}
//	case 2:
//	{
//		SetMuchModuleGeometryRadial(nStation, nLayer, nSide, nModule, scheme);          //For new radial geometry
//		break;
//	}
//	default:
//	{
//		std::cout<<"CbmClusteringGeometry: Error! Wrong detector geometry.\n";
//		break;
//	}
//	}
}

//Addition of a single pad
void CbmClusteringGeometry::CbmClusteringSetPad(Int_t nPad, Float_t x, Float_t y, Float_t dx, Float_t dy, Double_t phi1, Double_t phi2, Float_t r1, Float_t r2,
			   Int_t digiNum, UInt_t charge, Long64_t chID)
{
	fPadList[nPad].fDigiNum = digiNum;
	fPadList[nPad].fCharge = charge;
	fPadList[nPad].fX = x;
	fPadList[nPad].fY = y;
	fPadList[nPad].fDx = dx;
	fPadList[nPad].fDy = dy;
	fPadList[nPad].fPhi1 = phi1;
	fPadList[nPad].fPhi2 = phi2;
	fPadList[nPad].fR1 = r1;
	fPadList[nPad].fR2 = r2;
	fPadList[nPad].channelID = chID;
	fPadList[nPad].fNeighbors.clear();
	fPadList[nPad].fNofNeighbors = 0;
	fPadList[nPad].fNofGoodNeighbors = 0;
}

CbmClusteringGeometry::~CbmClusteringGeometry()
{
	delete [] fPadList;
}

/*void CbmClusteringGeometry::SetMuchModuleGeometryRectangular(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	CbmMuchModuleGem* module = (CbmMuchModuleGem*) scheme->GetModule(nStation, nLayer, nSide, nModule);
	fNofPads = module->GetNPads();
	fDetId = CbmMuchAddress::GetAddress(nStation, nLayer, nSide, nModule);
	fPadList = new PadInformation[fNofPads];
	fNofActivePads = 0;
	Int_t nofSectors = module->GetNSectors();
	Int_t padIterator = 0;
	for(Int_t iSector = 0; iSector < nofSectors; iSector++)
	{
		CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSector(iSector); //???
		Int_t nofPadsInSector = sector->GetNChannels();//(sector->GetNCols())*(sector->GetNRows());
		Int_t padNx = sector->GetPadNx();
		Int_t padNy = sector->GetPadNy();
		for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++)
		{
			CbmMuchPadRectangular* pad = (CbmMuchPadRectangular*) sector->GetPadByChannelIndex(iPad);	//!!!
			fPadList[padIterator].fDigiNum = 0;
			fPadList[padIterator].fCharge = 0;
			fPadList[padIterator].fX = pad->GetX();
			fPadList[padIterator].fY = pad->GetY();
			fPadList[padIterator].fDx = sector->GetPadDx();
			fPadList[padIterator].fDy = sector->GetPadDy();
			fPadList[padIterator].fNeighbors.clear();
			fPadList[padIterator].channelID = CbmMuchAddress::GetElementAddress(pad->GetAddress() ,kMuchChannel);
			fPadByChannelId[CbmMuchAddress::GetElementAddress(pad->GetAddress() ,kMuchChannel)] = padIterator;
			padIterator++;
		}
	}
	padIterator = 0;
	for(Int_t iPadMain = 0; iPadMain < fNofPads; iPadMain++)
	{
		//padIterator = 0;
		//Int_t padIterator2 = 0;
		fPadList[iPadMain].fNofNeighbors = 0;
		fPadList[iPadMain].fNofGoodNeighbors = 0;
		Float_t xLeft_1 = fPadList[iPadMain].fX - (fPadList[iPadMain].fDx / 2);
		Float_t xRight_1 = fPadList[iPadMain].fX + (fPadList[iPadMain].fDx / 2);
		Float_t yDown_1 = fPadList[iPadMain].fY - (fPadList[iPadMain].fDy / 2);
		Float_t yUp_1 = fPadList[iPadMain].fY + (fPadList[iPadMain].fDy / 2);
		for(Int_t iPadNeighbor = 0; iPadNeighbor < fNofPads; iPadNeighbor++)
		{
			if((fabs(fPadList[iPadMain].fX - fPadList[iPadNeighbor].fX) < (fPadList[iPadMain].fDx * 3)) &&
				(fabs(fPadList[iPadMain].fY - fPadList[iPadNeighbor].fY) < (fPadList[iPadNeighbor].fDy * 3))){
			Float_t xLeft_2 = fPadList[iPadNeighbor].fX - (fPadList[iPadNeighbor].fDx / 2);
			Float_t xRight_2 = fPadList[iPadNeighbor].fX + (fPadList[iPadNeighbor].fDx / 2);
			Float_t yDown_2 = fPadList[iPadNeighbor].fY - (fPadList[iPadNeighbor].fDy / 2);
			Float_t yUp_2 = fPadList[iPadNeighbor].fY + (fPadList[iPadNeighbor].fDy / 2);
			Float_t dX = fabs(xLeft_1 - xRight_1) * 0.1;
			Float_t dY = fabs(yDown_1 - yUp_1) * 0.1;
			if((((SubEqual(xLeft_1, xRight_2, dX) ||
			SubEqual(xRight_1, xLeft_2, dX)) &&
			(((GetMax(yDown_2, yUp_2) - dY) >
			(GetMin(yDown_1, yUp_1) + dY)) &&
			((GetMin(yDown_2, yUp_2) + dY) <
			(GetMax(yDown_1, yUp_1) - dY)))) ||
			((SubEqual(yDown_1, yUp_2, dY) ||
			SubEqual(yUp_1, yDown_2, dY)) &&
			(((GetMax(xLeft_2, xRight_2) - dX) >
			(GetMin(xLeft_1, xRight_1) + dX)) &&
			((GetMin(xLeft_2, xRight_2) + dX) <
			(GetMax(xLeft_1, xRight_1) - dX))))) &&
			(iPadMain != iPadNeighbor))
			{
				fPadList[iPadMain].fNeighbors.push_back(iPadNeighbor);
				fPadList[iPadMain].fNofGoodNeighbors++;
			}
		}}
	}
}*/

void CbmClusteringGeometry::SetMuchModuleGeometryRadial(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	CbmMuchModuleGemRadial* module = (CbmMuchModuleGemRadial*) scheme->GetModule(nStation, nLayer, nSide, nModule);
	fNofPads = module->GetNPads();
	fDetId = module->GetDetectorId();
	fPadList = new PadInformation[fNofPads];
	fNofActivePads = 0;
	Int_t nofSectors = module->GetNSectors();
	Int_t padIterator = 0;
	for(Int_t iSector = 0; iSector < nofSectors; iSector++)
	{
		CbmMuchSectorRadial* sector = (CbmMuchSectorRadial*) module->GetSectorByIndex(iSector);
		Int_t nofPadsInSector = sector->GetNChannels();
		for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++)
		{
			CbmMuchPadRadial* pad = (CbmMuchPadRadial*) sector->GetPadByChannelIndex(iPad);
			fPadList[padIterator].fDigiNum = 0;
			fPadList[padIterator].fCharge = 0;
			fPadList[padIterator].fPhi1 = pad->GetPhi1();
			fPadList[padIterator].fPhi2 = pad->GetPhi2();
			fPadList[padIterator].fR1 = pad->GetR1();
			fPadList[padIterator].fR2 = pad->GetR2();
			Float_t r = (fPadList[padIterator].fR1 + fPadList[padIterator].fR2) / 2;
			Double_t phi = (fPadList[padIterator].fPhi1 + fPadList[padIterator].fPhi2) / 2;
			fPadList[padIterator].fX = r * cos(phi);
			fPadList[padIterator].fY = r * sin(phi);
			fPadList[padIterator].fNeighbors.clear();
			fPadList[padIterator].channelID = CbmMuchAddress::GetElementAddress(pad->GetAddress() ,kMuchChannel);
			fPadByChannelId[pad->GetAddress()] = padIterator;
			fPadList[padIterator].nSector = iSector;
			padIterator++;
		}
	}
	padIterator = 0;
	vector<UInt_t> diagonalNeighbors;
	diagonalNeighbors.clear();
	for(Int_t iPadMain = 0; iPadMain < fNofPads; iPadMain++)
	{
		fPadList[iPadMain].fNofNeighbors = 0;
		fPadList[iPadMain].fNofGoodNeighbors = 0;
		Double_t Left_1 = GetMin(fPadList[iPadMain].fPhi1, fPadList[iPadMain].fPhi2);
		Double_t Right_1 = GetMax(fPadList[iPadMain].fPhi1, fPadList[iPadMain].fPhi2);
		Float_t Down_1 = GetMin(fPadList[iPadMain].fR1, fPadList[iPadMain].fR2);
		Float_t Up_1 = GetMax(fPadList[iPadMain].fR1, fPadList[iPadMain].fR2);
		Double_t dPhi = fabs(fPadList[iPadMain].fPhi1 - fPadList[iPadMain].fPhi2) * 0.1;
		Float_t dR = fabs(fPadList[iPadMain].fR1 - fPadList[iPadMain].fR2) * 0.1;
		Double_t lPhi = (Left_1 - Right_1) * 0.01;
		Float_t lR = (Down_1 - Up_1) * 0.01;
		for(Int_t iPadNeighbor = 0; iPadNeighbor < fNofPads; iPadNeighbor++)
		{
			if(iPadMain == iPadNeighbor)continue;
			Double_t Left_2 = GetMin(fPadList[iPadNeighbor].fPhi1, fPadList[iPadNeighbor].fPhi2);
			Double_t Right_2 = GetMax(fPadList[iPadNeighbor].fPhi1, fPadList[iPadNeighbor].fPhi2);
			Float_t Down_2 = GetMin(fPadList[iPadNeighbor].fR1, fPadList[iPadNeighbor].fR2);
			Float_t Up_2 = GetMax(fPadList[iPadNeighbor].fR1, fPadList[iPadNeighbor].fR2);
			if((Left_1 == Right_2) || (Right_1 == Left_2))
			{
				if(((Down_1 - lR) < Up_2) && ((Up_1 + lR) > Down_2))
				{
					fPadList[iPadMain].fNeighbors.push_back(iPadNeighbor);
					fPadList[iPadMain].fNofGoodNeighbors++;
				}
			}
			if((Up_1 == Down_2) || (Down_1 == Up_2))
			{
				if(((Left_1 - lPhi) < Right_2) && ((Right_1 + lPhi) > Left_2))
				{
					fPadList[iPadMain].fNeighbors.push_back(iPadNeighbor);
					fPadList[iPadMain].fNofGoodNeighbors++;
				}
			}
			fPadList[iPadMain].fNofNeighbors = fPadList[iPadMain].fNofGoodNeighbors;
			if(((Left_1 == Right_2) && (Up_1 == Down_2)) || ((Left_1 == Right_2) && (Up_2 == Down_1)) ||
					((Left_2 == Right_1) && (Up_1 == Down_2)) || ((Left_2 == Right_1) && (Up_2 == Down_1))){
				diagonalNeighbors.push_back(iPadNeighbor);
			}
		}
		for(Int_t iNeighbor = 0; iNeighbor < diagonalNeighbors.size(); iNeighbor++){
			fPadList[iPadMain].fNeighbors.push_back(diagonalNeighbors[iNeighbor]);
			fPadList[iPadMain].fNofNeighbors++;
		}
		diagonalNeighbors.clear();
	}
}

void CbmClusteringGeometry::SetMuchModuleGeometryRadialFast(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule, CbmMuchGeoScheme* scheme)
{
	CbmMuchModuleGemRadial* module = (CbmMuchModuleGemRadial*) scheme->GetModule(nStation, nLayer, nSide, nModule);
	fNofPads = module->GetNPads();
	fDetId = module->GetDetectorId();
	fPadList = new PadInformation[fNofPads];
	fNofActivePads = 0;
	Int_t nofSectors = module->GetNSectors();
	Int_t padIterator = 0;
	for(Int_t iSector = 0; iSector < nofSectors; iSector++)
	{
		CbmMuchSectorRadial* sector = (CbmMuchSectorRadial*) module->GetSectorByIndex(iSector);
		Int_t nofPadsInSector = sector->GetNChannels();
		for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++)
		{
			CbmMuchPadRadial* pad = (CbmMuchPadRadial*) sector->GetPadByChannelIndex(iPad);
			fPadList[padIterator].fDigiNum = 0;
			fPadList[padIterator].fCharge = 0;
			fPadList[padIterator].fPhi1 = pad->GetPhi1();
			fPadList[padIterator].fPhi2 = pad->GetPhi2();
			fPadList[padIterator].fR1 = pad->GetR1();
			fPadList[padIterator].fR2 = pad->GetR2();
			Float_t r = (fPadList[padIterator].fR1 + fPadList[padIterator].fR2) / 2;
			Double_t phi = (fPadList[padIterator].fPhi1 + fPadList[padIterator].fPhi2) / 2;
			fPadList[padIterator].fX = r * cos(phi);
			fPadList[padIterator].fY = r * sin(phi);
			fPadList[padIterator].fNeighbors.clear();
			fPadList[padIterator].fNofNeighbors = 0;
			fPadList[padIterator].fNofGoodNeighbors = 0;
			fPadList[padIterator].channelID = CbmMuchAddress::GetElementAddress(pad->GetAddress() ,kMuchChannel);
			fPadByChannelId[pad->GetAddress()] = padIterator;
			fPadList[padIterator].nSector = iSector;
			padIterator++;
		}
	}
	padIterator = 0;
	vector<UInt_t> diagonalNeighbors;
	diagonalNeighbors.clear();
	for(Int_t iSector = 0; iSector < nofSectors; iSector++){
		CbmMuchSectorRadial* sector = (CbmMuchSectorRadial*) module->GetSectorByIndex(iSector);
		Int_t nofPadsInSector = sector->GetNChannels();
		for(Int_t iPad = 0; iPad < nofPadsInSector; iPad++){
			Double_t dPhi = fabs(fPadList[iPad].fPhi1 - fPadList[iPad].fPhi2) * 0.1;
			Float_t dR = fabs(fPadList[iPad].fR1 - fPadList[iPad].fR2) * 0.1;
			CbmMuchPadRadial* pad = (CbmMuchPadRadial*) sector->GetPadByChannelIndex(iPad);
			vector<CbmMuchPad*> neighborsVector = pad->GetNeighbours();
			for(Int_t iNeighbor = 0; iNeighbor < neighborsVector.size(); iNeighbor++){
				Int_t padNeighbor = fPadByChannelId[neighborsVector.at(iNeighbor)->GetAddress()];
				if((SubEqual(fPadList[padIterator].fPhi1, fPadList[padNeighbor].fPhi2, dPhi) &&
						SubEqual(fPadList[padIterator].fR1, fPadList[padNeighbor].fR2, dR)) ||
						(SubEqual(fPadList[padIterator].fPhi1, fPadList[padNeighbor].fPhi2, dPhi) &&
						SubEqual(fPadList[padIterator].fR2, fPadList[padNeighbor].fR1, dR)) ||
					(SubEqual(fPadList[padIterator].fPhi2, fPadList[padNeighbor].fPhi1, dPhi) &&
						SubEqual(fPadList[padIterator].fR1, fPadList[padNeighbor].fR2, dR)) ||
						(SubEqual(fPadList[padIterator].fPhi2, fPadList[padNeighbor].fPhi1, dPhi) &&
						SubEqual(fPadList[padIterator].fR2, fPadList[padNeighbor].fR1, dR))){
					diagonalNeighbors.push_back(padNeighbor);
				}
				else{
					fPadList[padIterator].fNeighbors.push_back(padNeighbor);
					fPadList[padIterator].fNofGoodNeighbors++;
					fPadList[padIterator].fNofNeighbors++;
				}
			}
			for(Int_t iNeighbor = 0; iNeighbor < diagonalNeighbors.size(); iNeighbor++){
				fPadList[padIterator].fNeighbors.push_back(diagonalNeighbors.at(iNeighbor));
				fPadList[padIterator].fNofNeighbors++;
			}
			diagonalNeighbors.clear();
			padIterator++;
		}
	}
}

Float_t CbmClusteringGeometry::GetX0(Int_t iPad)
{
	return fPadList[iPad].fX;
}

Float_t CbmClusteringGeometry::GetDx(Int_t iPad)
{
	return fPadList[iPad].fDx;
}

Float_t CbmClusteringGeometry::GetDy(Int_t iPad)
{
	return fPadList[iPad].fDy;
}

Float_t CbmClusteringGeometry::GetY0(Int_t iPad)
{
	return fPadList[iPad].fY;
}

Int_t CbmClusteringGeometry::GetDigiNum(Int_t iPad)
{
	return fPadList[iPad].fDigiNum;
}

void CbmClusteringGeometry::SetDigiNum(Int_t iPad, Int_t iDigi)
{
	fPadList[iPad].fDigiNum = iDigi;
}

Int_t CbmClusteringGeometry::GetNeighborsNum(Int_t iPad)
{
	return fPadList[iPad].fNofNeighbors;
}

Int_t CbmClusteringGeometry::GetGoodNeighborsNum(Int_t iPad)
{
	return fPadList[iPad].fNofGoodNeighbors;
}

Int_t CbmClusteringGeometry::GetNeighbor(Int_t iPad, Int_t iNeighbor)
{
	return fPadList[iPad].fNeighbors[iNeighbor];
}

Long64_t CbmClusteringGeometry::GetPadID(Int_t iPad)
{
	return fPadList[iPad].channelID;
}

UInt_t CbmClusteringGeometry::GetPadCharge(Int_t iPad)
{
	return fPadList[iPad].fCharge;
}

void CbmClusteringGeometry::SetPadCharge(Int_t iPad, UInt_t iCharge)
{
	fPadList[iPad].fCharge = iCharge;
}

void CbmClusteringGeometry::SetAPadsNom(Int_t nPads)
{
	fNofActivePads = nPads;
}

void CbmClusteringGeometry::SetAPadsPlusOne()
{
	fNofActivePads++;
}

template <typename T1>
T1 CbmClusteringGeometry::GetMin(T1& a, T1& b)
{
	if(a > b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

template <typename T2>
T2 CbmClusteringGeometry::GetMax(T2& a, T2& b)
{
	if(a < b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

Bool_t CbmClusteringGeometry::SubEqual(Double_t x1, Double_t x2, Double_t l)
{
	//l = l * 0.1;
	if((x1 < (x2 + l)) && (x1 > (x2 - l)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Int_t CbmClusteringGeometry::GetPadByChannelId(Long64_t chId)
{
	return fPadByChannelId[chId];
}

Double_t CbmClusteringGeometry::GetPhi1(Int_t iPad)
{
	return fPadList[iPad].fPhi1;
}

Double_t CbmClusteringGeometry::GetPhi2(Int_t iPad)
{
	return fPadList[iPad].fPhi2;
}

Float_t CbmClusteringGeometry::GetR1(Int_t iPad)
{
	return fPadList[iPad].fR1;
}

Float_t CbmClusteringGeometry::GetR2(Int_t iPad)
{
	return fPadList[iPad].fR2;
}

vector<Int_t> CbmClusteringGeometry::GetNeighbors(Int_t iPad)
{
	return fPadList[iPad].fNeighbors;
}

Long64_t CbmClusteringGeometry::GetChannelID(Int_t iPad)
{
	return fPadList[iPad].channelID;
}
