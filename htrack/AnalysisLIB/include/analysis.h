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
///     - implements methods to analyze the trackfinding algorithm
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:07:01 $
/// $Revision: 1.27 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSIS_H
#define _ANALYSIS_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataObjectLIB/include/specialListMem.h"
#include "../../DataObjectLIB/include/histogramCellSignatures.h"
#include "../../DataRootObjectLIB/include/trackHitMem.h"
#include "../../DataRootObjectLIB/include/trackAnalogInformation.h"
#include "../../DataRootObjectLIB/include/trackfinderInputData.h"
#include "../../DataRootObjectLIB/include/trackData.h"
#include "../../DataRootObjectLIB/include/tables.h"
#include "../../DataRootObjectLIB/include/findableTrack.h"
#include "../../LutGeneratorLIB/include/lutImplementation.h"
#include "trackPropagationAnalysis.h"
#include "qualityAnalysis.h"
#include "momentumAnalysis.h"
#include "projectionAnalysis.h"
#include "magnetfieldAnalysis.h"
#include "magnetfieldFactorAnalysis.h"
#include "prelutRangeLayerAnalysis.h"
#include "histogramAnalysis.h"
#include "showAnalysis.h"
#include "totalAnalysis.h"
#include "hardwareAnalysis.h"
#include "houghPictures.h"
#include "visualAnalysis.h"
#include "peakfindingGeometryAnalysis.h"
#include "TStopwatch.h"
#include <list>


/*
 * This definitions sets the ranges for the evaluation of the magnetfield factors
 */
#define NUMBEROFFACTORS   501
#define FACTORMIN           5.0
#define FACTORMAX          10.0

#define HIGHOCCURENCEISGOODSIGNATURE				/**< This definition determines the actual good signature to be such one with the highest occurence instead of the one with the most counted ones. */


class tables;


typedef struct {

	/* data pointer */
	trackfinderInputData** eventData;
	histogramData**        histogram;
	trackData**            tracks;
	tables**               ratings;
	histogramSpace**       space;
	lutImplementation**    luts;
	/* momentum cut */
	double                 minP;
	/* filter type */
	bool                   initAutomatcFilterGeometry;
	/* configuration information enable */
	bool                   initConfiguration;
	bool                   initDetector;
	bool                   initEvent;
	bool                   initClassPriority;
	bool                   initMemory;
	bool                   initTime;
	/* track propagation analysis enable */
	bool                   initPropagationEventPoint;
	bool                   initPropagationEventHit;
	bool                   initPropagationTotalPoint;
	bool                   initPropagationTotalHit;
	bool                   initPropagationDisplay;
	/* EFGC analysis enable*/
	bool                   initQualityEFGCEventAbsolute;
	bool                   initQualityEFGCEventRelative;
	bool                   initQualityEFGCTotalAbsolute;
	bool                   initQualityEFGCTotalRelative;
	bool                   initMomentumEFGCEventPzEFGC;
	bool                   initMomentumEFGCEventPtEFGC;
	bool                   initMomentumEFGCTotalPzEFGC;
	bool                   initMomentumEFGCTotalPtEFGC;
	bool                   initProjectionEFGCNEvent12EFGCN;
	bool                   initProjectionEFGCNEvent13EFGCN;
	bool                   initProjectionEFGCNEvent32EFGCN;
	bool                   initProjectionEFGCNTotal12EFGCN;
	bool                   initProjectionEFGCNTotal13EFGCN;
	bool                   initProjectionEFGCNTotal32EFGCN;
	/* EFGC analysis display enable */
	bool                   initMomentumEvent;
	bool                   initMomentumTotal;
	bool                   initMomentumDisplay;
	bool                   initProjectionEvent;
	bool                   initProjectionTotal;
	bool                   initProjectionDisplay;
	/* magnetfield analysis enable */
	bool                   initMagnetfieldFactors;
	bool                   initMagnetfieldFactorDisplay;
	bool                   initMagnetfieldX;
	bool                   initMagnetfieldY;
	bool                   initMagnetfieldZ;
	/* magnetfield analysis display enable */
	bool                   initMagnetfieldDisplay;
	/* hardware analysis enable */
	bool                   initTracksPerColumn;
	bool                   initTracksPerRow;
	bool                   initTracksPerLayer;
	bool                   initHitReadoutDistribution;
	/* prelut range analysis enable */
	bool                   initPrelutRange;
	bool                   initPrelutRangeDisplay;
	unsigned short         initPrelutRangeDisplayMode;
	/* coding table generation thresholds */
	unsigned short         percentageOfHitsInSignature;
	unsigned short         percentageOfTracksForSignature;
	/* special analysis enable */
	unsigned long          analysisResultWarnings;
	unsigned long          analysisResultDisplays;
	unsigned long          analysisMoreResultWarnings;
	unsigned long          analysisMoreResultDisplays;

}initialParameter;

static const initialParameter defaultParameters = {

	/* data pointer */
	NULL, NULL, NULL, NULL, NULL, NULL,
	/* momentum cut */
	0.0,
	/* filter type */
	false,
	/* configuration information enable */
	false, false, true, true, false, false,
	/* track propagation analysis enable */
	false, false, false, false, false,
	/* EFGC analysis enable*/
	false, true, true, true, true, true, true, true, false, false, false, false, false, false,
	/* EFGC analysis display enable */
	false, true, true, false, false, false,
	/* magnetfield analysis enable */
	false, false, true, true, true,
	/* magnetfield analysis display enable */
	false,
	/* hardware analysis enable */
	false, false, false, false,
	/* prelut range analysis enable */
	false, false, CUTMAINRELATIVEDISPLAYMODE,
	/* coding table generation thresholds */
	0, 0,
	/* special analysis enable */
	0, 0, 0, 0

};

typedef struct {

	/* filename */
	const char* name;
	/* track propagation analysis enable */
	bool initPropagationToRoot;
	/* EFGC graphic displays */
	bool initMomentumToRoot;
	bool initProjectionToRoot;
	/* magnetfield displays */
	bool initMagnetfieldToRoot;
	/* histogram layer */
	bool initCreatedHistogramToRoot;
	bool initEncodedHistogramToRoot;
	bool initFilteredHistogramToRoot;
	/* just one layer */
	bool initJustOneCreatedHistogramToRoot;
	bool initJustOneEncodedHistogramToRoot;
	bool initJustOneFilteredHistogramToRoot;
	/* prelut range analysis enable */
	bool initPrelutRangeToRoot;

}initialFileParameter;

static const initialFileParameter defaultFileParameters = {

	/* filename */
	NULL,
	/* track propagation displays */
	false,
	/* EFGC graphic displays */
	false,
	false,
	/* magnetfield displays */
	false,
	/* histogram layer */
	false,
	false,
	false,
	/* just one layer */
	false,
	false,
	false,
	/* prelut range analysis enable */
	false

};


/* **************************************************************
 * CLASS analysis						 						*
 * **************************************************************/

class analysis {

 private:
  analysis(const analysis&);
  analysis& operator=(const analysis&);

protected:

	trackfinderInputData**            eventData;						/**< Object for accessing the input data. */
	trackData**                       tracks;							/**< Object for accessing the computed tracks. */
	tables**                          ratings;							/**< Object to access the tables to do the ratings. */
	histogramSpace**                  space;							/**< Object to store the needed values to compute the trackParameter object based on the trackCoordinates object. */
	lutImplementation**               luts;								/**< Object to make both neccessary look-up-tables accessible. */
	trackPropagationAnalysis*         trackPropagationAnalyser;			/**< Object for analysing the track propagation quality */
	qualityAnalysis*                  qualityAnalyser;					/**< Object for analysing the quality and writing on the standard output stream. */
	momentumAnalysis*                 momentumAnalyser;					/**< Object for analysing the quality and displaying it with ROOT graphics. */
	projectionAnalysis*               projectionAnalyser;				/**< Object for analysing the occupancy of the histogram and displaying it with ROOT graphics. */
	magnetfieldAnalysis*              magnetfieldAnalyser;				/**< Object for analysing the magnetfield and displaying it with ROOT graphics. */
	magnetfieldFactorAnalysis*        magnetfieldFactorAnalyser;		/**< Object for analysing the magnetfield factor and displaying it with ROOT graphics. */
	prelutRangeLayerAnalysis*         prelutRangeLayerAnalyser;			/**< Object for analysing the prelut range and displaying it with ROOT graphics. */
	histogramAnalysis*                histogramAnalyser;				/**< Object for analysing the histogram. */
	showAnalysis*                     showAnalyser;						/**< Object for showing and displaying some analysis it with ROOT graphics. */
	totalAnalysis*                    totalAnalyser;					/**< Object for evalution and showing of some total summaries of analysis results which cannot be evaluated and shown by the analysis itself. */
	hardwareAnalysis*                 hardwareAnalyser;					/**< Object for analysing the hardware prerequisites. */
	visualAnalysis*                   mcTrackVisualAnalyser;			/**< Object for visualizing the MCTracks with the corresponding hits.*/
	visualAnalysis*                   foundTrackVisualAnalyser;			/**< Object for visualizing the found tracks with the corresponding hits.*/
	peakfindingGeometryAnalysis*      peakfindingGeometryAnalyser;		/**< Object for analysing an automatic filter geometry. */
	double                            minimumP;							/**< Variable which defines the minimum momentum for a track which should be found. */
	bool                              configuration;					/**< Variable for enabling the configuration summary printed to the standard output stream. */
	bool                              detector;							/**< Variable for enabling the detector summary printed to the standard output stream. */
	bool                              event;							/**< Variable for enabling the event summary printed to the standard output stream. */
	bool                              classPriority;					/**< Variable for enabling the class summary printed to the standard output stream. */
	bool                              memory;							/**< Variable for enabling the memory summary printed to the standard output stream. */
	bool                              time;								/**< Variable for enabling the time summary printed to the standard output stream. */
	unsigned short                    percentageOfHitsInSignature;		/**< Variable to store the percentage of the number of hits which must be found to accept the signature. */
	unsigned short                    percentageOfTracksForSignature;	/**< Variable to store the percentage of the real tracks which must be found with the accepted signatures. */
	unsigned long                     analysisResultWarnings;			/**< Variable to store which result of an analysis should be printed on the standard output screen by a warning message. */
	unsigned long                     analysisResultDisplays;			/**< Variable to store which result of an analysis should be displayed with a graphical output screen. */
	unsigned long                     analysisMoreResultWarnings;		/**< Variable to store which result of an analysis should be printed on the standard output screen by a warning message. */
	unsigned long                     analysisMoreResultDisplays;		/**< Variable to store which result of an analysis should be displayed with a graphical output screen. */
	houghPictures*                    pictures;							/**< Object which draws the pictures for different Hough transforms. This pictures are static and are just needed fort he thesis. */
	TStopwatch*                       borderCreationTimer;				/**< Object to store the time for creating the borders for one event */
	TStopwatch*                       histogramCreationTimer;			/**< Object to store the time for creating the histogram layers for one event */
	TStopwatch*                       histogramEncodingTimer;			/**< Object to store the time for encoding the histogram layers for one event */
	TStopwatch*                       histogramDiagonalizingTimer;		/**< Object to store the time for diagonalizing the histogram layers for one event */
	TStopwatch*                       histogramPeakfindingTimer;		/**< Object to store the time for peak finding in the histogram layers for one event */
	TStopwatch*                       histogramFinalizingTimer;			/**< Object to store the time for finalizing the histogram layers for one event */
	TStopwatch*                       histogramResettingTimer;			/**< Object to store the time for resetting the histogram layers for one event */
	TStopwatch*                       trackPeakfindingTimer;			/**< Object to store the time for peak finding in the track candidates of neighbored layers for one event */
	double                            reservedSizeOfLBufferData;		/**< Variable to store the reserved size of the LBuffer data */
	double                            allocatedSizeOfLBufferData;		/**< Variable to store the allocated size of the LBuffer data */
	double                            usedSizeOfLBufferData;			/**< Variable to store the used size of the LBuffer data */

/**
 * method initializes the showAnalysis-object.
 */

	void initializeShowAnalysis();

/**
 * method initializes the totalAnalysis-object.
 */

	void initializeTotalAnalysis();

/**
 * method initializes the algorithm-evaluation.
 */

	void initializeAlgorithmEvaluation();

/**
 * method initializes the magnetfield-evaluation.
 */

	void initializeMagnetfieldEvaluation();

/**
 * method finalizes the magnetfield-evaluation.
 */

	void finalizeMagnetfieldEvaluation();

/**
 * method finalizes the algorithm-evaluation.
 */

	void finalizeAlgorithmEvaluation();

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing coordinates of the
 *        found track
 * @param parameter is the actual processing parameter of the
 *        found track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void peakToFakeTrack(trackCoordinates& coordinates, trackParameter& parameter);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing coordinates of the
 *        found track
 * @param parameter is the actual processing parameter of the
 *        found track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void peakToNoTrack(trackCoordinates& coordinates, trackParameter& parameter);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing coordinates of the
 *        found track
 * @param parameter is the actual processing parameter of the
 *        found track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void peakToOneTrack(trackCoordinates& coordinates, trackParameter& parameter, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing coordinates of the
 *        found track
 * @param parameter is the actual processing parameter of the
 *        found track
 * @param isClone marks this track as clone track
 * @param isWrong marks this track as wrong track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void peakToMoreTracks(trackCoordinates& coordinates, trackParameter& parameter, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void trackToNoPeak(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void trackToOnePeak(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void trackToMorePeaks(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 */

	void wellFoundTracks(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluateAlgorithm(std::streambuf* terminal = NULL)
 */

	void wrongFoundTracks(trackfinderInputTrack& actualTrack);

/**
 * This method evaluates if the given pattern is a track
 * pattern and returns the peak class from the P-table.
 * @param pattern is the value to search for
 * @return A value for the corresponding peak class
 */

	bitArray evaluateTrackPatternGradingP(bitArray& pattern);

/**
 * This method evaluates if the given pattern is a track
 * pattern and returns the peak class from the R-table.
 * @param pattern is the value to search for
 * @return A value for the corresponding peak class
 */

	bitArray evaluateTrackPatternGradingR(bitArray& pattern);

/**
 * This method evaluates if the given pattern is a track
 * pattern and returns the peak class from the C-table.
 * @param pattern is the value to search for
 * @return A value for the corresponding peak class
 */

	bitArray evaluateTrackPatternCoding(bitArray& pattern);

/**
 * This method evaluates the number of hits for an input
 * track.
 * @param actualTrack is the input track
 * @return The number of hits
 */

	unsigned short evaluateNumberOfRealTrackHits(trackfinderInputTrack* actualTrack);

/**
 * This method evaluates the number of hits for a given pattern
 * which represents a track.
 * @param pattern represents the hits in the stations
 * @return The number of hits
 */

	unsigned short evaluateNumberOfTrackHits(bitArray pattern);

/**
 * This method evaluates if the track is a  real track which
 * should be found.
 * @param  actualTrack is the input track
 * @return true if it is a track which should be found
 */

	bool evaluateFoundTrackIsRealTrack(trackfinderInputTrack* actualTrack);

/**
 * This method evaluates the track distribution of a peak.
 * @param track is the actual processing information of the
 *        found track
 * @return possibleTracks is a sorted list which consists
 *         of lists with hits according to the
 *         belonging input tracks
 */

	void evaluatePeakDistribution(trackDigitalInformation& track, specialListMem<trackHitMem>* possibleTracks);

/**
 * This method evaluates the category of a found track.
 * @param track is the input track
 * @param maximumHits is the number of hits belonging to
 *        this real track
 * @return isClone is true if the corresponding found
 *         track should be marked as clone. If isClone
 *         is false it is marked as real track.
 * @return isWrong is true if the corresponding found
 *         track should be marked as wrong
 */

	void evaluateFoundTracks(trackfinderInputTrack* track, unsigned short maximumHits, bool* isClone, bool* isWrong);

/**
 * This method evaluates how many tracks contribute to a peak.
 * @param track is the actual processing information of the
 *        found track
 * @return isClone is true if the corresponding found
 *         track should be marked as clone. If isClone
 *         is false it is marked as real track.
 * @return isWrong is true if the corresponding found
 *         track should be marked as wrong
 * @return isFake is true if the corresponding found
 *         track should be marked as fake
 */

	unsigned short evaluatePeakContribution(trackDigitalInformation& track, bool* isClone, bool* isWrong, bool* isFake);

/**
 * method previews the Hough space transformation for a track.
 */

	histogramCellSignature transformPreview(trackfinderInputTrack& track, trackCoordinates& estimation);

/**
 * method estimates the coordinates in the Hough space for a track.
 */

	bool estimateCoordinates(trackfinderInputTrack& track, trackCoordinates* coordinates);

/**
 * method computes the coordinates in the Hough space for a track.
 */

	bool computeCoordinates(trackfinderInputTrack& track, trackCoordinates* coordinates);

public:

/**
 * Default constructor
 */

	analysis();

/**
 * Constructor
 * @param name is the name of the file to place the graphical analysis
 * @param eventData is an object for accessing the input data.
 * @param histogram is an object for accessing the histogram
 * @param tracks is an object for accessing the computed tracks.
 * @param ratings is the object to access the tables for the ratings
 * @param space is the size of the histogram in the first three dimensions
 * @param minP is a variable which defines the minimum momentum for a track which should be found.
 * @param initConfiguration is a variable for enabling the summary printed to the standard output stream.
 * @param initDetector is a variable for enabling the summary printed to the standard output stream.
 * @param initEvent is a variable for enabling the summary printed to the standard output stream.
 * @param initClassPriority is a variable for enabling the summary printed to the standard output stream.
 * @param initMemory is a variable for enabling the summary about the memory printed to the standard output stream.
 * @param initTime is a variable for enabling the summary about the time printed to the standard output stream.
 * @param initQualityEFGCEventAbsolute initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones for each event with absolute numbers
 * @param initQualityEFGCEventRelative initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones for each event with percent numbers
 * @param initQualityEFGCTotalAbsolute initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events with absolute numbers
 * @param initQualityEFGCTotalRelative initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events with percent numbers
 * @param initMomentumEFGCEventPzEFGC initializes the graphical PZ-analysis for the efficiency, the fakes, the ghosts and the clones for each event
 * @param initMomentumEFGCEventPtEFGC initializes the graphical PT-analysis for the efficiency, the fakes, the ghosts and the clones for each event
 * @param initMomentumEFGCTotalPzEFGC initializes the graphical PZ-analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events
 * @param initMomentumEFGCTotalPtEFGC initializes the graphical PT-analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events
 * @param initMomentumEvent initializes the graphical analysis for each event
 * @param initMomentumTotal initializes the graphical analysis accumulating the results for all events
 * @param initMomentumDisplay initializes the display to show the results online
 * @param initMagnetfieldDisplay initializes the display to show the magnetfield
 * @param initMagnetfieldFactorDisplay initializes the display to show the factors of the magnetic field
 * @warning You can set just one display variable to true, because just one display can be drawn
 * @param initMagnetfieldFactors initializes the analysis for the factors of the magnetic field
 * @param initMagnetfieldX initializes the anaylsis of the x-dependency of the magnetic field
 * @param initMagnetfieldY initializes the anaylsis of the y-dependency of the magnetic field
 * @param initMagnetfieldZ initializes the anaylsis of the z-dependency of the magnetic field
 * @param initMomentumToRoot initializes the display of the algorithm to be written into the file
 * @param initMagnetfieldToRoot initializes the display of the magnetfields to be written into the file
 * @param initHistogramAnalysisToRoot initializes the object which can write the histogram into the file
 * @param initCreatedHistogramToRoot initializes the writing of the histogram after the creation step into the file
 * @param initEncodedHistogramToRoot initializes the writing of the histogram after the encoding step into the file
 * @param initFilteredHistogramToRoot initializes the writing of the histogram after the filtering step into the file
 * @param initJustOneCreatedHistogramToRoot initializes the writing of one layer of the histogram after the creation step into the file
 * @param initJustOneEncodedHistogramToRoot initializes the writing of one layer of the histogram after the encoding step into the file
 * @param initJustOneFilteredHistogramToRoot initializes the writing of one layer of the histogram after the filtering step into the file
 * @param initTracksPerColumn initializes the evaluation of the number of tracks per column in all histogram layers
 * @param initTracksPerRow initializes the evaluation of the number of tracks per row in all histogram layers
 * @param initTracksPerLayer initializes the evaluation of the number of tracks per histogram layer
 * @param percentageOfHitsInSignature is the percentage of the number of hits which must be found to accept the signature
 * @param percentageOfTracksForSignature is the percentage of the real tracks which must be found with the accepted signatures
 * @param analysisResultWarnings bit positions enable that the result of the corresponding analysis is printed on the standard output screen by a warning message
 * @param analysisResultDisplays bit positions enable that the result of the corresponding analysis is displayed with a graphical output screen
 */

	analysis(initialParameter parameters = defaultParameters,
		bool initHistogramAnalysisToRoot = false);
	analysis(initialParameter parameters = defaultParameters,
		initialFileParameter fileParameters = defaultFileParameters,
	    bool initCreatedHistogramToShow = false, bool initEncodedHistogramToShow = false,
	    bool initFilteredHistogramToShow = false, unsigned short initHistogramLayer = 0);

/**
 * Destructor
 */

	virtual ~analysis();

/**
 * This method initializes the object.
 * @param name is the name of the file to place the graphical analysis
 * @param eventData is an object for accessing the input data.
 * @param histogram is an object for accessing the histogram
 * @param tracks is an object for accessing the computed tracks.
 * @param ratings is the object to access the tables for the ratings
 * @param space is the size of the histogram in the first three dimensions
 * @param minP is a variable which defines the minimum momentum for a track which should be found.
 * @param initConfiguration is a variable for enabling the summary printed to the standard output stream.
 * @param initDetector is a variable for enabling the summary printed to the standard output stream.
 * @param initEvent is a variable for enabling the summary printed to the standard output stream.
 * @param initClassPriority is a variable for enabling the summary printed to the standard output stream.
 * @param initMemory is a variable for enabling the summary about the memory printed to the standard output stream.
 * @param initTime is a variable for enabling the summary about the time printed to the standard output stream.
 * @param initQualityEFGCEventAbsolute initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones for each event with absolute numbers
 * @param initQualityEFGCEventRelative initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones for each event with percent numbers
 * @param initQualityEFGCTotalAbsolute initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events with absolute numbers
 * @param initQualityEFGCTotalRelative initializes the quality analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events with percent numbers
 * @param initMomentumEFGCEventPzEFGC initializes the graphical PZ-analysis for the efficiency, the fakes, the ghosts and the clones for each event
 * @param initMomentumEFGCEventPtEFGC initializes the graphical PT-analysis for the efficiency, the fakes, the ghosts and the clones for each event
 * @param initMomentumEFGCTotalPzEFGC initializes the graphical PZ-analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events
 * @param initMomentumEFGCTotalPtEFGC initializes the graphical PT-analysis for the efficiency, the fakes, the ghosts and the clones accumulated for all computed events
 * @param initMomentumEvent initializes the graphical analysis for each event
 * @param initMomentumTotal initializes the graphical analysis accumulating the results for all events
 * @param initMomentumDisplay initializes the display to show the results online
 * @param initMagnetfieldDisplay initializes the display to show the magnetfield
 * @param initMagnetfieldFactorDisplay initializes the display to show the factors of the magnetic field
 * @warning You can set just one display variable to true, because just one display can be drawn
 * @param initMagnetfieldFactors initializes the analysis for the factors of the magnetic field
 * @param initMagnetfieldX initializes the anaylsis of the x-dependency of the magnetic field
 * @param initMagnetfieldY initializes the anaylsis of the y-dependency of the magnetic field
 * @param initMagnetfieldZ initializes the anaylsis of the z-dependency of the magnetic field
 * @param initMomentumToRoot initializes the display of the algorithm to be written into the file
 * @param initMagnetfieldToRoot initializes the display of the magnetfields to be written into the file
 * @param initHistogramAnalysisToRoot initializes the object which can write the histogram into the file
 * @param initCreatedHistogramToRoot initializes the writing of the histogram after the creation step into the file
 * @param initEncodedHistogramToRoot initializes the writing of the histogram after the encoding step into the file
 * @param initFilteredHistogramToRoot initializes the writing of the histogram after the filtering step into the file
 * @param initJustOneCreatedHistogramToRoot initializes the writing of one layer of the histogram after the creation step into the file
 * @param initJustOneEncodedHistogramToRoot initializes the writing of one layer of the histogram after the encoding step into the file
 * @param initJustOneFilteredHistogramToRoot initializes the writing of one layer of the histogram after the filtering step into the file
 * @param percentageOfHitsInSignature is the percentage of the number of hits which must be found to accept the signature
 * @param percentageOfTracksForSignature is the percentage of the real tracks which must be found with the accepted signatures
 * @param analysisResultWarnings bit positions enable that the result of the corresponding analysis is printed on the standard output screen by a warning message
 * @param analysisResultDisplays bit positions enable that the result of the corresponding analysis is displayed with a graphical output screen
 */

	void init(initialParameter parameters = defaultParameters,
		bool initHistogramAnalysisToRoot = false);
	void init(initialParameter parameters = defaultParameters,
		initialFileParameter fileParameters = defaultFileParameters,
	    bool initCreatedHistogramToShow = false, bool initEncodedHistogramToShow = false,
	    bool initFilteredHistogramToShow = false, unsigned short initHistogramLayer = 0);

/**
 * method searches all tracks to find the findable ones and to
 * compute their estimated position in the Hough space.
 * @param terminal is a buffer to place the process information
 */

	void setupFindableEstimatedTracks(std::list<findableTrack>* findableTracks, std::streambuf* terminal = NULL);

/**
 * method searches all tracks to find the findable ones and to
 * compute their computed position in the Hough space.
 * @param terminal is a buffer to place the process information
 */

	void setupFindableComputedTracks(std::list<findableTrack>* findableTracks, std::streambuf* terminal = NULL);

/**
 * method resets the peakfinding geometry for the actual layers
 */

	void resetPeakfindingLayerGeometryElements();

/**
 * method resets the actual used peakfinding geometry
 */

	void resetPeakfindingGeometryElements();

/**
 * method adds the geometry of the actual layer to the other layers
 */

	void addPeakfindingLayerGeometry(unsigned short layer);

/**
 * method updates the used peakfinding geometry by adding the geometry of the actual layers
 */

	void updatePeakfindingGeometry(trackCoordinates& center);

/**
 * method returns the peakfinding geometry
 * @return the automatic evaluated peakfinding geometry
 */

	peakfindingGeometry getPeakfindingGeometry();

/**
 * Method returns true if the track has a the specific track
 * pattern and has a specific P.
 * @param track is the track to be validated
 * @param minClassPriority is the minimum class which the track must have to be findable
 * @param globalTrackPattern can be used to get the trackPattern of this track. It will be not reset inside, just updated with true-values.
 * @return true if the track is a findable track
 */

	bool isFindableTrack(trackfinderInputTrack* track, bitArray minClassPriority, bitArray* globalTrackPattern);

/**
 * Method returns true if the track has a the specific track
 * pattern and has a specific P.
 * @param track is the track to be validated
 * @return true if the track is a findable standard track
 */

	bool isFindableStandardTrack(trackfinderInputTrack* track);

/**
 * Method returns the number of tracks with a the specific
 * track patterns of table-P and with a specific momentum.
 * @param minimumClassPriority is the minimal priority for
 *        track to be encountered
 * @return number of these tracks
 */

	int getNumberOfTracksWithP();
	int getNumberOfTracksWithP(bitArray minimumClassPriority);

/**
 * method evaluates the found tracks.
 * @param terminal is a buffer to place the process information
 */

	void evaluateAlgorithm(std::streambuf* terminal = NULL);

/**
 * method evaluates the track propagation goodness.
 * @param terminal is a buffer to place the process information
 */

	void evaluateTrackPropagationGoodness(std::streambuf* terminal = NULL);

/**
 * method evaluates the magnetfield factors.
 * @param isFirstEvent is used to deliver the information if the event is the first one. It is important for the initial values, if the averagingFactors flag is used
 * @param averagingFactors enables the averaging of the evaluated factors over events
 * @param terminal is a buffer to place the process information
 */

	void evaluateMagnetfieldFactors(bool isFirstEvent, bool averagingFactors, std::streambuf* terminal = NULL);

/**
 * method evaluates the prelut range.
 * @param isFirstEvent is used to deliver the information if the event is the first one. It is important for the initial values, if the averagingFactors flag is used
 * @param averagingFactors enables the averaging of the evaluated factors over events
 * @param chooseMainPrelutRange choose the range of the main analyser or the correct analyser
 * @param chooseConstraintPrelutRange choose between the maximum range or the range with regard to the constraint
 * @param terminal is a buffer to place the process information
 */

	void evaluatePrelutRange(bool isFirstEvent, bool averagingFactors, bool chooseMainPrelutRange, bool chooseConstraintPrelutRange, std::streambuf* terminal = NULL);

/**
 * method evaluates the prelut goodness.
 * @param terminal is a buffer to place the process information
 */

	void evaluatePrelutGoodness(std::streambuf* terminal = NULL);

/**
 * method evaluates the lut goodness.
 * @param terminal is a buffer to place the process information
 */

	void evaluateLutGoodness(std::streambuf* terminal = NULL);

/**
 * method evaluates the lut distribution.
 * @param numberOfTracksWithSignature is an array with bitArray::length which consists of the counted signatures for the MC Tracks
 * @param numberOfTracksWithPercentCorrectHits is an array with bitArray::length which consists of the counted signatures for the transformed MC Tracks in the correct histogram cell
 * @param numberOfTracksWithPercentHits is an array with bitArray::length which consists of the counted signatures for the transformed MC Tracks in one cell
 * @param distanceOfMaximumCellAndCorrectCell is a variable to store the distance of the correct cell and the cell with the maximum hits
 * @param numberOfTracksWithGoodLutSignature is a variable to store the number of tracks which have a good lut signature
 * @param terminalString is a string to display for the processing information
 * @param terminal is a buffer to place the process information
 */

	void evaluateLutDistribution(unsigned int* numberOfTracksWithSignature, unsigned int* numberOfTracksWithPercentCorrectHits, unsigned int* numberOfTracksWithPercentHits, double* distanceOfMaximumCellAndCorrectCell, unsigned long* numberOfTracksWithGoodLutSignature, std::string terminalString = "", std::streambuf* terminal = NULL);

/**
 * method evaluates the goodness of both luts.
 * @param numberOfTracksWithHits is an array with bitArray::length which consists of the counted signatures for the MC Tracks based on both look-up-tables
 * @param terminalString is a string to display for the processing information
 * @param terminal is a buffer to place the process information
 */

	void evaluateBothLutsDistribution(unsigned int* numberOfTracksWithHits, std::string terminalString = "", std::streambuf* terminal = NULL);

/**
 * method evaluates the magnetfield.
 * @param terminal is a buffer to place the process information
 */

	void evaluateMagnetfield(std::streambuf* terminal = NULL);

/**
 * method compares the magnetfield with the factors.
 */

	void evaluateMagnetfieldVSFactors(double constXPos = 0, double constYPos = 0, bool median = true);

/**
 * method evaluates the goodness of the used analytic formula.
 */

	void evaluateHoughTransformGoodness(std::streambuf* terminal = NULL);

/**
 * method evaluates the goodness of the used quatization.
 */

	void evaluateQuantizationGoodness(std::streambuf* terminal = NULL);

/**
 * method evaluates the goodness of the peak distance.
 */

	void evaluatePeakDistanceGoodness(std::streambuf* terminal = NULL);

/**
 * method evaluates the neccessary files for a CELL BE simulation.
 */

	void evaluateCellSimulationFiles(std::string hitFileName, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal = NULL);
	void evaluateCellSimulationFiles(std::string hitFileName, std::string prelutFileName, std::string lutFileName, unsigned int eventNumber, std::streambuf* terminal = NULL);

/**
 * method initializes the configuration-Analysis.
 */

	void initConfigurationAnalysis(bool enable = true);

/**
 * method initializes the configuration-Analysis.
 */

	void initDetectorAnalysis(bool enable = true);

/**
 * method initializes the memory-Analysis.
 */

	void initMemoryAnalysis(bool enable = true);

/**
 * method initializes the time-Analysis.
 */

	void initTimeAnalysis(bool enable = true);

/**
 * method initializes the event-Analysis.
 */

	void initEventAnalysis(bool enable = true);

/**
 * method initializes the classPriority-Analysis.
 */

	void initClassPriorityAnalysis(bool enable = true);

/**
 * method initializes the type of the filter.
 */

	void initAutomaticFilterGeometryAnalysis(histogramData** histogram, bool enable = true);

/**
 * method initializes the number of displays.
 * @param numberOfDisplays represents the value number
 */

	void initTrackPropagationNumberOfDisplays(unsigned short numberOfDisplays);

/**
 * method initializes the qualityEFGCEventAbsolute-Analysis.
 * @param enable enables or disables this analysis
 */

	void initTrackPropagationEventPointAnalysis(bool enable = true);
	void initTrackPropagationEventHitAnalysis(bool enable = true);
	void initTrackPropagationTotalPointAnalysis(bool enable = true);
	void initTrackPropagationTotalHitAnalysis(bool enable = true);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initTrackPropagationAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method initializes the track propagation's display.
 * @param enable enables or disables this analysis
 */

	void initTrackPropagationAnalysisDisplay(bool enable = true);

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
 * method initializes the momentumEFGCEventPzE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzF-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPzEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPzEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCEventPtE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtF-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCEventPtEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCEventPtEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPzE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzF-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPzEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPzEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * method initializes the momentumEFGCTotalPtE-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtEAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtF-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtFAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtG-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtGAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the momentumEFGCTotalPtEFGC-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initMomentumEFGCTotalPtEFGCAnalysis(bool enable = true, int nBins = 100, int xMin = 0, int xMax = 5);

/**
 * method initializes the projectionEFGCNEvent12E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12EAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent12F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12FAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent12G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12GAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent12C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12CAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent12N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12NAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent12EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent12EFGCNAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent13E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13EAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNEvent13F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13FAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNEvent13G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13GAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNEvent13C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13CAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNEvent13N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13NAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNEvent13EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent13EFGCNAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNEvent32E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32EAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent32F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32FAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent32G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32GAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent32C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32CAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent32N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32NAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNEvent32EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNEvent32EFGCNAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal12E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12EAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal12F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12FAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal12G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12GAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal12C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12CAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal12N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12NAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal12EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal12EFGCNAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal13E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13EAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNTotal13F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13FAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNTotal13G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13GAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNTotal13C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13CAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNTotal13N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13NAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNTotal13EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal13EFGCNAnalysis(bool enable = true, int nXBins = maxDim1, int nYBins = maxDim3);

/**
 * method initializes the projectionEFGCNTotal32E-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32EAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal32F-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32FAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal32G-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32GAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal32C-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32CAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal32N-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32NAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * method initializes the projectionEFGCNTotal32EFGCN-Analysis.
 * @param enable enables or disables this analysis
 * @param nXBins sets the binning for this ROOT-histogram in the x-dimension
 * @param nYBins sets the binning for this ROOT-histogram in the y-dimension
 */

	void initProjectionEFGCNTotal32EFGCNAnalysis(bool enable = true, int nXBins = maxDim3, int nYBins = maxDim2);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initMomentumAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method initializes the root directory for the displays.
 * @param enable is the enabling-flag
 * @param name is the name of the file. If null is supplied it is written to the standard root output file
 * @param justUpdate just updates the file instead of recreating
 */

	void initProjectionAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method initializes the graphic's display.
 */

	void initMomentumAnalysisDisplay(bool enable = true);

/**
 * This method initializes the graphic's display.
 */

	void initProjectionAnalysisDisplay(bool enable = true);

/**
 * method initializes the initMagnetfieldFactorAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 */

	void initMagnetfieldConstantAnalysis(bool enable = true, int nBins = NUMBEROFFACTORS, double min = FACTORMIN, double max = FACTORMAX);

/**
 * method initializes the initMagnetfieldXBxAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXBxAnalysis(bool enable = true, int nBins = 100, double min = -50, double max = 50, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldXByAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXByAnalysis(bool enable = true, int nBins = 100, double min = -50, double max = 50, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldXBzGAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXBzAnalysis(bool enable = true, int nBins = 100, double min = -50, double max = 50, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldXAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldXAnalysis(bool enable = true, int nBins = 100, double min = -300, double max = 300, double constValDim1 = 20, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYBxAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYBxAnalysis(bool enable = true, int nBins = 100, double min = -50, double max = 50, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYByAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYByAnalysis(bool enable = true, int nBins = 100, double min = -50, double max = 50, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYBzAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYBzAnalysis(bool enable = true, int nBins = 100, double min = -50, double max = 50, double constValDim1 = 0, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldYAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldYAnalysis(bool enable = true, int nBins = 100, double min = -300, double max = 300, double constValDim1 = 20, double constValDim2 = 50);

/**
 * method initializes the initMagnetfieldZBxAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZBxAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldZByAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZByAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldZBzAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZBzAnalysis(bool enable = true, int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldZAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldZAnalysis(bool enable = true, int nBins = 100, double min = -300, double max = 300, double constValDim1 = 0, double constValDim2 = 0);

/**
 * method initializes the initMagnetfieldAnalysis-Analysis.
 * @param enable enables or disables this analysis
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initMagnetfieldAnalysis(bool enable = true, int nBins = 100, double xMin = 0, double yMin = -50, double zMin = -50, double xMax = 100, double yMax = 50, double zMax = 50, double constValDim1 = 0, double constValDim2 = 0, double constValDim3 = 50);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initMagnetfieldAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method initializes the magnetfield's display.
 */

	void initMagnetfieldAnalysisDisplay(bool enable = true);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initMagnetfieldConstantAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method initializes the magnetfield constant's display.
 */

	void initMagnetfieldConstantAnalysisDisplay(bool enable = true);

/**
 * method initializes the initPrelutRangeAnalysis-Analysis.
 * @param enable enables or disables this analysis
 */

	void initPrelutRangeAnalysis(bool enable = true);
	void initPrelutRangeAnalysis(bool enable, unsigned short numberOfDisplays);
	void initPrelutRangeAnalysis(bool enable, unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);
	void initPrelutRangeAnalysis(bool enable, unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initPrelutRangeAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method initializes the magnetfield constant's display.
 * @param enable enables or disables this analysis
 * @param displayMode is the mode of the display drawing functionality
 */

	void initPrelutRangeAnalysisDisplay(bool enable = true, unsigned short displayMode = CUTMAINRELATIVEDISPLAYMODE);

/**
 * This method initializes the histogram analysis.
 */

	void initHistogramAnalysis(histogramData** histogram, bool enable = true);

/**
 * This method initializes the root directory for the analysis.
 * @param histogram is the object containing the information
 * @param enableCreated enables or disables the histogram analysis after the creation
 * @param enableEncoded enables or disables the histogram analysis after the encoding
 * @param enableFiltered enables or disables the histogram analysis after the filtering
 * @param name is the name of the file to place the analysis
 */

	void initHistogramAnalysis(bool enableCreated = true, bool enableEncoded = true, bool enableFiltered = true,
							   bool justOneCreated = false, bool justOneEncoded = false, bool justOneFiltered = false,
							   bool showCreated = false, bool showEncoded = false, bool showFiltered = false,
							   unsigned short layer = 0, const char* name = NULL);

/**
 * method returns true if the configuration-Analysis is enabled.
 */

	bool isConfigurationAnalysisEnabled();

/**
 * method returns true if the detector-Analysis is enabled.
 */

	bool isDetectorAnalysisEnabled();

/**
 * method returns true if the memory-Analysis is enabled.
 */

	bool isMemoryAnalysisEnabled();

/**
 * method returns true if the time-Analysis is enabled.
 */

	bool isTimeAnalysisEnabled();

/**
 * method returns true if the event-Analysis is enabled.
 */

	bool isEventAnalysisEnabled();

/**
 * method returns true if the classPriority-Analysis is enabled.
 */

	bool isClassPriorityAnalysisEnabled();

/**
 * method returns true if the filterGeometry-Analysis is enabled.
 */

	bool isAutomaticFilterGeometryAnalysisEnabled();

/**
 * method returns true if the track-propagation-Analysis is enabled.
 */

	bool isTrackPropagationEventAnalysisEnabled();
	bool isTrackPropagationTotalAnalysisEnabled();

/**
 * method returns true if the quality-Analysis for each event
 * is enabled.
 */

	bool isQualityEventAnalysisEnabled();

/**
 * method returns true if the quality-Analysis over all events
 * is enabled.
 */

	bool isQualityTotalAnalysisEnabled();

/**
 * method returns true if the momentumEFGC-Analysis for each
 * event is enabled.
 */

	bool isMomentumEFGCEventAnalysisEnabled();

/**
 * method returns true if the projectionEFGCN-Analysis for each
 * event is enabled.
 */

	bool isProjectionEFGCNEventAnalysisEnabled();

/**
 * method returns true if the momentumEFGC-Analysis over all
 * events is enabled.
 */

	bool isMomentumEFGCTotalAnalysisEnabled();

/**
 * method returns true if the projectionEFGCN-Analysis over all
 * events is enabled.
 */

	bool isProjectionEFGCNTotalAnalysisEnabled();

/**
 * method returns true if the track-propagation-to-root-analysis is enabled.
 */

	bool isTrackPropagationToRootEnabled();

/**
 * method returns true if the graphic-to-root-analysis is enabled.
 */

	bool isMomentumToRootEnabled();

/**
 * method returns true if the graphic-to-root-analysis is enabled.
 */

	bool isProjectionToRootEnabled();

/**
 * method returns true if the magnetfield-to-root-analysis is enabled.
 */

	bool isMagnetfieldToRootEnabled();

/**
 * method returns true if the magnetfield-constant-to-root-analysis is enabled.
 */

	bool isMagnetfieldFactorToRootEnabled();

/**
 * method returns true if the prelut-range-to-root-analysis is enabled.
 */

	bool isPrelutRangeToRootEnabled();

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

/**
 * This method returns true if the track-propagation's display is enabled.
 */

	bool isTrackPropagationDisplayEnabled();

/**
 * method updates the track-propagation-Analysis display.
 */

	void trackPropagationAnalysisUpdate();

/**
 * method draws the track-propagation-Analysis display.
 * @param preventPointDraw consists of flags which enables or disables the drawing of the point result displays separately
 * @param preventHitDraw consists of flags which enables or disables the drawing of the hit result displays separately
 */

	void trackPropagationAnalysisDraw(bitArray preventPointDraw = 0, bitArray preventHitDraw = 0);

/**
 * method returns true if the graphicAnalysis for each event
 * is enabled.
 */

	bool isMomentumEventAnalysisEnabled();

/**
 * method returns true if the graphicAnalysis over all events
 * is enabled.
 */

	bool isMomentumTotalAnalysisEnabled();

/**
 * This method returns true if the graphic's display is enabled.
 */

	bool isMomentumDisplayEnabled();

/**
 * method updates the graphic-Analysis display.
 */

	void momentumAnalysisUpdate();

/**
 * method draws the graphic-Analysis display.
 */

	void momentumAnalysisDraw();

/**
 * method returns true if the graphicAnalysis for each event
 * is enabled.
 */

	bool isProjectionEventAnalysisEnabled();

/**
 * method returns true if the graphicAnalysis over all events
 * is enabled.
 */

	bool isProjectionTotalAnalysisEnabled();

/**
 * This method returns true if the graphic's display is enabled.
 */

	bool isProjectionDisplayEnabled();

/**
 * method updates the graphic-Analysis display.
 */

	void projectionAnalysisUpdate();

/**
 * method draws the graphic-Analysis display.
 */

	void projectionAnalysisDraw();

/**
 * method returns true if the magnetfieldAnalysis
 * is enabled.
 */

	bool isMagnetfieldAnalysisEnabled();

/**
 * This method returns true if the magnetfield's display is enabled.
 */

	bool isMagnetfieldDisplayEnabled();

/**
 * method updates the magnetfield-Analysis display.
 */

	void magnetfieldAnalysisUpdate();

/**
 * method draws the magnetfield-Analysis display.
 */

	void magnetfieldAnalysisDraw();

/**
 * This method returns true if the magnetfield factor's display is enabled.
 */

	bool isMagnetfieldFactorDisplayEnabled();

/**
 * method updates the magnetfield factor-Analysis display.
 */

	void magnetfieldFactorAnalysisUpdate();

/**
 * method draws the magnetfield factor-Analysis display.
 * @param preventDraw consists of flags which enables or disables the drawing of displays separately
 */

	void magnetfieldFactorAnalysisDraw(bitArray preventDraw = 0);

/**
 * This method returns true if the prelut range's display is enabled.
 */

	bool isPrelutRangeDisplayEnabled();

/**
 * method updates the prelut range-Analysis display.
 */

	void prelutRangeAnalysisUpdate();

/**
 * method draws the prelut range-Analysis display.
 * @param preventStationDraw consists of flags which enables or disables the drawing of station result displays separately
 * @param preventStationSumDraw is a flag to prevent the drawing of the combined station display
 * @param preventConstraintDraw consists of flags which enables or disables the drawing of constraint result displays separately
 * @param preventConstraintSumDraw is a flag to prevent the drawing of the combined constraint display
 */

	void prelutRangeAnalysisDraw(bitArray preventStationDraw = 0, bool preventStationSumDraw = false, bitArray preventConstraintDraw = 0, bool preventConstraintSumDraw = false, bool preventRelativeDraw = false);

/**
 * method writes the track-propagation-Analysis into
 * a root file.
 */

	void trackPropagationEventAnalysisWrite(int eventNumber);
	void trackPropagationTotalAnalysisWrite(int eventNumber);

/**
 * method writes the momentumEvent-Analysis for each event into
 * a root file.
 */

	void momentumEventAnalysisWrite(int eventNumber);

/**
 * method writes the momentumTotal-Analysis over all events into
 * a root file.
 */

	void momentumTotalAnalysisWrite(int eventNumber);

/**
 * method writes the projectionEvent-Analysis for each event into
 * a root file.
 */

	void projectionEventAnalysisWrite(int eventNumber);

/**
 * method writes the projectionTotal-Analysis over all events into
 * a root file.
 */

	void projectionTotalAnalysisWrite(int eventNumber);

/**
 * method writes the magnetfield-Analysis into
 * a root file.
 */

	void magnetfieldAnalysisWrite();

/**
 * method writes the magnetfieldFactor-Analysis into
 * a root file.
 */

	void magnetfieldFactorAnalysisWrite(int eventNumber);

/**
 * method writes the prelut range-Analysis into
 * a root file.
 */

	void prelutRangeAnalysisWrite(int eventNumber);

/**
 * method initializes the momentumEFGCEventAnalysis.
 */

	void initMomentumEventAnalysis(bool enable = true);

/**
 * method initializes the momentumEFGCTotalAnalysis.
 */

	void initMomentumTotalAnalysis(bool enable = true);

/**
 * method initializes the projectionEFGCNEventAnalysis.
 */

	void initProjectionEventAnalysis(bool enable = true);

/**
 * method initializes the projectionEFGCNTotalAnalysis.
 */

	void initProjectionTotalAnalysis(bool enable = true);

/**
 * method returns a string representing the result of the
 * momentumEvent-Analysis.
 */

	std::string getMomentumEventAnalysis();

/**
 * method returns a string representing the result of the
 * momentumTotal-Analysis.
 */

	std::string getMomentumTotalAnalysis();

/**
 * method returns a string representing the result of the
 * magnetfield-Analysis.
 */

	std::string getMagnetfieldAnalysis();

/**
 * method returns if the created histogram layer can
 * be written into the file.
 */

	bool isWritingCreatedHistogramLayerEnabled();

/**
 * method returns if the encoded histogram layer can
 * be written into the file.
 */

	bool isWritingEncodedHistogramLayerEnabled();

/**
 * method returns if the filtered histogram layer can
 * be written into the file.
 */

	bool isWritingFilteredHistogramLayerEnabled();

/**
 * method returns if just one created histogram layer can
 * be accessed.
 */

	bool isWritingJustOneCreatedHistogramLayerEnabled();

/**
 * method returns if just one encoded histogram layer can
 * be accessed.
 */

	bool isWritingJustOneEncodedHistogramLayerEnabled();

/**
 * method returns if just one filtered histogram layer can
 * be accessed.
 */

	bool isWritingJustOneFilteredHistogramLayerEnabled();

/**
 * method returns if the created histogram layer can
 * be shown.
 */

	bool isShowingCreatedHistogramLayerEnabled();

/**
 * method returns if the encoded histogram layer can
 * be shown.
 */

	bool isShowingEncodedHistogramLayerEnabled();

/**
 * method returns if the filtered histogram layer can
 * be shown.
 */

	bool isShowingFilteredHistogramLayerEnabled();

/**
 * This method returns the index of the histogram layer
 * which should be accessed.
 */

	unsigned short getWritingHistogramLayerIndex();

/**
 * method writes the actual created histogram layer as the layer
 * 'layer' into the file.
 */

	void createdHistogramLayerWrite(unsigned short layer);

/**
 * method writes the actual encoded histogram layer as the layer
 * 'layer' into the file.
 */

	void encodedHistogramLayerWrite(unsigned short layer);

/**
 * method writes the actual filtered histogram layer as the layer
 * 'layer' into the file.
 */

	void filteredHistogramLayerWrite(unsigned short layer);

/**
 * This method shows the histogram after the filtering
 * @warning To use this function the corresponding ...Write function must be used first.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 * @param drawOption sets the options for drawing the TH2S object
 */

	void createdHistogramLayerShow(unsigned short layer, std::string drawOption = "");
	void createdHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption = "");

/**
 * This method shows the histogram after the creation
 * @warning To use this function the corresponding ...Write function must be used first.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 * @param drawOption sets the options for drawing the TH2S object
 */

	void encodedHistogramLayerShow(unsigned short layer, std::string drawOption = "");
	void encodedHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption = "");

/**
 * This method shows the histogram after the encoding
 * @warning To use this function the corresponding ...Write function must be used first.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 * @param drawOption sets the options for drawing the TH2S object
 */

	void filteredHistogramLayerShow(unsigned short layer, std::string drawOption = "");
	void filteredHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption = "");

/**
 * method returns the number of active detector stations
 */

	unsigned int getNumberOfActiveStations();

/**
 * method initializes the hardwareTracksPerColumnAnalysis.
 */

	void initHardwareTracksPerColumnAnalysis(bool enable = true);

/**
 * method initializes the hardwareTracksPerRowAnalysis.
 */

	void initHardwareTracksPerRowAnalysis(bool enable = true);

/**
 * method initializes the hardwareTracksPerLayerAnalysis.
 */

	void initHardwareTracksPerLayerAnalysis(bool enable = true);

/**
 * method initializes the hardwareHistogramLayerDistribution-
 * Analysis.
 */

	void initHardwareHitReadoutDistributionAnalysis(bool enable);

/**
 * method returns if the analysis for the number of
 * tracks in all columns of the histogram layers is
 * enabled or not
 */

	bool isNumberOfTracksInAllColumnsAnalysisEnabled();

/**
 * method returns if the analysis for the number of
 * tracks in all rows of the histogram layers is
 * enabled or not
 */

	bool isNumberOfTracksInAllRowsAnalysisEnabled();

/**
 * method returns if the analysis for the number of
 * tracks in all histogram layers is enabled or not
 */

	bool isNumberOfTracksInAllLayersAnalysisEnabled();

/**
 * method returns if the analysis for the histogram
 * layer distribution is enabled or not
 */

	bool isHitReadoutDistributionAnalysisEnabled();

/**
 * This method evaluates the minimal, the maximal and
 * the average number of tracks in all columns of the histogram.
 */

	void evaluateNumberOfTracksInAllColumns(std::streambuf* terminal);

/**
 * This method evaluates the minimal, the maximal and
 * the average number of tracks in all rows of the histogram.
 */

	void evaluateNumberOfTracksInAllRows(std::streambuf* terminal);

/**
 * This method evaluates the distribution of the number of tracks
 * in all layers of the histogram.
 */

	void evaluateNumberOfTracksInAllLayers(std::streambuf* terminal = NULL);

/**
 * The method evaluates the distribution how often a hit must be
 * inserted in a layer.
 */

	void evaluateHitReadoutDistribution(histogramData* histogram, std::streambuf* terminal = NULL);

/**
 * This method shows the minimal, the maximal and
 * the average number of tracks in all columns of the histogram.
 */

	void showNumberOfTracksInAllColumns();

/**
 * This method shows the minimal, the maximal and
 * the average number of tracks in all rows of the histogram.
 */

	void showNumberOfTracksInAllRows();

/**
 * This method shows the distribution of the number of
 * tracks in all layers of the histogram.
 */

	void showNumberOfTracksInAllLayers();

/**
 * This method shows the distribution of the histogram
 * layers for the hits.
 */

	void showHitReadoutDistribution(bool readoutColumnsInParallel);

/**
 * This method resets the totalAnalysis object with
 * its actual results.
 */

	void resetTotalAnalysis();

/**
 * This method adds the actual number of Hough transform
 * corrections for the total multi event summary to the
 * totalAnalysis object.
 */

	void addNumberOfHoughTransformCorrections(unsigned long numberOfCorrections, unsigned long numberOfCoordCorrections);

/**
 * This method adds the actual number of tracks which cannot be found
 */

	void addNumberOfTracksWhichCannotBeFound(unsigned long numberOfTracksWhichCannotBeFound);

/**
 * This method adds the actual number of tracks which have an
 * acceptable signature belonging to prelut.
 */

	void addNumberOfTracksWithGoodPrelutSignature(unsigned long numberOfTracksWithGoodPrelutSignature);

/**
 * This method adds the actual number of tracks which have an
 * acceptable signature belonging to lut.
 */

	void addNumberOfTracksWithGoodLutSignature(unsigned long numberOfTracksWithGoodLutSignature);

/**
 * This method adds the actual signature to the
 * acceptable signatures belonging to prelut.
 */

	void addGoodPrelutSignature(bitArray goodPrelutSignature, bitArray classification);

/**
 * This method adds the actual signature to the
 * acceptable signatures belonging to lut.
 */

	void addGoodLutSignature(bitArray goodLutSignature, bitArray classification);

/**
 * This method prints the actual results of the totalAnalysis.
 */

	void printTotalAnalysis();

/**
 * This method displays all arrays of the totalAnalysis object.
 */

	void displayTotalAnalysis();

/**
 * method returns true if the MCTrack distribution-Analysis is
 * enabled.
 */

	bool isMCTrackVisualizationAnalysisEnabled();

/**
 * This method displays all MCTracks with corresponding hits.
 * @param if trackIndex < 0, all tracks would be displayed, else just the track with the given index
 */

	std::string displayMCTracks(int trackIndex = -1);

/**
 * method returns true if the found track distribution-Analysis is
 * enabled.
 */

	bool isFoundTrackVisualizationAnalysisEnabled();

/**
 * method returns true if the display of the peakfinding
 * geometry is enabled.
 */

	bool isPeakfindingGeometryDisplayEnabled();

/**
 * method returns true if the display of the projected
 * peakfinding geometry is enabled.
 */

	bool isProjectedPeakfindingGeometryDisplayEnabled();

/**
 * method returns true if the display of the covered
 * peakfinding geometry is enabled.
 */

	bool isCoveredPeakfindingGeometryDisplayEnabled();

/**
 * method returns true if the display of the covered and
 * projected peakfinding geometry is enabled.
 */

	bool isCoveredProjectedPeakfindingGeometryDisplayEnabled();

/**
 * method returns true if the message of the peakfinding
 * geometry is enabled.
 */

	bool isPeakfindingGeometryMessageEnabled();

/**
 * method returns true if the message of the projected
 * peakfinding geometry is enabled.
 */

	bool isProjectedPeakfindingGeometryMessageEnabled();

/**
 * method returns true if the message of the covered
 * peakfinding geometry is enabled.
 */

	bool isCoveredPeakfindingGeometryMessageEnabled();

/**
 * method returns true if the message of the covered and
 * projected peakfinding geometry is enabled.
 */

	bool isCoveredProjectedPeakfindingGeometryMessageEnabled();

/**
 * This method displays all found tracks with corresponding hits.
 * @param if trackIndex < 0, all tracks would be displayed, else just the track with the given index
 */

	std::string displayFoundTracks(int trackIndex = -1);

/**
 * This method returns the value for the percentage of
 * hits which must be in a signature which is accepted
 */

	unsigned short getPercentageOfHitsInSignature();

/**
 * This method returns the value for the percentage of real
 * tracks which must be found with the accepted signatures
 */

	unsigned short getPercentageOfTracksForSignature();

/**
 * This method returns the actual measured time for creating the
 * border objects.
 */

	void borderCreationTimerReset();
	void borderCreationTimerStart(bool reset = false);
	void borderCreationTimerStop();

/**
 * This method returns the actual measured time for creating the
 * histogram layers.
 */

	void histogramCreationTimerReset();
	void histogramCreationTimerStart(bool reset = false);
	void histogramCreationTimerStop();

/**
 * This method returns the actual measured time for encoding the
 * histogram layers.
 */

	void histogramEncodingTimerReset();
	void histogramEncodingTimerStart(bool reset = false);
	void histogramEncodingTimerStop();

/**
 * This method returns the actual measured time for diagonalization the
 * histogram layers.
 */

	void histogramDiagonalizingTimerReset();
	void histogramDiagonalizingTimerStart(bool reset = false);
	void histogramDiagonalizingTimerStop();

/**
 * This method returns the actual measured time for peak finding the
 * histogram layers.
 */

	void histogramPeakfindingTimerReset();
	void histogramPeakfindingTimerStart(bool reset = false);
	void histogramPeakfindingTimerStop();

/**
 * This method returns the actual measured time for finalizing the
 * histogram layers.
 */

	void histogramFinalizingTimerReset();
	void histogramFinalizingTimerStart(bool reset = false);
	void histogramFinalizingTimerStop();

/**
 * This method returns the actual measured time for resetting the
 * histogram layers.
 */

	void histogramResettingTimerReset();
	void histogramResettingTimerStart(bool reset = false);
	void histogramResettingTimerStop();

/**
 * This method returns the actual measured time for peak finding in the
 * track candidates of neighbored layers.
 */

	void trackPeakfindingTimerReset();
	void trackPeakfindingTimerStart(bool reset = false);
	void trackPeakfindingTimerStop();

/**
 * This method returns the actual measured time for creating the
 * border objects.
 */

	double getBorderCreationRealTime();
	double getBorderCreationCpuTime();

/**
 * This method returns the actual measured time for creating the
 * histogram layers.
 */

	double getHistogramCreationRealTime();
	double getHistogramCreationCpuTime();

/**
 * This method returns the actual measured time for encoding the
 * histogram layers.
 */

	double getHistogramEncodingRealTime();
	double getHistogramEncodingCpuTime();

/**
 * This method returns the actual measured time for diagonalization the
 * histogram layers.
 */

	double getHistogramDiagonalizingRealTime();
	double getHistogramDiagonalizingCpuTime();

/**
 * This method returns the actual measured time for peak finding the
 * histogram layers.
 */

	double getHistogramPeakfindingRealTime();
	double getHistogramPeakfindingCpuTime();

/**
 * This method returns the actual measured time for finalizing the
 * histogram layers.
 */

	double getHistogramFinalizingRealTime();
	double getHistogramFinalizingCpuTime();

/**
 * This method returns the actual measured time for resetting the
 * histogram layers.
 */

	double getHistogramResettingRealTime();
	double getHistogramResettingCpuTime();

/**
 * This method returns the actual measured time for peak finding in the
 * track candidates of neighbored layers.
 */

	double getTrackPeakfindingRealTime();
	double getTrackPeakfindingCpuTime();

/**
 * This method evaluates the size of the memory for
 * the source data.
 */

	void evaluateSizeOfLBufferData();

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfLBufferData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfLBufferData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfLBufferData(unsigned short dimension = 0);

/**
 * This method returns a formated string containing the time
 * analysis.
 */

	std::string formatRealTimeAnalysis();
	std::string formatRealTimeSummaryAnalysis(double trackfinderRealTime, double eventRealTime);
	std::string formatCpuTimeAnalysis();
	std::string formatCpuTimeSummaryAnalysis(double trackfinderCpuTime, double eventCpuTime);
	std::string formatTimeAnalysis(double trackfinderRealTime, double eventRealTime, double trackfinderCpuTime, double eventCpuTime);

/**
 * This method returns a formated string containing the time
 * analysis.
 */

	std::string formatReservedMemoryAnalysis(histogramData* histogram = NULL, double inputReservedSize = 0);
	std::string formatAllocatedMemoryAnalysis(histogramData* histogram = NULL, double inputAllocatedSize = 0);
	std::string formatUsedMemoryAnalysis(histogramData* histogram = NULL, double inputUsedSize = 0);
	std::string formatMemoryAnalysis(histogramData* histogram = NULL, double inputReservedSize = 0, double inputAllocatedSize = 0, double inputUsedSize = 0);

};

#endif
