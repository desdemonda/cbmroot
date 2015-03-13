//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
///
/// *******************************************************************
/// 
/// Author(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   This is the object to put the trackfinding algorithm into the
///   root framework.
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2007-05-14 15:43:47 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _CBMHOUGHSTSTRACKFINDER_H
#define _CBMHOUGHSTSTRACKFINDER_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataRootObjectLIB/include/trackfinderInputData.h"
#include "../../DataRootObjectLIB/include/histogramData.h"
#include "../../DataRootObjectLIB/include/trackData.h"
#include "../../DataRootObjectLIB/include/tables.h"
#include "../../InputLIB/include/inputAscii.h"
#include "../../InputLIB/include/inputFRoot.h"
#include "../../LutGeneratorLIB/include/lutImplementation.h"
#include "../../TrackfinderLIB/include/trackfinder.h"
#include "../../OutputResultLIB/include/outputFTrack.h"
#include "../../AnalysisLIB/include/analysis.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../../MiscLIB/include/defs.h"
#include "../include/inf.h"
#include "CbmStsTrackFinder.h"
#include <string>


class CbmHoughStsTrackFinder : public CbmStsTrackFinder {

 private:

  CbmHoughStsTrackFinder(const CbmHoughStsTrackFinder&);
  CbmHoughStsTrackFinder& operator=(const CbmHoughStsTrackFinder&);

protected:

	unsigned int              eventNumber;						/**< Variable to store the number of the actual event. */
	inf*                      configurationFile;				/**< Object to read in a configuration file. */
	bool                      isFirstEvent;						/**< Variable stores if the actual event is the first event to compute or not. */
	inputData*	              input;							/**< Object to read in the input data.  */
	histogramSpace*           space;							/**< Object to store the dimension of the Hough-Histogram's space. */
	trackfinderInputData*     eventData;						/**< Object to store the data needed for the actual event. */
	histogramData*            histogram;						/**< Object to store the data of the histogram. */
	trackData*                tracks;							/**< Object to store the data of the computed tracks. */
	tables*                   ratings;							/**< Object to store the tables which are neccessary for the ratings. */
	lutImplementation*        luts;								/**< Object to make both neccessary look-up-tables accessible. */
	trackfinder*              houghTrackfinder;					/**< Object to compute the hough-transformation. */
	outputFTrack*             output;							/**< Object to save the tracks into the output file. */
	std::string               setAnalysisOutputFileName;		/**< Variable to store the name of the file to place the graphical analysis output. */

#ifndef NOANALYSIS

	analysis*                 analyser;							/**< Object to analyse the trackfinder internally. */

#endif

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(CbmHoughStsTrackFinder, 1);

/**
 * default constructor
 */

	CbmHoughStsTrackFinder();

/**
 * constructor
 * @param name is the name of the file to read out the configuration for the task
 * @param inputMinTracks is the cut for an event if the findable tracks are less
 * @param inputMaxTracks is the cut for an event if the findable tracks are more
 * @param inputDetectorMask is the information which detector should be not taken into the computation
 * @param inputMagneticFieldIntegrationStepwidthPerStation is the stepwidth of the points for the integration of the magnetic field
 * @param trackfinderGammaStep is the stepwidth of the first dimension of prelut to build a grid 
 * @param trackfinderThetaStep is the stepwidth of the first dimension of lut to build a grid 
 * @param trackfinderLutRadiusStep is the stepwidth of the second dimension of lut to build a grid 
 * @param trackfinderFirstFilterNeighborhoodDim1ClearRadius is the region of interest for the first filter in the first dimension 
 * @param trackfinderFirstFilterNeighborhoodDim2ClearRadius is the region of interest for the first filter in the second dimension 
 * @param trackfinderSecondFilterNeighborhoodDim1ClearRadius is the region of interest for the second filter in the first dimension 
 * @param trackfinderSecondFilterNeighborhoodDim2ClearRadius is the region of interest for the second filter in the second dimension 
 * @param trackfinderSecondFilterNeighborhoodDim3ClearRadius is the region of interest for the second filter in the third dimension 
 * @param initStatus is the enabling of the printing of the process information to the standard output stream
 * @param analysisOutputFileName is the name of the file to place the graphical output 
 */

	CbmHoughStsTrackFinder(const char* name);
	CbmHoughStsTrackFinder(bitArray inputDetectorMask,
						int inputMinTracks = 0, int inputMaxTracks = 0,
						unsigned short inputMagneticFieldIntegrationStepwidthPerStation = 10,
						int trackfinderGammaStep = 191,	int trackfinderThetaStep = 383,
						int trackfinderLutRadiusStep = 127,
						unsigned short trackFinderFirstFilterNeighborhoodDim1ClearRadius = 4,
						unsigned short trackFinderFirstFilterNeighborhoodDim2ClearRadius = 1,
						unsigned short trackFinderSecondFilterNeighborhoodDim1ClearRadius = 1,
						unsigned short trackFinderSecondFilterNeighborhoodDim2ClearRadius = 1,
						unsigned short trackFinderSecondFilterNeighborhoodDim3ClearRadius = 1,
						bool initStatus = true, const char* analysisOutputFileName = "");
	CbmHoughStsTrackFinder(const char* name,
						bitArray inputDetectorMask,
						int inputMinTracks = 0, int inputMaxTracks = 0,
						unsigned short inputMagneticFieldIntegrationStepwidthPerStation = 10,
						int trackfinderGammaStep = 191,	int trackfinderThetaStep = 383,
						int trackfinderLutRadiusStep = 127,
						unsigned short trackFinderFirstFilterNeighborhoodDim1ClearRadius = 4,
						unsigned short trackFinderFirstFilterNeighborhoodDim2ClearRadius = 1,
						unsigned short trackFinderSecondFilterNeighborhoodDim1ClearRadius = 1,
						unsigned short trackFinderSecondFilterNeighborhoodDim2ClearRadius = 1,
						unsigned short trackFinderSecondFilterNeighborhoodDim3ClearRadius = 1,
						bool initStatus = true, const char* analysisOutputFileName = "");

/**
 * destructor
 */

	~CbmHoughStsTrackFinder();

/**
 * This method sets the output file.
 */

	void SetOutputFile(const char* name);

/**
 * This method shows the specified histogram layer if possible.
 */

	void ShowHistogramLayer(unsigned int eventNumber, unsigned short layer, const char* drawOption = "");

/**
 * This method initializes the object.
 */

	void Init();

/**
 * This method does the trackfinding.
 * @return Number of tracks created
 */

	int DoFind();

/**
 * This method is executed after each Exec function call to
 * final results.
 */

	void Finish();

/**
 * SetVerbose(Int_t verbose)
 * verbose | Output
 *    0    | No output except warnings and errors
 *    1    | Additional output for event analysis with numbers in percent
 *    2    | Additional output for event analysis with absolute numbers
 *    3    | Additional graphical output
 *   else  | All output
 */

};

#endif
