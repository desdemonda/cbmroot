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
///     - class for handling the hough-transformation
///     - class computes the histogram
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:54 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HOUGHTRANSFORMATION_H
#define _HOUGHTRANSFORMATION_H


#include "../../DataRootObjectLIB/include/trackfinderInputData.h"
#include "../../DataRootObjectLIB/include/histogramData.h"
#include "../../LutGeneratorLIB/include/lutImplementation.h"
#include "../../AnalysisLIB/include/analysis.h"
#include <list>


/* **************************************************************
 * CLASS houghTransformation					 				*
 * **************************************************************/

class houghTransformation {

private:

  houghTransformation(const houghTransformation&);
  houghTransformation& operator=(const houghTransformation&);


	trackfinderInputData** eventData;			/**< Object for accessing the input data. */
	histogramData**        histogram;			/**< Object for accessing the histogram. */
	lutImplementation**    lut;					/**< Object for computing the borders of both look-up-tables. */
	unsigned short         actualLayer;			/**< Variable to store the actual number of the layer for computing. Needed to identify the next computation as next layer. */
	std::list<lutBorder*>  dynamicLayerMemory;	/**< Object to store the borders needed for following layers. */
	int                    debugTrackIndex;		/**< Variable to store the trackIndex, if the debugging of just one good track is enabled. */

/**
 * This method does the entries for one layer of the histogram
 * @param border is the actual border to do the entry
 * @param secondBorder is the border.lut object
 * @param hit is the hit-object which corresponds to the border
 */

	void doHistogramLayerEntry(std::vector<lutBorder>::iterator border);
	void doHistogramLayerEntry(lutBorder* border);
	void doHistogramLayerEntry(lutHoughBorder& secondBorder, trackfinderInputHit* hit);

public:

/**
 * Default constructor
 */

	houghTransformation();

/**
 * Constructor
 * @param eventData is an object for accessing the input data
 * @param histogram is an object for accessing the histogram
 * @param lut is an object for accessing both look-up-tables
 */

	houghTransformation(trackfinderInputData** eventData, histogramData** histogram, lutImplementation** lut);

/**
 * Destructor
 */

	virtual ~houghTransformation();

/**
 * This method initializes the object.
 * @param eventData is an object for accessing the input data
 * @param histogram is an object for accessing the histogram
 * @param lut is an object for accessing both look-up-tables
 */

	void init(trackfinderInputData** eventData, histogramData** histogram, lutImplementation** lut);

/**
 * This method creates the borders for each hit
 * @param track is a MCTrack with links to the corresponding hits
 * @param terminal is a buffer to place the process information
 * @param analyser is just used in combination with DEBUGJUSTONEGOODTRACK
 */

	void createBorders(trackfinderInputTrack* track);

#ifndef NOANALYSIS

	void createBorders(analysis* analyser = NULL, std::streambuf* terminal = NULL);

#else

	void createBorders(std::streambuf* terminal = NULL);

#endif

/**
 * This method reserves memory for the computation of the layer.
 */

	void reserveHistogramLayerMemory();

/**
 * This method creates one layer of the histogram
 * @param layer is the number of the layer to create
 */

	void createHistogramLayer(unsigned short layer);

/**
 * This method resets the layer of the histogram object.
 */

	void resetHistogramLayer();

/**
 * This method resets the histogram object.
 */

	void resetHistogram();

/**
 * This method returns the number of the histogram layers for
 * full processing.
 */

	unsigned short getNumberOfHistogramLayers();

/**
 * This method returns the trackIndex of the debugged track,
 * if DEBUGJUSTONEGOODTRACK is enabled.
 */

	int getDebugTrackIndex();

};

#endif
