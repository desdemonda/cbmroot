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
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTRANGEANALYSIS_H
#define _PRELUTRANGEANALYSIS_H


#include "../../MiscLIB/include/bitArray.h"
#include "TH2S.h"
#include "TCanvas.h"
#include <string>


typedef struct {

	TH2S** displays;
	int*   padNumbers;

} th2WithPad;

typedef struct {

	TH1S*  display;
	int    padNumber;

} th1WithPad;

/* **************************************************************
 * CLASS prelutRangeAnalysis									*
 * **************************************************************/

class prelutRangeAnalysis {

 private:
  prelutRangeAnalysis(const prelutRangeAnalysis&);
  prelutRangeAnalysis& operator=(const prelutRangeAnalysis&);

protected:

	bool            analysisEnabled;			/**< Variable to store if the analysis is enabled. */
	TCanvas*        window;						/**< Object for the main window. */
	unsigned short  numberOfWindowPadColumns;	/**< Number of columns which divide the window into pads. */
	unsigned short  numberOfWindowPadRows;		/**< Number of rows whihc divide the window into pads. */
	bool            enableDisplay;				/**< Variable to store the enable-state of the window. */
	std::string     outputFileName;				/**< Variable to store the name of the file for the output. */
	bool            writeToFile;				/**< Variable to store if writing into the file is enabled. */
	th1WithPad      relative;					/**< Object to store the relative display. */
	th2WithPad      constraint;					/**< Object to hold the constraint display for each test to evaluate the prelut ranges. */
	th2WithPad      value;						/**< Object to hold the value display for each test. */
	TH2S**          modifiedValueDisplays;		/**< Object to hold the modified value display which should be drawn as graphic for each test. */
	unsigned short  numberOfDisplays;			/**< Variable to store the number of evaluated tests/displays.*/
	unsigned int    numberOfMinFactors;			/**< Variable to store the number of evaluated minimas. */
	double          factorMinMin;				/**< Variable to store the minimal value for the minimum. */
	double          factorMinMax;				/**< Variable to store the maximal value for the minimum. */
	unsigned int    numberOfMaxFactors;			/**< Variable to store the number of evaluated maximas. */
	double          factorMaxMin;				/**< Variable to store the minimal value for the maximum. */
	double          factorMaxMax;				/**< Variable to store the maximal value for the maximum. */
	unsigned short  prelutRangeCut;				/**< Variable to store the percentage of tracks which must occur to choose the min/max combination. */

/**
 * allocates memory
 */

	void allocateMemory();

/**
 * deletes allocated memory
 */

	void deleteMemory();

/**
 * This method initializes the root directory for the displays.
 */

	void createPrelutRangeAnalysisToRoot(bool enable, const char* name, bool justUpdate);

/**
 * This method initializes the root directory for the displays.
 */

	void specializePrelutRangeAnalysisToRoot(bool enable);

/**
 * Method uses the relative display to convert the numbers
 * of the others into a percentage relation
 */

	void doRelative(TH2S* display, unsigned short relativeIndex);

/**
 * Method uses the relative display to convert the numbers
 * of the others into a percentage relation. Afterwards the
 * set cut is applied
 */

	void doRelativeCut(TH2S* display, unsigned short relativeIndex);

/**
 * This method writes a display into the file.
 */

	void writeDisplay(TH1S* display);
	void writeDisplay(TH2S* display);

/**
 * This method initializes the global style for each display.
 */

	void initDisplayStyle();

/**
 * This method initializes the window.
 */

	void initWindow();

/**
 * This method evaluate the window division factors.
 * @param numberOfEnabledPadColumns returns the number of neccessary columns to display everything in one window
 * @param numberOfEnabledPadRows returns the number of neccessary rows to display everything in one window
 * @param preventDraw consists of flags which enables or disables the drawing of displays separately
 * @param preventSumDraw is a flag to prevent the drawing of the combined display
 * @param preventRelativeDraw is a flag to prevent the drawing of the relative display
 */

	void evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw);

/**
 * This method returns the display name
 */

	virtual std::string getDisplayName() = 0;

/**
 * This method returns the display title
 */

	virtual std::string getDisplayTitle() = 0;

/**
 * This method returns the window name
 */

	virtual std::string getWindowName() = 0;

/**
 * This method returns the window name
 */

	virtual std::string getWindowTitle() = 0;

public:

/**
 * Default constructor
 */

	prelutRangeAnalysis();

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

	prelutRangeAnalysis(unsigned short numberOfDisplays);
	prelutRangeAnalysis(unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);
	prelutRangeAnalysis(unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);

/**
 * Destructor
 */

	virtual ~prelutRangeAnalysis();

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
 * resets the values in all displays
 */

	void reset();
/**
 * returns the value in the relative display at index
 */

	double getRelative(unsigned short index);

/**
 * sets the weight in the relative display at index
 */

	void setRelative(unsigned short index, double weight);

/**
 * adds the weight in the relative display at index
 */

	void addRelative(unsigned short index, double weight);

/**
 * returns the value in the constraint display at index
 */

	double getConstraint(unsigned short display, unsigned int indexMin, unsigned int indexMax);

/**
 * sets the weight in the constraint display at index
 */

	void setConstraint(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight);

/**
 * adds the weight in the constraint display at index
 */

	void addConstraint(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight);

/**
 * returns the value for the min/max combination identified by
 * indexMin and indexMax in a specific display
 */

	double getValue(unsigned short display, unsigned int indexMin, unsigned int indexMax);

/**
 * sets the weight for the min/max combination identified by
 * indexMin and indexMax in a specific display
 */

	void setValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight);

/**
 * adds the weight for the min/max combination identified by
 * indexMin and indexMax in a specific display
 */

	void addValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight);

/**
 * Method returns the best prelut range.
 * If the display is set to numberOfDisplays,
 * the best of all diatributions would be returned.
 */

	void getPrelutConstraintRange(unsigned short display, double& minimum, double& maximum, unsigned short& numberOfPrelutRanges, double& meanEntriesPerHit);
	void getPrelutMaximumRange(unsigned short display, double& minimum, double& maximum, unsigned short& maximumPercentage, double& meanEntriesPerHit);

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
	void prelutRangeAnalysisWriteRelative(int eventNumber);
	void prelutRangeAnalysisWriteRelativeCut(int eventNumber);

/**
 * This method initializes the prelutRange's display.
 */

	void initPrelutRangeAnalysisDisplay(bool enable);

/**
 * This method returns true if the prelutRange's display is enabled.
 */

	bool isPrelutRangeDisplayEnabled();

/**
 * method updates the prelutRange-Analysis display.
 */

	void prelutRangeAnalysisUpdate();
	void prelutRangeAnalysisUpdateRelative();
	void prelutRangeAnalysisUpdateRelativeCut();

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
	void prelutRangeAnalysisDrawRelative(bitArray preventStationDraw = 0, bool preventStationSumDraw = false, bitArray preventConstraintDraw = 0, bool preventConstraintSumDraw = false, bool preventRelativeDraw = false);
	void prelutRangeAnalysisDrawRelativeCut(bitArray preventStationDraw = 0, bool preventStationSumDraw = false, bitArray preventConstraintDraw = 0, bool preventConstraintSumDraw = false, bool preventRelativeDraw = false);

/**
 * This method returns true if the prelutRange's
 * analysis is enabled.
 */

	bool isPrelutRangeAnalysisEnabled();

};

#endif
