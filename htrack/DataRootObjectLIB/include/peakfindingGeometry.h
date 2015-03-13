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
///     - class for holding the information of the automatic
///       generated filter geometry
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:39:20 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PEAKFINDINGGEOMETRY_H
#define _PEAKFINDINGGEOMETRY_H


#include "../../DataObjectLIB/include/specialListMem.h"
#include "../../DataObjectLIB/include/peakfindingGeometryElement.h"
#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "TCanvas.h"
#include "TH2S.h"
#include "TStyle.h"


/* **************************************************************
 * CLASS peakfindingGeometry					 				*
 * **************************************************************/

class peakfindingGeometry {

protected:

	unsigned short                             percentageOfCoverage;						/**< Variable to store the percentage of coverage for the peak finding geometry. */
	specialListMem<peakfindingGeometryElement> geometry;									/**< Object to store the elements of the peakfinding geometry. */
	TCanvas*                                   canvas;										/**< Object for the main window to show the original source data. */
	unsigned short                             numberOfCanvasPadColumns;					/**< Number of columns which divide the window into pads. */
	unsigned short                             numberOfCanvasPadRows;						/**< Number of rows which divide the window into pads. */
	TH2S**                                     displays;									/**< Variable to hold the object for each display of the original source data. */
	unsigned short                             numberOfDisplays;							/**< Variable to store the number of evaluated displays.*/
	TCanvas*                                   projectionCanvas;							/**< Object for the main window to show the projected source data. */
	unsigned short                             numberOfProjectionCanvasPadColumns;			/**< Number of columns which divide the window into pads. */
	unsigned short                             numberOfProjectionCanvasPadRows;				/**< Number of rows which divide the window into pads. */
	TH2S**                                     projectionDisplays;							/**< Variable to hold the object for each display of the projected source data. */
	unsigned short                             numberOfProjectionDisplays;					/**< Variable to store the number of evaluated displays.*/
	TCanvas*                                   coverageCanvas;								/**< Object for the main window to show the covered source data. */
	unsigned short                             numberOfCoverageCanvasPadColumns;			/**< Number of columns which divide the window into pads. */
	unsigned short                             numberOfCoverageCanvasPadRows;				/**< Number of rows which divide the window into pads. */
	TH2S**                                     coverageDisplays;							/**< Variable to hold the object for each display of the covered source data. */
	unsigned short                             numberOfCoverageDisplays;					/**< Variable to store the number of evaluated displays.*/
	TCanvas*                                   coverageProjectionCanvas;					/**< Object for the main window to show the covered and projected source data. */
	unsigned short                             numberOfCoverageProjectionCanvasPadColumns;	/**< Number of columns which divide the window into pads. */
	unsigned short                             numberOfCoverageProjectionCanvasPadRows;		/**< Number of rows which divide the window into pads. */
	TH2S**                                     coverageProjectionDisplays;					/**< Variable to hold the object for each display of the covered and projected source data. */
	unsigned short                             numberOfCoverageProjectionDisplays;			/**< Variable to store the number of evaluated displays.*/

/**
 * creates a string representation for the object
 */

	std::string createString(bool projection = false, bool coverage = false);

/**
 * creates all displays for a normal drawing
 */

	void createDisplay(TH2S*** indicators, unsigned short* numberOfIndicators, std::string name, std::string title, bool projection = false, bool coverage = false);

/**
 * deletes allocated memory
 */

	void deleteDisplayMemory(TH2S*** memory, unsigned short numberOfMemoryElements);

/**
 * This method initializes the global style for each display.
 */

	void initDisplayStyle();

/**
 * This method shows the created displays.
 */

	void showDisplay(TCanvas* window, unsigned short* numberOfWindowPadColumns, unsigned short* numberOfWindowPadRows, TH2S** indicators, unsigned short numberOfIndicators, std::string name, std::string title);

/**
 * This method evaluate the window division factors.
 */

	void evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, unsigned short numberOfIndicators);

/**
 * build a list containing special peakfinding geometry elements
 */

	specialListMem<peakfindingGeometryElement> buildProjectedGeometry(specialListMem<peakfindingGeometryElement>& actualGeometry);
	specialListMem<peakfindingGeometryElement> buildCoveredGeometry(specialListMem<peakfindingGeometryElement>& actualGeometry);

public:

/**
 * Default constructor
 */

	peakfindingGeometry();

/**
 * Constructor
 */

	peakfindingGeometry(const peakfindingGeometry& value);

/**
 * Destructor
 */

	virtual ~peakfindingGeometry();

/**
 * operator = ()
 */

	const peakfindingGeometry& operator = (const peakfindingGeometry& value);

/**
 * get percentage of coverage
 */

	unsigned short getCoverage();

/**
 * set percentage of coverage
 */

	void setCoverage(unsigned short percentageOfCoverage);

/**
 * resets the object
 */

	void reset();

/**
 * removes all elements
 */

	void resetElements();

/**
 * Returns the element in the geometry which is farest away
 * from the origin element and smaller than this one.
 */

	peakfindingGeometryElement getCoveredProjectedMinElement();

/**
 * Returns the element in the geometry which is farest away
 * from the origin element and bigger than this one.
 */

	peakfindingGeometryElement getCoveredProjectedMaxElement();

/**
 * Returns the min/max in the geometry which is farest away
 * from the origin element.
 */

	void getMinMaxDimensions(short* dim1Min, short* dim1Max, short* dim2Min, short* dim2Max, short* dim3Min, short* dim3Max);

/**
 * get a list containing the peakfinding geometry elements
 */

	specialListMem<peakfindingGeometryElement> getGeometryElements();
	specialListMem<peakfindingGeometryElement> getProjectedGeometryElements();
	specialListMem<peakfindingGeometryElement> getCoveredGeometryElements();
	specialListMem<peakfindingGeometryElement> getCoveredProjectedGeometryElements();

/**
 * add element
 */

	void addGeometryElement(peakfindingGeometryElement& element);
	void addGeometryElement(short position1, short position2, short position3, unsigned short value);

/**
 * Method adds whole other peakfinding geometry elements, but will not modify the coverage
 * @param actualpeakfindingGeometry is the geometry which should be added
 * @param center defines the center of the filter geometry to add it relatively with a centered position
 */

	void addGeometryElements(specialListMem<peakfindingGeometryElement>& elements);
	void addGeometryElements(specialListMem<peakfindingGeometryElement>& elements, trackCoordinates& center);

/**
 * set whole other peakfinding geometry
 * @param actualPeakfindingGeometry is the geometry which should be added
 * @param center defines the center of the filter geometry to add it relatively with a centered position
 */

	void setGeometry(peakfindingGeometry& actualPeakfindingGeometry);
	void setGeometry(peakfindingGeometry& actualPeakfindingGeometry, trackCoordinates& center);

/**
 * method returns a string representation of the peakfinding geometry
 */
	std::string toString();
	std::string toProjectedString();
	std::string toCoveredString();
	std::string toCoveredProjectedString();

/**
 * method draws the peakfinding geometry
 */
	void draw();
	void drawProjection();
	void drawCoverage();
	void drawCoverageProjection();

/**
 * method reads a file to get the peakfinding geometry
 * @param fileName is the name of the file to read the data
 * @param terminal is a buffer to place the process information
 */

	void read(std::string fileName, std::streambuf* terminal = NULL);

/**
 * method writes a file representing the peakfinding geometry
 * @param fileName is the name of the file to write the data
 * @param name is the name of the filter geometry which should be written to file
 * @param terminal is a buffer to place the process information
 */

	void write(std::string fileName, std::string name, std::streambuf* terminal = NULL);

};

#endif
