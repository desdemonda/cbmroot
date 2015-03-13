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
///     - implements methods to analyze the trackfinding algorithm graphically
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MOMENTUMEFGCTOTALPZANALYSIS_H
#define _MOMENTUMEFGCTOTALPZANALYSIS_H


#include "momentumEFGCAnalysis.h"


/* **************************************************************
 * CLASS momentumEFGCTotalPzAnalysis				 			*
 * **************************************************************/

class momentumEFGCTotalPzAnalysis : public momentumEFGCAnalysis {

protected:

/**
 * resets the profiles for event and nothing for total
 */

	void reset();

/**
 * returns the corresponding impulse for each display entry.
 */

	double getEntryOfXAxi(trackMomentum& momentum);

/**
 * returns the corresponding string, which is drawn on that axi,
 * where the impulse is shown.
 */

	const char* getTitleOfXAxi();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfEMDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfEMDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfFMDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfFMDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfGMDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfGMDisplay();

/**
 * returns the corresponding string for the name identification.
 */

	const char* getNameOfCMDisplay();

/**
 * returns the corresponding string for the title.
 */

	const char* getTitleOfCMDisplay();

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

	momentumEFGCTotalPzAnalysis();

/**
 * Destructor
 */

	virtual ~momentumEFGCTotalPzAnalysis();

};

#endif
