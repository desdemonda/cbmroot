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
/// $Date: 2008-09-11 14:07:01 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PEAKFINDINGGEOMETRYANALYSIS_H
#define _PEAKFINDINGGEOMETRYANALYSIS_H


#include "../../DataRootObjectLIB/include/peakfindingGeometry.h"
#include "../../DataRootObjectLIB/include/histogramData.h"


/* **************************************************************
 * CLASS peakfindingGeometryAnalysis			 				*
 * **************************************************************/

class peakfindingGeometryAnalysis {

 private:
  peakfindingGeometryAnalysis(const peakfindingGeometryAnalysis&);
  peakfindingGeometryAnalysis& operator=(const peakfindingGeometryAnalysis&);

protected:

	histogramData**      histogram;
	peakfindingGeometry  actualPeakfindingLayerGeometry;
	peakfindingGeometry  actualPeakfindingGeometry;

public:

/**
 * Default constructor
 */

	peakfindingGeometryAnalysis();

/**
 * Constructor
 */

	peakfindingGeometryAnalysis(histogramData** histogram);

/**
 * Destructor
 */

	virtual ~peakfindingGeometryAnalysis();

/**
 * This method initializes the object.
 */

	void init(histogramData** histogram);

/**
 * method resets the peak finding geometry for the actual layers
 */

	void resetPeakfindingLayerGeometryElements();

/**
 * method resets the actual used filter geometry
 */

	void resetPeakfindingGeometryElements();

/**
 * method adds the geometry of the actual layer to the other layers
 */

	void addPeakfindingLayerGeometry(unsigned short layer);

/**
 * method updates the used peak finding geometry by adding the geometry of the actual layers
 */

	void updatePeakfindingGeometry(trackCoordinates& center);

/**
 * method returns the filter geometry
 */

	peakfindingGeometry getPeakfindingGeometry();

};

#endif
