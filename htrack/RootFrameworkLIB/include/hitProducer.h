//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - realizes a dummy for building hits from points
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:38:40 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _CBMHITPRODUCER_H
#define _CBMHITPRODUCER_H


#include "../../DataRootObjectLIB/include/trackfinderInputDetector.h"
#include "TClonesArray.h"


/**
 * Definition of the id used for such a hit-producer
 */
#define NONEHITPRODUCER          0
#define BINNINGHITPRODUCER       1
#define SMEARINGHITPRODUCER      2


/* **************************************************************
 * CLASS FairHitProducer							 				*
 * **************************************************************/

class FairHitProducer {

 private:

  FairHitProducer(const FairHitProducer&);
  FairHitProducer& operator=(const FairHitProducer&);

protected:

	int                            typeOfHitProducer;	/**< Variable stores the type of the hitproducer */
	trackfinderInputDetector*      detector;			/**< Object to store the detector for this event. */

/**
 * method produces the hits with nothing
 * @param xPosIn is the position in the x-dimension
 * @param yPosIn is the position in the y-dimension
 * @return xPosOut is the new computed position in x-dimension
 * @return yPosOut is the new computed position in y-dimension
 */

	void produceNo(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn);

/**
 * method produces the hits with binning
 * @param xPosIn is the position in the x-dimension
 * @param yPosIn is the position in the y-dimension
 * @return xPosOut is the new computed position in x-dimension
 * @return yPosOut is the new computed position in y-dimension
 */

	void produceBin(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn);

/**
 * method produces the hits with smearing
 * @param xPosIn is the position in the x-dimension
 * @param yPosIn is the position in the y-dimension
 * @return xPosOut is the new computed position in x-dimension
 * @return yPosOut is the new computed position in y-dimension
 */

	void produceSmear(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn);

public:

/**
 * Default constructor
 */

	FairHitProducer();

/**
 * Constructor
 * @param typeOfHitProducer is the type of the hitproducer
 */

	FairHitProducer(int hitProducer, trackfinderInputDetector* actualDetector);

/**
 * Destructor
 */

	virtual ~FairHitProducer();

/**
 * method sets the hitProducerType
 */

	void setHitProducerType(int hitProducer);

/**
 * method sets the detector
 * @param actualDetector is an object which contains information about each detector station
 */

	void setDetector(trackfinderInputDetector* actualDetector);

/**
 * method produces the hits
 * @param points is a pointer to the point-objects
 * @param mvdPoints is a pointer to the point-objects
 * @param stsPoints is a pointer to the point-objects
 * @return mapsHits is a pointer to the maps-hit-object
 * @return stripHits is a pointer to the strip-hit-object
 * @return hybridHits is a pointer to the hybrid-hit-object
 * @return stsHits is a pointer to the hit-object
 * @return mvdHits is a pointer to the hit-object
 */

	void produceOld(TClonesArray* mapsHits, TClonesArray* stripHits, TClonesArray* hybridHits, TClonesArray* points);
	void produceNew(TClonesArray* mvdHits, TClonesArray* stsHits, TClonesArray* mvdPoints, TClonesArray* stsPoints);

/**
 * method produce a hit
 * @param xPosIn is the position in the x-dimension
 * @param yPosIn is the position in the y-dimension
 * @return xPosOut is the new computed position in x-dimension
 * @return yPosOut is the new computed position in y-dimension
 * @param isMapsType defines if the hit is a maps-Hit
 * @param isHybridType defines if the hit is a hybrid-Hit
 * @param isStripType defines if the hit is a strip-Hit
 */

	void produceHit(double* xPosOut, double* yPosOut, double xPosIn, double yPosIn);
	void produceHit(double* xPosOut, double* yPosOut, bool* isMapsType, bool* isHybridType, bool* isStripType, double xPosIn, double yPosIn, int stationId);

};

#endif
