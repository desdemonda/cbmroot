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
//     - base class for holding the definition of the prelut
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:58:52 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/prelutDefinition.h"


/****************************************************************
 * CLASS prelutDefinition						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutDefinition::prelutDefinition() 
  : dim3Min(0),
    dim3Max(0),
    dim3Step(0),
    dim3StartEntry(0),
    dim3StopEntry(0)
{
  /*
	dim3Min        = 0;
	dim3Max        = 0;
	dim3Step       = 0;
	dim3StartEntry = 0;
	dim3StopEntry  = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutDefinition::prelutDefinition(const prelutDefinition& value) 
  : dim3Min(value.dim3Min),
    dim3Max(value.dim3Max),
    dim3Step(value.dim3Step),
    dim3StartEntry(value.dim3StartEntry),
    dim3StopEntry(value.dim3StopEntry)
{
  /*
	this->dim3Min        = value.dim3Min;
	this->dim3Max        = value.dim3Max;
	this->dim3Step       = value.dim3Step;
	this->dim3StartEntry = value.dim3StartEntry;
	this->dim3StopEntry  = value.dim3StopEntry;
  */
}
prelutDefinition::prelutDefinition(double _dim3Min, double _dim3Max, int _dim3Step, double _dim3StartEntry, double _dim3StopEntry) 
  : dim3Min(_dim3Min),
    dim3Max(_dim3Max),
    dim3Step(_dim3Step),
    dim3StartEntry(_dim3StartEntry),
    dim3StopEntry(_dim3StopEntry)
{
  /*
	this->dim3Min        = _dim3Min;
	this->dim3Max        = _dim3Max;
	this->dim3Step       = _dim3Step;
	this->dim3StartEntry = _dim3StartEntry;
	this->dim3StopEntry  = _dim3StopEntry;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutDefinition::~prelutDefinition() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const prelutDefinition& prelutDefinition::operator = (const prelutDefinition& value) {

	this->dim3Min        = value.dim3Min;
	this->dim3Max        = value.dim3Max;
	this->dim3Step       = value.dim3Step;
	this->dim3StartEntry = value.dim3StartEntry;
	this->dim3StopEntry  = value.dim3StopEntry;

	return *this;

}

/****************************************************************
 * operator << (stream)											*
 ****************************************************************/

std::ostream& operator << (std::ostream& os, prelutDefinition& anyObject) {

	os << anyObject.toString();

	return os;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/
	
std::string prelutDefinition::toString() {

	std::string returnValue;
	char        intBuffer[intConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];

	returnValue  = " Dim3Min: ";
	dtos(dim3Min, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim3Max: ";
	dtos(dim3Max, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim3Step: ";
	itos(dim3Step, intBuffer, 10, shortConversionDigits);
	returnValue += intBuffer;
	returnValue += "\n Dim3StartEntry: ";
	dtos(dim3StartEntry, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;
	returnValue += "\n Dim3StopEntry: ";
	dtos(dim3StopEntry, doubleBuffer, doubleConversionDigits);
	returnValue += doubleBuffer;

	return returnValue;

}
