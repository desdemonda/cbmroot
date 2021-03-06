//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
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
/// | FILTERSTRUCTURE  | FILTERSIZE                                                                                                | FILTERSTRUCTURE (rotate dim3 90�)
/// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// |                  |                                                                                                           |
/// |                  |                                                                                                           |
/// |        X inplace | f3                                                                                                        |  X X X X X
/// |                  |                                                                                                           |      inplace
/// |                  |                                                                                                           |
/// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIM3_H
#define _FILTERDIM3_H


#include "filterDimZ.h"


/* **************************************************************
 * CLASS filterDim3			 									*
 * **************************************************************/

class filterDim3 : public filterDimZ {

public:

/**
 * Default constructor
 */

	filterDim3();

/**
 * Constructor
 * @param tracks is an object for accessing the tracks
 * @param filterArithmetic defines the arithmetic which is used in the applied filter
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 * @param maximumClass is the maximal class which can occur while filtering
 */

	filterDim3( trackData**    tracks,
				unsigned short filterArithmetic,
				unsigned short size,
				unsigned short localSize,
				bitArray       maximumClass);

/**
 * Destructor
 */

	virtual ~filterDim3();

/**
 * This method initializes the object.
 * @param tracks is an object for accessing the tracks
 * @param filterArithmetic defines the arithmetic which is used in the applied filter
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 * @param maximumClass is the maximal class which can occur while filtering
 */

	void init( trackData**    tracks,
			   unsigned short filterArithmetic,
			   unsigned short size,
			   unsigned short localSize,
			   bitArray       maximumClass);

/**
 * This method implements the filter.
 * @param terminal is a buffer to place the process information
 */

	void filter(std::streambuf* terminal = NULL);

};

#endif
