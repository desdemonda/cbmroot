/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - implements methods to show analysis
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:25 $
// $Revision: 1.11 $
//
// *******************************************************************/


#ifndef _SHOWANALYSIS_H
#define _SHOWANALYSIS_H


#include "../../DataObjectLIB/include/magneticFieldValue.h"
#include "../../DataObjectLIB/include/table.h"
#include "TCanvas.h"
#include "TGraph.h"


/* **************************************************************
 * CLASS showAnalysis					 						*
 * **************************************************************/

class showAnalysis {

 private:
  showAnalysis(const showAnalysis&);
  showAnalysis& operator=(const showAnalysis&);

protected:

	TCanvas**      window;
	unsigned short numberOfWindows;
	TGraph**       display;
	unsigned short numberOfDisplays;
	TCanvas*       correctLayerWindow;
	TGraph*        correctLayerDisplay;
	unsigned short correctLayerAdditions;
	TCanvas*       correctSlopeWindow;
	TGraph*        correctSlopeDisplay;
	unsigned short correctSlopeAdditions;
	TCanvas*       layerWindow;
	TGraph*        layerDisplay;
	unsigned short layerAdditions;
	TCanvas*       signatureWindow;
	TGraph*        signatureDisplay;
	unsigned short signatureAdditions;
	TCanvas*       correctPrelutCellWindow;
	TGraph*        correctPrelutCellDisplay;
	unsigned short correctPrelutCellAdditions;
	TCanvas*       prelutCellWindow;
	TGraph*        prelutCellDisplay;
	unsigned short prelutCellAdditions;
	TCanvas*       correctLutCellWindow;
	TGraph*        correctLutCellDisplay;
	unsigned short correctLutCellAdditions;
	TCanvas*       lutCellWindow;
	TGraph*        lutCellDisplay;
	unsigned short lutCellAdditions;
	TCanvas*       momentaErrorWindow;
	TGraph*        momentaErrorDisplay;
	unsigned short momentaErrorAdditions;
	TCanvas*       momentaXWindow;
	TGraph*        momentaXDisplay;
	unsigned short momentaXAdditions;
	TCanvas*       momentaYWindow;
	TGraph*        momentaYDisplay;
	unsigned short momentaYAdditions;
	TCanvas*       momentaZWindow;
	TGraph*        momentaZDisplay;
	unsigned short momentaZAdditions;
	TCanvas*       dim1Window;
	TGraph*        dim1Display;
	unsigned short dim1Additions;
	TCanvas*       dim2Window;
	TGraph*        dim2Display;
	unsigned short dim2Additions;
	TCanvas*       dim3Window;
	TGraph*        dim3Display;
	unsigned short dim3Additions;
	TCanvas*       momentaWindow;
	TGraph*        momentaDisplay;
	unsigned short momentaAdditions;
	TCanvas*       coordinateWindow;
	TGraph*        coordinateDisplay;
	unsigned short coordinateAdditions;
	TCanvas*       dim1PeakDistanceWindow;
	TGraph*        dim1PeakDistanceDisplay;
	unsigned short dim1PeakDistanceAdditions;
	TCanvas*       dim2PeakDistanceWindow;
	TGraph*        dim2PeakDistanceDisplay;
	unsigned short dim2PeakDistanceAdditions;
	TCanvas*       dim3PeakDistanceWindow;
	TGraph*        dim3PeakDistanceDisplay;
	unsigned short dim3PeakDistanceAdditions;
	TCanvas*       accumulatedPeakDistanceWindow;
	TGraph*        accumulatedPeakDistanceDisplay;
	unsigned short accumulatedPeakDistanceAdditions;
	TCanvas*       minimalTrackColumnWindow;
	TGraph*        minimalTrackColumnDisplay;
	unsigned short minimalTrackColumnAdditions;
	TCanvas*       averageTrackColumnWindow;
	TGraph*        averageTrackColumnDisplay;
	unsigned short averageTrackColumnAdditions;
	TCanvas*       maximalTrackColumnWindow;
	TGraph*        maximalTrackColumnDisplay;
	unsigned short maximalTrackColumnAdditions;
	TCanvas*       fifoForColumnWindow;
	TGraph*        fifoForColumnDisplay;
	unsigned short fifoForColumnAdditions;
	TCanvas*       minimalTrackRowWindow;
	TGraph*        minimalTrackRowDisplay;
	unsigned short minimalTrackRowAdditions;
	TCanvas*       averageTrackRowWindow;
	TGraph*        averageTrackRowDisplay;
	unsigned short averageTrackRowAdditions;
	TCanvas*       maximalTrackRowWindow;
	TGraph*        maximalTrackRowDisplay;
	unsigned short maximalTrackRowAdditions;
	TCanvas*       fifoForRowWindow;
	TGraph*        fifoForRowDisplay;
	unsigned short fifoForRowAdditions;
	TCanvas*       trackLayerWindow;
	TGraph*        trackLayerDisplay;
	unsigned short trackLayerAdditions;
	TCanvas*       trackDensityLayerWindow;
	TGraph*        trackDensityLayerDisplay;
	unsigned short trackDensityLayerAdditions;
	TCanvas*       hitReadoutDistributionWindow;
	TGraph*        hitReadoutDistributionDisplay;
	unsigned short hitReadoutDistributionAdditions;
	TCanvas*       hitReadoutMeanDistributionDistributionWindow;
	TGraph*        hitReadoutMeanDistributionDistributionDisplay;
	unsigned short hitReadoutMeanDistributionDistributionAdditions;
	TCanvas*       fpgaHistogramProcessingTimeDistributionDistributionWindow;
	TGraph*        fpgaHistogramProcessingTimeDistributionDistributionDisplay;
	unsigned short fpgaHistogramProcessingTimeDistributionDistributionAdditions;

/**
 * This method initializes the global style for each display.
 */

	void initWindowStyle();

/**
 * This method initializes the window with a name and a title.
 * @param name is the name of the window
 * @param title is the title of the window
 */

	TCanvas* addWindow(std::string name, std::string title);

/**
 * This method initializes the display.
 */

	TGraph* addDisplay(int numberOfEntries, double* arrayX, double* arrayY);

/**
 * method updates the corresponding window- and display-object.
 */

	void updateWindowAndDisplay(TCanvas* windowToUpdate, TGraph* displayToUpdate, unsigned short numberOfAdditions, double* arrayY);

public:

/**
 * Default constructor
 */

	showAnalysis();

/**
 * Destructor
 */

	virtual ~showAnalysis();

/**
 * method adds a display to show the distribution of the correct layers for the hits
 */

	void addCorrectLayerDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the correct slopes for the hits
 */

	void addCorrectSlopeDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the layers for the hits
 */

	void addLayerDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the yz-projection of one track
 */

	void addYZProjection(double* arrayY, double* arrayZ, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the tracks' hit signature
 */

	void addSignatureDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the correct cells for the hits based on the prelut
 */

	void addCorrectPrelutCellDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the cells for the hits based on the prelut
 */

	void addPrelutCellDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the correct cells for the hits based on the lut
 */

	void addCorrectLutCellDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the cells for the hits based on the lut
 */

	void addLutCellDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the magnetfield factors dependant of z
 */

	void addMagnetfieldFactorsZ(double* arrayZ, double* arrayM, unsigned short numberOfEntries);

/**
 * method adds a display to show the magnetfield values dependant of z
 */

	void addMagnetfieldValuesZ(double* arrayZ, magneticFieldValue* arrayM, unsigned short numberOfEntries);

/**
 * method adds a display to show the integrated magnetfield values dependant of z
 */

	void addIntegratedMagnetfieldValuesZ(double* arrayZ, magneticFieldValue* arrayM, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the percentage of errors for the momentaError
 */

	void addMomentaErrorDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the momentas
 */

	void addMomentaXDistribution(unsigned int* array, unsigned short numberOfEntries, double min, double max);
	void addMomentaYDistribution(unsigned int* array, unsigned short numberOfEntries, double min, double max);
	void addMomentaZDistribution(unsigned int* array, unsigned short numberOfEntries, double min, double max);

/**
 * method adds a display to show the distribution of the coordinates
 */

	void addHoughspaceDim1Distribution(unsigned int* array, unsigned short numberOfEntries);
	void addHoughspaceDim2Distribution(unsigned int* array, unsigned short numberOfEntries);
	void addHoughspaceDim3Distribution(unsigned int* array, unsigned short numberOfEntries);
	void addMomentaDistribution(unsigned int* array, unsigned int numberOfEntries);
	void addCoordinateDistribution(unsigned int* array, unsigned int numberOfEntries);

/**
 * method adds a display to show the distribution of the peak distances
 */

	void addDim1PeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries);
	void addDim2PeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries);
	void addDim3PeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries);
	void addAccumulatedPeakDistanceDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * minimal found tracks' column
 */

	void addMinimalTrackColumnDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * average found tracks' column
 */

	void addAverageTrackColumnDistribution(double* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis);

/**
 * method adds a display to show the distribution of the
 * maximal found tracks' column
 */

	void addMaximalTrackColumnDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * fifos for the found tracks' column
 */

	void addFifoForColumnDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * minimal found tracks' row
 */

	void addMinimalTrackRowDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * average found tracks' row
 */

	void addAverageTrackRowDistribution(double* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis);

/**
 * method adds a display to show the distribution of the
 * maximal found tracks' row
 */

	void addMaximalTrackRowDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * fifos for the found tracks' row
 */

	void addFifoForRowDistribution(unsigned int* array, unsigned short numberOfEntries);

/**
 * method adds a display to show the distribution of the
 * maximal found tracks' layer
 */

	void addTrackLayerDistribution(unsigned int* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis);

/**
 * method adds a display to show the distribution of the
 * maximal found tracks' density layer
 */

	void addTrackDensityLayerDistribution(int* array, unsigned short numberOfEntries, unsigned int numberOfAnalysis);

/**
 * method adds a display to show the distribution of the
 * hit readouts while histogramming
 */

	void addHitReadoutDistribution(unsigned int* array, unsigned int numberOfEntries, unsigned int numberOfAnalysis);

/**
 * method adds a display to show the mean of the
 * hit readouts with different parallely implemented
 * histogram layers while histogramming
 */

	void addHitReadoutMeanDistribution(unsigned int* array, unsigned int numberOfEntries, unsigned int numberOfAnalysis);

/**
 * method adds a display to show the time 
 * consumption of the histogram processing with
 * different parallely implementedhistogram layers
 */

	void addFpgaHistogramProcessingTimeDistribution(unsigned int* array, unsigned int numberOfEntries, unsigned int numberOfAnalysis, bool readoutColumnsInParallel, unsigned short histogramDim1, unsigned short histogramDim2);

/**
 * method adds a display to show the distribution of the
 * two given arrays
 */

	void addDistribution(double* arrayX, double* arrayY, unsigned short numberOfEntries, std::string name, std::string title, std::string xAxiTitle, std::string yAxiTitle);

/**
 * method adds a display to show the distribution of the
 * given table
 */

	void addTable(table& source, std::string name, std::string title, std::string xAxiTitle, std::string yAxiTitle);

};

#endif
