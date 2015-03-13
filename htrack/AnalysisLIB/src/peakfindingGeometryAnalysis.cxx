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
///     - implements methods to analyze the peak finding geometry
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-03 14:32:53 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/errorHandling.h"
#include "../include/peakfindingGeometryAnalysis.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

peakfindingGeometryAnalysis::peakfindingGeometryAnalysis() 
  : histogram(NULL),
    actualPeakfindingLayerGeometry(),
    actualPeakfindingGeometry()
{
  /*
	histogram = NULL;
  */
	actualPeakfindingLayerGeometry.reset();
	actualPeakfindingGeometry.reset();
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

peakfindingGeometryAnalysis::peakfindingGeometryAnalysis(histogramData** _histogram) 
  : histogram(_histogram),
    actualPeakfindingLayerGeometry(),
    actualPeakfindingGeometry()
{
  /*
	this->histogram = _histogram;
  */
	actualPeakfindingLayerGeometry.reset();
	actualPeakfindingGeometry.reset();
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

peakfindingGeometryAnalysis::~peakfindingGeometryAnalysis() {

	actualPeakfindingLayerGeometry.reset();
	actualPeakfindingGeometry.reset();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void peakfindingGeometryAnalysis::init(histogramData** _histogram) {

	this->histogram = _histogram;
	actualPeakfindingLayerGeometry.reset();
	actualPeakfindingGeometry.reset();

}

/****************************************************************
 * method resets the peak finding geometry for the actual layers*
 ****************************************************************/

void peakfindingGeometryAnalysis::resetPeakfindingLayerGeometryElements() {

	actualPeakfindingLayerGeometry.resetElements();

}

/****************************************************************
 * method resets the actual used peak finding geometry			*
 ****************************************************************/

void peakfindingGeometryAnalysis::resetPeakfindingGeometryElements() {

	actualPeakfindingGeometry.resetElements();

}

/****************************************************************
 * method adds the geometry of the actual layer to the other	*
 * layers														*
 ****************************************************************/

void peakfindingGeometryAnalysis::addPeakfindingLayerGeometry(unsigned short layer) {

	bitArray comparator;

	if (histogram == NULL)
		throw cannotAccessHistogramError(ANALYSISLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(ANALYSISLIB);

	comparator = bitArray(0);
	for (unsigned short i = 0; i < (*histogram)->getValueDim1(); i++) {

		for (unsigned short j = 0; j < (*histogram)->getValueDim2(); j++) {

			if ((*histogram)->getCell(i, j)->value > comparator)
				actualPeakfindingLayerGeometry.addGeometryElement(i, j, layer, 1);

		}

	}

}

/****************************************************************
 * method updates the used filter geometry by adding the		*
 * geometry of the actual layers								*
 ****************************************************************/

void peakfindingGeometryAnalysis::updatePeakfindingGeometry(trackCoordinates& center) {

	specialListMem<peakfindingGeometryElement> temp;

	temp = actualPeakfindingLayerGeometry.getGeometryElements();

	actualPeakfindingGeometry.addGeometryElements(temp, center);

}

/****************************************************************
 * method returns the filter geometry							*
 ****************************************************************/

peakfindingGeometry peakfindingGeometryAnalysis::getPeakfindingGeometry() {

	return actualPeakfindingGeometry;

}
