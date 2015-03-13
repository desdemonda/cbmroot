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
// $Date: 2008-08-14 12:37:34 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/outputResultWarningMsg.h"
#include "../include/outputFTrack.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

outputFTrack::outputFTrack() 
  : outputTrack(),
    writeTracksToFile(true)
{

  //	writeTracksToFile = true;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outputFTrack::outputFTrack(trackData** _tracks) 
  : outputTrack(_tracks), 
    writeTracksToFile(true)
{

  //	writeTracksToFile = true;

}
outputFTrack::outputFTrack(trackData** _tracks, bool _writeTracksToFile) 
  : outputTrack(_tracks),
    writeTracksToFile(_writeTracksToFile)
{

  //	this->writeTracksToFile = _writeTracksToFile;

}
outputFTrack::outputFTrack(const char* name, trackData** _tracks) 
  : outputTrack(name, _tracks),
    writeTracksToFile(true)
{

  //	writeTracksToFile = true;

}
outputFTrack::outputFTrack(const char* name, trackData** _tracks, bool _writeTracksToFile) 
  : outputTrack(name, _tracks),
    writeTracksToFile(_writeTracksToFile)
{

  //	this->writeTracksToFile = _writeTracksToFile;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outputFTrack::~outputFTrack() {

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputFTrack::init() {

	if (writeTracksToFile)
		initialize();

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputFTrack::init(bool _writeTracksToFile) {

	this->writeTracksToFile = _writeTracksToFile;

	init();

}
void outputFTrack::init(trackData** _tracks, bool _writeTracksToFile) {

	this->writeTracksToFile = _writeTracksToFile;

	outputTrack::init(_tracks);

}
void outputFTrack::init(const char* name, trackData** _tracks, bool _writeTracksToFile) {

	this->writeTracksToFile = _writeTracksToFile;

	outputTrack::init(name, _tracks);

}

/****************************************************************
 * writes the evaluated tracks into the root file				*
 ****************************************************************/

void outputFTrack::write(std::streambuf* terminal, TClonesArray* trackArray) {

	if (!writeTracksToFile)
		data.init(trackArray);
	else {
	
		if (trackArray != NULL) {

			trackPointerIsDisabledWarningMsg* trackPointerIsDisabled = new trackPointerIsDisabledWarningMsg();
			trackPointerIsDisabled->warningMsg();
			if(trackPointerIsDisabled != NULL) {
				delete trackPointerIsDisabled;
				trackPointerIsDisabled = NULL;
			}

		}

	}

	writeTracks(terminal);

}
