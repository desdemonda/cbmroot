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
//     - base class for warnings occuring during dataObject-access
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:59:22 $
// $Revision: 1.12 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataRootObjectWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS dataRootObjectWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dataRootObjectWarningMsg::dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

dataRootObjectWarningMsg::~dataRootObjectWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void dataRootObjectWarningMsg::printMsg(std::string message) {

#ifndef NODATAROOTOBJECTWARNINGMESSAGE 

	std::cerr << std::endl;
	
	std::cerr << DATAOBJECTLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS noTrackWithIdFoundWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

#if (ARCHITECTURE != PS3)
noTrackWithIdFoundWarningMsg::noTrackWithIdFoundWarningMsg() : dataRootObjectWarningMsg(), trackId(0), hit(NULL) {

	  //	trackId = 0;


	  //	hit     = NULL;

}
#else
noTrackWithIdFoundWarningMsg::noTrackWithIdFoundWarningMsg() : dataRootObjectWarningMsg(), trackId(0) {

  //	trackId = 0;

}

#endif

/****************************************************************
 * Constructor													*
 ****************************************************************/

#if (ARCHITECTURE != PS3)
noTrackWithIdFoundWarningMsg::noTrackWithIdFoundWarningMsg(int actualTrackId, trackfinderInputHit* actualHit) : dataRootObjectWarningMsg(), trackId(actualTrackId), hit(actualHit)  {

  //	trackId = actualTrackId;
  //	hit     = actualHit;

}
#else
noTrackWithIdFoundWarningMsg::noTrackWithIdFoundWarningMsg(int actualTrackId) : dataRootObjectWarningMsg(), trackId(actualTrackId) {

  //	trackId = actualTrackId;

}
#endif

/****************************************************************
 * Destructor													*
 ****************************************************************/

noTrackWithIdFoundWarningMsg::~noTrackWithIdFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noTrackWithIdFoundWarningMsg::warningMsg() {

	std::string  temp;
	char         intBuffer[intConversionDigits+1];

#if (ARCHITECTURE != PS3)

	char         doubleBuffer[doubleConversion+1];
	CbmPixelHit*      cbmHit;
	FairMCPoint*  cbmPoint;

#endif

	temp = "There is no track with the id ";
	itos(trackId, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += " !!!";

#if (ARCHITECTURE != PS3)

	if (hit != NULL) {

		temp += "\nHit-Information:";
		temp += "\n isMapsHit: ";
		btobs(hit->isMapsHit(), intBuffer);
		temp += intBuffer;
		temp += "\n isHybridHit: ";
		btobs(hit->isHybridHit(), intBuffer);
		temp += intBuffer;
		temp += "\n isStripHit: ";
		btobs(hit->isStripHit(), intBuffer);
		temp += intBuffer;
		temp += "\n hitIndex: ";
		itos(hit->getHitIndex(), intBuffer, 10, intConversionDigits);
		temp += intBuffer;

		cbmHit = hit->getHit();
		if (cbmHit != NULL) {

			temp += "\nFairHit-Information:";
			temp += "\n StationID: ";
			itos(cbmHit->GetAddress(), intBuffer, 10, intConversionDigits);
			temp += intBuffer;
			temp += "\n PosX: ";
			dtos(cbmHit->GetX(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PosY: ";
			dtos(cbmHit->GetY(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PosZ: ";
			dtos(cbmHit->GetZ(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n DX: ";
			dtos(cbmHit->GetDx(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n DY: ";
			dtos(cbmHit->GetDy(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n DZ: ";
			dtos(cbmHit->GetDz(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n RefIndex: ";
			itos(cbmHit->GetRefId(), intBuffer, 10, intConversionDigits);
			temp += intBuffer;

		}

		cbmPoint = hit->getPoint();
		if (cbmPoint != NULL) {

			temp += "\nCbmPoint-Information:";
			temp += "\n TrackID: ";
			itos(cbmPoint->GetTrackID(), intBuffer, 10, intConversionDigits);
			temp += intBuffer;
			temp += "\n StationID: ";
			itos(cbmPoint->GetDetectorID(), intBuffer, 10, intConversionDigits);
			temp += intBuffer;
			temp += "\n PosX: ";
			dtos(cbmPoint->GetX(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PosY: ";
			dtos(cbmPoint->GetY(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PosZ: ";
			dtos(cbmPoint->GetZ(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PX: ";
			dtos(cbmPoint->GetPx(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PY: ";
			dtos(cbmPoint->GetPy(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n PZ: ";
			dtos(cbmPoint->GetPz(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n Time: ";
			dtos(cbmPoint->GetTime(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n Length: ";
			dtos(cbmPoint->GetLength(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;
			temp += "\n ELoss: ";
			dtos(cbmPoint->GetEnergyLoss(), doubleBuffer, doubleConversionDigits);
			temp += doubleBuffer;

		}

	}

#endif

	printMsg(temp);

}


/****************************************************************
 * CLASS tracksWithNoIdWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tracksWithNoIdWarningMsg::tracksWithNoIdWarningMsg() : dataRootObjectWarningMsg(), quantum(0) {

  //	quantum = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tracksWithNoIdWarningMsg::tracksWithNoIdWarningMsg(int actualQuantum) : dataRootObjectWarningMsg(), quantum(actualQuantum) {

  //	quantum = actualQuantum;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tracksWithNoIdWarningMsg::~tracksWithNoIdWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tracksWithNoIdWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp = "There are ";
	uitos(quantum, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " hits with a wrong trackId!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS wrongHitDefinitionWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

wrongHitDefinitionWarningMsg::wrongHitDefinitionWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

wrongHitDefinitionWarningMsg::~wrongHitDefinitionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void wrongHitDefinitionWarningMsg::warningMsg() {

	printMsg("The hit is ignored because of a wrong definition. It must be either a maps, strip or hybrid one!!!");

}


/****************************************************************
 * CLASS wrongStationDefinitionWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

wrongStationDefinitionWarningMsg::wrongStationDefinitionWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

wrongStationDefinitionWarningMsg::~wrongStationDefinitionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void wrongStationDefinitionWarningMsg::warningMsg() {

	printMsg("The detector station is ignored because of a wrong definition. It must be either a maps, strip or hybrid one!!!");

}


/****************************************************************
 * CLASS indexOfMaskedStationWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

indexOfMaskedStationWarningMsg::indexOfMaskedStationWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

indexOfMaskedStationWarningMsg::~indexOfMaskedStationWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void indexOfMaskedStationWarningMsg::warningMsg() {

	printMsg("The detector station with this index is masked!!!");

}


/****************************************************************
 * CLASS trackWithTooBigLayerWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithTooBigLayerWarningMsg::trackWithTooBigLayerWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithTooBigLayerWarningMsg::~trackWithTooBigLayerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackWithTooBigLayerWarningMsg::warningMsg() {

	printMsg("The layer of the actual track coming from the histogram is too big!!!");

}


/****************************************************************
 * CLASS tooBigLayerWarningMsg				 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigLayerWarningMsg::tooBigLayerWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigLayerWarningMsg::~tooBigLayerWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tooBigLayerWarningMsg::warningMsg() {

	printMsg("The layer for indexing the histogram is too big!!!");

}


/****************************************************************
 * CLASS cannotAccessRunAnaWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessRunAnaWarningMsg::cannotAccessRunAnaWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessRunAnaWarningMsg::~cannotAccessRunAnaWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotAccessRunAnaWarningMsg::warningMsg() {

	printMsg("The run-analysis object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessRuntimeDbWarningMsg	 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessRuntimeDbWarningMsg::cannotAccessRuntimeDbWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessRuntimeDbWarningMsg::~cannotAccessRuntimeDbWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotAccessRuntimeDbWarningMsg::warningMsg() {

	printMsg("The runtime-database object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessFieldParWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessFieldParWarningMsg::cannotAccessFieldParWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessFieldParWarningMsg::~cannotAccessFieldParWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotAccessFieldParWarningMsg::warningMsg() {

	printMsg("The fieldPar object is not accessible!!!");

}


/****************************************************************
 * CLASS unknownFieldTypeWarningMsg			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

unknownFieldTypeWarningMsg::unknownFieldTypeWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

unknownFieldTypeWarningMsg::~unknownFieldTypeWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void unknownFieldTypeWarningMsg::warningMsg() {

	printMsg("The fieldType which is read from the parameter container is unknown!!!");

}


/****************************************************************
 * CLASS cannotGetMagneticFieldWarningMsg	 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotGetMagneticFieldWarningMsg::cannotGetMagneticFieldWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotGetMagneticFieldWarningMsg::~cannotGetMagneticFieldWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotGetMagneticFieldWarningMsg::warningMsg() {

	printMsg("Cannot get the magnetic field from the run-analysis object!!!");

}


/****************************************************************
 * CLASS stationSetMapsTypeWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

stationSetMapsTypeWarningMsg::stationSetMapsTypeWarningMsg() : dataRootObjectWarningMsg(), stationId(0), isHybridType(false), isStripType(false) {

  //	stationId    = 0;
  //	isHybridType = false;
  //	isStripType  = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

stationSetMapsTypeWarningMsg::stationSetMapsTypeWarningMsg(int _stationId, bool _isHybridType, bool _isStripType) : dataRootObjectWarningMsg(), stationId(_stationId), isHybridType(_isHybridType), isStripType(_isStripType)  {

  //	this->stationId    = _stationId;
  //	this->isHybridType = _isHybridType;
  //	this->isStripType  = _isStripType;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

stationSetMapsTypeWarningMsg::~stationSetMapsTypeWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void stationSetMapsTypeWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The station with id = ";
	itos(stationId, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is already a ";
	if (isHybridType)
		temp += "hybrid";
	else if (isStripType)
		temp += "strip";
	temp += " type. So you cannot change it to be a maps type!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS stationSetHybridTypeWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

stationSetHybridTypeWarningMsg::stationSetHybridTypeWarningMsg() : dataRootObjectWarningMsg(), stationId(0), isMapsType(false), isStripType(false) {

  //	stationId   = 0;
  //	isMapsType  = false;
  //	isStripType = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

stationSetHybridTypeWarningMsg::stationSetHybridTypeWarningMsg(int _stationId, bool _isMapsType, bool _isStripType) : dataRootObjectWarningMsg(), stationId(_stationId), isMapsType(_isMapsType), isStripType(_isStripType)  {

	this->stationId   = _stationId;
	this->isMapsType  = _isMapsType;
	this->isStripType = _isStripType;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

stationSetHybridTypeWarningMsg::~stationSetHybridTypeWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void stationSetHybridTypeWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The station with id = ";
	itos(stationId, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is already a ";
	if (isMapsType)
		temp += "maps";
	else if (isStripType)
		temp += "strip";
	temp += " type. So you cannot change it to be a hybrid type!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS stationSetStripTypeWarningMsg		 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

stationSetStripTypeWarningMsg::stationSetStripTypeWarningMsg() : dataRootObjectWarningMsg(), stationId(0), isMapsType(false), isHybridType(false)  {

  //	stationId    = 0;
  //	isMapsType   = false;
  //	isHybridType = false;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

stationSetStripTypeWarningMsg::stationSetStripTypeWarningMsg(int _stationId, bool _isMapsType, bool _isHybridType) : dataRootObjectWarningMsg(), stationId(_stationId), isMapsType(_isMapsType), isHybridType(_isHybridType)  {

  //	this->stationId    = _stationId;
  //	this->isMapsType   = _isMapsType;
  //	this->isHybridType = _isHybridType;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

stationSetStripTypeWarningMsg::~stationSetStripTypeWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void stationSetStripTypeWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The station with id = ";
	itos(stationId, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is already a ";
	if (isMapsType)
		temp += "maps";
	else if (isHybridType)
		temp += "hybrid";
	temp += " type. So you cannot change it to be a strip type!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithNoPointWarningMsg			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithNoPointWarningMsg::trackWithNoPointWarningMsg() : dataRootObjectWarningMsg(), trackIndex(0) {

  //	trackIndex = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithNoPointWarningMsg::trackWithNoPointWarningMsg(int _trackIndex) : dataRootObjectWarningMsg(), trackIndex(_trackIndex) {

  //	this->trackIndex = _trackIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithNoPointWarningMsg::~trackWithNoPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackWithNoPointWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The track with index = ";
	itos(trackIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " has no points!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithNoHitWarningMsg			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithNoHitWarningMsg::trackWithNoHitWarningMsg() : dataRootObjectWarningMsg(), trackIndex(0) {

  //	trackIndex = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithNoHitWarningMsg::trackWithNoHitWarningMsg(int _trackIndex) : dataRootObjectWarningMsg(), trackIndex(_trackIndex) {

  //	this->trackIndex = _trackIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithNoHitWarningMsg::~trackWithNoHitWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackWithNoHitWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The track with index = ";
	itos(trackIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " has no hits!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS trackWithWrongNumberOfHitsToPointsWarningMsg 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackWithWrongNumberOfHitsToPointsWarningMsg::trackWithWrongNumberOfHitsToPointsWarningMsg() : dataRootObjectWarningMsg(), trackIndex(0), numberOfPoints(0), numberOfHits(0) {

  //	trackIndex     = 0;
  //	numberOfPoints = 0;
  //	numberOfHits   = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackWithWrongNumberOfHitsToPointsWarningMsg::trackWithWrongNumberOfHitsToPointsWarningMsg(int _trackIndex, int _numberOfPoints, int _numberOfHits) : dataRootObjectWarningMsg(), trackIndex(_trackIndex), numberOfPoints(_numberOfPoints), numberOfHits(_numberOfHits)  {

  //	this->trackIndex     = _trackIndex;
  //	this->numberOfPoints = _numberOfPoints;
  //	this->numberOfHits   = _numberOfHits;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackWithWrongNumberOfHitsToPointsWarningMsg::~trackWithWrongNumberOfHitsToPointsWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackWithWrongNumberOfHitsToPointsWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The track with index = ";
	itos(trackIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " has ";
	itos(numberOfPoints, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " point(s) but ";
	itos(numberOfHits, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " hit(s)!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tracksWithNoPointWarningMsg					 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tracksWithNoPointWarningMsg::tracksWithNoPointWarningMsg() : dataRootObjectWarningMsg(), tracksWithNoPoints(0) {

  //	tracksWithNoPoints = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tracksWithNoPointWarningMsg::tracksWithNoPointWarningMsg(unsigned short _tracksWithNoPoints) : dataRootObjectWarningMsg(), tracksWithNoPoints(_tracksWithNoPoints)  {

  //	this->tracksWithNoPoints = _tracksWithNoPoints;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tracksWithNoPointWarningMsg::~tracksWithNoPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tracksWithNoPointWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "There are ";
	itos(tracksWithNoPoints, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " tracks which have no points!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tracksWithNoHitWarningMsg						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tracksWithNoHitWarningMsg::tracksWithNoHitWarningMsg() : dataRootObjectWarningMsg(), tracksWithNoHits(0)  {

  //	tracksWithNoHits = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tracksWithNoHitWarningMsg::tracksWithNoHitWarningMsg(unsigned short _tracksWithNoHits) : dataRootObjectWarningMsg(), tracksWithNoHits(_tracksWithNoHits)  {

  //	this->tracksWithNoHits = _tracksWithNoHits;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tracksWithNoHitWarningMsg::~tracksWithNoHitWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tracksWithNoHitWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "There are ";
	itos(tracksWithNoHits, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " tracks which have no hits!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tracksWithWrongNumberOfHitsToPointsWarningMsg	 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tracksWithWrongNumberOfHitsToPointsWarningMsg::tracksWithWrongNumberOfHitsToPointsWarningMsg() : dataRootObjectWarningMsg(), trackWithWrongNumberOfHitsToPoints(0) {

  //	trackWithWrongNumberOfHitsToPoints = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tracksWithWrongNumberOfHitsToPointsWarningMsg::tracksWithWrongNumberOfHitsToPointsWarningMsg(unsigned short _trackWithWrongNumberOfHitsToPoints) : dataRootObjectWarningMsg(), trackWithWrongNumberOfHitsToPoints(_trackWithWrongNumberOfHitsToPoints) {

  //	this->trackWithWrongNumberOfHitsToPoints = _trackWithWrongNumberOfHitsToPoints;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tracksWithWrongNumberOfHitsToPointsWarningMsg::~tracksWithWrongNumberOfHitsToPointsWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tracksWithWrongNumberOfHitsToPointsWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "There are ";
	itos(trackWithWrongNumberOfHitsToPoints, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " tracks which have more or less points than hits!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS stationIndexIsBiggerThanLastStationIndexWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

stationIndexIsBiggerThanLastStationIndexWarningMsg::stationIndexIsBiggerThanLastStationIndexWarningMsg() : dataRootObjectWarningMsg(), stationIndex(0), lastStationIndex(0) {

  //	stationIndex     = 0;
  //	lastStationIndex = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

stationIndexIsBiggerThanLastStationIndexWarningMsg::stationIndexIsBiggerThanLastStationIndexWarningMsg(unsigned short _stationIndex, unsigned short _lastStationIndex) : dataRootObjectWarningMsg(), stationIndex(_stationIndex), lastStationIndex(_lastStationIndex)  {

  //	this->stationIndex     = _stationIndex;
  //	this->lastStationIndex = _lastStationIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

stationIndexIsBiggerThanLastStationIndexWarningMsg::~stationIndexIsBiggerThanLastStationIndexWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void stationIndexIsBiggerThanLastStationIndexWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The stationIndex ";
	itos(stationIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is bigger than the index of the last stationIndex (";
	itos(lastStationIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ")!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS integrationRangeOfZeroWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

integrationRangeOfZeroWarningMsg::integrationRangeOfZeroWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

integrationRangeOfZeroWarningMsg::~integrationRangeOfZeroWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void integrationRangeOfZeroWarningMsg::warningMsg() {

	printMsg("The integration range is from 0 to 0. this cannot be done!!!");

}


/****************************************************************
 * CLASS analyserIsNotAccessibleWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analyserIsNotAccessibleWarningMsg::analyserIsNotAccessibleWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analyserIsNotAccessibleWarningMsg::~analyserIsNotAccessibleWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void analyserIsNotAccessibleWarningMsg::warningMsg() {

	printMsg("The analyser object is not accessible!!!");

}


/****************************************************************
 * CLASS hitAnalysisNotSupportedWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitAnalysisNotSupportedWarningMsg::hitAnalysisNotSupportedWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitAnalysisNotSupportedWarningMsg::~hitAnalysisNotSupportedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitAnalysisNotSupportedWarningMsg::warningMsg() {

	printMsg("The analysis for the hits cannot be supported because the whole analysis is disabled at compilation!!!");

}


/****************************************************************
 * CLASS hitWithNoPointWarningMsg			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitWithNoPointWarningMsg::hitWithNoPointWarningMsg() : dataRootObjectWarningMsg(), hitIndex(0) {

  //	hitIndex = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

hitWithNoPointWarningMsg::hitWithNoPointWarningMsg(int _hitIndex) : dataRootObjectWarningMsg(), hitIndex(_hitIndex) {

  //	this->hitIndex = _hitIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitWithNoPointWarningMsg::~hitWithNoPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitWithNoPointWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The hit with index = ";
	itos(hitIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " has no point!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS hitWithNoTrackWarningMsg			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitWithNoTrackWarningMsg::hitWithNoTrackWarningMsg() : dataRootObjectWarningMsg(), hitIndex(0) {

  //	hitIndex = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

hitWithNoTrackWarningMsg::hitWithNoTrackWarningMsg(int _hitIndex) : dataRootObjectWarningMsg(), hitIndex(_hitIndex) {

  //	this->hitIndex = _hitIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitWithNoTrackWarningMsg::~hitWithNoTrackWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitWithNoTrackWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "The hit with index = ";
	itos(hitIndex, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " has no track!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS hitsWithNoPointWarningMsg						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitsWithNoPointWarningMsg::hitsWithNoPointWarningMsg() : dataRootObjectWarningMsg(), hitsWithNoPoint(0) {

  //	hitsWithNoPoint = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

hitsWithNoPointWarningMsg::hitsWithNoPointWarningMsg(unsigned short _hitsWithNoPoint) : dataRootObjectWarningMsg(), hitsWithNoPoint(_hitsWithNoPoint) {

  //	this->hitsWithNoPoint = _hitsWithNoPoint;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitsWithNoPointWarningMsg::~hitsWithNoPointWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitsWithNoPointWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "There are ";
	itos(hitsWithNoPoint, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " hits which have no point!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS hitsWithNoTrackWarningMsg						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

hitsWithNoTrackWarningMsg::hitsWithNoTrackWarningMsg() : dataRootObjectWarningMsg(), hitsWithNoTrack(0) {

  //	hitsWithNoTrack = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

hitsWithNoTrackWarningMsg::hitsWithNoTrackWarningMsg(unsigned short _hitsWithNoTrack) : dataRootObjectWarningMsg(), hitsWithNoTrack(_hitsWithNoTrack) {

  //	this->hitsWithNoTrack = _hitsWithNoTrack;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hitsWithNoTrackWarningMsg::~hitsWithNoTrackWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void hitsWithNoTrackWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp  = "There are ";
	itos(hitsWithNoTrack, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " hits which have no track!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS reReadMagneticFieldWarningMsg	 						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

reReadMagneticFieldWarningMsg::reReadMagneticFieldWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

reReadMagneticFieldWarningMsg::~reReadMagneticFieldWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void reReadMagneticFieldWarningMsg::warningMsg() {

	printMsg("The magnetic field is not delivered from the framework. So it must be reread manually!!!");

}


/****************************************************************
 * CLASS lutGoodnessNotPossibleForGradingPWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutGoodnessNotPossibleForGradingPWarningMsg::lutGoodnessNotPossibleForGradingPWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutGoodnessNotPossibleForGradingPWarningMsg::~lutGoodnessNotPossibleForGradingPWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void lutGoodnessNotPossibleForGradingPWarningMsg::warningMsg() {

	printMsg("The gradingP table cannot be generated with LUTGOODNESSTABLE, because the table is a prerequisite for this algorithm!!!");

}


/****************************************************************
 * CLASS missingOriginWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

missingOriginWarningMsg::missingOriginWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

missingOriginWarningMsg::~missingOriginWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void missingOriginWarningMsg::warningMsg() {

	printMsg("The origin is missing. Maybe the algorithm is going wrong, because it the origin is a prerequisite of many algorithms!!!");

}


/****************************************************************
 * CLASS noEntryInListWarningMsg								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noEntryInListWarningMsg::noEntryInListWarningMsg() : dataRootObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noEntryInListWarningMsg::~noEntryInListWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void noEntryInListWarningMsg::warningMsg() {

	printMsg("There is no entry in the list!!!");

}
