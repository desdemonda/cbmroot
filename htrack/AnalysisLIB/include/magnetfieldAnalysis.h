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
///     - implements methods to display the magnetfield
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:24 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAGNETFIELDANALYSIS_H
#define _MAGNETFIELDANALYSIS_H


#include "magnetfieldXAnalysis.h"
#include "magnetfieldYAnalysis.h"
#include "magnetfieldZAnalysis.h"
#include "TCanvas.h"
#include <string>


/* **************************************************************
 * CLASS magnetfieldAnalysis									*
 * **************************************************************/

class magnetfieldAnalysis {

 private:
  magnetfieldAnalysis(const magnetfieldAnalysis&);
  magnetfieldAnalysis& operator=(const magnetfieldAnalysis&);

protected:

	TCanvas*              window;						/**< Object for the main window */
	unsigned short        numberOfWindowPadColumns;		/**< Number of columns which divide the window into pads */
	unsigned short        numberOfWindowPadRows;		/**< Number of rows whihc divide the window into pads */
	bool                  enableDisplay;				/**< Variable to store the enable-state of the window */
	magnetfieldXAnalysis* magnetfieldX;					/**< Object to evaluate the B_x, the B_y and the B_z against x */
	magnetfieldYAnalysis* magnetfieldY;					/**< Object to evaluate the B_x, the B_y and the B_z against y */
	magnetfieldZAnalysis* magnetfieldZ;					/**< Object to evaluate the B_x, the B_y and the B_z against z */
	std::string           outputFileName;				/**< Variable to store the name of the file for the output */

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
 * This method evaluate the window division signature.
 * @param globalSignature holds the configuration of all pads of the window
 * @param sizeOfGlobalSignature is the size of globalSignature
 * @param localSignature holds the configuration of the pads for one analysis
 * @param sizeOfLocalSignature is the size of localSignature
 * @return globalSignature
 * @see magnetfieldAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows)
 */

	void evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature);

/**
 * This method evaluates the window division factors.
 * @return numberOfEnabledPadColumns is the number of window-pad-columns to display something
 * @return numberOfEnabledPadRows is the number of window-pad-rows to display something
 * @see magnetfieldAnalysis::magnetfieldAnalysisDraw()
 */

	void evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows);

/**
 * This method draws all window pads corresponding to the
 * columnId.
 */

	void drawWindowPads(unsigned short columnId);

public:

/**
 * Default constructor
 */

	magnetfieldAnalysis();

/**
 * Destructor
 */

	virtual ~magnetfieldAnalysis();

/**
 * This method initializes the root directory for the displays.
 * @param enable is the enabling-flag
 * @param name is the name of the file. If null is supplied it is written to the standard root output file
 * @param justUpdate just updates the file instead of recreating
 */

	void initMagnetfieldAnalysisToRoot(bool enable = true, const char* name = NULL, bool justUpdate = false);

/**
 * This method returns true if the magnetfield's display is written to file.
 */

	bool isMagnetfieldToRootEnabled();

/**
 * This method initializes the graphic's display.
 */

	void initMagnetfieldAnalysisDisplay(bool enable = true);

/**
 * This method returns true if the graphic's display is enabled.
 */

	bool isMagnetfieldDisplayEnabled();

/**
 * method initializes the initMagnetfieldXBxAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXBxAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldXByAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXByAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldXBzGAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXBzAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldXAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYBxAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYBxAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYByAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYByAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYBzAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYBzAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldZBxAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZBxAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldZByAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZByAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldZBzAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZBzAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldZAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldAnalysis(bool enable = true, int nBins = 100, double xMin = 0, double yMin = -50, double zMin = -50, double xMax = 100, double yMax = 50, double zMax = 50, double constValDim1 = 0, double constValDim2 = 0, double constValDim3 = 50);

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * This method returns true, if the bxDisplay is enabled.
 */

	bool isMagnetfieldXBxAnalysisEnabled();

/**
 * This method returns true, if the byDisplay is enabled.
 */

	bool isMagnetfieldXByAnalysisEnabled();

/**
 * This method returns true, if the bzDisplay is enabled.
 */

	bool isMagnetfieldXBzAnalysisEnabled();

/**
 * method returns true, if the magnetfieldX-Analysis is
 * enabled.
 */

	bool isMagnetfieldXAnalysisEnabled();

/**
 * This method returns true, if the bxDisplay is enabled.
 */

	bool isMagnetfieldYBxAnalysisEnabled();

/**
 * This method returns true, if the byDisplay is enabled.
 */

	bool isMagnetfieldYByAnalysisEnabled();

/**
 * This method returns true, if the bzDisplay is enabled.
 */

	bool isMagnetfieldYBzAnalysisEnabled();

/**
 * method returns true, if the magnetfieldY-Analysis is
 * enabled.
 */

	bool isMagnetfieldYAnalysisEnabled();

/**
 * This method returns true, if the bxDisplay is enabled.
 */

	bool isMagnetfieldZBxAnalysisEnabled();

/**
 * This method returns true, if the byDisplay is enabled.
 */

	bool isMagnetfieldZByAnalysisEnabled();

/**
 * This method returns true, if the bzDisplay is enabled.
 */

	bool isMagnetfieldZBzAnalysisEnabled();

/**
 * method returns true, if the magnetfieldZ-Analysis is
 * enabled.
 */

	bool isMagnetfieldZAnalysisEnabled();

/**
 * This method returns the xbxDisplay's axi range.
 */

	magnetfieldDisplayInfo* getXBxDisplayAxiRange();

/**
 * This method returns the xbyDisplay's axi range.
 */

	magnetfieldDisplayInfo* getXByDisplayAxiRange();

/**
 * This method returns the xbzDisplay's axi range.
 */

	magnetfieldDisplayInfo* getXBzDisplayAxiRange();

/**
 * This method returns the ybxDisplay's axi range.
 */

	magnetfieldDisplayInfo* getYBxDisplayAxiRange();

/**
 * This method returns the ybyDisplay's axi range.
 */

	magnetfieldDisplayInfo* getYByDisplayAxiRange();

/**
 * This method returns the ybzDisplay's axi range.
 */

	magnetfieldDisplayInfo* getYBzDisplayAxiRange();

/**
 * This method returns the zbxDisplay's axi range.
 */

	magnetfieldDisplayInfo* getZBxDisplayAxiRange();

/**
 * This method returns the zbyDisplay's axi range.
 */

	magnetfieldDisplayInfo* getZByDisplayAxiRange();

/**
 * This method returns the zbzDisplay's axi range.
 */

	magnetfieldDisplayInfo* getZBzDisplayAxiRange();

/**
 * This method returns the number of xBxDisplay entries.
 */

	unsigned int getXBxDisplayEntries();

/**
 * This method returns the number of xByDisplay entries.
 */

	unsigned int getXByDisplayEntries();

/**
 * This method returns the number of xBzDisplay entries.
 */

	unsigned int getXBzDisplayEntries();

/**
 * This method returns the number of yBxDisplay entries.
 */

	unsigned int getYBxDisplayEntries();

/**
 * This method returns the number of yByDisplay entries.
 */

	unsigned int getYByDisplayEntries();

/**
 * This method returns the number of yBzDisplay entries.
 */

	unsigned int getYBzDisplayEntries();

/**
 * This method returns the number of zBxDisplay entries.
 */

	unsigned int getZBxDisplayEntries();

/**
 * This method returns the number of zByDisplay entries.
 */

	unsigned int getZByDisplayEntries();

/**
 * This method returns the number of zBzDisplay entries.
 */

	unsigned int getZBzDisplayEntries();

/**
 * This method sets the values for x and y at the given point
 * at position index in the xbxDisplay.
 */

	void setXBxPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the xbyDisplay.
 */

	void setXByPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the xbzDisplay.
 */

	void setXBzPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the ybxDisplay.
 */

	void setYBxPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the ybyDisplay.
 */

	void setYByPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the ybzDisplay.
 */

	void setYBzPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the zbxDisplay.
 */

	void setZBxPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the zbyDisplay.
 */

	void setZByPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the zbzDisplay.
 */

	void setZBzPoint(int index, double xValue, double yValue);

/**
 * This method updates the window.
 */

	void magnetfieldAnalysisUpdate();

/**
 * This method draws the window.
 */

	void magnetfieldAnalysisDraw();

/**
 * method writes the magnetfield-Analysis into
 * a root file.
 */

	void magnetfieldAnalysisWrite();

/**
 * method  returns true, if the magnetfield-Analysis
 * is enabled.
 */

	bool isMagnetfieldAnalysisEnabled();

/**
 * method returns a string representing the result of the
 * magnetfield-Analysis.
 */

	std::string getMagnetfieldAnalysis();

};

#endif

