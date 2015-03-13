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
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/qualityAnalysis.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

qualityAnalysis::qualityAnalysis() 
  : qualityEFGCEvent(NULL),
    qualityEFGCEventAbsolute(false),
    qualityEFGCEventRelative(false),
    qualityEFGCTotal(NULL),
    qualityEFGCTotalAbsolute(false),
    qualityEFGCTotalRelative(false)
{
  /*
	qualityEFGCEvent         = NULL;
	qualityEFGCEventAbsolute = false;
	qualityEFGCEventRelative = false;
	qualityEFGCTotal         = NULL;
	qualityEFGCTotalAbsolute = false;
	qualityEFGCTotalRelative = false;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

qualityAnalysis::~qualityAnalysis() {

	if (qualityEFGCEvent != NULL) {
		delete qualityEFGCEvent;
		qualityEFGCEvent = NULL;
	}
	if (qualityEFGCTotal != NULL) {
		delete qualityEFGCTotal;
		qualityEFGCTotal = NULL;
	}

}

/****************************************************************
 * method initializes the qualityEFGCEventAbsolute-Analysis.	*
 ****************************************************************/

void qualityAnalysis::initQualityEFGCEventAbsoluteAnalysis(bool enable) {

	if ((!qualityEFGCEventRelative) && (!enable)) {

		if (qualityEFGCEvent != NULL) {
			delete qualityEFGCEvent;
			qualityEFGCEvent = NULL;
		}

	}

	if (enable)
		if (qualityEFGCEvent == NULL)
			qualityEFGCEvent = new qualityEFGCEventAnalysis();

	qualityEFGCEventAbsolute = enable;

}

/****************************************************************
 * method initializes the qualityEFGCEventRelative-Analysis.	*
 ****************************************************************/

void qualityAnalysis::initQualityEFGCEventRelativeAnalysis(bool enable) {

	if ((!qualityEFGCEventAbsolute) && (!enable)) {

		if (qualityEFGCEvent != NULL) {
			delete qualityEFGCEvent;
			qualityEFGCEvent = NULL;
		}

	}

	if (enable)
		if (qualityEFGCEvent == NULL)
			qualityEFGCEvent = new qualityEFGCEventAnalysis();

	qualityEFGCEventRelative = enable;

}

/****************************************************************
 * method initializes the qualityEFGCTotalAbsolute-Analysis.	*
 ****************************************************************/

void qualityAnalysis::initQualityEFGCTotalAbsoluteAnalysis(bool enable) {

	if ((!qualityEFGCTotalRelative) && (!enable)) {

		if (qualityEFGCTotal != NULL) {
			delete qualityEFGCTotal;
			qualityEFGCTotal = NULL;
		}

	}

	if (enable)
		if (qualityEFGCTotal == NULL)
			qualityEFGCTotal = new qualityEFGCTotalAnalysis();

	qualityEFGCTotalAbsolute = enable;

}

/****************************************************************
 * method initializes the qualityEFGCTotalRelative-Analysis.	*
 ****************************************************************/

void qualityAnalysis::initQualityEFGCTotalRelativeAnalysis(bool enable) {

	if ((!qualityEFGCTotalAbsolute) && (!enable)) {

		if (qualityEFGCTotal != NULL) {
			delete qualityEFGCTotal;
			qualityEFGCTotal = NULL;
		}

	}

	if (enable)
		if (qualityEFGCTotal == NULL)
			qualityEFGCTotal = new qualityEFGCTotalAnalysis();

	qualityEFGCTotalRelative = enable;

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void qualityAnalysis::initializeEvaluation() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->initializeEvaluation();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->initializeEvaluation();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void qualityAnalysis::finalizeEvaluation(int numberOfHits, int numberOfTracksWithP) {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->finalizeEvaluation(numberOfHits, numberOfTracksWithP);
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->finalizeEvaluation(numberOfHits, numberOfTracksWithP);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::peakToFakeTrack() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->peakToFakeTrack();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->peakToFakeTrack();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::peakToNoTrack() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->peakToNoTrack();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->peakToNoTrack();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::peakToOneTrack(bool isClone, bool isWrong) {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->peakToOneTrack(isClone, isWrong);
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->peakToOneTrack(isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::peakToMoreTracks(bool isClone, bool isWrong) {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->peakToMoreTracks(isClone, isWrong);
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->peakToMoreTracks(isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::trackToNoPeak() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->trackToNoPeak();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->trackToNoPeak();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::trackToOnePeak() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->trackToOnePeak();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->trackToOnePeak();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::trackToMorePeaks() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->trackToMorePeaks();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->trackToMorePeaks();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::wellFoundTracks() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->wellFoundTracks();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->wellFoundTracks();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void qualityAnalysis::wrongFoundTracks() {

	if (qualityEFGCEvent != NULL)
		qualityEFGCEvent->wrongFoundTracks();
	if (qualityEFGCTotal != NULL)
		qualityEFGCTotal->wrongFoundTracks();

}

/****************************************************************
 * method returns true, if the qualityEFGCEvent-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool qualityAnalysis::isQualityEFGCEventAnalysisEnabled() {

	bool returnValue;

	if (qualityEFGCEvent != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the qualityEFGCTotal-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool qualityAnalysis::isQualityEFGCTotalAnalysisEnabled() {

	bool returnValue;

	if (qualityEFGCTotal != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * qualityEFGCEvent-Analysis.									*
 ****************************************************************/

std::string qualityAnalysis::getQualityEFGCEventAnalysis() {

	std::string returnValue;

	returnValue  = getAbsoluteQualityEFGCEventAnalysis();
	returnValue += getRelativeQualityEFGCEventAnalysis();

	return returnValue;

}
std::string qualityAnalysis::getAbsoluteQualityEFGCEventAnalysis() {

	std::string returnValue;
	char        intBuffer[intConversionDigits + 1];

	returnValue = "";

	if(qualityEFGCEvent != NULL) {

		returnValue += "\n";		
		returnValue += "\n";		

		if (qualityEFGCEventAbsolute) {

			returnValue += "\n";		
			returnValue += "EVENT ALGORITHM RESULTS";
			returnValue += "\n";		

			if (qualityEFGCEventRelative) {

				returnValue += "Absolute Analysis:";
				returnValue += "\n";		

			}

			returnValue += "Peak Summary";
			returnValue += "\n";		
			returnValue += "Event number of peaks corresponding to no track:     \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->peakToFakeTrack + qualityEFGCEvent->getPeakToTrackInfo()->peakToNoTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks corresponding to one track:    \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->peakToOneTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks corresponding to more tracks:  \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->peakToMoreTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks:                               \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->numberOfPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "-------------------------------------------------";
			returnValue += "\n";		
			returnValue += "Event number of peaks classified as track:           \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->numberOfIdentifiedPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks classified as clone:           \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->numberOfClonePeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks classified as fake:            \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->peakToFakeTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks classified as ghost:           \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->peakToNoTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks classified as no track:        \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->numberOfWrongPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of peaks:                               \t";
			uitos(qualityEFGCEvent->getPeakToTrackInfo()->numberOfPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "=================================================";
			returnValue += "\n";		
			returnValue += "Track Summary";
			returnValue += "\n";		
			returnValue += "Event number of tracks corresponding to no peak:     \t";
			uitos(qualityEFGCEvent->getTrackToPeakInfo()->trackToNoPeak, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of tracks corresponding to one peak:    \t";
			uitos(qualityEFGCEvent->getTrackToPeakInfo()->trackToOnePeak, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of tracks corresponding to more peaks:  \t";
			uitos(qualityEFGCEvent->getTrackToPeakInfo()->trackToMorePeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of tracks:                              \t";
			uitos(qualityEFGCEvent->getTrackToPeakInfo()->numberOfTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "-------------------------------------------------";
			returnValue += "\n";		
			returnValue += "Event number of well indentified findable tracks:    \t";
			uitos(qualityEFGCEvent->getNumberOfWellFoundTracks(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of not well indentified findable tracks:\t";
			uitos(qualityEFGCEvent->getNumberOfNotWellFoundTracks(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of identified not findable tracks:      \t";
			uitos(qualityEFGCEvent->getTrackToPeakInfo()->numberOfWrongFoundTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of identified tracks:                   \t";
			uitos(qualityEFGCEvent->getNumberOfIdentifiedTracks() + qualityEFGCEvent->getTrackToPeakInfo()->numberOfWrongFoundTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";
			returnValue += "-------------------------------------------------";
			returnValue += "\n";		
			returnValue += "Event number of identified findable tracks:          \t";
			uitos(qualityEFGCEvent->getNumberOfIdentifiedTracks(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Event number of findable tracks:                     \t";
			uitos(qualityEFGCEvent->getTrackToPeakInfo()->numberOfTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		

		}

	}

	return returnValue;

}
std::string qualityAnalysis::getRelativeQualityEFGCEventAnalysis() {

	std::string returnValue;
	char        intBuffer[intConversionDigits + 1];

	returnValue = "";

	if(qualityEFGCEvent != NULL) {

		returnValue += "\n";		
		returnValue += "\n";		

		if (qualityEFGCEventRelative) {

			if (qualityEFGCEventAbsolute) {

				returnValue += "=================================================";
				returnValue += "\n";		
				returnValue += "\n";		
				returnValue += "Relative Analysis:";
				returnValue += "\n";		

			}
			else {

				returnValue += "EVENT ALGORITHM RESULTS";
				returnValue += "\n";		

			}

			returnValue += "Event efficiency [e] (aim 100%):                \t\t";
			uitos(qualityEFGCEvent->getTrackQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Event fake rate [f] (aim   0%):                 \t\t";
			uitos(qualityEFGCEvent->getFakeQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Event ghost rate [g] (aim   0%):                \t\t";
			uitos(qualityEFGCEvent->getGhostQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Event clones rate [c] (aim   0%):               \t\t";
			uitos(qualityEFGCEvent->getCloneQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Event identification rate [i] (aim 100%):       \t\t";
			uitos(qualityEFGCEvent->getIdentificationQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Event data reduction rate [r] (aim near 100%):       \t\t";
			uitos(qualityEFGCEvent->getReductionQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		

		}

	}

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * qualityEFGCTotal-Analysis.									*
 ****************************************************************/

std::string qualityAnalysis::getQualityEFGCTotalAnalysis() {

	std::string returnValue;

	returnValue  = getAbsoluteQualityEFGCTotalAnalysis();
	returnValue += getRelativeQualityEFGCTotalAnalysis();

	return returnValue;

}
std::string qualityAnalysis::getAbsoluteQualityEFGCTotalAnalysis() {

	std::string returnValue;
	char        intBuffer[intConversionDigits + 1];

	returnValue = "";

	if(qualityEFGCTotal != NULL) {

		returnValue += "\n";		
		returnValue += "\n";		

		if (qualityEFGCTotalAbsolute) {

			returnValue += "\n";		
			returnValue += "TOTAL ALGORITHM RESULTS";
			returnValue += "\n";		

			if (qualityEFGCTotalRelative) {

				returnValue += "Absolute Analysis:";
				returnValue += "\n";		

			}

			returnValue += "Peak Summary";
			returnValue += "\n";		
			returnValue += "Total number of peaks corresponding to no track:     \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->peakToFakeTrack + qualityEFGCTotal->getPeakToTrackInfo()->peakToNoTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks corresponding to one track:    \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->peakToOneTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks corresponding to more tracks:  \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->peakToMoreTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks:                               \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->numberOfPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "-------------------------------------------------";
			returnValue += "\n";
			returnValue += "Total number of peaks classified as track:           \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->numberOfIdentifiedPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks classified as clone:           \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->numberOfClonePeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks classified as fake:            \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->peakToFakeTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks classified as ghost:           \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->peakToNoTrack, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks classified as no track:        \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->numberOfWrongPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of peaks:                               \t";
			uitos(qualityEFGCTotal->getPeakToTrackInfo()->numberOfPeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "=================================================";
			returnValue += "\n";		
			returnValue += "Track Summary";
			returnValue += "\n";		
			returnValue += "Total number of tracks corresponding to no peak:     \t";
			uitos(qualityEFGCTotal->getTrackToPeakInfo()->trackToNoPeak, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of tracks corresponding to one peak:    \t";
			uitos(qualityEFGCTotal->getTrackToPeakInfo()->trackToOnePeak, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of tracks corresponding to more peaks:  \t";
			uitos(qualityEFGCTotal->getTrackToPeakInfo()->trackToMorePeaks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of tracks:                              \t";
			uitos(qualityEFGCTotal->getTrackToPeakInfo()->numberOfTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "-------------------------------------------------";
			returnValue += "\n";		
			returnValue += "Total number of well indentified findable tracks:    \t";
			uitos(qualityEFGCTotal->getNumberOfWellFoundTracks(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of not well indentified findable tracks:\t";
			uitos(qualityEFGCTotal->getNumberOfNotWellFoundTracks(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of identified not findable tracks:      \t";
			uitos(qualityEFGCTotal->getTrackToPeakInfo()->numberOfWrongFoundTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of identified tracks:                   \t";
			uitos(qualityEFGCTotal->getNumberOfIdentifiedTracks() + qualityEFGCTotal->getTrackToPeakInfo()->numberOfWrongFoundTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";
			returnValue += "-------------------------------------------------";
			returnValue += "\n";		
			returnValue += "Total number of identified findable tracks:          \t";
			uitos(qualityEFGCTotal->getNumberOfIdentifiedTracks(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		
			returnValue += "Total number of findable tracks:                     \t";
			uitos(qualityEFGCTotal->getTrackToPeakInfo()->numberOfTracks, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "\n";		

		}

	}

	return returnValue;

}
std::string qualityAnalysis::getRelativeQualityEFGCTotalAnalysis() {

	std::string returnValue;
	char        intBuffer[intConversionDigits + 1];

	returnValue = "";

	if(qualityEFGCTotal != NULL) {

		returnValue += "\n";		
		returnValue += "\n";		

		if (qualityEFGCTotalRelative) {

			if (qualityEFGCTotalAbsolute) {

				returnValue += "=================================================";
				returnValue += "\n";		
				returnValue += "\n";		
				returnValue += "Relative Analysis:";
				returnValue += "\n";		

			}
			else {

				returnValue += "TOTAL ALGORITHM RESULTS";
				returnValue += "\n";		

			}

			returnValue += "Total efficiency [e] (aim 100%):                \t";
			uitos(qualityEFGCTotal->getTrackQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Total fake rate [f] (aim   0%):                 \t";
			uitos(qualityEFGCTotal->getFakeQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Total ghost rate [g] (aim   0%):                \t";
			uitos(qualityEFGCTotal->getGhostQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Total clones rate [c] (aim   0%):               \t";
			uitos(qualityEFGCTotal->getCloneQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Total identification rate [i] (aim 100%):       \t";
			uitos(qualityEFGCTotal->getIdentificationQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		
			returnValue += "Total data reduction rate [r] (aim near 100%):       \t";
			uitos(qualityEFGCTotal->getReductionQuality(), intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%";
			returnValue += "\n";		

		}

	}

	return returnValue;

}
