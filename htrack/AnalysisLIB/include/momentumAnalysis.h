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
///     - implements methods to display the trackfinding algorithm's quality
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:24 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MOMENTUMANALYSIS_H
#define _MOMENTUMANALYSIS_H


#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include "momentumEFGCEventPzAnalysis.h"
#include "momentumEFGCEventPtAnalysis.h"
#include "momentumEFGCTotalPzAnalysis.h"
#include "momentumEFGCTotalPtAnalysis.h"
#include "TCanvas.h"
#include <string>


/* **************************************************************
 * CLASS momentumAnalysis				 						*
 * **************************************************************/

class momentumAnalysis {

 private:
  momentumAnalysis(const momentumAnalysis&);
  momentumAnalysis& operator=(const momentumAnalysis&);

protected:

	TCanvas*                     window;					/**< Object for the main window */
	unsigned short               numberOfWindowPadColumns;	/**< Number of columns which divide the window into pads */
	unsigned short               numberOfWindowPadRows;		/**< Number of rows which divide the window into pads */
	bool                         enableDisplay;				/**< Variable to store the enable-state of the window */
	momentumEFGCEventPzAnalysis* efgcEventPz;				/**< Object to evaluate the efficiency, the fakes, the ghosts and the clones for each event against the momentum */
	momentumEFGCEventPtAnalysis* efgcEventPt;				/**< Object to evaluate the efficiency, the fakes, the ghosts and the clones for each event against the transversale impulse */
	momentumEFGCTotalPzAnalysis* efgcTotalPz;				/**< Object to evaluate the efficiency, the fakes, the ghosts and the clones accumulated for all events against the momentum */
	momentumEFGCTotalPtAnalysis* efgcTotalPt;				/**< Object to evaluate the efficiency, the fakes, the ghosts and the clones accumulated for all events against the transversale impulse */
	bool                         efgcEventAnalysis;			/**< Variable to store the enable-state of the efficiency-, the fakes-, the ghosts- and the clones-analysis for each event */
	bool                         efgcTotalAnalysis;			/**< Variable to store the enable-state of the accumulated efficiency-, the fakes-, the ghosts- and the clones-analysis for all events */
	std::string                  outputFileName;			/**< Variable to store the name of the file for the output */

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
 * @see momentumAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows)
 */

	void evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature);

/**
 * This method evaluates the window division factors.
 * @return numberOfEnabledPadColumns is the number of window-pad-columns to display something
 * @return numberOfEnabledPadRows is the number of window-pad-rows to display something
 * @see momentumAnalysis::momentumAnalysisDraw()
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

	momentumAnalysis();

/**
 * Destructor
 */

	virtual ~momentumAnalysis();

/**
 * This method initializes the root directory for the displays.
 * @param enable is the enabling-flag
 * @param name is the name of the file. If null is supplied it is written to the standard root output file
 * @param justUpdate just updates the file instead of recreating
 */

	void initMomentumAnalysisToRoot(bool enable = true, const char* name = NULL, bool justUpdate = false);

/**
 * This method returns true if the momentum's display is written to file.
 */

	bool isMomentumToRootEnabled();

/**
 * This method initializes the momentum's display.
 */

	void initMomentumAnalysisDisplay(bool enable = true);

/**
 * This method returns true if the momentum's display is enabled.
 */

	bool isMomentumDisplayEnabled();

/**
 * method initializes the momentumEFGCEventPzE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPtE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPzE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPtE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param track is the actual processing information of the
 *        found track
 * @param space is the actual object containing the Hough dimensions
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToFakeTrack(trackParameter& parameter, histogramSpace& space);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param track is the actual processing information of the
 *        found track
 * @param space is the actual object containing the Hough dimensions
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToNoTrack(trackParameter& parameter, histogramSpace& space);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param track is the actual processing information of the
 *        found track
 * @param space is the actual object containing the Hough dimensions
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToOneTrack(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param track is the actual processing information of the
 *        found track
 * @param space is the actual object containing the Hough dimensions
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToMoreTracks(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToNoPeak(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToOnePeak(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToMorePeaks(trackfinderInputTrack& actualTrack);

/**
 * method returns true, if the momentumEFGCEventPz-Analysis is
 * enabled.
 */

	bool isMomentumEFGCEventPzAnalysisEnabled();

/**
 * method returns true, if the momentumEFGCTotalPz-Analysis is
 * enabled.
 */

	bool isMomentumEFGCTotalPzAnalysisEnabled();

/**
 * method returns true, if the momentumEFGCEventPt-Analysis is
 * enabled.
 */

	bool isMomentumEFGCEventPtAnalysisEnabled();

/**
 * method returns true, if the momentumEFGCTotalPt-Analysis is
 * enabled.
 */

	bool isMomentumEFGCTotalPtAnalysisEnabled();

/**
 * This method updates the window.
 */

	void momentumAnalysisUpdate();

/**
 * This method draws the window.
 */

	void momentumAnalysisDraw();

/**
 * method writes the momentumEvent-Analysis for each event into
 * a root file.
 */

	void momentumEventAnalysisWrite(int eventNumber);

/**
 * method writes the momentumTotal-Analysis over all events into
 * a root file.
 */

	void momentumTotalAnalysisWrite(int eventNumber);

/**
 * method initializes the momentumEventAnalysis.
 */

	void initMomentumEventAnalysis(bool enable = true);

/**
 * method initializes the momentumTotalAnalysis.
 */

	void initMomentumTotalAnalysis(bool enable = true);

/**
 * method  returns true, if the momentumEFGCEventAnalysis
 * is enabled.
 */

	bool isMomentumEventAnalysisEnabled();

/**
 * method  returns true, if the momentumEFGCTotalAnalysis
 * is enabled.
 */

	bool isMomentumTotalAnalysisEnabled();

/**
 * method returns a string representing the result of the
 * momentumEFGCEvent-Analysis.
 */

	std::string getMomentumEFGCEventAnalysis();

/**
 * method returns a string representing the result of the
 * momentumEFGCTotal-Analysis.
 */

	std::string getMomentumEFGCTotalAnalysis();

};

#endif

