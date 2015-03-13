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
///     - class for evaluating the basic filter method for
///		 maxMorphSearch
///
/// |      FILTEROBJECT      | FILTERSTRATEGY   
/// ------------------------------------------------------------------------------------------------------------------
/// | filterBasicSpecial     | if array[element] >= maximumClass use filterBasicSimpleMod, else use filterBasicSimple
/// ------------------------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:33 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERBASICSPECIAL_H
#define _FILTERBASICSPECIAL_H


#include "filterBasicStyle.h"


/* **************************************************************
 * CLASS filterBasicSpecial	 									*
 * **************************************************************/

class filterBasicSpecial : public filterBasicStyle {

private:

  filterBasicSpecial(const filterBasicSpecial&);
  filterBasicSpecial& operator=(const filterBasicSpecial&);

	bitArray          maxClass;			/**> Object to store the maximum classification which can occur. */
	filterBasicStyle* filterMax;		/**< Object to use a filter for a maximum value. */
	filterBasicStyle* filterElse;		/**< Object to use a filter for all other values. */

public:

/**
 * Default constructor
 */

	filterBasicSpecial(bitArray maximumClass);

/**
 * Destructor
 */

	virtual ~filterBasicSpecial();

/**
 * This method implements the filter without respect to the
 * axis.
 * @param array consists of the values to filter
 * @param length is the length of the array and the length of the global filter
 * @param small is the length of the local filter
 * @param element is the index of the element to filter for
 */

	bitArray filter( bitArray* array, unsigned int length,
					 unsigned int small, unsigned int element);

};

#endif
