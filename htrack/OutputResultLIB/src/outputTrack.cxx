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
//     - writes the evaluated tracks into a root-file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:41:57 $
// $Revision: 1.10 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../../DataObjectLIB/include/trackMomentum.h"
#include "../../DataObjectLIB/include/specialListMem.h"
#include "../../DataRootObjectLIB/include/trackHitMem.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/rootManager.h"
#else
	#include "FairRootManager.h"
#endif
#include "../include/outputTrack.h"


/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputTrack::initialize() {

	FairRootManager* manager;

	data.init();

	manager = FairRootManager::Instance();
	if (manager == NULL)
		throw cannotAccessRootManagerError(OUTPUTRESULTLIB);

#if (ARCHITECTURE == STANDALONE)

	manager->Register(STSTRACKBRANCH, STSFOLDER, data.getTrackData());

#endif

}
void outputTrack::initialize(trackData** _tracks) {

	this->tracks = _tracks;

}

void outputTrack::initialize(const char* name, trackData** _tracks) {

	std::string     tempName;
	std::string     fileName;
	FairRootManager* manager;

	manager = FairRootManager::Instance();
	if (manager == NULL)
		throw cannotAccessRootManagerError(OUTPUTRESULTLIB);

	tempName = name;
	fileName.clear();

	if ((tempName.find("../") == 0) || (tempName.find("./") == 0)) {

		if (getenv("VMCWORKDIR") != NULL) {

			fileName  = getenv("VMCWORKDIR");
			fileName += "/";

		}

	}

	fileName += tempName;

	manager->OpenOutFile(fileName.c_str());

	initialize(_tracks);

}

/****************************************************************
 * writes the evaluated tracks into the root file				*
 ****************************************************************/

void outputTrack::writeTracks(std::streambuf* terminal) {

	terminalSequence        statusSequence;
	trackAnalogInformation  actualTrackParameter;
	hitArray                hits;
	trackFrameworkMomentum  frameworkTrackMomentum;
	trackfinderInputHit*    hit;

#if (HITSTOTRACKVERSION != 1)

	unsigned long               m;
	trackHitMem                 actualTrack;
	specialListMem<trackHitMem> possibleTracks;
	bitArray                    actualTrackPattern;
	bitArray                    trackPattern;
	unsigned long               maximumNumberOfHits;

#endif

	if (tracks == NULL)
		throw cannotAccessTrackDataError(OUTPUTRESULTLIB);
	if (*tracks == NULL)
		throw cannotAccessTrackDataError(OUTPUTRESULTLIB);

	data.removeAllTracks();
	(*tracks)->resetActiveObject();

	createTerminalStatusSequence(&statusSequence, terminal, "Write tracks to file:\t\t\t\t", (unsigned int)(*tracks)->getNumberOfTracks());
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < (*tracks)->getNumberOfTracks(); i++) {

		(*tracks)->getNextTrackAnalogInfo(&actualTrackParameter);

		formula.evaluateP(actualTrackParameter.position, (*tracks)->getHistogramSpace(), &frameworkTrackMomentum, NULL);

		data.addTrack(frameworkTrackMomentum);

#if (HITSTOTRACKVERSION != 1)

		possibleTracks.clear();

#endif

#ifndef NOANALYSIS

		hits = actualTrackParameter.hits;
		for (unsigned short j = 0; j < hits.getNumberOfMemories(); j++) {

			hits[j].resetActiveObject();
			for (unsigned short k = 0; k < hits[j].getNumberOfEntries(); k++) {
				
				hit = hits[j].readActiveObjectAndMakeNextOneActive();

#if (HITSTOTRACKVERSION == 1)

				data.addHitToActualTrack(hit);

#else

				actualTrack.removeAllHits();
				actualTrack.addHit(hit);

				if (possibleTracks.isFound(actualTrack, true))		/* track is old track */
					possibleTracks.getActiveObject()->addHit(hit);
				else												/* track not found, so add it; fakes have no track => they are always not found */
					possibleTracks.push(actualTrack);

#endif

			}

		}

#endif

#if (HITSTOTRACKVERSION != 1)

		actualTrack.removeAllHits();

		trackPattern.reset();
		maximumNumberOfHits = (unsigned long) -1;
		possibleTracks.sort();
		possibleTracks.resetActiveObject();
		for (unsigned long l = 0; l < possibleTracks.getNumberOfEntries(); l++) {
	
			actualTrack = possibleTracks.readActiveObjectAndMakeNextOneActive();

			if ((l == 0) || (maximumNumberOfHits == actualTrack.getNumberOfHits())) {

#if (HITSTOTRACKVERSION == 3)

				maximumNumberOfHits = actualTrack.getNumberOfHits();

#endif

				actualTrack.resetHitPointer();
				for (m = 0; m < actualTrack.getNumberOfHits(); m++) {
				
					hit = actualTrack.getHitAndMakeNextActive();
					data.addHitToActualTrack(hit);
				
				}

				actualTrackPattern = actualTrack.getTrackPattern();

				trackPattern |= actualTrackPattern;

			}
			else {

				actualTrack.resetHitPointer();
				for (m = 0; m < actualTrack.getNumberOfHits(); m++) {
				
					hit = actualTrack.getHitAndMakeNextActive();
					if (!trackPattern[hit->getStation()->getIndex()]) {
					
						data.addHitToActualTrack(hit);
						trackPattern[hit->getStation()->getIndex()] = true;
					
					}

				}

			}

		}

#endif

		terminalOverwriteWithIncrement(statusSequence);

	}

#if (HITSTOTRACKVERSION != 1)

	actualTrack.removeAllHits();
	possibleTracks.clear();

#endif

	terminalFinalize(statusSequence);

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

outputTrack::outputTrack() 
  : tracks(NULL),
    formula(),
    data()
{

  //	tracks = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outputTrack::outputTrack(trackData** _tracks) 
  : tracks(NULL),
    formula(),
    data()
{

	initialize(_tracks);

}
outputTrack::outputTrack(const char* name, trackData** _tracks) 
  : tracks(NULL),
    formula(),
    data()
{

	initialize(name, _tracks);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outputTrack::~outputTrack() {

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputTrack::init(trackData** _tracks) {

	initialize(_tracks);

	init();

}

void outputTrack::init(const char* name, trackData** _tracks) {

	initialize(name, _tracks);

	init();

}
