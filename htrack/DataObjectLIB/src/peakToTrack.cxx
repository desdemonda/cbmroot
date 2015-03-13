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
//     - class for holding the peakToTrack analysis information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:43 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/peakToTrack.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

peakToTrack::peakToTrack() 
  : peakToFakeTrack(0),
    peakToNoTrack(0),
    peakToOneTrack(0),
    peakToMoreTracks(0),
    numberOfPeaks(0),
    numberOfIdentifiedPeaks(0),
    numberOfClonePeaks(0),
    numberOfWrongPeaks(0)
{
  /*
	peakToFakeTrack         = 0;
	peakToNoTrack           = 0;
	peakToOneTrack          = 0;
	peakToMoreTracks        = 0;
	numberOfPeaks           = 0;
	numberOfIdentifiedPeaks = 0;
	numberOfClonePeaks      = 0;
	numberOfWrongPeaks      = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

peakToTrack::peakToTrack(const peakToTrack& value) 
  : peakToFakeTrack(value.peakToFakeTrack),
    peakToNoTrack(value.peakToNoTrack),
    peakToOneTrack(value.peakToOneTrack),
    peakToMoreTracks(value.peakToMoreTracks),
    numberOfPeaks(value.numberOfPeaks),
    numberOfIdentifiedPeaks(value.numberOfIdentifiedPeaks),
    numberOfClonePeaks(value.numberOfClonePeaks),
    numberOfWrongPeaks(value.numberOfWrongPeaks)
{
  /*
	peakToFakeTrack         = value.peakToFakeTrack;
	peakToNoTrack           = value.peakToNoTrack;
	peakToOneTrack          = value.peakToOneTrack;
	peakToMoreTracks        = value.peakToMoreTracks;
	numberOfPeaks           = value.numberOfPeaks;
	numberOfIdentifiedPeaks = value.numberOfIdentifiedPeaks;
	numberOfClonePeaks      = value.numberOfClonePeaks;
	numberOfWrongPeaks      = value.numberOfWrongPeaks;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

peakToTrack::~peakToTrack() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const peakToTrack& peakToTrack::operator = (const peakToTrack& value) {

	peakToFakeTrack         = value.peakToFakeTrack;
	peakToNoTrack           = value.peakToNoTrack;
	peakToOneTrack          = value.peakToOneTrack;
	peakToMoreTracks        = value.peakToMoreTracks;
	numberOfPeaks           = value.numberOfPeaks;
	numberOfIdentifiedPeaks = value.numberOfIdentifiedPeaks;
	numberOfClonePeaks      = value.numberOfClonePeaks;
	numberOfWrongPeaks      = value.numberOfWrongPeaks;

	return *this;

}
