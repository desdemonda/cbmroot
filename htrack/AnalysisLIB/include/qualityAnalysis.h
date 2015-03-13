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
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _QUALITYANALYSIS_H
#define _QUALITYANALYSIS_H


#include "qualityEFGCEventAnalysis.h"
#include "qualityEFGCTotalAnalysis.h"
#include <string>


/* **************************************************************
 * CLASS qualityAnalysis				 						*
 * **************************************************************/

class qualityAnalysis {

 private:
  qualityAnalysis(const qualityAnalysis&);
  qualityAnalysis& operator=(const qualityAnalysis&);

protected:

	qualityEFGCEventAnalysis* qualityEFGCEvent;			/**< Object for analysing the quality per event and writing on the standard output stream. */
	bool                      qualityEFGCEventAbsolute;	/**< Variable for enabling the quality analysis per event and with absolute numbers. */
	bool                      qualityEFGCEventRelative;	/**< Variable for enabling the quality analysis per event and with percent numbers. */
	qualityEFGCTotalAnalysis* qualityEFGCTotal;			/**< Object for analysing the quality accumulated for all events and writing on the standard output stream. */
	bool                      qualityEFGCTotalAbsolute;	/**< Variable for enabling the quality analysis accumulated for all events and with absolute numbers. */
	bool                      qualityEFGCTotalRelative;	/**< Variable for enabling the quality analysis accumulated for all events and with percent numbers. */

public:

/**
 * Default constructor
 */

	qualityAnalysis();

/**
 * Destructor
 */

	virtual ~qualityAnalysis();

/**
 * method initializes the qualityEFGCEventAbsolute-Analysis.
 */

	void initQualityEFGCEventAbsoluteAnalysis(bool enable = true);

/**
 * method initializes the qualityEFGCEventRelative-Analysis.
 */

	void initQualityEFGCEventRelativeAnalysis(bool enable = true);

/**
 * method initializes the qualityEFGCTotalAbsolute-Analysis.
 */

	void initQualityEFGCTotalAbsoluteAnalysis(bool enable = true);

/**
 * method initializes the qualityEFGCTotalRelative-Analysis.
 */

	void initQualityEFGCTotalRelativeAnalysis(bool enable = true);

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
 * method returns true, if the qualityEFGCEvent-Analysis is
 * enabled.
 */

	bool isQualityEFGCEventAnalysisEnabled();

/**
 * method returns true, if the qualityEFGCTotal-Analysis is
 * enabled.
 */

	bool isQualityEFGCTotalAnalysisEnabled();

/**
 * method returns a string representing the result of the
 * qualityEFGCEvent-Analysis.
 */

	std::string getQualityEFGCEventAnalysis();
	std::string getAbsoluteQualityEFGCEventAnalysis();
	std::string getRelativeQualityEFGCEventAnalysis();

/**
 * method returns a string representing the result of the
 * qualityEFGCTotal-Analysis.
 */

	std::string getQualityEFGCTotalAnalysis();
	std::string getAbsoluteQualityEFGCTotalAnalysis();
	std::string getRelativeQualityEFGCTotalAnalysis();

};

#endif
