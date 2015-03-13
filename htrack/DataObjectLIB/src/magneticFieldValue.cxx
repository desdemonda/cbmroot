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
///     - consists of the information for one point in the fieldmap
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:43 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/magneticFieldValue.h"


/**
 * Default constructor
 */

magneticFieldValue::magneticFieldValue() {

	value[0] = 0;
	value[1] = 0;
	value[2] = 0;

}

/**
 * Constructor
 */

magneticFieldValue::magneticFieldValue(const magneticFieldValue& _value) {

	this->value[0] = _value.value[0];
	this->value[1] = _value.value[1];
	this->value[2] = _value.value[2];

}

/**
 * Destructor
 */

magneticFieldValue::~magneticFieldValue() {

}

/**
 * operator = ()
 */

const magneticFieldValue& magneticFieldValue::operator = (const magneticFieldValue& _value) {

	this->value[0] = _value.value[0];
	this->value[1] = _value.value[1];
	this->value[2] = _value.value[2];

	return *this;

}

/**
 * This method returns the magnetic field value of the x-dimension
 */

double magneticFieldValue::getFieldX() {

	return value[0];

}

/**
 * This method returns the magnetic field value of the y-dimension
 */

double magneticFieldValue::getFieldY() {

	return value[1];

}

/**
 * This method returns the magnetic field value of the z-dimension
 */

double magneticFieldValue::getFieldZ() {

	return value[2];

}

/**
 * This method sets the magnetic field value of the x-dimension
 */

void magneticFieldValue::setFieldX(double _value) {

	this->value[0] = _value;

}

/**
 * This method sets the magnetic field value of the y-dimension
 */

void magneticFieldValue::setFieldY(double _value) {

	this->value[1] = _value;

}

/**
 * This method sets the magnetic field value of the z-dimension
 */

void magneticFieldValue::setFieldZ(double _value) {

	this->value[2] = _value;

}
