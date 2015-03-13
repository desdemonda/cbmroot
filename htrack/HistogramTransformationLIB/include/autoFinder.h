//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
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
///     - class for evaluating the histogram
///     - class computes the tracks
///
///     CAUTION: This class cannot be backward compatible with Version 4.0.
///              This is because of the histogram reduction from 3D to 2D. So
///              the filter had to be reduced in the corresponding way. And
///              the reduction of the filter cannot be done bittrue.
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:40:41 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _AUTOFINDER_H
#define _AUTOFINDER_H


#include "../../DataRootObjectLIB/include/peakfindingGeometry.h"
#include "../../MiscLIB/include/bitArray.h"
#include "filterBasicStyle.h"
#include "histogramTransformation.h"


typedef struct {

	bitArray* pointer;
	bool      isLocal;

}geometryMemoryProfile;


/* **************************************************************
 * CLASS autoFinder			 									*
 * **************************************************************/

class autoFinder : public histogramTransformation {

 private:

  autoFinder(const autoFinder&);
  autoFinder& operator=(const autoFinder&);

protected:

	peakfindingGeometry    geometry;					/**< Object implements the geometry which should be used for peak finding. */
	bool                   useFirstFilterMod;			/**< Variable to store the way of filtering for the 2D peakfinding. If the value is true, the way is defined by not directly overwriting the histogram values. */
	unsigned short         firstFilterMiddleIndex;		/**< Variable defines the middle index in the peakfinding geometry and is used by the firstBaseFilter object. */
	unsigned short         sizeOfFirstFilterMem;		/**< Variable stores the size of memory allocated for the firstFilterMem array. */
	bitArray*              firstFilterMem;				/**< Array to store the input values for 2D peakfinding. */
	unsigned short         sizeOfFirstBufferMem;		/**< Variable stores the size of memory allocated for the firstBufferMem array. */
	geometryMemoryProfile* firstBufferMem;				/**< Array to fill the firstFilterMem by moving this memory like a window via the histogram layers. */
	filterBasicStyle*      firstBaseFilter;				/**< Object to build the filtering strategy for the 2D peakfinding. */
	bool                   useSecondFilterMod;			/**< Variable to store the way of filtering for the 3D peakfinding. If the value is true, the way is defined by not directly overwriting the histogram values. */
	unsigned short         secondFilterMiddleIndex;		/**< Variable defines the middle index in the peakfinding geometry and is used by the secondBaseFilter object. */
	unsigned short         sizeOfSecondFilterMem;		/**< Variable stores the size of memory allocated for the secondFilterMem array. */
	bitArray*              secondFilterMem;				/**< Array to store the input values for 3D peakfinding. */
	filterBasicStyle*      secondBaseFilter;			/**< Object to build the filtering strategy for the 3D peakfinding. */

/**
 * deletes allocated and initialized memory
 */

	void deleteMemory();
	void deleteFirstFilterMemory();
	void deleteSecondFilterMemory();

/**
 * This method inits the memory in a speciallized manner.
 */

	void initMemory();
	void initFirstFilterMemoryWithoutDirectOverwriting();
	void initSecondFilterMemoryWithoutDirectOverwriting();
	void initFirstFilterMemoryWithDirectOverwriting();
	void initSecondFilterMemoryWithDirectOverwriting();

/**
 * This method filters the peakclasses in a speciallized manner.
 */

	void filteringHistogramLayer();
	void filteringHistogramLayerWithoutDirectOverwriting();
	void filteringHistogramLayerWithDirectOverwriting();

/**
 * This method filters the tracks in a speciallized manner
 * again.
 * @param terminal is a buffer to place the process information
 */

	void filteringHistogram(std::streambuf* terminal = NULL);
	void filteringHistogramWithoutDirectOverwriting(std::streambuf* terminal = NULL);
	void filteringHistogramWithDirectOverwriting(std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	autoFinder();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param firstFilterArithmetic defines the arithmetic which is used in the first applied filter
 * @param useFirstFilterMod defines the way of filtering for the the first applied filter. If the value is true, the way is defined by not directly overwriting the histogram values.
 * @param secondFilterArithmetic defines the arithmetic which is used in the second applied filter
 * @param useSecondFilterMod defines the way of filtering for the second applied filter. If the value is true, the way is defined by not directly overwriting the histogram values.
 */

	autoFinder(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short firstFilterArithmetic, bool useFirstFilterMod, unsigned short secondFilterArithmetic, bool useSecondFilterMod);

/**
 * Destructor
 */

	virtual ~autoFinder();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param firstFilterArithmetic defines the arithmetic which is used in the first applied filter
 * @param useFirstFilterMod defines the way of filtering for the first applied filter. If the value is true, the way is defined by not directly overwriting the histogram values.
 * @param secondFilterArithmetic defines the arithmetic which is used in the second applied filter
 * @param useSecondFilterMod defines the way of filtering for the second applied filter. If the value is true, the way is defined by not directly overwriting the histogram values.
 */

	void init(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short filterArithmetic, bool useFirstFilterMod, unsigned short secondFilterArithmetic, bool useSecondFilterMod);

/**
 * object setup for the peakfinding process
 * HINT: No peakfinding would be possible without a setup
 * call, if the object initialization is finished
 */

	void setup();

/**
 * get percentage of coverage for the peak finding geometry
 */

	unsigned short getPeakfindingCoverage();

/**
 * method returns the peakfinding geometry
 * @return the automatic evaluated peakfinding geometry
 */

	peakfindingGeometry getPeakfindingGeometry();

/**
 * set percentage of coverage for the peakfinding geometry
 */

	void setPeakfindingCoverage(unsigned short percentageOfCoverage);

/**
 * set whole other peakfinding geometry
 * @param actualPeakfindingGeometry is the geometry which should be added
 */

	void setPeakfindingGeometry(peakfindingGeometry& actualPeakfindingGeometry);

/**
 * updates whole other peakfinding geometry
 * @param actualPeakfindingGeometry is the geometry which should be added
 */

	void updatePeakfindingGeometry(peakfindingGeometry& actualPeakfindingGeometry);

/**
 * method reads a file to get the peakfinding geometry
 * @param fileName is the name of the file to read the data
 * @param terminal is a buffer to place the process information
 */

	void readPeakfindingGeometry(std::string fileName, std::streambuf* terminal = NULL);

/**
 * method writes a file representing the peak finding geometry
 * @param fileName is the name of the file to write the data
 * @param terminal is a buffer to place the process information
 */

	void writePeakfindingGeometry(std::string fileName, std::streambuf* terminal = NULL);

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

};

#endif
