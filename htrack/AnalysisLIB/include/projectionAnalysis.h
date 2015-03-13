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
///     - implements methods to display the histogram's occupancy
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PROJECTIONANALYSIS_H
#define _PROJECTIONANALYSIS_H


#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "projectionEFGCNEvent12Analysis.h"
#include "projectionEFGCNEvent13Analysis.h"
#include "projectionEFGCNEvent32Analysis.h"
#include "projectionEFGCNTotal12Analysis.h"
#include "projectionEFGCNTotal13Analysis.h"
#include "projectionEFGCNTotal32Analysis.h"
#include "TCanvas.h"
#include <string>


/* **************************************************************
 * CLASS projectionAnalysis				 						*
 * **************************************************************/

class projectionAnalysis {

 private:
  projectionAnalysis(const projectionAnalysis&);
  projectionAnalysis& operator=(const projectionAnalysis&);

protected:

	TCanvas*                        window;						/**< Object for the main window */
	unsigned short                  numberOfWindowPadColumns;	/**< Number of columns which divide the window into pads */
	unsigned short                  numberOfWindowPadRows;		/**< Number of rows which divide the window into pads */
	bool                            enableDisplay;				/**< Variable to store the enable-state of the window */
	projectionEFGCNEvent12Analysis* efgcnEvent12;				/**< Object to project the histogram's efficiency, fakes, ghosts, clones and not-founds occupancy for each event into dimensions 1 and 2 */
	projectionEFGCNEvent13Analysis* efgcnEvent13;				/**< Object to project the histogram's efficiency, fakes, ghosts, clones and not-founds occupancy for each event into dimensions 1 and 3 */
	projectionEFGCNEvent32Analysis* efgcnEvent32;				/**< Object to project the histogram's efficiency, fakes, ghosts, clones and not-founds occupancy for each event into dimensions 2 and 2 */
	projectionEFGCNTotal12Analysis* efgcnTotal12;				/**< Object to project the histogram's efficiency, fakes, ghosts, clones and not-founds occupancy accumulated for all events into dimensions 1 and 2 */
	projectionEFGCNTotal13Analysis* efgcnTotal13;				/**< Object to project the histogram's efficiency, fakes, ghosts, clones and not-founds occupancy accumulated for all events into dimensions 1 and 3 */
	projectionEFGCNTotal32Analysis* efgcnTotal32;				/**< Object to project the histogram's efficiency, fakes, ghosts, clones and not-founds occupancy accumulated for all events into dimensions 3 and 2 */
	bool                            efgcnEventAnalysis;			/**< Variable to store the enable-state of the efficiency-, the fakes-, the ghosts-, the clones- and the not-found-analysis for each event */
	bool                            efgcnTotalAnalysis;			/**< Variable to store the enable-state of the accumulated efficiency-, the fakes-, the ghosts-, the clones- and the not-found-analysis for all events */
	std::string                     outputFileName;				/**< Variable to store the name of the file for the output */

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
 * @see projectionAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows)
 */

	void evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature);

/**
 * This method evaluates the window division factors.
 * @return numberOfEnabledPadColumns is the number of window-pad-columns to display something
 * @return numberOfEnabledPadRows is the number of window-pad-rows to display something
 * @see projectionAnalysis::projectionAnalysisDraw()
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

	projectionAnalysis();

/**
 * Destructor
 */

	virtual ~projectionAnalysis();

/**
 * This method initializes the root directory for the displays.
 * @param enable is the enabling-flag
 * @param name is the name of the file. If null is supplied it is written to the standard root output file
 * @param justUpdate just updates the file instead of recreating
 */

	void initProjectionAnalysisToRoot(bool enable = true, const char* name = NULL, bool justUpdate = false);

/**
 * This method returns true if the projection's display is written to file.
 */

	bool isProjectionToRootEnabled();

/**
 * This method initializes the projection's display.
 */

	void initProjectionAnalysisDisplay(bool enable = true);

/**
 * This method returns true if the projection's display is enabled.
 */

	bool isProjectionDisplayEnabled();

/**
 * method initializes the projectionEFGCNEvent12E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12EAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent12F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12FAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent12G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12GAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent12C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12CAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent12N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12NAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent12EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12EFGCNAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent13E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13EAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent13F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13FAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent13G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13GAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent13C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13CAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent13N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13NAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent13EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13EFGCNAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent32E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32EAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent32F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32FAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent32G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32GAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent32C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32CAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent32N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32NAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNEvent32EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32EFGCNAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal12E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12EAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal12F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12FAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal12G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12GAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal12C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12CAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal12N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12NAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal12EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12EFGCNAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal13E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13EAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal13F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13FAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal13G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13GAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal13C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13CAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal13N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13NAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal13EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13EFGCNAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal32E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32EAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal32F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32FAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal32G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32GAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal32C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32CAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal32N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32NAnalysis(bool enable, int nXBins, int nYBins);

/**
 * method initializes the projectionEFGCNTotal32EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32EFGCNAnalysis(bool enable, int nXBins, int nYBins);

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
 * @param coordinates is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToFakeTrack(trackCoordinates& coordinates);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToNoTrack(trackCoordinates& coordinates);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        input track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToOneTrack(trackCoordinates& coordinates, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        input track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToMoreTracks(trackCoordinates& coordinates, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        found track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToNoPeak(trackCoordinates& coordinates);

/**
 * method returns true, if the projectionEFGCNEvent12-Analysis is
 * enabled.
 */

	bool isProjectionEFGCNEvent12AnalysisEnabled();

/**
 * method returns true, if the projectionEFGCNEvent13-Analysis is
 * enabled.
 */

	bool isProjectionEFGCNEvent13AnalysisEnabled();

/**
 * method returns true, if the projectionEFGCNEvent32-Analysis is
 * enabled.
 */

	bool isProjectionEFGCNEvent32AnalysisEnabled();

/**
 * method returns true, if the projectionEFGCNTotal12-Analysis is
 * enabled.
 */

	bool isProjectionEFGCNTotal12AnalysisEnabled();

/**
 * method returns true, if the projectionEFGCNTotal13-Analysis is
 * enabled.
 */

	bool isProjectionEFGCNTotal13AnalysisEnabled();

/**
 * method returns true, if the projectionEFGCNTotal32-Analysis is
 * enabled.
 */

	bool isProjectionEFGCNTotal32AnalysisEnabled();

/**
 * This method updates the window.
 */

	void projectionAnalysisUpdate();

/**
 * This method draws the window.
 */

	void projectionAnalysisDraw();

/**
 * method writes the projectionEvent-Analysis for each event into
 * a root file.
 */

	void projectionEventAnalysisWrite(int eventNumber);

/**
 * method writes the projectionTotal-Analysis over all events into
 * a root file.
 */

	void projectionTotalAnalysisWrite(int eventNumber);

/**
 * method initializes the projectionEventAnalysis.
 */

	void initProjectionEventAnalysis(bool enable = true);

/**
 * method initializes the projectionTotalAnalysis.
 */

	void initProjectionTotalAnalysis(bool enable = true);

/**
 * method  returns true, if the projectionEFGCNEventAnalysis
 * is enabled.
 */

	bool isProjectionEventAnalysisEnabled();

/**
 * method  returns true, if the projectionEFGCNTotalAnalysis
 * is enabled.
 */

	bool isProjectionTotalAnalysisEnabled();

/**
 * method  controls the projectionEFGCN12EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent12ERange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent12FRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent12GRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent12CRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent12NRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent12EFGCNRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN13EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent13ERange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent13FRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent13GRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent13CRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent13NRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent13EFGCNRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN32EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent32ERange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent32FRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent32GRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent32CRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent32NRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32EventAnalysis for the range.
 */

	void controlProjectionEFGCNEvent32EFGCNRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN12TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal12ERange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal12FRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal12GRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal12CRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal12NRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN12TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal12EFGCNRange(unsigned short range1, unsigned short range2);

/**
 * method  controls the projectionEFGCN13TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal13ERange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal13FRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal13GRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal13CRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal13NRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN13TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal13EFGCNRange(unsigned short range1, unsigned short range3);

/**
 * method  controls the projectionEFGCN32TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal32ERange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal32FRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal32GRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal32CRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal32NRange(unsigned short range3, unsigned short range2);

/**
 * method  controls the projectionEFGCN32TotalAnalysis for the range.
 */

	void controlProjectionEFGCNTotal32EFGCNRange(unsigned short range3, unsigned short range2);

};

#endif
