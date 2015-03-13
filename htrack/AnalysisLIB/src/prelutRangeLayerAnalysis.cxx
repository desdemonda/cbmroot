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
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:27 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/analysisError.h"
#include "../include/prelutRangeLayerAnalysis.h"


/* **************************************************************
 * CLASS prelutRangeLayerAnalysis								*
 * **************************************************************/


/* **************************************************************
 * Default constructor											*
 * **************************************************************/

prelutRangeLayerAnalysis::prelutRangeLayerAnalysis() 
  : displayMode(CUTMAINRELATIVEDISPLAYMODE),
    prelutRangeCorrectLayerAnalyser(),
    prelutRangeMainLayerAnalyser()
{

  //	displayMode = CUTMAINRELATIVEDISPLAYMODE;

}

/* **************************************************************
 * Constructor													*
 * **************************************************************/

prelutRangeLayerAnalysis::prelutRangeLayerAnalysis(unsigned short numberOfDisplays) 
  : displayMode(CUTMAINRELATIVEDISPLAYMODE),
    prelutRangeCorrectLayerAnalyser(),
    prelutRangeMainLayerAnalyser()
{

  //	displayMode = CUTMAINRELATIVEDISPLAYMODE;
	prelutRangeCorrectLayerAnalyser.init(numberOfDisplays);
	prelutRangeMainLayerAnalyser.init(numberOfDisplays);

}
prelutRangeLayerAnalysis::prelutRangeLayerAnalysis(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) 
  : displayMode(CUTMAINRELATIVEDISPLAYMODE),
    prelutRangeCorrectLayerAnalyser(),
    prelutRangeMainLayerAnalyser()
{

  //	displayMode = CUTMAINRELATIVEDISPLAYMODE;
	prelutRangeCorrectLayerAnalyser.init(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
	prelutRangeMainLayerAnalyser.init(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

}
prelutRangeLayerAnalysis::prelutRangeLayerAnalysis(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) 
  : displayMode(CUTMAINRELATIVEDISPLAYMODE),
    prelutRangeCorrectLayerAnalyser(),
    prelutRangeMainLayerAnalyser()
{

  //	displayMode = CUTMAINRELATIVEDISPLAYMODE;
	prelutRangeCorrectLayerAnalyser.init(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
	prelutRangeMainLayerAnalyser.init(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

}

/* **************************************************************
 * Destructor													*
 * **************************************************************/

prelutRangeLayerAnalysis::~prelutRangeLayerAnalysis() {

}

/* **************************************************************
 * Method inits the variables.									*
 * **************************************************************/

void prelutRangeLayerAnalysis::init(unsigned short numberOfDisplays) {

	prelutRangeCorrectLayerAnalyser.init(numberOfDisplays);
	prelutRangeMainLayerAnalyser.init(numberOfDisplays);

}
void prelutRangeLayerAnalysis::init(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) {

	prelutRangeCorrectLayerAnalyser.init(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
	prelutRangeMainLayerAnalyser.init(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

}
void prelutRangeLayerAnalysis::init(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) {

	prelutRangeCorrectLayerAnalyser.init(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
	prelutRangeMainLayerAnalyser.init(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

}

/* **************************************************************
 * Method sets the variables.									*
 * **************************************************************/

void prelutRangeLayerAnalysis::set(unsigned short numberOfDisplays) {

	prelutRangeCorrectLayerAnalyser.set(numberOfDisplays);
	prelutRangeMainLayerAnalyser.set(numberOfDisplays);

}
void prelutRangeLayerAnalysis::set(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) {

	prelutRangeCorrectLayerAnalyser.set(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
	prelutRangeMainLayerAnalyser.set(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

}
void prelutRangeLayerAnalysis::set(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) {

	prelutRangeCorrectLayerAnalyser.set(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
	prelutRangeMainLayerAnalyser.set(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

}

/* **************************************************************
 * returns the number of value steps							*
 * **************************************************************/

unsigned int prelutRangeLayerAnalysis::getNumberOfMinFactors() {

	return prelutRangeCorrectLayerAnalyser.getNumberOfMinFactors();

}
unsigned int prelutRangeLayerAnalysis::getNumberOfMaxFactors() {

	return prelutRangeCorrectLayerAnalyser.getNumberOfMaxFactors();

}

/* **************************************************************
 * returns the value of the index								*
 * **************************************************************/

double prelutRangeLayerAnalysis::getMinFactor(unsigned int index) {

	return prelutRangeCorrectLayerAnalyser.getMinFactor(index);

}
double prelutRangeLayerAnalysis::getMaxFactor(unsigned int index) {

	return prelutRangeCorrectLayerAnalyser.getMaxFactor(index);

}

/* **************************************************************
 * resets the values of all displays							*
 * **************************************************************/

void prelutRangeLayerAnalysis::reset() {

	prelutRangeCorrectLayerAnalyser.reset();
	prelutRangeMainLayerAnalyser.reset();

}

/* **************************************************************
 * returns the value for the min/max combination identified by	*
 * indexMin and indexMax in a specific display					*
 * **************************************************************/

double prelutRangeLayerAnalysis::getRelativeValue(unsigned short index) {

	double correctValue;
	double mainValue;
	double returnValue;

	correctValue = prelutRangeCorrectLayerAnalyser.getRelative(index);
	mainValue    = prelutRangeMainLayerAnalyser.getRelative(index);

	if (correctValue == mainValue)
		returnValue = correctValue;
	else
		throw wrongRelativeValueFoundError();

	return returnValue;

}
double prelutRangeLayerAnalysis::getCorrectValue(unsigned short display, unsigned int indexMin, unsigned int indexMax) {

	return prelutRangeCorrectLayerAnalyser.getValue(display, indexMin, indexMax);

}
double prelutRangeLayerAnalysis::getMainValue(unsigned short display, unsigned int indexMin, unsigned int indexMax) {

	return prelutRangeMainLayerAnalyser.getValue(display, indexMin, indexMax);

}
double prelutRangeLayerAnalysis::getConstraintValue(unsigned short display, unsigned int indexMin, unsigned int indexMax) {

	double correctValue;
	double mainValue;
	double returnValue;

	correctValue = prelutRangeCorrectLayerAnalyser.getConstraint(display, indexMin, indexMax);
	mainValue    = prelutRangeMainLayerAnalyser.getConstraint(display, indexMin, indexMax);

	if (correctValue == mainValue)
		returnValue = correctValue;
	else
		throw wrongConstraintValueFoundError();

	return returnValue;

}

/* **************************************************************
 * sets the value for the min/max combination identified by		*
 * indexMin and indexMax in a specific display					*
 * **************************************************************/

void prelutRangeLayerAnalysis::setRelativeValue(unsigned short index, double value) {

	prelutRangeCorrectLayerAnalyser.setRelative(index, value);
	prelutRangeMainLayerAnalyser.setRelative(index, value);

}
void prelutRangeLayerAnalysis::setCorrectValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	prelutRangeCorrectLayerAnalyser.setValue(display, indexMin, indexMax, value);

}
void prelutRangeLayerAnalysis::setMainValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	prelutRangeMainLayerAnalyser.setValue(display, indexMin, indexMax, value);

}
void prelutRangeLayerAnalysis::setConstraintValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	prelutRangeCorrectLayerAnalyser.setConstraint(display, indexMin, indexMax, value);
	prelutRangeMainLayerAnalyser.setConstraint(display, indexMin, indexMax, value);

}
void prelutRangeLayerAnalysis::setValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	setCorrectValue(display, indexMin, indexMax, value);
	setMainValue(display, indexMin, indexMax, value);
	setConstraintValue(display, indexMin, indexMax, value);

}

/* **************************************************************
 * sets the value for the min/max combination identified by		*
 * indexMin and indexMax in a specific display					*
 * **************************************************************/

void prelutRangeLayerAnalysis::addRelativeValue(unsigned short index, double value) {

	prelutRangeCorrectLayerAnalyser.addRelative(index, value);
	prelutRangeMainLayerAnalyser.addRelative(index, value);

}
void prelutRangeLayerAnalysis::addCorrectValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	prelutRangeCorrectLayerAnalyser.addValue(display, indexMin, indexMax, value);

}
void prelutRangeLayerAnalysis::addMainValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	prelutRangeMainLayerAnalyser.addValue(display, indexMin, indexMax, value);

}
void prelutRangeLayerAnalysis::addConstraintValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value) {

	prelutRangeCorrectLayerAnalyser.addConstraint(display, indexMin, indexMax, value);
	prelutRangeMainLayerAnalyser.addConstraint(display, indexMin, indexMax, value);

}

/****************************************************************
 * Method returns the best prelut range.						*
 * If the display is set to numberOfDisplays,					*
 * the best of all diatributions would be returned.				*
 ****************************************************************/

void prelutRangeLayerAnalysis::getCorrectPrelutConstraintRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit) {

	prelutRangeCorrectLayerAnalyser.getPrelutConstraintRange(display, minimum, maximum, info, meanEntriesPerHit);

}
void prelutRangeLayerAnalysis::getMainPrelutConstraintRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit) {

	prelutRangeMainLayerAnalyser.getPrelutConstraintRange(display, minimum, maximum, info, meanEntriesPerHit);

}
void prelutRangeLayerAnalysis::getCorrectPrelutMaximumRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit) {

	prelutRangeCorrectLayerAnalyser.getPrelutMaximumRange(display, minimum, maximum, info, meanEntriesPerHit);

}
void prelutRangeLayerAnalysis::getMainPrelutMaximumRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit) {

	prelutRangeMainLayerAnalyser.getPrelutMaximumRange(display, minimum, maximum, info, meanEntriesPerHit);

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void prelutRangeLayerAnalysis::initPrelutRangeAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	prelutRangeCorrectLayerAnalyser.initPrelutRangeAnalysisToRoot(enable, name, justUpdate);
	prelutRangeMainLayerAnalyser.initPrelutRangeAnalysisToRoot(enable, name, justUpdate);

}

/****************************************************************
 * This method returns true if the prelut range's display is	*
 * written to file.												*
 ****************************************************************/

bool prelutRangeLayerAnalysis::isPrelutRangeToRootEnabled() {

	return (prelutRangeCorrectLayerAnalyser.isPrelutRangeToRootEnabled() | prelutRangeMainLayerAnalyser.isPrelutRangeToRootEnabled());

}

/****************************************************************
 * method writes the prelutRange-Analysis into					*
 * a root file.													*
 ****************************************************************/

void prelutRangeLayerAnalysis::prelutRangeAnalysisWrite(int eventNumber) {

	switch(displayMode) {

		case CORRECTRELATIVEDISPLAYMODE:
		case MAINRELATIVEDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeToRootEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisWriteRelative(eventNumber);
			if (prelutRangeMainLayerAnalyser.isPrelutRangeToRootEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisWriteRelative(eventNumber);
			break;
		case CUTCORRECTRELATIVEDISPLAYMODE:
		case CUTMAINRELATIVEDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeToRootEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisWriteRelativeCut(eventNumber);
			if (prelutRangeMainLayerAnalyser.isPrelutRangeToRootEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisWriteRelativeCut(eventNumber);
			break;
		default: // NORMALCORRECTDISPLAYMODE || NORMALMAINDISPLAYMODE
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeToRootEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisWrite(eventNumber);
			if (prelutRangeMainLayerAnalyser.isPrelutRangeToRootEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisWrite(eventNumber);
			break;

	}

}

/****************************************************************
 * This method initializes the magnetfield factor's display.	*
 ****************************************************************/

void prelutRangeLayerAnalysis::initPrelutRangeAnalysisDisplay(bool enable, unsigned short _displayMode) {

	this->displayMode = _displayMode;

	switch(this->displayMode) {

		case CORRECTRELATIVEDISPLAYMODE:
		case CUTCORRECTRELATIVEDISPLAYMODE:
		case NORMALCORRECTDISPLAYMODE:
			if (enable)
				prelutRangeMainLayerAnalyser.initPrelutRangeAnalysisDisplay(!enable);
			prelutRangeCorrectLayerAnalyser.initPrelutRangeAnalysisDisplay(enable);
			break;
		case MAINRELATIVEDISPLAYMODE:
		case CUTMAINRELATIVEDISPLAYMODE:
		default: // NORMALMAINDISPLAYMODE
			if (enable)
				prelutRangeCorrectLayerAnalyser.initPrelutRangeAnalysisDisplay(!enable);
			prelutRangeMainLayerAnalyser.initPrelutRangeAnalysisDisplay(enable);
			break;

	}

}

/****************************************************************
 * This method returns true if the prelutRange's display		*
 * is enabled.													*
 ****************************************************************/

bool prelutRangeLayerAnalysis::isPrelutRangeDisplayEnabled() {

	return (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled() | prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled());

}

/****************************************************************
 * method updates the prelutRange-Analysis display.				*
 ****************************************************************/

void prelutRangeLayerAnalysis::prelutRangeAnalysisUpdate() {

	switch(displayMode) {

		case CORRECTRELATIVEDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisUpdateRelative();
			break;
		case CUTCORRECTRELATIVEDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisUpdateRelativeCut();
			break;
		case NORMALCORRECTDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisUpdate();
			break;
		case MAINRELATIVEDISPLAYMODE:
			if (prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisUpdateRelative();
			break;
		case CUTMAINRELATIVEDISPLAYMODE:
			if (prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisUpdateRelativeCut();
			break;
		default: // NORMALMAINDISPLAYMODE
			if (prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisUpdate();
			break;

	}

}

/****************************************************************
 * method draws the prelutRange-Analysis display.				*
 ****************************************************************/

void prelutRangeLayerAnalysis::prelutRangeAnalysisDraw(bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw) {

	switch(displayMode) {

		case CORRECTRELATIVEDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisDrawRelative(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);
			break;
		case CUTCORRECTRELATIVEDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisDrawRelativeCut(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);
			break;
		case NORMALCORRECTDISPLAYMODE:
			if (prelutRangeCorrectLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeCorrectLayerAnalyser.prelutRangeAnalysisDraw(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);
			break;
		case MAINRELATIVEDISPLAYMODE:
			if (prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisDrawRelative(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);
			break;
		case CUTMAINRELATIVEDISPLAYMODE:
			if (prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisDrawRelativeCut(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);
			break;
		default: // NORMALMAINDISPLAYMODE
			if (prelutRangeMainLayerAnalyser.isPrelutRangeDisplayEnabled())
				prelutRangeMainLayerAnalyser.prelutRangeAnalysisDraw(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);
			break;

	}

}

/****************************************************************
 * This method returns true if the prelutRange's				*
 * analysis is enabled.											*
 ****************************************************************/

bool prelutRangeLayerAnalysis::isPrelutRangeAnalysisEnabled() {

	return (prelutRangeCorrectLayerAnalyser.isPrelutRangeAnalysisEnabled() | prelutRangeMainLayerAnalyser.isPrelutRangeAnalysisEnabled());

}
