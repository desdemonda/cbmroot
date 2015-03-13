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
///     - class implements the trackfinding algorithm
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:43:10 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDER_H
#define _TRACKFINDER_H


#include "../../DataRootObjectLIB/include/trackfinderInputData.h"
#include "../../HoughTransformationLIB/include/houghTransformation.h"
#include "../../HistogramTransformationLIB/include/eraser.h"
#include "../../HistogramTransformationLIB/include/maxMorphSearch.h"
#include "../../HistogramTransformationLIB/include/autoFinder.h"
#include "../../AnalysisLIB/include/analysis.h"


/* **************************************************************
 * CLASS trackfinder							 				*
 * **************************************************************/

class trackfinder {

private:

  trackfinder(const trackfinder&);
  trackfinder& operator=(const trackfinder&);

	houghTransformation*     houghTransform;				/**< Object which fills the histogram by transforming the hits */
	histogramTransformation* histoTransform;				/**< Object which evaluates the tracks basing on the histogram */
	unsigned short           filterType;					/**< Variable to store the general filter type. */
	unsigned short           sourceDataPercentage;			/**< Variable to store the percentage of the source data which should be used. Less than 100% speeds the algorithm up. */
	std::string              filterFileName;				/**< Variable to store the name of the file. */

#ifndef NOANALYSIS

	analysis*                analyser;						/**< Object to analyse the trackfinder internally. */

#endif

/**
 * This applies the algorithm to evaluate the filter geometry
 * @param terminal is a buffer to place the process information
 */

	void evaluatePeakfindingGeometry(std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	trackfinder();

/**
 * Constructor
 * @param eventData is an object for accessing the input data
 * @return histogram is an object for accessing the histogram
 * @return tracks is an object for accessing the tracks
 * @param ratings is the object to access the tables for the ratings
 * @param lut is an object for accessing both look-up-tables
 * @param filterType defines the type of the filter
 * @param sourceDataPercentage defines the percentage of the source data which should be used. Less than 100% speeds the algorithm up
 * @param filterFileName defines the name of the file
 * @param firstFilterGeometry defines the geometry of the first applied filter
 * @param firstFilterArithmetic defines the arithmetic which is used in the first applied filter
 * @param secondFilterGeometry defines the geometry of the second applied filter
 * @param secondFilterArithmetic defines the arithmetic which is used in the second applied filter
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	trackfinder(trackfinderInputData** eventData, histogramData** histogram,
			    trackData** tracks, tables** ratings, lutImplementation** lut,
				unsigned short filterType, unsigned short sourceDataPercentage,
				std::string filterFileName,
				unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
				unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
				unsigned short firstFilterDim1ClearRadius,
				unsigned short firstFilterDim2ClearRadius,
				unsigned short secondFilterDim1ClearRadius,
				unsigned short secondFilterDim2ClearRadius,
				unsigned short secondFilterDim3ClearRadius);


/**
 * Destructor
 */

	virtual ~trackfinder();

/**
 * Method inits the variables based on the detector stations.
 * @param eventData is an object for accessing the input data
 * @return histogram is an object for accessing the histogram
 * @return tracks is an object for accessing the tracks
 * @param ratings is the object to access the tables for the ratings
 * @param lut is an object for accessing both look-up-tables
 * @param filterType defines the type of the filter
 * @param sourceDataPercentage defines the percentage of the source data which should be used. Less than 100% speeds the algorithm up
 * @param filterFileName defines the name of the file
 * @param firstFilterGeometry defines the geometry of the first applied filter
 * @param firstFilterArithmetic defines the arithmetic which is used in the first applied filter
 * @param secondFilterGeometry defines the geometry of the second applied filter
 * @param secondFilterArithmetic defines the arithmetic which is used in the second applied filter
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	void init(trackfinderInputData** eventData, histogramData** histogram,
			  trackData** tracks, tables** ratings, lutImplementation** lut,
			  unsigned short filterType, unsigned short sourceDataPercentage,
			  std::string filterFileName,
			  unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
			  unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
			  unsigned short firstFilterDim1ClearRadius,
			  unsigned short firstFilterDim2ClearRadius,
			  unsigned short secondFilterDim1ClearRadius,
			  unsigned short secondFilterDim2ClearRadius,
			  unsigned short secondFilterDim3ClearRadius);

/**
 * This method sets the object for analysing the trackfinding algorithm
 * @param analyser is a pointer to the analysis object which has the ability to analyze the algorithm
 * @warning The analysis object must have the functionality of: writeCreatedHistogramLayer(), writeEncodedHistogramLayer() and writeFilteredHistogramLayer()
 */

	void setAnalyser(analysis* analyser = NULL);

/**
 * This applies the track-finding algorithm
 * @param terminal is a buffer to place the process information
 */

	void evaluate(std::streambuf* terminal = NULL);

/**
 * This applies the algorithm to generate the filter geometry
 * @param filterCoverPercentage sets the percentage of occurence frequency for a geometry element to be used
 * @param isFirstEvent is a flag which determines the first event
 * @param terminal is a buffer to place the process information
 */

	void generateFilterGeometry(unsigned short filterCoverPercentage, bool isFirstEvent, std::streambuf* terminal = NULL);

/**
 * This method writes the peakfinding geometry into a file
 * @param terminal is a buffer to place the process information
 */

	void writePeakfindingGeometry(std::streambuf* terminal = NULL);

/**
 * This method draws the peakfinding geometry
 */

	void drawPeakfindingGeometry();
	void drawProjectedPeakfindingGeometry();
	void drawCoveredPeakfindingGeometry();
	void drawCoveredProjectedPeakfindingGeometry();

/**
 * This method returns a string representation for the
 * peakfinding geometry
 */

	std::string peakfindingGeometryToString();
	std::string projectedPeakfindingGeometryToString();
	std::string coveredPeakfindingGeometryToString();
	std::string coveredProjectedPeakfindingGeometryToString();

/**
 * method returns true if the automatic generation of the filter
 * geometry is enabled.
 */

	bool isAutomaticFilterGeometryEnabled();

/**
 * method returns true if the generation of the filter
 * geometry is enabled.
 */

	bool isFilterGeometryGenerationEnabled();

/**
 * This method returns the trackIndex of the debugged track,
 * if DEBUGJUSTONEGOODTRACK is enabled.
 */

	int getDebugTrackIndex();

};

#endif
