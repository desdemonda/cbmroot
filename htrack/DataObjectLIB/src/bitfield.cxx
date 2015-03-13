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
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/bitfield.h"


/****************************************************************
 * CLASS bitfield								 				*
 ****************************************************************/

/****************************************************************
 * Method compares two bitsets to greater value.				*
 ****************************************************************/

bool bitfield::greater(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	bool returnValue;

	if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned char op1 = operand1.to_ulong();
		unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 > op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned short op1 = operand1.to_ulong();
		unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 > op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned int op1 = operand1.to_ulong();
		unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 > op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned long op1 = operand1.to_ulong();
		unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 > op2);

	}
	else {

		std::bitset<numberOfBits> temp;

		returnValue = false;
		temp        = operand1 ^ operand2;

		for (size_t i = numberOfBits; i > 0; i--) {

			if (temp[i-1]) {

				returnValue = operand1[i-1];
				break;

			}

		}

	}

	return returnValue;

}

/****************************************************************
 * Method compares two bitsets to greater or equal value.		*
 ****************************************************************/

bool bitfield::greaterOrEqual(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	bool                      returnValue;

	if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned char op1 = operand1.to_ulong();
		unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 >= op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned short op1 = operand1.to_ulong();
		unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 >= op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned int op1 = operand1.to_ulong();
		unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 >= op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned long op1 = operand1.to_ulong();
		unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = (op1 >= op2);

	}
	else {

		std::bitset<numberOfBits> temp;

		returnValue = true;
		temp        = operand1 ^ operand2;

		for (size_t i = numberOfBits; i > 0; i--) {

			if (temp[i-1]) {

				returnValue = operand1[i-1];
				break;

			}

		}

	}

	return returnValue;

}

/****************************************************************
 * Method adds two bitsets										*
 ****************************************************************/

std::bitset<numberOfBits> bitfield::add(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	std::bitset<numberOfBits> returnValue;

	if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned char op1 = operand1.to_ulong();
		unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 + op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned short op1 = operand1.to_ulong();
		unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 + op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned int op1 = operand1.to_ulong();
		unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 + op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned long op1 = operand1.to_ulong();
		unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 + op2);

	}
	else {

		bool carryIn;
		bool carryOut;

		carryIn     = false;
		returnValue = 0;

		for (size_t i = 0; i < numberOfBits; i++) {
		
			if ((operand1[i] == 0) && (operand2[i] == 0) && (carryIn == 0))
				continue;

			returnValue[i] = carryIn ^ operand1[i] ^ operand2[i];
			carryOut       = (carryIn && (operand1[i] || operand2[i])) || (operand1[i] && operand2[i]);
			carryIn        = carryOut;

		}

	}

	return returnValue;

}

/****************************************************************
 * Method subtracts two bitsets									*
 ****************************************************************/

std::bitset<numberOfBits> bitfield::sub(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	std::bitset<numberOfBits> returnValue;

	if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned char op1 = operand1.to_ulong();
		unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 - op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned short op1 = operand1.to_ulong();
		unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 - op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned int op1 = operand1.to_ulong();
		unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 - op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned long op1 = operand1.to_ulong();
		unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 - op2);

	}
	else {

		bool carryIn;
		bool carryOut;

		carryIn     = false;
		returnValue = 0;

		for (size_t i = 0; i < numberOfBits; i++) {
		
			if ((operand1[i] == 0) && (operand2[i] == 0) && (carryIn == 0))
				continue;

			returnValue[i] = carryIn ^ operand1[i] ^ operand2[i];
			carryOut       = ((operand2[i] ^ carryIn) && !operand1[i]) || (operand2[i] && carryIn);
			carryIn        = carryOut;

		}

	}

	return returnValue;

}

/****************************************************************
 * Method multiplies two bitsets								*
 ****************************************************************/

std::bitset<numberOfBits> bitfield::mult(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	std::bitset<numberOfBits> returnValue;

	if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned char op1 = operand1.to_ulong();
		unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 * op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned short op1 = operand1.to_ulong();
		unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 * op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned int op1 = operand1.to_ulong();
		unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 * op2);

	}
	else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
		unsigned long op1 = operand1.to_ulong();
		unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

		returnValue = std::bitset<numberOfBits>(op1 * op2);

	}
	else {

		returnValue = 0;

		for (size_t i = numberOfBits; i > 0; i--) {

			returnValue <<= 1;

			if (operand2[i-1])
				returnValue = add(returnValue, operand1);

		}

	}

	return returnValue;

}

/****************************************************************
 * Method divides two bitsets									*
 ****************************************************************/

std::bitset<numberOfBits> bitfield::div(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	std::bitset<numberOfBits> returnValue;

	returnValue = 0;

	if (operand2.any()) {

		if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned char op1 = operand1.to_ulong();
			unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 / op2);

		}
		else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned short op1 = operand1.to_ulong();
			unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 / op2);

		}
		else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned int op1 = operand1.to_ulong();
			unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 / op2);

		}
		else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned long op1 = operand1.to_ulong();
			unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 / op2);

		}
		else {

			std::bitset<numberOfBits> temp;

			temp          = 0;

			for (size_t i = numberOfBits; i > 0; i--) {

				temp    <<= 1;
				temp[0]   = operand1[i-1];

				if (greaterOrEqual(temp, operand2)) {

					temp             = sub(temp, operand2);
					returnValue[i-1] = true;

				}

			}

		}

	}
	else {

		throw zeroDivisionError(DATAOBJECTLIB);

	}

	return returnValue;

}

/****************************************************************
 * Method computes the rest of the two-bitsets-division			*
 ****************************************************************/

std::bitset<numberOfBits> bitfield::mod(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2) {

	std::bitset<numberOfBits> returnValue;

	returnValue = 0;

	if (operand2.any()) {

		if (numberOfBits < 8 * sizeof(unsigned char) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned char op1 = operand1.to_ulong();
			unsigned char op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 % op2);

		}
		else if (numberOfBits < 8 * sizeof(unsigned short) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned short op1 = operand1.to_ulong();
			unsigned short op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 % op2);

		}
		else if (numberOfBits < 8 * sizeof(unsigned int) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned int op1 = operand1.to_ulong();
			unsigned int op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 % op2);

		}
		else if (numberOfBits < 8 * sizeof(unsigned long) + 1) {

#if (ARCHITECTURE == STANDALONE)
#pragma warning(disable : 4244)
#endif
			unsigned long op1 = operand1.to_ulong();
			unsigned long op2 = operand2.to_ulong();
#if (ARCHITECTURE == STANDALONE)
#pragma warning(default : 4244)
#endif

			returnValue = std::bitset<numberOfBits>(op1 % op2);

		}
		else {

			for (size_t i = numberOfBits; i > 0; i--) {

				returnValue    <<= 1;
				returnValue[0]   = operand1[i-1];

				if (greaterOrEqual(returnValue, operand2))
					returnValue = sub(returnValue, operand2);

			}

		}

	}
	else {

		throw zeroModuloError(DATAOBJECTLIB);

	}

	return returnValue;

}

/****************************************************************
 * Method computes the value based on a string					*
 ****************************************************************/

void bitfield::valueFromString(std::string value) {

	int radix;

	extractRadix(&radix, &value);

	bits  = std::bitset<numberOfBits>(stoul(value, radix));

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

bitfield::bitfield() : bits() {

	reset();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

bitfield::bitfield(const bitfield& value) : bits(value.bits) {

  //	this->bits = value.bits;

}
bitfield::bitfield(unsigned long value) : bits(std::bitset<numberOfBits>(value))
{

  //	bits = std::bitset<numberOfBits>(value);

}
bitfield::bitfield(std::string value) : bits() {

	valueFromString(value);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

bitfield::~bitfield() {

}

/****************************************************************
 * Method returns the length of the array.						*
 ****************************************************************/

size_t bitfield::length() {

	return bits.size();

}

/****************************************************************
 * Method returns the size of the array in bytes.				*
 ****************************************************************/

size_t bitfield::size() {

	return sizeof(bitfield);

}

/****************************************************************
 * Method counts the ones in the array and returns the number.	*
 ****************************************************************/

size_t bitfield::count() {

	return bits.count();

}

/****************************************************************
 * Method counts the maximal consecutive ones in the array and	*
 * returns the number.											*
 ****************************************************************/

size_t bitfield::count_consecutive() {

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

bool bitfield::any() {

	return bits.any();

}

/****************************************************************
 * Method returns true, if no bit is set.						*
 ****************************************************************/

bool bitfield::none() {

	return bits.none();

}

/****************************************************************
 * Method flips the value of every bit.							*
 ****************************************************************/

void bitfield::flip() {

	bits.flip();

}

/****************************************************************
 * Method sets the array to one.								*
 ****************************************************************/

void bitfield::set() {

	bits.set();

}

/****************************************************************
 * Method sets the array to zero.								*
 ****************************************************************/

void bitfield::reset() {

	bits.reset();

}

/****************************************************************
 * Method flips the value of bit index.							*
 ****************************************************************/

void bitfield::flip(size_t index) {

	bits.flip(index);

}

/****************************************************************
 * Method sets the value of bit index to value.					*
 ****************************************************************/

void bitfield::set(size_t index, bool value) {

	bits.set(index, value);

}

/****************************************************************
 * Method sets the value of bit index to zero.					*
 ****************************************************************/

void bitfield::reset(size_t index) {

	bits.reset(index);

}

/****************************************************************
 * Method returns true if bit index is set.						*
 ****************************************************************/

bool bitfield::test(size_t index) {

	return bits.test(index);

}

/****************************************************************
 * operator []													*
 ****************************************************************/

bool bitfield::operator [] (size_t index) const {

	return bits[index];

}
std::bitset<numberOfBits>::reference bitfield::operator [] (size_t index) {

	return bits[index];

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const bitfield& bitfield::operator = (const unsigned long& value) {

	bits = value;

	return *this;

}
const bitfield& bitfield::operator = (const bitfield& value) {

	bits = value.bits;

	return *this;

}

/****************************************************************
 * operator == ()												*
 ****************************************************************/

bool bitfield::operator == (unsigned long value) {

	return (bits == std::bitset<numberOfBits>(value));

}
bool bitfield::operator == (bitfield& value) {

	return (bits == value.bits);

}

bool operator == (unsigned long operand1, bitfield& operand2) {
	
	return (std::bitset<numberOfBits>(operand1) == operand2.bits);

}

/****************************************************************
 * operator != ()												*
 ****************************************************************/

bool bitfield::operator != (unsigned long value) {

	return (bits != std::bitset<numberOfBits>(value));

}
bool bitfield::operator != (bitfield& value) {

	return (bits != value.bits);

}
bool operator != (unsigned long operand1, bitfield& operand2) {

	return (std::bitset<numberOfBits>(operand1) != operand2.bits);

}

/****************************************************************
 * operator ~ () makes the inverse								*
 ****************************************************************/

bitfield& bitfield::operator ~ () {

	bits.flip();

	return (*this);

}
std::bitset<numberOfBits>::reference& operator ~ (std::bitset<numberOfBits>::reference& operand) {

//	return operand.flip();
	operand.flip();
	return operand;

}

/****************************************************************
 * operator ! () makes the inverse								*
 ****************************************************************/

bitfield bitfield::operator ! () {

	bitfield returnValue;

	returnValue.bits = bits;

	returnValue.flip();

	return returnValue;

}
bool operator ! (std::bitset<numberOfBits>::reference operand) {

	bool returnValue;

	returnValue = operand;

	return !returnValue;

}

/****************************************************************
 * operator ^ () makes the logical xor							*
 ****************************************************************/

bitfield bitfield::operator ^ (bool operand) {

	bitfield returnValue;

	if (operand)
		returnValue.bits = (bits ^ std::bitset<numberOfBits>((unsigned long)-1));
	else
		returnValue.bits = (bits ^ std::bitset<numberOfBits>(0));

	return returnValue;

}
bitfield operator ^ (bool operand1, bitfield& operand2) {

	bitfield returnValue;

	if (operand1)
		returnValue.bits = (std::bitset<numberOfBits>((unsigned long)-1) ^ operand2.bits);
	else
		returnValue.bits = (std::bitset<numberOfBits>(0) ^ operand2.bits);

	return returnValue;

}
bitfield bitfield::operator ^ (bitfield& operand) {


	bitfield returnValue;

	returnValue.bits = (bits ^ operand.bits);

	return returnValue;

}

/****************************************************************
 * operator & () makes the logical and							*
 ****************************************************************/

bitfield bitfield::operator & (bool operand) {

	bitfield returnValue;

	if (operand)
		returnValue.bits = (bits & std::bitset<numberOfBits>((unsigned long)-1));
	else
		returnValue.bits = (bits & std::bitset<numberOfBits>(0));

	return returnValue;

}
bitfield operator & (bool operand1, bitfield& operand2) {

	bitfield returnValue;

	if (operand1)
		returnValue.bits = (std::bitset<numberOfBits>((unsigned long)-1) & operand2.bits);
	else
		returnValue.bits = (std::bitset<numberOfBits>(0) & operand2.bits);

	return returnValue;

}
bitfield bitfield::operator & (bitfield& operand) {


	bitfield returnValue;

	returnValue.bits = (bits & operand.bits);

	return returnValue;

}

/****************************************************************
 * operator && () makes the logical and							*
 ****************************************************************/

bitfield bitfield::operator && (bool operand) {

	return (*this & operand);

}
bitfield operator && (bool operand1, bitfield& operand2) {

	return (operand1 & operand2);

}
bitfield bitfield::operator && (bitfield& operand) {

	return (*this & operand);

}

/****************************************************************
 * operator | () makes the logical or							*
 ****************************************************************/

bitfield bitfield::operator | (bool operand) {

	bitfield returnValue;

	if (operand)
		returnValue.bits = (bits | std::bitset<numberOfBits>((unsigned long)-1));
	else
		returnValue.bits = (bits | std::bitset<numberOfBits>(0));

	return returnValue;

}
bitfield operator | (bool operand1, bitfield& operand2) {

	bitfield returnValue;

	if (operand1)
		returnValue.bits = (std::bitset<numberOfBits>((unsigned long)-1) | operand2.bits);
	else
		returnValue.bits = (std::bitset<numberOfBits>(0) | operand2.bits);

	return returnValue;

}
bitfield bitfield::operator | (bitfield& operand) {

	bitfield returnValue;

	returnValue.bits = (bits | operand.bits);

	return returnValue;

}

/****************************************************************
 * operator || () makes the logical or							*
 ****************************************************************/

bitfield bitfield::operator || (bool operand) {

	return (*this | operand);

}
bitfield operator || (bool operand1, bitfield& operand2) {

	return (operand1 | operand2);

}
bitfield bitfield::operator || (bitfield& operand) {

	return (*this | operand);

}

/****************************************************************
 * operator ^= () makes the logical xor							*
 ****************************************************************/

bitfield& bitfield::operator ^= (bool operand) {

	*this = *this ^ operand;

	return (*this);

}
std::bitset<numberOfBits>::reference& operator ^= (std::bitset<numberOfBits>::reference& operand1 , bool operand2) {

	operand1 = operand1 ^ operand2;

	return operand1;

}
bitfield& bitfield::operator ^= (bitfield& operand) {

	bits ^= operand.bits;

	return (*this);

}

/****************************************************************
 * operator &= () makes the logical and							*
 ****************************************************************/

bitfield& bitfield::operator &= (bool operand) {

	*this = *this & operand;

	return (*this);

}
std::bitset<numberOfBits>::reference& operator &= (std::bitset<numberOfBits>::reference& operand1 , bool operand2) {

	operand1 = operand1 & operand2;

	return operand1;

}
bitfield& bitfield::operator &= (bitfield& operand) {

	bits &= operand.bits;

	return (*this);

}

/****************************************************************
 * operator |= () makes the logical and							*
 ****************************************************************/

bitfield& bitfield::operator |= (bool operand) {

	*this = *this | operand;

	return (*this);

}
std::bitset<numberOfBits>::reference& operator |= (std::bitset<numberOfBits>::reference& operand1 , bool operand2) {

	operand1 = operand1 | operand2;

	return operand1;

}
bitfield& bitfield::operator |= (bitfield& operand) {

	bits |= operand.bits;

	return (*this);

}

/****************************************************************
 * operator << ()												*
 ****************************************************************/

bitfield bitfield::operator << (size_t value) {

	bitfield returnValue;

	returnValue.bits = (bits << value);

	return returnValue;

}

/****************************************************************
 * operator <<= ()												*
 ****************************************************************/

bitfield& bitfield::operator <<= (size_t value) {

	bits <<= value;

	return *this;

}

/****************************************************************
 * operator >> ()												*
 ****************************************************************/

bitfield bitfield::operator >> (size_t value) {

	bitfield returnValue;

	returnValue.bits = (bits >> value);

	return returnValue;

}

/****************************************************************
 * operator >>= ()												*
 ****************************************************************/

bitfield& bitfield::operator >>= (size_t value) {

	bits >>= value;	/* reduction to the used number of bits */

	return *this;

}

/****************************************************************
 * operator < ()												*
 ****************************************************************/

bool bitfield::operator < (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	return greater(operand.bits, this->bits);

}
bool bitfield::operator < (bitfield& value) {

	return greater(value.bits, this->bits);

}

/****************************************************************
 * operator <= ()												*
 ****************************************************************/

bool bitfield::operator <= (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	return greaterOrEqual(operand.bits, this->bits);

}
bool bitfield::operator <= (bitfield& value) {

	return greaterOrEqual(value.bits, this->bits);

}

/****************************************************************
 * operator > ()												*
 ****************************************************************/

bool bitfield::operator > (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	return greater(this->bits, operand.bits);

}
bool bitfield::operator > (bitfield& value) {

	return greater(this->bits, value.bits);

}

/****************************************************************
 * operator >= ()												*
 ****************************************************************/

bool bitfield::operator >= (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	return greaterOrEqual(this->bits, operand.bits);

}
bool bitfield::operator >= (bitfield& value) {

	return greaterOrEqual(this->bits, value.bits);

}

/****************************************************************
 * operator + ()												*
 ****************************************************************/

bitfield bitfield::operator + (unsigned long value) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(value);
	returnValue.bits = add(this->bits, operand.bits);

	return returnValue;

}
bitfield bitfield::operator + (bitfield& value) {

	bitfield returnValue;

	returnValue.bits = add(this->bits, value.bits);

	return returnValue;

}
bitfield operator + (unsigned long operand1, bitfield& operand2) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(operand1);
	returnValue.bits = operand2.add(operand.bits, operand2.bits);

	return returnValue;

}

/****************************************************************
 * operator += ()												*
 ****************************************************************/

bitfield& bitfield::operator += (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	bits    = add(bits, operand.bits);

	return *this;

}
bitfield& bitfield::operator += (bitfield& value) {

	bits = add(bits, value.bits);

	return *this;

}

/****************************************************************
 * operator - ()												*
 ****************************************************************/

bitfield bitfield::operator - (unsigned long value) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(value);
	returnValue.bits = sub(this->bits, operand.bits);

	return returnValue;

}
bitfield bitfield::operator - (bitfield& value) {

	bitfield returnValue;

	returnValue.bits = sub(this->bits, value.bits);

	return returnValue;

}
bitfield operator - (unsigned long operand1, bitfield& operand2) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(operand1);
	returnValue.bits = operand2.sub(operand.bits, operand2.bits);

	return returnValue;

}

/****************************************************************
 * operator -= ()												*
 ****************************************************************/

bitfield& bitfield::operator -= (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	bits    = sub(bits, operand.bits);

	return *this;

}
bitfield& bitfield::operator -= (bitfield& value) {

	bits = sub(bits, value.bits);

	return *this;

}

/****************************************************************
 * operator * ()												*
 ****************************************************************/

bitfield bitfield::operator * (unsigned long value) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(value);
	returnValue.bits = mult(this->bits, operand.bits);

	return returnValue;

}
bitfield bitfield::operator * (bitfield& value) {

	bitfield returnValue;

	returnValue.bits = mult(this->bits, value.bits);

	return returnValue;

}
bitfield operator * (unsigned long operand1, bitfield& operand2) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(operand1);
	returnValue.bits = operand2.mult(operand.bits, operand2.bits);

	return returnValue;

}

/****************************************************************
 * operator *=													*
 ****************************************************************/

bitfield& bitfield::operator *= (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	bits    = mult(bits, operand.bits);

	return *this;

}
bitfield& bitfield::operator *= (bitfield& value) {

	bits = mult(bits, value.bits);

	return *this;

}

/****************************************************************
 * operator / ()												*
 ****************************************************************/

bitfield bitfield::operator / (unsigned long value) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(value);
	returnValue.bits = div(this->bits, operand.bits);

	return returnValue;

}
bitfield bitfield::operator / (bitfield& value) {

	bitfield returnValue;

	returnValue.bits = div(this->bits, value.bits);

	return returnValue;

}
bitfield operator / (unsigned long operand1, bitfield& operand2) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(operand1);
	returnValue.bits = operand2.div(operand.bits, operand2.bits);

	return returnValue;

}

/****************************************************************
 * operator /= ()												*
 ****************************************************************/

bitfield& bitfield::operator /= (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	bits    = div(bits, operand.bits);

	return *this;

}
bitfield& bitfield::operator /= (bitfield& value) {

	bits = div(bits, value.bits);

	return *this;

}

/****************************************************************
 * operator % ()												*
 ****************************************************************/

bitfield bitfield::operator % (unsigned long value) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(value);
	returnValue.bits = mod(this->bits, operand.bits);

	return returnValue;

}
bitfield bitfield::operator % (bitfield& value) {

	bitfield returnValue;

	returnValue.bits = mod(this->bits, value.bits);

	return returnValue;

}
bitfield operator % (unsigned long operand1, bitfield& operand2) {

	bitfield operand;
	bitfield returnValue;

	operand          = bitfield(operand1);
	returnValue.bits = operand2.mod(operand.bits, operand2.bits);

	return returnValue;

}

/****************************************************************
 * operator %= ()												*
 ****************************************************************/

bitfield& bitfield::operator %= (unsigned long value) {

	bitfield operand;

	operand = bitfield(value);
	bits    = mod(bits, operand.bits);

	return *this;

}
bitfield& bitfield::operator %= (bitfield& value) {

	bits = mod(bits, value.bits);

	return *this;

}

/****************************************************************
 * operator ++													*
 ****************************************************************/

bitfield& bitfield::operator ++ () {

	bitfield operand;

	operand    = bitfield(1);
	this->bits = add(this->bits, operand.bits);

	return *this;

}
bitfield bitfield::operator ++ (int) {

	bitfield operand;
	bitfield returnValue;

	operand     = bitfield(1);
	returnValue = *this;
	this->bits  = add(this->bits, operand.bits);

	return returnValue;

}

/****************************************************************
 * operator --													*
 ****************************************************************/

bitfield& bitfield::operator -- () {

	bitfield operand;

	operand    = bitfield(1);
	this->bits = sub(this->bits, operand.bits);

	return *this;

}
bitfield bitfield::operator -- (int) {

	bitfield operand;
	bitfield returnValue;

	operand     = bitfield(1);
	returnValue = *this;
	this->bits  = sub(this->bits, operand.bits);

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

void bitfield::formatStream(std::ostream& stream, unsigned short format) {

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

std::ostream& operator << (std::ostream& os, std::bitset<numberOfBits>::reference& anyObject) {

	if (os.flags() & std::ios_base::boolalpha) {

		os << "b";
		if (anyObject)
			os << "1";
		else
			os << "0";

	}
	else
		os << anyObject;

	return os;

}
std::ostream& operator << (std::ostream& os, bitfield& anyObject) {

	if (os.flags() & std::ios_base::boolalpha) {

		os << "b";
		os << anyObject.bits;

	}
	else
		os << anyObject.bits.to_ulong();

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

std::istream& operator >> (std::istream& is, bitfield& anyObject) {

	std::string value;

	is >> value;

	anyObject.valueFromString(value);

	return is;

}

/****************************************************************
 * Method computes the a string representation based on the		*
 * value														*
 ****************************************************************/

std::string bitfield::toString(int radix) {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	std::string conversion;;

	returnValue = "000";
	counts      = addRadix(radix, returnValue);
	maxCounts   = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	if (radix == 2)
		for (size_t i = length(); i > 0; i--)
			if (bits[i-1])
				returnValue += "1";
			else
				returnValue += "0";
	else {
		ultos(toULong(), &conversion, radix, 0);
		returnValue += conversion;
	}

	return returnValue;

}

/****************************************************************
 * Method computes the unsigned long representation based on the*
 * value. If not possible the value is modulo max UNSIGNED LONG	*
 ****************************************************************/

unsigned long bitfield::toULong() {

	return bits.to_ulong();

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double bitfield::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(bits);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double bitfield::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double bitfield::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = ((double)numberOfBits) / 8;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
