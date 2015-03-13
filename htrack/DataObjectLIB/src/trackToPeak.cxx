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
//     - class for holding the trackToPeak analysis information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:44 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/trackToPeak.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackToPeak::trackToPeak() 
  : trackToNoPeak(0),
    trackToOnePeak(0),
    trackToMorePeaks(0),
    numberOfTracks(0),
    numberOfWellFoundTracks(0),
    numberOfWrongFoundTracks(0),
    numberOfTracksWithP(0),
    numberOfHits(0)
{
  /*
	trackToNoPeak            = 0;
	trackToOnePeak           = 0;
	trackToMorePeaks         = 0;
	numberOfTracks           = 0;
	numberOfWellFoundTracks  = 0;
	numberOfWrongFoundTracks = 0;
	numberOfTracksWithP      = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackToPeak::trackToPeak(const trackToPeak& value) 
  : trackToNoPeak(value.trackToNoPeak),		  
    trackToOnePeak(value.trackToOnePeak),	  
    trackToMorePeaks(value.trackToMorePeaks),	  
    numberOfTracks(value.numberOfTracks),	  
    numberOfWellFoundTracks(value.numberOfWellFoundTracks), 
    numberOfWrongFoundTracks(value.numberOfWrongFoundTracks),
    numberOfTracksWithP(value.numberOfTracksWithP),
    numberOfHits(value.numberOfHits)
{
  /*
	trackToNoPeak            = value.trackToNoPeak;
	trackToOnePeak           = value.trackToOnePeak;
	trackToMorePeaks         = value.trackToMorePeaks;
	numberOfTracks           = value.numberOfTracks;
	numberOfWellFoundTracks  = value.numberOfWellFoundTracks;
	numberOfWrongFoundTracks = value.numberOfWrongFoundTracks;
	numberOfTracksWithP      = value.numberOfTracksWithP;     
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackToPeak::~trackToPeak() {

}

/**
 * operator = ()
 */

const trackToPeak& trackToPeak::operator = (const trackToPeak& value) {

	trackToNoPeak            = value.trackToNoPeak;
	trackToOnePeak           = value.trackToOnePeak;
	trackToMorePeaks         = value.trackToMorePeaks;
	numberOfTracks           = value.numberOfTracks;
	numberOfWellFoundTracks  = value.numberOfWellFoundTracks;
	numberOfWrongFoundTracks = value.numberOfWrongFoundTracks;
	numberOfTracksWithP      = value.numberOfTracksWithP;

	return *this;

}
