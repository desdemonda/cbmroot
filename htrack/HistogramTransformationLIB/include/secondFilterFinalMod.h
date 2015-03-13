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
///     - class for evaluating the basic filter method for
///		 maxMorphSearch
///
/// | FILTERSTRUCTURE  | FILTERSIZE                                                                                                | FILTERSTRUCTURE (rotate dim3 90°)
/// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// |        XXXX      |                                                                                                           |     XXX
/// |        XXXX      |                                                                                                           |     XXX
/// |        X copy    | (f3-1) * ( 2 * { S[i=0;f1/2-1] (f2+1)/2 } + 1) + 1 = (f3-1)*((f1/2)*(f2+1)+1)+1 different localFiltersize |     XXX
/// |     XXXX         |                                                                                                           |     XXX copy
/// |     XXXX         |                                                                                                           |     XXX
/// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _SECONDFILTERFINALMOD_H
#define _SECONDFILTERFINALMOD_H


#include "filterDimZDimZDimZ.h"


/* **************************************************************
 * CLASS secondFilterFinalMod	 								*
 * **************************************************************/

class secondFilterFinalMod : public filterDimZDimZDimZ {

public:

/**
 * Default constructor
 */

	secondFilterFinalMod();

/**
 * Constructor
 * @param tracks is an object for accessing the tracks
 * @param filterArithmetic defines the arithmetic which is used in the applied filter
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size3 is the filtersize in the third dimension
 * @param localSize1 is the local filtersize in the first dimension
 * @param localSize2 is the local filtersize in the second dimension
 * @param localSize3 is the local filtersize in the third dimension
 * @param maximumClass is the maximal class which can occur while filtering
 */

	secondFilterFinalMod( trackData**    tracks,
					      unsigned short filterArithmetic,
					      unsigned short size1,
					      unsigned short size2,
					      unsigned short size3,
					      unsigned short localSize1,
					      unsigned short localSize2,
					      unsigned short localSize3,
					      bitArray       maximumClass);

/**
 * Destructor
 */

	virtual ~secondFilterFinalMod();

/**
 * This method initializes the object.
 * @param tracks is an object for accessing the tracks
 * @param filterArithmetic defines the arithmetic which is used in the applied filter
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param size3 is the filtersize in the third dimension
 * @param localSize1 is the local filtersize in the first dimension
 * @param localSize2 is the local filtersize in the second dimension
 * @param localSize3 is the local filtersize in the third dimension
 * @param maximumClass is the maximal class which can occur while filtering
 */

	void init( trackData**    tracks,
			   unsigned short filterArithmetic,
			   unsigned short size1,
			   unsigned short size2,
			   unsigned short size3,
			   unsigned short localSize1,
			   unsigned short localSize2,
			   unsigned short localSize3,
			   bitArray       maximumClass);

/**
 * This method implements the filter.
 * @param terminal is a buffer to place the process information
 */

	void filter(std::streambuf* terminal = NULL);

};

#endif
