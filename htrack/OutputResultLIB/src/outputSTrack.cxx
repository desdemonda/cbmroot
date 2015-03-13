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
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/outputResultWarningMsg.h"
#include "../include/outputSTrack.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

outputSTrack::outputSTrack() : outputTrack() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outputSTrack::outputSTrack(trackData** _tracks) : outputTrack(_tracks) {

}
outputSTrack::outputSTrack(const char* name, trackData** _tracks) : outputTrack(name, _tracks) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outputSTrack::~outputSTrack() {

}

/****************************************************************
 * Method inits the variables.									*
 ****************************************************************/

void outputSTrack::init() {

	initialize();

}

/****************************************************************
 * writes the evaluated tracks into the root file				*
 ****************************************************************/

void outputSTrack::write(std::streambuf* terminal, TClonesArray* trackArray) {

	if (trackArray != NULL) {

		trackPointerIsNotSupportedWarningMsg* trackPointerIsNotSupported = new trackPointerIsNotSupportedWarningMsg();
		trackPointerIsNotSupported->warningMsg();
		if(trackPointerIsNotSupported != NULL) {
			delete trackPointerIsNotSupported;
			trackPointerIsNotSupported = NULL;
		}

	}

	writeTracks(terminal);

}
