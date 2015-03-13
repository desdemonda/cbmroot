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
///       with regard to the range for the correct layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:27 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/prelutRangeAnalysisDef.h"
#include "../include/prelutRangeCorrectLayerAnalysis.h"


/* **************************************************************
 * CLASS prelutRangeCorrectLayerAnalysis						*
 * **************************************************************/

/****************************************************************
 * This method returns the display name							*
 ****************************************************************/

std::string prelutRangeCorrectLayerAnalysis::getDisplayName() {

	return PRELUTRANGECORRETLAYERNAME;

}

/****************************************************************
 * This method returns the display title						*
 ****************************************************************/

	std::string prelutRangeCorrectLayerAnalysis::getDisplayTitle() {

	return PRELUTRANGECORRECTLYERTITLE;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeCorrectLayerAnalysis::getWindowName() {

	return PRELUTRANGECORRECTLAYERWINDOWNAME;

}

/****************************************************************
 * This method returns the window name							*
 ****************************************************************/

std::string prelutRangeCorrectLayerAnalysis::getWindowTitle() {

	return PRELUTRANGECORRECTLAYERWINDOWTITLE;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis() : prelutRangeAnalysis() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis(unsigned short _numberOfDisplays) : prelutRangeAnalysis(_numberOfDisplays) {

}
prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis(unsigned short _prelutRangeCut, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) : prelutRangeAnalysis(_prelutRangeCut, _numberOfMinFactors, _factorMinMin, _factorMinMax, _numberOfMaxFactors, _factorMaxMin, _factorMaxMax) {

}
prelutRangeCorrectLayerAnalysis::prelutRangeCorrectLayerAnalysis(unsigned short _prelutRangeCut, unsigned short _numberOfDisplays, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) : prelutRangeAnalysis(_prelutRangeCut, _numberOfDisplays, _numberOfMinFactors, _factorMinMin, _factorMinMax, _numberOfMaxFactors, _factorMaxMin, _factorMaxMax) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutRangeCorrectLayerAnalysis::~prelutRangeCorrectLayerAnalysis() {

}
