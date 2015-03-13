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
///     - class for holding the information of an element in the
///       automatic generated filter geometry
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:58:52 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectError.h"
#include "../include/peakfindingGeometryElement.h"


#define sqr(a)  ((a) * (a)) 
#define RADIX   10


/****************************************************************
 * Default constructor											*
 ****************************************************************/

peakfindingGeometryElement::peakfindingGeometryElement() : dim(), value(0) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		dim[i] = 0;
	//	value      = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

peakfindingGeometryElement::peakfindingGeometryElement(const peakfindingGeometryElement& _value) : dim(), value(_value.value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = _value.dim[i];
	//	this->value      = _value.value;

}
peakfindingGeometryElement::peakfindingGeometryElement(short dim1, short dim2, short dim3, unsigned short _value) : dim(), value(_value) {

	set(dim1, DIM1);
	set(dim2, DIM2);
	set(dim3, DIM3);
	//	setValue(_value);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

peakfindingGeometryElement::~peakfindingGeometryElement() {
}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const peakfindingGeometryElement& peakfindingGeometryElement::operator = (const peakfindingGeometryElement& _value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = _value.dim[i];
	this->value      = _value.value;

	return *this;

}

/****************************************************************
 * returns the value in the dimension							*
 ****************************************************************/

short peakfindingGeometryElement::get(unsigned short dimension) {

	short returnValue = 0;

	if (dimension < DIMENSIONS)
		returnValue = dim[dimension];
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}
std::string peakfindingGeometryElement::getString(unsigned short dimension) {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[shortConversionDigits+1];

	if (dimension < DIMENSIONS) {
		returnValue  = "000";
		counts       = addRadix(RADIX, returnValue);
		maxCounts    = (int)returnValue.length();
		if (counts < maxCounts)
			returnValue.erase(counts, maxCounts - counts);
		stos(dim[dimension], buffer, RADIX, shortConversionDigits);
		returnValue += buffer;
	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * get value													*
 ****************************************************************/

unsigned short peakfindingGeometryElement::getValue() {

	return value;

}
std::string peakfindingGeometryElement::getValueString() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[shortConversionDigits+1];

	returnValue  = "000";
	counts       = addRadix(RADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	ustos(value, buffer, RADIX, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * sets the value in the dimension								*
 ****************************************************************/

void peakfindingGeometryElement::set(short _set, unsigned short dimension) {

	if (dimension < DIMENSIONS)
		dim[dimension] = _set;
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}
void peakfindingGeometryElement::setString(std::string& _set, unsigned short dimension) {

	std::string temp;
	int         radix;

	temp           = _set;
	extractRadix(&radix, &temp);
	dim[dimension] = stos(temp, radix);

}

/****************************************************************
 * set value													*
 ****************************************************************/

void peakfindingGeometryElement::setValue(unsigned short _value) {

	this->value = _value;

}
void peakfindingGeometryElement::setValueString(std::string& _value) {

	std::string temp;
	int         radix;

	temp         = _value;
	extractRadix(&radix, &temp);
	this->value  = stous(temp, radix);

}

/****************************************************************
 * increment value												*
 ****************************************************************/

void peakfindingGeometryElement::incrementValue(unsigned short increment) {

	this->value += increment;

}

/****************************************************************
 * This method compares to objects of this class. The result is	*
 * - zero, if the object is equal to the element				*
 * - else, if the object is not equal to the element			*
 ****************************************************************/

int peakfindingGeometryElement::compare(peakfindingGeometryElement& element) {

	int returnValue;

	returnValue = sqr(this->get(DIM3) - element.get(DIM3)) + sqr(this->get(DIM2) - element.get(DIM2)) + sqr(this->get(DIM1) - element.get(DIM1));

	if ((this->get(DIM3) < element.get(DIM3)) || ((this->get(DIM3) == element.get(DIM3)) && (this->get(DIM2) < element.get(DIM2))) || ((this->get(DIM3) == element.get(DIM3)) && (this->get(DIM2) == element.get(DIM2)) && (this->get(DIM1) < element.get(DIM1))))
		returnValue = - returnValue;

	return returnValue;

}

/****************************************************************
 * This method compares to objects of this class. The result is	*
 * - true, if the object is bigger than the element				*
 * - false, if the object is not bigger than the element		*
 ****************************************************************/

bool peakfindingGeometryElement::isBigger(peakfindingGeometryElement& element) {

	bool returnValue;

	if (compare(element) < 0)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * returns a pointer to this object								*
 ****************************************************************/

peakfindingGeometryElement* peakfindingGeometryElement::getPointer() {

	return this;

}

/****************************************************************
 * returns a string representation for this object				*
 ****************************************************************/

std::string peakfindingGeometryElement::toString() {

	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	returnValue  = "dim1 = ";
	stos(get(DIM1), buffer, 10, shortConversionDigits);
	returnValue += buffer;
	returnValue += ", dim2 = ";
	stos(get(DIM2), buffer, 10, shortConversionDigits);
	returnValue += buffer;
	returnValue += ", dim3 = ";
	stos(get(DIM3), buffer, 10, shortConversionDigits);
	returnValue += buffer;
	returnValue += " : value = ";
	ustos(getValue(), buffer, 10, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double peakfindingGeometryElement::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(dim);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double peakfindingGeometryElement::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double peakfindingGeometryElement::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(dim);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
