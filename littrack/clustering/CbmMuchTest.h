/*
 * CbmMuchTest.h
 *
 *  Created on: May 2, 2012
 *      Author: kozlov
 */

#ifndef CBMMUCHTEST_H_
#define CBMMUCHTEST_H_

#include "FairTask.h"

//class CbmMuchGeoScheme;
//class CbmMuchCluster;
//class CbmMuchPad;
//class TClonesArray;

class CbmMuchTest//: public FairTask
{
public:
   /**
    * \brief Constructor.
    */
	CbmMuchTest();
	CbmMuchTest(Int_t nofPoints, Float_t subStep);
	//CbmMuchGeoCl(Int_t nStation, Int_t nLayer, Bool_t nSide, Int_t nModule);

	void SetNPoints(Int_t nofPoints);
	void SetXIn(Float_t xIn, Int_t iPoint);
	void SetYIn(Float_t yIn, Int_t iPoint);
	void SetXOut(Float_t xOut, Int_t iPoint);
	void SetYOut(Float_t yOut, Int_t iPoint);
	//void SetNHits(Int_t nofHits);
	void SetSubStep(Int_t iStep);

	void SetNClusters(Int_t NCl);//	{ fNofClusters = NCl; }

	Int_t GetNPoints()	const { return fNofPoints; }
	Int_t GetNSbCenters()	const {return fNofSubCenters; }

	void RebuildPoints();

	//---
	void SetNCl(Int_t nCl, Int_t iCl);
	void SetXCl(Float_t xCl, Int_t iCl);
	void SetYCl(Float_t yCl, Int_t iCl);
	void SetCharge(UInt_t chCl, Int_t iCl);
	void SetNPads(Int_t nofPads, Int_t iCl);
	void SetPadInCl(Int_t nPad, Int_t iCl, Int_t iPad);
	void SetCluster(Int_t nCl, Float_t xCl, Float_t yCl, UInt_t chCl, Int_t nofPads, Int_t iCl);

	//--
	void CalculateEfficiency();
	void CalculateAccuracy();
	void MainTesting();
	Float_t GetMeanAccuracy()	const { return ((fMeanErrorOnX + fMeanErrorOnY) / 2); }
	Float_t GetMeanAccuracyForSubCenters()	const { return ((fMeanErrorOnXforSubCenters + fMeanErrorOnYforSubCenters) / 2); }
	Float_t GetMeanEfficiency()	const { return fEfficiency; }
	Float_t GetMeanEfficiencyForSubCenters()	const { return fEfficiencyForSubCenters; }
	/**
    * \brief Destructor.
    */
   virtual ~CbmMuchTest();

private:

   Int_t fNofPoints;
   Int_t fNofSubCenters;
   Float_t fSubStep;
   static const Int_t fMaxSubCentersPerPoint = 10;

   struct M_Point{
	   Float_t x1, y1, x2, y2, xc, yc;
	   Int_t nofSubCenters;
	   Float_t pLenght;
	   Float_t xSC[fMaxSubCentersPerPoint];
	   Float_t ySC[fMaxSubCentersPerPoint];
   };
   M_Point* fRealPoints;

   /*Int_t fNofHits;
   struct M_Hit{
	   Float_t x, y;
	   //UInt_t charge;
   };
   M_Hit* fHits;*/

   Int_t fNofClusters;
   struct Cluster{
   	   Int_t nofCluster;
   	   Float_t xc;
   	   Float_t yc;
   	   UInt_t sumClCharge;
   	   Int_t nofPads;
   	   Int_t padsInCluster[30];
   };
   Cluster* fClusters;
   Float_t fMeanErrorOnX;
   Float_t fMeanErrorOnY;
   Float_t* fErrorsOnX;
   Float_t* fErrorsOnY;
   Float_t fMeanErrorOnXforSubCenters;
   Float_t fMeanErrorOnYforSubCenters;
   Float_t* fErrorsOnXforSubCenters;
   Float_t* fErrorsOnYforSubCenters;

   Float_t fEfficiency;
   Float_t fEfficiencyForSubCenters;

   CbmMuchTest(const CbmMuchTest&);
   CbmMuchTest& operator=(const CbmMuchTest&);
};

#endif /* CBMMUCHTEST_H_ */
