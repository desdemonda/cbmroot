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
//     - derived from errorHandling
//     - class for errors occuring while interacting with the luts
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 14:01:50 $
// $Revision: 1.9 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorError.h"


/****************************************************************
 * CLASS lutGeneratorError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutGeneratorError::lutGeneratorError() : errorHandling(LUTGENERATORLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutGeneratorError::~lutGeneratorError() {

}


/****************************************************************
 * CLASS noHitError								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noHitError::noHitError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noHitError::~noHitError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noHitError::errorMsg() {

	printMsg("There is no hit to use!!!");

}


/****************************************************************
 * CLASS magneticFieldFactorCannotBeZero		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldFactorCannotBeZero::magneticFieldFactorCannotBeZero() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldFactorCannotBeZero::~magneticFieldFactorCannotBeZero() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void magneticFieldFactorCannotBeZero::errorMsg() {

	printMsg("The factor for the magnetic field cannot be set to zero!!!");

}


/****************************************************************
 * CLASS tooBigIndexForDigitalHitLutError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigIndexForDigitalHitLutError::tooBigIndexForDigitalHitLutError() 
  : lutGeneratorError(), 
    index(0),
    numberOfEntries(0)
{
  /*
	index           = 0;
	numberOfEntries = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigIndexForDigitalHitLutError::tooBigIndexForDigitalHitLutError(unsigned long _index, unsigned long _numberOfEntries) 
  : lutGeneratorError(), 
    index(_index),
    numberOfEntries(_numberOfEntries)
{
  /*
	this->index           = _index;
	this->numberOfEntries = _numberOfEntries;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigIndexForDigitalHitLutError::~tooBigIndexForDigitalHitLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigIndexForDigitalHitLutError::errorMsg() {

	std::string temp;
	char buffer[longConversionDigits+1];

	temp  = "Cannot access the digital hit look up table memory at position ";
	ultos(index, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += ", because there are just ";
	ultos(numberOfEntries, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " entries!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tooBigIndexForPrelutError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigIndexForPrelutError::tooBigIndexForPrelutError() 
  : lutGeneratorError(),
    index(0),
    numberOfEntries(0)
{
  /*
	index           = 0;
	numberOfEntries = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigIndexForPrelutError::tooBigIndexForPrelutError(unsigned long _index, unsigned long _numberOfEntries) 
  : lutGeneratorError(), 
    index(_index),
    numberOfEntries(_numberOfEntries)
{

  //	this->index           = _index;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigIndexForPrelutError::~tooBigIndexForPrelutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigIndexForPrelutError::errorMsg() {

	std::string temp;
	char buffer[longConversionDigits+1];

	temp  = "Cannot access the pre look up table memory at position ";
	ultos(index, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += ", because there are just ";
	ultos(numberOfEntries, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " entries!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tooBigIndexForLutError					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigIndexForLutError::tooBigIndexForLutError() 
  : lutGeneratorError(),
    index(0),
    numberOfEntries(0)
{
  /*
	index           = 0;
	numberOfEntries = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigIndexForLutError::tooBigIndexForLutError(unsigned long _index, unsigned long _numberOfEntries) 
  : lutGeneratorError(), 
    index(_index),
    numberOfEntries(_numberOfEntries)
{
  /*
	this->index           = _index;
	this->numberOfEntries = _numberOfEntries;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigIndexForLutError::~tooBigIndexForLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigIndexForLutError::errorMsg() {

	std::string temp;
	char buffer[longConversionDigits+1];

	temp  = "Cannot access the look up table memory at position ";
	ultos(index, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += ", because there are just ";
	ultos(numberOfEntries, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " entries!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotAccessDigitalHitLutError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessDigitalHitLutError::cannotAccessDigitalHitLutError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessDigitalHitLutError::~cannotAccessDigitalHitLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessDigitalHitLutError::errorMsg() {

	printMsg("The digital hit look up table cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessPrelutError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessPrelutError::cannotAccessPrelutError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessPrelutError::~cannotAccessPrelutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessPrelutError::errorMsg() {

	printMsg("The pre look up table cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessLutError					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessLutError::cannotAccessLutError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessLutError::~cannotAccessLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessLutError::errorMsg() {

	printMsg("The look up table cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotGetDefinitionOfUndefinedObjectError 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetDefinitionOfUndefinedObjectError::cannotGetDefinitionOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetDefinitionOfUndefinedObjectError::~cannotGetDefinitionOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetDefinitionOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the definition of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotGetBorderOfUndefinedObjectError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetBorderOfUndefinedObjectError::cannotGetBorderOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetBorderOfUndefinedObjectError::~cannotGetBorderOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetBorderOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the border of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotClearBorderOfUndefinedObjectError 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotClearBorderOfUndefinedObjectError::cannotClearBorderOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotClearBorderOfUndefinedObjectError::~cannotClearBorderOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotClearBorderOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot clear the border of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotSetPrelutRangeOfUndefinedObjectError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetPrelutRangeOfUndefinedObjectError::cannotSetPrelutRangeOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetPrelutRangeOfUndefinedObjectError::~cannotSetPrelutRangeOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotSetPrelutRangeOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot set the prelut range of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotGetMagneticFieldOfUndefinedObjectError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMagneticFieldOfUndefinedObjectError::cannotGetMagneticFieldOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMagneticFieldOfUndefinedObjectError::~cannotGetMagneticFieldOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetMagneticFieldOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the magnetic field of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotGetMagneticFieldFactorOfUndefinedObjectError		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMagneticFieldFactorOfUndefinedObjectError::cannotGetMagneticFieldFactorOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMagneticFieldFactorOfUndefinedObjectError::~cannotGetMagneticFieldFactorOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotGetMagneticFieldFactorOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot get the magnetic field factor of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotSetMagneticFieldOfUndefinedObjectError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetMagneticFieldOfUndefinedObjectError::cannotSetMagneticFieldOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetMagneticFieldOfUndefinedObjectError::~cannotSetMagneticFieldOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotSetMagneticFieldOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot set the magnetic field of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotSetMagneticFieldFactorOfUndefinedObjectError		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetMagneticFieldFactorOfUndefinedObjectError::cannotSetMagneticFieldFactorOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetMagneticFieldFactorOfUndefinedObjectError::~cannotSetMagneticFieldFactorOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotSetMagneticFieldFactorOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot set the magnetic field factor of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotAccessCorrectionsOfUndefinedObjectError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessCorrectionsOfUndefinedObjectError::cannotAccessCorrectionsOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessCorrectionsOfUndefinedObjectError::~cannotAccessCorrectionsOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessCorrectionsOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot access the correction functionality of an undefined object!!!");

}


/****************************************************************
 * CLASS cannotEvaluateBorderOfUndefinedObjectError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotEvaluateBorderOfUndefinedObjectError::cannotEvaluateBorderOfUndefinedObjectError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotEvaluateBorderOfUndefinedObjectError::~cannotEvaluateBorderOfUndefinedObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotEvaluateBorderOfUndefinedObjectError::errorMsg() {

	printMsg("You cannot evaluate the border of an undefined object!!!");

}


/****************************************************************
 * CLASS wrongLutTypeForUsedFunctionalityDetectedError			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

wrongLutTypeForUsedFunctionalityDetectedError::wrongLutTypeForUsedFunctionalityDetectedError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

wrongLutTypeForUsedFunctionalityDetectedError::~wrongLutTypeForUsedFunctionalityDetectedError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void wrongLutTypeForUsedFunctionalityDetectedError::errorMsg() {

	printMsg("There is a wrong LUT type detected for the used functionality!!!");

}


/****************************************************************
 * CLASS cannotAccessRungeKuttaListError						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessRungeKuttaListError::cannotAccessRungeKuttaListError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessRungeKuttaListError::~cannotAccessRungeKuttaListError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessRungeKuttaListError::errorMsg() {

	printMsg("The Runge-Kutta list object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessRungeKuttaInvertedListError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessRungeKuttaInvertedListError::cannotAccessRungeKuttaInvertedListError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessRungeKuttaInvertedListError::~cannotAccessRungeKuttaInvertedListError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessRungeKuttaInvertedListError::errorMsg() {

	printMsg("The Runge-Kutta inverted list object is not accessible!!!");

}


/****************************************************************
 * CLASS noEntryInListError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

wrongEntryInListError::wrongEntryInListError() : lutGeneratorError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

wrongEntryInListError::~wrongEntryInListError() {

}

/****************************************************************
 * This method displays an error message.						*
 ****************************************************************/

void wrongEntryInListError::errorMsg() {

	printMsg("There is a wrong entry in the list!!!");

}
