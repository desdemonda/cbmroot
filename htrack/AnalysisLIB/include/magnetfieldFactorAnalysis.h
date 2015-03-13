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
///     - implements methods to display the magnetfield factors
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:07:01 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAGNETFIELDFACTORANALYSIS_H
#define _MAGNETFIELDFACTORANALYSIS_H


#include "../../MiscLIB/include/bitArray.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <string>


/* **************************************************************
 * CLASS magnetfieldFactorAnalysis								*
 * **************************************************************/

class magnetfieldFactorAnalysis {

 private:
  magnetfieldFactorAnalysis(const magnetfieldFactorAnalysis&);
  magnetfieldFactorAnalysis& operator=(const magnetfieldFactorAnalysis&);

protected:

	bool           analysisEnabled;				/**< Variable to store if the analysis is enabled. */
	TCanvas*       window;						/**< Object for the main window. */
	unsigned short numberOfWindowPadColumns;	/**< Number of columns which divide the window into pads. */
	unsigned short numberOfWindowPadRows;		/**< Number of rows which divide the window into pads. */
	bool           enableDisplay;				/**< Variable to store the enable-state of the window. */
	std::string    outputFileName;				/**< Variable to store the name of the file for the output. */
	bool           writeToFile;					/**< Variable to store if writing into the file is enabled. */
	TGraph**       displays;					/**< Variable to hold the display for each test. */
	double**       distances;					/**< Variable to store the distances for all evaluated factors and all evaluated tests/displays. */
	double*        factors;						/**< Variable to store all evaluated factors. */
	unsigned short numberOfDisplays;			/**< Variable to store the number of evaluated tests/displays.*/
	unsigned int   numberOfFactors;				/**< Variable to store number of evaluated factors. */
	double         factorMin;					/**< Variable to store the minimal value for the factor. */
	double         factorMax;					/**< Variable to store the maximal value for the factor. */

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

	void createMagnetfieldFactorAnalysisToRoot(bool enable, const char* name, bool justUpdate);

/**
 * This method initializes the root directory for the displays.
 */

	void specializeMagnetfieldFactorAnalysisToRoot(bool enable);

/**
 * This method writes a display into the file.
 */

	void writeDisplay(TGraph* display);

/**
 * This method initializes the global style for each display.
 */

	void initDisplayStyle();

/**
 * This method initializes the window.
 */

	void initWindow();

/**
 * This method sets the range of the axis for each display.
 */

	void setAutomaticAxisRange();

/**
 * This method evaluate the window division factors.
 */

	void evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, bitArray preventDraw);

public:

/**
 * Default constructor
 */

	magnetfieldFactorAnalysis();

/**
 * Constructor
 * @param numberOfFactors is the number of different factors which should be tested
 * @param factorMin is the minimal value for the factor
 * @param factorMax is the maximal value for the factor
 * @param numberOfDisplays is the number of different tests
 */

	magnetfieldFactorAnalysis(unsigned short numberOfDisplays);
	magnetfieldFactorAnalysis(unsigned int numberOfFactors, double factorMin, double factorMax);
	magnetfieldFactorAnalysis(unsigned short numberOfDisplays, unsigned int numberOfFactors, double factorMin, double factorMax);

/**
 * Destructor
 */

	virtual ~magnetfieldFactorAnalysis();

/**
 * Method inits the variables.
 * @param numberOfFactors is the number of different factors which should be tested
 * @param factorMin is the minimal value for the factor
 * @param factorMax is the maximal value for the factor
 * @param numberOfDisplays is the number of different tests
 */

	void init(unsigned short numberOfDisplays);
	void init(unsigned int numberOfFactors, double factorMin, double factorMax);
	void init(unsigned short numberOfDisplays, unsigned int numberOfFactors, double factorMin, double factorMax);

/**
 * Method sets the variables.
 * @param numberOfFactors is the number of different factors which should be tested
 * @param factorMin is the minimal value for the factor
 * @param factorMax is the maximal value for the factor
 * @param numberOfDisplays is the number of different tests
 */

	void set(unsigned short numberOfDisplays);
	void set(unsigned int numberOfFactors, double factorMin, double factorMax);
	void set(unsigned short numberOfDisplays, unsigned int numberOfFactors, double factorMin, double factorMax);

/**
 * returns the number of magnetfield factor steps
 */

	unsigned int getNumberOfFactors();

/**
 * returns the magnetfield factor of the index
 */

	double getFactor(unsigned int index);

/**
 * sets the distance for the index of a magnetfield factor of a specific display
 */

	void setDistance(unsigned short display, unsigned int index, double value);

/**
 * adds the distance for the index of a magnetfield factor of a specific display
 */

	void addDistance(unsigned short display, unsigned int index, double value);

/**
 * method returns the magnetfield factors for the hough	transformation
 */

	double getMagnetfieldFactor(unsigned short display);

/**
 * This method initializes the root directory for the displays.
 */

	void initMagnetfieldFactorAnalysisToRoot(bool enable, const char* name, bool justUpdate);

/**
 * This method returns true if the magnetfield's display is written to file.
 */

	bool isMagnetfieldFactorToRootEnabled();

/**
 * method writes the magnetfieldFactor-Analysis into
 * a root file.
 */

	void magnetfieldFactorAnalysisWrite(int eventNumber);

/**
 * This method initializes the magnetfield factor's display.
 */

	void initMagnetfieldFactorAnalysisDisplay(bool enable);

/**
 * This method returns true if the magnetfield factor's display is enabled.
 */

	bool isMagnetfieldFactorDisplayEnabled();

/**
 * method updates the magnetfield factor-Analysis display.
 */

	void magnetfieldFactorAnalysisUpdate();

/**
 * method draws the magnetfield factor-Analysis display.
 * @param preventDraw consists of flags which enables or disables the drawing of displays separately
 */

	void magnetfieldFactorAnalysisDraw(bitArray preventDraw = 0);

/**
 * This method returns true if the magnetfield factor's
 * analysis is enabled.
 */

	bool isMagnetfieldFactorAnalysisEnabled();

};

#endif
