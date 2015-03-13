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
//     - reads the event information from a special file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:16 $
// $Revision: 1.11 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../../DataRootObjectLIB/include/trackfinderInputHit.h"
#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include "../include/inputError.h"
#include "../include/inputWarningMsg.h"
#include "../include/inputAscii.h"


#ifdef DETECTORINFO
	#include <iostream>
#endif
#ifdef HITINFO
#ifndef DETECTORINFO
	#include <iostream>
#endif
#endif
#ifdef TRACKINFO
#ifndef HITINFO
#ifndef DETECTORINFO
	#include <iostream>
#endif
#endif
#endif


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputAscii::inputAscii() 
  : inputData(),
    dataValidation(false),
    sourceFile()
{

   //	dataValidation = false;

}

/****************************************************************
 * Constructor													*
 * Errors:														*
 * - cannotOpenFileError										*
 ****************************************************************/

inputAscii::inputAscii(const char* name, bitArray detMask, int hitProducer) 
  : inputData(detMask, hitProducer),
    dataValidation(false),
    sourceFile()
{

	dataValidation = false;

	/* open file if possible */
	sourceFile.open(name);
	if (sourceFile.fail())
		throw cannotOpenFileError(INPUTLIB, name);

}

/****************************************************************
 * Destructor													*
 * Errors:														*
 * - cannotCloseFileError										*
 ****************************************************************/

inputAscii::~inputAscii() {

	/* close file if possible */
	if (sourceFile.is_open()) {
		sourceFile.close();
		if (sourceFile.fail())
			throw cannotCloseFileError(INPUTLIB);
	}

}

/****************************************************************
 * method initializes the object.								*
 ****************************************************************/

void inputAscii::init() {

	data.init();
	data.initDefaultDetector();

}

void inputAscii::init(const char* name, int hitProducer) {

	inputData::init(hitProducer);

	if (sourceFile.is_open()) {
		sourceFile.close();
		if (sourceFile.fail())
			throw cannotCloseFileError(INPUTLIB);
	}
	sourceFile.open(name);
	if (sourceFile.fail())
		throw cannotOpenFileError(INPUTLIB, name);

	init();

}
void inputAscii::init(const char* name, bitArray detMask, int hitProducer) {

	inputData::init(detMask);
	init(name, hitProducer);

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void inputAscii::readDetectorInfo(CbmStsDigiScheme* scheme) {

	int                     filePosition;
	int                     temp;
	trackfinderInputStation actualStation;
	int                     id;
	double                  distance;

	if (scheme != NULL) {

		digiSchemeIsNotSupportedInAsciiWarningMsg* digiSchemeIsNotSupportedInAscii = new digiSchemeIsNotSupportedInAsciiWarningMsg();
		digiSchemeIsNotSupportedInAscii->warningMsg();
		if(digiSchemeIsNotSupportedInAscii != NULL) {
			delete digiSchemeIsNotSupportedInAscii;
			digiSchemeIsNotSupportedInAscii = NULL;
		}

	}

	filePosition = sourceFile.tellg();

	sourceFile >> temp;

	if (temp < 0) {

		data.removeDetector();

		for (int i = 0; i < (-temp); i++) {

			/* set the distance for the detector */
			sourceFile >> id >> distance;
			actualStation.setId(id);
			actualStation.setDistance(distance);

			/* reset the type of the detector */
			actualStation.removeType();

			data.addStation(actualStation);

#ifdef DETECTORINFO

			std::cout << "Station "             << i                               << std::endl;
			std::cout << " id: "                << actualStation.getId()           << std::endl;
			std::cout << " distance: "          << actualStation.getDistance()     << std::endl;
			std::cout << " isMaps: "            << actualStation.isMapsType()      << std::endl;
			std::cout << " isStrip: "           << actualStation.isStripType()     << std::endl;
			std::cout << " isHybrid: "          << actualStation.isHybridType()    << std::endl;
			std::cout << " isMasked: "          << actualStation.isMasked()        << std::endl;
			std::cout << std::endl;

#endif

		}

		data.maskDetector(detectorMask);
	
	}
	else
		sourceFile.seekg(filePosition);

}

/****************************************************************
 * method reads the source data from a file based on the		*
 * CbmStsMapsHit-class, CbmStsHybridHit-class and				*
 * CbmStsStripHit-class											*
 ****************************************************************/

void inputAscii::readDataSource(unsigned int event, TClonesArray* mHitArray, TClonesArray* hHitArray, TClonesArray* sHitArray) {

	double                  xCoord;
	double                  yCoord;
	double                  zCoord;
	int                     trackID;
	int                     pdgID;
	int                     stationID;
	int                     charge;
	int                     filePosition;
	int                     temp;
	unsigned int            i;
	unsigned int            numberOfTracks;
	unsigned int            numberOfHits;
	bool                    isMapsType;
	bool                    isHybridType;
	bool                    isStripType;
	double                  hitXCoord;
	double                  hitYCoord;
	FairHitProducer*         hitProducer;

#ifdef TRACKINFO

	trackfinderInputTrack*  track;

#endif

#ifdef HITINFO

	trackfinderInputHit*    hit;
	magneticFieldValue      magnetField;

#endif

	if ((mHitArray != NULL) || (hHitArray != NULL) || (sHitArray != NULL)) {

		hitArrayPointerIsNotSupportedInAsciiWarningMsg* hitArrayPointerIsNotSupportedInAscii = new hitArrayPointerIsNotSupportedInAsciiWarningMsg();
		hitArrayPointerIsNotSupportedInAscii->warningMsg();
		if(hitArrayPointerIsNotSupportedInAscii != NULL) {
			delete hitArrayPointerIsNotSupportedInAscii;
			hitArrayPointerIsNotSupportedInAscii = NULL;
		}

	}

	hitProducer = new FairHitProducer(typeOfHitProducer, data.getDetectorPointer());

	/* read number of tracks and Hits */
	while ((data.getEventNumber() != event) || (!dataValidation)) {

		filePosition = sourceFile.tellg();
		sourceFile >> temp;

		if (temp < 0) {
			sourceFile.seekg(filePosition);
			readDetectorInfo();
			continue;
		}
		else
			data.setEventNumber((unsigned int)temp);

		sourceFile >> numberOfTracks >> numberOfHits;

		if (sourceFile.eof()) {
			if (data.getEventNumber() != event)
				throw eventNotFoundError(event);
			else
				throw unexpectedEofFoundError(INPUTLIB);
		}

		if (data.getEventNumber() == event) {

			/* READ THE TRACKS AND HITS FROM THE STSDETECTOR */
		
			if ((numberOfTracks == 0) || (numberOfHits == 0))
				throw zeroTracksOrHitsError(numberOfTracks, numberOfHits);

			/* ERASING DATA STORAGE */

			data.removeAllTracks();
			data.removeAllHits();

			/* SETTING THE INPUT */

			for (i = 0; i < numberOfTracks; i++) {
	
				sourceFile >> trackID >> pdgID >> charge >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);
				else
					data.addTrack(pdgID, xCoord, yCoord, zCoord, trackID, charge);
	
			}
	
			for (i = 0; i < numberOfHits; i++) {
	
				sourceFile >> stationID >> trackID >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);
				else {
					hitProducer->produceHit(&hitXCoord, &hitYCoord, &isMapsType, &isHybridType, &isStripType, xCoord, yCoord, stationID);
/* This is just used because the type of the station must be derived from the hits */
					if (isMapsType)
						if (data.getDetectorPointer()->getStationPointerById(stationID) != NULL)
							data.getDetectorPointer()->getStationPointerById(stationID)->setMapsType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */
/* This is just used because the type of the station must be derived from the hits */
					if (isHybridType)
						if (data.getDetectorPointer()->getStationPointerById(stationID) != NULL)
							data.getDetectorPointer()->getStationPointerById(stationID)->setHybridType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */
/* This is just used because the type of the station must be derived from the hits */
					if (isStripType)
						if (data.getDetectorPointer()->getStationPointerById(stationID) != NULL)
							data.getDetectorPointer()->getStationPointerById(stationID)->setStripType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */
					if (!data.getDetector().getStationById(stationID).isMasked())
						data.addHit(trackID, stationID, hitXCoord, hitYCoord, xCoord, yCoord, zCoord, i, i, isMapsType, isStripType, isHybridType);
				}
	
			}

			data.finalizeHitTrackAssignment(true);

			if ((data.getNumberOfTracks() == 0) || (data.getNumberOfHits() == 0))
				throw zeroTracksOrHitsError(data.getNumberOfTracks(), data.getNumberOfHits());

#ifdef TRACKINFO

			std::cout << std::endl;
			std::cout << "numberOfTracks: " << data.getNumberOfTracks() << std::endl;

			for (i = 0; i < (unsigned int)data.getNumberOfTracks(); i++ ) {
			
				if ((i > 5) && (i < 10)) {

					track = data.getTrackByOrder(i);

					std::cout << " PdgCode: "      << track->getPdgCode() << std::endl;
					std::cout << " Charge:    "    << track->getCharge()  << std::endl;

					std::cout << " momemtumX: "    << track->getMomX()    << std::endl;
					std::cout << " momentumY: "    << track->getMomY()    << std::endl;
					std::cout << " momentumZ: "    << track->getMomZ()    << std::endl;

					std::cout << std::endl;

				}

			}

#endif

#ifdef HITINFO

			std::cout << std::endl;
			std::cout << "numberOfHits: " << data.getNumberOfHits() << std::endl;

			for (i = 0; i < (unsigned int)data.getNumberOfHits(); i++ ) {

				if ((i > 5) && (i < 10)) {

					hit = data.getHitByIndex(i);
					if (hit == NULL)
						throw cannotAccessHitsOrTracksError(INPUTLIB);
		
					data.getMagneticField()->getFieldValues(hit, &magnetField);

					std::cout << "TrackID: "     << hit->getPoint()->GetTrackID() << std::endl;
					std::cout << " StationID: "  << hit->getStationId()           << std::endl;

					std::cout << " PosX: "       << hit->getPosX()                << std::endl;
					std::cout << " PosY: "       << hit->getPosY()                << std::endl;
					std::cout << " PosZ: "       << hit->getPosZ()                << std::endl;

					std::cout << " FieldX: "     << magnetField.getFieldX()       << std::endl;
					std::cout << " FieldY: "     << magnetField.getFieldY()       << std::endl;
					std::cout << " FieldZ: "     << magnetField.getFieldZ()       << std::endl;

					std::cout << std::endl;
		
				}
		
			}

#endif

			dataValidation = true;

		}
		else {

			for (i = 0; i < numberOfTracks; i++) {

				sourceFile >> trackID >> pdgID >> charge >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);

			}

			for (i = 0; i < numberOfHits; i++) {

				sourceFile >> stationID >> trackID >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);

			}

		}

	}

	if (hitProducer != NULL) {
		delete hitProducer;
		hitProducer = NULL;
	}

}

/****************************************************************
 * method reads the source data from a file based on the		*
 * CbmStsHit-class												*
 ****************************************************************/

void inputAscii::readDataSource(unsigned int event, TClonesArray* mvdHitArray, TClonesArray* stsHitArray) {

	double                  xCoord;
	double                  yCoord;
	double                  zCoord;
	int                     trackID;
	int                     pdgID;
	int                     stationID;
	int                     charge;
	int                     filePosition;
	int                     temp;
	unsigned int            i;
	unsigned int            numberOfTracks;
	unsigned int            numberOfHits;
	double                  hitXCoord;
	double                  hitYCoord;
	FairHitProducer*         hitProducer;

#ifdef TRACKINFO

	trackfinderInputTrack*  track;

#endif

#ifdef HITINFO

	trackfinderInputHit*    hit;
	magneticFieldValue      magnetField;

#endif

	if ((mvdHitArray != NULL) || (stsHitArray != NULL)) {

		hitArrayPointerIsNotSupportedInAsciiWarningMsg* hitArrayPointerIsNotSupportedInAscii = new hitArrayPointerIsNotSupportedInAsciiWarningMsg();
		hitArrayPointerIsNotSupportedInAscii->warningMsg();
		if(hitArrayPointerIsNotSupportedInAscii != NULL) {
			delete hitArrayPointerIsNotSupportedInAscii;
			hitArrayPointerIsNotSupportedInAscii = NULL;
		}

	}

	hitProducer = new FairHitProducer(typeOfHitProducer, data.getDetectorPointer());

	/* read number of tracks and Hits */
	while ((data.getEventNumber() != event) || (!dataValidation)) {

		filePosition = sourceFile.tellg();
		sourceFile >> temp;

		if (temp < 0) {
			sourceFile.seekg(filePosition);
			readDetectorInfo();
			continue;
		}
		else
			data.setEventNumber((unsigned int)temp);

		sourceFile >> numberOfTracks >> numberOfHits;

		if (sourceFile.eof()) {
			if (data.getEventNumber() != event)
				throw eventNotFoundError(event);
			else
				throw unexpectedEofFoundError(INPUTLIB);
		}

		if (data.getEventNumber() == event) {

			/* READ THE TRACKS AND HITS FROM THE STSDETECTOR */
		
			if ((numberOfTracks == 0) || (numberOfHits == 0))
				throw zeroTracksOrHitsError(numberOfTracks, numberOfHits);

			/* ERASING DATA STORAGE */

			data.removeAllTracks();
			data.removeAllHits();

			/* SETTING THE INPUT */

			for (i = 0; i < numberOfTracks; i++) {
	
				sourceFile >> trackID >> pdgID >> charge >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);
				else
					data.addTrack(pdgID, xCoord, yCoord, zCoord, trackID, charge);
	
			}
	
			for (i = 0; i < numberOfHits; i++) {
	
				sourceFile >> stationID >> trackID >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);
				else {
					hitProducer->produceHit(&hitXCoord, &hitYCoord, xCoord, yCoord);

					if (!data.getDetector().getStationById(stationID).isMasked())
						data.addHit(trackID, stationID, hitXCoord, hitYCoord, xCoord, yCoord, zCoord, i, i);
				}
	
			}

			data.finalizeHitTrackAssignment(true);

			if ((data.getNumberOfTracks() == 0) || (data.getNumberOfHits() == 0))
				throw zeroTracksOrHitsError(data.getNumberOfTracks(), data.getNumberOfHits());

#ifdef TRACKINFO

			std::cout << std::endl;
			std::cout << "numberOfTracks: " << data.getNumberOfTracks() << std::endl;

			for (i = 0; i < (unsigned int)data.getNumberOfTracks(); i++ ) {
			
				if ((i > 5) && (i < 10)) {

					track = data.getTrackByOrder(i);

					std::cout << " PdgCode: "      << track->getPdgCode() << std::endl;
					std::cout << " Charge:    "    << track->getCharge()  << std::endl;

					std::cout << " momemtumX: "    << track->getMomX()    << std::endl;
					std::cout << " momentumY: "    << track->getMomY()    << std::endl;
					std::cout << " momentumZ: "    << track->getMomZ()    << std::endl;

					std::cout << std::endl;

				}

			}

#endif

#ifdef HITINFO

			std::cout << std::endl;
			std::cout << "numberOfHits: " << data.getNumberOfHits() << std::endl;

			for (i = 0; i < (unsigned int)data.getNumberOfHits(); i++ ) {

				if ((i > 5) && (i < 10)) {

					hit = data.getHitByIndex(i);
					if (hit == NULL)
						throw cannotAccessHitsOrTracksError(INPUTLIB);
		
					data.getMagneticField()->getFieldValues(hit, &magnetField);

					std::cout << "TrackID: "     << hit->getPoint()->GetTrackID() << std::endl;
					std::cout << " StationID: "  << hit->getStationId()           << std::endl;

					std::cout << " PosX: "       << hit->getPosX()                << std::endl;
					std::cout << " PosY: "       << hit->getPosY()                << std::endl;
					std::cout << " PosZ: "       << hit->getPosZ()                << std::endl;

					std::cout << " FieldX: "     << magnetField.getFieldX()       << std::endl;
					std::cout << " FieldY: "     << magnetField.getFieldY()       << std::endl;
					std::cout << " FieldZ: "     << magnetField.getFieldZ()       << std::endl;

					std::cout << std::endl;
		
				}
		
			}

#endif

			dataValidation = true;

		}
		else {

			for (i = 0; i < numberOfTracks; i++) {

				sourceFile >> trackID >> pdgID >> charge >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);

			}

			for (i = 0; i < numberOfHits; i++) {

				sourceFile >> stationID >> trackID >> xCoord >> yCoord >> zCoord;

				if (sourceFile.eof())
					throw unexpectedEofFoundError(INPUTLIB);

			}

		}

	}

	if (hitProducer != NULL) {
		delete hitProducer;
		hitProducer = NULL;
	}

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double inputAscii::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputData::getReservedSizeOfData(0);
	returnValue += sizeof(dataValidation);
	returnValue += sizeof(sourceFile);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double inputAscii::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputData::getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double inputAscii::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputData::getUsedSizeOfData(0);
	returnValue += sizeof(dataValidation);
	returnValue += sizeof(sourceFile);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(inputAscii)
