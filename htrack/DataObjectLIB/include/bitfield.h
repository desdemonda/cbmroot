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
///     - The class optimizes the storage needed by array of bools
///     - The restriction is that there must be 8 bools or less
///
///     - Some basic functions are:
///     - length():  returns the number of elements
///     - size():    returns the storage amount in bytes
///     - count():   returns the number of elements which have the value true
///     - any():     returns true if at least one bit is true else false
///     - none():    returns true if no bit is true else false
///     - flip():    flips every bit
///     - set():     sets the value of each element to true
///     - reset():   sets the value of each element to false
///     - flip(n):   flips bit n
///     - set(n):    sets the value of element n to true
///     - reset(n):  sets the value of element n to false
///     - test(n):   returns true if the value of element n is true else false
///
///     - The class defines some basic operators like:
///       - []
///       - =, ==, !=
///       - ~, !, ^, &, &&, |, ||, ^=, &=, |=
///       - <<, <<=, >>, >>=
///       - <, <=, >, >=
///       - +, -, *, /, %, +=, -=, *=, /=, %=   /// The arithmetic operands are always used without []
///       - ++(), ()++, --(), ()--
///       - <<, >>
///
///     CAUTION:
///     - The operator & which returns the address is not supported
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:36:23 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _BITFIELD_H
#define _BITFIELD_H


#include "../../MiscLIB/include/memoryDef.h"
#include <bitset>
#include <iostream>


#define numberOfBits maxDimSH	/**< This defines the number of bits to store. */


/* **************************************************************
 * CLASS bitfield								 				*
 * **************************************************************/

class bitfield {

private:

/**
 * Method compares two bitsets to greater value.
 */

	bool greater(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method compares two bitsets to greater or equal value.
 */

	bool greaterOrEqual(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method adds two bitsets
 */

	std::bitset<numberOfBits> add(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method subtracts two bitsets
 */

	std::bitset<numberOfBits> sub(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method multiplies two bitsets
 */

	std::bitset<numberOfBits> mult(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method divides two bitsets
 */

	std::bitset<numberOfBits> div(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method computes the rest of the two-bitsets-division
 */

	std::bitset<numberOfBits> mod(std::bitset<numberOfBits>& operand1, std::bitset<numberOfBits>& operand2);

/**
 * Method computes the value based on a string
 */

	void valueFromString(std::string value);

protected:

	std::bitset<numberOfBits> bits;

public:

/**
 * Default constructor
 */

	bitfield();

/**
 * Constructor
 */

	bitfield(const bitfield& value);
	bitfield(unsigned long value);
	bitfield(std::string value);

/**
 * Destructor
 */

	virtual ~bitfield();

/**
 * Method returns the length of the array.
 */

	size_t length();

/**
 * Method returns the size of the array in bytes.
 */

	size_t size();

/**
 * Method counts the ones in the array and returns the number.
 */

	size_t count();

/**
 * Method counts the maximal consecutive ones in the array and returns the number.
 */

	size_t count_consecutive();

/**
 * Method returns true, if at least one bit is set.
 */

	bool any();

/**
 * Method returns true, if no bit is set.
 */

	bool none();

/**
 * Method flips the value of every bit.
 */

	void flip();

/**
 * Method sets the array to one.
 */

	void set();

/**
 * Method sets the array to zero.
 */

	void reset();

/**
 * Method flips the value of bit index.
 */

	void flip(size_t index);

/**
 * Method sets the value of bit index to value.
 */

	void set(size_t index, bool value = true);

/**
 * Method sets the value of bit index to zero.
 */

	void reset(size_t index);

/**
 * Method returns true if bit index is set.
 */

	bool test(size_t index);

/**
 * operator []
 */

	bool operator [] (size_t index) const;
	std::bitset<numberOfBits>::reference operator [] (size_t index);

/**
 * operator = ()
 */

	const bitfield& operator = (const unsigned long& value);
	const bitfield& operator = (const bitfield& value);

/**
 * operator == ()
 */

	bool operator == (unsigned long value);
	bool operator == (bitfield& value);
	friend bool operator == (unsigned long operand1, bitfield& operand2);

/**
 * operator != ()
 */

	bool operator != (unsigned long value);
	bool operator != (bitfield& value);
	friend bool operator != (unsigned long operand1, bitfield& operand2);

/**
 * operator ~ () makes the logical inverse
 */

	bitfield& operator ~ ();
	friend std::bitset<numberOfBits>::reference& operator ~ (std::bitset<numberOfBits>::reference& operand);

/**
 * operator ! () makes the logical inverse
 */

	bitfield operator ! ();
	friend bool operator ! (std::bitset<numberOfBits>::reference operand);

/**
 * operator ^ () makes the logical xor
 */

	bitfield operator ^ (bool operand);
	friend bitfield operator ^ (bool operand1, bitfield& operand2);
	bitfield operator ^ (bitfield& operand);

/**
 * operator & () makes the logical and
 */

	bitfield operator & (bool operand);
	friend bitfield operator & (bool operand1, bitfield& operand2);
	bitfield operator & (bitfield& operand);

/**
 * operator && () makes the logical and
 */

	bitfield operator && (bool operand);
	friend bitfield operator && (bool operand1, bitfield& operand2);
	bitfield operator && (bitfield& operand);

/**
 * operator | () makes the logical or
 */

	bitfield operator | (bool operand);
	friend bitfield operator | (bool operand1, bitfield& operand2);
	bitfield operator | (bitfield& operand);

/**
 * operator || () makes the logical or
 */

	bitfield operator || (bool operand);
	friend bitfield operator || (bool operand1, bitfield& operand2);
	bitfield operator || (bitfield& operand);

/**
 * operator ^= () makes the logical xor
 */

	bitfield& operator ^= (bool operand);
	friend std::bitset<numberOfBits>::reference& operator ^= (std::bitset<numberOfBits>::reference& operand1, bool operand2);
	bitfield& operator ^= (bitfield& operand);

/**
 * operator &= () makes the logical and
 */

	bitfield& operator &= (bool operand);
	friend std::bitset<numberOfBits>::reference& operator &= (std::bitset<numberOfBits>::reference& operand1, bool operand2);
	bitfield& operator &= (bitfield& operand);

/**
 * operator |= () makes the logical and
 */

	bitfield& operator |= (bool operand);
	friend std::bitset<numberOfBits>::reference& operator |= (std::bitset<numberOfBits>::reference& operand1, bool operand2);
	bitfield& operator |= (bitfield& operand);

/**
 * operator << ()
 */

	bitfield operator << (size_t value);

/**
 * operator <<= ()
 */

	bitfield& operator <<= (size_t value);

/**
 * operator >> ()
 */

	bitfield operator >> (size_t value);

/**
 * operator >>= ()
 */

	bitfield& operator >>= (size_t value);

/**
 * operator < ()
 */

	bool operator < (unsigned long value);
	bool operator < (bitfield& value);

/**
 * operator <= ()
 */

	bool operator <= (unsigned long value);
	bool operator <= (bitfield& value);

/**
 * operator > ()
 */

	bool operator > (unsigned long value);
	bool operator > (bitfield& value);

/**
 * operator >= ()
 */

	bool operator >= (unsigned long value);
	bool operator >= (bitfield& value);

/**
 * operator + ()
 */

	bitfield operator + (unsigned long value);
	bitfield operator + (bitfield& value);
	friend bitfield operator + (unsigned long operand1, bitfield& operand2);

/**
 * operator += ()
 */

	bitfield& operator += (unsigned long value);
	bitfield& operator += (bitfield& value);

/**
 * operator - ()
 */

	bitfield operator - (unsigned long value);
	bitfield operator - (bitfield& value);
	friend bitfield operator - (unsigned long operand1, bitfield& operand2);

/**
 * operator -= ()
 */

	bitfield& operator -= (unsigned long value);
	bitfield& operator -= (bitfield& value);

/**
 * operator * ()
 */

	bitfield operator * (unsigned long value);
	bitfield operator * (bitfield& value);
	friend bitfield operator * (unsigned long operand1, bitfield& operand2);

/**
 * operator *= ()
 */

	bitfield& operator *= (unsigned long value);
	bitfield& operator *= (bitfield& value);

/**
 * operator / ()
 */

	bitfield operator / (unsigned long value);
	bitfield operator / (bitfield& value);
	friend bitfield operator / (unsigned long operand1, bitfield& operand2);

/**
 * operator /= ()
 */

	bitfield& operator /= (unsigned long value);
	bitfield& operator /= (bitfield& value);

/**
 * operator % ()
 */

	bitfield operator % (unsigned long value);
	bitfield operator % (bitfield& value);
	friend bitfield operator % (unsigned long operand1, bitfield& operand2);

/**
 * operator %= ()
 */

	bitfield& operator %= (unsigned long value);
	bitfield& operator %= (bitfield& value);

/**
 * operator ++ ()
 */

	bitfield& operator ++ ();
	bitfield  operator ++ (int);

/**
 * operator -- ()
 */

	bitfield& operator -- ();
	bitfield  operator -- (int);

/**
 * Method sets the format of the stream:
 * -  0: default
 * -  2: binary
 * -  8: octal
 * - 10: decimal
 * - 16: hexadecimal
 */

	void formatStream(std::ostream& stream, unsigned short format);

/**
 * operator << (stream)
 */

	friend std::ostream& operator << (std::ostream& os, std::bitset<numberOfBits>::reference& anyObject);
	friend std::ostream& operator << (std::ostream& os, bitfield& anyObject);

/**
 * operator >> (stream)
 * Understanding number conventions:
 * - Binary:
 *   - Sign|b|Zahl		(example: b5, -b7, +b6)
 *   - b|Sign|Zahl		(example: b5, b-7, b+6)
 *   - Sign|B|Zahl		(example: B5, -B7, +B6)
 *   - B|Sign|Zahl		(example: B5, B-7, B+6)
 *   - Sign|2x|Zahl		(example: 2x5, -2x7, +2x6)
 *   - 2x|Sign|Zahl		(example: 2x5, 2x-7, 2x+6)
 *   - Sign|2X|Zahl		(example: 2X5, -2X7, +2X6)
 *   - 2X|Sign|Zahl		(example: 2X5, 2X-7, 2X+6)
 * - Octal:
 *   - Sign|0|Zahl		(example: 05, -07, +06)
 *   - 0|Sign|Zahl		(example: 05, 0-7, 0+6)
 *   - Sign|o|Zahl		(example: o5, -o7, +o6)
 *   - o|Sign|Zahl		(example: o5, o-7, o+6)
 *   - Sign|O|Zahl		(example: O5, -O7, +O6)
 *   - O|Sign|Zahl		(example: O5, O-7, O+6)
 *   - Sign|8x|Zahl		(example: 8x5, -8x7, +8x6)
 *   - 8x|Sign|Zahl		(example: 8x5, 8x-7, 8x+6)
 *   - Sign|8X|Zahl		(example: 8X5, -8X7, +8X6)
 *   - 8X|Sign|Zahl		(example: 8X5, 8X-7, 8X+6)
 * - Decimal:
 *   - Sign|Zahl		(example: 5, -7, +6)
 *   - Sign|d|Zahl		(example: d5, -d7, +d6)
 *   - d|Sign|Zahl		(example: d5, d-7, d+6)
 *   - Sign|D|Zahl		(example: D5, -D7, +D6)
 *   - D|Sign|Zahl		(example: D5, D-7, D+6)
 *   - Sign|10x|Zahl	(example: 10x5, -10x7, +10x6)
 *   - 10x|Sign|Zahl	(example: 10x5, 10x-7, 10x+6)
 *   - Sign|10X|Zahl	(example: 10X5, -10X7, +10X6)
 *   - 10X|Sign|Zahl	(example: 10X5, 10X-7, 10X+6)
 * - Hexadecimal:
 *   - Sign|h|Zahl		(example: h5, -h7, +h6)
 *   - h|Sign|Zahl		(example: h5, h-7, h+6)
 *   - Sign|H|Zahl		(example: H5, -H7, +H6)
 *   - H|Sign|Zahl		(example: H5, H-7, H+6)
 *   - Sign|x|Zahl		(example: x5, -x7, +x6)
 *   - x|Sign|Zahl		(example: x5, x-7, x+6)
 *   - Sign|X|Zahl		(example: X5, -X7, +X6)
 *   - X|Sign|Zahl		(example: X5, X-7, X+6)
 *   - Sign|0x|Zahl		(example: 0x5, -0x7, +0x6)
 *   - 0x|Sign|Zahl		(example: 0x5, 0x-7, 0x+6)
 *   - Sign|0X|Zahl		(example: 0X5, -0X7, +0X6)
 *   - 0X|Sign|Zahl		(example: 0X5, 0X-7, 0X+6)
 *   - Sign|16x|Zahl	(example: 16x5, -16x7, +16x6)
 *   - 16x|Sign|Zahl	(example: 16x5, 16x-7, 16x+6)
 *   - Sign|16X|Zahl	(example: 16X5, -16X7, +16X6)
 *   - 16X|Sign|Zahl	(example: 16X5, 16X-7, 16X+6)
 */

	friend std::istream& operator >> (std::istream& is, bitfield& anyObject);

/**
 * Method computes the a string representation based on the
 * value
 */

	std::string toString(int radix = 2);

/**
 * Method computes the unsigned long representation based on the
 * value. If not possible the value is modulo max UNSIGNED LONG
 */

	unsigned long toULong();

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

};

#endif
