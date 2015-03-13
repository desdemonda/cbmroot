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
//     - class for computing the analytic formula
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:40 $
/// $Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYTICFORMULA_H
#define _ANALYTICFORMULA_H


#include "histogramSpace.h"
#include "trackCoordinates.h"
#include "trackParameter.h"
#include "trackMomentum.h"
#include "trackFrameworkMomentum.h"


/* **************************************************************
 * CLASS analyticFormula						 				*
 * **************************************************************/

class analyticFormula {

public:

/**
 * Default constructor
 */

	analyticFormula();

/**
 * destructor
 */

	virtual ~analyticFormula();

/**
 * method returns the absolute value of the momentum.
 * @param momentum is the vector of the momentum components in each direction
 */

	double evaluateAbsP(trackMomentum& momentum);

/**
 * method returns the absolute value of the transversal momentum.
 * @param momentum is the vector of the momentum components in each direction
 */

	double evaluateAbsPt(trackMomentum& momentum);

/**
 * method returns the momentum components in each direction.
 * @param coordinates is the vector of the coordinates for the cell in the hough space
 * @param space defines the range of the histogram's space
 * @param parameter is the vector of the parameters for the cell in the hough space
 * @param xPos is the input y-position for the formula
 * @param yPos is the input y-position for the formula
 * @param zPos is the input y-position for the formula
 * @param bField is the input magnetic field value for the formula
 * @return momentum is the vector of the momentum components in each direction
 * @return charge is the value for the charge. Here it can be just +1 or -1
 */

	void evaluateP(trackCoordinates& coordinates, histogramSpace& space, trackMomentum* momentum, double* charge = NULL);
	void evaluateP(trackCoordinates& coordinates, histogramSpace& space, trackFrameworkMomentum* momentum, double* charge = NULL);
	void evaluateP(trackParameter& parameter, histogramSpace& space, trackMomentum* momentum, double* charge = NULL);
	void evaluateP(trackParameter& parameter, histogramSpace& space, trackFrameworkMomentum* momentum, double* charge = NULL);
	void evaluatePWithCare(trackParameter& parameter, double care, trackMomentum* momentum, double* charge = NULL);

/**
 * method returns the computed values for the hough
 * cell based on the given momentum
 * @param momentum is the vector of the momentum components in each direction
 * @param charge is the charge for this particle
 * @param space defines the range of the histogram's space
 * @return coordinates is the vector of the coordinates for the cell in the hough space
 * @return parameter is the vector of the parameters for the cell in the hough space
 */

	void evaluateHough(trackMomentum& momentum, double charge, histogramSpace& space, trackCoordinates* coordinates);
	void evaluateHough(trackFrameworkMomentum& momentum, histogramSpace& space, trackCoordinates* coordinates);
	void evaluateHough(trackMomentum& momentum, double charge, trackParameter* parameter);
	void evaluateHough(trackFrameworkMomentum& momentum, trackParameter* parameter);

/**
 * This formula implements the computation neccessary for the prelut
 * @param yPos is the input y-position for the formula
 * @param zPos is the input y-position for the formula
 * @param gamma is the input angle for the formula
 * @return The returning value is the result m of the computation
 */

	double evaluatePrelut(double yPos, double zPos, double gamma);

/**
 * This formula implements the computation neccessary for the lut
 * @param xPos is the input y-position for the formula
 * @param zPos is the input y-position for the formula
 * @param theta is the input angle for the formula
 * @param bField is the input magnetic field value for the formula
 * @return The returning value is the result m of the computation
 */

	double evaluateLut(double xPos, double zPos, double theta, double bField);

/**
 * This formula implements the computation of the original
 * formula in the coordinate space.
 * @param layerFactor is the value of the number of overlappings for the formula
 * @param momentumX is the value of the momentum in y direction for the formula
 * @param momentumZ is the value of the momentum in z direction for the formula
 * @param zPos is the input z-position for the formula
 * @return The returning value is the result y coordinate of the computation
 */

	double evaluateFormulaPrelut(double layerFactor, double momentumY, double momentumZ, double zPos);

/**
 * This formula implements the computation of the original
 * formula in the coordinate space.
 * @param numberOfElementaryCharges is the value of the number of elementary charges for the formula
 * @param momentumX is the value of the momentum in x direction for the formula
 * @param momentumZ is the value of the momentum in z direction for the formula
 * @param zPos is the input z-position for the formula
 * @param bField is the input magnetic field value for the formula
 * @return The returning value is the result x coordinate of the computation
 */

	double evaluateFormulaLut(double numberOfElementaryCharges, double momentumX, double momentumZ, double zPos, double bField);

};

#endif
