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
//   file:
//     - conversion routines
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:50:20 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../include/miscError.h"
#include "../include/miscWarningMsg.h"
#include "../include/conversionRoutines.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef C_COMPATIBILITY
#include <string.h>
#include <sstream>
#endif


#define DECASCIIBASE   48		/**< Defines the decimal startindex in the asciitable for numbers */
#define CHARASCIITABLE 65		/**< Defines the decimal startindex in the asciitable for capital letters */
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

/****************************************************************
 * This method adds a character value infront of a char array.	*
 ****************************************************************/

void strinsert(char value, char* string) {

	char         insert;
	char         temp;
	unsigned int pos;

	insert = value;

	for (pos = 0; pos < strlen(string) + 1; pos++) {
	
		temp        = string[pos];
		string[pos] = insert;
		insert      = temp;

	}

	string[pos] = insert;

}
#ifndef C_COMPATIBILITY
void strinsert(char value, std::string* string) {

	string->insert(0, 1, value);

}
#endif

/****************************************************************
 * This method adds an integer value infront of a char array.	*
 ****************************************************************/

void strinsert(unsigned short value, char* string) {

	strinsert(ustoc(value), string);

}
#ifndef C_COMPATIBILITY
void strinsert(unsigned short value, std::string* string) {

	strinsert(ustoc(value), string);

}
#endif

/****************************************************************
 * This method converts an unsigned short value into a char.	*
 ****************************************************************/

char ustoc(unsigned short value) {

	char returnValue;

	if (value > 35)
		throw ustocValueTooBigError();

	if (value < 10)
		returnValue = (unsigned char)(value + DECASCIIBASE);
	else
		returnValue = (unsigned char)(value - 10 + CHARASCIITABLE);

	return returnValue;

}

/****************************************************************
 * This method converts a bool value to a char array. The bool	*
 * value will be represented by true or false.					*
 ****************************************************************/

int btobs(bool value, char* string) {

	int charCounter = -1;

	if (value) {
		if (string != NULL) {
			strcpy(string, "true");
			charCounter = 4;
		}
	}
	else {
		if (string != NULL) {
			strcpy(string, "false");
			charCounter = 5;
		}
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int btobs(bool value, std::string* string) {

	int charCounter = -1;

	if (value) {
		if (string != NULL) {
			*string = "true";
			charCounter = 4;
		}
	}
	else {
		if (string != NULL) {
			*string = "false";
			charCounter = 5;
		}
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts a bool value to a char array. The bool	*
 * value will be represented by 1 or 0.							*
 ****************************************************************/

int btods(bool value, char* string) {

	int returnValue = -1;

	if (string != NULL) {

		if (value)
			strcpy(string, "1");
		else
			strcpy(string, "0");
		
		returnValue = 1;

	}

	return returnValue;

}
#ifndef C_COMPATIBILITY
int btods(bool value, std::string* string) {

	int returnValue = -1;

	if (string != NULL) {

		if (value)
			*string = "1";
		else
			*string = "0";
		
		returnValue = 1;

	}

	return returnValue;

}
#endif

/****************************************************************
 * This method converts an unsigned short value into a char		*
 * array.														*
 ****************************************************************/

int ustos(unsigned short value, char* string, int radix, int digits) {

	unsigned short temp;
	int            charCounter;

	charCounter = 0;
	if (string != NULL)
		strcpy(string, "");

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				strcpy(string, "0");
		}
		else {
			
			temp = value;

			while (temp != 0) {
			
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

		}

	}
	else {
		charCounter = -1;
		if (string != NULL)
			strcpy(string, "R");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int ustos(unsigned short value, std::string* string, int radix, int digits) {

	unsigned short temp;
	int            charCounter;

	charCounter = 0;
	if (string != NULL)
		string->clear();

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				*string = "0";
		}
		else {
			
			temp = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

		}

	}
	else {
		charCounter = -1;
		if (string != NULL)
			*string = "R";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts a short value into a char array.		*
 ****************************************************************/

int stos(short value, char* string, int radix, int digits) {

	short temp;
	int   charCounter;

	charCounter = 0;
	if (string != NULL)
		strcpy(string, "");

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				strcpy(string, "0");
		}
		else {
			
			if (value < 0)
				temp    = - value;
			else
				temp    = value;

			while (temp != 0) {
			
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

			if (value < 0) {
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
				}
				else {
					charCounter++;
					if (string != NULL)
						strinsert('-', string);
				}
			}

		}
	}
	else {
		charCounter = -1;
		if (string != NULL)
			strcpy(string, "R");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int stos(short value, std::string* string, int radix, int digits) {

	short temp;
	int   charCounter;

	charCounter = 0;
	if (string != NULL)
		string->clear();

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				*string = "0";
		}
		else {
			
			if (value < 0)
				temp    = - value;
			else
				temp    = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

			if (value < 0) {
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
				}
				else {
					charCounter++;
					if (string != NULL)
						strinsert('-', string);
				}
			}

		}
	}
	else {
		charCounter = -1;
		if (string != NULL)
			*string = "R";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts an unsigned integer value to a char		*
 * array by having radix as base for the numerative system.		*
 ****************************************************************/

int uitos(unsigned int value, char* string, int radix, int digits) {

	unsigned int temp;
	int          charCounter;

	charCounter = 0;
	if (string != NULL)
		strcpy(string, "");

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				strcpy(string, "0");
		}
		else {
			
			temp = value;

			while (temp != 0) {
			
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

		}

	}
	else {
		charCounter = -1;
		if (string != NULL)
			strcpy(string, "R");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int uitos(unsigned int value, std::string* string, int radix, int digits) {

	unsigned int temp;
	int          charCounter;

	charCounter = 0;
	if (string != NULL)
		string->clear();

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				*string = "0";
		}
		else {
			
			temp = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

		}

	}
	else {
		charCounter = -1;
		if (string != NULL)
			*string = "R";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts an integer value to a char array by		*
 * having radix as base for the numerative system.				*
 ****************************************************************/

int itos(int value, char* string, int radix, int digits) {

	int temp;
	int charCounter;

	charCounter = 0;
	if (string != NULL)
		strcpy(string, "");

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				strcpy(string, "0");
		}
		else {
			
			if (value < 0)
				temp    = - value;
			else
				temp    = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

			if (value < 0) {
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
				}
				else {
					charCounter++;
					if (string != NULL)
						strinsert('-', string);
				}
			}

		}

	}
	else {
		charCounter = -1;
		if (string != NULL)
			strcpy(string, "R");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int itos(int value, std::string* string, int radix, int digits) {

	int temp;
	int charCounter;

	charCounter = 0;
	if (string != NULL)
		string->clear();

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				*string = "0";
		}
		else {
			
			if (value < 0)
				temp    = - value;
			else
				temp    = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

			if (value < 0) {
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
				}
				else {
					charCounter++;
					if (string != NULL)
						strinsert('-', string);
				}
			}

		}

	}
	else {
		charCounter = -1;
		if (string != NULL)
			*string = "R";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts a long value to a char array by			*
 * having radix as base for the numerative system.				*
 ****************************************************************/

int ultos(unsigned long value, char* string, int radix, int digits) {

	unsigned long  temp;
	int            charCounter;

	charCounter = 0;
	if (string != NULL)
		strcpy(string, "");

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				strcpy(string, "0");
		}
		else {
			
			temp = value;

			while (temp != 0) {
			
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

		}
	}
	else {
		charCounter = -1;
		if (string != NULL)
			strcpy(string, "R");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int ultos(unsigned long value, std::string* string, int radix, int digits) {

	unsigned long  temp;
	int            charCounter;

	charCounter = 0;
	if (string != NULL)
		string->clear();

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				*string = "0";
		}
		else {
			
			temp = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

		}
	}
	else {
		charCounter = -1;
		if (string != NULL)
			*string = "R";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts a long value to a char array by			*
 * having radix as base for the numerative system.				*
 ****************************************************************/

int ltos(long value, char* string, int radix, int digits) {

	long temp;
	int  charCounter;

	charCounter = 0;
	if (string != NULL)
		strcpy(string, "");

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				strcpy(string, "0");
		}
		else {
			
			if (value < 0)
				temp    = - value;
			else
				temp    = value;

			while (temp != 0) {
			
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

			if (value < 0) {
				if (charCounter == digits) {
					charCounter = -1;
					if (string != NULL)
						strcpy(string, "D");
				}
				else {
					charCounter++;
					if (string != NULL)
						strinsert('-', string);
				}
			}

		}
	}
	else {
		charCounter = -1;
		if (string != NULL)
			strcpy(string, "R");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int ltos(long value, std::string* string, int radix, int digits) {

	long temp;
	int  charCounter;

	charCounter = 0;
	if (string != NULL)
		string->clear();

	if ((radix > 1) && (radix < 37)) {

		if (value == 0) {
			charCounter = 1;
			if (string != NULL)
				*string = "0";
		}
		else {
			
			if (value < 0)
				temp    = - value;
			else
				temp    = value;

			while (temp != 0) {
			
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
					break;
				}
				charCounter++;
				if (string != NULL)
					strinsert((unsigned short)(temp % radix), string);
				temp /= radix;

			}

			if (value < 0) {
				if ((charCounter == digits) && (digits > 0)) {
					charCounter = -1;
					if (string != NULL)
						*string = "D";
				}
				else {
					charCounter++;
					if (string != NULL)
						strinsert('-', string);
				}
			}

		}
	}
	else {
		charCounter = -1;
		if (string != NULL)
			*string = "R";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts a double value to a char array.			*
 ****************************************************************/

int dtos(double value, char* string, int digits) {

	int charCounter;

	charCounter = sprintf(string, "%f", value);

	if (charCounter > digits) {
		charCounter = -1;
		strcpy(string, "D");
	}

	return charCounter;

}
#ifndef C_COMPATIBILITY
int dtos(double value, std::string* string, int digits) {

	int charCounter;
	std::ostringstream tempString;

	if (tempString << value) {

		*string     = tempString.str();
		charCounter = (int)string->length();

		if (digits > 0) {

			if (charCounter > digits) {
				charCounter = -1;
				*string     = "D";
			}

		}

	}
	else {
		charCounter = -1;
		*string     = "D";
	}

	return charCounter;

}
#endif

/****************************************************************
 * This method converts a char into an unsigned short value.	*
 ****************************************************************/

unsigned short ctous(char value) {

	unsigned short returnValue;
	unsigned short valueUS;

	valueUS = (unsigned short)toupper(value);

	if ((valueUS >= DECASCIIBASE) && (valueUS < DECASCIIBASE + 10))
		returnValue = valueUS - DECASCIIBASE;
	else if ((valueUS >= CHARASCIITABLE) && (valueUS < CHARASCIITABLE + 26))
		returnValue = valueUS - CHARASCIITABLE + 10;
	else
		throw ctousValueTooBigError();

	return returnValue;

}

/****************************************************************
 * This method returns the boolean value for the char array.	*
 ****************************************************************/

bool bstob(char* value) {

	int temp[]      = {'t', 'r', 'u', 'e'};
	bool returnValue = true;

	if (strlen(value) != 4)
		returnValue = false;
	else {
		for (unsigned int i = 0; i < strlen(value); i++)
			if (temp[i] != tolower(value[i]))
				returnValue = false;
	}

	return returnValue;

}
#ifndef C_COMPATIBILITY
bool bstob(std::string& value) {

	std::string temp = "true";
	bool returnValue = true;

	if (value.length() != temp.length())
		returnValue = false;
	else {
		for (unsigned int i = 0; i < value.length(); i++)
			if (temp.c_str()[i] != tolower(value.c_str()[i]))
				returnValue = false;
	}

	return returnValue;

}
#endif

/****************************************************************
 * This method returns the boolean value for the char.			*
 ****************************************************************/

bool dstob(char value) {

	bool returnValue;

	if (value == '1')
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}
#ifndef C_COMPATIBILITY
bool dstob(std::string& value) {

	bool returnValue;

	if (value == "1")
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a char array into an unsigned short		*
 * value.														*
 ****************************************************************/

unsigned short stous(char* value, int radix) {

	unsigned short stringIndex;
	unsigned short radixCheck;
	int            stringLength;
	bool           negativeValue;
	unsigned short returnValue;

	stringIndex   = 0;
	stringLength  = (int)strlen(value);
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0) && (value[stringIndex] != '-')) {

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (unsigned short)ctous(value[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

	}
	else
		returnValue = (unsigned short) -1;

	return returnValue;

}
#ifndef C_COMPATIBILITY
unsigned short stous(std::string& value, int radix) {

	unsigned short stringIndex;
	unsigned short radixCheck;
	int            stringLength;
	bool           negativeValue;
	unsigned short returnValue;

	stringIndex   = 0;
	stringLength  = (int)value.length();
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0) && (value.c_str()[stringIndex] != '-')) {

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (unsigned short)ctous(value.c_str()[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

	}
	else
		returnValue = (unsigned short) -1;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a char array into a short value.		*
 ****************************************************************/

short stos(char* value, int radix) {

	unsigned short stringIndex;
	short          radixCheck;
	int            stringLength;
	bool           negativeValue;
	short          returnValue;

	stringIndex   = 0;
	stringLength  = (int)strlen(value);
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0)) {

		if (value[stringIndex] == '-') {
			stringIndex++;
			negativeValue = true;
		}

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (short)ctous(value[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

		if (negativeValue)
			returnValue = - returnValue;

	}
	else
		returnValue = -1;

	return returnValue;

}
#ifndef C_COMPATIBILITY
short stos(std::string& value, int radix) {

	unsigned short stringIndex;
	short          radixCheck;
	int            stringLength;
	bool           negativeValue;
	short          returnValue;

	stringIndex   = 0;
	stringLength  = (int)value.length();
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0)) {

		if (value.c_str()[stringIndex] == '-') {
			stringIndex++;
			negativeValue = true;
		}

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (short)ctous(value.c_str()[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

		if (negativeValue)
			returnValue = - returnValue;

	}
	else
		returnValue = -1;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a char array to an unsigned integer		*
 * value by	having radix as base for the numerative system.		*
 ****************************************************************/

unsigned int stoui(char* value, int radix) {

	unsigned short stringIndex;
	unsigned int   radixCheck;
	int            stringLength;
	bool           negativeValue;
	unsigned int   returnValue;

	stringIndex   = 0;
	stringLength  = (int)strlen(value);
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0) && (value[stringIndex] != '-')) {

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (unsigned int)ctous(value[stringIndex]);
			if (radixCheck < (unsigned int)radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

	}
	else
		returnValue = (unsigned int) -1;

	return returnValue;

}
#ifndef C_COMPATIBILITY
unsigned int stoui(std::string value, int radix) {

	unsigned short stringIndex;
	unsigned int   radixCheck;
	int            stringLength;
	bool           negativeValue;
	unsigned int   returnValue;

	stringIndex   = 0;
	stringLength  = (int)value.length();
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0) && (value.c_str()[stringIndex] != '-')) {

		for (; stringIndex < stringLength; stringIndex++) {

			radixCheck   = (unsigned int)ctous(value.c_str()[stringIndex]);
			if (radixCheck < (unsigned int)radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

	}
	else
		returnValue = (unsigned int) -1;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a char array to an integer value by		*
 * having radix as base for the numerative system.				*
 ****************************************************************/

int stoi(char* value, int radix) {

	unsigned short stringIndex;
	int            radixCheck;
	int            stringLength;
	bool           negativeValue;
	int            returnValue;

	stringIndex   = 0;
	stringLength  = (int)strlen(value);
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0)) {

		if (value[stringIndex] == '-') {
			stringIndex++;
			negativeValue = true;
		}

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (int)ctous(value[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

		if (negativeValue)
			returnValue = - returnValue;

	}
	else
		returnValue = -1;

	return returnValue;

}
#ifndef C_COMPATIBILITY
int stoi(std::string& value, int radix) {

	unsigned short stringIndex;
	int            radixCheck;
	int            stringLength;
	bool           negativeValue;
	int            returnValue;

	stringIndex   = 0;
	stringLength  = (int)value.length();
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0)) {

		if (value.c_str()[stringIndex] == '-') {
			stringIndex++;
			negativeValue = true;
		}

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (int)ctous(value.c_str()[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

		if (negativeValue)
			returnValue = - returnValue;

	}
	else
		returnValue = -1;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a char array to an unsigned long value	*
 * by having radix as base for the numerative system.			*
 ****************************************************************/

unsigned long stoul(char* value, int radix) {

	unsigned short stringIndex;
	unsigned long  radixCheck;
	int            stringLength;
	bool           negativeValue;
	unsigned long  returnValue;

	stringIndex   = 0;
	stringLength  = (int)strlen(value);
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0) && (value[stringIndex] != '-')) {

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (unsigned long)ctous(value[stringIndex]);
			if (radixCheck < (unsigned long)radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

	}
	else
		returnValue = (unsigned long) -1;

	return returnValue;

}
#ifndef C_COMPATIBILITY
unsigned long stoul(std::string& value, int radix) {

	unsigned short stringIndex;
	unsigned long  radixCheck;
	int            stringLength;
	bool           negativeValue;
	unsigned long  returnValue;

	stringIndex   = 0;
	stringLength  = (int)value.length();
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0) && (value.c_str()[stringIndex] != '-')) {

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (unsigned long)ctous(value.c_str()[stringIndex]);
			if (radixCheck < (unsigned long)radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

	}
	else
		returnValue = (unsigned long) -1;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a char array to a long value by			*
 * having radix as base for the numerative system.				*
 ****************************************************************/

long stol(char* value, int radix) {

	unsigned short stringIndex;
	long           radixCheck;
	int            stringLength;
	bool           negativeValue;
	long           returnValue;

	stringIndex   = 0;
	stringLength  = (int)strlen(value);
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0)) {

		if (value[stringIndex] == '-') {
			stringIndex++;
			negativeValue = true;
		}

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (long)ctous(value[stringIndex]);
			if (radixCheck < (long)radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

		if (negativeValue)
			returnValue = - returnValue;

	}
	else
		returnValue = -1;

	return returnValue;

}
#ifndef C_COMPATIBILITY
long stol(std::string& value, int radix) {

	unsigned short stringIndex;
	long           radixCheck;
	int            stringLength;
	bool           negativeValue;
	long           returnValue;

	stringIndex   = 0;
	stringLength  = (int)value.length();
	negativeValue = false;
	returnValue   = 0;

	if ((radix > 1) && (radix < 37) && (stringLength > 0)) {

		if (value.c_str()[stringIndex] == '-') {
			stringIndex++;
			negativeValue = true;
		}

		for (; stringIndex < stringLength; stringIndex++) {
			
			radixCheck   = (long)ctous(value.c_str()[stringIndex]);
			if (radixCheck < radix) {
				returnValue *= radix;
				returnValue += radixCheck;
			}
			else {
				valueBiggerThanRadixWarningMsg* valueBiggerThanRadix = new valueBiggerThanRadixWarningMsg(radixCheck, radix);
				valueBiggerThanRadix->warningMsg();
				if(valueBiggerThanRadix != NULL) {
					delete valueBiggerThanRadix;
					valueBiggerThanRadix = NULL;
				}
			}

		}

		if (negativeValue)
			returnValue = - returnValue;

	}
	else
		returnValue = -1;

	return returnValue;

}
#endif

/****************************************************************
 * This method converts a double value to a char array.			*
 ****************************************************************/

double stod(char* value) {

	return atof(value);

}
#ifndef C_COMPATIBILITY
double stod(std::string& value) {

	return atof(value.c_str());

}
#endif

/****************************************************************
 * This method evaluates the radix of the number in the given	*
 * char array.													*
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

int extractRadix(int* radix, char* value) {

	int   i;
	int   signPos;
	char  swapspace;
	int   xPos;
	char* radixPointer;
	char  radixBuffer[3];
	int   returnValue;

	signPos = -1;
	for (i = 0; i < (int)strlen(value); i++) {

		if ((value[i] == '-') || (value[i] == '+')) {

			signPos = i;
			break;

		}

	}

	for (i = signPos; i > 0; i--) {

		swapspace  = value[i];
		value[i]   = value[i-1];
		value[i-1] = swapspace;

	}

	xPos = -1;
	for (i = 0; i < (int)strlen(value); i++) {

		if (tolower(value[i]) == 'x') {
		
			xPos = i;
			break;

		}

	}

	if (signPos == -1)
		radixPointer = &value[0];
	else
		radixPointer = &value[1];

	if (((xPos > 0) && (signPos == -1)) || ((xPos > 1) && (signPos != -1))) {

		radixBuffer[0] = *radixPointer;
		*radixPointer  = '0';
		if (signPos == -1) {
			if (xPos != 1) {
				radixBuffer[1] = value[1];
				value[1]       = '0';
			}
			else
				radixBuffer[1] = '\0';
		}
		else {
			if (xPos != 2) {
				radixBuffer[1] = value[2];
				value[2]       = '0';
			}
			else
				radixBuffer[1] = '\0';
		}
		radixBuffer[2] = '\0';
		value[xPos]    = '0';

		if (radix != NULL) {
			*radix = stoi(radixBuffer, 10);
			if (*radix == 0) /* hex standard */
				*radix = 16;
		}

		returnValue = xPos + 1;

	}
	else {

		returnValue = 1;

		switch (tolower(*radixPointer)) {

			case 'b':
				if (radix != NULL)
					*radix    = 2;
				*radixPointer = '0';
				break;

			case 'o':
			case '0':
				if (radix != NULL)
					*radix    = 8;
				*radixPointer = '0';
				break;

			case 'd':
			case 'x':
				if (radix != NULL)
					*radix    = 10;
				*radixPointer = '0';
				break;

			case 'h':
				if (radix != NULL)
					*radix    = 16;
				*radixPointer = '0';
				break;

			default:
				if (radix != NULL)
					*radix    = 10;
				returnValue = 0;
				break;

		}

	}

	return returnValue;

}
#ifndef C_COMPATIBILITY
int extractRadix(int* radix, std::string* value) {

	std::string::iterator signPos;
	char                  swapspace;
	std::string::iterator xPos;
	std::string::iterator radixPointer;
	char                  radixBuffer[3];
	int                   returnValue;

	signPos = value->end();
	for (std::string::iterator i = value->begin(); i != value->end(); i++) {

		if ((*i == '-') || (*i == '+')) {
		
			signPos = i;
			break;

		}

	}

	if (signPos != value->end()) {

		for (std::string::iterator j = signPos; j != value->begin(); j--) {

			swapspace  = *j;
			*j         = *(j-1);
			*(j-1)     = swapspace;

		}

	}

	xPos = value->end();
	for (std::string::iterator k = value->begin(); k != value->end(); k++) {

		if (tolower(*k) == 'x') {
		
			xPos = k;
			break;

		}

	}


	if (signPos == value->end())
		radixPointer = value->begin();
	else
		radixPointer = value->begin() + 1;

	if (((xPos > value->begin()) && (xPos != value->end()) && (signPos == value->end())) || ((xPos > value->begin() + 1) && (xPos != value->end()) && (signPos != value->end()))) {

		radixBuffer[0] = *radixPointer;
		*radixPointer  = '0';
		if (signPos == value->end()) {
			if (xPos != value->begin() + 1) {
				radixBuffer[1]        = *(value->begin() + 1);
				*(value->begin() + 1) = '0';
			}
			else
				radixBuffer[1]        = '\0';
		}
		else {
			if (xPos != value->begin() + 2) {
				radixBuffer[1]        = *(value->begin() + 2);
				*(value->begin() + 2) = '0';
			}
			else
				radixBuffer[1]        = '\0';
		}
		radixBuffer[2] = '\0';
		*xPos          = '0';

		if (radix != NULL) {
			*radix = stoi(radixBuffer, 10);
			if (*radix == 0) /* hex standard */
				*radix = 16;
		}

		returnValue = (int)strlen(radixBuffer) + 1;

	}
	else {

		returnValue = 1;

		switch (tolower(*radixPointer)) {

			case 'b':
				if (radix != NULL)
					*radix    = 2;
				*radixPointer = '0';
				break;

			case 'o':
			case '0':
				if (radix != NULL)
					*radix    = 8;
				*radixPointer = '0';
				break;

			case 'd':
			case 'x':
				if (radix != NULL)
					*radix    = 10;
				*radixPointer = '0';
				break;

			case 'h':
				if (radix != NULL)
					*radix    = 16;
				*radixPointer = '0';
				break;

			default:
				if (radix != NULL)
					*radix    = 10;
				returnValue   = 0;
				break;

		}

	}

	return returnValue;

}
#endif

/****************************************************************
 * This method adds the radix of the number in the given		*
 * char array. The return value is the length of the radix.		*
 * Understanding number conventions:							*
 *   - (radix)x|Sign|Zahl		(example: 2X5, 3X-7, 9X+6)		*
 * with radix between 2 and 36									*
 ****************************************************************/

int addRadix(int radix, char* value) {

	int counts = -1;

	if ((radix > 1) && (radix < 37)) {

		counts = itos(radix, value, 10, 2);
		value[counts] = 'x';

	}

	return counts + 1;

}
#ifndef C_COMPATIBILITY
int addRadix(int radix, std::string& value) {

	int counts;

	char buffer[4];

	counts = addRadix(radix, buffer);

	buffer[counts] = '\0';

	value.replace(0, counts, buffer, counts);

	return counts;

}
#endif
