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
///     - implements methods to display the track propagation quality
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2009-01-12 14:16:24 $
/// $Revision: 1.0 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKPROPAGATIONANALYSIS_H
#define _TRACKPROPAGATIONANALYSIS_H


#include "../../MiscLIB/include/bitArray.h"
#include "trackPropagationEventPointAnalysis.h"
#include "trackPropagationEventHitAnalysis.h"
#include "trackPropagationTotalPointAnalysis.h"
#include "trackPropagationTotalHitAnalysis.h"
#include "TCanvas.h"
#include <string>


/* **************************************************************
 * CLASS trackPropagationAnalysis								*
 * **************************************************************/

class trackPropagationAnalysis {

 private:
  trackPropagationAnalysis(const trackPropagationAnalysis&);
  trackPropagationAnalysis& operator=(const trackPropagationAnalysis&);

protected:

	TCanvas*                            window;								/**< Object for the main window */
	unsigned short                      numberOfWindowPadColumns;			/**< Number of columns which divide the window into pads */
	unsigned short                      numberOfWindowPadRows;				/**< Number of rows which divide the window into pads */
	bool                                enableDisplay;						/**< Variable to store the enable-state of the window */
	trackPropagationEventPointAnalysis* trackPropagationEventPoint;			/**< Object to evaluate the number of tracks for each event against their distances */
	trackPropagationEventHitAnalysis*   trackPropagationEventHit;			/**< Object to evaluate the number of tracks for each event against their distances */
	trackPropagationTotalPointAnalysis* trackPropagationTotalPoint;			/**< Object to evaluate the number of tracks accumulated for all events against their distances */
	trackPropagationTotalHitAnalysis*   trackPropagationTotalHit;			/**< Object to evaluate the number of tracks accumulated for all events against their distances */
	std::string                         outputFileName;						/**< Variable to store the name of the file for the output */

/**
 * This method initializes the global style for each display.
 */

	void initDisplayStyle();

/**
 * This method initializes the window.
 */

	void initWindow();

/**
 * This method evaluate the window division signature.
 * @param globalSignature holds the configuration of all pads of the window
 * @param sizeOfGlobalSignature is the size of globalSignature
 * @param localSignature holds the configuration of the pads for one analysis
 * @param sizeOfLocalSignature is the size of localSignature
 * @return globalSignature
 * @see trackPropagationAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows)
 */

	void evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature);

/**
 * This method evaluates the window division factors.
 * @return numberOfEnabledPadColumns is the number of window-pad-columns to display something
 * @return numberOfEnabledPadRows is the number of window-pad-rows to display something
 * @param preventPointDraw consists of flags which enables or disables the drawing of the point result displays separately
 * @param preventHitDraw consists of flags which enables or disables the drawing of the hit result displays separately
 * @see trackPropagationAnalysis::trackPropagationAnalysisDraw()
 */

	void evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, bitArray preventPointDraw, bitArray preventHitDraw);

/**
 * This method draws all window pads corresponding to the
 * columnId.
 */

	void drawWindowPads(unsigned short columnId);

public:

/**
 * Default constructor
 */

	trackPropagationAnalysis();

/**
 * Destructor
 */

	virtual ~trackPropagationAnalysis();

/**
 * This method initializes the root directory for the displays.
 * @param enable is the enabling-flag
 * @param name is the name of the file. If null is supplied it is written to the standard root output file
 * @param justUpdate just updates the file instead of recreating
 */

	void initTrackPropagationAnalysisToRoot(bool enable = true, const char* name = NULL, bool justUpdate = false);

/**
 * This method returns true if the momentum's display is written to file.
 */

	bool isTrackPropagationToRootEnabled();

/**
 * This method initializes the track propagation's display.
 */

	void initTrackPropagationAnalysisDisplay(bool enable = true);

/**
 * This method returns true if the track propagation's display is enabled.
 */

	bool isTrackPropagationDisplayEnabled();

/**
 * method initializes the number of displays.
 * @param numberOfDisplays represents the value number
 */

	void initNumberOfDisplays(unsigned short numberOfDisplays);

/**
 * method initializes the track propagation-Analysis.
 * @param enable enables or disables this analysis
 */

	void initTrackPropagationEventPointAnalysis(bool enable = true, unsigned short index = (unsigned short)-1, int nBins = 100, int xMin = 0, int xMax = 10);
	void initTrackPropagationEventHitAnalysis(bool enable = true, unsigned short index = (unsigned short)-1, int nBins = 100, int xMin = 0, int xMax = 10);
	void initTrackPropagationTotalPointAnalysis(bool enable = true, unsigned short index = (unsigned short)-1, int nBins = 100, int xMin = 0, int xMax = 10);
	void initTrackPropagationTotalHitAnalysis(bool enable = true, unsigned short index = (unsigned short)-1, int nBins = 100, int xMin = 0, int xMax = 10);

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * method adds a point distance.
 * @param index represents the display index
 * @param distance represents the value to be entried
 */

	void addPointDistance(unsigned short index, double distance);

/**
 * method adds a hit distance.
 * @param index represents the display index
 * @param distance represents the value to be entried
 */

	void addHitDistance(unsigned short index, double distance);

/**
 * method returns true, if the track-propagation-Analysis is
 * enabled.
 */

	bool isTrackPropagationEventPointAnalysisEnabled();
	bool isTrackPropagationEventHitAnalysisEnabled();
	bool isTrackPropagationTotalPointAnalysisEnabled();
	bool isTrackPropagationTotalHitAnalysisEnabled();
	bool isTrackPropagationEventAnalysisEnabled();
	bool isTrackPropagationTotalAnalysisEnabled();

/**
 * This method updates the window.
 */

	void trackPropagationAnalysisUpdate();

/**
 * This method draws the window.
 * @param preventPointDraw consists of flags which enables or disables the drawing of the point result displays separately
 * @param preventHitDraw consists of flags which enables or disables the drawing of the hit result displays separately
 */

	void trackPropagationAnalysisDraw(bitArray preventPointDraw = 0, bitArray preventHitDraw = 0);

/**
 * method writes the track-propagation-Analysis for each event into
 * a root file.
 */

	void trackPropagationEventAnalysisWrite(int eventNumber);

/**
 * method writes the track-propagation-Analysis over all events into
 * a root file.
 */

	void trackPropagationTotalAnalysisWrite(int eventNumber);

/**
 * method returns a string representing the result of the
 * momentumEFGCEvent-Analysis.
 */

	std::string getTrackPropagationEventAnalysis();

/**
 * method returns a string representing the result of the
 * momentumEFGCTotal-Analysis.
 */

	std::string getTrackPropagationTotalAnalysis();

};

#endif

