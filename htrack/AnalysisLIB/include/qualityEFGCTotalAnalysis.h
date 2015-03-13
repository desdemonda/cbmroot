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
///     - implements methods to analyze the trackfinding algorithm's quality
///		 for each event
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _QUALITYEFGCTOTALANALYSIS_H
#define _QUALITYEFGCTOTALANALYSIS_H


#include "qualityEFGCAnalysis.h"


/* **************************************************************
 * CLASS qualityEFGCTotalAnalysis				 				*
 * **************************************************************/

class qualityEFGCTotalAnalysis : public qualityEFGCAnalysis {

protected:

/**
 * resets the info classes for the peaks and the tracks
 */

	void reset();

public:

/**
 * Default constructor
 */

	qualityEFGCTotalAnalysis();

/**
 * Destructor
 */

	virtual ~qualityEFGCTotalAnalysis();

};

#endif

