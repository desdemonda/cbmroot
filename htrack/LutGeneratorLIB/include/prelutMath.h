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
///     - derived from prelut
///     - class for the first look-up-table implemented with math
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:37 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTMATH_H
#define _PRELUTMATH_H


#include "../../DataObjectLIB/include/analyticFormula.h"
#include "prelut.h"


/* **************************************************************
 * CLASS prelutMath								 				*
 * **************************************************************/

class prelutMath : public prelut {

protected:

	analyticFormula formula;		/**< This object implements the formulas which are used to generate the prelut. */

public:

/**
 * Default constructor
 */

	prelutMath();

/**
 * Constructor
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 */

	prelutMath(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);

/**
 * Destructor
 */

	virtual ~prelutMath();

/**
 * This method evaluates the value from the prelut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluate(trackfinderInputHit* hit, prelutHoughBorder* borderPointer = NULL);
	void evaluateV1(trackfinderInputHit* hit, prelutHoughBorder* borderPointer = NULL);
	void evaluateV2(trackfinderInputHit* hit, prelutHoughBorder* borderPointer = NULL);

};

#endif
