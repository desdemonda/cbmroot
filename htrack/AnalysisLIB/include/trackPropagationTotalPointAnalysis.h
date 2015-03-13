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
///     - implements methods to analyze the track propagation's quality
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2009-01-13 15:50:11 $
/// $Revision: 1.0 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKPROPAGATIONTOTALPOINTANALYSIS_H
#define _TRACKPROPAGATIONTOTALPOINTANALYSIS_H


#include "trackPropagationDAnalysis.h"


/* **************************************************************
 * CLASS trackPropagationTotalPointAnalysis			 			*
 * **************************************************************/

class trackPropagationTotalPointAnalysis : public trackPropagationDAnalysis {

protected:

/**
 * resets the profiles for event and nothing for total
 */

	void reset();

/**
 * returns the corresponding string for the name identification.
 * @param index represents the order index of the display which should be identified
 */

	std::string getNameOfDistanceDisplay(unsigned short index);

/**
 * returns the corresponding string for the title.
 * @param index represents the order index of the display which should be identified
 */

	std::string getTitleOfDistanceDisplay(unsigned short index);

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

	trackPropagationTotalPointAnalysis();

/**
 * Destructor
 */

	virtual ~trackPropagationTotalPointAnalysis();

};

#endif
