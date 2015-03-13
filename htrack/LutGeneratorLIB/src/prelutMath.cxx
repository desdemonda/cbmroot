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
//     - derived from prelut
//     - class for the first look-up-table implemented with math
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:37 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/lutGeneratorError.h"
#include "../include/prelutMath.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutMath::prelutMath() 
  : prelut(),
    formula()
{

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutMath::prelutMath(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) 
  : prelut(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry),
    formula()
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutMath::~prelutMath() {

}

/****************************************************************
 * This method evaluates the value from the prelut table.		*
 ****************************************************************/

void prelutMath::evaluate(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	evaluateV1(hit, borderPointer);

}
void prelutMath::evaluateV1(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	double posY;
	double posZ;
	double gammaIncr;
	double gamma;
	double m;
	bool   startFound = false;

	if (hit == NULL)
		throw noHitError();

	posY       = hit->getPosY();
	posZ       = hit->getPosZ();
	gammaIncr  = (def.dim3Max - def.dim3Min) / def.dim3Step;

	if (borderPointer != NULL) {
		borderPointer->start = 0;
		borderPointer->stop  = 0;
	}
	else {
		border.start = 0;
		border.stop  = 0;
	}

	for (int i = 0; i < def.dim3Step; i++) {
		
		gamma = i * gammaIncr + def.dim3Min;
		m     = formula.evaluatePrelut(posY, posZ, gamma);

		if ((m >= def.dim3StartEntry) && (startFound == false)) {
			startFound = true;
			if (borderPointer != NULL) {
				borderPointer->start = i;
				borderPointer->stop  = i;
			}
			else {
				border.start = i;
				border.stop  = i;
			}
		}
		if (m <= def.dim3StopEntry) {
			if (startFound == true) {
				if (borderPointer != NULL) {
					borderPointer->stop  = i;
				}
				else {
					border.stop = i;
				}
			}
		}

	}

}
void prelutMath::evaluateV2(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	prelutHoughBorder* actualBorderPointer;
	double             posY;
	double             posZ;
	double             gammaIncr;
	double             gamma;
	double             formulaM;
	double             m;
	double             mPrev;
	bool               startFound;

	if (hit == NULL)
		throw noHitError();

	if (borderPointer != NULL)
		actualBorderPointer = borderPointer;
	else
		actualBorderPointer = &border;

	actualBorderPointer->start = 0;
	actualBorderPointer->stop  = 0;

	posY      = hit->getPosY();
	posZ      = hit->getPosZ();
	gammaIncr = (def.dim3Max - def.dim3Min) / (def.dim3Step - 1);

	startFound = false;

	for (int i = 0; i < def.dim3Step; i++) {
		
		gamma    = i * gammaIncr + def.dim3Min;
		formulaM = formula.evaluatePrelut(posY, posZ, gamma);

		if (i == 0)
			mPrev = formulaM;
		else
			mPrev = m;
	
		m        = formulaM;

		/* Set the start value if the minimum of the range is reached */
		if ((m >= def.dim3StartEntry) && (startFound == false)) {
			startFound                 = true;
			actualBorderPointer->start = i;
		}
		/* Set the stop value */
		if (startFound == true) {
			/* Set the value if the radius is in the range */
			if (m <= def.dim3StopEntry) {
				/* Correct the value if just one radius is in the range */
				if (actualBorderPointer->start == i) {
					actualBorderPointer->start = i - 1;
					actualBorderPointer->stop  = i + 1;
				}
				else
					actualBorderPointer->stop  = i;
			}
			else {
				/* Correct the value if no radius is in the range */
				if (mPrev < def.dim3StartEntry) {
					actualBorderPointer->start = i - 1;
					actualBorderPointer->stop  = i + 1;
				}
			}
		}

	}

}
