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
///     - abstract class for all filter methods for
///		 maxMorphSearch
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIMZDIMZ_H
#define _FILTERDIMZDIMZ_H


#include "filterDimZ.h"


/* **************************************************************
 * CLASS filterDimZDimZ		 									*
 * **************************************************************/

class filterDimZDimZ : public filterDimZ {

protected:

	unsigned short filterSize1;		/**< Variable to store the size of the filter in the first dimension. */
	unsigned short filterSize2;		/**< Variable to store the size of the filter in the second dimension. */

public:

/**
 * Default constructor
 */

	filterDimZDimZ();

/**
 * Constructor
 * @param tracks is an object for accessing the tracks
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimZDimZ( trackData**    tracks,
					unsigned short size1,
					unsigned short size2,
					unsigned short size,
					unsigned short localSize);

/**
 * Default destructor
 */

	virtual ~filterDimZDimZ();

/**
 * This method initializes the object.
 * @param tracks is an object for accessing the tracks
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( trackData**    tracks,
			   unsigned short size1,
			   unsigned short size2,
			   unsigned short size,
			   unsigned short localSize);

};

#endif
