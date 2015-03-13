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
//     - The class optimizes the storage needed by array of bools
//     - The restriction is that there must be 8 bools or less
//
//     - Some basic functions are:
//     - length()	// returns the number of elements
//     - size()		// returns the storage amount in bytes
//     - count()	// returns the number of elements which have the value true
//     - any()		// returns true if at least one bit is true else false
//     - none()		// returns true if no bit is true else false
//     - flip()		// flips every bit
//     - set()		// sets the value of each element to true
//     - reset()	// sets the value of each element to false
//     - flip(n)	// flips bit n
//     - set(n)		// sets the value of element n to true
//     - reset(n)	// sets the value of element n to false
//     - test(n)	// returns true if the value of element n is true else false
//
//     - The class defines some basic operators like:
//       - []
//       - =, ==, !=
//       - ~, !, ^, &, &&, |, ||, ^=, &=, |=
//       - <<, <<=, >>, >>=
//       - <, <=, >, >=
//       - +, -, *, /, %, +=, -=, *=, /=, %=   // The arithmetic operands are always used without []
//       - ++(), ()++, --(), ()--
//       - <<, >>
//
//     CAUTION:
//     - The operator & which returns the address is not supported
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:47:05 $
// $Revision: 1.7 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../include/bytefield.h"
#include <string>


/****************************************************************
 * CLASS bytefield								 				*
 ****************************************************************/

/****************************************************************
 * Method computes the value based on a string					*
 ****************************************************************/

void bytefield::valueFromString(std::string value) {
	
	int radix;

	extractRadix(&radix, &value);

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
	byte   = stoul(value, radix);	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif
	byte <<= cutBits;
	byte >>= cutBits;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

bytefield::bytefield() : byte(0), position(0) {

	reset();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

bytefield::bytefield(const bytefield& value) : byte(value.byte), position(value.position) {
	
  //	this->byte     = value.byte;
  //	this->position = value.position;

}
bytefield::bytefield(unsigned long value) : byte((byteSourceType)value), position(0) {

  //	byte     = (byteSourceType)value;	/* reduction to the used number of bits */

	byte   <<= cutBits;
	byte   >>= cutBits;
	//	position = 0;

}
bytefield::bytefield(std::string value) : byte(), position(0) {

	valueFromString(value);	/* reduction to the used number of bits */
	//	position = 0;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

bytefield::~bytefield() {

}

/****************************************************************
 * Method returns the length of the array.						*
 ****************************************************************/

size_t bytefield::length() {

	return maxDimSH;

}

/****************************************************************
 * Method returns the size of the array in bytes.				*
 ****************************************************************/

size_t bytefield::size() {

	return sizeof(bytefield);

}

/****************************************************************
 * Method counts the ones in the array and returns the number.	*
 ****************************************************************/

size_t bytefield::count() {

	unsigned char index       = this->position;
	size_t        returnValue = 0;

	for (unsigned char i = 0; i < length(); i++)
		if ((*this)[i])
			returnValue++;

	this->position = index;

	return returnValue;

}

/****************************************************************
 * Method counts the maximal consecutive ones in the array and	*
 * returns the number.											*
 ****************************************************************/

size_t bytefield::count_consecutive() {

	size_t actualConsecutiveOnes  = 0;
	size_t maximalConsecutiveOnes = 0;

	for (size_t i = 0; i < length(); i++) {

		if (test(i))
			actualConsecutiveOnes++;
		else {

			if (actualConsecutiveOnes > maximalConsecutiveOnes)
				maximalConsecutiveOnes = actualConsecutiveOnes;
			actualConsecutiveOnes = 0;

		}

	}
	if (actualConsecutiveOnes > maximalConsecutiveOnes)
		maximalConsecutiveOnes = actualConsecutiveOnes;

	return maximalConsecutiveOnes;

}

/****************************************************************
 * Method returns true, if at least one bit is set.				*
 ****************************************************************/

bool bytefield::any() {

	bool returnValue;

	if (count() > 0)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * Method returns true, if no bit is set.						*
 ****************************************************************/

bool bytefield::none() {

	return !any();

}

/****************************************************************
 * Method flips the value of every bit.							*
 ****************************************************************/

void bytefield::flip() {

	unsigned char index = this->position;

	this->position = 0;
	(*this)        = this->operator ^ (true);

	this->position = index;

}

/****************************************************************
 * Method sets the array to one.								*
 ****************************************************************/

void bytefield::set() {

	byte     = (byteSourceType)-1;
	byte   <<= cutBits;
	byte   >>= cutBits;
	position = 0;

}

/****************************************************************
 * Method sets the array to zero.								*
 ****************************************************************/

void bytefield::reset() {

	byte     = 0;
	position = 0;

}

/****************************************************************
 * Method flips the value of bit index.							*
 ****************************************************************/

void bytefield::flip(size_t index) {

	unsigned char tempIndex = this->position;

	(*this)[index] = ((*this)[index]) ^ (true);

	this->position = tempIndex;

}

/****************************************************************
 * Method sets the value of bit index to value.					*
 ****************************************************************/

void bytefield::set(size_t index, bool value) {

	unsigned char tempIndex = this->position;

	(*this)[index] = value;

	this->position = tempIndex;

}

/****************************************************************
 * Method sets the value of bit index to zero.					*
 ****************************************************************/

void bytefield::reset(size_t index) {

	unsigned char tempIndex = this->position;

	(*this)[index] = false;

	this->position = tempIndex;

}

/****************************************************************
 * Method returns true if bit index is set.						*
 ****************************************************************/

bool bytefield::test(size_t index) {

	unsigned char tempIndex = this->position;

	bool returnValue = (*this)[index];

	this->position   = tempIndex;

	return returnValue;

}

/****************************************************************
 * typecast to bool value										*
 ****************************************************************/

bytefield::operator bool() {

	byteSourceType temp;
	bool           returnValue;

	if ((unsigned char)(position - 1) < maxBits) {

		temp =   (byte << (maxBits - position));
		temp >>= (maxBits - 1);
	
		if (temp)
			returnValue = true;
		else
			returnValue = false;

	}
	else {

		temp = (byteSourceType)-1;

		if (byte == temp)
			returnValue = true;
		else
			returnValue = false;

	}

	position = 0;

	return returnValue;

}

/****************************************************************
 * operator []													*
 ****************************************************************/

bytefield& bytefield::operator [] (size_t index) {

	if (index < length()) {

		this->position = (unsigned char)index + 1;

	}
	else {

		throw outOfRangeError(DATAOBJECTLIB);

	}

	return (*this);

}

/****************************************************************
 * operator = ()												*
 * This operator handles also bool values because it			*
 * implicitely converts bool values to their numerical value	*
 * 0 or 1. So if an index is given on the left side of the		*
 * operator the value is interpreted as bool, else it is		*
 * treated as unsigned char.									*
 ****************************************************************/

const bytefield& bytefield::operator = (const unsigned long& value) {

	byteSourceType temp;

	if ((unsigned char)(position - 1) < length()) {

		temp = 1;
		temp <<= position - 1;

		if (value)
			byte |= temp;
		else
			byte &= (-temp-1);	// build !temp with dual-complement

	}

	else {

		byte = (byteSourceType)value;	/* reduction to the used number of bits */

	}

	byte   <<= cutBits;
	byte   >>= cutBits;
	position = 0;

	return (*this);

}
const bytefield& bytefield::operator = (const bytefield& value) {

	if ((unsigned char)(this->position - 1) < length()) {

		*this = value.byte;

	}
	else {
		
		this->byte     = value.byte;
		this->position = value.position;

	}

	this->byte   <<= cutBits;
	this->byte   >>= cutBits;

	return (*this);

}

/****************************************************************
 * operator == ()												*
 ****************************************************************/

bool bytefield::operator == (bool value) {

	bool           returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(position - 1) < length()) {

		returnValue = (bool)(*this);
		if (!value)
			returnValue = !returnValue;

	}

	else {

		minusOne   = (byteSourceType)-1;
		minusOne <<= cutBits;
		minusOne >>= cutBits;

		if (value) {
			if (byte == minusOne)
				returnValue = true;
			else
				returnValue = false;
		}
		else {
			if (byte == 0)
				returnValue = true;
			else
				returnValue = false;
		}

	}

	position = 0;

	return returnValue;

}
bool bytefield::operator == (unsigned long value) {

	bool           returnValue;
	byteSourceType tempValue;

	if ((unsigned char)(position - 1) < length()) {

		returnValue = (bool)(*this);
		if (!value)
			returnValue = !returnValue;

	}

	else {

		tempValue   = (byteSourceType)value;	/* reduction to the used number of bits */
		tempValue <<= cutBits;
		tempValue >>= cutBits;

		returnValue = (byte == tempValue);

	}

	position = 0;

	return returnValue;

}
bool bytefield::operator == (bytefield& value) {

	unsigned char _length;
	bool          returnValue;

	_length = (unsigned char)this->length();

	if ((unsigned char)(value.position - 1) < _length)
		returnValue = ((*this) == (bool)value); 
	else if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(value.position - 1) >= _length))
		returnValue = (value == (bool)(*this)); 
	else
		returnValue = (value.byte == (*this).byte);

	this->position = 0;
	value.position = 0;

	return returnValue;

}
bool operator == (bool operand1, bytefield& operand2) {

	bool           returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(operand2.position - 1) < operand2.length()) {

		returnValue = (bool)operand2;
		if (!operand1)
			returnValue = !returnValue;

	}

	else {

		if (operand1) {

			minusOne   = (byteSourceType)-1;
			minusOne <<= cutBits;
			minusOne >>= cutBits;

			if (operand2.byte == minusOne)
				returnValue = true;
			else
				returnValue = false;

		}
		else {
			if (operand2.byte == 0)
				returnValue = true;
			else
				returnValue = false;
		}

	}

	operand2.position = 0;

	return returnValue;

}
bool operator == (unsigned long operand1, bytefield& operand2) {

	bool           returnValue;
	byteSourceType tempOperand1;

	if ((unsigned char)(operand2.position - 1) < operand2.length()) {

		returnValue = (bool)operand2;
		if (!operand1)
			returnValue = !returnValue;

	}

	else {

		tempOperand1   = (byteSourceType)operand1;	/* reduction to the used number of bits */
		tempOperand1 <<= cutBits;
		tempOperand1 >>= cutBits;
		returnValue    = (operand2.byte == tempOperand1);

	}

	operand2.position = 0;

	return returnValue;

}

/****************************************************************
 * operator != ()												*
 ****************************************************************/

bool bytefield::operator != (bool value) {

	return !((*this) == value);

}
bool bytefield::operator != (unsigned long value) {

	return !((*this) == value);

}

bool bytefield::operator != (bytefield& value) {

	return !((*this) == value);

}
bool operator != (bool operand1, bytefield& operand2) {

	return !(operand1 == operand2);

}
bool operator != (unsigned long operand1, bytefield& operand2) {

	return !(operand1 == operand2);

}

/****************************************************************
 * operator ~ () makes the inverse								*
 ****************************************************************/

bytefield& bytefield::operator ~ () {

	unsigned char index = (unsigned char)(this->position - 1);

	if (index < length())
		(*this)[index] = ((*this)[index]) ^ (true);
	else
		(*this) = this->operator ^ (true);

	this->position = 0;

	return (*this);

}

/****************************************************************
 * operator ! () makes the inverse								*
 ****************************************************************/

bytefield bytefield::operator ! () {

	bytefield returnValue;

	if ((unsigned char)(position - 1) < length()) {
		returnValue[0]       = ((*this)[(unsigned char)(position - 1)]) ^ (true);
		returnValue.position = 1;
	}
	else
		returnValue          = this->operator ^ (true);

	this->position = 0;

	return returnValue;

}

/****************************************************************
 * operator ^ () makes the logical xor							*
 ****************************************************************/

bytefield bytefield::operator ^ (bool operand) {

	bytefield      returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(position - 1) < length()) {

		returnValue[0]       = ((bool)(*this)) ^ operand;
		returnValue.position = 1;

	}
	else {

		if (operand) {
			minusOne         = (byteSourceType)-1;
			minusOne       <<= cutBits;
			minusOne       >>= cutBits;
			returnValue.byte = this->byte ^ minusOne;
		}
		else
			returnValue.byte = this->byte ^ (0);
		returnValue.position = 0;

	}

	position = 0;

	return returnValue;

}
bytefield operator ^ (bool operand1, bytefield& operand2) {

	bytefield      returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(operand2.position - 1) < operand2.length()) {

		returnValue[0]       = operand1 ^ ((bool)operand2);
		returnValue.position = 1;

	}
	else {

		if (operand1) {
			minusOne         = (byteSourceType)-1;
			minusOne       <<= cutBits;
			minusOne       >>= cutBits;
			returnValue.byte = operand2.byte ^ minusOne;
		}
		else
			returnValue.byte = operand2.byte ^ (0);
		returnValue.position = 0;

	}

	operand2.position = 0;

	return returnValue;

}
bytefield bytefield::operator ^ (bytefield& operand) {

	bool          operand1;
	bool          operand2;
	bytefield     returnValue;
	unsigned char _length = (unsigned char)this->length();

	if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(operand.position - 1) < _length)) {

		operand1             = (bool)(*this);
		operand2             = (bool)operand;
		returnValue[0]       = operand1 ^ operand2;
		returnValue.position = 1;

	}
	else if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(operand.position - 1) >= _length)) {

		operand1    = (bool)(*this);
		returnValue = operand.operator ^ (operand1);

	}
	else if (((unsigned char)(this->position - 1) >= _length) && ((unsigned char)(operand.position - 1) < _length)) {

		operand2    = (bool)operand;
		returnValue = this->operator ^ (operand2);

	}
	else {

		returnValue.byte     = this->byte ^ operand.byte;
		returnValue.position = 0;

	}

	this->position   = 0;
	operand.position = 0;

	return returnValue;

}

/****************************************************************
 * operator & () makes the logical and							*
 ****************************************************************/

bytefield bytefield::operator & (bool operand) {

	bytefield      returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(position - 1) < length()) {

		returnValue[0]       = ((bool)(*this)) & operand;
		returnValue.position = 1;

	}
	else {

		if (operand) {
			minusOne         = (byteSourceType)-1;
			minusOne       <<= cutBits;
			minusOne       >>= cutBits;
			returnValue.byte = this->byte & minusOne;
		}
		else
			returnValue.byte = this->byte & (0);
		returnValue.position = 0;

	}

	position = 0;

	return returnValue;

}
bytefield operator & (bool operand1, bytefield& operand2) {

	bytefield      returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(operand2.position - 1) < operand2.length()) {

		returnValue[0]       = operand1 & ((bool)operand2);
		returnValue.position = 1;

	}
	else {

		if (operand1) {
			minusOne         = (byteSourceType)-1;
			minusOne       <<= cutBits;
			minusOne       >>= cutBits;
			returnValue.byte = operand2.byte ^ minusOne;
		}
		else
			returnValue.byte = operand2.byte ^ (0);
		returnValue.position = 0;

	}

	operand2.position = 0;

	return returnValue;

}
bytefield bytefield::operator & (bytefield& operand) {

	bool          operand1;
	bool          operand2;
	bytefield     returnValue;
	unsigned char _length = (unsigned char)this->length();

	if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(operand.position - 1) < _length)) {

		operand1             = (bool)(*this);
		operand2             = (bool)operand;
		returnValue[0]       = operand1 & operand2;
		returnValue.position = 1;

	}
	else if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(operand.position - 1) >= _length)) {

		operand1    = (bool)(*this);
		returnValue = operand.operator & (operand1);

	}
	else if (((unsigned char)(this->position - 1) >= _length) && ((unsigned char)(operand.position - 1) < _length)) {

		operand2    = (bool)operand;
		returnValue = this->operator & (operand2);

	}
	else {

		returnValue.byte     = this->byte & operand.byte;
		returnValue.position = 0;

	}

	this->position   = 0;
	operand.position = 0;

	return returnValue;

}

/****************************************************************
 * operator && () makes the logical and							*
 ****************************************************************/

bytefield bytefield::operator && (bool operand) {

	return (*this & operand);

}
bytefield operator && (bool operand1, bytefield& operand2) {

	return (operand1 & operand2);

}
bytefield bytefield::operator && (bytefield& operand) {

	return (*this & operand);

}

/****************************************************************
 * operator | () makes the logical or							*
 ****************************************************************/

bytefield bytefield::operator | (bool operand) {

	bytefield      returnValue;
	byteSourceType minusOne = (byteSourceType)-1;

	if ((unsigned char)(position - 1) < length()) {

		returnValue[0]     = ((bool)(*this)) | operand;
		returnValue.position = 1;

	}
	else {

		if (operand) {
			minusOne         = (byteSourceType)-1;
			minusOne       <<= cutBits;
			minusOne       >>= cutBits;
			returnValue.byte = this->byte | minusOne;
		}
		else
			returnValue.byte = this->byte | (0);
		returnValue.position = 0;

	}

	position = 0;

	return returnValue;

}
bytefield operator | (bool operand1, bytefield& operand2) {

	bytefield      returnValue;
	byteSourceType minusOne;

	if ((unsigned char)(operand2.position - 1) < operand2.length()) {

		returnValue[0]       = operand1 | ((bool)operand2);
		returnValue.position = 1;

	}
	else {

		if (operand1) {
			minusOne         = (byteSourceType)-1;
			minusOne       <<= cutBits;
			minusOne       >>= cutBits;
			returnValue.byte = operand2.byte | minusOne;
		}
		else
			returnValue.byte = operand2.byte | (0);
		returnValue.position = 0;

	}

	operand2.position = 0;

	return returnValue;

}
bytefield bytefield::operator | (bytefield& operand) {

	bool          operand1;
	bool          operand2;
	bytefield     returnValue;
	unsigned char _length = (unsigned char)this->length();

	if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(operand.position - 1) < _length)) {

		operand1             = (bool)(*this);
		operand2             = (bool)operand;
		returnValue[0]       = operand1 | operand2;
		returnValue.position = 1;

	}
	else if (((unsigned char)(this->position - 1) < _length) && ((unsigned char)(operand.position - 1) >= _length)) {

		operand1    = (bool)(*this);
		returnValue = operand.operator | (operand1);

	}
	else if (((unsigned char)(this->position - 1) >= _length) && ((unsigned char)(operand.position - 1) < _length)) {

		operand2    = (bool)operand;
		returnValue = this->operator | (operand2);

	}
	else {

		returnValue.byte     = this->byte | operand.byte;
		returnValue.position = 0;

	}

	this->position   = 0;
	operand.position = 0;

	return returnValue;

}

/****************************************************************
 * operator || () makes the logical or							*
 ****************************************************************/

bytefield bytefield::operator || (bool operand) {

	return (*this | operand);

}
bytefield operator || (bool operand1, bytefield& operand2) {

	return (operand1 | operand2);

}
bytefield bytefield::operator || (bytefield& operand) {

	return (*this | operand);

}

/****************************************************************
 * operator ^= () makes the logical xor							*
 ****************************************************************/

bytefield& bytefield::operator ^= (bool operand) {

	unsigned char index;
	bytefield     temp;

	index = (unsigned char)(this->position - 1);
	temp  = *this ^ operand;

	if (index < length())
		(*this)[index] = temp;
	else
		*this = temp;

	this->position = 0;

	return (*this);

}
bytefield& bytefield::operator ^= (bytefield& operand) {

	unsigned char index;
	bytefield     temp;

	index = (unsigned char)(this->position - 1);
	temp  = *this ^ operand;

	if (index < length())
		(*this)[index] = temp;
	else
		*this = temp;

	this->position   = 0;
	operand.position = 0;

	return (*this);

}

/****************************************************************
 * operator &= () makes the logical and							*
 ****************************************************************/

bytefield& bytefield::operator &= (bool operand) {

	unsigned char index;
	bytefield     temp;

	index = (unsigned char)(this->position - 1);
	temp  = *this & operand;

	if (index < length())
		(*this)[index] = temp;
	else
		*this = temp;

	this->position = 0;

	return (*this);

}
bytefield& bytefield::operator &= (bytefield& operand) {

	unsigned char index;
	bytefield     temp;

	index = (unsigned char)(this->position - 1);
	temp  = *this & operand;

	if (index < length())
		(*this)[index] = temp;
	else
		*this = temp;

	this->position   = 0;
	operand.position = 0;

	return (*this);

}

/****************************************************************
 * operator |= () makes the logical and							*
 ****************************************************************/

bytefield& bytefield::operator |= (bool operand) {

	unsigned char index;
	bytefield     temp;

	index = (unsigned char)(this->position - 1);
	temp  = *this | operand;

	if (index < length())
		(*this)[index] = temp;
	else
		*this = temp;

	this->position = 0;

	return (*this);

}
bytefield& bytefield::operator |= (bytefield& operand) {

	unsigned char index;
	bytefield     temp;

	index = (unsigned char)(this->position - 1);
	temp  = *this | operand;

	if (index < length())
		(*this)[index] = temp;
	else
		*this = temp;

	this->position   = 0;
	operand.position = 0;

	return (*this);

}

/****************************************************************
 * operator << ()												*
 ****************************************************************/

bytefield bytefield::operator << (size_t value) {

	bytefield returnValue;

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4267)
#endif
	returnValue.byte     = (byte << value);	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4267)
#endif
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	this->position       = 0;

	return returnValue;

}
bytefield bytefield::operator << (int value) {

	return (*this << (size_t)value);

}

/****************************************************************
 * operator <<= ()												*
 ****************************************************************/

bytefield& bytefield::operator <<= (size_t value) {

	this->byte   <<= value;
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;

	return *this;

}
bytefield& bytefield::operator <<= (int value) {

	return (*this <<= (size_t)value);

}

/****************************************************************
 * operator >> ()												*
 ****************************************************************/

bytefield bytefield::operator >> (size_t value) {

	bytefield returnValue;

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4267)
#endif
	returnValue.byte     = (byte >> value);	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4267)
#endif
	returnValue.position = 0;
	this->position = 0;

	return returnValue;

}
bytefield bytefield::operator >> (int value) {

	return (*this >> (size_t)value);

}

/****************************************************************
 * operator >>= ()												*
 ****************************************************************/

bytefield& bytefield::operator >>= (size_t value) {

	this->byte   >>= value;
	this->position = 0;

	return *this;

}
bytefield& bytefield::operator >>= (int value) {

	return (*this >>= (size_t)value);

}

/****************************************************************
 * operator < ()												*
 ****************************************************************/

bool bytefield::operator < (unsigned long value) {

	byteSourceType tempValue;

	tempValue   = (byteSourceType)value;	/* reduction to the used number of bits */
	tempValue <<= cutBits;
	tempValue >>= cutBits;

	this->position = 0;
	return (this->byte < tempValue);

}
bool bytefield::operator < (bytefield& value) {

	this->position = 0;
	value.position = 0;
	return (this->byte < value.byte);

}

/****************************************************************
 * operator <= ()												*
 ****************************************************************/

bool bytefield::operator <= (unsigned long value) {

	byteSourceType tempValue;

	tempValue   = (byteSourceType)value;	/* reduction to the used number of bits */
	tempValue <<= cutBits;
	tempValue >>= cutBits;

	this->position = 0;
	return (this->byte <= tempValue);

}
bool bytefield::operator <= (bytefield& value) {

	this->position = 0;
	value.position = 0;
	return (this->byte <= value.byte);

}

/****************************************************************
 * operator > ()												*
 ****************************************************************/

bool bytefield::operator > (unsigned long value) {

	byteSourceType tempValue;

	tempValue   = (byteSourceType)value;	/* reduction to the used number of bits */
	tempValue <<= cutBits;
	tempValue >>= cutBits;

	this->position = 0;
	return (this->byte > tempValue);

}
bool bytefield::operator > (bytefield& value) {

	this->position = 0;
	value.position = 0;
	return (this->byte > value.byte);

}

/****************************************************************
 * operator >= ()												*
 ****************************************************************/

bool bytefield::operator >= (unsigned long value) {

	byteSourceType tempValue;

	tempValue   = (byteSourceType)value;	/* reduction to the used number of bits */
	tempValue <<= cutBits;
	tempValue >>= cutBits;

	this->position = 0;
	return (this->byte >= tempValue);

}
bool bytefield::operator >= (bytefield& value) {

	this->position = 0;
	value.position = 0;
	return (this->byte >= value.byte);

}

/****************************************************************
 * operator +													*
 ****************************************************************/

bytefield bytefield::operator + (unsigned long value) {

	bytefield returnValue;

	returnValue.byte     = (byteSourceType)(this->byte + value);	/* reduction to the used number of bits */
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	this->position       = 0;

	return returnValue;

}
bytefield bytefield::operator + (bytefield& value) {

	bytefield returnValue;

	returnValue.byte     = this->byte + value.byte;
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;

	returnValue.position = 0;
	this->position       = 0;
	value.position       = 0;

	return returnValue;

}
bytefield operator + (unsigned long operand1, bytefield& operand2) {

	bytefield returnValue;

	returnValue.byte     = (byteSourceType)(operand1 + operand2.byte);	/* reduction to the used number of bits */
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	operand2.position    = 0;

	return returnValue;

}

/****************************************************************
 * operator +=													*
 ****************************************************************/

bytefield& bytefield::operator += (unsigned long value) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
	this->byte    += value;	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;

	return *this;

}
bytefield& bytefield::operator += (bytefield& value) {

	this->byte    += value.byte;
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;
	value.position = 0;

	return *this;

}

/****************************************************************
 * operator -													*
 ****************************************************************/

bytefield bytefield::operator - (unsigned long value) {

	bytefield returnValue;

	returnValue.byte     = (byteSourceType)(this->byte - value);	/* reduction to the used number of bits */
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	this->position       = 0;

	return returnValue;

}
bytefield bytefield::operator - (bytefield& value) {

	bytefield returnValue;

	returnValue.byte     = this->byte - value.byte;
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	this->position       = 0;
	value.position       = 0;

	return returnValue;

}
bytefield operator - (unsigned long operand1, bytefield& operand2) {

	bytefield returnValue;

	returnValue.byte     = (byteSourceType)(operand1 - operand2.byte);	/* reduction to the used number of bits */
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	operand2.position    = 0;

	return returnValue;

}

/****************************************************************
 * operator -=													*
 ****************************************************************/

bytefield& bytefield::operator -= (unsigned long value) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
	this->byte    -= value;	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;

	return *this;

}
bytefield& bytefield::operator -= (bytefield& value) {

	this->byte    -= value.byte;
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;
	value.position = 0;

	return *this;

}

/****************************************************************
 * operator *													*
 ****************************************************************/

bytefield bytefield::operator * (unsigned long value) {

	bytefield returnValue;

	returnValue.byte     = (byteSourceType)(this->byte * value);	/* reduction to the used number of bits */
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	this->position       = 0;

	return returnValue;

}
bytefield bytefield::operator * (bytefield& value) {

	bytefield returnValue;

	returnValue.byte     = this->byte * value.byte;
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	this->position       = 0;
	value.position       = 0;

	return returnValue;

}
bytefield operator * (unsigned long operand1, bytefield& operand2) {

	bytefield returnValue;

	returnValue.byte     = (byteSourceType)(operand1 * operand2.byte);	/* reduction to the used number of bits */
	returnValue.byte   <<= cutBits;
	returnValue.byte   >>= cutBits;
	returnValue.position = 0;
	operand2.position    = 0;

	return returnValue;

}

/****************************************************************
 * operator *=													*
 ****************************************************************/

bytefield& bytefield::operator *= (unsigned long value) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
	this->byte    *= value;	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;

	return *this;

}
bytefield& bytefield::operator *= (bytefield& value) {

	this->byte    *= value.byte;
	this->byte   <<= cutBits;
	this->byte   >>= cutBits;
	this->position = 0;
	value.position = 0;

	return *this;

}

/****************************************************************
 * operator /													*
 ****************************************************************/

bytefield bytefield::operator / (unsigned long value) {

	bytefield returnValue;

	if (value != 0) {

		returnValue.byte     = (byteSourceType)(this->byte / value);	/* reduction to the used number of bits */
		returnValue.byte   <<= cutBits;
		returnValue.byte   >>= cutBits;
		returnValue.position = 0;
		this->position       = 0;

	}
	else {

		returnValue = 0;
		throw zeroDivisionError(DATAOBJECTLIB);

	}

	return returnValue;

}
bytefield bytefield::operator / (bytefield& value) {

	bytefield returnValue;

	if (value.byte != 0) {

		returnValue.byte     = this->byte / value.byte;
		returnValue.byte   <<= cutBits;
		returnValue.byte   >>= cutBits;
		returnValue.position = 0;
		this->position       = 0;
		value.position       = 0;
	
	}
	else {

		returnValue = 0;
		throw zeroDivisionError(DATAOBJECTLIB);

	}

	return returnValue;

}
bytefield operator / (unsigned long operand1, bytefield& operand2) {

	bytefield returnValue;

	if (operand2.byte != 0) {

		returnValue.byte     = (byteSourceType)(operand1 / operand2.byte);	/* reduction to the used number of bits */
		returnValue.byte   <<= cutBits;
		returnValue.byte   >>= cutBits;
		returnValue.position = 0;
		operand2.position    = 0;

	}
	else {

		returnValue = 0;
		throw zeroDivisionError(DATAOBJECTLIB);

	}

	return returnValue;

}

/****************************************************************
 * operator /=													*
 ****************************************************************/

bytefield& bytefield::operator /= (unsigned long value) {

	if (value != 0) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		this->byte    /= value;	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif
		this->byte   <<= cutBits;
		this->byte   >>= cutBits;
		this->position = 0;

	}
	else {

		throw zeroDivisionError(DATAOBJECTLIB);

	}

	return *this;

}
bytefield& bytefield::operator /= (bytefield& value) {

	if (value.byte != 0) {

		this->byte    /= value.byte;
		this->byte   <<= cutBits;
		this->byte   >>= cutBits;
		this->position = 0;
		value.position = 0;

	}
	else {

		throw zeroDivisionError(DATAOBJECTLIB);

	}

	return *this;

}

/****************************************************************
 * operator %													*
 ****************************************************************/

bytefield bytefield::operator % (unsigned long value) {

	bytefield returnValue;

	if (value != 0) {

		returnValue.byte     = (byteSourceType)(this->byte % value);	/* reduction to the used number of bits */
		returnValue.byte   <<= cutBits;
		returnValue.byte   >>= cutBits;
		returnValue.position = 0;
		this->position       = 0;

	}
	else {

		returnValue = 0;
		throw zeroModuloError(DATAOBJECTLIB);

	}

	return returnValue;

}
bytefield bytefield::operator % (bytefield& value) {

	bytefield returnValue;

	if (value.byte != 0) {

		returnValue.byte     = this->byte % value.byte;
		returnValue.byte   <<= cutBits;
		returnValue.byte   >>= cutBits;
		returnValue.position = 0;
		this->position       = 0;
		value.position       = 0;

	}
	else {

		returnValue = 0;
		throw zeroModuloError(DATAOBJECTLIB);

	}

	return returnValue;

}
bytefield operator % (unsigned long operand1, bytefield& operand2) {

	bytefield returnValue;

	if (operand2.byte != 0) {

		returnValue.byte     = (byteSourceType)(operand1 % operand2.byte);	/* reduction to the used number of bits */
		returnValue.byte   <<= cutBits;
		returnValue.byte   >>= cutBits;
		returnValue.position = 0;
		operand2.position    = 0;

	}
	else {

		returnValue = 0;
		throw zeroModuloError(DATAOBJECTLIB);

	}

	return returnValue;

}

/****************************************************************
 * operator %=													*
 ****************************************************************/

bytefield& bytefield::operator %= (unsigned long value) {

	if (value != 0) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		this->byte    %= value;	/* reduction to the used number of bits */
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif
		this->byte   <<= cutBits;
		this->byte   >>= cutBits;
		this->position = 0;

	}
	else {

		throw zeroModuloError(DATAOBJECTLIB);

	}

	return *this;

}
bytefield& bytefield::operator %= (bytefield& value) {

	if (value.byte != 0) {

		this->byte    %= value.byte;
		this->byte   <<= cutBits;
		this->byte   >>= cutBits;
		this->position = 0;
		value.position = 0;

	}
	else {

		throw zeroModuloError(DATAOBJECTLIB);

	}

	return *this;

}

/****************************************************************
 * operator ++													*
 ****************************************************************/

bytefield& bytefield::operator ++ () {

	this->byte  += 1;
	this->byte <<= cutBits;
	this->byte >>= cutBits;

	return *this;

}
bytefield  bytefield::operator ++ (int) {

	bytefield returnValue;

	returnValue  = *this;
	this->byte  += 1;
	this->byte <<= cutBits;
	this->byte >>= cutBits;

	return returnValue;

}

/****************************************************************
 * operator --													*
 ****************************************************************/

bytefield& bytefield::operator -- () {

	this->byte  -= 1;
	this->byte <<= cutBits;
	this->byte >>= cutBits;

	return *this;

}
bytefield  bytefield::operator -- (int) {

	bytefield returnValue;

	returnValue  = *this;
	this->byte  -= 1;
	this->byte <<= cutBits;
	this->byte >>= cutBits;

	return returnValue;

}

/****************************************************************
 * Method sets the format of the stream:						*
 * -  0: default												*
 * -  2: binary													*
 * -  8: octal													*
 * - 10: decimal												*
 * - 16: hexadecimal											*
 ****************************************************************/

void bytefield::formatStream(std::ostream& stream, unsigned short format) {

	stream.unsetf(std::ios_base::oct);
	stream.unsetf(std::ios_base::dec);
	stream.unsetf(std::ios_base::hex);
	stream.unsetf(std::ios_base::uppercase);
	stream.unsetf(std::ios_base::boolalpha);
	stream.unsetf(std::ios_base::showbase);

	switch(format) {
		
		case 2:
			stream.setf(std::ios_base::dec);
			stream.setf(std::ios_base::boolalpha);
			break;
		case 8:
			stream.setf(std::ios_base::showbase);
			stream.setf(std::ios_base::oct);
			break;
		case 16:
			stream.setf(std::ios_base::uppercase);
			stream.setf(std::ios_base::showbase);
			stream.setf(std::ios_base::hex);
			break;
		default:
			stream.setf(std::ios_base::dec);
			break;

	}

}

/****************************************************************
 * operator << (stream)											*
 ****************************************************************/

std::ostream& operator << (std::ostream& os, bytefield& anyObject) {

	unsigned char index = (unsigned char)(anyObject.position - 1);

	if (os.flags() & std::ios_base::boolalpha) {

		os << "b";

		if (index < anyObject.length()) {

			if (anyObject[index])
				os << "1";
			else
				os << "0";

		}
		else {

			for (unsigned char i = (unsigned char)anyObject.length(); i > 0; i--) {
				if (anyObject[i-1])
					os << "1";
				else
					os << "0";
			}

			anyObject.position = index + 2;
	
		}

	}
	else
		os << (unsigned long)anyObject.byte;

	return os;

}

/****************************************************************
 * operator >> (stream)											*
 * Understanding number conventions:							*
 * - Binary:													*
 *   - Sign|b|Zahl		(example: b5, -b7, +b6)					*
 *   - b|Sign|Zahl		(example: b5, b-7, b+6)					*
 *   - Sign|B|Zahl		(example: B5, -B7, +B6)					*
 *   - B|Sign|Zahl		(example: B5, B-7, B+6)					*
 *   - Sign|2x|Zahl		(example: 2x5, -2x7, +2x6)				*
 *   - 2x|Sign|Zahl		(example: 2x5, 2x-7, 2x+6)				*
 *   - Sign|2X|Zahl		(example: 2X5, -2X7, +2X6)				*
 *   - 2X|Sign|Zahl		(example: 2X5, 2X-7, 2X+6)				*
 * - Octal:														*
 *   - Sign|0|Zahl		(example: 05, -07, +06)					*
 *   - 0|Sign|Zahl		(example: 05, 0-7, 0+6)					*
 *   - Sign|o|Zahl		(example: o5, -o7, +o6)					*
 *   - o|Sign|Zahl		(example: o5, o-7, o+6)					*
 *   - Sign|O|Zahl		(example: O5, -O7, +O6)					*
 *   - O|Sign|Zahl		(example: O5, O-7, O+6)					*
 *   - Sign|8x|Zahl		(example: 8x5, -8x7, +8x6)				*
 *   - 8x|Sign|Zahl		(example: 8x5, 8x-7, 8x+6)				*
 *   - Sign|8X|Zahl		(example: 8X5, -8X7, +8X6)				*
 *   - 8X|Sign|Zahl		(example: 8X5, 8X-7, 8X+6)				*
 * - Decimal:													*
 *   - Sign|Zahl		(example: 5, -7, +6)					*
 *   - Sign|d|Zahl		(example: d5, -d7, +d6)					*
 *   - d|Sign|Zahl		(example: d5, d-7, d+6)					*
 *   - Sign|D|Zahl		(example: D5, -D7, +D6)					*
 *   - D|Sign|Zahl		(example: D5, D-7, D+6)					*
 *   - Sign|10x|Zahl	(example: 10x5, -10x7, +10x6)			*
 *   - 10x|Sign|Zahl	(example: 10x5, 10x-7, 10x+6)			*
 *   - Sign|10X|Zahl	(example: 10X5, -10X7, +10X6)			*
 *   - 10X|Sign|Zahl	(example: 10X5, 10X-7, 10X+6)			*
 * - Hexadecimal:												*
 *   - Sign|h|Zahl		(example: h5, -h7, +h6)					*
 *   - h|Sign|Zahl		(example: h5, h-7, h+6)					*
 *   - Sign|H|Zahl		(example: H5, -H7, +H6)					*
 *   - H|Sign|Zahl		(example: H5, H-7, H+6)					*
 *   - Sign|x|Zahl		(example: x5, -x7, +x6)					*
 *   - x|Sign|Zahl		(example: x5, x-7, x+6)					*
 *   - Sign|X|Zahl		(example: X5, -X7, +X6)					*
 *   - X|Sign|Zahl		(example: X5, X-7, X+6)					*
 *   - Sign|0x|Zahl		(example: 0x5, -0x7, +0x6)				*
 *   - 0x|Sign|Zahl		(example: 0x5, 0x-7, 0x+6)				*
 *   - Sign|0X|Zahl		(example: 0X5, -0X7, +0X6)				*
 *   - 0X|Sign|Zahl		(example: 0X5, 0X-7, 0X+6)				*
 *   - Sign|16x|Zahl	(example: 16x5, -16x7, +16x6)			*
 *   - 16x|Sign|Zahl	(example: 16x5, 16x-7, 16x+6)			*
 *   - Sign|16X|Zahl	(example: 16X5, -16X7, +16X6)			*
 *   - 16X|Sign|Zahl	(example: 16X5, 16X-7, 16X+6)			*
 ****************************************************************/

std::istream& operator >> (std::istream& is, bytefield& anyObject) {

	std::string value;

	is >> value;

	anyObject.valueFromString(value);	/* reduction to the used number of bits */
	anyObject.position = 0;

	return is;

}

/****************************************************************
 * Method computes the a string representation based on the		*
 * value														*
 ****************************************************************/

std::string bytefield::toString(int radix) {

	unsigned char index = this->position;
	std::string   returnValue;
	int           counts;
	int           maxCounts;
	std::string   conversion;

	returnValue = "000";
	counts      = addRadix(radix, returnValue);
	maxCounts   = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	if (radix == 2)
		for (size_t i = length(); i > 0; i--)
			if ((*this)[i-1])
				returnValue += "1";
			else
				returnValue += "0";
	else {
		ultos(toULong(), &conversion, radix, 0);
		returnValue += conversion;
	}

	this->position = index;

	return returnValue;

}

/****************************************************************
 * Method computes the unsigned long representation based on the*
 * value. If not possible the value is modulo max UNSIGNED LONG	*
 ****************************************************************/

unsigned long bytefield::toULong() {

	return (unsigned long)byte;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double bytefield::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(byte);
	returnValue += sizeof(position);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double bytefield::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double bytefield::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(byte);
	returnValue += sizeof(position);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
