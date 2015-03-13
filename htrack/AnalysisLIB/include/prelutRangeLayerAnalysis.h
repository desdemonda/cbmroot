//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
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
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTRANGELAYERANALYSIS_H
#define _PRELUTRANGELAYERANALYSIS_H


#include "../include/prelutRangeCorrectLayerAnalysis.h"
#include "../include/prelutRangeMainLayerAnalysis.h"


enum {CORRECTRELATIVEDISPLAYMODE, CUTCORRECTRELATIVEDISPLAYMODE, NORMALCORRECTDISPLAYMODE, MAINRELATIVEDISPLAYMODE, CUTMAINRELATIVEDISPLAYMODE, NORMALMAINDISPLAYMODE};


/* **************************************************************
 * CLASS prelutRangeLayerAnalysis								*
 * **************************************************************/

class prelutRangeLayerAnalysis {

protected:

	unsigned short                  displayMode;						/**< Variable to store the mode of the display drawing functionality */
	prelutRangeCorrectLayerAnalysis prelutRangeCorrectLayerAnalyser;	/**< Object to analyze the correct layer */
	prelutRangeMainLayerAnalysis    prelutRangeMainLayerAnalyser;		/**< Object to analyze the main layer */

public:

/**
 * Default constructor
 */

	prelutRangeLayerAnalysis();

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

	prelutRangeLayerAnalysis(unsigned short numberOfDisplays);
	prelutRangeLayerAnalysis(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);
	prelutRangeLayerAnalysis(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);

/**
 * Destructor
 */

	virtual ~prelutRangeLayerAnalysis();

/**
 * Method inits the variables.
 * @param prelutRangeCut is the percentage of occuring tracks which must be reached to choose the min/max combination
 * @param numberOfDisplays is the number of different tests
 * @param numberOfMinFactors is the number of different minimas which should be tested
 * @param factorMinMin is the minimal value for the minimum
 * @param factorMinMax is the maximal value for the minimum
 * @param numberOfMaxFactors is the number of different maximas which should be tested
 * @param factorMaxMin is the minimal value for the maximum
 * @param factorMaxMax is the maximal value for the maximum
 */

	void init(unsigned short numberOfDisplays);
	void init(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);
	void init(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);

/**
 * Method sets the variables.
 * @param prelutRangeCut is the percentage of occuring tracks which must be reached to choose the min/max combination
 * @param numberOfDisplays is the number of different tests
 * @param numberOfMinFactors is the number of different minimas which should be tested
 * @param factorMinMin is the minimal value for the minimum
 * @param factorMinMax is the maximal value for the minimum
 * @param numberOfMaxFactors is the number of different maximas which should be tested
 * @param factorMaxMin is the minimal value for the maximum
 * @param factorMaxMax is the maximal value for the maximum
 */

	void set(unsigned short numberOfDisplays);
	void set(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);
	void set(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);

/**
 * returns the number of value steps
 */

	unsigned int getNumberOfMinFactors();
	unsigned int getNumberOfMaxFactors();

/**
 * returns the value of the index
 */

	double getMinFactor(unsigned int index);
	double getMaxFactor(unsigned int index);

/**
 * resets the values of all displays
 */

	void reset();

/**
 * returns the value for the min/max combination identified by
 * indexMin and indexMax in a specific display
 */

	double getRelativeValue(unsigned short index);
	double getCorrectValue(unsigned short display, unsigned int indexMin, unsigned int indexMax);
	double getMainValue(unsigned short display, unsigned int indexMin, unsigned int indexMax);
	double getConstraintValue(unsigned short display, unsigned int indexMin, unsigned int indexMax);

/**
 * sets the value for the min/max combination identified by
 * indexMin and indexMax in a specific display
 */

	void setRelativeValue(unsigned short index, double value);
	void setCorrectValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);
	void setMainValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);
	void setConstraintValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);
	void setValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);

/**
 * adds the value for the min/max combination identified by
 * indexMin and indexMax in a specific display
 */

	void addRelativeValue(unsigned short index, double value);
	void addCorrectValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);
	void addMainValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);
	void addConstraintValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double value);

/**
 * Method returns the best prelut range.
 * If the display is set to numberOfDisplays,
 * the best of all diatributions would be returned.
 */

	void getCorrectPrelutConstraintRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit);
	void getMainPrelutConstraintRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit);
	void getCorrectPrelutMaximumRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit);
	void getMainPrelutMaximumRange(unsigned short display, double& minimum, double& maximum, unsigned short& info, double& meanEntriesPerHit);

/**
 * This method initializes the root directory for the displays.
 */

	void initPrelutRangeAnalysisToRoot(bool enable, const char* name, bool justUpdate);

/**
 * This method returns true if the prelut range's display is written to file.
 */

	bool isPrelutRangeToRootEnabled();

/**
 * method writes the prelutRange-Analysis into
 * a root file.
 */

	void prelutRangeAnalysisWrite(int eventNumber);

/**
 * This method initializes the prelutRange's display.
 * @param enable enables or disables this analysis
 * @param displayMode is the mode of the display drawing functionality
 */

	void initPrelutRangeAnalysisDisplay(bool enable, unsigned short displayMode = CUTMAINRELATIVEDISPLAYMODE);

/**
 * This method returns true if the prelutRange's display is enabled.
 */

	bool isPrelutRangeDisplayEnabled();

/**
 * method updates the prelutRange-Analysis display.
 */

	void prelutRangeAnalysisUpdate();

/**
 * method draws the prelutRange-Analysis display.
 * @param preventStationDraw consists of flags which enables or disables the drawing of station result displays separately
 * @param preventStationSumDraw is a flag to prevent the drawing of the combined station display
 * @param preventConstraintDraw consists of flags which enables or disables the drawing of constraint result displays separately
 * @param preventConstraintSumDraw is a flag to prevent the drawing of the combined constraint display
 * @param preventRelativeDraw is a flag to prevent the drawing of the relative display
 *
 */

	void prelutRangeAnalysisDraw(bitArray preventStationDraw = 0, bool preventStationSumDraw = false, bitArray preventConstraintDraw = 0, bool preventConstraintSumDraw = false, bool preventRelativeDraw = false);

/**
 * This method returns true if the prelutRange's
 * analysis is enabled.
 */

	bool isPrelutRangeAnalysisEnabled();

};

#endif
