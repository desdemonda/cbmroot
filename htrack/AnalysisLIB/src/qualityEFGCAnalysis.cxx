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
//     - implements methods to analyze the trackfinding algorithm's quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-07 10:34:06 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/qualityEFGCAnalysis.h"


/****************************************************************
 * resets the info structs for the peaks and the tracks			*
 ****************************************************************/

void qualityEFGCAnalysis::resetPeakAndTrackInfo() 
{
  
	peakInfo.peakToFakeTrack           = 0;
	peakInfo.peakToNoTrack             = 0;
	peakInfo.peakToOneTrack            = 0;
	peakInfo.peakToMoreTracks          = 0;
	peakInfo.numberOfPeaks             = 0;
	peakInfo.numberOfIdentifiedPeaks   = 0;
	peakInfo.numberOfClonePeaks        = 0;
	peakInfo.numberOfWrongPeaks        = 0;
	trackInfo.trackToNoPeak            = 0;
	trackInfo.trackToOnePeak           = 0;
	trackInfo.trackToMorePeaks         = 0;
	trackInfo.numberOfTracks           = 0;
	trackInfo.numberOfWellFoundTracks  = 0;
	trackInfo.numberOfWrongFoundTracks = 0;
	trackInfo.numberOfTracksWithP      = 0;
	trackInfo.numberOfHits             = 0;
 
}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

qualityEFGCAnalysis::qualityEFGCAnalysis() 
  : peakInfo(),
    trackInfo()
{

	resetPeakAndTrackInfo();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

qualityEFGCAnalysis::~qualityEFGCAnalysis() {

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void qualityEFGCAnalysis::initializeEvaluation() {

	reset();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void qualityEFGCAnalysis::finalizeEvaluation(int numberOfHits, int numberOfTracksWithP) {

	peakInfo.numberOfPeaks         = peakInfo.peakToMoreTracks + peakInfo.peakToOneTrack + peakInfo.peakToNoTrack + peakInfo.peakToFakeTrack;
	trackInfo.numberOfTracks       = trackInfo.trackToMorePeaks + trackInfo.trackToOnePeak + trackInfo.trackToNoPeak;
	trackInfo.numberOfTracksWithP += (unsigned int)numberOfTracksWithP;
	trackInfo.numberOfHits        += (unsigned int)numberOfHits;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::peakToFakeTrack() {

	peakInfo.peakToFakeTrack++;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::peakToNoTrack() {

	peakInfo.peakToNoTrack++;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::peakToOneTrack(bool isClone, bool isWrong) {

	peakInfo.peakToOneTrack++;

	if (isWrong)
		peakInfo.numberOfWrongPeaks++;
	else {
		if (isClone)
			peakInfo.numberOfClonePeaks++;
		else
			peakInfo.numberOfIdentifiedPeaks++;
	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::peakToMoreTracks(bool isClone, bool isWrong) {

	peakInfo.peakToMoreTracks++;

	if (isWrong)
		peakInfo.numberOfWrongPeaks++;
	else {
		if (isClone)
			peakInfo.numberOfClonePeaks++;
		else
			peakInfo.numberOfIdentifiedPeaks++;
	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::trackToNoPeak() {

	trackInfo.trackToNoPeak++;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::trackToOnePeak() {

	trackInfo.trackToOnePeak++;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::trackToMorePeaks() {

	trackInfo.trackToMorePeaks++;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::wellFoundTracks() {

	trackInfo.numberOfWellFoundTracks++;

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityEFGCAnalysis::wrongFoundTracks() {

	trackInfo.numberOfWrongFoundTracks++;

}

/****************************************************************
 * method returns information on the peaks according to tracks.	*
 ****************************************************************/

peakToTrack* qualityEFGCAnalysis::getPeakToTrackInfo() {

	return &peakInfo;

}

/****************************************************************
 * method returns information on the tracks according to peaks.	*
 ****************************************************************/

trackToPeak* qualityEFGCAnalysis::getTrackToPeakInfo() {

	return &trackInfo;

}

/****************************************************************
 * method returns the number of the clone tracks.				*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getNumberOfCloneTracks() {

	return (unsigned short)peakInfo.numberOfClonePeaks;

}

/****************************************************************
 * method returns the number of the fake tracks.				*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getNumberOfFakeTracks() {

	return (unsigned short)peakInfo.peakToFakeTrack;

}

/****************************************************************
 * method returns the number of the ghost tracks.				*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getNumberOfGhostTracks() {

	return (unsigned short)peakInfo.peakToNoTrack;

}

/****************************************************************
 * method returns the number of the indentified tracks.			*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getNumberOfIdentifiedTracks() {

	return (unsigned short)(trackInfo.trackToOnePeak + trackInfo.trackToMorePeaks);

}

/****************************************************************
 * method returns the number of the not well found tracks.		*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getNumberOfNotWellFoundTracks() {

	unsigned short returnValue;

	if (getNumberOfIdentifiedTracks() > (unsigned short)trackInfo.numberOfWellFoundTracks)
		returnValue = getNumberOfIdentifiedTracks() - (unsigned short)trackInfo.numberOfWellFoundTracks;
	else
		returnValue = 0;

	return returnValue;

}

/****************************************************************
 * method returns the number of the well found tracks.			*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getNumberOfWellFoundTracks() {

	return (unsigned short)trackInfo.numberOfWellFoundTracks;

}

/****************************************************************
 * method returns the quality of track finding in percent.		*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getTrackQuality() {

	return (unsigned short)((double)((int)(100 * ((double)trackInfo.trackToOnePeak + (double)trackInfo.trackToMorePeaks) / ((double)trackInfo.numberOfTracksWithP) + 0.5)));

}

/****************************************************************
 * method returns the quality of well track finding in percent.	*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getWellTrackQuality() {

	return (unsigned short)((double)((int)(100 * ((double)trackInfo.numberOfWellFoundTracks) / ((double)trackInfo.numberOfTracksWithP) + 0.5)));

}

/****************************************************************
 * method returns the quality of fake tracks in percent.		*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getFakeQuality() {

	return (unsigned short)((double)((int)(100 * ((double)peakInfo.peakToFakeTrack) / ((double)peakInfo.numberOfPeaks) + 0.5)));

}

/****************************************************************
 * method returns the quality of ghost tracks in percent.		*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getGhostQuality() {

	return (unsigned short)((double)((int)(100 * ((double)peakInfo.peakToNoTrack) / ((double)peakInfo.numberOfPeaks) + 0.5)));

}

/****************************************************************
 * method returns the quality of indetified tracks in percent.	*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getIdentificationQuality() {

	return (unsigned short)((double)((int)(100 * ((double)trackInfo.trackToOnePeak + (double)trackInfo.trackToMorePeaks) / ((double)peakInfo.numberOfPeaks) + 0.5)));

}

/****************************************************************
 * method returns the quality of data reduction in percent.		*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getReductionQuality() {

	return (unsigned short)((double)((int)(100 * ((double)trackInfo.numberOfHits - peakInfo.numberOfPeaks) / ((double)trackInfo.numberOfHits) + 0.5)));

}

/****************************************************************
 * method returns the quality of cloned tracks in percent.		*
 ****************************************************************/

unsigned short qualityEFGCAnalysis::getCloneQuality() {

	return (unsigned short)((double)((int)(100 * ((double)peakInfo.numberOfClonePeaks) / ((double)peakInfo.numberOfPeaks) + 0.5)));

}
