//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - implements methods to analyze the trackfinding algorithm's quality
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:34:05 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _QUALITYEFGCANALYSIS_H
#define _QUALITYEFGCANALYSIS_H


#include "../../DataObjectLIB/include/peakToTrack.h"
#include "../../DataObjectLIB/include/trackToPeak.h"


/* **************************************************************
 * CLASS qualityEFGCAnalysis						 			*
 * **************************************************************/

class qualityEFGCAnalysis {

protected:

	peakToTrack peakInfo;	/**< Object for storing the detailed information based on the peaks. */
	trackToPeak trackInfo;	/**< Object for storing the detailed information based on the tracks. */

/**
 * resets the info classes for the peaks and the tracks
 */

	void resetPeakAndTrackInfo();

/**
 * resets the info structs for event and nothing for total
 */

	virtual void reset() = 0;

public:

/**
 * Default constructor
 */

	qualityEFGCAnalysis();

/**
 * Destructor
 */

	virtual ~qualityEFGCAnalysis();

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 * @param numberOfHits is the number of hits which occur
 * @param numberOfTracksWithP is the number of tracks which
 *        should be found
 * @see analysis::getNumberOfTracksWithP(unsigned short minimumClassPriority)
 */

	void finalizeEvaluation(int numberOfHits, int numberOfTracksWithP);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToFakeTrack();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToNoTrack();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param isClone marks this track as clone track
 * @param isWrong marks this track as wrong track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToOneTrack(bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param isClone marks this track as clone track
 * @param isWrong marks this track as wrong track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToMoreTracks(bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToNoPeak();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToOnePeak();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToMorePeaks();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void wellFoundTracks();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void wrongFoundTracks();

/**
 * method returns information on the peaks according to tracks.
 * @return Object for storing the detailed information based on the peaks
 */

	peakToTrack* getPeakToTrackInfo();

/**
 * method returns information on the tracks according to peaks.
 * @return Object for storing the detailed information based on the tracks
 */

	trackToPeak* getTrackToPeakInfo();

/**
 * method returns the number of the clone tracks.
 */

	unsigned short getNumberOfCloneTracks();

/**
 * method returns the number of the fake tracks.
 */

	unsigned short getNumberOfFakeTracks();

/**
 * method returns the number of the ghost tracks.
 */

	unsigned short getNumberOfGhostTracks();

/**
 * method returns the number of the indentified tracks.
 */

	unsigned short getNumberOfIdentifiedTracks();

/**
 * method returns the number of the not well found tracks.
 */

	unsigned short getNumberOfNotWellFoundTracks();

/**
 * method returns the number of the well found tracks.
 */

	unsigned short getNumberOfWellFoundTracks();

/**
 * method returns the quality of found tracks in percent.
 */

	unsigned short getTrackQuality();

/**
 * method returns the quality of well track finding in percent.
 */

	unsigned short getWellTrackQuality();

/**
 * method returns the quality of fake tracks in percent.
 */

	unsigned short getFakeQuality();

/**
 * method returns the quality of ghost tracks in percent.
 */

	unsigned short getGhostQuality();

/**
 * method returns the quality of indetified tracks in percent.
 */

	unsigned short getIdentificationQuality();

/**
 * method returns the quality of data reduction in percent.
 */

	unsigned short getReductionQuality();

/**
 * method returns the quality of cloned tracks in percent.
 */

	unsigned short getCloneQuality();

};

#endif

