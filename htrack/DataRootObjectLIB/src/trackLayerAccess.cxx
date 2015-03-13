/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
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
//     - base class for easy access the iterators of one track layer
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/trackLayerAccess.h"


/****************************************************************
 * CLASS trackLayerAccess						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackLayerAccess::trackLayerAccess() 
  : begin(),
    end(),
    isValid(false)
{

  //	isValid  = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackLayerAccess::trackLayerAccess(const trackLayerAccess& value) 
  : begin(value.begin),
    end(value.end),
    isValid(value.isValid)
{
  /*
  	this->begin    = value.begin;
	this->end      = value.end;
	this->isValid  = value.isValid;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackLayerAccess::~trackLayerAccess() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackLayerAccess& trackLayerAccess::operator = (const trackLayerAccess& value) {

	this->begin    = value.begin;
	this->end      = value.end;
	this->isValid  = value.isValid;

	return *this;

}
