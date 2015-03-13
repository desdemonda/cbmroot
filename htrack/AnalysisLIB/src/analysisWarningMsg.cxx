/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
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
//     - base class for warnings occuring during analysis
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-09-11 14:07:02 $
// $Revision: 1.25 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../include/analysisWarningMsg.h"
#include <iostream>


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * CLASS analysisWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysisWarningMsg::analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analysisWarningMsg::~analysisWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void analysisWarningMsg::printMsg(std::string message) {

#ifndef NOANALYSISWARNINGMESSAGE

	std::cerr << std::endl;
	
	std::cerr << ANALYSISLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}

/****************************************************************
 * CLASS trackNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackNotFoundWarningMsg::trackNotFoundWarningMsg() 
  : analysisWarningMsg(),
    track(NULL)
{

  //	track = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackNotFoundWarningMsg::trackNotFoundWarningMsg(trackfinderInputTrack* actualTrack) 
  : analysisWarningMsg(),
    track(actualTrack)
{

  //	track = actualTrack;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackNotFoundWarningMsg::~trackNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackNotFoundWarningMsg::warningMsg() {

	std::string temp;
	char        intBuffer[intConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];

	temp = "The following track is not found:\n";
	temp += " TrackId: ";
	itos(track->getTrackIndex(), intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += "\n";
	temp += "  PdgCode: ";
	itos(track->getPdgCode(), intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += "\n";
	temp += "  Charge: ";
	dtos(track->getCharge(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "\n";
	temp += "  momemtumX: ";
	dtos(track->getMomX(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "\n";
	temp += "  momentumY: ";
	dtos(track->getMomY(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "\n";
	temp += "  momentumZ: ";
	dtos(track->getMomZ(), doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	printMsg(temp);

}

/****************************************************************
 * CLASS displayAnalyserNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayAnalyserNotFoundWarningMsg::displayAnalyserNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayAnalyserNotFoundWarningMsg::~displayAnalyserNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayAnalyserNotFoundWarningMsg::warningMsg() {

	printMsg("The display analyser is not found!!!\nCheck the init()-functions for correctness.\nCaution: The ...ToRoot()-function must be the last init()-function for displays.");

}


/****************************************************************
 * CLASS hardwareAnalyserNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hardwareAnalyserNotFoundWarningMsg::hardwareAnalyserNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hardwareAnalyserNotFoundWarningMsg::~hardwareAnalyserNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hardwareAnalyserNotFoundWarningMsg::warningMsg() {

	printMsg("The hardware analyser is not found!!!\nCheck the init()-functions for correctness.");

}


/****************************************************************
 * CLASS displayCannotBeWrittenToFileWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayCannotBeWrittenToFileWarningMsg::displayCannotBeWrittenToFileWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayCannotBeWrittenToFileWarningMsg::~displayCannotBeWrittenToFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayCannotBeWrittenToFileWarningMsg::warningMsg() {

	printMsg("A display cannot be written to file!!!");

}


/****************************************************************
 * CLASS displayNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayNotFoundWarningMsg::displayNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayNotFoundWarningMsg::~displayNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void displayNotFoundWarningMsg::warningMsg() {

	printMsg("A display is not found during writing to file!!!");

}


/****************************************************************
 * CLASS cannotCreateSignatureWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotCreateSignatureWarningMsg::cannotCreateSignatureWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotCreateSignatureWarningMsg::~cannotCreateSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotCreateSignatureWarningMsg::warningMsg() {

	printMsg("The signature for the enabled displays cannot be created!!!");

}


/****************************************************************
 * CLASS cannotWriteWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteWarningMsg::cannotWriteWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteWarningMsg::~cannotWriteWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotWriteWarningMsg::warningMsg() {

	printMsg("The display can not be written!!!");

}


/****************************************************************
 * CLASS notAllDisplaysCanBeDrawnWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

notAllDisplaysCanBeDrawnWarningMsg::notAllDisplaysCanBeDrawnWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

notAllDisplaysCanBeDrawnWarningMsg::~notAllDisplaysCanBeDrawnWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void notAllDisplaysCanBeDrawnWarningMsg::warningMsg() {

	printMsg("There is not enough space to draw all displays!!!");

}


/****************************************************************
 * CLASS cannotAccessParameterWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessParameterWarningMsg::cannotAccessParameterWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessParameterWarningMsg::~cannotAccessParameterWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotAccessParameterWarningMsg::warningMsg() {

	printMsg("The parameter for the display entries can not be accessed!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackWarningMsg::cannotAccessTrackWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackWarningMsg::~cannotAccessTrackWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotAccessTrackWarningMsg::warningMsg() {

	printMsg("The track for the display entries can not be accessed!!!");

}


/****************************************************************
 * CLASS cannotGetDisplayEntriesWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetDisplayEntriesWarningMsg::cannotGetDisplayEntriesWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetDisplayEntriesWarningMsg::~cannotGetDisplayEntriesWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotGetDisplayEntriesWarningMsg::warningMsg() {

	printMsg("The display entries can not be accessed!!!");

}


/****************************************************************
 * CLASS noHitInDetectorStationWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noHitInDetectorStationWarningMsg::noHitInDetectorStationWarningMsg() 
  : analysisWarningMsg(),
    index((unsigned short)-1)
{

  //	index = (unsigned short)-1;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noHitInDetectorStationWarningMsg::noHitInDetectorStationWarningMsg(unsigned short actualIndex) 
  : analysisWarningMsg(),
    index(actualIndex)
{

  //	index = actualIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noHitInDetectorStationWarningMsg::~noHitInDetectorStationWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void noHitInDetectorStationWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "There is no hit found in the station with the index ";
	ustos(index, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " of the detector!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS noAnalysisOutputFileNameSpecifiedWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noAnalysisOutputFileNameSpecifiedWarningMsg::noAnalysisOutputFileNameSpecifiedWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noAnalysisOutputFileNameSpecifiedWarningMsg::~noAnalysisOutputFileNameSpecifiedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noAnalysisOutputFileNameSpecifiedWarningMsg::warningMsg() {

	printMsg("The name of the output file for the analyser is not specified!!!");

}


/****************************************************************
 * CLASS noAnalysisProjectionOutputFileNameSpecifiedWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noAnalysisProjectionOutputFileNameSpecifiedWarningMsg::noAnalysisProjectionOutputFileNameSpecifiedWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noAnalysisProjectionOutputFileNameSpecifiedWarningMsg::~noAnalysisProjectionOutputFileNameSpecifiedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noAnalysisProjectionOutputFileNameSpecifiedWarningMsg::warningMsg() {

	printMsg("The name of the output file for the projection analyser is not specified!!!");

}


/****************************************************************
 * CLASS enableJustOneDisplayWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

enableJustOneDisplayWarningMsg::enableJustOneDisplayWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

enableJustOneDisplayWarningMsg::~enableJustOneDisplayWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void enableJustOneDisplayWarningMsg::warningMsg() {

	printMsg("You actually enable a second display. So the previous one is disabled!!!");

}


/****************************************************************
 * CLASS cannotSetPointWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotSetPointWarningMsg::cannotSetPointWarningMsg() 
  : analysisWarningMsg(),
    index(0)
{

  //	index = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotSetPointWarningMsg::cannotSetPointWarningMsg(int _index) 
  : analysisWarningMsg(),
    index(_index)
{

  //	this->index = _index;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotSetPointWarningMsg::~cannotSetPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotSetPointWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp =  "The values for the point with index ";
	itos(index, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " cannot be set.\nMaybe the display is not enabled or the index is out of bounds!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS positionIsOutOfSpaceWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

positionIsOutOfSpaceWarningMsg::positionIsOutOfSpaceWarningMsg() 
  : analysisWarningMsg(),
    space(NULL),
    coordinates(NULL)
{

  //	space       = NULL;
  //	coordinates = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

positionIsOutOfSpaceWarningMsg::positionIsOutOfSpaceWarningMsg(histogramSpace* _space, trackCoordinates* _coordinates) 
  : analysisWarningMsg(),
    space(_space),
    coordinates(_coordinates)
{

  //	this->space       = _space;
  //	this->coordinates = _coordinates;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

positionIsOutOfSpaceWarningMsg::~positionIsOutOfSpaceWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void positionIsOutOfSpaceWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp =  "The position";
	if (coordinates != NULL) {
		temp += " (";
		itos(coordinates->get(DIM1), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(coordinates->get(DIM2), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(coordinates->get(DIM3), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += ")";
	}
	temp += " is not in the specified histogram maximal range";
	if (space != NULL) {
		temp += " of (";
		itos(space->getStep(DIM1), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(space->getStep(DIM2), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " ,";
		itos(space->getStep(DIM3), buffer, 10, intConversionDigits);
		temp += buffer;
		temp += ")";
	}
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfWrongThirdDimensionBordersFoundWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfWrongThirdDimensionBordersFoundWarningMsg::numberOfWrongThirdDimensionBordersFoundWarningMsg() 
  : analysisWarningMsg(),
    number(0)
{

  //	number = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfWrongThirdDimensionBordersFoundWarningMsg::numberOfWrongThirdDimensionBordersFoundWarningMsg(int _number) 
  : analysisWarningMsg(),
    number(_number)
{

  //	this->number = _number;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfWrongThirdDimensionBordersFoundWarningMsg::~numberOfWrongThirdDimensionBordersFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void numberOfWrongThirdDimensionBordersFoundWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[intConversionDigits+1];

	temp =  "There are ";
	itos(number, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " hits found with the correct 2D entry out of range of the prelut border!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS setMagnetfieldFactorWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

setMagnetfieldFactorWarningMsg::setMagnetfieldFactorWarningMsg() 
  : analysisWarningMsg(),
    stationIndex(0),
    magnetfieldFactor(0)
{

  //	stationIndex      = 0;
  //	magnetfieldFactor = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

setMagnetfieldFactorWarningMsg::setMagnetfieldFactorWarningMsg(unsigned short _stationIndex, double _magnetfieldFactor) 
  : analysisWarningMsg(),
    stationIndex(_stationIndex),
    magnetfieldFactor(_magnetfieldFactor)
{

  //	this->stationIndex      = _stationIndex;
  //	this->magnetfieldFactor = _magnetfieldFactor;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

setMagnetfieldFactorWarningMsg::~setMagnetfieldFactorWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void setMagnetfieldFactorWarningMsg::warningMsg() {

	std::string temp;
	char        shortBuffer[shortConversionDigits+1];
	char        doubleBuffer[doubleConversion+1];

	temp =  "The magnetfieldFactor of the station with the index ";
	ustos(stationIndex, shortBuffer, 10, shortConversionDigits);
	temp += shortBuffer;
	temp += " is set to ";
	dtos(magnetfieldFactor, doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS setPrelutRangeWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

setPrelutRangeWarningMsg::setPrelutRangeWarningMsg() 
  : analysisWarningMsg(),
    rangeStart(0),
    rangeStop(0)
{

  //	rangeStart = 0;
  //	rangeStop  = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

setPrelutRangeWarningMsg::setPrelutRangeWarningMsg(double _rangeStart, double _rangeStop) 
  : analysisWarningMsg(),
    rangeStart(_rangeStart),
    rangeStop(_rangeStop)
{

  //	this->rangeStart = _rangeStart;
  //	this->rangeStop  = _rangeStop;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

setPrelutRangeWarningMsg::~setPrelutRangeWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void setPrelutRangeWarningMsg::warningMsg() {

	std::string temp;
	char        doubleBuffer[doubleConversion+1];

	temp =  "The prelut range is actually set to [";
	dtos(rangeStart, doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += ", ";
	dtos(rangeStop, doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS analysisLutCorrectionWarningMsg			 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysisLutCorrectionWarningMsg::analysisLutCorrectionWarningMsg() 
  : analysisWarningMsg(),
    borderCorrections(0),
    borderPosCorrections(0)
{

  //	borderCorrections    = 0;
  //	borderPosCorrections = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

analysisLutCorrectionWarningMsg::analysisLutCorrectionWarningMsg(unsigned long corrections, unsigned long posCorrections) 
  : analysisWarningMsg(),
    borderCorrections(corrections),
    borderPosCorrections(posCorrections)
{

  //	borderCorrections    = corrections;
  //	borderPosCorrections = posCorrections;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analysisLutCorrectionWarningMsg::~analysisLutCorrectionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void analysisLutCorrectionWarningMsg::warningMsg() {

	std::string temp;
	char buffer[longConversionDigits+1];

	temp = "There are ";
	ultos(borderPosCorrections, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " corrections which are done while lut calculation.\n";
	ultos(borderCorrections, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " different IDs are affected\nSo in the middle there are ";
	ultos(((unsigned long)(borderPosCorrections/borderCorrections+0.5)), buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " positions corrected per ID!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS rootHistogramNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

rootHistogramNotFoundWarningMsg::rootHistogramNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

rootHistogramNotFoundWarningMsg::~rootHistogramNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void rootHistogramNotFoundWarningMsg::warningMsg() {

	printMsg("A root histogram to draw a display is not found!!!");

}


/****************************************************************
 * CLASS cannotUseToRootBeforeInitializing		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotUseToRootBeforeInitializingWarningMsg::cannotUseToRootBeforeInitializingWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotUseToRootBeforeInitializingWarningMsg::~cannotUseToRootBeforeInitializingWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotUseToRootBeforeInitializingWarningMsg::warningMsg() {

	printMsg("You cannot use ...ToRoot before initializing the analysis!!!");

}


/****************************************************************
 * CLASS trackWithHitsInCorrectLayerDistributionWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInCorrectLayerDistributionWarningMsg::trackWithHitsInCorrectLayerDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInCorrectLayerDistributionWarningMsg::trackWithHitsInCorrectLayerDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInCorrectLayerDistributionWarningMsg::~trackWithHitsInCorrectLayerDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInCorrectLayerDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the percentage of hits in the correct layer:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Hits:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Hits: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsWithCorrectSlopeDistributionWarningMsg	*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsWithCorrectSlopeDistributionWarningMsg::trackWithHitsWithCorrectSlopeDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsWithCorrectSlopeDistributionWarningMsg::trackWithHitsWithCorrectSlopeDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsWithCorrectSlopeDistributionWarningMsg::~trackWithHitsWithCorrectSlopeDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsWithCorrectSlopeDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the percentage of hits with the correct slope:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Hits:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Hits: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsInLayerDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInLayerDistributionWarningMsg::trackWithHitsInLayerDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInLayerDistributionWarningMsg::trackWithHitsInLayerDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInLayerDistributionWarningMsg::~trackWithHitsInLayerDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInLayerDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the percentage of hits in one layer:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Hits:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Hits: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS arrayToDisplayIsNotAccessibleWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

arrayToDisplayIsNotAccessibleWarningMsg::arrayToDisplayIsNotAccessibleWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

arrayToDisplayIsNotAccessibleWarningMsg::~arrayToDisplayIsNotAccessibleWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void arrayToDisplayIsNotAccessibleWarningMsg::warningMsg() {

	printMsg("The array with the information to display is not accessible!!!");

}


/****************************************************************
 * CLASS trackWithSignatureDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithSignatureDistributionWarningMsg::trackWithSignatureDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithSignatureDistributionWarningMsg::trackWithSignatureDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithSignatureDistributionWarningMsg::~trackWithSignatureDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithSignatureDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bitArray       signature;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the signature of hits:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				signature = bitArray(i);

				temp += "Signature: ";
				temp += signature.toString();
				temp += " \t=> #Tracks: ";
				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsInCorrectCellDistributionWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInCorrectCellDistributionWarningMsg::trackWithHitsInCorrectCellDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInCorrectCellDistributionWarningMsg::trackWithHitsInCorrectCellDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInCorrectCellDistributionWarningMsg::~trackWithHitsInCorrectCellDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInCorrectCellDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bitArray       signature;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the signature of hits in the correct cell:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				signature = bitArray(i);

				temp += "Signature: ";
				temp += signature.toString();
				temp += " \t=> #Tracks: ";
				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithHitsInCellDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithHitsInCellDistributionWarningMsg::trackWithHitsInCellDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithHitsInCellDistributionWarningMsg::trackWithHitsInCellDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithHitsInCellDistributionWarningMsg::~trackWithHitsInCellDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithHitsInCellDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bitArray       signature;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the signature of hits in one cell:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				signature = bitArray(i);

				temp += "Signature: ";
				temp += signature.toString();
				temp += " \t=> #Tracks: ";
				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS middleDistanceOfMaximumAndCorrectCellWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectCellWarningMsg::middleDistanceOfMaximumAndCorrectCellWarningMsg() 
  : analysisWarningMsg(),
    distance(0)
{

  //	distance = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectCellWarningMsg::middleDistanceOfMaximumAndCorrectCellWarningMsg(double _distance) 
  : analysisWarningMsg(),
    distance(_distance)
{

  //	this->distance = _distance;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectCellWarningMsg::~middleDistanceOfMaximumAndCorrectCellWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void middleDistanceOfMaximumAndCorrectCellWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp  = "The middle distance of the cell with the maximum hits and the correct cell for all tracks is: ";
	dtos(distance, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS middleDistanceOfMaximumAndCorrectLayerWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectLayerWarningMsg::middleDistanceOfMaximumAndCorrectLayerWarningMsg() 
  : analysisWarningMsg(),
    distance(0)
{

  //	distance = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectLayerWarningMsg::middleDistanceOfMaximumAndCorrectLayerWarningMsg(double _distance) 
  : analysisWarningMsg(),
    distance(_distance)
{

  //	this->distance = _distance;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

middleDistanceOfMaximumAndCorrectLayerWarningMsg::~middleDistanceOfMaximumAndCorrectLayerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void middleDistanceOfMaximumAndCorrectLayerWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp  = "The middle distance of the layer with the maximum hits and the correct layer for all tracks is: ";
	dtos(distance, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotFillNotExistingDisplayWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotFillNotExistingDisplayWarningMsg::cannotFillNotExistingDisplayWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotFillNotExistingDisplayWarningMsg::~cannotFillNotExistingDisplayWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotFillNotExistingDisplayWarningMsg::warningMsg() {
	
	printMsg("You cannot fill a display which is not existing!!!");

}


/****************************************************************
 * CLASS displayCannotBeReadFromFileWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayCannotBeReadFromFileWarningMsg::displayCannotBeReadFromFileWarningMsg() 
  : analysisWarningMsg(),
    name(),
    title(),
    fileName()
{
  /*
	name.clear();
	title.clear();
	fileName.clear();
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

displayCannotBeReadFromFileWarningMsg::displayCannotBeReadFromFileWarningMsg(std::string _name, std::string _title, std::string _fileName) 
  : analysisWarningMsg(),
    name(_name),
    title(_title),
    fileName(_fileName)
{
  /*
	this->name     = _name;
	this->title    = _title;
	this->fileName = _fileName;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayCannotBeReadFromFileWarningMsg::~displayCannotBeReadFromFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayCannotBeReadFromFileWarningMsg::warningMsg() {
	
	std::string temp;

	temp  = "The display with the name '";
	temp += name;
	temp += "' and the title '";
	temp += title;
	temp += "' cannot be found either in the file '";
	temp += fileName;
	temp += "' nor in gDirectory!!!";
	
	printMsg(temp);

}


/****************************************************************
 * CLASS displayNotAccessibleWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

displayNotAccessibleWarningMsg::displayNotAccessibleWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

displayNotAccessibleWarningMsg::~displayNotAccessibleWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void displayNotAccessibleWarningMsg::warningMsg() {
	
	printMsg("The display is not accessible!!!");

}


/****************************************************************
 * CLASS cannotWriteNotEnabledLayerWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotWriteNotEnabledLayerWarningMsg::cannotWriteNotEnabledLayerWarningMsg() 
  : analysisWarningMsg(),
    layerToWrite(0),
    enabledLayer(0)
{

  //	layerToWrite = 0;
  //	enabledLayer = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotWriteNotEnabledLayerWarningMsg::cannotWriteNotEnabledLayerWarningMsg(unsigned short _layerToWrite, unsigned short _enabledLayer) 
  : analysisWarningMsg(),
    layerToWrite(_layerToWrite),
    enabledLayer(_enabledLayer)
{

  //	this->layerToWrite = _layerToWrite;
  //	this->enabledLayer = _enabledLayer;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotWriteNotEnabledLayerWarningMsg::~cannotWriteNotEnabledLayerWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotWriteNotEnabledLayerWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "You cannot write the layer ";
	ustos(layerToWrite, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " into the file, because you enabled layer ";
	ustos(enabledLayer, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotShowNotEnabledLayerWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotShowNotEnabledLayerWarningMsg::cannotShowNotEnabledLayerWarningMsg() 
  : analysisWarningMsg(),
    layerToShow(0),
    enabledLayer(0)
{

  //	layerToShow  = 0;
  //	enabledLayer = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotShowNotEnabledLayerWarningMsg::cannotShowNotEnabledLayerWarningMsg(unsigned short _layerToShow, unsigned short _enabledLayer) 
  : analysisWarningMsg(),
    layerToShow(_layerToShow),
    enabledLayer(_enabledLayer)
{
  /*
	this->layerToShow  = _layerToShow;
	this->enabledLayer = _enabledLayer;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotShowNotEnabledLayerWarningMsg::~cannotShowNotEnabledLayerWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotShowNotEnabledLayerWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "You cannot show the layer ";
	ustos(layerToShow, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", because you enabled layer ";
	ustos(enabledLayer, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += "!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS enableLayerIsOutOfRangeWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

enableLayerIsOutOfRangeWarningMsg::enableLayerIsOutOfRangeWarningMsg() 
  : analysisWarningMsg(),
    layer(0),
    layerMin(0),
    layerMax(0)
{
  /*
	layer    = 0;
	layerMin = 0;
	layerMax = 0;
*/
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

enableLayerIsOutOfRangeWarningMsg::enableLayerIsOutOfRangeWarningMsg(unsigned short _layer, unsigned short _layerMin, unsigned short _layerMax) 
  : analysisWarningMsg(),
    layer(_layer),
    layerMin(_layerMin),
    layerMax(_layerMax)
{
  /*
	this->layer    = _layer;
	this->layerMin = _layerMin;
	this->layerMax = _layerMax;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

enableLayerIsOutOfRangeWarningMsg::~enableLayerIsOutOfRangeWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void enableLayerIsOutOfRangeWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "You cannot set the layer to ";
	ustos(layer, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", because this is out of range [";
	ustos(layerMin, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", ";
	ustos(layerMax, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS cannotShowHistogramWithoutWritingToRootWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotShowHistogramWithoutWritingToRootWarningMsg::cannotShowHistogramWithoutWritingToRootWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotShowHistogramWithoutWritingToRootWarningMsg::~cannotShowHistogramWithoutWritingToRootWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotShowHistogramWithoutWritingToRootWarningMsg::warningMsg() {
	
	printMsg("For showing a histogram layer you have to enable at least this one to be written to file!!!");

}


/****************************************************************
 * CLASS trackWithMomentaErrorDistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithMomentaErrorDistributionWarningMsg::trackWithMomentaErrorDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(0),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithMomentaErrorDistributionWarningMsg::trackWithMomentaErrorDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithMomentaErrorDistributionWarningMsg::~trackWithMomentaErrorDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void trackWithMomentaErrorDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the percentage of error in the momenta:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				if (i == 0) {

					temp += "Error:  0% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}
				else {

					temp += "Error: ";
					uitos((unsigned int)((((double)i * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "% - <";
					uitos((unsigned int)((((double)(i + 1) * 100) / (double)numberOfEntries) + 0.5), buffer, 10, intConversionDigits);
					temp += buffer;
					temp += "%\t=> #Tracks: ";

				}

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS quantizedMomentaXDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedMomentaXDistributionWarningMsg::quantizedMomentaXDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    minimum(0),
    maximum(0)
{
  /*
	distribution    = NULL;
	numberOfEntries = 0;
	min             = 0;
	max             = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedMomentaXDistributionWarningMsg::quantizedMomentaXDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries, double _min, double _max) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    minimum(_min),
    maximum(_max)
{
  /*
	this->distribution    = _distribution;
	this->numberOfEntries = _numberOfEntries;
	this->min             = _min;
	this->max             = _max;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedMomentaXDistributionWarningMsg::~quantizedMomentaXDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedMomentaXDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           doubleBuffer[doubleConversion+1];
	char           intBuffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the momenta in X:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Momentum: ";
				dtos(((double)i * (maximum - minimum)) / (double)(numberOfEntries - 1) + minimum, doubleBuffer, doubleConversionDigits);
				temp += doubleBuffer;
				temp += "GeV/c\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS quantizedMomentaYDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedMomentaYDistributionWarningMsg::quantizedMomentaYDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    minimum(0),
    maximum(0)
{
  /*
	distribution    = NULL;
	numberOfEntries = 0;
	min             = 0;
	max             = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedMomentaYDistributionWarningMsg::quantizedMomentaYDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries, double _min, double _max) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    minimum(_min),
    maximum(_max)
{
  /*
	this->distribution    = _distribution;
	this->numberOfEntries = _numberOfEntries;
	this->min             = _min;
	this->max             = _max;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedMomentaYDistributionWarningMsg::~quantizedMomentaYDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedMomentaYDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           doubleBuffer[doubleConversion+1];
	char           intBuffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the momenta in Y:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Momentum: ";
				dtos(((double)i * (maximum - minimum)) / (double)(numberOfEntries - 1) + minimum, doubleBuffer, doubleConversionDigits);
				temp += doubleBuffer;
				temp += "GeV/c\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS quantizedMomentaZDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedMomentaZDistributionWarningMsg::quantizedMomentaZDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    minimum(0),
    maximum(0)
{
  /*
	distribution    = NULL;
	numberOfEntries = 0;
	min             = 0;
	max             = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedMomentaZDistributionWarningMsg::quantizedMomentaZDistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries, double _min, double _max) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    minimum(_min),
    maximum(_max)
{
  /*
	this->distribution    = _distribution;
	this->numberOfEntries = _numberOfEntries;
	this->min             = _min;
	this->max             = _max;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedMomentaZDistributionWarningMsg::~quantizedMomentaZDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedMomentaZDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           doubleBuffer[doubleConversion+1];
	char           intBuffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the momenta in Z:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Momentum: ";
				dtos(((double)i * (maximum - minimum)) / (double)(numberOfEntries - 1) + minimum, doubleBuffer, doubleConversionDigits);
				temp += doubleBuffer;
				temp += "GeV/c\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS quantizedHoughspaceDim1DistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedHoughspaceDim1DistributionWarningMsg::quantizedHoughspaceDim1DistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedHoughspaceDim1DistributionWarningMsg::quantizedHoughspaceDim1DistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedHoughspaceDim1DistributionWarningMsg::~quantizedHoughspaceDim1DistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedHoughspaceDim1DistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the Hough space dim1:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS quantizedHoughspaceDim2DistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedHoughspaceDim2DistributionWarningMsg::quantizedHoughspaceDim2DistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedHoughspaceDim2DistributionWarningMsg::quantizedHoughspaceDim2DistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedHoughspaceDim2DistributionWarningMsg::~quantizedHoughspaceDim2DistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedHoughspaceDim2DistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the Hough space dim2:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS quantizedHoughspaceDim3DistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedHoughspaceDim3DistributionWarningMsg::quantizedHoughspaceDim3DistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedHoughspaceDim3DistributionWarningMsg::quantizedHoughspaceDim3DistributionWarningMsg(unsigned int* _distribution, unsigned short _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedHoughspaceDim3DistributionWarningMsg::~quantizedHoughspaceDim3DistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedHoughspaceDim3DistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	char           buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the Hough space dim3:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((((double)distribution[i] * 100) / (double)numberOfTracks) + 0.5), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "% (";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithMomentaDistributionWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedMomentaDistributionWarningMsg::quantizedMomentaDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedMomentaDistributionWarningMsg::quantizedMomentaDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedMomentaDistributionWarningMsg::~quantizedMomentaDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedMomentaDistributionWarningMsg::warningMsg() {

	unsigned int i;
	bool         zeroDistribution;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the momenta:\n";

	if (distribution != NULL) {

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithCoordinateDistributionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

quantizedHoughspaceDistributionWarningMsg::quantizedHoughspaceDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

quantizedHoughspaceDistributionWarningMsg::quantizedHoughspaceDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{
  /*
	this->distribution    = _distribution;
	this->numberOfEntries = _numberOfEntries;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

quantizedHoughspaceDistributionWarningMsg::~quantizedHoughspaceDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void quantizedHoughspaceDistributionWarningMsg::warningMsg() {

	unsigned int i;
	bool         zeroDistribution;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	zeroDistribution = true;
	temp  = "Distribution of the tracks which have the Hough space:\n";

	if (distribution != NULL) {

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";
				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksWhichCannotBeFoundWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksWhichCannotBeFoundWarningMsg::numberOfTracksWhichCannotBeFoundWarningMsg() 
  : analysisWarningMsg(),
    numberOfTracks(0)
{

  //	numberOfTracks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksWhichCannotBeFoundWarningMsg::numberOfTracksWhichCannotBeFoundWarningMsg(unsigned long _numberOfTracks) 
  : analysisWarningMsg(),
    numberOfTracks(_numberOfTracks)
{

  //	this->numberOfTracks = _numberOfTracks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksWhichCannotBeFoundWarningMsg::~numberOfTracksWhichCannotBeFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksWhichCannotBeFoundWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfTracks, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " tracks which cannot be found, because they are in the same histogram cell than another track!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS dim1PeakDistanceDistributionWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dim1PeakDistanceDistributionWarningMsg::dim1PeakDistanceDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

dim1PeakDistanceDistributionWarningMsg::dim1PeakDistanceDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

dim1PeakDistanceDistributionWarningMsg::~dim1PeakDistanceDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void dim1PeakDistanceDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	double         mean;
	char           intBuffer[intConversionDigits+1];
	char           doubleBuffer[doubleConversion+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the peak distances in dim1:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean = 0;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				mean += (double)(i * distribution[i]) / (double)numberOfTracks;

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)(((double)distribution[i] * 100) / (double)numberOfTracks), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";
	else {
		temp += "\nMean peak distance in dim1: ";
		dtos(mean, doubleBuffer, doubleConversionDigits);
		temp += doubleBuffer;
		temp += " cells!!!\n";
	}

	printMsg(temp);

}


/****************************************************************
 * CLASS dim2PeakDistanceDistributionWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dim2PeakDistanceDistributionWarningMsg::dim2PeakDistanceDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

dim2PeakDistanceDistributionWarningMsg::dim2PeakDistanceDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

dim2PeakDistanceDistributionWarningMsg::~dim2PeakDistanceDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void dim2PeakDistanceDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	double         mean;
	char           intBuffer[intConversionDigits+1];
	char           doubleBuffer[doubleConversion+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the peak distances in dim2:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean = 0;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				mean += (double)(i * distribution[i]) / (double)numberOfTracks;

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)(((double)distribution[i] * 100) / (double)numberOfTracks), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";
	else {
		temp += "\nMean peak distance in dim2: ";
		dtos(mean, doubleBuffer, doubleConversionDigits);
		temp += doubleBuffer;
		temp += " cells!!!\n";
	}

	printMsg(temp);

}


/****************************************************************
 * CLASS dim3PeakDistanceDistributionWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dim3PeakDistanceDistributionWarningMsg::dim3PeakDistanceDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

dim3PeakDistanceDistributionWarningMsg::dim3PeakDistanceDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

dim3PeakDistanceDistributionWarningMsg::~dim3PeakDistanceDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void dim3PeakDistanceDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	double         mean;
	char           intBuffer[intConversionDigits+1];
	char           doubleBuffer[doubleConversion+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the peak distances in dim3:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean = 0;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				mean += (double)(i * distribution[i]) / (double)numberOfTracks;

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)(((double)distribution[i] * 100) / (double)numberOfTracks), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";
	else {
		temp += "\nMean peak distance in dim3: ";
		dtos(mean, doubleBuffer, doubleConversionDigits);
		temp += doubleBuffer;
		temp += " cells!!!\n";
	}

	printMsg(temp);

}


/****************************************************************
 * CLASS accumulatedPeakDistanceDistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

accumulatedPeakDistanceDistributionWarningMsg::accumulatedPeakDistanceDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

accumulatedPeakDistanceDistributionWarningMsg::accumulatedPeakDistanceDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

accumulatedPeakDistanceDistributionWarningMsg::~accumulatedPeakDistanceDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void accumulatedPeakDistanceDistributionWarningMsg::warningMsg() {

	unsigned short i;
	bool           zeroDistribution;
	std::string    temp;
	unsigned int   numberOfTracks;
	double         mean;
	char           intBuffer[intConversionDigits+1];
	char           doubleBuffer[doubleConversion+1];

	zeroDistribution = true;
	temp             = "Distribution of the tracks which have the accumulated peak distances in all dimensions:\n";

	if (distribution != NULL) {

		numberOfTracks = 0;
		for (i = 0; i < numberOfEntries; i++)
			numberOfTracks += distribution[i];

		/* This is just to avoid the division with zero */
		if (numberOfTracks == 0)
			numberOfTracks = 1;

		mean = 0;

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				mean += (double)(i * distribution[i]) / (double)numberOfTracks;

				zeroDistribution = false;
				temp += "Index: ";
				uitos(i, intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)(((double)distribution[i] * 100) / (double)numberOfTracks), intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += "% (";
				uitos(distribution[i], intBuffer, 10, intConversionDigits);
				temp += intBuffer;
				temp += ")\n";

			}

		}

	}

	if (zeroDistribution)
		temp += "The distribution is zero allover!!!\n";
	else {
		temp += "\nAccumulated mean peak distance: ";
		dtos(mean, doubleBuffer, doubleConversionDigits);
		temp += doubleBuffer;
		temp += " cells!!!\n";
	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksWithGoodPrelutSignatureWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksWithGoodPrelutSignatureWarningMsg::numberOfTracksWithGoodPrelutSignatureWarningMsg() 
  : analysisWarningMsg(),
    numberOfTracks(0)
{

  //	numberOfTracks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksWithGoodPrelutSignatureWarningMsg::numberOfTracksWithGoodPrelutSignatureWarningMsg(unsigned long _numberOfTracks) 
  : analysisWarningMsg(),
    numberOfTracks(_numberOfTracks)
{

  //	this->numberOfTracks = _numberOfTracks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksWithGoodPrelutSignatureWarningMsg::~numberOfTracksWithGoodPrelutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksWithGoodPrelutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfTracks, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " tracks which have an acceptable signature belonging to the prelut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksWithGoodLutSignatureWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksWithGoodLutSignatureWarningMsg::numberOfTracksWithGoodLutSignatureWarningMsg() 
  : analysisWarningMsg(),
    numberOfTracks(0)
{

  //	numberOfTracks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksWithGoodLutSignatureWarningMsg::numberOfTracksWithGoodLutSignatureWarningMsg(unsigned long _numberOfTracks) 
  : analysisWarningMsg(),
    numberOfTracks(_numberOfTracks)
{

  //	this->numberOfTracks = _numberOfTracks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksWithGoodLutSignatureWarningMsg::~numberOfTracksWithGoodLutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksWithGoodLutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfTracks, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " tracks which have an acceptable signature belonging to the lut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfGoodPrelutSignatureWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfGoodPrelutSignatureWarningMsg::numberOfGoodPrelutSignatureWarningMsg() 
  : analysisWarningMsg(),
    numberOfSignatures(0)
{

  //	numberOfSignatures = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfGoodPrelutSignatureWarningMsg::numberOfGoodPrelutSignatureWarningMsg(unsigned long _numberOfSignatures) 
  : analysisWarningMsg(),
    numberOfSignatures(_numberOfSignatures)
{

  //	this->numberOfSignatures = _numberOfSignatures;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfGoodPrelutSignatureWarningMsg::~numberOfGoodPrelutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfGoodPrelutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfSignatures, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " signatures which are accepted belonging to the prelut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfGoodLutSignatureWarningMsg						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfGoodLutSignatureWarningMsg::numberOfGoodLutSignatureWarningMsg() 
  : analysisWarningMsg(),
    numberOfSignatures(0)
{

  //	numberOfSignatures = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfGoodLutSignatureWarningMsg::numberOfGoodLutSignatureWarningMsg(unsigned long _numberOfSignatures) 
  : analysisWarningMsg(),
    numberOfSignatures(_numberOfSignatures)
{

  //	this->numberOfSignatures = _numberOfSignatures;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfGoodLutSignatureWarningMsg::~numberOfGoodLutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfGoodLutSignatureWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[longConversionDigits+1];

	temp  = "There are ";
	ultos(numberOfSignatures, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " signatures which are accepted belonging to the lut!!!\n";

	printMsg(temp);

}


/****************************************************************
 * CLASS goodPrelutSignatureWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

goodPrelutSignatureWarningMsg::goodPrelutSignatureWarningMsg() 
  : analysisWarningMsg(),
    stringTable()
{

  //	stringTable.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

goodPrelutSignatureWarningMsg::goodPrelutSignatureWarningMsg(std::string _stringTable) 
  : analysisWarningMsg(),
    stringTable(_stringTable)
{

  //	this->stringTable = _stringTable;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

goodPrelutSignatureWarningMsg::~goodPrelutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void goodPrelutSignatureWarningMsg::warningMsg() {

	std::string  temp;

	temp  = "The good signatures belonging to the prelut are:\n";
	temp += stringTable;

	printMsg(temp);

}


/****************************************************************
 * CLASS goodLutSignatureWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

goodLutSignatureWarningMsg::goodLutSignatureWarningMsg() 
  : analysisWarningMsg(),
    stringTable()
{

  //	stringTable.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

goodLutSignatureWarningMsg::goodLutSignatureWarningMsg(std::string _stringTable) 
  : analysisWarningMsg(),
    stringTable(_stringTable)
{

  //	this->stringTable = _stringTable;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

goodLutSignatureWarningMsg::~goodLutSignatureWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void goodLutSignatureWarningMsg::warningMsg() {

	std::string  temp;

	temp  = "The good signatures belonging to the lut are:\n";
	temp += stringTable;

	printMsg(temp);

}


/****************************************************************
 * CLASS usedTablesWarningMsg									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

usedTablesWarningMsg::usedTablesWarningMsg() 
  : analysisWarningMsg(),
    stringTable()
{

  //	stringTable.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

usedTablesWarningMsg::usedTablesWarningMsg(std::string _stringTable) 
  : analysisWarningMsg(),
    stringTable(_stringTable)
{

  //	this->stringTable = _stringTable;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

usedTablesWarningMsg::~usedTablesWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void usedTablesWarningMsg::warningMsg() {

	std::string  temp;

	temp  = "The used tables are:\n";
	temp += stringTable;

	printMsg(temp);

}


/****************************************************************
 * CLASS sameHoughspaceCellDistributionWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

sameHoughspaceCellDistributionWarningMsg::sameHoughspaceCellDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

sameHoughspaceCellDistributionWarningMsg::sameHoughspaceCellDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

sameHoughspaceCellDistributionWarningMsg::~sameHoughspaceCellDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void sameHoughspaceCellDistributionWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the tracks which have the same histogram cell:\n";
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 1) {

				nothingDetected = false;
				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No histogram cells consisting of more than one track found!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS volumeNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

volumeNotFoundWarningMsg::volumeNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

volumeNotFoundWarningMsg::~volumeNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void volumeNotFoundWarningMsg::warningMsg() {

	printMsg("The volume in the TGeoManager is not found!!!");

}


/****************************************************************
 * CLASS listOfVolumesNotFoundWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

listOfVolumesNotFoundWarningMsg::listOfVolumesNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

listOfVolumesNotFoundWarningMsg::~listOfVolumesNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void listOfVolumesNotFoundWarningMsg::warningMsg() {

	printMsg("The list of the volumes in the TGeoManager is not found!!!");

}


/****************************************************************
 * CLASS actualTrackNotFoundWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

actualTrackNotFoundWarningMsg::actualTrackNotFoundWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

actualTrackNotFoundWarningMsg::~actualTrackNotFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void actualTrackNotFoundWarningMsg::warningMsg() {

	printMsg("The actual track is not defined!!!");

}


/****************************************************************
 * CLASS timeViolationWarningMsg		 						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

timeViolationWarningMsg::timeViolationWarningMsg() 
  : analysisWarningMsg(),
    time1(0),
    time2(0)
{

  //	time1 = 0;
  //	time2 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

timeViolationWarningMsg::timeViolationWarningMsg(double _time1, double _time2) 
  : analysisWarningMsg(),
    time1(_time1),
    time2(_time2)
{

  //	this->time1 = _time1;
  //	this->time2 = _time2;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

timeViolationWarningMsg::~timeViolationWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void timeViolationWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp  = "The time for the added point is not acceptable because it is not rising. The violating time is ";
	dtos(time2, buffer, doubleConversionDigits);
	temp += buffer;
	temp += " while the last correct time was ";
	dtos(time1, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "!!!";
	
	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTracksInAllLayersWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTracksInAllLayersWarningMsg::numberOfTracksInAllLayersWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTracksInAllLayersWarningMsg::numberOfTracksInAllLayersWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage)
{
  /*
	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTracksInAllLayersWarningMsg::~numberOfTracksInAllLayersWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTracksInAllLayersWarningMsg::warningMsg() {

	bool          nothingDetected;
	std::string   temp;
	char          buffer[intConversionDigits+1];
	unsigned long numberOfTracks;

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;
	temp             = "Distribution of the number of track candidates per histogram layer:\n";
	numberOfTracks   = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos((unsigned int)((double)distribution[i] / (double)numberOfSummedEventsToAverage), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

				numberOfTracks += distribution[i];

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "Number of tracks: ";
		ultos((unsigned int)((double)numberOfTracks / (double)numberOfSummedEventsToAverage), buffer, 10, longConversionDigits);
		temp += buffer;

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfTrackDensitiesInAllLayersWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfTrackDensitiesInAllLayersWarningMsg::numberOfTrackDensitiesInAllLayersWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfTrackDensitiesInAllLayersWarningMsg::numberOfTrackDensitiesInAllLayersWarningMsg(int* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage)
{
  /*
	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfTrackDensitiesInAllLayersWarningMsg::~numberOfTrackDensitiesInAllLayersWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfTrackDensitiesInAllLayersWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;
	temp             = "Distribution of the difference number of track candidates between consecutive histogram layers:\n";
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] != 0) {

				nothingDetected = false;

				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				itos((int)((double)distribution[i] / (double)numberOfSummedEventsToAverage), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "The track density is equal for each layer!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfMinimalTracksInAllColumnsWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfMinimalTracksInAllColumnsWarningMsg::numberOfMinimalTracksInAllColumnsWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfMinimalTracksInAllColumnsWarningMsg::numberOfMinimalTracksInAllColumnsWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfMinimalTracksInAllColumnsWarningMsg::~numberOfMinimalTracksInAllColumnsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfMinimalTracksInAllColumnsWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the minimal number of found tracks per histogram column in all layers:\n";
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 0) {

				sum      += distribution[i];

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " tracks found in the worst case in one layer.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfAverageTracksInAllColumnsWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfAverageTracksInAllColumnsWarningMsg::numberOfAverageTracksInAllColumnsWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfAverageTracksInAllColumnsWarningMsg::numberOfAverageTracksInAllColumnsWarningMsg(double* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage)
{
  /*
	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfAverageTracksInAllColumnsWarningMsg::~numberOfAverageTracksInAllColumnsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfAverageTracksInAllColumnsWarningMsg::warningMsg() {

	unsigned int i;
	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the average number of found tracks per histogram column in all layers:\n";

	if (distribution != NULL) {

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				dtos(distribution[i] / (double)numberOfSummedEventsToAverage, buffer, doubleConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfMaximalTracksInAllColumnsWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfMaximalTracksInAllColumnsWarningMsg::numberOfMaximalTracksInAllColumnsWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

	distribution    = NULL;
	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfMaximalTracksInAllColumnsWarningMsg::numberOfMaximalTracksInAllColumnsWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

	this->distribution    = _distribution;
	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfMaximalTracksInAllColumnsWarningMsg::~numberOfMaximalTracksInAllColumnsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfMaximalTracksInAllColumnsWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the maximal number of found tracks per histogram column in all layers:\n";
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			sum      += distribution[i];

			if (distribution[i] > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " tracks found in the worst case in one layer.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS sizeOfColumnSeparatorFifosWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

sizeOfColumnSeparatorFifosWarningMsg::sizeOfColumnSeparatorFifosWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

sizeOfColumnSeparatorFifosWarningMsg::sizeOfColumnSeparatorFifosWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

sizeOfColumnSeparatorFifosWarningMsg::~sizeOfColumnSeparatorFifosWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void sizeOfColumnSeparatorFifosWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int value;
	unsigned int counter;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the assumed histogram column FIFO sizes for the separator of the found tracks:\n";
	value            = 0;
	counter          = 0;
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			value    = min(counter, distribution[i]);

			counter += distribution[i];

			sum     += value;

			if (value > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> Fifo-size: ";

				uitos(value, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No Fifos are needed!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " FIFOs needed at all.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfMinimalTracksInAllRowsWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfMinimalTracksInAllRowsWarningMsg::numberOfMinimalTracksInAllRowsWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfMinimalTracksInAllRowsWarningMsg::numberOfMinimalTracksInAllRowsWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfMinimalTracksInAllRowsWarningMsg::~numberOfMinimalTracksInAllRowsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfMinimalTracksInAllRowsWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the minimal number of found tracks per histogram row in all layers:\n";
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] > 0) {

				sum      += distribution[i];

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " tracks found in the worst case in one layer.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfAverageTracksInAllRowsWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfAverageTracksInAllRowsWarningMsg::numberOfAverageTracksInAllRowsWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfAverageTracksInAllRowsWarningMsg::numberOfAverageTracksInAllRowsWarningMsg(double* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage)
{
  /*
	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfAverageTracksInAllRowsWarningMsg::~numberOfAverageTracksInAllRowsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfAverageTracksInAllRowsWarningMsg::warningMsg() {

	unsigned int i;
	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the average number of found tracks per histogram row in all layers:\n";

	if (distribution != NULL) {

		for (i = 0; i < numberOfEntries; i++) {

			if (distribution[i] > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				dtos(distribution[i] / (double)numberOfSummedEventsToAverage, buffer, doubleConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS numberOfMaximalTracksInAllRowsWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfMaximalTracksInAllRowsWarningMsg::numberOfMaximalTracksInAllRowsWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfMaximalTracksInAllRowsWarningMsg::numberOfMaximalTracksInAllRowsWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfMaximalTracksInAllRowsWarningMsg::~numberOfMaximalTracksInAllRowsWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfMaximalTracksInAllRowsWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the maximal number of found tracks per histogram row in all layers:\n";
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			sum      += distribution[i];

			if (distribution[i] > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Tracks: ";

				uitos(distribution[i], buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No track is found!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " tracks found in the worst case in one layer.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS sizeOfRowSeparatorFifosWarningMsg						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

sizeOfRowSeparatorFifosWarningMsg::sizeOfRowSeparatorFifosWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0)
{

  //	distribution    = NULL;
  //	numberOfEntries = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

sizeOfRowSeparatorFifosWarningMsg::sizeOfRowSeparatorFifosWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries)
{

  //	this->distribution    = _distribution;
  //	this->numberOfEntries = _numberOfEntries;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

sizeOfRowSeparatorFifosWarningMsg::~sizeOfRowSeparatorFifosWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void sizeOfRowSeparatorFifosWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	unsigned int value;
	unsigned int counter;
	unsigned int sum;
	char         buffer[intConversionDigits+1];

	nothingDetected  = true;
	temp             = "Distribution of the assumed histogram row FIFO sizes for the separator of the found tracks:\n";
	value            = 0;
	counter          = 0;
	sum              = 0;
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			value    = min(counter, distribution[i]);

			counter += distribution[i];

			sum     += value;

			if (value > 0) {

				nothingDetected = false;

				temp += "Index: ";
				uitos(i, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> Fifo-size: ";

				uitos(value, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "No Fifos are needed!!!";

	}
	else {

		temp += "There are ";
		uitos(sum, buffer, 10, intConversionDigits);
		temp += buffer;
		temp += " FIFOs needed at all.";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg::youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg::~youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg::warningMsg() {

	printMsg("The distribution with the tracks with the same momentas cannot be enabled in combination with the distribution with the same coordinates!!!");

}


/****************************************************************
 * CLASS relativeDisplayNotAccessibleWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

relativeDisplayNotAccessibleWarningMsg::relativeDisplayNotAccessibleWarningMsg() : analysisWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

relativeDisplayNotAccessibleWarningMsg::~relativeDisplayNotAccessibleWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void relativeDisplayNotAccessibleWarningMsg::warningMsg() {

	printMsg("The relative display is not accessible!!!");

}


/****************************************************************
 * CLASS numberOfPrelutRangesFoundWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

numberOfPrelutRangesFoundWarningMsg::numberOfPrelutRangesFoundWarningMsg() 
  : analysisWarningMsg(),
    numberofPrelutRanges(0)
{

  //	numberofPrelutRanges = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

numberOfPrelutRangesFoundWarningMsg::numberOfPrelutRangesFoundWarningMsg(unsigned short _numberofPrelutRanges) 
  : analysisWarningMsg(),
    numberofPrelutRanges(_numberofPrelutRanges)
{

  //	this->numberofPrelutRanges = _numberofPrelutRanges;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

numberOfPrelutRangesFoundWarningMsg::~numberOfPrelutRangesFoundWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void numberOfPrelutRangesFoundWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "There are ";
	ustos(numberofPrelutRanges, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " prelut ranges detected which fullfills the requirements!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS maximumPrelutRangePercentageWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

maximumPrelutRangePercentageWarningMsg::maximumPrelutRangePercentageWarningMsg() 
  : analysisWarningMsg(),
    percentage(0)
{

  //	percentage = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

maximumPrelutRangePercentageWarningMsg::maximumPrelutRangePercentageWarningMsg(unsigned short _percentage) 
  : analysisWarningMsg(),
    percentage(_percentage)
{

  //	this->percentage = _percentage;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

maximumPrelutRangePercentageWarningMsg::~maximumPrelutRangePercentageWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void maximumPrelutRangePercentageWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp = "The maximum hit percentage in the prelut range is ";
	ustos(percentage, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += "%!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS meanPrelutRangeEntriesPerHitWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

meanPrelutRangeEntriesPerHitWarningMsg::meanPrelutRangeEntriesPerHitWarningMsg() 
  : analysisWarningMsg(),
    mean(0)
{

  //	mean = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

meanPrelutRangeEntriesPerHitWarningMsg::meanPrelutRangeEntriesPerHitWarningMsg(double _mean) 
  : analysisWarningMsg(),
    mean(_mean)
{

  //	this->mean = _mean;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

meanPrelutRangeEntriesPerHitWarningMsg::~meanPrelutRangeEntriesPerHitWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void meanPrelutRangeEntriesPerHitWarningMsg::warningMsg() {

	std::string temp;
	char        doubleBuffer[doubleConversion+1];

	temp =  "The prelut range causes a mean of ";
	dtos(mean, doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += " entries per hit!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS hitReadoutDistributionWarningMsg						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitReadoutDistributionWarningMsg::hitReadoutDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

hitReadoutDistributionWarningMsg::hitReadoutDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage)
{
  /*
	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitReadoutDistributionWarningMsg::~hitReadoutDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void hitReadoutDistributionWarningMsg::warningMsg() {

	bool         nothingDetected;
	std::string  temp;
	char         buffer[intConversionDigits+1];

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;
	temp             = "Distribution of the hit readout/processing to build all histogram layers:\n";
	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (distribution != NULL) {

			if (distribution[i] != 0) {

				nothingDetected = false;

				temp += "Number of readouts: ";
				uitos(i+1, buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\t=> #Hits: ";

				uitos((unsigned int)((double)distribution[i] / (double)numberOfSummedEventsToAverage), buffer, 10, intConversionDigits);
				temp += buffer;
				temp += "\n";

			}

		}

	}

	if (nothingDetected) {

		temp += "The distribution of the hit readout/processing to build all histogram layers is not found!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS hitReadoutDistributionWarningMsg						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitReadoutMeanDistributionWarningMsg::hitReadoutMeanDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

hitReadoutMeanDistributionWarningMsg::hitReadoutMeanDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage)
{
  /*
	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitReadoutMeanDistributionWarningMsg::~hitReadoutMeanDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void hitReadoutMeanDistributionWarningMsg::warningMsg() {

	unsigned int j;
	bool         nothingDetected;
	double       sum;
	std::string  temp;
	double       meanValue;
	char         intBuffer[intConversionDigits+1];
	char         doubleBuffer[doubleConversion+1];

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;

	sum   = 0;
	for (j = 0; j < numberOfEntries; j++)
		sum += (double)distribution[j] / (double)numberOfSummedEventsToAverage;

	if (sum == 0)
		sum = 1;

	temp  = "Distribution how often a single hit must be read in the mean, if a certain number of parallely implemented histogram layers are used:\n";

	for (unsigned int i = 0; i < numberOfEntries; i++) {

		meanValue = 0;
		for (j = 0; j < numberOfEntries; j++) {

			if (distribution != NULL) {

				if (distribution[j] != 0) {

					nothingDetected = false;

					if (i + 1 < numberOfEntries)
						meanValue += (1 + ((double)j / (double)(i + 1))) * ((double)distribution[j] / (double)numberOfSummedEventsToAverage);
					else
						meanValue += (double)distribution[j] / (double)numberOfSummedEventsToAverage;

				}

			}

		}

		if (meanValue > 0) {

			temp += "Parallely implemented layers: ";
			uitos(i + 1, intBuffer, 10, shortConversionDigits);
			temp += intBuffer;
			temp += "\t=> mean: ";
			dtos(meanValue / sum, doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += " times\n";
	
		}

	}

	if (nothingDetected) {

		temp += "The mean could just be computed, if there are any histogram layer distributions!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS fpgaHistogramProcessingTimeDistributionWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

fpgaHistogramProcessingTimeDistributionWarningMsg::fpgaHistogramProcessingTimeDistributionWarningMsg() 
  : analysisWarningMsg(),
    distribution(NULL),
    numberOfEntries(0),
    numberOfSummedEventsToAverage(0),
    readoutColumnsInParallel(false),
    histogramDim1(0),
    histogramDim2(0)
{
  /*
	distribution                  = NULL;
	numberOfEntries               = 0;
	numberOfSummedEventsToAverage = 0;
	readoutColumnsInParallel      = false;
	histogramDim1                 = 0;
	histogramDim2                 = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

fpgaHistogramProcessingTimeDistributionWarningMsg::fpgaHistogramProcessingTimeDistributionWarningMsg(unsigned int* _distribution, unsigned int _numberOfEntries, unsigned int _numberOfSummedEventsToAverage, bool _readoutColumnsInParallel, unsigned short _histogramDim1, unsigned short _histogramDim2) 
  : analysisWarningMsg(),
    distribution(_distribution),
    numberOfEntries(_numberOfEntries),
    numberOfSummedEventsToAverage(_numberOfSummedEventsToAverage),
    readoutColumnsInParallel(_readoutColumnsInParallel),
    histogramDim1(_histogramDim1),
    histogramDim2(_histogramDim2)
{
  /*
  	this->distribution                  = _distribution;
	this->numberOfEntries               = _numberOfEntries;
	this->numberOfSummedEventsToAverage = _numberOfSummedEventsToAverage;
	this->readoutColumnsInParallel      = _readoutColumnsInParallel;
	this->histogramDim1                 = _histogramDim1;
	this->histogramDim2                 = _histogramDim2;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

fpgaHistogramProcessingTimeDistributionWarningMsg::~fpgaHistogramProcessingTimeDistributionWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void fpgaHistogramProcessingTimeDistributionWarningMsg::warningMsg() {

	unsigned int j;
	bool         nothingDetected;
	std::string  temp;
	unsigned int timeHistogramReadout;
	double       timeHistogramming;
	char         buffer[intConversionDigits+1];

	/* This is just to avoid the division with zero */
	if (numberOfSummedEventsToAverage == 0)
		numberOfSummedEventsToAverage = 1;

	nothingDetected  = true;

	temp  = "Distribution of the time needed for processing the histogram, if a certain number of parallely implemented histogram layers are used:\n";

	for (unsigned int i = 0; i < numberOfEntries; i++) {

		if (readoutColumnsInParallel)
			timeHistogramReadout = histogramDim2 * ((numberOfEntries + i) / (i + 1));
		else
			timeHistogramReadout = histogramDim1 * ((numberOfEntries + i) / (i + 1));

		timeHistogramming    = 0;
		for (j = 0; j < numberOfEntries; j++) {

			if (distribution != NULL) {

				if (distribution[j] != 0) {

					nothingDetected = false;

					if (i + 1 < numberOfEntries)
						timeHistogramming += (1 + ((double)j / (double)(i + 1))) * ((double)distribution[j] / (double)numberOfSummedEventsToAverage);
					else
						timeHistogramming += (double)distribution[j] / (double)numberOfSummedEventsToAverage;

				}

			}

		}

		if (timeHistogramming > 0) {

			temp += "Parallely implemented layers: ";
			uitos(i + 1, buffer, 10, shortConversionDigits);
			temp += buffer;
			temp += "\t=> time: ";
			uitos((unsigned int)ceil(timeHistogramming) + timeHistogramReadout, buffer, 10, shortConversionDigits);
			temp += buffer;
			temp += " clock cycles\n";
	
		}

	}

	if (nothingDetected) {

		temp += "The mean could just be computed, if there are any histogram layer distributions!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS meanValueWarningMsg									*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

meanValueWarningMsg::meanValueWarningMsg() 
  : analysisWarningMsg(),
    mean(0),
    dimension()
{

  //	mean = 0;
  //	dimension.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

meanValueWarningMsg::meanValueWarningMsg(double _mean, std::string _dimension) 
  : analysisWarningMsg(),
    mean(_mean),
    dimension(_dimension)
{

  //	this->mean      = _mean;
  //	this->dimension = _dimension;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

meanValueWarningMsg::~meanValueWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void meanValueWarningMsg::warningMsg() {

	std::string temp;
	char        doubleBuffer[doubleConversion+1];

	temp += "\n";
	temp += dimension;
	temp += ": ";
	dtos(mean, doubleBuffer, doubleConversionDigits);
	temp += doubleBuffer;
	temp += " cells!!!\n";

	printMsg(temp);

}
