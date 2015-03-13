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
///     - derived from lut
///     - class for the second look-up-table implemented with math
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:37 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTMATH_H
#define _LUTMATH_H


#include "../../DataObjectLIB/include/analyticFormula.h"
#include "lut.h"


/* **************************************************************
 * CLASS lutMath								 				*
 * **************************************************************/

class lutMath : public lut {

 private:

  lutMath(const lutMath&);
  lutMath& operator=(const lutMath&);

protected:

	unsigned long                  numberOfCorrections;			/**< Variable stores the number of corrections, if at least one coordinate is corrected. */
	unsigned long                  numberOfCoordCorrections;	/**< Variable stores the number of all coordinate corrections. */
	trackfinderInputMagneticField* magneticField;				/**< Object to store the field map for this event. */
	double                         magneticFieldFactor;			/**< Variable to store the factor for using instead of the magneticField. */
	analyticFormula                formula;						/**< This object implements the formulas which are used to generate the lut. */

/**
 * method returns the magnetfield factor for the station
 * @param hit is the actual hit-object to compute the magnetfieldfactor
 */

	double evaluateMagnetfieldFactor(trackfinderInputHit* hit);

public:

/**
 * Default constructor
 */

	lutMath();

/**
 * constructor
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	lutMath(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * destructor
 */

	virtual ~lutMath();

/**
 * This method returns the magnetic field to use.
 */

	trackfinderInputMagneticField* getMagneticField();

/**
 * This method returns the magnetic field factor to use instead of
 * the magneticField.
 */

	double getMagneticFieldFactor();

/**
 * This method sets the magnetic field to use.
 */

	void setMagneticField(trackfinderInputMagneticField* magneticField);

/**
 * This method sets the magnetic field factor to use instead of
 * the magneticField.
 */

	void setMagneticFieldFactor(double magneticFieldFactor);

/**
 * This method returns the number of corrections.
 */

	unsigned long getNumberOfCorrections();

/**
 * This method returns the number of coord corrections.
 */

	unsigned long getNumberOfCoordCorrections();

/**
 * This method resets both correction counters.
 */

	void resetCorrectionCounter();

/**
 * This method realizes the lut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer = NULL);
	void evaluateHistogramBorderV1(trackfinderInputHit* hit, lutHoughBorder* borderPointer = NULL);
	void evaluateHistogramBorderV2(trackfinderInputHit* hit, lutHoughBorder* borderPointer = NULL);

};

#endif
