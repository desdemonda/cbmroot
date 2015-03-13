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
// $Date: 2008-10-07 10:38:09 $
// $Revision: 1.10 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == CBMROOT)
	#include "FairRunAna.h"
	#include "FairRuntimeDb.h"
	#include "FairGeoVector.h"
	#include "FairGeoNode.h"
	#include "CbmGeoStsPar.h"
	#include "CbmStsStation.h"
	#include "CbmMvdGeoPar.h"
//	#include "CbmMvdStation.h"
#endif
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../include/inputError.h"
#include "../include/inputWarningMsg.h"
#include "../include/inputFRoot.h"


/****************************************************************
 * Method returns the relative path for the geometry file.		*
 ****************************************************************/

std::string getGeometryFileRelativePath() {

	return "../..";

}

/****************************************************************
 * Method accesses objects for the dictionarys.					*
 ****************************************************************/

void inputFRoot::addDictionaries() {

}

/****************************************************************
 * Method reads the variables based on the detector stations	*
 * by hand.														*
 ****************************************************************/

bool inputFRoot::readDetectorByInterface(CbmStsDigiScheme* scheme) {

	bool isMvdDetectorRead;
	bool isStsDetectorRead;
	bool isDetectorRead;

	data.removeDetector();

	if (readMapsHits)
		isMvdDetectorRead = readMvdDetectorByInterface(NULL);
	else
		isMvdDetectorRead = false;

	if (readHybridHits || readStripHits)
		isStsDetectorRead = readStsDetectorByInterface(scheme);
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
bool inputFRoot::readMvdDetectorByInterface(CbmStsDigiScheme* scheme) {

	bool                    isDetectorRead = false;
/*
#if (ARCHITECTURE == CBMROOT)

	CbmMvdStation*          station;
	trackfinderInputStation actualStation;

#endif

	if (scheme == NULL)
		isDetectorRead = false;
	else {

#if (ARCHITECTURE == CBMROOT)

		isDetectorRead = true;

		for (int i = 0; i < scheme->GetNStations(); i++) {

			station = scheme->GetStation(i);

			if (station != NULL) {

				/* set the id of the station */
//				actualStation.setId(station->GetStationNr());
				/* set the distance of the station */
//				actualStation.setDistance(station->GetZ());
				/* reset the type of the station */
//				actualStation.removeType();

				/* add the station */
/*				data.addStation(actualStation);

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
			else {

				cannotGetMvdStationWithIndexWarningMsg* cannotGetMvdStationWithIndex = new cannotGetMvdStationWithIndexWarningMsg(i);
				cannotGetMvdStationWithIndex->warningMsg();
				if(cannotGetMvdStationWithIndex != NULL) {
					delete cannotGetMvdStationWithIndex;
					cannotGetMvdStationWithIndex = NULL;
				}
				isDetectorRead = false;
				break;

			}

		}

#else

		isDetectorRead = false;

#endif

	}
*/
	return isDetectorRead;

}
bool inputFRoot::readStsDetectorByInterface(CbmStsDigiScheme* scheme) {

	bool                    isDetectorRead;

#if (ARCHITECTURE == CBMROOT)

	CbmStsStation*          station;
	trackfinderInputStation actualStation;

#endif

	if (scheme == NULL)
		isDetectorRead = false;
	else {

#if (ARCHITECTURE == CBMROOT)

		isDetectorRead = true;

		for (int i = 0; i < scheme->GetNStations(); i++) {

			station = scheme->GetStation(i);

			if (station != NULL) {

				/* set the id of the station */
				actualStation.setId(station->GetStationNr());
				/* set the distance of the station */
				actualStation.setDistance(station->GetZ());
				/* reset the type of the station */
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
			else {

				cannotGetStsStationWithIndexWarningMsg* cannotGetStsStationWithIndex = new cannotGetStsStationWithIndexWarningMsg(i);
				cannotGetStsStationWithIndex->warningMsg();
				if(cannotGetStsStationWithIndex != NULL) {
					delete cannotGetStsStationWithIndex;
					cannotGetStsStationWithIndex = NULL;
				}
				isDetectorRead = false;
				break;

			}

		}

#else

		isDetectorRead = false;

#endif

	}

	return isDetectorRead;

}

/****************************************************************
 * Method reads the variables based on the detector stations	*
 * by hand.														*
 ****************************************************************/

bool inputFRoot::readDetectorByHand() {

	bool isMvdDetectorRead;
	bool isStsDetectorRead;
	bool isDetectorRead;

	data.removeDetector();

	if (readMapsHits)
		isMvdDetectorRead = readMvdDetectorByHand();
	else
		isMvdDetectorRead = false;

	if (readHybridHits || readStripHits)
		isStsDetectorRead = readStsDetectorByHand();
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
bool inputFRoot::readMvdDetectorByHand() {

	bool                    isDetectorRead;

#if (ARCHITECTURE == CBMROOT)

	FairRunAna*              runAnalysisInstance;
	FairRuntimeDb*           runDataBase;
	CbmMvdGeoPar*           mvdParameterContainer;
	TObjArray*              mvdGeoSensitiveNodes;
	std::string             stationVolumeName;
	unsigned int            stationCounter;
	FairGeoNode*             stationNode;
	FairGeoVector            volumeIV;
	FairGeoVector            centerV;
	trackfinderInputStation actualStation;

	isDetectorRead        = true;
	runAnalysisInstance   = FairRunAna::Instance();
	if (runAnalysisInstance == NULL) {
		cannotGetRunAnaInstanceWarningMsg* cannotGetRunAnaInstance = new cannotGetRunAnaInstanceWarningMsg();
		cannotGetRunAnaInstance->warningMsg();
		if(cannotGetRunAnaInstance != NULL) {
			delete cannotGetRunAnaInstance;
			cannotGetRunAnaInstance = NULL;
		}
		isDetectorRead = false;
	}

	if (isDetectorRead) {
		runDataBase       = runAnalysisInstance->GetRuntimeDb();
		if (runDataBase == NULL) {
			cannotGetRuntimeDbWarningMsg* cannotGetRuntimeDb = new cannotGetRuntimeDbWarningMsg();
			cannotGetRuntimeDb->warningMsg();
			if(cannotGetRuntimeDb != NULL) {
				delete cannotGetRuntimeDb;
				cannotGetRuntimeDb = NULL;
			}
			isDetectorRead = false;
		}
	}

	if (isDetectorRead) {
		mvdParameterContainer = (CbmMvdGeoPar*)runDataBase->getContainer("CbmMvdGeoPar");
		if (mvdParameterContainer == NULL) {
			cannotGetMvdParameterContainerWarningMsg* cannotGetMvdParameterContainer = new cannotGetMvdParameterContainerWarningMsg();
			cannotGetMvdParameterContainer->warningMsg();
			if(cannotGetMvdParameterContainer != NULL) {
				delete cannotGetMvdParameterContainer;
				cannotGetMvdParameterContainer = NULL;
			}
			isDetectorRead = false;
		}
	}

	if (isDetectorRead) {
		mvdGeoSensitiveNodes  = mvdParameterContainer->GetGeoSensitiveNodes();
		if (mvdGeoSensitiveNodes == NULL) {
			cannotGetMvdGeoSensitiveNodesWarningMsg* cannotGetMvdGeoSensitiveNodes = new cannotGetMvdGeoSensitiveNodesWarningMsg();
			cannotGetMvdGeoSensitiveNodes->warningMsg();
			if(cannotGetMvdGeoSensitiveNodes != NULL) {
				delete cannotGetMvdGeoSensitiveNodes;
				cannotGetMvdGeoSensitiveNodes = NULL;
			}
			isDetectorRead = false;
		}
	}
		
	stationCounter = 0;
	while (1 == 1) {

		stationVolumeName = generateMvdStationVolumeName(stationCounter + 1);
		stationNode       = dynamic_cast<FairGeoNode*>(mvdGeoSensitiveNodes->FindObject(stationVolumeName.c_str()));

		if (stationNode == NULL)
			break;

		/* set the id of the station */
		actualStation.setId(stationNode->getMCid());
		/* set the distance of the station */
		if (stationNode->getLabTransform() != NULL)
			volumeIV = stationNode->getLabTransform()->getTranslation();
		else {
			cannotGetLabTransformWarningMsg* cannotGetLabTransform = new cannotGetLabTransformWarningMsg(stationNode->getMCid());
			cannotGetLabTransform->warningMsg();
			if(cannotGetLabTransform != NULL) {
				delete cannotGetLabTransform;
				cannotGetLabTransform = NULL;
			}
			volumeIV.setXYZ(0, 0, 0);
		}
		centerV  = stationNode->getCenterPosition().getTranslation();
		actualStation.setDistance(volumeIV.getZ() + centerV.getZ());
		/* reset the type of the station */
		actualStation.removeType();

		/* add the station */
		data.addStation(actualStation);

#ifdef DETECTORINFO

		std::cout << "actualStationCounter: "    << stationCounter                               << std::endl;
		std::cout << " id: "                     << actualStation.getId()           << std::endl;
		std::cout << " distance: "               << actualStation.getDistance()     << std::endl;
		std::cout << " isMaps: "                 << actualStation.isMapsType()      << std::endl;
		std::cout << " isHybrid: "               << actualStation.isHybridType()    << std::endl;
		std::cout << " isStrip: "                << actualStation.isStripType()     << std::endl;
		std::cout << std::endl;

#endif

		stationCounter++;

	}

	if (stationCounter == 0)
		isDetectorRead    = false;

#else

	isDetectorRead        = false;

#endif

	return isDetectorRead;

}
bool inputFRoot::readStsDetectorByHand() {

	bool                    isDetectorRead;

#if (ARCHITECTURE == CBMROOT)

	FairRunAna*              runAnalysisInstance;
	FairRuntimeDb*           runDataBase;
	CbmGeoStsPar*           stsParameterContainer;
	TObjArray*              stsGeoSensitiveNodes;
	std::string             stationVolumeName;
	unsigned int            stationCounter;
	FairGeoNode*             stationNode;
	FairGeoVector            volumeIV;
	FairGeoVector            centerV;
	trackfinderInputStation actualStation;

	isDetectorRead        = true;
	runAnalysisInstance   = FairRunAna::Instance();
	if (runAnalysisInstance == NULL) {
		cannotGetRunAnaInstanceWarningMsg* cannotGetRunAnaInstance = new cannotGetRunAnaInstanceWarningMsg();
		cannotGetRunAnaInstance->warningMsg();
		if(cannotGetRunAnaInstance != NULL) {
			delete cannotGetRunAnaInstance;
			cannotGetRunAnaInstance = NULL;
		}
		isDetectorRead = false;
	}

	if (isDetectorRead) {
		runDataBase       = runAnalysisInstance->GetRuntimeDb();
		if (runDataBase == NULL) {
			cannotGetRuntimeDbWarningMsg* cannotGetRuntimeDb = new cannotGetRuntimeDbWarningMsg();
			cannotGetRuntimeDb->warningMsg();
			if(cannotGetRuntimeDb != NULL) {
				delete cannotGetRuntimeDb;
				cannotGetRuntimeDb = NULL;
			}
			isDetectorRead = false;
		}
	}

	if (isDetectorRead) {
		stsParameterContainer = (CbmGeoStsPar*)runDataBase->getContainer("CbmGeoStsPar");
		if (stsParameterContainer == NULL) {
			cannotGetStsParameterContainerWarningMsg* cannotGetStsParameterContainer = new cannotGetStsParameterContainerWarningMsg();
			cannotGetStsParameterContainer->warningMsg();
			if(cannotGetStsParameterContainer != NULL) {
				delete cannotGetStsParameterContainer;
				cannotGetStsParameterContainer = NULL;
			}
			isDetectorRead = false;
		}
	}

	if (isDetectorRead) {
		stsGeoSensitiveNodes  = stsParameterContainer->GetGeoSensitiveNodes();
		if (stsGeoSensitiveNodes == NULL) {
			cannotGetStsGeoSensitiveNodesWarningMsg* cannotGetStsGeoSensitiveNodes = new cannotGetStsGeoSensitiveNodesWarningMsg();
			cannotGetStsGeoSensitiveNodes->warningMsg();
			if(cannotGetStsGeoSensitiveNodes != NULL) {
				delete cannotGetStsGeoSensitiveNodes;
				cannotGetStsGeoSensitiveNodes = NULL;
			}
			isDetectorRead = false;
		}
	}
		
	stationCounter = 0;
	while (1 == 1) {

		stationVolumeName = generateStsStationVolumeName(stationCounter + 1);
		stationNode       = dynamic_cast<FairGeoNode*>(stsGeoSensitiveNodes->FindObject(stationVolumeName.c_str()));

		if (stationNode == NULL)
			break;

		/* set the id of the station */
		actualStation.setId(stationNode->getMCid());
		/* set the distance of the station */
		if (stationNode->getLabTransform() != NULL)
			volumeIV = stationNode->getLabTransform()->getTranslation();
		else {
			cannotGetLabTransformWarningMsg* cannotGetLabTransform = new cannotGetLabTransformWarningMsg(stationNode->getMCid());
			cannotGetLabTransform->warningMsg();
			if(cannotGetLabTransform != NULL) {
				delete cannotGetLabTransform;
				cannotGetLabTransform = NULL;
			}
			volumeIV.setXYZ(0, 0, 0);
		}
		centerV  = stationNode->getCenterPosition().getTranslation();
		actualStation.setDistance(volumeIV.getZ() + centerV.getZ());
		/* reset the type of the station */
		actualStation.removeType();

		/* add the station */
		data.addStation(actualStation);

#ifdef DETECTORINFO

		std::cout << "actualStationCounter: "    << stationCounter                               << std::endl;
		std::cout << " id: "                     << actualStation.getId()           << std::endl;
		std::cout << " distance: "               << actualStation.getDistance()     << std::endl;
		std::cout << " isMaps: "                 << actualStation.isMapsType()      << std::endl;
		std::cout << " isHybrid: "               << actualStation.isHybridType()    << std::endl;
		std::cout << " isStrip: "                << actualStation.isStripType()     << std::endl;
		std::cout << std::endl;

#endif

		stationCounter++;

	}

	if (stationCounter == 0)
		isDetectorRead    = false;

#else

	isDetectorRead        = false;

#endif

	return isDetectorRead;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputFRoot::inputFRoot() 
  : inputRoot(),
    disableAutomaticDetector()
{

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputFRoot::inputFRoot(bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputRoot(detMask, enableJustPoints, enableHitsFromFile, enableMapsHits, enableHybridHits, enableStripHits), 
    disableAutomaticDetector(false)
{

  //	this->disableAutomaticDetector = false;

}
inputFRoot::inputFRoot(const char* name, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputRoot(name, detMask, hitProducer, enableJustPoints, enableHitsFromFile, enableMapsHits, enableHybridHits, enableStripHits), 
    disableAutomaticDetector(false)
{

  //	this->disableAutomaticDetector = false;

}
inputFRoot::inputFRoot(const char* _detectorFileName, unsigned short _numberOfVolumesInfrontOfSTS, bool _disableAutomaticDetector, bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputRoot(_detectorFileName, _numberOfVolumesInfrontOfSTS, detMask, enableJustPoints, enableHitsFromFile, enableMapsHits, enableHybridHits, enableStripHits), disableAutomaticDetector(_disableAutomaticDetector)

{

  //	this->disableAutomaticDetector = _disableAutomaticDetector;

}
inputFRoot::inputFRoot(const char* name, const char* _detectorFileName, unsigned short _numberOfVolumesInfrontOfSTS, bool _disableAutomaticDetector, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits, bool enableHybridHits, bool enableStripHits) 
  : inputRoot(name, _detectorFileName, _numberOfVolumesInfrontOfSTS, detMask, hitProducer, enableJustPoints, enableHitsFromFile, enableMapsHits, enableHybridHits, enableStripHits),
    disableAutomaticDetector(_disableAutomaticDetector)
 {

   //	this->disableAutomaticDetector = _disableAutomaticDetector;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputFRoot::~inputFRoot() {

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void inputFRoot::readDetectorInfo(CbmStsDigiScheme* scheme) {

	bool           isDetectorRead;

	if (disableAutomaticDetector)
		isDetectorRead = readDetector();
	else {

		isDetectorRead = readDetectorByInterface(scheme);

		if (!isDetectorRead)
			isDetectorRead = readDetectorByHand();

		if (!isDetectorRead) {
		
			cannotAutomaticReadDetectorWarningMsg* cannotAutomaticReadDetector = new cannotAutomaticReadDetectorWarningMsg();
			cannotAutomaticReadDetector->warningMsg();
			if(cannotAutomaticReadDetector != NULL) {
				delete cannotAutomaticReadDetector;
				cannotAutomaticReadDetector = NULL;
			}
			isDetectorRead = readDetector();

		}

	}

	if (isDetectorRead)
		data.maskDetector(detectorMask);
	else
		throw noDetectorError(INPUTLIB);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double inputFRoot::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputRoot::getReservedSizeOfData(0);
	returnValue += sizeof(disableAutomaticDetector);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double inputFRoot::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputRoot::getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double inputFRoot::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = inputRoot::getUsedSizeOfData(0);
	returnValue += sizeof(disableAutomaticDetector);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(inputFRoot)
