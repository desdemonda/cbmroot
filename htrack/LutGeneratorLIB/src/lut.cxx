/* *******************************************************************
// (C)opyright 2004
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
//     - base class for the second look-up-tables
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:37 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorError.h"
#include "../include/lut.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

lut::lut() 
  : def(),
    border()
{

	def.dim1Min              = 0;
	def.dim1Max              = 0;
	def.dim1Step             = 0;
	def.dim2Min              = 0;
	def.dim2Max              = 0;
	def.dim2Step             = 0;
	border.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

lut::lut(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) 
  : def(),
    border()
{

	init(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step);
	border.clear();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lut::~lut() {

}

/****************************************************************
 * Method initializes the object.								*
 ****************************************************************/

void lut::init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step) {

	def.dim1Min  = dim1Min;
	def.dim1Max  = dim1Max;
	def.dim1Step = dim1Step;
	def.dim2Min  = dim2Min;
	def.dim2Max  = dim2Max;
	def.dim2Step = dim2Step;

}

/****************************************************************
 * This method returns the Definition of the lut object.		*
 ****************************************************************/

lutDefinition lut::getLutDefinition() {

	return def;

}

/****************************************************************
 * This method returns the border.								*
 ****************************************************************/

lutHoughBorder lut::getBorder() {

	return border;

}

/****************************************************************
 * This method clears the border.								*
 ****************************************************************/

void lut::clear(lutHoughBorder* borderPointer) {

	lutHoughBorder* actualBorderPointer;

	if (borderPointer == NULL)
		actualBorderPointer = &border;
	else
		actualBorderPointer = borderPointer;

	actualBorderPointer->clear();

}
