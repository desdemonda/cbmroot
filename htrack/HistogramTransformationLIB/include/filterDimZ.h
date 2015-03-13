//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
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
/// $Date: 2008-10-07 10:37:45 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIMZ_H
#define _FILTERDIMZ_H


#include "../../DataRootObjectLIB/include/trackData.h"
#include "../../MiscLIB/include/bitArray.h"
#include "filterBasicStyle.h"


/* **************************************************************
 * CLASS filterDimZ			 									*
 * **************************************************************/

class filterDimZ {

 private:

  filterDimZ(const filterDimZ&);
  filterDimZ& operator=(const filterDimZ&);

protected:

	trackData**       tracks;				/**< Object for accessing the tracks. */
	bitArray*         filterMem;			/**< Array to store the input values for filtering. */
	filterBasicStyle* baseFilter;			/**< Object to build the filtering strategy. */
	unsigned short    filterSize;			/**< Variable to store the complete size of the area of the filter. */
	unsigned short    filterLocalSize;		/**< Variable to store the complete size of the area of the local filter. */

public:

/**
 * Default constructor
 */

	filterDimZ();

/**
 * Constructor
 * @param tracks is an object for accessing the tracks
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimZ( trackData**    tracks,
				unsigned short size,
				unsigned short localSize);

/**
 * Default destructor
 */

	virtual ~filterDimZ();

/**
 * This method initializes the object.
 * @param tracks is an object for accessing the tracks
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( trackData**    tracks,
			   unsigned short size,
			   unsigned short localSize);

/**
 * This method implements the filter.
 */

	virtual void filter(std::streambuf* terminal = NULL) = 0;

};

#endif
