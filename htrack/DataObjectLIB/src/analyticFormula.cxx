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
//     - class for computing the analytic formula
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:43 $
// $Revision: 1.14 $
//
// *******************************************************************/


#include "../include/dataObjectError.h"
#include "../include/analyticFormula.h"
#include <math.h>
#include <stdio.h>


#define sqr(a)  ((a) * (a)) 

#define mToCm   (double)100
#define TToKG   (double)10
#define vlight  (double)0.299792458


/****************************************************************
 * Default constructor											*
 ****************************************************************/

analyticFormula::analyticFormula() {

}

/****************************************************************
 * destructor													*
 ****************************************************************/

analyticFormula::~analyticFormula() {

}

/****************************************************************
 * method returns the absolute value of the momentum.			*
 ****************************************************************/

double analyticFormula::evaluateAbsP(trackMomentum& momentum) {

	double returnValue;

	returnValue  = sqr(momentum.get(PX));
	returnValue += sqr(momentum.get(PY));
	returnValue += sqr(momentum.get(PZ));

	returnValue  = sqrt(returnValue);

	return returnValue;

}

/****************************************************************
 * method returns the absolute value of the transversal			*
 * momentum.													*
 ****************************************************************/

double analyticFormula::evaluateAbsPt(trackMomentum& momentum) {

	double returnValue;

	returnValue  = sqr(momentum.get(PX));
	returnValue += sqr(momentum.get(PY));

	returnValue = sqrt(returnValue);

	return returnValue;

}

/****************************************************************
 * method returns the momentum components in each direction.	*
 ****************************************************************/

void analyticFormula::evaluateP(trackCoordinates& coordinates, histogramSpace& space, trackMomentum* momentum, double* charge) {

	trackParameter parameter;

	parameter.set(space.getAnalogFromCell(coordinates.get(DIM1), DIM1), DIM1);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM2), DIM2), DIM2);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM3), DIM3), DIM3);

	evaluateP(parameter, space, momentum, charge);

}
void analyticFormula::evaluateP(trackCoordinates& coordinates, histogramSpace& space, trackFrameworkMomentum* momentum, double* charge) {

	trackParameter parameter;

	parameter.set(space.getAnalogFromCell(coordinates.get(DIM1), DIM1), DIM1);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM2), DIM2), DIM2);
	parameter.set(space.getAnalogFromCell(coordinates.get(DIM3), DIM3), DIM3);

	evaluateP(parameter, space, momentum, charge);

}
void analyticFormula::evaluateP(trackParameter& parameter, histogramSpace& space, trackMomentum* momentum, double* charge) {

	double q_p_xz;
	double value;

	if (momentum != NULL) {

		q_p_xz = parameter.get(HRADIUS);

		/*
		 * If q_p_xz is zero, there would be a track which has a momentum in z
		 * direction which is so big that the quantization in the Hough space
		 * is not big enough. So this track is found in the Hough space at a
		 * coordinate representing an infinite momentum in z direction.
		 * Ordinary this infinite coordinate in the Hough space just exists, if
		 * the quantization is chosen in the way that there is a cell
		 * representing zero. If one adds just one to the chosen quatization,
		 * there are two cells with 0+epsilon and 0-epsilon instead of an
		 * infinite cell. So q_p_xz can never be zero. But a solution for this
		 * problem is to give such a track a momentum which is not infinite
		 * but at least twice times higher than the highest momentumg which
		 * lies in this cell when transforming.
		 */
		if (q_p_xz == 0)
			q_p_xz = space.getIncr(HRADIUS) / 2;
		
		if (q_p_xz < 0) {

			if (charge != NULL)
				*charge = -1;
			q_p_xz = -q_p_xz;

		}
		else
			if (charge != NULL)
				*charge = +1;

		value = 1.0 / (q_p_xz * sqrt(sqr(tan(parameter.get(HTHETA))) + 1));
		momentum->set(value, MRADIUS);

		value = tan(parameter.get(HGAMMA)) * momentum->get(MRADIUS);
		momentum->set(value, MGAMMA);

		value = tan(parameter.get(HTHETA)) * momentum->get(MRADIUS);
		momentum->set(value, MTHETA);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateP(trackParameter& parameter, histogramSpace& space, trackFrameworkMomentum* momentum, double* charge) {

	double q_p_xz;
	double sign;
	double value;

	if (momentum != NULL) {

		q_p_xz = parameter.get(HRADIUS);
		
		/*
		 * If q_p_xz is zero, there would be a track which has a momentum in z
		 * direction which is so big that the quantization in the Hough space
		 * is not big enough. So this track is found in the Hough space at a
		 * coordinate representing an infinite momentum in z direction.
		 * Ordinary this infinite coordinate in the Hough space just exists, if
		 * the quantization is chosen in the way that there is a cell
		 * representing zero. If one adds just one to the chosen quatization,
		 * there are two cells with 0+epsilon and 0-epsilon instead of an
		 * infinite cell. So q_p_xz can never be zero. But a solution for this
		 * problem is to give such a track a momentum which is not infinite
		 * but at least twice times higher than the highest momentumg which
		 * lies in this cell when transforming.
		 */
		if (q_p_xz == 0)
			q_p_xz = space.getIncr(HRADIUS) / 2;
		
		if (q_p_xz < 0) {

			sign   = -1;
			q_p_xz = -q_p_xz;

		}
		else
			sign   = +1;

		if (charge != NULL)
			*charge = sign;

		value = tan(parameter.get(HTHETA));
		momentum->setTx(value);

		value = tan(parameter.get(HGAMMA));
		momentum->setTy(value);

		value = sqrt(sqr(tan(parameter.get(HTHETA))) + sqr(tan(parameter.get(HGAMMA))) + 1) / (q_p_xz * (sqrt(sqr(tan(parameter.get(HTHETA))) + 1)));
		momentum->setQp(sign * value);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluatePWithCare(trackParameter& parameter, double care, trackMomentum* momentum, double* charge) {

	double q_p_xz;
	double value;

	if (momentum != NULL) {

		q_p_xz = parameter.get(HRADIUS);

		/*
		 * If q_p_xz is zero, there would be a track which has a momentum in z
		 * direction which is so big that the quantization in the Hough space
		 * is not big enough. So this track is found in the Hough space at a
		 * coordinate representing an infinite momentum in z direction.
		 * Ordinary this infinite coordinate in the Hough space just exists, if
		 * the quantization is chosen in the way that there is a cell
		 * representing zero. If one adds just one to the chosen quatization,
		 * there are two cells with 0+epsilon and 0-epsilon instead of an
		 * infinite cell. So q_p_xz can never be zero. But a solution for this
		 * problem is to give such a track a momentum which is not infinite
		 * but at least very high.
		 */
		if (q_p_xz == 0)
			q_p_xz = care;
		
		if (q_p_xz < 0) {

			if (charge != NULL)
				*charge = -1;
			q_p_xz = -q_p_xz;

		}
		else
			if (charge != NULL)
				*charge = +1;

		value = 1.0 / (q_p_xz * sqrt(sqr(tan(parameter.get(HTHETA))) + 1));
		momentum->set(value, MRADIUS);

		value = tan(parameter.get(HGAMMA)) * momentum->get(MRADIUS);
		momentum->set(value, MGAMMA);

		value = tan(parameter.get(HTHETA)) * momentum->get(MRADIUS);
		momentum->set(value, MTHETA);

	}
	else
		throw resultPointerIsNotAccessibleError();

}

/****************************************************************
 * method returns the computed values for the hough				*
 * cell based on the given momentum								*
 ****************************************************************/

void analyticFormula::evaluateHough(trackMomentum& momentum, double charge, histogramSpace& space, trackCoordinates* coordinates) {

	trackParameter parameter;

	if (coordinates != NULL) {

		evaluateHough(momentum, charge, &parameter);

		coordinates->set(space.getCellFromAnalog(parameter.get(DIM1), DIM1), DIM1);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM2), DIM2), DIM2);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM3), DIM3), DIM3);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateHough(trackFrameworkMomentum& momentum, histogramSpace& space, trackCoordinates* coordinates) {

	trackParameter parameter;

	if (coordinates != NULL) {

		evaluateHough(momentum, &parameter);

		coordinates->set(space.getCellFromAnalog(parameter.get(DIM1), DIM1), DIM1);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM2), DIM2), DIM2);
		coordinates->set(space.getCellFromAnalog(parameter.get(DIM3), DIM3), DIM3);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateHough(trackMomentum& momentum, double charge, trackParameter* parameter) {

	double value;

	if (parameter != NULL) {

		value = atan(momentum.get(MTHETA) / momentum.get(MRADIUS));
		parameter->set(value, HTHETA);

		value = atan(momentum.get(MGAMMA) / momentum.get(MRADIUS));
		parameter->set(value, HGAMMA);

		value = (- charge) / (momentum.get(MRADIUS) * sqrt(sqr(momentum.get(MTHETA) / momentum.get(MRADIUS)) + 1));
		parameter->set(value, HRADIUS);

	}
	else
		throw resultPointerIsNotAccessibleError();

}
void analyticFormula::evaluateHough(trackFrameworkMomentum& momentum, trackParameter* parameter) {

	double value;

	if (parameter != NULL) {

		value = atan(momentum.getTx());
		parameter->set(value, HTHETA);

		value = atan(momentum.getTy());
		parameter->set(value, HGAMMA);

		value = sqrt(1 + (sqr(momentum.getTy()) / (sqr(momentum.getTx()) + 1))) / momentum.getQp();
		parameter->set(value, HRADIUS);

	}
	else
		throw resultPointerIsNotAccessibleError();

}

/****************************************************************
 * This formula implements the computation neccessary for the	*
 * prelut														*
 ****************************************************************/

double analyticFormula::evaluatePrelut(double yPos, double zPos, double gamma) {

	double sinGamma;
	double cosGamma;
	double m;

	sinGamma = sin(gamma);
	cosGamma = cos(gamma);

	m        =  ((mToCm * 2) * (zPos * sinGamma - yPos * cosGamma))
			 /	sqr(yPos * sinGamma + zPos * cosGamma);

	return m;

}

/****************************************************************
 * This formula implements the computation neccessary for the	*
 * lut															*
 ****************************************************************/

double analyticFormula::evaluateLut(double xPos, double zPos, double theta, double bField) {

	double sinTheta;
	double cosTheta;
	double q_p_xz;

	sinTheta = sin(theta);
	cosTheta = cos(theta);

	q_p_xz   = (((mToCm * TToKG / vlight) * 2) * (zPos * sinTheta - xPos * cosTheta))
		     / (bField * sqr(xPos * sinTheta + zPos * cosTheta));

	return q_p_xz;

}

/****************************************************************
 * This formula implements the computation of the original		*
 ****************************************************************/

double analyticFormula::evaluateFormulaPrelut(double layerFactor, double momentumY, double momentumZ, double zPos) {

	double sinGamma;
	double cosGamma;
	double constant;
	double p_2;
	double q;
	double y;

	sinGamma = sin(atan(momentumY / momentumZ));
	cosGamma = cos(atan(momentumY / momentumZ));
	constant = mToCm * 2;
	p_2      = (cosGamma / (2 * sinGamma)) * (zPos + (constant / (layerFactor * sinGamma)));
	q        = (zPos / sinGamma) * (((zPos * sqr(cosGamma)) / sinGamma) - (constant / layerFactor));

	if (p_2 > 0)
		y    = - p_2 + sqrt(sqr(p_2) - q);
	else
		y    = - p_2 - sqrt(sqr(p_2) - q);

	return y;

}

/****************************************************************
 * This formula implements the computation of the original		*
 * formula in the coordinate space.								*
 ****************************************************************/

double analyticFormula::evaluateFormulaLut(double numberOfElementaryCharges, double momentumX, double momentumZ, double zPos, double bField) {

	double sinTheta;
	double cosTheta;
	double bQ_p_xz;
	double constant;
	double p_2;
	double q;
	double x;

	sinTheta = sin(atan(momentumX / momentumZ));
	cosTheta = cos(atan(momentumX / momentumZ));
	bQ_p_xz  = bField * (numberOfElementaryCharges / sqrt(sqr(momentumX) + sqr(momentumZ)));
	constant = (mToCm * TToKG / vlight) * 2;
	p_2      = (cosTheta / (2 * sinTheta)) * (zPos - (constant / (bQ_p_xz * sinTheta)));
	q        = (zPos / sinTheta) * (((zPos * sqr(cosTheta)) / sinTheta) + (constant / bQ_p_xz));

	if (p_2 > 0)
		x    = - p_2 + sqrt(sqr(p_2) - q);
	else
		x    = - p_2 - sqrt(sqr(p_2) - q);

	return x;

}
