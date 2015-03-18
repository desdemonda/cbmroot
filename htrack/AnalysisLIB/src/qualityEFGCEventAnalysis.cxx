/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - implements methods to analyze the trackfinding algorithm's quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:27 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/qualityEFGCEventAnalysis.h"


/****************************************************************
 * resets the info structs for event and nothing for total		*
 ****************************************************************/

void qualityEFGCEventAnalysis::reset() {

	resetPeakAndTrackInfo();

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

qualityEFGCEventAnalysis::qualityEFGCEventAnalysis() : qualityEFGCAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

qualityEFGCEventAnalysis::~qualityEFGCEventAnalysis() {

}