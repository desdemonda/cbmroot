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
///     - implements methods to display the prelut ranges analysis results
///       with regard to the range for the main layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTRANGEMAINLAYERANALYSIS_H
#define _PRELUTRANGEMAINLAYERANALYSIS_H


#include "prelutRangeAnalysis.h"


/* **************************************************************
 * CLASS prelutRangeMainLayerAnalysis							*
 * **************************************************************/

class prelutRangeMainLayerAnalysis : public prelutRangeAnalysis {

protected:

/**
 * This method returns the display name
 */

	std::string getDisplayName();

/**
 * This method returns the display title
 */

	std::string getDisplayTitle();

/**
 * This method returns the window name
 */

	std::string getWindowName();

/**
 * This method returns the window name
 */

	std::string getWindowTitle();

public:

/**
 * Default constructor
 */

	prelutRangeMainLayerAnalysis();

/**
 * Constructor
 * @param prelutRangeCut is the percentage of occuring tracks which must be reached to choose the min/max combination
 * @param numberOfDisplays is the number of different tests
 * @param numberOfMinFactors is the number of different minimas which should be tested
 * @param factorMinMin is the minimal value for the minimum
 * @param factorMinMax is the maximal value for the minimum
 * @param numberOfMaxFactors is the number of different maximas which should be tested
 * @param factorMaxMin is the minimal value for the maximum
 * @param factorMaxMax is the maximal value for the maximum
 */

	prelutRangeMainLayerAnalysis(unsigned short numberOfDisplays);
	prelutRangeMainLayerAnalysis(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);
	prelutRangeMainLayerAnalysis(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);

/**
 * Destructor
 */

	virtual ~prelutRangeMainLayerAnalysis();

};

#endif
