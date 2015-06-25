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
//     - reads the event information from a special file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:41:08 $
// $Revision: 1.16 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../include/inputError.h"
#include "../include/inputWarningMsg.h"
#include "../include/inputRoot.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/CbmStsPoint.h"
	#include "../../RootFrameworkLIB/include/CbmMvdPoint.h"
	#include "../../RootFrameworkLIB/include/CbmStsMapsHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsStripHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsHybridHit.h"
	#include "../../RootFrameworkLIB/include/CbmStsHit.h"
	#include "../../RootFrameworkLIB/include/CbmMvdHit.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "CbmStsPoint.h"
	#ifdef HITCOMPATIBILITY
		#include "CbmStsMapsHit.h"
		#include "CbmStsStripHit.h"
		#include "CbmStsHybridHit.h"
	#else
		#include "CbmMvdPoint.h"
		#include "CbmStsHit.h"
		#include "CbmMvdHit.h"
      #include "CbmStsAddress.h"
	#endif
#endif
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoShape.h"
#include "TGeoTube.h"
#ifndef ROOT_TParticlePDG
 #include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
 #include "TDatabasePDG.h"
#endif
#include <stdlib.h>
#include <fstream>


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


#define numberOfAvoidingPdgCodes 8
const int avoidingPdgCodes[numberOfAvoidingPdgCodes] = {10010020, 10010030, 50000050, 50010051, 10020040, 1000010020, 1000010030, 1000020040};


/****************************************************************
 * Method generates the name of the actual file to read.		*
 ****************************************************************/

std::string inputRoot::generateFileName(std::string name) {

	std::string returnValue;

	returnValue.clear();

	if ((name.find("../") == 0) || (name.find("./") == 0)) {

		if (getenv("VMCWORKDIR") != NULL) {

			returnValue  = getenv("VMCWORKDIR");
			returnValue += "/";

		}

	}

	returnValue += name;

	return returnValue;

}

/****************************************************************
 * Method generates the name of the actual file to read.		*
 ****************************************************************/

std::string inputRoot::generateMvdStationFileName(unsigned int stationNumber) {

	char        intBuffer[intConversionDigits + 1];
	std::string returnValue;

	returnValue  = geometryFileDirectory;
	returnValue += "mvd_station";
	uitos(stationNumber, intBuffer, 10, intConversionDigits);
	returnValue += intBuffer;
	returnValue += "_geom.";
	returnValue += ASCIIVERSION;
	returnValue += ".par";

	return returnValue;

}
std::string inputRoot::generateStsStationFileName(unsigned int stationNumber) {

	char        intBuffer[intConversionDigits + 1];
	std::string returnValue;

	returnValue  = geometryFileDirectory;
	returnValue += "sts_station";
	uitos(stationNumber, intBuffer, 10, intConversionDigits);
	returnValue += intBuffer;
	returnValue += "_geom.";
	returnValue += ASCIIVERSION;
	returnValue += ".par";

	return returnValue;

}

/****************************************************************
 * Method generates the name of the actual volume to read.		*
 ****************************************************************/

std::string inputRoot::generateMvdStationVolumeName(unsigned int stationNumber, bool newFormat) {

	std::string returnValue;
	char        intBuffer[intConversionDigits + 1];

	returnValue  = "mvdstation";
	uitos(stationNumber, intBuffer, 10, intConversionDigits);
	if ((newFormat) && (stationNumber < 10))
		returnValue += "0";
	returnValue += intBuffer;

	return returnValue;

}
std::string inputRoot::generateStsStationVolumeName(unsigned int stationNumber, bool newFormat) {

	std::string returnValue;
	char        intBuffer[intConversionDigits + 1];

	returnValue  = "stsstation";
	uitos(stationNumber, intBuffer, 10, intConversionDigits);
	if ((newFormat) && (stationNumber < 10))
		returnValue += "0";
	returnValue += intBuffer;

	return returnValue;

}

/****************************************************************
 * Method reads the variables based on the detector stations.	*
 ****************************************************************/

bool inputRoot::readDetector() {

	bool returnValue;

	returnValue = readDetectorFromAsciiFile();

	if (!returnValue)
		returnValue = readDetectorFromAsciiFiles();

	if (!returnValue)
		returnValue = readDetectorFromTGeoManager();

	return returnValue;

}

/**
 * Method reads the variables of the detector from ascii file.
 */

bool inputRoot::readDetectorFromAsciiFile() {

	bool                    returnValue;
	unsigned int            numberOfStations;
	ifstream*               inputFile;
	std::string             skipInformation;
	trackfinderInputStation actualStation;
	int                     id;
	std::string             type;
	double                  thickness;
	double                  distance;
	double                  radiusIn;
	double                  radiusOut;
	double                  thicknessIn;
	double                  thicknessOut;

	numberOfStations = 0;

	if (!detectorFileName.empty()) {

		inputFile = new ifstream(detectorFileName.c_str());

		if (inputFile->is_open()) {

			*inputFile >> skipInformation >> numberOfStations;

#ifdef DETECTORINFO

			std::cout << std::endl;
			std::cout << "numberOfStations: " << numberOfStations << std::endl;

#endif

			/* skip some information parameters in the file */
			for (unsigned int i = 0; i < 16; i++) {

				*inputFile >> skipInformation;

			}

			data.removeDetector();

			/* get the information for each station */
			for (unsigned int i = 0; i < numberOfStations; i++) {

				*inputFile >> id >> type >> thickness >> distance >> radiusIn >> radiusOut >> thicknessIn >> thicknessOut;

				/* set the id of the station */
				actualStation.setId(id + numberOfVolumesInfrontOfSTS);
				/* set the distnace of the station */
				actualStation.setDistance(distance / 10);	/* factor to convert from mm to cm */
				/* set the type of the station */
				actualStation.removeType();

				/* add the station */
				data.addStation(actualStation);

#ifdef DETECTORINFO

				std::cout << "actualStationCounter: "    << i                               << std::endl;
				std::cout << " id: "                     << actualStation.getId()           << std::endl;
				std::cout << " distance: "               << actualStation.getDistance()     << std::endl;
				std::cout << " isMaps: "                 << actualStation.isMapsType()      << std::endl;
				std::cout << " isHybrid: "               << actualStation.isHybridType()    << std::endl;
				std::cout << " isStrip: "                << actualStation.isStripType()     << std::endl;
				std::cout << std::endl;

#endif

			}

			inputFile->close();
		
			if (inputFile != NULL) {
				delete inputFile;
				inputFile = NULL;
			}

		}

	}

	if (numberOfStations > 0)
		returnValue = true;
	else {

		asciiFileNotFoundWarningMsg* asciiFileNotFound = new asciiFileNotFoundWarningMsg(detectorFileName);
		asciiFileNotFound->warningMsg();
		if(asciiFileNotFound != NULL) {
			delete asciiFileNotFound;
			asciiFileNotFound = NULL;
		}

		returnValue = false;

	}

	return returnValue;

}

/****************************************************************
 * Method reads the variables of the detector from ascii files.	*
 ****************************************************************/

bool inputRoot::readDetectorFromAsciiFiles() {

	bool isMvdDetectorRead;
	bool isStsDetectorRead;
	bool isDetectorRead;

	data.removeDetector();

	if (readMapsHits)
		isMvdDetectorRead = readDetectorFromAsciiFiles();
	else
		isMvdDetectorRead = false;

	if (readHybridHits || readStripHits)
		isStsDetectorRead = readDetectorFromAsciiFiles();
	else
		isStsDetectorRead = false;

	if (readMapsHits) {

		if (readHybridHits || readStripHits)
			isDetectorRead = (isMvdDetectorRead && isStsDetectorRead);
		else
			isDetectorRead = (isMvdDetectorRead && !isStsDetectorRead);

	}
	else {

		if (readHybridHits || readStripHits)
			isDetectorRead = (!isMvdDetectorRead && isStsDetectorRead);
		else
			isDetectorRead = false;

	}

	return isDetectorRead;

}
bool inputRoot::readMvdDetectorFromAsciiFiles() {

	bool                    returnValue;
	unsigned int            numberOfStations;
	std::string             actualStationFileName;
	std::string             actualStationName;
	ifstream*               inputFile;
	trackfinderInputStation actualStation;
	int                     id;
	double                  distance;

	numberOfStations  = 0;

	/* get just the number of stations */
	while (1 == 1) {

		actualStationFileName = generateMvdStationFileName(numberOfStations + 1);

		inputFile             = new ifstream(actualStationFileName.c_str());

		if (!inputFile->is_open())
			break;
		
		inputFile->close();
		
		if (inputFile != NULL) {
			delete inputFile;
			inputFile = NULL;
		}

		numberOfStations++;

	}

#ifdef DETECTORINFO

	std::cout << std::endl;
	std::cout << "numberOfStations: " << numberOfStations << std::endl;

#endif

	/* get the information for each station */
	for (unsigned int i = 0; i < numberOfStations; i++) {

		actualStationFileName = generateMvdStationFileName(i + 1);

		inputFile             = new ifstream(actualStationFileName.c_str());

		if (!inputFile->is_open()) {

			asciiFilesNotFoundWarningMsg* asciiFilesNotFound = new asciiFilesNotFoundWarningMsg();
			asciiFilesNotFound->warningMsg();
			if(asciiFilesNotFound != NULL) {
				delete asciiFilesNotFound;
				asciiFilesNotFound = NULL;
			}

			numberOfStations = i;

		}

		*inputFile >> actualStationName >> id >> distance;
		actualStation.setId(id);
		actualStation.setDistance(distance);

		inputFile->close();
		
		if (inputFile != NULL) {
			delete inputFile;
			inputFile = NULL;
		}

		/* set the type of the station */
		actualStation.removeType();

		/* add the station */
		data.addStation(actualStation);

#ifdef DETECTORINFO

		std::cout << actualStationName           << ":"                             << std::endl;
		std::cout << " actualStationFileName: "  << actualStationFileName           << std::endl;
		std::cout << " id: "                     << actualStation.getId()           << std::endl;
		std::cout << " distance: "               << actualStation.getDistance()     << std::endl;
		std::cout << " isMaps: "                 << actualStation.isMapsType()      << std::endl;
		std::cout << " isHybrid: "               << actualStation.isHybridType()    << std::endl;
		std::cout << " isStrip: "                << actualStation.isStripType()     << std::endl;
		std::cout << std::endl;

#endif

	}

	if (numberOfStations > 0)
		returnValue = true;
	else {

		asciiFilesNotFoundWarningMsg* asciiFilesNotFound = new asciiFilesNotFoundWarningMsg();
		asciiFilesNotFound->warningMsg();
		if(asciiFilesNotFound != NULL) {
			delete asciiFilesNotFound;
			asciiFilesNotFound = NULL;
		}

		returnValue = false;

	}

	return returnValue;

}
bool inputRoot::readStsDetectorFromAsciiFiles() {

	bool                    returnValue;
	unsigned int            numberOfStations;
	std::string             actualStationFileName;
	std::string             actualStationName;
	ifstream*               inputFile;
	trackfinderInputStation actualStation;
	int                     id;
	double                  distance;

	numberOfStations  = 0;

	/* get just the number of stations */
	while (1 == 1) {

		actualStationFileName = generateStsStationFileName(numberOfStations + 1);

		inputFile             = new ifstream(actualStationFileName.c_str());

		if (!inputFile->is_open())
			break;
		
		inputFile->close();
		
		if (inputFile != NULL) {
			delete inputFile;
			inputFile = NULL;
		}

		numberOfStations++;

	}

#ifdef DETECTORINFO

	std::cout << std::endl;
	std::cout << "numberOfStations: " << numberOfStations << std::endl;

#endif

	/* get the information for each station */
	for (unsigned int i = 0; i < numberOfStations; i++) {

		actualStationFileName = generateStsStationFileName(i + 1);

		inputFile             = new ifstream(actualStationFileName.c_str());

		if (!inputFile->is_open()) {

			asciiFilesNotFoundWarningMsg* asciiFilesNotFound = new asciiFilesNotFoundWarningMsg();
			asciiFilesNotFound->warningMsg();
			if(asciiFilesNotFound != NULL) {
				delete asciiFilesNotFound;
				asciiFilesNotFound = NULL;
			}

			numberOfStations = i;

		}

		*inputFile >> actualStationName >> id >> distance;
		actualStation.setId(id);
		actualStation.setDistance(distance);

		inputFile->close();
		
		if (inputFile != NULL) {
			delete inputFile;
			inputFile = NULL;
		}

		/* set the type of the station */
		actualStation.removeType();

		/* add the station */
		data.addStation(actualStation);

#ifdef DETECTORINFO

		std::cout << actualStationName           << ":"                             << std::endl;
		std::cout << " actualStationFileName: "  << actualStationFileName           << std::endl;
		std::cout << " id: "                     << actualStation.getId()           << std::endl;
		std::cout << " distance: "               << actualStation.getDistance()     << std::endl;
		std::cout << " isMaps: "                 << actualStation.isMapsType()      << std::endl;
		std::cout << " isHybrid: "               << actualStation.isHybridType()    << std::endl;
		std::cout << " isStrip: "                << actualStation.isStripType()     << std::endl;
		std::cout << std::endl;

#endif

	}

	if (numberOfStations > 0)
		returnValue = true;
	else {

		asciiFilesNotFoundWarningMsg* asciiFilesNotFound = new asciiFilesNotFoundWarningMsg();
		asciiFilesNotFound->warningMsg();
		if(asciiFilesNotFound != NULL) {
			delete asciiFilesNotFound;
			asciiFilesNotFound = NULL;
		}

		returnValue = false;

	}

	return returnValue;

}

/****************************************************************
 * Method reads the variables of the detector from TGeoManager.	*
 ****************************************************************/

bool inputRoot::readDetectorFromTGeoManager() {

	bool isMvdDetectorRead;
	bool isStsDetectorRead;
	bool isDetectorRead;

	data.removeDetector();

	if (readMapsHits)
		isMvdDetectorRead = readDetectorFromTGeoManager();
	else
		isMvdDetectorRead = false;

	if (readHybridHits || readStripHits)
		isStsDetectorRead = readDetectorFromTGeoManager();
	else
		isStsDetectorRead = false;

	if (readMapsHits) {

		if (readHybridHits || readStripHits)
			isDetectorRead = (isMvdDetectorRead && isStsDetectorRead);
		else
			isDetectorRead = (isMvdDetectorRead && !isStsDetectorRead);

	}
	else {

		if (readHybridHits || readStripHits)
			isDetectorRead = (!isMvdDetectorRead && isStsDetectorRead);
		else
			isDetectorRead = false;

	}

	return isDetectorRead;

}
bool inputRoot::readMvdDetectorFromTGeoManager() {

	bool                    returnValue;
	unsigned int            i;
	unsigned int            numberOfStations;
	bool                    foundNodeName;
	std::string             actualStationName;
	std::string             nodeName;
	TGeoVolume*             volume;
	trackfinderInputStation actualStation;

#ifdef DETECTORINFO

	TGeoTube*       shape;
	TGeoMaterial*   material;

#endif

	TGeoNode*       node;
	TGeoMatrix*     matrix;
	const double*   translation;

	volume      = NULL;

#ifdef DETECTORINFO

	shape       = NULL;
	material    = NULL;

#endif

	node        = NULL;
	matrix      = NULL;
	translation = NULL;

	numberOfStations = 0;

	if (gGeoManager != NULL) {

		while (gGeoManager) {

	        /* get name from volume */
			actualStationName  = generateMvdStationVolumeName(numberOfStations + 1, false);
		
			if ((volume = gGeoManager->GetVolume(actualStationName.c_str())) == NULL)
				break;

			numberOfStations++;

#ifdef DETECTORINFO

			/* get the shape from volume */
			shape    = (TGeoTube*) volume->GetShape();
			if (shape == NULL) {
				stationShapeNotFoundWarningMsg* noShape = new stationShapeNotFoundWarningMsg(actualStationName);
				noShape->warningMsg();
				if(noShape != NULL) {
					delete noShape;
					noShape = NULL;
				}
			}

			/* get the material from volume */
			material = (TGeoMaterial*) volume->GetMaterial();
			if (material == NULL) {
				stationMaterialNotFoundWarningMsg* noMaterial = new stationMaterialNotFoundWarningMsg(actualStationName);
				noMaterial->warningMsg();
				if(noMaterial != NULL) {
					delete noMaterial;
					noMaterial = NULL;
				}
			}

			std::cout << actualStationName  << ":" << std::endl;
			std::cout << " UID:     " << gGeoManager->GetUID(actualStationName.c_str()) << std::endl;
			if (shape != NULL) {
				std::cout << " Dz:      " << shape->GetDz()                   << std::endl;
				std::cout << " Rmin:    " << shape->GetRmin()                 << std::endl;
				std::cout << " Rmax:    " << shape->GetRmax()                 << std::endl;
			}
			if (material != NULL) {
				std::cout << " A:       " << material->GetA()                 << std::endl;
				std::cout << " Z:       " << material->GetZ()                 << std::endl;
				std::cout << " Density: " << material->GetDensity()           << std::endl;
				std::cout << " RadLen:  " << material->GetRadLen()            << std::endl;
			}

			std::cout << std::endl;

#endif

		}

#ifdef DETECTORINFO

		std::cout << std::endl;
		std::cout << "numberOfStations: " << numberOfStations << std::endl;

#endif

		for (i = 0; i < numberOfStations; i++) {

			node = NULL;

	        /* get name from volume */
			actualStationName = generateMvdStationVolumeName(i + 1, false);

			/* get id from volume */
			actualStation.setId(gGeoManager->GetUID(actualStationName.c_str()));

			/* get name from node for volume, if it is in the vacuum */
			nodeName  = topnode;
			nodeName += "/";
			nodeName += STSVACUUMFODLER;
			nodeName += "/";
			nodeName += actualStationName;
			nodeName += "_1";

			/* change to the node for the volume, if it exists */
			foundNodeName = gGeoManager->cd(nodeName.c_str());

			/* the node for the volume is not in the vacuum */
			if (!foundNodeName) {

				/* get name from node for volume, if it is not in the vacuum */
				nodeName  = topnode;
				nodeName += "/";
				nodeName += actualStationName;
				nodeName += "_1";

				/* change to the node for the volume, if it exists */
				foundNodeName = gGeoManager->cd(nodeName.c_str());

			}

			if (foundNodeName)
			/* get the node for the volume */
				node = gGeoManager->GetCurrentNode();
			else
				throw detectorNodeNotFoundError(nodeName);

			/* get the matrix from node for volume */
			if (node != NULL)
				matrix = node->GetMatrix();

			/* get the translation from the matrix for node for volume */
			if (matrix != NULL)
				translation = matrix->GetTranslation();
			else
				throw detectorMatrixNotFoundError(nodeName);
		
			/* set the distance for the detector */
			if (translation != NULL)
				actualStation.setDistance(translation[2]);
			else
				throw detectorTranslationNotFoundError(nodeName);

			/* set the type of the station */
			actualStation.removeType();

			/* add the station */
			data.addStation(actualStation);

#ifdef DETECTORINFO

			std::cout << actualStationName      << ":"                              << std::endl;
			std::cout << " nodeName: "          << nodeName                         << std::endl;
			std::cout << " translation: "       << translation[0]                   << " , ";
			std::cout << translation[1]         << " , " << translation[2]          << std::endl;
			std::cout << " id: "                << actualStation.getId()            << std::endl;
			std::cout << " distance: "          << actualStation.getDistance()      << std::endl;
			std::cout << " isMaps: "            << actualStation.isMapsType()       << std::endl;
			std::cout << " isHybrid: "          << actualStation.isHybridType()     << std::endl;
			std::cout << " isStrip: "           << actualStation.isStripType()      << std::endl;
			std::cout << std::endl;

#endif

		}

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

	if (numberOfStations > 0)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}
bool inputRoot::readStsDetectorFromTGeoManager() {

	bool                    returnValue;
	unsigned int            i;
	unsigned int            numberOfStations;
	bool                    foundNodeName;
	std::string             actualStationName;
	std::string             nodeName;
	TGeoVolume*             volume;
	trackfinderInputStation actualStation;

#ifdef DETECTORINFO

	TGeoTube*       shape;
	TGeoMaterial*   material;

#endif

	TGeoNode*       node;
	TGeoMatrix*     matrix;
	const double*   translation;

	volume      = NULL;

#ifdef DETECTORINFO

	shape       = NULL;
	material    = NULL;

#endif

	node        = NULL;
	matrix      = NULL;
	translation = NULL;

	numberOfStations = 0;

	if (gGeoManager != NULL) {

		while (gGeoManager) {

	        /* get name from volume */
			actualStationName  = generateStsStationVolumeName(numberOfStations + 1, false);
		
			if ((volume = gGeoManager->GetVolume(actualStationName.c_str())) == NULL)
				break;

			numberOfStations++;

#ifdef DETECTORINFO

			/* get the shape from volume */
			shape    = (TGeoTube*) volume->GetShape();
			if (shape == NULL) {
				stationShapeNotFoundWarningMsg* noShape = new stationShapeNotFoundWarningMsg(actualStationName);
				noShape->warningMsg();
				if(noShape != NULL) {
					delete noShape;
					noShape = NULL;
				}
			}

			/* get the material from volume */
			material = (TGeoMaterial*) volume->GetMaterial();
			if (material == NULL) {
				stationMaterialNotFoundWarningMsg* noMaterial = new stationMaterialNotFoundWarningMsg(actualStationName);
				noMaterial->warningMsg();
				if(noMaterial != NULL) {
					delete noMaterial;
					noMaterial = NULL;
				}
			}

			std::cout << actualStationName  << ":" << std::endl;
			std::cout << " UID:     " << gGeoManager->GetUID(actualStationName.c_str()) << std::endl;
			if (shape != NULL) {
				std::cout << " Dz:      " << shape->GetDz()                   << std::endl;
				std::cout << " Rmin:    " << shape->GetRmin()                 << std::endl;
				std::cout << " Rmax:    " << shape->GetRmax()                 << std::endl;
			}
			if (material != NULL) {
				std::cout << " A:       " << material->GetA()                 << std::endl;
				std::cout << " Z:       " << material->GetZ()                 << std::endl;
				std::cout << " Density: " << material->GetDensity()           << std::endl;
				std::cout << " RadLen:  " << material->GetRadLen()            << std::endl;
			}

			std::cout << std::endl;

#endif

		}

#ifdef DETECTORINFO

		std::cout << std::endl;
		std::cout << "numberOfStations: " << numberOfStations << std::endl;

#endif

		for (i = 0; i < numberOfStations; i++) {

			node = NULL;

	        /* get name from volume */
			actualStationName = generateStsStationVolumeName(i + 1, false);

			/* get id from volume */
			actualStation.setId(gGeoManager->GetUID(actualStationName.c_str()));

			/* get name from node for volume, if it is in the vacuum */
			nodeName  = topnode;
			nodeName += "/";
			nodeName += STSVACUUMFODLER;
			nodeName += "/";
			nodeName += actualStationName;
			nodeName += "_1";

			/* change to the node for the volume, if it exists */
			foundNodeName = gGeoManager->cd(nodeName.c_str());

			/* the node for the volume is not in the vacuum */
			if (!foundNodeName) {

				/* get name from node for volume, if it is not in the vacuum */
				nodeName  = topnode;
				nodeName += "/";
				nodeName += actualStationName;
				nodeName += "_1";

				/* change to the node for the volume, if it exists */
				foundNodeName = gGeoManager->cd(nodeName.c_str());

			}

			if (foundNodeName)
			/* get the node for the volume */
				node = gGeoManager->GetCurrentNode();
			else
				throw detectorNodeNotFoundError(nodeName);

			/* get the matrix from node for volume */
			if (node != NULL)
				matrix = node->GetMatrix();

			/* get the translation from the matrix for node for volume */
			if (matrix != NULL)
				translation = matrix->GetTranslation();
			else
				throw detectorMatrixNotFoundError(nodeName);
		
			/* set the distance for the detector */
			if (translation != NULL)
				actualStation.setDistance(translation[2]);
			else
				throw detectorTranslationNotFoundError(nodeName);

			/* set the type of the station */
			actualStation.removeType();

			/* add the station */
			data.addStation(actualStation);

#ifdef DETECTORINFO

			std::cout << actualStationName      << ":"                              << std::endl;
			std::cout << " nodeName: "          << nodeName                         << std::endl;
			std::cout << " translation: "       << translation[0]                   << " , ";
			std::cout << translation[1]         << " , " << translation[2]          << std::endl;
			std::cout << " id: "                << actualStation.getId()            << std::endl;
			std::cout << " distance: "          << actualStation.getDistance()      << std::endl;
			std::cout << " isMaps: "            << actualStation.isMapsType()       << std::endl;
			std::cout << " isHybrid: "          << actualStation.isHybridType()     << std::endl;
			std::cout << " isStrip: "           << actualStation.isStripType()      << std::endl;
			std::cout << std::endl;

#endif

		}

	}
	else {

		geoManagerNotFoundWarningMsg* geoManagerNotFound = new geoManagerNotFoundWarningMsg();
		geoManagerNotFound->warningMsg();
		if(geoManagerNotFound != NULL) {
			delete geoManagerNotFound;
			geoManagerNotFound = NULL;
		}

	}

	if (numberOfStations > 0)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * Method initializes special parts of the object.				*
 ****************************************************************/

void inputRoot::initSpecial(FairRootManager* manager) {

	std::string stsHitBranch;

	stsHitBranch.clear();

#ifdef HITCOMPATIBILITY

	if (readMapsHits)
		stsHitBranch += STSMAPSHITBRANCH;
	if (readHybridHits) {
		if (readMapsHits)
			stsHitBranch += ", ";
		stsHitBranch += STSHYBRIDHITBRANCH;
	}
	if (readStripHits) {
		if ((readMapsHits) || (readHybridHits))
			stsHitBranch += ", ";
		stsHitBranch += STSSTRIPHITBRANCH;
	}

#else

	if (readMapsHits)
		stsHitBranch += MVDHITBRANCH;
	if (readHybridHits || readStripHits) {
		if (readMapsHits)
			stsHitBranch += ", ";
		stsHitBranch += STSHITBRANCH;
	}

#endif

	if (getenv("VMCWORKDIR") != NULL) {

		geometryFileDirectory = getenv("VMCWORKDIR");

	}
	else {

		geometryFileDirectory = getGeometryFileRelativePath();

#ifdef ENABLEALLWARNINGS

		cbmrootEnvironmentVariableNotFoundWarningMsg* cbmrootEnvironmentVariableNotFound = new cbmrootEnvironmentVariableNotFoundWarningMsg(geometryFileDirectory);
		cbmrootEnvironmentVariableNotFound->warningMsg();
		if(cbmrootEnvironmentVariableNotFound != NULL) {
			delete cbmrootEnvironmentVariableNotFound;
			cbmrootEnvironmentVariableNotFound = NULL;
		}

#endif

	}

	geometryFileDirectory += "/geometry/";

	/* get pointer to the list of mc tracks */
	inputTracks = (TClonesArray*) manager->GetObject(MCTRACKBRANCH);

	if (inputTracks == NULL)
		throw noTrackInFileError(MCTRACKBRANCH);

	if (readPointsFromFile) {

		/* get pointer to the list of sts points */

#ifdef HITCOMPATIBILITY

		if (readMapsHits || readHybridHits || readStripHits) {
	
			inputStsPoints = (TClonesArray*) manager->GetObject(STSPOINTBRANCH);

			if (inputStsPoints == NULL)
				throw noPointInFileError(STSPOINTBRANCH);

		}

#else

		if (readMapsHits) {
	
			inputMvdPoints = (TClonesArray*) manager->GetObject(MVDPOINTBRANCH);

			if (inputMvdPoints == NULL)
				throw noPointInFileError(MVDPOINTBRANCH);

		}

		if (readHybridHits || readStripHits) {
	
			inputStsPoints = (TClonesArray*) manager->GetObject(STSPOINTBRANCH);

			if (inputStsPoints == NULL)
				throw noPointInFileError(STSPOINTBRANCH);

		}

#endif

	}

	if (readHitsFromFile) {

#ifdef HITCOMPATIBILITY

		if (readMapsHits)

#ifdef INPUTCOMPATIBILITY

			inputMapsHits   = (TClonesArray*) manager->GetRegisteredObject(STSMAPSHITBRANCH);

#else

			inputMapsHits   = (TClonesArray*) manager->GetObject(STSMAPSHITBRANCH);

#endif

		if (readHybridHits)

#ifdef INPUTCOMPATIBILITY

			inputHybridHits = (TClonesArray*) manager->GetRegisteredObject(STSHYBRIDHITBRANCH);

#else

			inputHybridHits = (TClonesArray*) manager->GetObject(STSHYBRIDHITBRANCH);

#endif

		if (readStripHits)

#ifdef INPUTCOMPATIBILITY

			inputStripHits  = (TClonesArray*) manager->GetRegisteredObject(STSSTRIPHITBRANCH);

#else

			inputStripHits  = (TClonesArray*) manager->GetObject(STSSTRIPHITBRANCH);

#endif

#else

		if (readMapsHits)

#ifdef INPUTCOMPATIBILITY

			inputMvdHits   = (TClonesArray*) manager->GetRegisteredObject(MVDHITBRANCH);

#else

			inputMvdHits   = (TClonesArray*) manager->GetObject(MVDHITBRANCH);

#endif

		if (readHybridHits || readStripHits)

#ifdef INPUTCOMPATIBILITY

			inputStsHits = (TClonesArray*) manager->GetRegisteredObject(STSHITBRANCH);

#else

			inputStsHits = (TClonesArray*) manager->GetObject(STSHITBRANCH);

#endif

#endif

		if ((inputMvdHits == NULL) && (inputStsHits == NULL) && (inputMapsHits == NULL) && (inputHybridHits == NULL) && (inputStripHits == NULL)) {

			noHitWarningMsg* noHit = new noHitWarningMsg(stsHitBranch, STSPOINTBRANCH);
			noHit->warningMsg();
			if(noHit != NULL) {
				delete noHit;
				noHit = NULL;
			}

		}

	}

}

/****************************************************************
 * Method reads the data from the file.							*
 ****************************************************************/

void inputRoot::read(unsigned int event, TClonesArray* mvdHitArray, TClonesArray* stsHitArray, TClonesArray* mHitArray, TClonesArray* hHitArray, TClonesArray* sHitArray) {

	bool readEventFromRootManager;

	addDictionaries();

	if (readPointsFromFile) {

		if (readHitsFromFile) {

			disableAutomaticReadHitsWarningMsg* disableAutomaticReadHits = new disableAutomaticReadHitsWarningMsg();
			disableAutomaticReadHits->warningMsg();
			if(disableAutomaticReadHits != NULL) {
				delete disableAutomaticReadHits;
				disableAutomaticReadHits = NULL;
			}

		}
		else {

#ifdef HITCOMPATIBILITY

			if (readMapsHits)
				inputMapsHits   = mHitArray;
			if (readHybridHits)
				inputHybridHits = hHitArray;
			if (readStripHits)
				inputStripHits  = sHitArray;

#else

			if (readMapsHits)
				inputMvdHits    = mvdHitArray;
			if (readHybridHits || readStripHits)
				inputStsHits    = stsHitArray;

#endif

		}

		readEventFromRootManager = true;

	}
	else {

		noHitToPointWarningMsg* noHitToPoint = new noHitToPointWarningMsg();
		noHitToPoint->warningMsg();
		if(noHitToPoint != NULL) {
			delete noHitToPoint;
			noHitToPoint = NULL;
		}

		if (readHitsFromFile) {

			disableAutomaticReadHitsWarningMsg* disableAutomaticReadHits = new disableAutomaticReadHitsWarningMsg();
			disableAutomaticReadHits->warningMsg();
			if(disableAutomaticReadHits != NULL) {
				delete disableAutomaticReadHits;
				disableAutomaticReadHits = NULL;
			}

			readEventFromRootManager = true;

		}
		else {

#ifdef HITCOMPATIBILITY

			if (readMapsHits)
				inputMapsHits   = mHitArray;
			if (readHybridHits)
				inputHybridHits = hHitArray;
			if (readStripHits)
				inputStripHits  = sHitArray;

#else

			if (readMapsHits)
				inputMvdHits    = mvdHitArray;
			if (readHybridHits || readStripHits)
				inputStsHits    = stsHitArray;

#endif

			readEventFromRootManager = false;

		}

	}

#if (ARCHITECTURE == STANDALONE)

	if (readEventFromRootManager) {

		/* get pointer to ROOT manager */
		FairRootManager* manager = FairRootManager::Instance();
		if (manager == NULL)
			throw cannotAccessRootManagerError(INPUTLIB);

		if (!manager->getEvent(event))
			throw eventNotFoundError(event);

	}

#endif

}

/**
 * Method returns the relative path for the geometry file.
 */

std::string inputRoot::getGeometryFileRelativePath() {

	return ".";

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputRoot::inputRoot() 
  : inputData(), 
    inputStsPoints (NULL),
    inputMvdPoints (NULL),
    inputMapsHits  (NULL),
    inputHybridHits(NULL),
    inputStripHits (NULL),
    inputMvdHits   (NULL),
    inputStsHits   (NULL),
    inputTracks    (NULL),
    detectorFileName(),
    numberOfVolumesInfrontOfSTS(0),
    geometryFileDirectory(),
    topnode(),
    hitsProduced(false),
    readPointsFromFile(false),
    readHitsFromFile(false),
    readMapsHits   (false),
    readHybridHits (false),
    readStripHits  (false)
{
  /*
	inputTracks        = NULL;
	inputStsPoints     = NULL;
	inputMvdPoints     = NULL;
	inputMapsHits      = NULL;
	inputStripHits     = NULL;
	inputHybridHits    = NULL;
	inputStsHits       = NULL;
	inputMvdHits       = NULL;
	detectorFileName.clear();
	numberOfVolumesInfrontOfSTS = 0;
	geometryFileDirectory.clear();
	topnode.clear();
	hitsProduced       = false;
	readPointsFromFile = false;
	readHitsFromFile   = false;
	readMapsHits       = false;
	readHybridHits     = false;
	readStripHits      = false;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputRoot::inputRoot(bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputData(detMask), 
    inputStsPoints (NULL),
    inputMvdPoints (NULL),
    inputMapsHits  (NULL),
    inputHybridHits(NULL),
    inputStripHits (NULL),
    inputMvdHits   (NULL),
    inputStsHits   (NULL),
    inputTracks    (NULL),
    detectorFileName(),
    numberOfVolumesInfrontOfSTS(0),
    geometryFileDirectory(),
    topnode(),
    hitsProduced(false),
    readPointsFromFile(enableJustPoints),
    readHitsFromFile(enableHitsFromFile),
    readMapsHits   (enableMapsHits),
    readHybridHits (enableHybridHits),
    readStripHits  (enableStripHits)
{

  /*	inputTracks        = NULL;
	inputStsPoints     = NULL;
	inputMvdPoints     = NULL;
	inputMapsHits      = NULL;
	inputStripHits     = NULL;
	inputHybridHits    = NULL;
	inputStsHits       = NULL;
	inputMvdHits       = NULL;
	detectorFileName.clear();
	numberOfVolumesInfrontOfSTS = 0;
	geometryFileDirectory.clear();
	topnode.clear();
	hitsProduced       = false;
	readPointsFromFile = enableJustPoints;
	readHitsFromFile   = enableHitsFromFile;
	readMapsHits       = enableMapsHits;
	readHybridHits     = enableHybridHits;
	readStripHits      = enableStripHits;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputRoot::inputRoot(const char* name, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputData(detMask, hitProducer),
    inputStsPoints (NULL),
    inputMvdPoints (NULL),
    inputMapsHits  (NULL),
    inputHybridHits(NULL),
    inputStripHits (NULL),
    inputMvdHits   (NULL),
    inputStsHits   (NULL),
    inputTracks    (NULL),
    detectorFileName(),
    numberOfVolumesInfrontOfSTS(0),
    geometryFileDirectory(),
    topnode(),
    hitsProduced(false),
    readPointsFromFile(enableJustPoints),
    readHitsFromFile(enableHitsFromFile),
    readMapsHits   (enableMapsHits),
    readHybridHits (enableHybridHits),
    readStripHits  (enableStripHits)
{

	FairRootManager* manager;
	std::string     fileName;

	/* get pointer to ROOT manager */
	manager  = FairRootManager::Instance();
	if (manager == NULL)
		throw cannotAccessRootManagerError(INPUTLIB);

	fileName = generateFileName(name);

        manager->SetInputFile(fileName.c_str());
        manager->OpenSignalChain();
//        manager->OpenInChain();
//	manager->OpenInFile(fileName.c_str());

/*
	inputTracks        = NULL;
	inputStsPoints     = NULL;
	inputMvdPoints     = NULL;
	inputMapsHits      = NULL;
	inputStripHits     = NULL;
	inputHybridHits    = NULL;
	inputStsHits       = NULL;
	inputMvdHits       = NULL;
	detectorFileName.clear();
	numberOfVolumesInfrontOfSTS = 0;
	geometryFileDirectory.clear();
	topnode.clear();
	hitsProduced       = false;
	readPointsFromFile = enableJustPoints;
	readHitsFromFile   = enableHitsFromFile;
	readMapsHits       = enableMapsHits;
	readHybridHits     = enableHybridHits;
	readStripHits      = enableStripHits;
*/
}
inputRoot::inputRoot(const char* _detectorFileName, unsigned short _numberOfVolumesInfrontOfSTS, bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputData(detMask), 
    inputStsPoints (NULL),
    inputMvdPoints (NULL),
    inputMapsHits  (NULL),
    inputHybridHits(NULL),
    inputStripHits (NULL),
    inputMvdHits   (NULL),
    inputStsHits   (NULL),
    inputTracks    (NULL),
    detectorFileName(_detectorFileName),
    numberOfVolumesInfrontOfSTS(_numberOfVolumesInfrontOfSTS),
    geometryFileDirectory(),
    topnode(),
    hitsProduced(false),
    readPointsFromFile(enableJustPoints),
    readHitsFromFile(enableHitsFromFile),
    readMapsHits   (enableMapsHits),
    readHybridHits (enableHybridHits),
    readStripHits  (enableStripHits)
{

  /*	inputTracks                       = NULL;
	inputStsPoints                    = NULL;
	inputMvdPoints                    = NULL;
	inputMapsHits                     = NULL;
	inputStripHits                    = NULL;
	inputHybridHits                   = NULL;
	inputStsHits                      = NULL;
	inputMvdHits                      = NULL;
	this->detectorFileName            = _detectorFileName;
	this->numberOfVolumesInfrontOfSTS = _numberOfVolumesInfrontOfSTS;
	geometryFileDirectory.clear();
	topnode.clear();
	hitsProduced                      = false;
	readPointsFromFile                = enableJustPoints;
	readHitsFromFile                  = enableHitsFromFile;
	readMapsHits                      = enableMapsHits;
	readHybridHits                    = enableHybridHits;
	readStripHits                     = enableStripHits;
  */
}
inputRoot::inputRoot(const char* name, const char* _detectorFileName, unsigned short _numberOfVolumesInfrontOfSTS, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputData(detMask, hitProducer),
    inputStsPoints (NULL),
    inputMvdPoints (NULL),
    inputMapsHits  (NULL),
    inputHybridHits(NULL),
    inputStripHits (NULL),
    inputMvdHits   (NULL),
    inputStsHits   (NULL),
    inputTracks    (NULL),
    detectorFileName(_detectorFileName),
    numberOfVolumesInfrontOfSTS(_numberOfVolumesInfrontOfSTS),
    geometryFileDirectory(),
    topnode(),
    hitsProduced(false),
    readPointsFromFile(enableJustPoints),
    readHitsFromFile(enableHitsFromFile),
    readMapsHits   (enableMapsHits),
    readHybridHits (enableHybridHits),
    readStripHits  (enableStripHits)
 {

	FairRootManager* manager;
	std::string     fileName;

	/* get pointer to ROOT manager */
	manager  = FairRootManager::Instance();
	if (manager == NULL)
		throw cannotAccessRootManagerError(INPUTLIB);

	fileName = generateFileName(name);

        manager->SetInputFile(fileName.c_str());
        manager->OpenSignalChain();
//	manager->OpenInFile(fileName.c_str());

/*	inputTracks                       = NULL;
	inputStsPoints                    = NULL;
	inputMvdPoints                    = NULL;
	inputMapsHits                     = NULL;
	inputStripHits                    = NULL;
	inputHybridHits                   = NULL;
	inputStsHits                      = NULL;
	inputMvdHits                      = NULL;
	this->detectorFileName            = _detectorFileName;
	this->numberOfVolumesInfrontOfSTS = _numberOfVolumesInfrontOfSTS;
	geometryFileDirectory.clear();
	topnode.clear();
	hitsProduced                      = false;
	readPointsFromFile                = enableJustPoints;
	readHitsFromFile                  = enableHitsFromFile;
	readMapsHits                      = enableMapsHits;
	readHybridHits                    = enableHybridHits;
	readStripHits                     = enableStripHits;
*/
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputRoot::~inputRoot() {

	/* delete allocated space */
	if (hitsProduced) {
		if (inputMapsHits != NULL) {
			inputMapsHits->RemoveAll();
			delete inputMapsHits;
			inputMapsHits = NULL;
		}
		if (inputStripHits != NULL) {
			inputStripHits->RemoveAll();
			delete inputStripHits;
			inputStripHits = NULL;
		}
		if (inputHybridHits != NULL) {
			inputHybridHits->RemoveAll();
			delete inputHybridHits;
			inputHybridHits = NULL;
		}
		if (inputStsHits != NULL) {
			inputStsHits->RemoveAll();
			delete inputStsHits;
			inputStsHits = NULL;
		}
		if (inputMvdHits != NULL) {
			inputMvdHits->RemoveAll();
			delete inputMvdHits;
			inputMvdHits = NULL;
		}
	}

}

/****************************************************************
 * method initializes the object.								*
 ****************************************************************/

void inputRoot::init() {

	FairRootManager* manager;
	TGeoNode*       topGeoNode;

	inputTracks     = NULL;
	inputStsPoints  = NULL;
	inputMvdPoints  = NULL;
	inputMapsHits   = NULL;
	inputStripHits  = NULL;
	inputHybridHits = NULL;
	inputStsHits    = NULL;
	inputMvdHits    = NULL;
	hitsProduced    = false;
	manager         = NULL;

	data.init();

	/* get pointer to the topnode of the geoManager */
	if (gGeoManager != NULL) {

		topGeoNode = gGeoManager->GetTopNode();

		if (topGeoNode != NULL)
			topnode = TString(topGeoNode->GetName()).Data();
		else
			topnode.clear();

	}
	else
		topnode.clear();

	/* get pointer to ROOT manager */
	manager     = FairRootManager::Instance();
	if (manager == NULL)
		throw cannotAccessRootManagerError(INPUTLIB);

	initSpecial(manager);

	data.initDefaultDetector();

}
void inputRoot::init(const char* name, int hitProducer) {

	FairRootManager* manager;
	std::string     fileName;

	/* get pointer to ROOT manager */
	manager  = FairRootManager::Instance();
	if (manager == NULL)
		throw cannotAccessRootManagerError(INPUTLIB);

	fileName = generateFileName(name);

        manager->SetInputFile(fileName.c_str());
        manager->OpenSignalChain();
//	manager->OpenInFile(fileName.c_str());

	inputData::init(hitProducer);

	init();

}
void inputRoot::init(const char* _detectorFileName, unsigned int _numberOfVolumesInfrontOfSTS) {

	this->detectorFileName            = _detectorFileName;
	this->numberOfVolumesInfrontOfSTS = _numberOfVolumesInfrontOfSTS;

}
void inputRoot::init(const char* name, int hitProducer, const char* _detectorFileName, unsigned int _numberOfVolumesInfrontOfSTS) {

	init(name, hitProducer);
	init(_detectorFileName, _numberOfVolumesInfrontOfSTS);

}
void inputRoot::init(bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) {

	inputData::init(detMask);

	readPointsFromFile = enableJustPoints;
	readHitsFromFile   = enableHitsFromFile;
	readMapsHits       = enableMapsHits;
	readHybridHits     = enableHybridHits;
	readStripHits      = enableStripHits;

}
void inputRoot::init(const char* name, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) {

	inputData::init(detMask);
	init(name, hitProducer);

	readPointsFromFile = enableJustPoints;
	readHitsFromFile   = enableHitsFromFile;
	readMapsHits       = enableMapsHits;
	readHybridHits     = enableHybridHits;
	readStripHits      = enableStripHits;

}
void inputRoot::init(const char* _detectorFileName, unsigned short _numberOfVolumesInfrontOfSTS, bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) {

	init(_detectorFileName, _numberOfVolumesInfrontOfSTS);
	init(detMask, enableJustPoints, enableHitsFromFile, enableMapsHits, enableHybridHits, enableStripHits);

}
void inputRoot::init(const char* name, const char* _detectorFileName, unsigned short _numberOfVolumesInfrontOfSTS, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) {

	init(_detectorFileName, _numberOfVolumesInfrontOfSTS);
	init(name, detMask, hitProducer, enableJustPoints, enableHitsFromFile, enableMapsHits, enableHybridHits, enableStripHits);

}

/****************************************************************
 * method reads the source data from a file based on the		*
 * CbmStsMapsHit-class, CbmStsHybridHit-class and				*
 * CbmStsStripHit-class											*
 ****************************************************************/

void inputRoot::readDataSource(unsigned int event, TClonesArray* mHitArray, TClonesArray* hHitArray, TClonesArray* sHitArray) {

	int                      i;
	bool                     zeroTracks;
	bool                     zeroMapsHits;
	bool                     zeroHybridHits;
	bool                     zeroStripHits;
	unsigned int             numberOfTracks;
	unsigned int             numberOfHits;
	FairHitProducer*          hitProducer;
	CbmMCTrack*              inputTrack;
	CbmStsPoint*             inputStsPoint;
	CbmPixelHit*                  inputHit;
	bool                     avoidPdgCode;

#ifdef TRACKINFO

	trackfinderInputTrack* track;

#endif

#ifdef HITINFO

	trackfinderInputHit*   hit;
	magneticFieldValue     magnetField;

#endif

	if (hitsProduced) {
		if (inputMapsHits != NULL) {
			inputMapsHits->RemoveAll();
			delete inputMapsHits;
			inputMapsHits = NULL;
		}
		if (inputStripHits != NULL) {
			inputStripHits->RemoveAll();
			delete inputStripHits;
			inputStripHits = NULL;
		}
		if (inputHybridHits != NULL) {
			inputHybridHits->RemoveAll();
			delete inputHybridHits;
			inputHybridHits = NULL;
		}
		if (inputStsHits != NULL) {
			inputStsHits->RemoveAll();
			delete inputStsHits;
			inputStsHits = NULL;
		}
		if (inputMvdHits != NULL) {
			inputMvdHits->RemoveAll();
			delete inputMvdHits;
			inputMvdHits = NULL;
		}
	}

	/* READ THE TRACKS AND HITS FROM THE STSDETECTOR */

	read(event, NULL, NULL, mHitArray, hHitArray, sHitArray);

	if ((inputMapsHits == NULL) && (inputHybridHits == NULL) && (inputStripHits == NULL)) {

		takeMyHitProducerWarningMsg* takeMyHitProducer = new takeMyHitProducerWarningMsg();
		takeMyHitProducer->warningMsg();
		if(takeMyHitProducer != NULL) {
			delete takeMyHitProducer;
			takeMyHitProducer = NULL;
		}

#ifdef HITCOMPATIBILITY

		inputMapsHits   = new TClonesArray("CbmStsMapsHit");
		inputStripHits  = new TClonesArray("CbmStsStripHit");
		inputHybridHits = new TClonesArray("CbmStsHybridHit");

#else

		throw functionIsDeprecatedError(INPUTLIB);

#endif

		hitsProduced    = true;
		hitProducer     = new FairHitProducer(typeOfHitProducer, data.getDetectorPointer());
		hitProducer->produceOld(inputMapsHits, inputStripHits, inputHybridHits, inputStsPoints);
		delete hitProducer;

	}

	numberOfTracks = 0;

	if (inputTracks != NULL) {
		numberOfTracks += inputTracks->GetEntries();
		if (inputTracks->GetEntries() == 0)
			zeroTracks = true;
		else
			zeroTracks = false;
	}
	else
		zeroTracks     = true;

	numberOfHits   = 0;

	if (inputMapsHits != NULL) {
		numberOfHits += inputMapsHits->GetEntries();
		if (inputMapsHits->GetEntries() == 0)
			zeroMapsHits = true;
		else
			zeroMapsHits = false;
	}
	else
		zeroMapsHits = true;
	if (inputHybridHits != NULL) {
		numberOfHits += inputHybridHits->GetEntries();
		if (inputHybridHits->GetEntries() == 0)
			zeroHybridHits = true;
		else
			zeroHybridHits = false;
	}
	else
		zeroHybridHits = true;
	if (inputStripHits != NULL) {
		numberOfHits += inputStripHits->GetEntries();
		if (inputStripHits->GetEntries() == 0)
			zeroStripHits = true;
		else
			zeroStripHits = false;
	}
	else
		zeroStripHits = true;

	if ((zeroTracks) || (zeroMapsHits && zeroHybridHits && zeroStripHits))
		throw zeroTracksOrHitsError(numberOfTracks, numberOfHits);

	/* SETTING THE INPUT */

	data.setEventNumber(event);

	/* ERASING DATA STORAGE */

	data.removeAllTracks();
	data.removeAllHits();

	if (inputTracks != NULL) {

		for (i = 0; i < inputTracks->GetEntries(); i++ ) {

			inputTrack = (CbmMCTrack*)inputTracks->At(i);
			if (inputTrack == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);


			if (inputTrack->GetNPoints(kSTS) > 0) {

				/* what are these particles????? */
				avoidPdgCode = false;
				for (int j = 0; j < numberOfAvoidingPdgCodes; j++) {
					if (inputTrack->GetPdgCode() == avoidingPdgCodes[j]) {
						avoidPdgCode = true;
						break;
					}
				}

				if (!avoidPdgCode)
					data.addTrack(inputTrack, i, TDatabasePDG::Instance()->GetParticle(inputTrack->GetPdgCode())->Charge() / 3.0);

			}

		}

	}

	if (inputMapsHits != NULL) {

		for (i = 0; i < inputMapsHits->GetEntries(); i++) {

			inputHit = (CbmPixelHit*)inputMapsHits->At(i);
			if (inputHit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			if ((inputHit->GetRefId() < 0) || (inputStsPoints == NULL))
				inputStsPoint = NULL;
			else
				inputStsPoint = (CbmStsPoint*)inputStsPoints->At(inputHit->GetRefId());

/* This is just used because the type of the station must be derived from the hits */
			if (data.getDetectorPointer()->getStationPointerById(inputHit->GetAddress()) != NULL)
				data.getDetectorPointer()->getStationPointerById(inputHit->GetAddress())->setMapsType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */

			if (data.getDetector().getStationById(inputHit->GetAddress()).isMapsType()) {

				if (!data.getDetector().getStationById(inputHit->GetAddress()).isMasked()) {

					data.addHit(inputHit->GetAddress(), inputHit, (FairMCPoint*)inputStsPoint, i);

				}

			}
			else {

				hitWithWrongStationFoundWarningMsg* hitWithWrongStationFound = new hitWithWrongStationFoundWarningMsg();
				hitWithWrongStationFound->warningMsg();
				if(hitWithWrongStationFound != NULL) {
					delete hitWithWrongStationFound;
					hitWithWrongStationFound = NULL;
				}

			}

		}

	}
	if (inputHybridHits != NULL) {

		for (i = 0; i < inputHybridHits->GetEntries(); i++) {

			inputHit = (CbmPixelHit*)inputHybridHits->At(i);
			if (inputHit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			if ((inputHit->GetRefId() < 0) || (inputStsPoints == NULL))
				inputStsPoint = NULL;
			else
				inputStsPoint = (CbmStsPoint*)inputStsPoints->At(inputHit->GetRefId());

/* This is just used because the type of the station must be derived from the hits */
			if (data.getDetectorPointer()->getStationPointerById(inputHit->GetAddress()) != NULL)
				data.getDetectorPointer()->getStationPointerById(inputHit->GetAddress())->setHybridType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */

			if (data.getDetector().getStationById(inputHit->GetAddress()).isHybridType()) {

				if (!data.getDetector().getStationById(inputHit->GetAddress()).isMasked()) {

					data.addHit(inputHit->GetAddress(), inputHit, (FairMCPoint*)inputStsPoint, i);

				}

			}
			else {

				hitWithWrongStationFoundWarningMsg* hitWithWrongStationFound = new hitWithWrongStationFoundWarningMsg();
				hitWithWrongStationFound->warningMsg();
				if(hitWithWrongStationFound != NULL) {
					delete hitWithWrongStationFound;
					hitWithWrongStationFound = NULL;
				}

			}

		}
	}
	if (inputStripHits != NULL) {

		for (i = 0; i < inputStripHits->GetEntries(); i++) {

			inputHit = (CbmPixelHit*)inputStripHits->At(i);
			if (inputHit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			if ((inputHit->GetRefId() < 0) || (inputStsPoints == NULL))
				inputStsPoint = NULL;
			else
				inputStsPoint = (CbmStsPoint*)inputStsPoints->At(inputHit->GetRefId());

/* This is just used because the type of the station must be derived from the hits */
			if (data.getDetectorPointer()->getStationPointerById(inputHit->GetAddress()) != NULL)
				data.getDetectorPointer()->getStationPointerById(inputHit->GetAddress())->setStripType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */

			if (data.getDetector().getStationById(inputHit->GetAddress()).isStripType()) {

				if (!data.getDetector().getStationById(inputHit->GetAddress()).isMasked()) {

					data.addHit(inputHit->GetAddress(), inputHit, (FairMCPoint*)inputStsPoint, i);

				}

			}
			else {

				hitWithWrongStationFoundWarningMsg* hitWithWrongStationFound = new hitWithWrongStationFoundWarningMsg();
				hitWithWrongStationFound->warningMsg();
				if(hitWithWrongStationFound != NULL) {
					delete hitWithWrongStationFound;
					hitWithWrongStationFound = NULL;
				}

			}

		}

	}

	data.finalizeHitTrackAssignment(readPointsFromFile);

	if ((data.getNumberOfTracks() == 0) || (data.getNumberOfHits() == 0))
		throw zeroTracksOrHitsError(data.getNumberOfTracks(), data.getNumberOfHits());

#ifdef TRACKINFO

	std::cout << std::endl;
	std::cout << "numberOfTracks: " << data.getNumberOfTracks() << std::endl;

	for (i = 0; i < data.getNumberOfTracks(); i++ ) {
	
		if ((i > 5) && (i < 10)) {
			
			track = data.getTrackByOrder(i);
			if (track == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			std::cout << "TrackId: "       << track->getTrackIndex()     << std::endl;
			std::cout << " Hits: "         << track->getNumberOfHits()   << std::endl;
			std::cout << " Points: "       << track->getNumberOfPoints() << std::endl;
			std::cout << " PdgCode: "      << track->getPdgCode()        << std::endl;
			std::cout << " Charge:    "    << track->getCharge()         << std::endl;

			std::cout << " momemtumX: "    << track->getMomX()           << std::endl;
			std::cout << " momentumY: "    << track->getMomY()           << std::endl;
			std::cout << " momentumZ: "    << track->getMomZ()           << std::endl;

			std::cout << std::endl;

		}

	}

#endif

#ifdef HITINFO

	std::cout << std::endl;
	std::cout << "numberOfHits: " << data.getNumberOfHits() << std::endl;

	for (i = 0; i < data.getNumberOfHits(); i++ ) {

		if ((i > 5) && (i < 10)) {

			hit = data.getHitByIndex(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			data.getMagneticField()->getFieldValues(hit, &magnetField);

			std::cout << "HitID: "        << hit->getHitIndex()               << std::endl;
			if (hit->getPoint() != NULL)
				std::cout << " PointID: " << hit->getHit()->GetRefIndex()     << std::endl;
			else
				std::cout << " PointID: missing"                              << std::endl;
			if (hit->getTrack() != NULL)
				std::cout << " TrackID: " << hit->getTrack()->getTrackIndex() << std::endl;
			else
				std::cout << " TrackID: missing"                              << std::endl;
			std::cout << " DetectorID: "  << hit->getStationId()              << std::endl;

			std::cout << " PosX: "        << hit->getPosX()                   << std::endl;
			std::cout << " PosY: "        << hit->getPosY()                   << std::endl;
			std::cout << " PosZ: "        << hit->getPosZ()                   << std::endl;

			std::cout << " FieldX: "      << magnetField.getFieldX()          << std::endl;
			std::cout << " FieldY: "      << magnetField.getFieldY()          << std::endl;
			std::cout << " FieldZ: "      << magnetField.getFieldZ()          << std::endl;
			
			std::cout << std::endl;

		}

	}

#endif

}

/****************************************************************
 * method reads the source data from a file based on the		*
 * CbmStsHit-class												*
 ****************************************************************/

void inputRoot::readDataSource(unsigned int event, TClonesArray* mvdHitArray, TClonesArray* stsHitArray) {

	int                      i;
	bool                     zeroTracks;
	bool                     zeroStsHits;
	bool                     zeroMvdHits;
	unsigned int             numberOfTracks;
	unsigned int             numberOfHits;
	FairHitProducer*          hitProducer;
	CbmMCTrack*              inputTrack;
	CbmStsPoint*             inputStsPoint;
	CbmMvdPoint*             inputMvdPoint;
	CbmStsHit*               inputStsHit;
	CbmMvdHit*               inputMvdHit;
	bool                     avoidPdgCode;

#ifdef TRACKINFO

	trackfinderInputTrack* track;

#endif

#ifdef HITINFO

	trackfinderInputHit*   hit;
	magneticFieldValue     magnetField;

#endif

	if (hitsProduced) {
		if (inputMapsHits != NULL) {
			inputMapsHits->RemoveAll();
			delete inputMapsHits;
			inputMapsHits = NULL;
		}
		if (inputStripHits != NULL) {
			inputStripHits->RemoveAll();
			delete inputStripHits;
			inputStripHits = NULL;
		}
		if (inputHybridHits != NULL) {
			inputHybridHits->RemoveAll();
			delete inputHybridHits;
			inputHybridHits = NULL;
		}
		if (inputStsHits != NULL) {
			inputStsHits->RemoveAll();
			delete inputStsHits;
			inputStsHits = NULL;
		}
		if (inputMvdHits != NULL) {
			inputMvdHits->RemoveAll();
			delete inputMvdHits;
			inputMvdHits = NULL;
		}
	}

	/* READ THE TRACKS AND HITS FROM THE STSDETECTOR */

	read(event, mvdHitArray, stsHitArray, NULL, NULL, NULL);

	if ((inputMvdHits == NULL) && (inputStsHits == NULL)) {

		takeMyHitProducerWarningMsg* takeMyHitProducer = new takeMyHitProducerWarningMsg();
		takeMyHitProducer->warningMsg();
		if(takeMyHitProducer != NULL) {
			delete takeMyHitProducer;
			takeMyHitProducer = NULL;
		}

#ifndef HITCOMPATIBILITY

		inputStsHits    = new TClonesArray("CbmStsHit");
		inputMvdHits    = new TClonesArray("CbmMvdHit");

#else

		throw functionIsDeprecatedError(INPUTLIB);

#endif

		hitsProduced = true;
		hitProducer  = new FairHitProducer(typeOfHitProducer, data.getDetectorPointer());
		hitProducer->produceNew(inputMvdHits, inputStsHits, inputMvdPoints, inputStsPoints);
		delete hitProducer;

	}

	numberOfTracks = 0;

	if (inputTracks != NULL) {
		numberOfTracks += inputTracks->GetEntries();
		if (inputTracks->GetEntries() == 0)
			zeroTracks = true;
		else
			zeroTracks = false;
	}
	else
		zeroTracks     = true;

	numberOfHits   = 0;

	if (inputStsHits != NULL) {
		numberOfHits += inputStsHits->GetEntries();
		if (inputStsHits->GetEntries() == 0)
			zeroStsHits = true;
		else
			zeroStsHits = false;
	}
	else
		zeroStsHits = true;
	if (inputMvdHits != NULL) {
		numberOfHits += inputMvdHits->GetEntries();
		if (inputMvdHits->GetEntries() == 0)
			zeroMvdHits = true;
		else
			zeroMvdHits = false;
	}
	else
		zeroMvdHits = true;

	if ((zeroTracks) || ((zeroStsHits) && (zeroMvdHits)))
		throw zeroTracksOrHitsError(numberOfTracks, numberOfHits);

	/* SETTING THE INPUT */

	data.setEventNumber(event);

	/* ERASING DATA STORAGE */

	data.removeAllTracks();
	data.removeAllHits();

	if (inputTracks != NULL) {

		for (i = 0; i < inputTracks->GetEntries(); i++ ) {

			inputTrack = (CbmMCTrack*)inputTracks->At(i);
			if (inputTrack == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);


			if (inputTrack->GetNPoints(kSTS) > 0) {

				/* what are these particles????? */
				avoidPdgCode = false;
				for (int j = 0; j < numberOfAvoidingPdgCodes; j++) {
					if (inputTrack->GetPdgCode() == avoidingPdgCodes[j]) {
						avoidPdgCode = true;
						break;
					}
				}

				if (!avoidPdgCode)
					data.addTrack(inputTrack, i, TDatabasePDG::Instance()->GetParticle(inputTrack->GetPdgCode())->Charge() / 3.0);

			}

		}

	}

	if (inputMvdHits != NULL) {

		for (i = 0; i < inputMvdHits->GetEntries(); i++) {

			inputMvdHit = (CbmMvdHit*)inputMvdHits->At(i);
			if (inputMvdHit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			if ((inputMvdHit->GetRefIndex() < 0) || (inputMvdPoints == NULL))
				inputMvdPoint = NULL;
			else
				inputMvdPoint = (CbmMvdPoint*)inputMvdPoints->At(inputMvdHit->GetRefIndex());

/* This is just used because the type of the station must be derived from the hits */
			if (data.getDetectorPointer()->getStationPointerById(inputMvdHit->GetStationNr()) != NULL)
				data.getDetectorPointer()->getStationPointerById(inputMvdHit->GetStationNr())->setMapsType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */

			if (data.getDetector().getStationById(inputMvdHit->GetStationNr()).isMapsType()) {

				if (!data.getDetector().getStationById(inputMvdHit->GetStationNr()).isMasked()) {

					data.addHit(inputMvdHit->GetStationNr(), (CbmPixelHit*)inputMvdHit, (FairMCPoint*)inputMvdPoint, i);

				}

			}
			else {

				hitWithWrongStationFoundWarningMsg* hitWithWrongStationFound = new hitWithWrongStationFoundWarningMsg();
				hitWithWrongStationFound->warningMsg();
				if(hitWithWrongStationFound != NULL) {
					delete hitWithWrongStationFound;
					hitWithWrongStationFound = NULL;
				}

			}

		}

	}

	if (inputStsHits != NULL) {

		for (i = 0; i < inputStsHits->GetEntries(); i++) {

			inputStsHit = (CbmStsHit*)inputStsHits->At(i);
			if (inputStsHit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			if ((inputStsHit->GetRefId() < 0) || (inputStsPoints == NULL))
				inputStsPoint = NULL;
			else
				inputStsPoint = (CbmStsPoint*)inputStsPoints->At(inputStsHit->GetRefId());

/* This is just used because the type of the station must be derived from the hits */
			if (data.getDetectorPointer()->getStationPointerById(inputStsHit->GetAddress()) != NULL)
				data.getDetectorPointer()->getStationPointerById(inputStsHit->GetAddress())->setHybridType(true);
/* Later the type is derived from the station itself. So it must be checked that the hit is correct */

			if (data.getDetector().getStationById(CbmStsAddress::GetElementId(inputStsHit->GetAddress(), kStsStation)).isHybridType() || data.getDetector().getStationById(CbmStsAddress::GetElementId(inputStsHit->GetAddress(), kStsStation)).isStripType()) {

				if (!data.getDetector().getStationById(CbmStsAddress::GetElementId(inputStsHit->GetAddress(), kStsStation)).isMasked()) {
					
					data.addHit(CbmStsAddress::GetElementId(inputStsHit->GetAddress(), kStsStation), (CbmPixelHit*)inputStsHit, (FairMCPoint*)inputStsPoint, i);

				}

			}
			else {

				hitWithWrongStationFoundWarningMsg* hitWithWrongStationFound = new hitWithWrongStationFoundWarningMsg();
				hitWithWrongStationFound->warningMsg();
				if(hitWithWrongStationFound != NULL) {
					delete hitWithWrongStationFound;
					hitWithWrongStationFound = NULL;
				}

			}

		}

	}

	data.finalizeHitTrackAssignment(readPointsFromFile);

	if ((data.getNumberOfTracks() == 0) || (data.getNumberOfHits() == 0))
		throw zeroTracksOrHitsError(data.getNumberOfTracks(), data.getNumberOfHits());

#ifdef TRACKINFO

	std::cout << std::endl;
	std::cout << "numberOfTracks: " << data.getNumberOfTracks() << std::endl;

	for (i = 0; i < data.getNumberOfTracks(); i++ ) {
	
		if ((i > 5) && (i < 10)) {
			
			track = data.getTrackByOrder(i);
			if (track == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			std::cout << "TrackId: "       << track->getTrackIndex()     << std::endl;
			std::cout << " Hits: "         << track->getNumberOfHits()   << std::endl;
			std::cout << " Points: "       << track->getNumberOfPoints() << std::endl;
			std::cout << " PdgCode: "      << track->getPdgCode()        << std::endl;
			std::cout << " Charge:    "    << track->getCharge()         << std::endl;

			std::cout << " momemtumX: "    << track->getMomX()           << std::endl;
			std::cout << " momentumY: "    << track->getMomY()           << std::endl;
			std::cout << " momentumZ: "    << track->getMomZ()           << std::endl;

			std::cout << std::endl;

		}

	}

#endif

#ifdef HITINFO

	std::cout << std::endl;
	std::cout << "numberOfHits: " << data.getNumberOfHits() << std::endl;

	for (i = 0; i < data.getNumberOfHits(); i++ ) {

		if ((i > 5) && (i < 10)) {

			hit = data.getHitByIndex(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(INPUTLIB);

			data.getMagneticField()->getFieldValues(hit, &magnetField);

			std::cout << "HitID: "        << hit->getHitIndex()               << std::endl;
			if (hit->getPoint() != NULL)
				std::cout << " PointID: " << hit->getHit()->GetRefIndex()     << std::endl;
			else
				std::cout << " PointID: missing"                              << std::endl;
			if (hit->getTrack() != NULL)
				std::cout << " TrackID: " << hit->getTrack()->getTrackIndex() << std::endl;
			else
				std::cout << " TrackID: missing"                              << std::endl;
			std::cout << " DetectorID: "  << hit->getStationId()              << std::endl;

			std::cout << " PosX: "        << hit->getPosX()                   << std::endl;
			std::cout << " PosY: "        << hit->getPosY()                   << std::endl;
			std::cout << " PosZ: "        << hit->getPosZ()                   << std::endl;

			std::cout << " FieldX: "      << magnetField.getFieldX()          << std::endl;
			std::cout << " FieldY: "      << magnetField.getFieldY()          << std::endl;
			std::cout << " FieldZ: "      << magnetField.getFieldZ()          << std::endl;
			
			std::cout << std::endl;

		}

	}

#endif

}

/****************************************************************
 * This method returns the size of the reserved memory for the	*
 * source data.													*
 ****************************************************************/

double inputRoot::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputData::getReservedSizeOfData(0);

	returnValue += sizeof(inputStsPoints);

#ifdef HITCOMPATIBILITY

	returnValue += sizeof(inputMapsHits);
	returnValue += sizeof(inputStripHits);
	returnValue += sizeof(inputHybridHits);

#else

	returnValue += sizeof(inputMvdPoints);
	returnValue += sizeof(inputMvdHits);
	returnValue += sizeof(inputStsHits);

#endif

	returnValue += sizeof(inputTracks);

	returnValue += sizeof(detectorFileName);
	returnValue += sizeof(numberOfVolumesInfrontOfSTS);
	returnValue += sizeof(geometryFileDirectory);
	returnValue += sizeof(topnode);
	returnValue += sizeof(hitsProduced);
	returnValue += sizeof(readPointsFromFile);
	returnValue += sizeof(readHitsFromFile);
	returnValue += sizeof(readMapsHits);
	returnValue += sizeof(readHybridHits);
	returnValue += sizeof(readStripHits);

	returnValue = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double inputRoot::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputData::getAllocatedSizeOfData(0);

	if (inputStsPoints != NULL)
		returnValue += inputStsPoints->Capacity()   * sizeof(CbmStsPoint);

#ifdef HITCOMPATIBILITY

	if (inputMapsHits != NULL)
		returnValue += inputMapsHits->Capacity()    * sizeof(CbmStsMapsHit);
	if (inputStripHits != NULL)
		returnValue += inputStripHits->Capacity()   * sizeof(CbmStsStripHit);
	if (inputHybridHits != NULL)
		returnValue += inputHybridHits->Capacity()  * sizeof(CbmStsHybridHit);

#else

	if (inputMvdPoints != NULL)
		returnValue   = inputMvdPoints->Capacity()  * sizeof(CbmMvdPoint);
	if (inputMvdHits != NULL)
		returnValue  += inputMvdHits->Capacity()    * sizeof(CbmMvdHit);
	if (inputStsHits != NULL)
		returnValue  += inputStsHits->Capacity()    * sizeof(CbmStsHit);

#endif

	if (inputTracks != NULL)
		returnValue += inputTracks->Capacity()      * sizeof(CbmMCTrack);

	returnValue += detectorFileName.capacity()      * sizeof(char);
	returnValue += geometryFileDirectory.capacity() * sizeof(char);
	returnValue += topnode.capacity()               * sizeof(char);

	returnValue = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for the		*
 * source data.													*
 ****************************************************************/

double inputRoot::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputData::getUsedSizeOfData(0);

	returnValue += sizeof(inputStsPoints);
	if (inputStsPoints != NULL)
		returnValue += inputStsPoints->GetEntries()  * sizeof(CbmStsPoint);

#ifdef HITCOMPATIBILITY

	returnValue += sizeof(inputMapsHits);
	if (inputMapsHits != NULL)
		returnValue += inputMapsHits->GetEntries()   * sizeof(CbmStsMapsHit);
	returnValue += sizeof(inputStripHits);
	if (inputStripHits != NULL)
		returnValue += inputStripHits->GetEntries()  * sizeof(CbmStsStripHit);
	returnValue += sizeof(inputHybridHits);
	if (inputHybridHits != NULL)
		returnValue += inputHybridHits->GetEntries() * sizeof(CbmStsHybridHit);

#else

	returnValue += sizeof(inputMvdPoints);
	if (inputMvdPoints != NULL)
		returnValue   = inputMvdPoints->GetEntries() * sizeof(CbmMvdPoint);
	returnValue += sizeof(inputMvdHits);
	if (inputMvdHits != NULL)
		returnValue  += inputMvdHits->GetEntries()   * sizeof(CbmMvdHit);
	returnValue += sizeof(inputStsHits);
	if (inputStsHits != NULL)
		returnValue  += inputStsHits->GetEntries()   * sizeof(CbmStsHit);

#endif

	returnValue += sizeof(inputTracks);
	if (inputTracks != NULL)
		returnValue += inputTracks->GetEntries()     * sizeof(CbmMCTrack);

	returnValue += sizeof(detectorFileName)      + detectorFileName.size()      * sizeof(char);
	returnValue += sizeof(numberOfVolumesInfrontOfSTS);
	returnValue += sizeof(geometryFileDirectory) + geometryFileDirectory.size() * sizeof(char);
	returnValue += sizeof(topnode)               + topnode.size()               * sizeof(char);
	returnValue += sizeof(hitsProduced);
	returnValue += sizeof(readPointsFromFile);
	returnValue += sizeof(readHitsFromFile);
	returnValue += sizeof(readMapsHits);
	returnValue += sizeof(readHybridHits);
	returnValue += sizeof(readStripHits);

	returnValue = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(inputRoot)
