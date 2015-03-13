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
//     - class for errors occuring while doing some analysis operations
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-09-11 14:07:02 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisError.h"


/****************************************************************
 * CLASS analysisError							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysisError::analysisError() : errorHandling(ANALYSISLIB) {

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

analysisError::~analysisError() {

}


/****************************************************************
 * CLASS windowNotFoundError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

windowNotFoundError::windowNotFoundError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

windowNotFoundError::~windowNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void windowNotFoundError::errorMsg() {

	printMsg("The window is not found!!!");

}


/****************************************************************
 * CLASS fakePassingGradingRError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

fakePassingGradingRError::fakePassingGradingRError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

fakePassingGradingRError::~fakePassingGradingRError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void fakePassingGradingRError::errorMsg() {

	printMsg("A fake passes the gradingR table. You have to change the table so that this situation cannot occur any longer!!!");

}


/****************************************************************
 * CLASS magneticFieldNotFoundError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldNotFoundError::magneticFieldNotFoundError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldNotFoundError::~magneticFieldNotFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void magneticFieldNotFoundError::errorMsg() {

	printMsg("The magnetic field is not found!!!");

}


/****************************************************************
 * CLASS cannotAccessMagnetfieldAnalyserError				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagnetfieldAnalyserError::cannotAccessMagnetfieldAnalyserError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagnetfieldAnalyserError::~cannotAccessMagnetfieldAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagnetfieldAnalyserError::errorMsg() {

	printMsg("The magnetfield analyser object cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessDisplayInfoError					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessDisplayInfoError::cannotAccessDisplayInfoError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessDisplayInfoError::~cannotAccessDisplayInfoError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessDisplayInfoError::errorMsg() {

	printMsg("The display information cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessFindableTracksMemoryError			 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFindableTracksMemoryError::cannotAccessFindableTracksMemoryError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFindableTracksMemoryError::~cannotAccessFindableTracksMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessFindableTracksMemoryError::errorMsg() {

	printMsg("The memory to store the findableTracks cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessFirstAnalysisLutError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFirstAnalysisLutError::cannotAccessFirstAnalysisLutError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFirstAnalysisLutError::~cannotAccessFirstAnalysisLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessFirstAnalysisLutError::errorMsg() {

	printMsg("The first look-up-table for the magnetfield analysis is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessSecondAnalysisLutError				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessSecondAnalysisLutError::cannotAccessSecondAnalysisLutError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessSecondAnalysisLutError::~cannotAccessSecondAnalysisLutError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessSecondAnalysisLutError::errorMsg() {

	printMsg("The second look-up-table for the magnetfield analysis is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessAllMagnetfieldFactorDistributionsError		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessAllMagnetfieldFactorDistributionsError::cannotAccessAllMagnetfieldFactorDistributionsError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessAllMagnetfieldFactorDistributionsError::~cannotAccessAllMagnetfieldFactorDistributionsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessAllMagnetfieldFactorDistributionsError::errorMsg() {

	printMsg("Not all magnetfield factor distributions are accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessMagnetfieldFactorAnalyserError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagnetfieldFactorAnalyserError::cannotAccessMagnetfieldFactorAnalyserError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagnetfieldFactorAnalyserError::~cannotAccessMagnetfieldFactorAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagnetfieldFactorAnalyserError::errorMsg() {

	printMsg("The object for analysing the magnetfield factors is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessPrelutRangeAnalyserError					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessPrelutRangeAnalyserError::cannotAccessPrelutRangeAnalyserError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessPrelutRangeAnalyserError::~cannotAccessPrelutRangeAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessPrelutRangeAnalyserError::errorMsg() {

	printMsg("The object for analysing the prelut range is not accessible!!!");

}


/****************************************************************
 * CLASS indexIsOutOfDisplayRangeError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

indexIsOutOfDisplayRangeError::indexIsOutOfDisplayRangeError() 
  : analysisError(), 
    index(0),
    numberOfDisplays(0)
{
  /*
	this->index            = 0;
	this->numberOfDisplays = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

indexIsOutOfDisplayRangeError::indexIsOutOfDisplayRangeError(unsigned short _index, unsigned short _numberOfDisplays) 
  : analysisError(), 
    index(_index),
    numberOfDisplays(_numberOfDisplays)
{
  /*
  	this->index            = _index;
	this->numberOfDisplays = _numberOfDisplays;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

indexIsOutOfDisplayRangeError::~indexIsOutOfDisplayRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void indexIsOutOfDisplayRangeError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The display with the index ";
	ustos(index, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfDisplays - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS indexIsOutOfFactorRangeError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

indexIsOutOfFactorRangeError::indexIsOutOfFactorRangeError() 
  : analysisError(),
    index(0),
    numberOfFactors(0)
{
  /*
	this->index           = 0;
	this->numberOfFactors = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

indexIsOutOfFactorRangeError::indexIsOutOfFactorRangeError(unsigned short _index, unsigned short _numberOfFactors) 
  : analysisError(), 
    index(_index),
    numberOfFactors(_numberOfFactors)
{
  /*
	this->index           = _index;
	this->numberOfFactors = _numberOfFactors;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

indexIsOutOfFactorRangeError::~indexIsOutOfFactorRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void indexIsOutOfFactorRangeError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The factor with the index ";
	ustos(index, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfFactors - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tooManyActiveStationsError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooManyActiveStationsError::tooManyActiveStationsError() 
  : analysisError(),
    actualNumberOfStations(0),
    maximumNumberOfStations(0)
{
  /*
	this->actualNumberOfStations  = 0;
	this->maximumNumberOfStations = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooManyActiveStationsError::tooManyActiveStationsError(unsigned short _actualNumberOfStations, unsigned short _maximumNumberOfStations) 
  : analysisError(), 
    actualNumberOfStations(_actualNumberOfStations),
    maximumNumberOfStations(_maximumNumberOfStations)
{

  //	this->actualNumberOfStations  = _actualNumberOfStations;
  //	this->maximumNumberOfStations = _maximumNumberOfStations;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooManyActiveStationsError::~tooManyActiveStationsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooManyActiveStationsError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The actual number of detector stations ( ";
	ustos(actualNumberOfStations, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ") exceeds the range of ";
	ustos(maximumNumberOfStations, buffer, shortConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS magneticFieldNotFoundForComparissonToFactorsError 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magneticFieldNotFoundForComparissonToFactorsError::magneticFieldNotFoundForComparissonToFactorsError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magneticFieldNotFoundForComparissonToFactorsError::~magneticFieldNotFoundForComparissonToFactorsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void magneticFieldNotFoundForComparissonToFactorsError::errorMsg() {

	printMsg("The magnetic field is not found to do the comparisson to the constant factors!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackCoordinatesError				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackCoordinatesError::cannotAccessTrackCoordinatesError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackCoordinatesError::~cannotAccessTrackCoordinatesError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackCoordinatesError::errorMsg() {

	printMsg("The track coordinates cannot be accessed!!!");

}


/****************************************************************
 * CLASS indexIsOutOfTrackRangeError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

indexIsOutOfTrackRangeError::indexIsOutOfTrackRangeError() 
  : analysisError(), 
    index(0),
    numberOfTracks(0)
{
  /*
	this->index          = 0;
	this->numberOfTracks = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

indexIsOutOfTrackRangeError::indexIsOutOfTrackRangeError(unsigned int _index, unsigned int _numberOfTracks) 
  : analysisError(),
    index(_index),
    numberOfTracks(_numberOfTracks)
{
  /*
	this->index          = _index;
	this->numberOfTracks = _numberOfTracks;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

indexIsOutOfTrackRangeError::~indexIsOutOfTrackRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void indexIsOutOfTrackRangeError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The track with the index ";
	ustos(index, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfTracks - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS wrongRelativeValueFoundError					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

wrongRelativeValueFoundError::wrongRelativeValueFoundError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

wrongRelativeValueFoundError::~wrongRelativeValueFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void wrongRelativeValueFoundError::errorMsg() {

	printMsg("The actual relative value is wrong!!!");

}


/****************************************************************
 * CLASS wrongConstraintValueFoundError					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

wrongConstraintValueFoundError::wrongConstraintValueFoundError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

wrongConstraintValueFoundError::~wrongConstraintValueFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void wrongConstraintValueFoundError::errorMsg() {

	printMsg("The actual constraint value is wrong!!!");

}


/****************************************************************
 * CLASS initHistogramDimensionsFirstError				 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

initHistogramDimensionsFirstError::initHistogramDimensionsFirstError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

initHistogramDimensionsFirstError::~initHistogramDimensionsFirstError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void initHistogramDimensionsFirstError::errorMsg() {

	printMsg("If using the row or column analysis, the histogram dimensions should be set first with the function initHardwareHistogramDimensions(...)!!!");

}


/****************************************************************
 * CLASS cannotAccessPeakfindingGeometryAnalyserError		 	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessPeakfindingGeometryAnalyserError::cannotAccessPeakfindingGeometryAnalyserError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessPeakfindingGeometryAnalyserError::~cannotAccessPeakfindingGeometryAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessPeakfindingGeometryAnalyserError::errorMsg() {

	printMsg("The peakfinding geometry analyser object cannot be accessed. Maybe it is not initialized!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackPropagationAnalyserError				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackPropagationAnalyserError::cannotAccessTrackPropagationAnalyserError() : analysisError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackPropagationAnalyserError::~cannotAccessTrackPropagationAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackPropagationAnalyserError::errorMsg() {

	printMsg("The track propagation analyser object is not accessible!!!");

}
