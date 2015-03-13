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
/// | filterBasicComplexMod  | delete array[element] if not global maximum or is not the absolute local maximum
/// ------------------------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:33 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERBASICCOMPLEXMOD_H
#define _FILTERBASICCOMPLEXMOD_H


#include "filterBasicStyle.h"


/* **************************************************************
 * CLASS filterBasicComplexMod 									*
 * **************************************************************/

class filterBasicComplexMod : public filterBasicStyle {

public:

/**
 * Default constructor
 */

	filterBasicComplexMod();

/**
 * Destructor
 */

	virtual ~filterBasicComplexMod();

/**
 * This method implements the filter without respect to the
 * axis.
 * @param array consists of the values to filter
 * @param length is the length of the array and the length of the global filter
 * @param small is the length of the local filter
 * @param element is the index of the element to filter for
 */

	bitArray filter(bitArray* array, unsigned int length,
						 unsigned int small, unsigned int element);

};

#endif
