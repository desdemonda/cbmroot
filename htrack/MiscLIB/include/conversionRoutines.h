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
///   file:
///     - conversion routines
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-10 13:50:20 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _CONVERSIONROUTINES_H
#define _CONVERSIONROUTINES_H


//#define C_COMPATIBILITY	/**< This definition prevents the usage of std::string. Just char* can be used. So it is conform to ansii c */
#undef C_COMPATIBILITY


#ifndef C_COMPATIBILITY
#include <string>
#endif


/**
 * Definitions to calculate the needed char array length for a given number representation.
 * The definition structure must be:
 * ...ConversionDigits (int)(number of char places for BASE * ((number of bits for ...) / BASE) + roundup) + negative sign
 */

#define shortConversionDigits  (int)(3 * (((float)8 * sizeof(short)) / 10) + 1) + 1
#define intConversionDigits    (int)(3 * (((float)8 * sizeof(int)) / 10) + 1) + 1
#define longConversionDigits   (int)(3 * (((float)8 * sizeof(long)) / 10) + 1) + 1
#define doubleExponentBits     11
#define doubleConversionDigits (int)(3 * (((float)8 * sizeof(double) - doubleExponentBits) / 10) + 1) + 1
#define doubleConversionFormat (int)(3 * (((float)doubleExponentBits) / 10) + 1) + 4	/* sign information, decimal point, exponent information and exponent sign information*/
#define doubleConversion       doubleConversionDigits + doubleConversionFormat


/**
 * This method adds a character value infront of a char array.
 * @param value is the character to insert
 * @param string is the old string with enough space for a new character
 * @return string is also the new string
 */

void strinsert(char value, char* string);
#ifndef C_COMPATIBILITY
void strinsert(char value, std::string* string);
#endif

/**
 * This method adds an integer value infront of a char array.
 * @param value is the character to insert
 * @param string is the old string with enough space for a new character
 * @return string is also the new string
 */

void strinsert(unsigned short value, char* string);
#ifndef C_COMPATIBILITY
void strinsert(unsigned short value, std::string* string);
#endif

/**
 * This method converts an unsigned short value into a char.
 */

char ustoc(unsigned short value);

/**
 * This method converts a bool value to a char array. The bool
 * value will be represented by true or false.
 * @param value is the character to insert
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int btobs(bool value, char* string);
#ifndef C_COMPATIBILITY
int btobs(bool value, std::string* string);
#endif

/**
 * This method converts a bool value to a char array. The bool
 * value will be represented by 1 or 0.
 * @param value is the character to insert
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int btods(bool value, char* string);
#ifndef C_COMPATIBILITY
int btods(bool value, std::string* string);
#endif

/**
 * This method converts an unsigned short value into a char
 * array.
 * @param value is the character to insert
 * @param radix is the base of the numbering system for the conversion
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int ustos(unsigned short value, char* string, int radix = 10, int digits = shortConversionDigits);
#ifndef C_COMPATIBILITY
int ustos(unsigned short value, std::string* string, int radix = 10, int digits = 0);
#endif

/**
 * This method converts a short value into a char array.
 * @param value is the character to insert
 * @param radix is the base of the numbering system for the conversion
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int stos(short value, char* string, int radix = 10, int digits = shortConversionDigits);
#ifndef C_COMPATIBILITY
int stos(short value, std::string* string, int radix = 10, int digits = 0);
#endif

/**
 * This method converts an unsigned integer value to a char
 * array by having radix as base for the numerative system.
 * @param value is the character to insert
 * @param radix is the base of the numbering system for the conversion
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int uitos(unsigned int value, char* string, int radix = 10, int digits = intConversionDigits);
#ifndef C_COMPATIBILITY
int uitos(unsigned int value, std::string* string, int radix = 10, int digits = 0);
#endif

/**
 * This method converts an integer value to a char array by
 * having radix as base for the numerative system.
 * @param value is the character to insert
 * @param radix is the base of the numbering system for the conversion
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int itos(int value, char* string, int radix = 10, int digits = intConversionDigits);
#ifndef C_COMPATIBILITY
int itos(int value, std::string* string, int radix = 10, int digits = 0);
#endif

/**
 * This method converts an unsigned long value to a char array
 * by having radix as base for the numerative system.
 * @param value is the character to insert
 * @param radix is the base of the numbering system for the conversion
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int ultos(unsigned long value, char* string, int radix = 10, int digits = longConversionDigits);
#ifndef C_COMPATIBILITY
int ultos(unsigned long value, std::string* string, int radix = 10, int digits = 0);
#endif

/**
 * This method converts a long value to a char array by
 * having radix as base for the numerative system.
 * @param value is the character to insert
 * @param radix is the base of the numbering system for the conversion
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int ltos(long value, char* string, int radix = 10, int digits = longConversionDigits);
#ifndef C_COMPATIBILITY
int ltos(long value, std::string* string, int radix = 10, int digits = 0);
#endif

/**
 * This method converts a double value to a char array.
 * @param value is the character to insert
 * @param digits is the maximal number of digits which should be converted.
 *        Especially for the char* array this is of importance. Here zero
 *        means no restriction. So convert always all numbers.
 * @return string is also the new string
 * @return int is the number of characters or the error code(negative value)
 */

int dtos(double value, char* string, int digits = doubleConversion);
#ifndef C_COMPATIBILITY
int dtos(double value, std::string* string, int digits = 0);
#endif

/**
 * This method converts a char into an unsigned short value.
 */

unsigned short ctous(char value);

/**
 * This method returns the boolean value for the char array.
 */

bool bstob(char* value);
#ifndef C_COMPATIBILITY
bool bstob(std::string& value);
#endif

/**
 * This method returns the boolean value for the char.
 */

bool dstob(char value);
#ifndef C_COMPATIBILITY
bool dstob(std::string& value);
#endif

/**
 * This method converts a char array into an unsigned short
 * value.
 * @param value is the character representation
 * @param radix is the base of the numbering system for the conversion
 * @return the number
 */

unsigned short stous(char* value, int radix = 10);
#ifndef C_COMPATIBILITY
unsigned short stous(std::string& value, int radix = 10);
#endif

/**
 * This method converts a char array into a short value.
 * @param value is the character representation
 * @param radix is the base of the numbering system for the conversion
 * @return the number
 */

short stos(char* value, int radix = 10);
#ifndef C_COMPATIBILITY
short stos(std::string& value, int radix = 10);
#endif

/**
 * This method converts a char array to an unsigned integer
 * value by	having radix as base for the numerative system.
 * @param value is the character representation
 * @param radix is the base of the numbering system for the conversion
 * @return the number
 */

unsigned int stoui(char* value, int radix = 10);
#ifndef C_COMPATIBILITY
unsigned int stoui(std::string& value, int radix = 10);
#endif

/**
 * This method converts a char array to an integer value by
 * having radix as base for the numerative system.
 * @param value is the character representation
 * @param radix is the base of the numbering system for the conversion
 * @return the number
 */

int stoi(char* value, int radix = 10);
#ifndef C_COMPATIBILITY
int stoi(std::string& value, int radix = 10);
#endif

/**
 * This method converts a char array to an unsigned long value
 * by having radix as base for the numerative system.
 * @param value is the character representation
 * @param radix is the base of the numbering system for the conversion
 * @return the number
 */

unsigned long stoul(char* value, int radix = 10);
#ifndef C_COMPATIBILITY
unsigned long stoul(std::string& value, int radix = 10);
#endif

/**
 * This method converts a char array to a long value by
 * having radix as base for the numerative system.
 * @param value is the character representation
 * @param radix is the base of the numbering system for the conversion
 * @return the number
 */

long stol(char* value, int radix = 10);
#ifndef C_COMPATIBILITY
long stol(std::string& value, int radix = 10);
#endif

/**
 * This method converts a double value to a char array.
 * @param value is the character representation
 * @return the number
 */

double stod(char* value);
#ifndef C_COMPATIBILITY
double stod(std::string& value);
#endif

/**
 * This method evaluates the radix of the number in the given
 * char array. The return value is the length of the radix.
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

int extractRadix(int* radix, char* value);
#ifndef C_COMPATIBILITY
int extractRadix(int* radix, std::string* value);
#endif

/**
 * This method adds the radix of the number in the given
 * char array. The return value is the length of the radix.
 * Understanding number conventions:
 *   - (radix)x|Sign|Zahl		(example: 2X5, 3X-7, 9X+6)
 * with radix between 2 and 36
 */

int addRadix(int radix, char* value);
#ifndef C_COMPATIBILITY
int addRadix(int radix, std::string& value);
#endif

#endif
