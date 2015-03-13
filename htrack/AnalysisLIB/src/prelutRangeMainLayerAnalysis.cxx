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
/// $Date: 2008-08-14 12:29:27 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/prelutRangeAnalysisDef.h"
#include "../include/prelutRangeMainLayerAnalysis.h"


/* **************************************************************
 * CLASS prelutRangeMainLayerAnalysis							*
 * **************************************************************/

/****************************************************************
 * This method returns the display name							*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getDisplayName() {

	return PRELUTRANGEMAINLAYERNAME;

}

/****************************************************************
 * This method returns the display title						*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getDisplayTitle() {

	return RELUTRANGEMAINLAYERTITLE;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getWindowName() {

	return PRELUTRANGEMAINLAYERWINDOWNAME;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeMainLayerAnalysis::getWindowTitle() {

	return PRELUTRANGEMAINLAYERWINDOWTITLE;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis() : prelutRangeAnalysis() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis(unsigned short _numberOfDisplays) : prelutRangeAnalysis(_numberOfDisplays) {

}
prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis(unsigned short _prelutRangeCut, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) : prelutRangeAnalysis(_prelutRangeCut, _numberOfMinFactors, _factorMinMin, _factorMinMax, _numberOfMaxFactors, _factorMaxMin, _factorMaxMax) {

}
prelutRangeMainLayerAnalysis::prelutRangeMainLayerAnalysis(unsigned short _prelutRangeCut, unsigned short _numberOfDisplays, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) : prelutRangeAnalysis(_prelutRangeCut, _numberOfDisplays, _numberOfMinFactors, _factorMinMin, _factorMinMax, _numberOfMaxFactors, _factorMaxMin, _factorMaxMax) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutRangeMainLayerAnalysis::~prelutRangeMainLayerAnalysis() {

}
