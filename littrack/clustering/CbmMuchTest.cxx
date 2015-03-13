/*
 * CbmMuchTest.cxx
 *
 *  Created on: May 2, 2012
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
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchTest.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

CbmMuchTest::CbmMuchTest():
		fMeanErrorOnXforSubCenters(),
		fMeanErrorOnYforSubCenters(),
		fErrorsOnXforSubCenters(),
		fErrorsOnYforSubCenters(),
		fEfficiencyForSubCenters(),
		fEfficiency(),
		fMeanErrorOnY(),
		fMeanErrorOnX(),
		fClusters(),
		fErrorsOnX(),
		fErrorsOnY()
{
	fNofPoints = 0;
	//fNofHits = 0;
	fSubStep = 0;
	fNofSubCenters = 0;
	fNofClusters = 0;
	fRealPoints = 0;
}

CbmMuchTest::CbmMuchTest(Int_t nofPoints, Float_t subStep):
		fMeanErrorOnXforSubCenters(),
		fMeanErrorOnYforSubCenters(),
		fErrorsOnXforSubCenters(),
		fErrorsOnYforSubCenters(),
		fMeanErrorOnY(),
		fMeanErrorOnX(),
		fErrorsOnX(),
		fErrorsOnY(),
		fEfficiencyForSubCenters(),
		fEfficiency(),
		fClusters()
{
	fNofPoints = nofPoints;
	fSubStep = subStep;
	fNofSubCenters = 0;
	fRealPoints = new M_Point[fNofPoints];
	fNofClusters = 0;
}

CbmMuchTest::~CbmMuchTest()
{

}

void CbmMuchTest::RebuildPoints()
{
	Float_t pLenght = 0;
	for(Int_t iPoint = 0; iPoint < fNofPoints; iPoint++)
	{
		pLenght = sqrt((fRealPoints[iPoint].x1 - fRealPoints[iPoint].x2) *
				(fRealPoints[iPoint].x1 - fRealPoints[iPoint].x2) +
				(fRealPoints[iPoint].y1 - fRealPoints[iPoint].y2) *
				(fRealPoints[iPoint].y1 - fRealPoints[iPoint].y2));
		fRealPoints[iPoint].pLenght = pLenght;
		//std::cout<<"pLenght: "<<pLenght<<"; Step: "<<fSubStep<<"\n";
		if(pLenght < (fSubStep * 2))
		{
			fRealPoints[iPoint].xc = (fRealPoints[iPoint].x1 + fRealPoints[iPoint].x2) / 2;
			fRealPoints[iPoint].yc = (fRealPoints[iPoint].y1 + fRealPoints[iPoint].y2) / 2;
			fRealPoints[iPoint].nofSubCenters = 1;
			fRealPoints[iPoint].xSC[0] = fRealPoints[iPoint].xc;
			fRealPoints[iPoint].ySC[0] = fRealPoints[iPoint].yc;
			fNofSubCenters++;
		}
		else
		{
			Int_t Dstep = pLenght / fSubStep;
			//std::cout<<"---Dstep: "<<Dstep<<"\n";
			for(Int_t iStep = 0; iStep < Dstep; iStep++)
			{
				fRealPoints[iPoint].xSC[iStep] = fRealPoints[iPoint].x1 +
						(((fRealPoints[iPoint].x2 - fRealPoints[iPoint].x1) / Dstep) * (iStep + 1));
				fRealPoints[iPoint].ySC[iStep] = fRealPoints[iPoint].y1 +
						(((fRealPoints[iPoint].y2 - fRealPoints[iPoint].y1) / Dstep) * (iStep + 1));
				fNofSubCenters++;
				//std::cout<<"------iStep: "<<iStep<<"; xSC: "<<fRealPoints[iPoint].xSC[iStep]<<"; ySC: "<<fRealPoints[iPoint].ySC[iStep]<<"\n";
			}
		}
	}
	//std::cout<<"fNofSubCenters: "<<fNofSubCenters<<"\n";
}

void CbmMuchTest::SetNPoints(Int_t nofPoints)
{
	fNofPoints = nofPoints;
	fRealPoints = new M_Point[fNofPoints];
}
/*void CbmMuchTest::SetNHits(Int_t nofHits)
{
	fNofHits = nofHits;
}*/

void CbmMuchTest::SetXIn(Float_t xIn, Int_t iPoint)
{
	if(iPoint >= fNofPoints)	//Bad condition!!!
	{
		std::cout<<"Error! fRealPoint is not declared!\n";
	}
	else
	{
		fRealPoints[iPoint].x1 = xIn;
		//std::cout<<"-> XIn: "<<fRealPoints[iPoint].x1<<" <- "<<iPoint<<"\n";
	}
}

void CbmMuchTest::SetYIn(Float_t yIn, Int_t iPoint)
{
	if(iPoint >= fNofPoints)	//Bad condition!!!
	{
		std::cout<<"Error! fRealPoint is not declared!\n";
	}
	else
	{
		fRealPoints[iPoint].y1 = yIn;
	}
}

void CbmMuchTest::SetXOut(Float_t xOut, Int_t iPoint)
{
	if(iPoint >= fNofPoints)	//Bad condition!!!
	{
		std::cout<<"Error! fRealPoint is not declared!\n";
	}
	else
	{
		fRealPoints[iPoint].x2 = xOut;
	}
}

void CbmMuchTest::SetYOut(Float_t yOut, Int_t iPoint)
{
	if(iPoint >= fNofPoints)	//Bad condition!!!
	{
		std::cout<<"Error! fRealPoint is not declared!\n";
	}
	else
	{
		fRealPoints[iPoint].y2 = yOut;
	}
}

void CbmMuchTest::SetSubStep(Int_t iStep)
{
	fSubStep = iStep;
}

void CbmMuchTest::SetNClusters(Int_t NCl)
{
	fNofClusters = NCl;
	fClusters = new Cluster [fNofClusters];
	fErrorsOnX = new Float_t [fNofClusters];
	fErrorsOnY = new Float_t [fNofClusters];
	fErrorsOnXforSubCenters = new Float_t [fNofClusters];
	fErrorsOnYforSubCenters = new Float_t [fNofClusters];
	fEfficiency = 0;
	fEfficiencyForSubCenters = 0;
}
void CbmMuchTest::SetNCl(Int_t nCl, Int_t iCl)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].nofCluster = nCl;
	}
}
void CbmMuchTest::SetXCl(Float_t xCl, Int_t iCl)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].xc = xCl;
	}
}
void CbmMuchTest::SetYCl(Float_t yCl, Int_t iCl)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].yc = yCl;
	}
}
void CbmMuchTest::SetCharge(UInt_t chCl, Int_t iCl)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].sumClCharge = chCl;
	}
}
void CbmMuchTest::SetNPads(Int_t nofPads, Int_t iCl)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].nofPads = nofPads;
	}
}
void CbmMuchTest::SetPadInCl(Int_t nPad, Int_t iCl, Int_t iPad)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].padsInCluster[iPad] = nPad;
	}
}
void CbmMuchTest::SetCluster(Int_t nCl, Float_t xCl, Float_t yCl, UInt_t chCl, Int_t nofPads, Int_t iCl)
{
	if(fNofClusters == 0)
	{
		std::cout<<"Error! fClusters is not declared!\n";
	}
	else
	{
		fClusters[iCl].nofCluster = nCl;
		fClusters[iCl].xc = xCl;
		fClusters[iCl].yc = yCl;
		fClusters[iCl].sumClCharge = chCl;
		fClusters[iCl].nofPads = nofPads;
	}
}

void CbmMuchTest::CalculateEfficiency()
{
	/*std::cout<<"Start\n";
	std::cout<<"fNofClusters: "<<fNofClusters<<"\n";
	std::cout<<"fNofPoints: "<<fNofPoints<<"\n";*/
	fEfficiency = 100.0 * (Float_t)fNofClusters / (Float_t)fNofPoints;
	fEfficiencyForSubCenters = 100.0 * (Float_t)fNofClusters / (Float_t)fNofSubCenters;
}
void CbmMuchTest::CalculateAccuracy()
{
	fMeanErrorOnX = 0;
	fMeanErrorOnY = 0;
	for(Int_t iCl = 0; iCl < fNofClusters; iCl++)
	{
		fErrorsOnX[iCl] = 100000;
		fErrorsOnY[iCl] = 100000;
		fErrorsOnXforSubCenters[iCl] = 100000;
		fErrorsOnYforSubCenters[iCl] = 100000;
		//std::cout<<"-1----- "<<fMeanErrorOnX<<"   ---   "<<fMeanErrorOnY<<"\n";
		Float_t r1 = 100000;
		for(Int_t jCl = 0; jCl < fNofPoints; jCl++)
		{
			Float_t r2 = sqrt((fClusters[iCl].xc - fRealPoints[jCl].xc) *
							(fClusters[iCl].xc - fRealPoints[jCl].xc) +
							(fClusters[iCl].yc - fRealPoints[jCl].yc) *
							(fClusters[iCl].yc - fRealPoints[jCl].yc));
			if(r2 < r1)
			{
				r1 = r2;
				fErrorsOnX[iCl] = fabs(fClusters[iCl].xc - fRealPoints[jCl].xc);
				fErrorsOnY[iCl] = fabs(fClusters[iCl].yc - fRealPoints[jCl].yc);
			}
			/*Не работает эффективность по субцентрам
			 * скорее всего, проблема в модулях с
			 * ячейками разлицного размера.
			 * SV при вычислении fErrorsOnYforSubCenters[iCl].
			 * Скорее всего, проблема с fRealPoints[jCl].ySC[kCl].
			 */
			/*if(fRealPoints[jCl].nofSubCenters > 1)
			{
				for(Int_t kCl = 0; kCl < fRealPoints[jCl].nofSubCenters; kCl++)
				{
					r2 = sqrt((fClusters[iCl].xc - fRealPoints[jCl].xSC[kCl]) *
							(fClusters[iCl].xc - fRealPoints[jCl].xSC[kCl]) +
							(fClusters[iCl].yc - fRealPoints[jCl].ySC[kCl]) *
							(fClusters[iCl].yc - fRealPoints[jCl].ySC[kCl]));
					if(r2 < r1)
					{
						fErrorsOnXforSubCenters[iCl] = fabs(fClusters[iCl].xc - fRealPoints[jCl].xSC[kCl]);
						fErrorsOnYforSubCenters[iCl] = fabs(fClusters[iCl].yc - fRealPoints[jCl].ySC[kCl]);
					}
				}
			}
			else
			{*/
				fErrorsOnXforSubCenters[iCl] = fErrorsOnX[iCl];
				fErrorsOnYforSubCenters[iCl] = fErrorsOnY[iCl];
			//}
		}
		fMeanErrorOnX += fErrorsOnX[iCl];
		fMeanErrorOnY += fErrorsOnY[iCl];
		fMeanErrorOnXforSubCenters += fErrorsOnXforSubCenters[iCl];
		fMeanErrorOnYforSubCenters += fErrorsOnYforSubCenters[iCl];
		//std::cout<<"-2----- "<<fMeanErrorOnX<<"   ---   "<<fMeanErrorOnY<<"\n";
	}
	fMeanErrorOnX = fMeanErrorOnX / fNofClusters;
	fMeanErrorOnY = fMeanErrorOnY / fNofClusters;
	fMeanErrorOnXforSubCenters = fMeanErrorOnXforSubCenters / fNofClusters;
	fMeanErrorOnYforSubCenters = fMeanErrorOnYforSubCenters / fNofClusters;
}
void CbmMuchTest::MainTesting()
{
	CalculateEfficiency();
	CalculateAccuracy();
}
