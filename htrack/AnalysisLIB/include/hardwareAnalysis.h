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
//     - implements methods to analyze hardware prerequisites
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:24 $
// $Revision: 1.5 $
//
// *******************************************************************/


#ifndef _HARDWAREANALYSIS_H
#define _HARDWAREANALYSIS_H


#include "../../DataRootObjectLIB/include/histogramData.h"
#include "../../DataRootObjectLIB/include/trackData.h"


/* **************************************************************
 * CLASS hardwareAnalysis				 						*
 * **************************************************************/

class hardwareAnalysis {

 private:
  hardwareAnalysis(const hardwareAnalysis&);
  hardwareAnalysis& operator=(const hardwareAnalysis&);

protected:

	unsigned int  numberOfColumns;							/**< Variable to store the number of entries for all column distributions. */
	unsigned int  numberOfRows;								/**< Variable to store the number of histogram rows to compute a correct fifo size distribution. */
	bool          isTracksPerColumnEnabled;					/**< Variable for enabling the evaluation of the number of tracks per column in all histogram layers. */
	unsigned int  numberOfColumnAnalysis;					/**< Variable to store the number of analysis which are made to evaluate the number of tracks per column in all histogram layers. */
	unsigned int* minimalNumberOfTracksPerColumn;			/**< Variable to store the column distribution for the minimal number of tracks in all histogram layers. */
	double*       averageNumberOfTracksPerColumn;			/**< Variable to store the column distribution for the average number of tracks in all histogram layers. */
	unsigned int* maximalNumberOfTracksPerColumn;			/**< Variable to store the column distribution for the maximal number of tracks in all histogram layers. */
	unsigned int* numberOfFifoSizesPerColumn;				/**< Variable to store the column distribution for the fifo sizes for the tracks in all histogram layers. */
	bool          isTracksPerRowEnabled;					/**< Variable for enabling the evaluation of the number of tracks per row in all histogram layers. */
	unsigned int  numberOfRowAnalysis;						/**< Variable to store the number of analysis which are made to evaluate the number of tracks per row in all histogram layers. */
	unsigned int* minimalNumberOfTracksPerRow;				/**< Variable to store the row distribution for the minimal number of tracks in all histogram layers. */
	double*       averageNumberOfTracksPerRow;				/**< Variable to store the row distribution for the average number of tracks in all histogram layers. */
	unsigned int* maximalNumberOfTracksPerRow;				/**< Variable to store the row distribution for the maximal number of tracks in all histogram layers. */
	unsigned int* numberOfFifoSizesPerRow;					/**< Variable to store the row distribution for the fifo sizes for the tracks in all histogram layers. */
	bool          isTracksPerLayerEnabled;					/**< Variable for enabling the evaluation of the number of tracks per histogram layer. */
	unsigned int  numberOfLayerAnalysis;					/**< Variable to store the number of analysis which are made to evaluate the number of tracks per histogram layer. */
	unsigned int  numberOfTracksInAllLayerEntries;			/**< Varibale to store the number of entries for all layer distributions. */
	unsigned int* numberOfTracksPerLayer;					/**< Variable to store the distribution of the number of tracks per layer. */
	int*          numberOfTrackDensitiesPerLayer;			/**< Variable to store the distribution of the track densities for the number of tracks per layer. */
	bool          isHistogramLayerDistributionEnabled;		/**< Variable for enabling the evaluation of the distribution of the histogram layers for the hits. */
	unsigned int  numberOfHitReadoutDistributionAnalysis;	/**< Variable to store the number of analysis which are made to evaluate the histogram layer distributions for the hits. */
	unsigned int  numberOfHitReadoutDistributionEntries;	/**< Varibale to store the number of entries for the histogram layer distributions for the hits. */
	unsigned int* hitReadoutDistribution;					/**< Variable to store the distribution of the histogram layers for the hits. */

public:

/**
 * Default constructor
 */

	hardwareAnalysis();

/**
 * Destructor
 */

	virtual ~hardwareAnalysis();

/**
 * method returns if the analysis for the number of
 * tracks in all columns of the histogram layers is
 * enabled or not
 */

	bool isNumberOfTracksInAllColumnsAnalysisEnabled();

/**
 * method returns if the analysis for the number of
 * tracks in all rows of the histogram layers is
 * enabled or not
 */

	bool isNumberOfTracksInAllRowsAnalysisEnabled();

/**
 * method returns the number of actually made column anaylsis
 */

	unsigned int getNumberOfColumnAnalysis();

/**
 * method returns the number of actually made row anaylsis
 */

	unsigned int getNumberOfRowAnalysis();

/**
 * method returns the number of entries in the distribution
 * for all number of tracks per column in all layers
 */

	unsigned int getNumberOfTracksInAllColumnEntries();

/**
 * method returns the number of entries in the distribution
 * for all number of tracks per row in all layers
 */

	unsigned int getNumberOfTracksInAllRowEntries();

/**
 * method returns the distribution for the minimal number
 * of tracks per column in all layers
 */

	unsigned int* getMinimalNumberOfTracksInAllColumnsDistribution();

/**
 * method returns the distribution for the minimal number
 * of tracks per row in all layers
 */

	unsigned int* getMinimalNumberOfTracksInAllRowsDistribution();

/**
 * method returns the distribution for the average number
 * of tracks per column in all layers
 */

	double* getAverageNumberOfTracksInAllColumnsDistribution();

/**
 * method returns the distribution for the average number
 * of tracks per row in all layers
 */

	double* getAverageNumberOfTracksInAllRowsDistribution();

/**
 * method returns the distribution for the maximal number
 * of tracks per column in all layers
 */

	unsigned int* getMaximalNumberOfTracksInAllColumnsDistribution();

/**
 * method returns the distribution for the maximal number
 * of tracks per row in all layers
 */

	unsigned int* getMaximalNumberOfTracksInAllRowsDistribution();

/**
 * method returns the distribution for the fifo sizes for
 * the tracks per column in all layers
 */

	unsigned int* getNumberOfFifoSizesInAllColumnsDistribution();

/**
 * method returns the distribution for the fifo sizes for
 * the tracks per row in all layers
 */

	unsigned int* getNumberOfFifoSizesInAllRowsDistribution();

/**
 * method initializes the hardware histogram dimensions.
 * @param numberOfColumns determines the number of columns of the histogram
 * @param numberOfRows determines the number of rows in the histogram
 */

	void initHardwareHistogramDimensions(unsigned short numberOfColumns, unsigned short numberOfRows);

/**
 * method initializes the hardwareTracksPerColumnAnalysis.
 * @param enable determines the processing of the analysis or not
 */

	void initHardwareTracksPerColumnAnalysis(bool enable = true);

/**
 * method initializes the hardwareTracksPerRowAnalysis.
 * @param enable determines the processing of the analysis or not
 */

	void initHardwareTracksPerRowAnalysis(bool enable = true);

/**
 * This method evaluates the minimal, the maximal and
 * the average number of tracks in all columns of the histogram.
 */

	void evaluateNumberOfTracksInAllColumns(trackData* tracks, std::streambuf* terminal = NULL);

/**
 * This method evaluates the minimal, the maximal and
 * the average number of tracks in all rows of the histogram.
 */

	void evaluateNumberOfTracksInAllRows(trackData* tracks, std::streambuf* terminal = NULL);

/**
 * method returns if the analysis for the number of
 * tracks in all histogram layers is enabled or not
 */

	bool isNumberOfTracksInAllLayersAnalysisEnabled();

/**
 * method returns the number of actually made layer anaylsis
 */

	unsigned int getNumberOfLayerAnalysis();

/**
 * method returns the number of entries in the distribution
 * for all number of tracks in layers
 */

	unsigned int getNumberOfTracksInAllLayersEntries();

/**
 * method returns the distribution for the number
 * of tracks per layers
 */

	unsigned int* getNumberOfTracksPerLayerDistribution();

/**
 * method returns the distribution for the number
 * of track densities per layer
 */

	int* getNumberOfTrackDensitiesPerLayerDistribution();

/**
 * method initializes the hardwareTracksPerLayerAnalysis.
 */

	void initHardwareTracksPerLayerAnalysis(unsigned int numberOfTracksInAllLayerEntries, bool enable = true);

/**
 * This method evaluates the distribution of the number of tracks
 * in all layers of the histogram.
 */

	void evaluateNumberOfTracksInAllLayers(trackData* tracks, std::streambuf* terminal = NULL);

/**
 * method returns if the analysis for the distribution how
 * often a hit must be inserted in a layer is enabled or not
 */

	bool isHitReadoutDistributionAnalysisEnabled();

/**
 * method initializes the hardwareHistogramLayerDistributionAnalysis.
 */

	void initHardwareHitReadoutDistributionAnalysis(unsigned int numberOfLayers, bool enable = true);

/**
 * This method evaluates the distribution how often a hit must
 * be inserted in a layer.
 */

	void evaluateHitReadoutDistribution(histogramData* histogram, std::streambuf* terminal = NULL);

/**
 * method returns the number of actually made hit readout 
 * distribution analysis
 */

	unsigned int getNumberOfHitReadoutDistributionAnalysis();

/**
 * method returns the number of entries in the hit readout
 * distribution
 */

	unsigned int getNumberOfHitReadoutDistributionEntries();

/**
 * method returns the distribution for the hit readout. This
 * tells how often the hits must be preccessed for the whole
 * histogramming.
 */

	unsigned int* getHitReadoutDistribution();

};

#endif
