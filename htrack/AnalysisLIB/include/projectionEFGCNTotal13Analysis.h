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
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PROJECTIONEFGCNTOTAL13ANALYSIS_H
#define _PROJECTIONEFGCNTOTAL13ANALYSIS_H


#include "projectionEFGCNAnalysis.h"


/**
 * CLASS projectionEFGCNTotal13Analysis
 */

class projectionEFGCNTotal13Analysis : public projectionEFGCNAnalysis {

protected:

/**
 * resets the profiles for event and nothing for total
 */

	void reset();

/**
 * returns the corresponding entry for the x-axi.
 */

	int getEntryOfXAxi(trackCoordinates& coordinates);

/**
 * returns the corresponding entry for the y-axi.
 */

	int getEntryOfYAxi(trackCoordinates& coordinates);

/**
 * returns the corresponding string, which is drawn on the x-axi.
 */

	const char* getTitleOfXAxi();

/**
 * returns the corresponding string, which is drawn on the y-axi.
 */

	const char* getTitleOfYAxi();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfEDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfEDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfFDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfFDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfGDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfGDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfCDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfCDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfNDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfNDisplay();

/**
 * returns the corresponding string for the saving directory.
 */

	const char* getNameOfSavingDirectory();

/**
 * returns true, if the saving should be in subdirectories.
 */

	bool getSavingInSubDirectory();

public:

/**
 * Default constructor
 */

	projectionEFGCNTotal13Analysis();

/**
 * Destructor
 */

	virtual ~projectionEFGCNTotal13Analysis();

};

#endif
