/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - realizes a dummy for building hits from points
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-07 10:39:35 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../include/frameworkError.h"
#include "../include/hitProducer.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/CbmStsPoint.h"
	#include "../../RootFrameworkLIB/include/CbmMvdPoint.h"
	#include "../../RootFrameworkLIB/include/CbmStsMapsHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsStripHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsHybridHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsHit.h"
	#include "../../RootFrameworkLIB/include/CbmMvdHit.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "CbmStsPoint.h"
	#ifdef HITCOMPATIBILITY
		#include "CbmStsMapsHit.h"
		#include "CbmStsStripHit.h"
		#include "CbmStsHybridHit.h"
	#else
		#include "CbmMvdPoint.h"
		#include "CbmStsHit.h"
		#include "CbmMvdHit.h"
	#endif
#endif
#include "TRandom.h"
#include "TVector3.h"


#define binningXResolution 0.005		/**< Defines the resolution in the x-dimension for a binning grid. */
#define binningYResolution 0.005		/**< Defines the resolution in the y-dimension for a binning grid. */
#define smearingXSigma     0.002		/**< Defines the sigma distribution for a gaussian in x-dimension. */
#define smearingYSigma     0.002		/**< Defines the sigma distribution for a gaussian in y-dimension. */


/****************************************************************
 * method produces the hits with nothing						*
 ****************************************************************/

void FairHitProducer::produceNo(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn) {

	*xPosOut = xPosIn;
	*yPosOut = yPosIn;

}

/****************************************************************
 * method produces the hits with binning						*
 ****************************************************************/

void FairHitProducer::produceBin(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn) {

	*xPosOut = ((double)((int)((xPosIn + binningXResolution / 2) / binningXResolution))) * binningXResolution;
	*yPosOut = ((double)((int)((yPosIn + binningYResolution / 2) / binningYResolution))) * binningYResolution;

}

/****************************************************************
 * method produces the hits with smearing						*
 ****************************************************************/

void FairHitProducer::produceSmear(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn) {

	if (gRandom != NULL) {

		*xPosOut = gRandom->Gaus(xPosIn, smearingXSigma);
		*yPosOut = gRandom->Gaus(yPosIn, smearingYSigma);

	}
	else
		throw cannotSmearError();

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

FairHitProducer::FairHitProducer() 
  : typeOfHitProducer(NONEHITPRODUCER),
    detector(NULL)
{

  //	typeOfHitProducer = NONEHITPRODUCER;
  //	detector          = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

FairHitProducer::FairHitProducer(int hitProducer, trackfinderInputDetector* actualDetector) 
  : typeOfHitProducer(hitProducer),
    detector(actualDetector)
{

  //	typeOfHitProducer = hitProducer;
  //	detector          = actualDetector;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

FairHitProducer::~FairHitProducer() {

}

/****************************************************************
 * method sets the hitProducerType								*
 ****************************************************************/

void FairHitProducer::setHitProducerType(int hitProducer) {

	typeOfHitProducer = hitProducer;

}

/**
 * method sets the detector
 */

void FairHitProducer::setDetector(trackfinderInputDetector* actualDetector) {

	detector          = actualDetector;

}

/****************************************************************
 * method produces the hits										*
 ****************************************************************/

void FairHitProducer::produceOld(TClonesArray* mapsHits, TClonesArray* stripHits, TClonesArray* hybridHits, TClonesArray* points) {

	double                  xPosIn;
	double                  xPosOut;
	double                  yPosIn;
	double                  yPosOut;
	bool                    isMapsType;
	bool                    isHybridType;
	bool                    isStripType;
	CbmStsPoint*            point;
	TVector3                position;
	TVector3                positionError;

	if (mapsHits == NULL)
		throw cannotAccessHitsError();
	if (stripHits == NULL)
		throw cannotAccessHitsError();
	if (hybridHits == NULL)
		throw cannotAccessHitsError();

	if (points == NULL)
		throw cannotAccessPointError();

#ifdef HITCOMPATIBILITY

	TClonesArray&    mapsHitsReference   = *mapsHits;
	TClonesArray&    stripHitsReference  = *stripHits;
	TClonesArray&    hybridHitsReference = *hybridHits;

#else

		throw functionIsDeprecatedError(ROOTFRAMEWORKLIB);

#endif

	for (int i = 0; i < points->GetEntries(); i++) {

		point = (CbmStsPoint*)points->At(i);
		if (point == NULL)
			throw cannotAccessPointError();

		xPosIn = point->GetX(0);
		yPosIn = point->GetY(0);

		produceHit(&xPosOut, &yPosOut, &isMapsType, &isHybridType, &isStripType, xPosIn, yPosIn, point->GetDetectorID());

		position.SetXYZ(xPosIn, yPosIn, point->GetZ());
		positionError.SetXYZ(xPosOut - xPosIn, yPosOut - yPosIn, 0);

#ifdef HITCOMPATIBILITY

		if (isMapsType)
			new(mapsHitsReference[mapsHits->GetEntries()]) CbmStsMapsHit(point->GetDetectorID(), position, positionError, i, -1);
		else if (isHybridType)
			new(hybridHitsReference[hybridHits->GetEntries()]) CbmStsHybridHit(point->GetDetectorID(), position, positionError, i, -1, -1);
		else if (isStripType)
			new(stripHitsReference[stripHits->GetEntries()]) CbmStsStripHit(point->GetDetectorID(), -1, position, positionError, i, -1, -1, -1, -1, -1, -1, -1, -1, -1);

#else

		throw functionIsDeprecatedError(ROOTFRAMEWORKLIB);

#endif

	}

}
void FairHitProducer::produceNew(TClonesArray* mvdHits, TClonesArray* stsHits, TClonesArray* mvdPoints, TClonesArray* stsPoints) {

	double                  xPosIn;
	double                  xPosOut;
	double                  yPosIn;
	double                  yPosOut;
	CbmStsPoint*            stsPoint;
	CbmMvdPoint*            mvdPoint;
	TVector3                position;
	TVector3                positionError;

	if (stsHits == NULL)
		throw cannotAccessHitsError();

	if (stsPoints == NULL)
		throw cannotAccessPointError();

	TClonesArray& stsHitsReference = *stsHits;

	for (int i = 0; i < stsPoints->GetEntries(); i++) {

		stsPoint = (CbmStsPoint*)stsPoints->At(i);
		if (stsPoint == NULL)
			throw cannotAccessPointError();

		xPosIn = stsPoint->GetX(0);
		yPosIn = stsPoint->GetY(0);

		produceHit(&xPosOut, &yPosOut, xPosIn, yPosIn);

		position.SetXYZ(xPosIn, yPosIn, stsPoint->GetZ());
		positionError.SetXYZ(xPosOut - xPosIn, yPosOut - yPosIn, 0);

#ifndef HITCOMPATIBILITY

		new(stsHitsReference[stsHits->GetEntries()]) CbmStsHit(stsPoint->GetDetectorID(), position, positionError, -1, -1, -1, -1, -1, -1);

#else

		throw functionIsDeprecatedError(ROOTFRAMEWORKLIB);

#endif

	}

	if (mvdHits == NULL)
		throw cannotAccessHitsError();

	if (mvdPoints == NULL)
		throw cannotAccessPointError();

	TClonesArray& mvdHitsReference = *mvdHits;

	for (int i = 0; i < mvdPoints->GetEntries(); i++) {

		mvdPoint = (CbmMvdPoint*)mvdPoints->At(i);
		if (mvdPoint == NULL)
			throw cannotAccessPointError();

		xPosIn = mvdPoint->GetX();
		yPosIn = mvdPoint->GetY();

		produceHit(&xPosOut, &yPosOut, xPosIn, yPosIn);

		position.SetXYZ(xPosIn, yPosIn, mvdPoint->GetZ());
		positionError.SetXYZ(xPosOut - xPosIn, yPosOut - yPosIn, 0);

#ifndef HITCOMPATIBILITY

		new(mvdHitsReference[mvdHits->GetEntries()]) CbmMvdHit(mvdPoint->GetDetectorID(), position, positionError, -1);

#else

		throw functionIsDeprecatedError(ROOTFRAMEWORKLIB);

#endif

	}

}

/****************************************************************
 * method produce a hit											*
 ****************************************************************/

void FairHitProducer::produceHit(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn) {

	if (xPosOut == NULL)
		throw cannotAccessPointError();

	if (yPosOut == NULL)
		throw cannotAccessPointError();

	switch (typeOfHitProducer) {

		case BINNINGHITPRODUCER:
			produceBin(xPosOut, yPosOut, xPosIn, yPosIn);
			break;

		case SMEARINGHITPRODUCER:
			produceSmear(xPosOut, yPosOut, xPosIn, yPosIn);
			break;

		default:
			produceNo(xPosOut, yPosOut, xPosIn, yPosIn);
			break;

	}

}
void FairHitProducer::produceHit(double* xPosOut, double* yPosOut, bool* isMapsType, bool* isHybridType, bool* isStripType, double xPosIn, double yPosIn, int stationId) {

	unsigned int   detectorStations;
	unsigned short detectorIndex;

	if (isMapsType == NULL)
		throw noDetectorTypeError();

	if (isHybridType == NULL)
		throw noDetectorTypeError();

	if (isStripType == NULL)
		throw noDetectorTypeError();

	if (detector == NULL)
		throw noDetectorError(ROOTFRAMEWORKLIB);

	produceHit(xPosOut, yPosOut, xPosIn, yPosIn);

	detectorStations = detector->getNumberOfActiveStations();
	detectorIndex    = detector->getStationById(stationId).getIndex();

	switch ((3 * detectorIndex) / detectorStations) {

		case 0:
			*isMapsType   = true;
			*isHybridType = false;
			*isStripType  = false;
			break;

		case 1:
			*isMapsType   = false;
			*isHybridType = true;
			*isStripType  = false;
			break;

		default:
			*isMapsType   = false;
			*isHybridType = false;
			*isStripType  = true;
			break;

	}

}
