/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
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
//     - derived from lut
//     - class for the second look-up-table implemented with math
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:37 $
// $Revision: 1.11 $
//
// *******************************************************************/


#include "../../DataObjectLIB/include/magneticFieldValue.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/lutMath.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * method returns the magnetfield factor for the station		*
 ****************************************************************/

double lutMath::evaluateMagnetfieldFactor(trackfinderInputHit* hit) {

	double returnValue;

	if (magneticField != NULL)
		returnValue = magneticField->getFieldFactor(hit);
	else
		returnValue = magneticFieldFactor;

	return returnValue;

}


/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutMath::lutMath() 
  : lut(), 
    numberOfCorrections(0),
    numberOfCoordCorrections(0),
    magneticField(NULL),
    magneticFieldFactor(1),
    formula()
{
  /*
	resetCorrectionCounter();
	magneticField       = NULL;
	magneticFieldFactor = 1;
  */
}

/****************************************************************
 * constructor													*
 * Errors:														*
 * - memoryAllocationError										*
 ****************************************************************/

lutMath::lutMath(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) 
  : lut(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step), 
    numberOfCorrections(0),
    numberOfCoordCorrections(0),
    magneticField(NULL),
    magneticFieldFactor(1),
    formula()
{
  /*
	resetCorrectionCounter();
	magneticField       = NULL;
	magneticFieldFactor = 1;
  */
}

/****************************************************************
 * destructor													*
 ****************************************************************/

lutMath::~lutMath() {

}

/**
 * This method returns the magnetic field to use.
 */

trackfinderInputMagneticField* lutMath::getMagneticField() {

	return magneticField;

}

/**
 * This method returns the magnetic field factor to use instead of
 * the magneticField.
 */

double lutMath::getMagneticFieldFactor() {

	return magneticFieldFactor;

}

/****************************************************************
 * This method sets the magnetic field to use.					*
 ****************************************************************/

void lutMath::setMagneticField(trackfinderInputMagneticField* _magneticField) {

	if (this->magneticField != _magneticField) {

		this->magneticField       = _magneticField;
		this->magneticFieldFactor = 1;

	}

}

/****************************************************************
 * This method sets the magnetic field factor to use instead of	*
 * the magneticField.											*
 ****************************************************************/

void lutMath::setMagneticFieldFactor(double _magneticFieldFactor) {

	if (_magneticFieldFactor != 0) {

		if (this->magneticFieldFactor != _magneticFieldFactor) {

			this->magneticField       = NULL;
			this->magneticFieldFactor = _magneticFieldFactor;

		}

	}
	else
		throw magneticFieldFactorCannotBeZero();

}

/****************************************************************
 * This method returns the number of corrections.				*
 ****************************************************************/

unsigned long lutMath::getNumberOfCorrections() {

	return numberOfCorrections;

}

/****************************************************************
 * This method returns the number of coord corrections.			*
 ****************************************************************/

unsigned long lutMath::getNumberOfCoordCorrections() {

	return numberOfCoordCorrections;

}

/****************************************************************
 * This method resets both correction counters.					*
 ****************************************************************/

void lutMath::resetCorrectionCounter() {

	numberOfCorrections      = 0;
	numberOfCoordCorrections = 0;

}

/****************************************************************
 * This method realizes the lut table.							*
 ****************************************************************/

void lutMath::evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

//	evaluateHistogramBorderV1(hit, borderPointer);
	evaluateHistogramBorderV2(hit, borderPointer);

}
void lutMath::evaluateHistogramBorderV1(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

	double               posX;
	double               posZ;
	double               thetaIncr;
	double               q_p_xzIncr;
	double               theta;
	double               _magnetfieldFactor;
	double               q_p_xzPrev;
	double               q_p_xz;
	double               q_p_xzBorder;
	bool                 correction;
	lutHoughBorder*      actualBorderPointer;
	houghBorderPosition  actualHoughCoord;

	if (borderPointer != NULL)
		actualBorderPointer = borderPointer;
	else
		actualBorderPointer = &border;

#if (LUTVERSION > 2)

	houghBorderPosition  previousHoughCoord;
	bool                 houghCoordIsEmpty = true;

#endif

	if (hit == NULL)
		throw noHitError();

	posX              = hit->getPosX();
	posZ              = hit->getPosZ();
	thetaIncr         = (def.dim1Max  - def.dim1Min)  / def.dim1Step;
	q_p_xzIncr        = (def.dim2Max - def.dim2Min) / def.dim2Step;

	_magnetfieldFactor = evaluateMagnetfieldFactor(hit);

	correction        = true;
	q_p_xzPrev        = def.dim2Min;
	actualBorderPointer->houghCoord.clear();

	theta             = def.dim1Min;

	for (int i = 0; i < def.dim1Step; i++) {

		q_p_xz                = formula.evaluateLut(posX, posZ, theta, _magnetfieldFactor);

		actualHoughCoord.pos1 = i;
		q_p_xzBorder          = def.dim2Min;

		for (int j = 0; j < def.dim2Step; j++) {

			if (q_p_xzBorder < q_p_xz) {
				
				if (q_p_xzBorder >= q_p_xzPrev) {

					actualHoughCoord.pos2 = j;

#if (LUTVERSION > 2)

					/* the minimal corrected slope for the entries must be 45 degree */
					if (!houghCoordIsEmpty) {
					
						if (i - previousHoughCoord.pos1 > 1) {

							actualHoughCoord.pos1 = previousHoughCoord.pos1 + 1;
							numberOfCoordCorrections++;

							if (correction) {

								numberOfCorrections++;
								correction = false;

							}

						}
						else
							actualHoughCoord.pos1 = i;

					}

					houghCoordIsEmpty       = false;
					previousHoughCoord.pos1 = actualHoughCoord.pos1;
					previousHoughCoord.pos2 = actualHoughCoord.pos2;

#endif

					actualBorderPointer->houghCoord.push(actualHoughCoord);

				}

			}

			q_p_xzBorder += q_p_xzIncr;

		}

		q_p_xzPrev        = q_p_xz;
		theta            += thetaIncr;

	}

}
void lutMath::evaluateHistogramBorderV2(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

	unsigned short       j;
	unsigned short       pos2Start;
	double               posX;
	double               posZ;
	double               thetaIncr;
	double               q_p_xzIncr;
	double               _magnetfieldFactor;
	double               theta;
	double               q_p_xz;
	double               q_p_xz_previous;
	unsigned short       actualPos2;
	lutHoughBorder*      actualBorderPointer;
	houghBorderPosition  actualHoughCoord;
	houghBorderPosition  previousHoughCoord;
	bool                 houghCoordPos2IsEmpty = true;

	if (borderPointer != NULL)
		actualBorderPointer = borderPointer;
	else
		actualBorderPointer = &border;

#if (LUTVERSION == 1)

	pos2Start = 0;

#else

	pos2Start = 1;

#endif

#if (LUTVERSION > 2)

	bool                 correction            = true;

#endif

	if (hit == NULL)
		throw noHitError();

	posX              = hit->getPosX();
	posZ              = hit->getPosZ();
	thetaIncr         = (def.dim1Max  - def.dim1Min)  / (def.dim1Step - 1);
	q_p_xzIncr        = (def.dim2Max - def.dim2Min) / (def.dim2Step - 1);

	_magnetfieldFactor = evaluateMagnetfieldFactor(hit);
	theta             = def.dim1Min;
	q_p_xz_previous   = def.dim2Min - 1;

	if (_magnetfieldFactor == 0)
		throw magneticFieldFactorCannotBeZero();

	actualBorderPointer->houghCoord.clear();

	for (int i = 0; i < def.dim1Step; i++) {

		/* compute the value in the second dimension according to the value of the first dimension */
		q_p_xz = formula.evaluateLut(posX, posZ, theta, _magnetfieldFactor);

		/* the value of the second dimension must be in the range */
		if ((def.dim2Min <= q_p_xz) && (q_p_xz <= def.dim2Max)) {

			/* compute the position of the value of the first dimension */
			actualHoughCoord.pos1 = i;
			/* compute the position of the value of the second dimension */
			actualPos2            = (unsigned short)(((q_p_xz - def.dim2Min) / q_p_xzIncr) + 0.0);

#if (LUTVERSION > 2)

			/* check for not possible entries in hardware and correct them */
			if (!houghCoordPos2IsEmpty) {

				/* the last cell in dim2 dimension is set and we can exit now */
				if (previousHoughCoord.pos2 == def.dim2Step - 1)
					break;

				/* check if the next dim1 entry would be empty */
				if (previousHoughCoord.pos2 >= actualPos2) {

					/* if the dim1 entry would be empty set one cell with 45 degree */
					actualPos2 = previousHoughCoord.pos2 + 1;

					numberOfCoordCorrections++;
						
					if (correction) {

						numberOfCorrections++;
						correction = false;

					}

				}

			}

#endif

			/* do the first entry */
			if (houghCoordPos2IsEmpty) {

				actualHoughCoord.pos2   = 0;
				previousHoughCoord.pos2 = 0;
				actualBorderPointer->houghCoord.push(actualHoughCoord);
				houghCoordPos2IsEmpty   = false;

			}

			/* do all entries for the computed one to the last one */
			for (j = previousHoughCoord.pos2 + pos2Start; j <= min(actualPos2, def.dim2Step - 1); j++) {

				actualHoughCoord.pos2 = j;
				actualBorderPointer->houghCoord.push(actualHoughCoord);

			}

			previousHoughCoord.pos2 = actualHoughCoord.pos2;

		}
		else if ((def.dim2Min > q_p_xz_previous) && (q_p_xz > def.dim2Max)) {

			/* compute the position of the value of the first dimension */
			actualHoughCoord.pos1  = i;
			/* compute the position of the value of the second dimension */
			actualPos2             = (unsigned short)((((q_p_xz + q_p_xz_previous) / 2 - def.dim2Min) / q_p_xzIncr) + 0.0);;

		}

		theta                     += thetaIncr;
		q_p_xz_previous            = q_p_xz;

	}

	if (houghCoordPos2IsEmpty) {

		/* do all entries for the computed one to the last one, if the q_p_xz range is to small */
		for (j = 0; j <= min(actualPos2, def.dim2Step - 1); j++) {

			previousHoughCoord.pos2 = j;
			actualHoughCoord.pos2   = j;
			actualBorderPointer->houghCoord.push(actualHoughCoord);

		}

		houghCoordPos2IsEmpty   = false;

	}

	/* finalize the entries if there is at least one */
	if (!houghCoordPos2IsEmpty) {

		/*
		 * set the entries to the next position in the first dimension
		 * while assuming that the next computed entry would be there
		 * if it is not out of the range of the second dimension. Do
		 * not do entries if the entry would be out of the range of
		 * the first dimension.
		 */
		if (actualHoughCoord.pos1 + 1 < def.dim1Step) {

			actualHoughCoord.pos1++;

			for (j = previousHoughCoord.pos2 + pos2Start; j < def.dim2Step; j++) {

				actualHoughCoord.pos2 = j;
				actualBorderPointer->houghCoord.push(actualHoughCoord);

			}

		}

	}

}
