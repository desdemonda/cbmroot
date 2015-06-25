/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
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
// $Date: 2008-11-21 13:58:07 $
// $Revision: 1.40 $
//
// *******************************************************************/

#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../DataRootObjectLIB/include/rungeKuttaInterface.h"
#include "../../DataObjectLIB/include/analyticFormula.h"
#include "../../LutGeneratorLIB/include/prelutMath.h"
#include "../../LutGeneratorLIB/include/digitalHitAccess.h"
#include "../../LutGeneratorLIB/include/prelutAccess.h"
#include "../../LutGeneratorLIB/include/lutAccess.h"
#include "../include/analysisError.h"
#include "../include/analysisWarningMsg.h"
#include "../include/analysisDef.h"
#include "../include/analysis.h"
#include <stdio.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif



#define NUMBEROFDIFFERENTMOMENTAS    100			/**< This definition allows to look in Hough transform goodness to the percentage of wrong momentas. */
#define NUMBEROFDIFFERENTCORRECTHITS 10				/**< This definition allows to look in prelut goodness to the percentage of hits, which meet the correct border in the histogram. */
#define STARTSLOPE                   0.003			/**< This definition sets for the prelut goodness the allowed slope range for the first station */
#define INCRSLOPE                    0.001			/**< This definition sets for the prelut goodness the incremental slope range for the stations after the first one */


#define           numberOfVolumes 3
const std::string volumes[numberOfVolumes] = {"cave", "pipevac", "sts"};


#undef TRANSFORMPREVIEW2D							/**< This definition switches the transform preview function to use a 3D or a special 2D widened mode. */


#define sqr(a)  ((a) * (a)) 


typedef struct {

	unsigned short frequency;
	double         value;

} stationMinimum;


typedef struct {

	unsigned short border;
	unsigned short frequency;
	double         distance;

} hitsInHistogramBorderFrequency;


/****************************************************************
 * method initializes the showAnalysis-object.					*
 ****************************************************************/

void analysis::initializeShowAnalysis() {

	if (showAnalyser == NULL)
		showAnalyser = new showAnalysis();

	if (showAnalyser == NULL)
		throw memoryAllocationError(ANALYSISLIB);

}


/****************************************************************
 * method initializes the totalAnalysis-object.					*
 ****************************************************************/

void analysis::initializeTotalAnalysis() {

	if (totalAnalyser == NULL)
		totalAnalyser = new totalAnalysis();

	if (totalAnalyser == NULL)
		throw memoryAllocationError(ANALYSISLIB);


}

/****************************************************************
 * method initializes the algorithm-evaluation.					*
 ****************************************************************/

void analysis::initializeAlgorithmEvaluation() {

	if (qualityAnalyser != NULL)
		qualityAnalyser->initializeEvaluation();

	if (momentumAnalyser != NULL)
		momentumAnalyser->initializeEvaluation();

	if (projectionAnalyser != NULL)
		projectionAnalyser->initializeEvaluation();

}

/****************************************************************
 * method initializes the magnetfield-evaluation.				*
 ****************************************************************/

void analysis::initializeMagnetfieldEvaluation() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->initializeEvaluation();


}

/****************************************************************
 * method finalizes the algorithm-evaluation.					*
 ****************************************************************/

void analysis::finalizeAlgorithmEvaluation() {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->finalizeEvaluation((*eventData)->getNumberOfHits(), getNumberOfTracksWithP());

	if (momentumAnalyser != NULL)
		momentumAnalyser->finalizeEvaluation();

	if (projectionAnalyser != NULL)
		projectionAnalyser->finalizeEvaluation();

}

/****************************************************************
 * method finalizes the magnetfieldAnalyser-evaluation.			*
 ****************************************************************/

void analysis::finalizeMagnetfieldEvaluation() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->finalizeEvaluation();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToFakeTrack(trackCoordinates& coordinates, trackParameter& parameter) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToFakeTrack();

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToFakeTrack(parameter, *(*space));

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToFakeTrack(coordinates);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToNoTrack(trackCoordinates& coordinates, trackParameter& parameter) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToNoTrack();

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToNoTrack(parameter, *(*space));

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToNoTrack(coordinates);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToOneTrack(trackCoordinates& coordinates, trackParameter& parameter, bool isClone, bool isWrong) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToOneTrack(isClone, isWrong);

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToOneTrack(parameter, *(*space), isClone, isWrong);

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToOneTrack(coordinates, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::peakToMoreTracks(trackCoordinates& coordinates, trackParameter& parameter, bool isClone, bool isWrong) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (qualityAnalyser != NULL)
		qualityAnalyser->peakToMoreTracks(isClone, isWrong);

	if (momentumAnalyser != NULL)
		momentumAnalyser->peakToMoreTracks(parameter, *(*space), isClone, isWrong);

	if (projectionAnalyser != NULL)
		projectionAnalyser->peakToMoreTracks(coordinates, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::trackToNoPeak(trackfinderInputTrack& actualTrack) {

	trackCoordinates actualCoordinates;

	if (qualityAnalyser != NULL)
		qualityAnalyser->trackToNoPeak();

	if (momentumAnalyser != NULL)
		momentumAnalyser->trackToNoPeak(actualTrack);

	if (projectionAnalyser != NULL) {

		if (estimateCoordinates(actualTrack, &actualCoordinates))
			projectionAnalyser->trackToNoPeak(actualCoordinates);
	
	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::trackToOnePeak(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->trackToOnePeak();

	if (momentumAnalyser != NULL)
		momentumAnalyser->trackToOnePeak(actualTrack);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::trackToMorePeaks(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->trackToMorePeaks();

	if (momentumAnalyser != NULL)
		momentumAnalyser->trackToMorePeaks(actualTrack);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::wellFoundTracks(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->wellFoundTracks();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void analysis::wrongFoundTracks(trackfinderInputTrack& actualTrack) {

	if (qualityAnalyser != NULL)
		qualityAnalyser->wrongFoundTracks();

}

/****************************************************************
 * This method evaluates if the given pattern is a track		*
 * pattern and returns the peakclass.							*
 ****************************************************************/

bitArray analysis::evaluateTrackPatternGradingP(bitArray& pattern) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return (*ratings)->getGradingPTableClassification(pattern);

}

/****************************************************************
 * This method evaluates if the given pattern is a track		*
 * pattern and returns the peakclass.							*
 ****************************************************************/

bitArray analysis::evaluateTrackPatternGradingR(bitArray& pattern) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return (*ratings)->getGradingRTableClassification(pattern);

}

/****************************************************************
 * This method evaluates if the given pattern is a track		*
 * pattern and returns the peakclass.							*
 ****************************************************************/

bitArray analysis::evaluateTrackPatternCoding(bitArray& pattern) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return (*ratings)->getCodingTableClassification(pattern);

}

/****************************************************************
 * This method evaluates the number of hits for a given real	*
 * track id.													*
 ****************************************************************/

unsigned short analysis::evaluateNumberOfRealTrackHits(trackfinderInputTrack* actualTrack) {

	trackfinderInputHit* hit;
	bitArray             trackPattern;
	unsigned short       returnValue;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (actualTrack == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	trackPattern.reset();
	actualTrack->resetHitPointer();
	for (unsigned short i = 0; i < actualTrack->getNumberOfHits(); i++) {

		hit = actualTrack->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);

		trackPattern[hit->getStation()->getIndex()] = true;

	}

	returnValue = evaluateNumberOfTrackHits(trackPattern);

	return returnValue;

}

/****************************************************************
 * This method evaluates the number of hits for a given pattern	*
 * which represents a track.									*
 ****************************************************************/

unsigned short analysis::evaluateNumberOfTrackHits(bitArray pattern) {

	return (unsigned short)pattern.count();

}

/****************************************************************
 * This method evaluates if the track is a  real track which	*
 * should be found.												*
 ****************************************************************/

bool analysis::evaluateFoundTrackIsRealTrack(trackfinderInputTrack* actualTrack) {

	bitArray             trackPattern;
	unsigned short       i;
	trackfinderInputHit* hit;
	bool                 returnValue = true;

#ifndef FOUNDTRACKISREALTRACKCOMPTABILITYMODE

	bitArray comparator;

#endif

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (actualTrack == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	trackPattern.reset();
	actualTrack->resetHitPointer();
	for (i = 0; i < actualTrack->getNumberOfHits(); i++) {

		hit = actualTrack->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);

		trackPattern[hit->getStation()->getIndex()] = true;

	}

#ifdef FOUNDTRACKISREALTRACKCOMPTABILITYMODE

	for (i = 1; i < actualTrack->getNumberOfHits() - 2; i++)
		if (!trackPattern[i])
			returnValue = false;

#else

	comparator = bitArray(0);

	if (evaluateTrackPatternGradingP(trackPattern) == comparator)
		returnValue = false;

#endif

	return returnValue;

}

/****************************************************************
 * This method evaluates the track distribution of a peak.		*
 ****************************************************************/

void analysis::evaluatePeakDistribution(trackDigitalInformation& track, specialListMem<trackHitMem>* possibleTracks) {

#ifndef NOANALYSIS

	trackHitMem          actualTrack;
	trackfinderInputHit* hit;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	for (unsigned int i = 0; i < track.hits.getNumberOfMemories(); i++) {

		track.hits[i].resetActiveObject();
		for(unsigned short j = 0; j < track.hits[i].getNumberOfEntries(); j++) {

			actualTrack.removeAllHits();

			hit = track.hits[i].readActiveObjectAndMakeNextOneActive();

			actualTrack.addHit(hit);
		
			if (possibleTracks->isFound(actualTrack, true))		/* track is old track */
				possibleTracks->getActiveObject()->addHit(hit);
			else												/* track not found, so add it; fakes have no track => they are always not found */
				possibleTracks->push(actualTrack);

		}

	}

	actualTrack.removeAllHits();

#endif

}

/****************************************************************
 * This method evaluates the category of a found track.			*
 ****************************************************************/

void analysis::evaluateFoundTracks(trackfinderInputTrack* track, unsigned short maximumHits, bool* isClone, bool* isWrong) {

	bool allowWrong = true;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (track == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	/* peak is a clone or not */
	if ((track->getMomZ() > minimumP) && evaluateFoundTrackIsRealTrack(track)) {

		allowWrong = false;
		*isWrong   = false;
				
		if ((track->getFrequency() > 0) && (*isClone))
			*isClone = true;	/* mark peak as clone (priority 2) */
		else
			*isClone = false;	/* mark peak as real track (priority 1) */
			
	}
	else
		if (allowWrong)
			*isWrong = true;	/* mark peak as wrong track (priority 3) */
			
	/* save track */
	track->incrementFrequency();
	track->modifyMaximumNumberOfHits(maximumHits);

}

/****************************************************************
 * This method evaluates how many tracks contribute to a peak.	*
 ****************************************************************/

unsigned short analysis::evaluatePeakContribution(trackDigitalInformation& track, bool* isClone, bool* isWrong, bool* isFake) {

	specialListMem<trackHitMem> possibleTracks;
	trackHitMem                 actualTrack;
	bitArray                    trackPattern;
	trackfinderInputTrack*      realTrack;
	bitArray                    actualTrackPattern;
	bitArray                    actualClass;
	bitArray                    minimumClass;
	unsigned short              maximumHits;
	unsigned short              returnValue = 0;

#ifdef PEAKCONTRIBUTIONCOMPTABILITYMODE

	trackfinderInputTrack*      maximumTrack;
	bitArray                    maximumClass;
	bool                        maximumFound = false;

#endif

	*isClone   = true;
	*isWrong   = false;
	realTrack  = NULL;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	possibleTracks.clear();
	evaluatePeakDistribution(track, &possibleTracks);

	trackPattern.reset();
	possibleTracks.resetActiveObject();
	minimumClass = bitArray(0);
	for (unsigned long i = 0; i < possibleTracks.getNumberOfEntries(); i++) {

		actualTrack        = possibleTracks.readActiveObjectAndMakeNextOneActive();

		actualTrackPattern = actualTrack.getTrackPattern();

		actualClass        = evaluateTrackPatternGradingR(actualTrackPattern);

		realTrack          = actualTrack.getRealTrack();

		if (realTrack != NULL)
			trackPattern |= actualTrackPattern;

		if (actualClass > minimumClass) {

#ifdef PEAKCONTRIBUTIONCOMPTABILITYMODE

			if (maximumFound) {

				if (actualClass > maximumClass) {

					maximumClass = actualClass;
					maximumTrack = realTrack;
					maximumHits  = evaluateNumberOfTrackHits(actualTrackPattern);

				}

			}
			else {

				maximumClass = actualClass;
				maximumTrack = realTrack;
				maximumHits  = evaluateNumberOfTrackHits(actualTrackPattern);
				returnValue  = 1;
				maximumFound = true;

			}

#else

			returnValue++;
	
			maximumHits = evaluateNumberOfTrackHits(actualTrackPattern);
			if (realTrack != NULL)
				evaluateFoundTracks(realTrack, maximumHits, isClone, isWrong);
			else
				throw fakePassingGradingRError();

#endif

		}

	}

	if (evaluateTrackPatternCoding(trackPattern) > minimumClass)
		*isFake = false;
	else
		*isFake = true;

#ifdef PEAKCONTRIBUTIONCOMPTABILITYMODE

	if (returnValue == 1) {

		if (maximumTrack != NULL)
			evaluateFoundTracks(maximumTrack, maximumHits, isClone, isWrong);
		else
			throw fakePassingGradingRError();

	}

#endif

	return returnValue;

}

/****************************************************************
 * method previews the Hough space transformation for a track.	*
 ****************************************************************/

histogramCellSignature analysis::transformPreview(trackfinderInputTrack& track, trackCoordinates& estimation) {

#ifdef TRANSFORMPREVIEW2D

	prelutHoughBorder*              firstBorder;
	lutHoughBorder*                 secondBorder;
	hitsInHistogramBorderFrequency* histogramBorderFrequency;
	unsigned short                  numberOfHistogramBorderEntries;
	histogramCellSignatures         cellSignatures;
	histogramCellSignature          bestCellSignature;
	unsigned short                  i;
	trackfinderInputHit*            hit;
	unsigned short                  stationIndex;
	prelutHoughBorder               evaluateFirstBorder;
	lutBorder                       border;
	unsigned short                  borderIndex;
	hitsInHistogramBorderFrequency  maximumHistogramBorderFrequency;
	houghBorderPosition             actualPosition;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError();

	firstBorder                    = border.getPrelutHoughBorderPointer();
	secondBorder                   = border.getLutHoughBorderPointer();
	histogramBorderFrequency       = NULL;
	numberOfHistogramBorderEntries = 0;
	cellSignatures.reset();
	track.resetHitPointer();

	for (i = 0; i < track.getNumberOfHits(); i++) {

		hit = track.getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		(*luts)->evaluatePrelut(hit, &evaluateFirstBorder);

		for (unsigned short j = evaluateFirstBorder.start; j <= evaluateFirstBorder.stop; j++) {

			if (numberOfHistogramBorderEntries == 0) {

				numberOfHistogramBorderEntries        = 1;
				histogramBorderFrequency              = (hitsInHistogramBorderFrequency*)calloc(numberOfHistogramBorderEntries, sizeof(hitsInHistogramBorderFrequency));
				histogramBorderFrequency[0].border    = j;
				histogramBorderFrequency[0].frequency = 1;

			}
			else {
					
				borderIndex = numberOfHistogramBorderEntries;
				for (unsigned short k = 0; k < numberOfHistogramBorderEntries; k++) {

					if (histogramBorderFrequency[k].border == j) {

						borderIndex = k;
						break;

					}

				}
					
				if (borderIndex == numberOfHistogramBorderEntries) {

					numberOfHistogramBorderEntries++;
					histogramBorderFrequency                        = (hitsInHistogramBorderFrequency*)realloc(histogramBorderFrequency, numberOfHistogramBorderEntries * sizeof(hitsInHistogramBorderFrequency));
					histogramBorderFrequency[borderIndex].border    = j;
					histogramBorderFrequency[borderIndex].frequency = 1;

				}
				else
					histogramBorderFrequency[borderIndex].frequency++;

			}

		}

	}

	maximumHistogramBorderFrequency.frequency = 0;
	maximumHistogramBorderFrequency.distance  = 0;
	for (i = 0; i < numberOfHistogramBorderEntries; i++) {

		histogramBorderFrequency[i].distance = sqrt((double)sqr(histogramBorderFrequency[i].border - estimation.get(DIM3)));

		if (histogramBorderFrequency[i].frequency > maximumHistogramBorderFrequency.frequency)
			maximumHistogramBorderFrequency = histogramBorderFrequency[i];
		else if ((histogramBorderFrequency[i].frequency == maximumHistogramBorderFrequency.frequency) && (histogramBorderFrequency[i].distance < maximumHistogramBorderFrequency.distance))
			maximumHistogramBorderFrequency = histogramBorderFrequency[i];

	}

	if (histogramBorderFrequency != NULL) {
		free(histogramBorderFrequency);
		histogramBorderFrequency       = NULL;
		numberOfHistogramBorderEntries = 0;
	}

	for (i = 0; i < track.getNumberOfHits(); i++) {

		hit = track.getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);
		else
			stationIndex = hit->getStation()->getIndex();

		(*luts)->evaluate(hit, &border);

		if ((firstBorder->start <= maximumHistogramBorderFrequency.border) && (firstBorder->stop >= maximumHistogramBorderFrequency.border)) {

			secondBorder->houghCoord.resetActiveObject();
			for (unsigned short l = 0; l < secondBorder->houghCoord.getNumberOfEntries(); l++) {

				actualPosition = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();

				cellSignatures.add(actualPosition, stationIndex);

			}

		}

	}

	bestCellSignature = cellSignatures.getBest(estimation);

	cellSignatures.reset();

	return bestCellSignature;

#else

	lutBorder               border;
	prelutHoughBorder*      firstBorder;
	lutHoughBorder*         secondBorder;
	histogramCellSignatures cellSignatures;
	histogramCellSignature  bestCellSignature;
	trackfinderInputHit*    hit;
	unsigned short          stationIndex;
	houghBorderPosition     actualPosition;

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError();

	firstBorder  = border.getPrelutHoughBorderPointer();
	secondBorder = border.getLutHoughBorderPointer();
	cellSignatures.reset();
	track.resetHitPointer();

	for (unsigned short i = 0; i < track.getNumberOfHits(); i++) {

		hit = track.getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);
		else
			stationIndex = hit->getStation()->getIndex();

		(*luts)->evaluate(hit, &border);

		for (unsigned short j = firstBorder->start; j <= firstBorder->stop; j++) {

			secondBorder->houghCoord.resetActiveObject();
			for (unsigned short k = 0; k < secondBorder->houghCoord.getNumberOfEntries(); k++) {

				actualPosition = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();

				cellSignatures.add(actualPosition, j, stationIndex);

			}

		}

	}

	bestCellSignature = cellSignatures.getBest(estimation);

	cellSignatures.reset();

	return bestCellSignature;

#endif

}

/****************************************************************
 * method estimates the coordinates in the Hough space for a	*
 * track.														*
 ****************************************************************/

bool analysis::estimateCoordinates(trackfinderInputTrack& track, trackCoordinates* coordinates) {
	
	bool                   returnValue;
	trackMomentum          actualMomentum;
	analyticFormula        formula;
	trackParameter         actualParameter;
	trackCoordinates       actualPosition;
	trackCoordinates       estimatedPosition;
	trackCoordinates       bestPosition;
	double                 estimatedMomentumDifferenceQuad;
	double                 bestMomentumDifferenceQuad;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (coordinates == NULL)
		throw cannotAccessTrackCoordinatesError();

	returnValue = false;

	/* setup the correct momentum */
	actualMomentum.set(track.getMomX(), PX);
	actualMomentum.set(track.getMomY(), PY);
	actualMomentum.set(track.getMomZ(), PZ);

	/* compute the coordinates for the correct momentum */
	formula.evaluateHough(actualMomentum, track.getCharge(), *(*space), &actualPosition);

	/* CAUTION:
	 * The actualPosition is always rounded to the next value in each dimension above. So a
	 * correction of the position must be applied because one cannot round each dimension independant of the others.
	 */

	/* compute the estimated momentum for the computed coordinates */
	formula.evaluateP(actualPosition, *(*space), &actualMomentum, NULL);

	/* compute the quadrat of the phytagoras difference for the correct and the estimated momentum */
	bestMomentumDifferenceQuad = sqr(track.getMomX() - actualMomentum.get(DIM1)) + sqr(track.getMomY() - actualMomentum.get(DIM2)) + sqr(track.getMomZ() - actualMomentum.get(DIM3));

	/* set the estimated position with the actual minimal difference */
	bestPosition = actualPosition;

	/* search the neighbourhood for a smaller difference */
	for (short neighbourhoodDim1 = -1; neighbourhoodDim1 <= 1; neighbourhoodDim1++) {
		for (short neighbourhoodDim2 = -1; neighbourhoodDim2 <= 1; neighbourhoodDim2++) {
			for (short neighbourhoodDim3 = -1; neighbourhoodDim3 <= 1; neighbourhoodDim3++) {

				estimatedPosition.set(actualPosition.get(DIM1) + neighbourhoodDim1, DIM1);
				estimatedPosition.set(actualPosition.get(DIM2) + neighbourhoodDim2, DIM2);
				estimatedPosition.set(actualPosition.get(DIM3) + neighbourhoodDim3, DIM3);

				if ((estimatedPosition.get(DIM1) < (*space)->getStep(DIM1)) && (estimatedPosition.get(DIM2) < (*space)->getStep(DIM2)) && (estimatedPosition.get(DIM3) < (*space)->getStep(DIM3))) {

					/* compute the estimated neighbourhood momentum for the estimated coordinates */
					formula.evaluateP(estimatedPosition, *(*space), &actualMomentum, NULL);

					/* compute the quadrat of the phytagoras difference for the correct and the estimated neighbourhood momentum */
					estimatedMomentumDifferenceQuad = sqr(track.getMomX() - actualMomentum.get(DIM1)) + sqr(track.getMomY() - actualMomentum.get(DIM2)) + sqr(track.getMomZ() - actualMomentum.get(DIM3));

					if (estimatedMomentumDifferenceQuad < bestMomentumDifferenceQuad) {

						/* set the quadrat of the phytagoras difference for the correct and the new estimated momentum */
						bestMomentumDifferenceQuad = estimatedMomentumDifferenceQuad;

						/* set the estimated position with the actual minimal difference */
						bestPosition               = estimatedPosition;

					}

				}

			}
		}
	}

	if ((bestPosition.get(DIM1) < (*space)->getStep(DIM1)) && (bestPosition.get(DIM2) < (*space)->getStep(DIM2)) && (bestPosition.get(DIM3) < (*space)->getStep(DIM3))) {

		*coordinates = bestPosition;
		returnValue  = true;

	}
	else {

		positionIsOutOfSpaceWarningMsg* positionIsOutOfSpace = new positionIsOutOfSpaceWarningMsg(*space, &estimatedPosition);
		positionIsOutOfSpace->warningMsg();
		if(positionIsOutOfSpace != NULL) {
			delete positionIsOutOfSpace;
			positionIsOutOfSpace = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * method computes the coordinates in the Hough space for a		*
 * track.														*
 ****************************************************************/

bool analysis::computeCoordinates(trackfinderInputTrack& track, trackCoordinates* coordinates) {

	bool                   returnValue;
	trackCoordinates       estimatedCoordinates;
	histogramCellSignature bestCellSignature;

	if (coordinates == NULL)
		throw cannotAccessTrackCoordinatesError();

	returnValue       = false;
	estimateCoordinates(track, &estimatedCoordinates);

	bestCellSignature = transformPreview(track, estimatedCoordinates);

	if ((bestCellSignature.pos.get(DIM1) < (*space)->getStep(DIM1)) && (bestCellSignature.pos.get(DIM2) < (*space)->getStep(DIM2)) && (bestCellSignature.pos.get(DIM3) < (*space)->getStep(DIM3))) {

		*coordinates = bestCellSignature.pos;
		returnValue  = true;

	}
	else {

		positionIsOutOfSpaceWarningMsg* positionIsOutOfSpace = new positionIsOutOfSpaceWarningMsg(*space, &estimatedCoordinates);
		positionIsOutOfSpace->warningMsg();
		if(positionIsOutOfSpace != NULL) {
			delete positionIsOutOfSpace;
			positionIsOutOfSpace = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analysis::analysis() 
  : eventData(NULL),
    tracks(NULL),
    ratings(NULL),
    space(NULL),
    luts(NULL),
    trackPropagationAnalyser(NULL),
    qualityAnalyser(NULL),
    momentumAnalyser(NULL),
    projectionAnalyser(NULL),
    magnetfieldAnalyser(NULL),
    magnetfieldFactorAnalyser(NULL),
    prelutRangeLayerAnalyser(NULL),
    histogramAnalyser(NULL),
    showAnalyser(NULL),
    totalAnalyser(NULL),
    hardwareAnalyser(NULL),
    mcTrackVisualAnalyser(NULL),
    foundTrackVisualAnalyser(NULL),
    peakfindingGeometryAnalyser(NULL),
    minimumP(0),
    configuration(false),
    detector(false),
    event(false),
    classPriority(false),
    memory(false),
    time(false),
    percentageOfHitsInSignature(0),
    percentageOfTracksForSignature(0),
    analysisResultWarnings(0),
    analysisResultDisplays(0),
    analysisMoreResultWarnings(0),
    analysisMoreResultDisplays(0),
    pictures(NULL),
    borderCreationTimer(NULL),
    histogramCreationTimer(NULL),
    histogramEncodingTimer(NULL),
    histogramDiagonalizingTimer(NULL),
    histogramPeakfindingTimer(NULL),
    histogramFinalizingTimer(NULL),
    histogramResettingTimer(NULL),
    trackPeakfindingTimer(NULL),
    reservedSizeOfLBufferData(0),
    allocatedSizeOfLBufferData(0),
    usedSizeOfLBufferData(0)
{
  /*
        eventData                      = NULL;
	tracks                         = NULL;
	ratings                        = NULL;
	space                          = NULL;
	luts                           = NULL;
	trackPropagationAnalyser       = NULL;
	qualityAnalyser                = NULL;
	momentumAnalyser               = NULL;
	projectionAnalyser             = NULL;
	magnetfieldAnalyser            = NULL;
	magnetfieldFactorAnalyser      = NULL;
	prelutRangeLayerAnalyser       = NULL;
	histogramAnalyser              = NULL;
	showAnalyser                   = NULL;
	totalAnalyser                  = NULL;
	hardwareAnalyser               = NULL;
	mcTrackVisualAnalyser          = NULL;
	foundTrackVisualAnalyser       = NULL;
	peakfindingGeometryAnalyser         = NULL;

	minimumP                       = 0;

	configuration                  = false;
	detector                       = false;
	memory                         = false;
	event                          = false;
	classPriority                  = false;

	percentageOfHitsInSignature    = 0;
	percentageOfTracksForSignature = 0;
	analysisResultWarnings         = 0;
	analysisResultDisplays         = 0;
	analysisMoreResultWarnings     = 0;
	analysisMoreResultDisplays     = 0;

	pictures                       = NULL;
	
	borderCreationTimer            = NULL;
	histogramCreationTimer         = NULL;
	histogramEncodingTimer         = NULL;
	histogramDiagonalizingTimer    = NULL;
	histogramPeakfindingTimer      = NULL;
	histogramFinalizingTimer       = NULL;
	histogramResettingTimer        = NULL;
	trackPeakfindingTimer          = NULL;

	reservedSizeOfLBufferData      = 0;
	allocatedSizeOfLBufferData     = 0;
	usedSizeOfLBufferData          = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

analysis::analysis(initialParameter parameters,
				   bool initHistogramAnalysisToRoot) 
  : eventData(parameters.eventData),
    tracks(parameters.tracks),
    ratings(parameters.ratings),
    space(parameters.space),
    luts(parameters.luts),
    trackPropagationAnalyser(NULL),
    qualityAnalyser(NULL),
    momentumAnalyser(NULL),
    projectionAnalyser(NULL),
    magnetfieldAnalyser(NULL),
    magnetfieldFactorAnalyser(NULL),
    prelutRangeLayerAnalyser(NULL),
    histogramAnalyser(NULL),
    showAnalyser(NULL),
    totalAnalyser(NULL),
    hardwareAnalyser(NULL),
    mcTrackVisualAnalyser(NULL),
    foundTrackVisualAnalyser(NULL),
    peakfindingGeometryAnalyser(NULL),
    minimumP(parameters.minP),
    configuration(parameters.initConfiguration),
    detector(parameters.initDetector),
    event(parameters.initEvent),
    classPriority(parameters.initClassPriority),
    memory(parameters.initMemory),
    time(parameters.initTime),
    percentageOfHitsInSignature(parameters.percentageOfHitsInSignature),
    percentageOfTracksForSignature(parameters.percentageOfTracksForSignature),
    analysisResultWarnings(parameters.analysisResultWarnings),
    analysisResultDisplays(parameters.analysisResultDisplays),
    analysisMoreResultWarnings(parameters.analysisMoreResultWarnings),
    analysisMoreResultDisplays(parameters.analysisMoreResultDisplays),
    pictures(NULL),
    borderCreationTimer(NULL),
    histogramCreationTimer(NULL),
    histogramEncodingTimer(NULL),
    histogramDiagonalizingTimer(NULL),
    histogramPeakfindingTimer(NULL),
    histogramFinalizingTimer(NULL),
    histogramResettingTimer(NULL),
    trackPeakfindingTimer(NULL),
    reservedSizeOfLBufferData(0),
    allocatedSizeOfLBufferData(0),
    usedSizeOfLBufferData(0)
{
  /*
	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;
	trackPropagationAnalyser       = NULL;
	qualityAnalyser                = NULL;
	momentumAnalyser               = NULL;
	projectionAnalyser             = NULL;
	magnetfieldAnalyser            = NULL;
	magnetfieldFactorAnalyser      = NULL;
	prelutRangeLayerAnalyser       = NULL;
	histogramAnalyser              = NULL;
	showAnalyser                   = NULL;
	totalAnalyser                  = NULL;
	hardwareAnalyser               = NULL;
	mcTrackVisualAnalyser          = NULL;
	foundTrackVisualAnalyser       = NULL;
	peakfindingGeometryAnalyser    = NULL;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;
	memory                         = parameters.initMemory;
	time                           = parameters.initTime;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;
	analysisMoreResultWarnings     = parameters.analysisMoreResultWarnings;
	analysisMoreResultDisplays     = parameters.analysisMoreResultDisplays;

	pictures                       = NULL;

	borderCreationTimer            = NULL;
	histogramCreationTimer         = NULL;
	histogramEncodingTimer         = NULL;
	histogramDiagonalizingTimer    = NULL;
	histogramPeakfindingTimer      = NULL;
	histogramFinalizingTimer       = NULL;
	histogramResettingTimer        = NULL;
	trackPeakfindingTimer          = NULL;

	reservedSizeOfLBufferData      = 0;
	allocatedSizeOfLBufferData     = 0;
	usedSizeOfLBufferData          = 0;
*/
	initAutomaticFilterGeometryAnalysis(parameters.histogram, parameters.initAutomatcFilterGeometry);

	initTrackPropagationEventPointAnalysis(parameters.initPropagationEventPoint);
	initTrackPropagationEventHitAnalysis(parameters.initPropagationEventHit);
	initTrackPropagationTotalPointAnalysis(parameters.initPropagationTotalPoint);
	initTrackPropagationTotalHitAnalysis(parameters.initPropagationTotalHit);
	initTrackPropagationAnalysisDisplay(parameters.initPropagationDisplay);

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);

	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);

	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);

	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);

	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);

	initHistogramAnalysis(parameters.histogram, initHistogramAnalysisToRoot);

	initPrelutRangeAnalysis(parameters.initPrelutRange);
	initPrelutRangeAnalysisDisplay(parameters.initPrelutRangeDisplay, parameters.initPrelutRangeDisplayMode);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerRowAnalysis(parameters.initTracksPerRow);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);
	initHardwareHitReadoutDistributionAnalysis(parameters.initHitReadoutDistribution);

}
analysis::analysis(initialParameter parameters,
		   initialFileParameter fileParameters,
		   bool initCreatedHistogramToShow, 
                   bool initEncodedHistogramToShow,
		   bool initFilteredHistogramToShow, 
		   unsigned short initHistogramLayer) 
  : eventData(parameters.eventData),
    tracks(parameters.tracks),
    ratings(parameters.ratings),
    space(parameters.space),
    luts(parameters.luts),
    trackPropagationAnalyser(NULL),
    qualityAnalyser(NULL),
    momentumAnalyser(NULL),
    projectionAnalyser(NULL),
    magnetfieldAnalyser(NULL),
    magnetfieldFactorAnalyser(NULL),
    prelutRangeLayerAnalyser(NULL),
    histogramAnalyser(NULL),
    showAnalyser(NULL),
    totalAnalyser(NULL),
    hardwareAnalyser(NULL),
    mcTrackVisualAnalyser(NULL),
    foundTrackVisualAnalyser(NULL),
    peakfindingGeometryAnalyser(NULL),
    minimumP(parameters.minP),
    configuration(parameters.initConfiguration),
    detector(parameters.initDetector),
    event(parameters.initEvent),
    classPriority(parameters.initClassPriority),
    memory(parameters.initMemory),
    time(parameters.initTime),
    percentageOfHitsInSignature(parameters.percentageOfHitsInSignature),
    percentageOfTracksForSignature(parameters.percentageOfTracksForSignature),
    analysisResultWarnings(parameters.analysisResultWarnings),
    analysisResultDisplays(parameters.analysisResultDisplays),
    analysisMoreResultWarnings(parameters.analysisMoreResultWarnings),
    analysisMoreResultDisplays(parameters.analysisMoreResultDisplays),
    pictures(NULL),
    borderCreationTimer(NULL),
    histogramCreationTimer(NULL),
    histogramEncodingTimer(NULL),
    histogramDiagonalizingTimer(NULL),
    histogramPeakfindingTimer(NULL),
    histogramFinalizingTimer(NULL),
    histogramResettingTimer(NULL),
    trackPeakfindingTimer(NULL),
    reservedSizeOfLBufferData(0),
    allocatedSizeOfLBufferData(0),
    usedSizeOfLBufferData(0)
{
  /*
	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;
	trackPropagationAnalyser       = NULL;
	qualityAnalyser                = NULL;
	momentumAnalyser               = NULL;
	projectionAnalyser             = NULL;
	magnetfieldAnalyser            = NULL;
	magnetfieldFactorAnalyser      = NULL;
	prelutRangeLayerAnalyser       = NULL;
	histogramAnalyser              = NULL;
	showAnalyser                   = NULL;
	totalAnalyser                  = NULL;
	hardwareAnalyser               = NULL;
	mcTrackVisualAnalyser          = NULL;
	foundTrackVisualAnalyser       = NULL;
	peakfindingGeometryAnalyser    = NULL;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;
	memory                         = parameters.initMemory;
	time                           = parameters.initTime;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;
	analysisMoreResultWarnings     = parameters.analysisMoreResultWarnings;
	analysisMoreResultDisplays     = parameters.analysisMoreResultDisplays;

	pictures                       = NULL;

	borderCreationTimer            = NULL;
	histogramCreationTimer         = NULL;
	histogramEncodingTimer         = NULL;
	histogramDiagonalizingTimer    = NULL;
	histogramPeakfindingTimer      = NULL;
	histogramFinalizingTimer       = NULL;
	histogramResettingTimer        = NULL;
	trackPeakfindingTimer          = NULL;

	reservedSizeOfLBufferData      = 0;
	allocatedSizeOfLBufferData     = 0;
	usedSizeOfLBufferData          = 0;
  */
	initAutomaticFilterGeometryAnalysis(parameters.histogram, parameters.initAutomatcFilterGeometry);

	initTrackPropagationEventPointAnalysis(parameters.initPropagationEventPoint);
	initTrackPropagationEventHitAnalysis(parameters.initPropagationEventHit);
	initTrackPropagationTotalPointAnalysis(parameters.initPropagationTotalPoint);
	initTrackPropagationTotalHitAnalysis(parameters.initPropagationTotalHit);
	initTrackPropagationAnalysisDisplay(parameters.initPropagationDisplay);
	initTrackPropagationAnalysisToRoot(fileParameters.initPropagationToRoot, fileParameters.name);

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);

	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);

	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);
	initMomentumAnalysisToRoot(fileParameters.initMomentumToRoot, fileParameters.name);

	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);
	initProjectionAnalysisToRoot(fileParameters.initProjectionToRoot, fileParameters.name);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);

	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);
	initMagnetfieldAnalysisToRoot(fileParameters.initMagnetfieldToRoot, fileParameters.name);

	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot | fileParameters.initEncodedHistogramToRoot | fileParameters.initFilteredHistogramToRoot, parameters.histogram);
	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot, fileParameters.initEncodedHistogramToRoot, fileParameters.initFilteredHistogramToRoot, fileParameters.initJustOneCreatedHistogramToRoot, fileParameters.initJustOneEncodedHistogramToRoot, fileParameters.initJustOneFilteredHistogramToRoot, initCreatedHistogramToShow, initEncodedHistogramToShow, initFilteredHistogramToShow, initHistogramLayer, fileParameters.name);

	initPrelutRangeAnalysis(parameters.initPrelutRange);
	initPrelutRangeAnalysisDisplay(parameters.initPrelutRangeDisplay, parameters.initPrelutRangeDisplayMode);
	initPrelutRangeAnalysisToRoot(fileParameters.initPrelutRangeToRoot, fileParameters.name);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerRowAnalysis(parameters.initTracksPerRow);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);
	initHardwareHitReadoutDistributionAnalysis(parameters.initHitReadoutDistribution);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analysis::~analysis() {

	if (qualityAnalyser != NULL) {
		delete qualityAnalyser;
		qualityAnalyser = NULL;
	}

	if (momentumAnalyser != NULL) {
		delete momentumAnalyser;
		momentumAnalyser = NULL;
	}

	if (projectionAnalyser != NULL) {
		delete projectionAnalyser;
		projectionAnalyser = NULL;
	}

	if (magnetfieldAnalyser != NULL) {
		delete magnetfieldAnalyser;
		magnetfieldAnalyser = NULL;
	}

	if (magnetfieldFactorAnalyser != NULL) {
		delete magnetfieldFactorAnalyser;
		magnetfieldFactorAnalyser = NULL;
	}
	if (prelutRangeLayerAnalyser != NULL) {
		delete prelutRangeLayerAnalyser;
		prelutRangeLayerAnalyser = NULL;
	}
	if (showAnalyser != NULL) {
		delete showAnalyser;
		showAnalyser = NULL;
	}
	if (totalAnalyser != NULL) {
		delete totalAnalyser;
		totalAnalyser = NULL;
	}
	if (hardwareAnalyser != NULL) {
		delete hardwareAnalyser;
		hardwareAnalyser = NULL;
	}
	if (mcTrackVisualAnalyser != NULL) {
		delete mcTrackVisualAnalyser;
		mcTrackVisualAnalyser = NULL;
	}
	if (foundTrackVisualAnalyser != NULL) {
		delete foundTrackVisualAnalyser;
		foundTrackVisualAnalyser = NULL;
	}
	if (peakfindingGeometryAnalyser != NULL) {
		delete peakfindingGeometryAnalyser;
		peakfindingGeometryAnalyser = NULL;
	}
	if (pictures != NULL) {
		delete pictures;
		pictures = NULL;
	}
	if (borderCreationTimer != NULL) {
		delete borderCreationTimer;
		borderCreationTimer = NULL;
	}
	if (histogramCreationTimer != NULL) {
		delete histogramCreationTimer;
		histogramCreationTimer = NULL;
	}
	if (histogramEncodingTimer != NULL) {
		delete histogramEncodingTimer;
		histogramEncodingTimer = NULL;
	}
	if (histogramDiagonalizingTimer != NULL) {
		delete histogramDiagonalizingTimer;
		histogramDiagonalizingTimer = NULL;
	}
	if (histogramPeakfindingTimer != NULL) {
		delete histogramPeakfindingTimer;
		histogramPeakfindingTimer = NULL;
	}
	if (histogramFinalizingTimer != NULL) {
		delete histogramFinalizingTimer;
		histogramFinalizingTimer = NULL;
	}
	if (histogramResettingTimer != NULL) {
		delete histogramResettingTimer;
		histogramResettingTimer = NULL;
	}
	if (trackPeakfindingTimer != NULL) {
		delete trackPeakfindingTimer;
		trackPeakfindingTimer = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void analysis::init(initialParameter parameters,
					bool initHistogramAnalysisToRoot) {

	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;
	memory                         = parameters.initMemory;
	time                           = parameters.initTime;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;
	analysisMoreResultWarnings     = parameters.analysisMoreResultWarnings;
	analysisMoreResultDisplays     = parameters.analysisMoreResultDisplays;

	if (pictures != NULL) {
		delete pictures;
		pictures = NULL;
	}

	initAutomaticFilterGeometryAnalysis(parameters.histogram, parameters.initAutomatcFilterGeometry);

	initTrackPropagationEventPointAnalysis(parameters.initPropagationEventPoint);
	initTrackPropagationEventHitAnalysis(parameters.initPropagationEventHit);
	initTrackPropagationTotalPointAnalysis(parameters.initPropagationTotalPoint);
	initTrackPropagationTotalHitAnalysis(parameters.initPropagationTotalHit);
	initTrackPropagationAnalysisDisplay(parameters.initPropagationDisplay);

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);

	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);

	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);

	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);
	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);

	initHistogramAnalysis(parameters.histogram, initHistogramAnalysisToRoot);

	initPrelutRangeAnalysis(parameters.initPrelutRange);
	initPrelutRangeAnalysisDisplay(parameters.initPrelutRangeDisplay, parameters.initPrelutRangeDisplayMode);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerRowAnalysis(parameters.initTracksPerRow);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);
	initHardwareHitReadoutDistributionAnalysis(parameters.initHitReadoutDistribution);

}
void analysis::init(initialParameter parameters,
					initialFileParameter fileParameters,
					bool initCreatedHistogramToShow, bool initEncodedHistogramToShow,
					bool initFilteredHistogramToShow, unsigned short initHistogramLayer) {

	this->eventData                = parameters.eventData;
	this->tracks                   = parameters.tracks;
	this->ratings                  = parameters.ratings;
	this->space                    = parameters.space;
	this->luts                     = parameters.luts;

	minimumP                       = parameters.minP;

	configuration                  = parameters.initConfiguration;
	detector                       = parameters.initDetector;
	event                          = parameters.initEvent;
	classPriority                  = parameters.initClassPriority;
	memory                         = parameters.initMemory;
	time                           = parameters.initTime;

	percentageOfHitsInSignature    = parameters.percentageOfHitsInSignature;
	percentageOfTracksForSignature = parameters.percentageOfTracksForSignature;
	analysisResultWarnings         = parameters.analysisResultWarnings;
	analysisResultDisplays         = parameters.analysisResultDisplays;
	analysisMoreResultWarnings     = parameters.analysisMoreResultWarnings;
	analysisMoreResultDisplays     = parameters.analysisMoreResultDisplays;

	if (pictures != NULL) {
		delete pictures;
		pictures = NULL;
	}

	initAutomaticFilterGeometryAnalysis(parameters.histogram, parameters.initAutomatcFilterGeometry);

	initTrackPropagationEventPointAnalysis(parameters.initPropagationEventPoint);
	initTrackPropagationEventHitAnalysis(parameters.initPropagationEventHit);
	initTrackPropagationTotalPointAnalysis(parameters.initPropagationTotalPoint);
	initTrackPropagationTotalHitAnalysis(parameters.initPropagationTotalHit);
	initTrackPropagationAnalysisDisplay(parameters.initPropagationDisplay);
	initTrackPropagationAnalysisToRoot(fileParameters.initPropagationToRoot, fileParameters.name);

	initQualityEFGCEventAbsoluteAnalysis(parameters.initQualityEFGCEventAbsolute);
	initQualityEFGCEventRelativeAnalysis(parameters.initQualityEFGCEventRelative);
	initQualityEFGCTotalAbsoluteAnalysis(parameters.initQualityEFGCTotalAbsolute);
	initQualityEFGCTotalRelativeAnalysis(parameters.initQualityEFGCTotalRelative);

	initMomentumEFGCEventPzEFGCAnalysis(parameters.initMomentumEFGCEventPzEFGC);
	initMomentumEFGCEventPtEFGCAnalysis(parameters.initMomentumEFGCEventPtEFGC);
	initMomentumEFGCTotalPzEFGCAnalysis(parameters.initMomentumEFGCTotalPzEFGC);
	initMomentumEFGCTotalPtEFGCAnalysis(parameters.initMomentumEFGCTotalPtEFGC);

	initProjectionEFGCNEvent12EFGCNAnalysis(parameters.initProjectionEFGCNEvent12EFGCN);
	initProjectionEFGCNEvent13EFGCNAnalysis(parameters.initProjectionEFGCNEvent13EFGCN);
	initProjectionEFGCNEvent32EFGCNAnalysis(parameters.initProjectionEFGCNEvent32EFGCN);
	initProjectionEFGCNTotal12EFGCNAnalysis(parameters.initProjectionEFGCNTotal12EFGCN);
	initProjectionEFGCNTotal13EFGCNAnalysis(parameters.initProjectionEFGCNTotal13EFGCN);
	initProjectionEFGCNTotal32EFGCNAnalysis(parameters.initProjectionEFGCNTotal32EFGCN);

	initMomentumEventAnalysis(parameters.initMomentumEvent);
	initMomentumTotalAnalysis(parameters.initMomentumTotal);
	initMomentumAnalysisDisplay(parameters.initMomentumDisplay);

	initProjectionEventAnalysis(parameters.initProjectionEvent);
	initProjectionTotalAnalysis(parameters.initProjectionTotal);
	initProjectionAnalysisDisplay(parameters.initProjectionDisplay);

	initMomentumAnalysisToRoot(fileParameters.initMomentumToRoot, fileParameters.name);
	initProjectionAnalysisToRoot(fileParameters.initProjectionToRoot, fileParameters.name);

	initMagnetfieldConstantAnalysis(parameters.initMagnetfieldFactors);
	initMagnetfieldConstantAnalysisDisplay(parameters.initMagnetfieldFactorDisplay);

	initMagnetfieldXAnalysis(parameters.initMagnetfieldX);
	initMagnetfieldYAnalysis(parameters.initMagnetfieldY);
	initMagnetfieldZAnalysis(parameters.initMagnetfieldZ);

	initMagnetfieldAnalysisDisplay(parameters.initMagnetfieldDisplay);
	initMagnetfieldAnalysisToRoot(fileParameters.initMagnetfieldToRoot, fileParameters.name);

	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot | fileParameters.initEncodedHistogramToRoot | fileParameters.initFilteredHistogramToRoot, parameters.histogram);
	initHistogramAnalysis(fileParameters.initCreatedHistogramToRoot, fileParameters.initEncodedHistogramToRoot, fileParameters.initFilteredHistogramToRoot, fileParameters.initJustOneCreatedHistogramToRoot, fileParameters.initJustOneEncodedHistogramToRoot, fileParameters.initJustOneFilteredHistogramToRoot, initCreatedHistogramToShow, initEncodedHistogramToShow, initFilteredHistogramToShow, initHistogramLayer, fileParameters.name);

	initPrelutRangeAnalysis(parameters.initPrelutRange);
	initPrelutRangeAnalysisDisplay(parameters.initPrelutRangeDisplay, parameters.initPrelutRangeDisplayMode);
	initPrelutRangeAnalysisToRoot(fileParameters.initPrelutRangeToRoot, fileParameters.name);

	initHardwareTracksPerColumnAnalysis(parameters.initTracksPerColumn);
	initHardwareTracksPerRowAnalysis(parameters.initTracksPerRow);
	initHardwareTracksPerLayerAnalysis(parameters.initTracksPerLayer);
	initHardwareHitReadoutDistributionAnalysis(parameters.initHitReadoutDistribution);

}

/****************************************************************
 * method searches all tracks to find the findable ones and to	*
 * computer their estimated position in the Hough space.		*
 ****************************************************************/

void analysis::setupFindableEstimatedTracks(std::list<findableTrack>* findableTracks, std::streambuf* terminal) {

	trackfinderInputTrack* actualMCTrack;
	trackCoordinates       estimatedPosition;
	findableTrack          actualFindableTrack;
	terminalSequence       statusSequenceForFindableTracks;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (findableTracks == NULL)
		throw cannotAccessFindableTracksMemoryError();

	createTerminalStatusSequence(&statusSequenceForFindableTracks, terminal, "\nSetup findable estimated tracks:\t\t\t", (unsigned int)(*eventData)->getNumberOfTracks());
	terminalInitialize(statusSequenceForFindableTracks);

	findableTracks->clear();

	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		actualMCTrack = (*eventData)->getTrackByOrder(i);
		if (actualMCTrack == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (isFindableStandardTrack(actualMCTrack)) {

			if (estimateCoordinates(*actualMCTrack, &estimatedPosition)) {

				actualFindableTrack.setParameter(actualMCTrack, estimatedPosition);
				findableTracks->push_back(actualFindableTrack);

			}

		}

		terminalOverwriteWithIncrement(statusSequenceForFindableTracks);

	}

	terminalFinalize(statusSequenceForFindableTracks);

}

/****************************************************************
 * method searches all tracks to find the findable ones and to	*
 * computer their computed position in the Hough space.			*
 ****************************************************************/

void analysis::setupFindableComputedTracks(std::list<findableTrack>* findableTracks, std::streambuf* terminal) {

	trackfinderInputTrack* actualMCTrack;
	trackCoordinates       estimatedPosition;
	findableTrack          actualFindableTrack;
	terminalSequence       statusSequenceForFindableTracks;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (findableTracks == NULL)
		throw cannotAccessFindableTracksMemoryError();

	createTerminalStatusSequence(&statusSequenceForFindableTracks, terminal, "\nSetup findable computed tracks:\t\t\t", (unsigned int)(*eventData)->getNumberOfTracks());
	terminalInitialize(statusSequenceForFindableTracks);

	findableTracks->clear();

	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		actualMCTrack = (*eventData)->getTrackByOrder(i);
		if (actualMCTrack == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (isFindableStandardTrack(actualMCTrack)) {

			if (computeCoordinates(*actualMCTrack, &estimatedPosition)) {

				actualFindableTrack.setParameter(actualMCTrack, estimatedPosition);
				findableTracks->push_back(actualFindableTrack);

			}

		}

		terminalOverwriteWithIncrement(statusSequenceForFindableTracks);

	}

	terminalFinalize(statusSequenceForFindableTracks);

}

/****************************************************************
 * method resets the peakfinding geometry for the actual layers	*
 ****************************************************************/

void analysis::resetPeakfindingLayerGeometryElements() {

	if (peakfindingGeometryAnalyser != NULL)
		peakfindingGeometryAnalyser->resetPeakfindingLayerGeometryElements();
	else
		throw cannotAccessPeakfindingGeometryAnalyserError();

}

/****************************************************************
 * method resets the actual used peakfinding geometry			*
 ****************************************************************/

void analysis::resetPeakfindingGeometryElements() {

	if (peakfindingGeometryAnalyser != NULL)
		peakfindingGeometryAnalyser->resetPeakfindingGeometryElements();
	else
		throw cannotAccessPeakfindingGeometryAnalyserError();

}

/****************************************************************
 * method adds the geometry of the actual layer to the other	*
 * layers														*
 ****************************************************************/

void analysis::addPeakfindingLayerGeometry(unsigned short layer) {

	if (peakfindingGeometryAnalyser != NULL)
		peakfindingGeometryAnalyser->addPeakfindingLayerGeometry(layer);
	else
		throw cannotAccessPeakfindingGeometryAnalyserError();

}

/****************************************************************
 * method updates the used peakfinding geometry by adding the	*
 * geometry of the actual layers								*
 ****************************************************************/

void analysis::updatePeakfindingGeometry(trackCoordinates& center) {

	if (peakfindingGeometryAnalyser != NULL)
		peakfindingGeometryAnalyser->updatePeakfindingGeometry(center);
	else
		throw cannotAccessPeakfindingGeometryAnalyserError();

}

/****************************************************************
 * method returns the peakfinding geometry						*
 ****************************************************************/

peakfindingGeometry analysis::getPeakfindingGeometry() {

	peakfindingGeometry returnValue;

	returnValue.reset();

	if (peakfindingGeometryAnalyser != NULL)
		returnValue = peakfindingGeometryAnalyser->getPeakfindingGeometry();
	else
		throw cannotAccessPeakfindingGeometryAnalyserError();

	return returnValue;

}

/****************************************************************
 * method returns true if the track has a the specific track	*
 * pattern and has a specific P.								*
 ****************************************************************/

bool analysis::isFindableTrack(trackfinderInputTrack* track, bitArray minClassPriority, bitArray* globalTrackPattern) {

	bitArray               localTrackPattern;
	trackfinderInputHit*   hit;
	unsigned short         detectorIndex;
	bitArray               _classPriority;
	bool                   returnValue;

	localTrackPattern = bitArray(0);

	if (track == NULL)
		throw cannotAccessHitsOrTracksError(ANALYSISLIB);

	track->resetHitPointer();

	for (unsigned short j = 0; j < track->getNumberOfHits(); j++) {

		hit = track->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (hit->getStation() == NULL)
			throw cannotAccessStationError(ANALYSISLIB);

		detectorIndex = hit->getStation()->getIndex();
		localTrackPattern[detectorIndex]  = true;
		if (globalTrackPattern != NULL)
			(*globalTrackPattern)[detectorIndex] = true;

	}

	_classPriority = evaluateTrackPatternGradingP(localTrackPattern);
	if((_classPriority >= minClassPriority) && (track->getMomZ() > minimumP))
		returnValue = true;
	else
		returnValue = false;
	
	return returnValue;

}

/****************************************************************
 * method returns true if the track has a the specific track	*
 * pattern and has a specific P.								*
 ****************************************************************/

bool analysis::isFindableStandardTrack(trackfinderInputTrack* track) {

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	return isFindableTrack(track, (*ratings)->getGradingPTableMinimumClassification(), NULL);

}

/****************************************************************
 * method returns the number of tracks with a the specific		*
 * track patterns and with a specific P.						*
 ****************************************************************/

int analysis::getNumberOfTracksWithP() {

	return getNumberOfTracksWithP((*ratings)->getGradingPTableMinimumClassification());

}
int analysis::getNumberOfTracksWithP(bitArray minimumClassPriority) {

	bitArray               globalTrackPattern;
	trackfinderInputTrack* track;
	int                    returnValue;
	unsigned short         k;
	unsigned short         numberOfInactiveStations;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((*eventData)->getDetector().getNumberOfActiveStations() > (unsigned short)globalTrackPattern.length())
		throw tooManyActiveStationsError((*eventData)->getDetector().getNumberOfActiveStations(), (unsigned short)globalTrackPattern.length());

	globalTrackPattern = bitArray(0);
	returnValue        = 0;

	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		track = (*eventData)->getTrackByOrder(i);
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (isFindableTrack(track, minimumClassPriority, &globalTrackPattern))
			returnValue++;

	}

	numberOfInactiveStations = 0;
	for (k = 0; k < (*eventData)->getDetector().getNumberOfActiveStations(); k++) {
		
		if (!globalTrackPattern[k])
			numberOfInactiveStations++;

	}

	if (numberOfInactiveStations < (*eventData)->getDetector().getNumberOfActiveStations()) {

		for (k = 0; k < (*eventData)->getDetector().getNumberOfActiveStations(); k++) {

			if (!globalTrackPattern[k]) {

				noHitInDetectorStationWarningMsg* noHitInDetectorStation = new noHitInDetectorStationWarningMsg(k);
				noHitInDetectorStation->warningMsg();
				if(noHitInDetectorStation != NULL) {
					delete noHitInDetectorStation;
					noHitInDetectorStation = NULL;
				}

			}

		}

	}

	return returnValue;

}

/****************************************************************
 * method evaluates the found tracks.							*
 ****************************************************************/

void analysis::evaluateAlgorithm(std::streambuf* terminal) {

	unsigned short         i;
	unsigned short         trackFrequency;
	trackDigitalInformation       trackInfo;
	trackParameter         trackPara;
	trackfinderInputTrack* realTrack;
	bool                   actualTrackIsRealTrack;
	bool                   foundClone;
	bool                   foundWrong;
	bool                   foundFake;
	terminalSequence       statusSequenceForFoundTracks;
	terminalSequence       statusSequenceForRealTracks;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	if (*tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);

	initializeAlgorithmEvaluation();

	(*tracks)->resetActiveObject();

	createTerminalStatusSequence(&statusSequenceForFoundTracks, terminal, "Analyse found tracks:\t\t\t\t", (unsigned int)(*tracks)->getNumberOfTracks());
	terminalInitialize(statusSequenceForFoundTracks);

	for (i = 0; i < (*tracks)->getNumberOfTracks(); i++) {

		(*tracks)->getNextTrackDigitalInfo(&trackInfo);
		(*tracks)->getTrackParameterFromTrackDigitalInfo(trackInfo, &trackPara);

		switch (evaluatePeakContribution(trackInfo, &foundClone, &foundWrong, &foundFake)) {
			case 0:
				if (foundFake)
					peakToFakeTrack(trackInfo.position, trackPara);
				else
					peakToNoTrack(trackInfo.position, trackPara);
				break;
			case 1:
				peakToOneTrack(trackInfo.position, trackPara, foundClone, foundWrong);
				break;
			default:
				peakToMoreTracks(trackInfo.position, trackPara, foundClone, foundWrong);
				break;
		}

		terminalOverwriteWithIncrement(statusSequenceForFoundTracks);

	}

	terminalFinalize(statusSequenceForFoundTracks);

	createTerminalStatusSequence(&statusSequenceForRealTracks, terminal, "Analyse real tracks:\t\t\t\t", (unsigned int)(*eventData)->getNumberOfTracks());
	terminalInitialize(statusSequenceForRealTracks);

	for (i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		realTrack = (*eventData)->getTrackByOrder(i);
		if (realTrack == NULL)
			throw cannotAccessTrackDataError(ANALYSISLIB);

		trackFrequency = realTrack->getFrequency();

		if (realTrack->getMomZ() > minimumP) {

			actualTrackIsRealTrack = evaluateFoundTrackIsRealTrack(realTrack);

			if (trackFrequency > 0) {
				
				if (trackFrequency == 1) {

					if (actualTrackIsRealTrack) {

						trackToOnePeak(*realTrack);
						if ((double)realTrack->getMaximumNumberOfHits() >= WELLFOUNDHITFACTOR * (double)evaluateNumberOfRealTrackHits(realTrack))
							wellFoundTracks(*realTrack);
					
					}
					else
						wrongFoundTracks(*realTrack);
				
				}
				else {

					if (actualTrackIsRealTrack) {

						trackToMorePeaks(*realTrack);
						if ((double)realTrack->getMaximumNumberOfHits() >= WELLFOUNDHITFACTOR * (double)evaluateNumberOfRealTrackHits(realTrack))
							wellFoundTracks(*realTrack);
					
					}
					else
						wrongFoundTracks(*realTrack);
				
				}
			}
			else {

				if(actualTrackIsRealTrack) {
			
					trackToNoPeak(*realTrack);

#ifdef WHICHTRACKIDISNOTFOUND

					terminalFinalize(statusSequenceForRealTracks);

					trackNotFoundWarningMsg* trackNotFound = new trackNotFoundWarningMsg(realTrack);
					trackNotFound->warningMsg();
					if(trackNotFound != NULL) {
						delete trackNotFound;
						trackNotFound = NULL;
					}

					terminalInitialize(statusSequenceForRealTracks);

#endif

				}

			}

		}
		else {

			if (trackFrequency > 0)
				wrongFoundTracks(*realTrack);

		}

		terminalOverwriteWithIncrement(statusSequenceForRealTracks);

	}

	terminalFinalize(statusSequenceForRealTracks);

	finalizeAlgorithmEvaluation();

}

/****************************************************************
 * method evaluates the track propagation goodness.				*
 ****************************************************************/

void analysis::evaluateTrackPropagationGoodness(std::streambuf* terminal) {

	terminalSequence                         statusSequenceForTrackPropagation;
	trackfinderInputTrack*                   track;
	trackMomentum                            momentum;
	rungeKuttaInterface                      computationFormula;
	std::list<trackfinderInputHit>           propagatedHits;
	trackfinderInputHit*                     trackHit;
	std::list<trackfinderInputHit>::iterator propagatedHit;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (trackPropagationAnalyser == NULL)
		throw cannotAccessTrackPropagationAnalyserError();

	computationFormula.init((*eventData)->getDetectorPointer());

	createTerminalStatusSequence(&statusSequenceForTrackPropagation, terminal, "\nAnalyse track propagation goodness:\t\t", (*eventData)->getNumberOfTracks());
	terminalInitialize(statusSequenceForTrackPropagation);

	trackPropagationAnalyser->initializeEvaluation();
	/* Loop over all MCTracks to compute the propagation and compare to the hit and point positions */
	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		/* get each track */
		track = (*eventData)->getTrackByOrder(i);
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		/* test if the track is a findable one */
		if (isFindableStandardTrack(track)) {

			/* propagate the track */
			momentum.set(track->getMomX(), PX);
			momentum.set(track->getMomY(), PY);
			momentum.set(track->getMomZ(), PZ);
			propagatedHits = computationFormula.evaluate(momentum, track->getCharge());

			/* compare the positions */
			track->resetHitPointer();
			for (unsigned short j = 0; j < track->getNumberOfHits(); j++) {

				trackHit = track->getHit();
				if (trackHit == NULL)
					throw cannotAccessHitsOrTracksError(ANALYSISLIB);

				if (trackHit->getStation() != NULL) {

					if (!trackHit->getStation()->isMasked()) {

						/* search for the corresponding propagated hit */
						for (propagatedHit = propagatedHits.begin(); propagatedHit != propagatedHits.end();  propagatedHit++) {

							if (propagatedHit->getStationId() == trackHit->getStationId())
								break;

						}

						if (propagatedHit != propagatedHits.end()) {

							/* do the comparisson and add the value to the graphic objects */
							trackPropagationAnalyser->addHitDistance(trackHit->getStation()->getIndex(), sqrt(sqr(trackHit->getPosX() - propagatedHit->getPosX()) + sqr(trackHit->getPosY() - propagatedHit->getPosY()) + sqr(trackHit->getPosZ() - propagatedHit->getPosZ())));

							if (trackHit->getPoint() != NULL) {
								trackPropagationAnalyser->addPointDistance(trackHit->getStation()->getIndex(), sqrt(sqr(trackHit->getPoint()->GetX() - propagatedHit->getPosX()) + sqr(trackHit->getPoint()->GetY() - propagatedHit->getPosY()) + sqr(trackHit->getPoint()->GetZ() - propagatedHit->getPosZ())));
							}

						}

					}

				}

			}

		}

		terminalOverwriteWithIncrement(statusSequenceForTrackPropagation);

	}
	trackPropagationAnalyser->finalizeEvaluation();

	terminalFinalize(statusSequenceForTrackPropagation);

}

/****************************************************************
 * method evaluates the magnetfield factors.					*
 ****************************************************************/

void analysis::evaluateMagnetfieldFactors(bool isFirstEvent, bool averagingFactors, std::streambuf* terminal) {

	trackfinderInputMagneticField* magneticField;
	unsigned short                 j;
	terminalSequence               statusSequenceForFactors;
	std::list<findableTrack>       findableTracks;
	int                            wrongThirdDimensionBorder;
	double                         actualFactor;
	stationMinimum*                stationInfo;
	double                         actualHoughDistance;
	double                         minimalHoughDistance;
	trackfinderInputTrack*         track;
	trackfinderInputHit*           hit;
	unsigned short                 stationIndex;
	lutBorder                      border;
	prelutHoughBorder*             firstBorder;
	lutHoughBorder*                secondBorder;
	houghBorderPosition            actualHoughCoord;
	double                         _magnetfieldFactor;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	magneticField = (*eventData)->getMagneticField();
	if (magneticField == NULL)
		throw cannotAccessMagneticFieldObjectError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError(ANALYSISLIB);

	if (magnetfieldFactorAnalyser == NULL)
		throw cannotAccessMagnetfieldFactorAnalyserError();

	magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());

	firstBorder  = border.getPrelutHoughBorderPointer();
	secondBorder = border.getLutHoughBorderPointer();

	/* allocate the memory to store the actual freqency and
	 * the minimum distance value for each station for one hit. */
	stationInfo = new stationMinimum[(*eventData)->getNumberOfActiveStations()];
	if (stationInfo == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableEstimatedTracks(&findableTracks, terminal);

	/* the counter for the number of hits which have no entry in the
	 * correct third dimension */
	wrongThirdDimensionBorder = 0;

	/* reset the counter for the corrections in the first and second
	 * dimension. Corrected are these values which have a difference
	 * of more than one in the first dimension according to the previous
	 * value */
	if ((*luts)->typeUsesCorrections())
		(*luts)->resetCorrectionCounter();

	createTerminalStatusSequence(&statusSequenceForFactors, terminal, "Analyse magnetfield factors:\t", (unsigned int)(magnetfieldFactorAnalyser->getNumberOfFactors() * findableTracks.size()));
	terminalInitialize(statusSequenceForFactors);

	/* loop to evaluate the minimal distance for each magnetfield factor */
	for (unsigned short i = 0; i < magnetfieldFactorAnalyser->getNumberOfFactors(); i++) {

		actualFactor = magnetfieldFactorAnalyser->getFactor(i);

		/* set the magnetfield factor for the lut computation */
		if ((*luts)->typeUsesMagnetism())
			(*luts)->setMagneticFieldFactor(actualFactor);

		/* reset the actual distances for this magnetfield factor */
		for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++)
			magnetfieldFactorAnalyser->setDistance(j, i, 0);

		/* loop to evaluate the magnetfield factor for all findable tracks */
		for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

			track = actualTrack->getTrack();
			if (track == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			/* reset the frequency of hits occuring in the same station and
			 * the value of the minimal distance for the station */
			for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++) {
				stationInfo[j].frequency = 0;
				stationInfo[j].value     = 0;
			}
			track->resetHitPointer();

			/* loop to evaluate the magnetfield factor for all hits of this track */
			for (unsigned short k = 0; k < track->getNumberOfHits(); k++) {

				hit = track->getHit();
				if (hit == NULL)
					throw cannotAccessHitsOrTracksError(ANALYSISLIB);

				if (hit->getStation() == NULL)
					throw cannotAccessStationError(ANALYSISLIB);
				else
					stationIndex = hit->getStation()->getIndex();

				/* compute the borders for this hit */
				(*luts)->evaluate(hit, &border);

				if (secondBorder->houghCoord.getNumberOfEntries() == 0) {

					/* separate case because there is no houghCoord in the range.
					 * Take always the biggest value to fall out of range. */
					minimalHoughDistance = sqrt((double)sqr(0 - actualTrack->getPosition().get(DIM1)) + (double)sqr(0 - actualTrack->getPosition().get(DIM2)));
					actualHoughDistance  = sqrt((double)sqr((*space)->getStep(DIM1) - actualTrack->getPosition().get(DIM1)) + (double)sqr((*space)->getStep(DIM2) - actualTrack->getPosition().get(DIM2)));

					if (actualHoughDistance > minimalHoughDistance)
						minimalHoughDistance = actualHoughDistance;

				}
				else {

					secondBorder->houghCoord.resetActiveObject();

					/* loop to evaluate the minimal distance for this hit */
					for (unsigned int l = 0; l < secondBorder->houghCoord.getNumberOfEntries(); l++) {

						actualHoughCoord     = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();
						actualHoughDistance  = sqrt((double)sqr(actualHoughCoord.pos1 - actualTrack->getPosition().get(DIM1)) + (double)sqr(actualHoughCoord.pos2 - actualTrack->getPosition().get(DIM2)));

						if (l == 0) {

							minimalHoughDistance = actualHoughDistance;

						}
						else {

							if (actualHoughDistance < minimalHoughDistance)
								minimalHoughDistance = actualHoughDistance;

						}

					}

				}

				/* Check if the third dimension border is in the range */
				if ((firstBorder->start <= actualTrack->getPosition().get(DIM3)) && (firstBorder->stop >= actualTrack->getPosition().get(DIM3))) {

					/* Add the minimal distance of the hit to the minimal
					 * distance of the correct station. Here it can happen
					 * that more than one hit can belong to the same station.
					 * This is counted by the frequency. This variable holds
					 * the information for all hits of one track. */
					stationInfo[stationIndex].frequency++;
					stationInfo[stationIndex].value += minimalHoughDistance;

				}
				else
					wrongThirdDimensionBorder++;

			}

			/* Loop to add the minimal distance for all stations of this track.
			 * If the frequency is zero, no hit would be in this station.
			 * If the frequency is one, one hit would be in this station.
			 * If the frequency is bigger than one, more than one hit would
			 * be in this station. In this case, the middle is taken as
			 * minimal distance for this station. */
			for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++) {
	
				if (stationInfo[j].frequency > 0)
					magnetfieldFactorAnalyser->addDistance(j, i, stationInfo[j].value / stationInfo[j].frequency);

			}

			terminalOverwriteWithIncrement(statusSequenceForFactors);

		}

	}

	terminalFinalize(statusSequenceForFactors);

	if (stationInfo != NULL) {
		delete [] stationInfo;
		stationInfo = NULL;
	}

#ifdef ENABLEALLWARNINGS

	if (wrongThirdDimensionBorder > 0) {

		numberOfWrongThirdDimensionBordersFoundWarningMsg* numberOfWrongThirdDimensionBordersFound = new numberOfWrongThirdDimensionBordersFoundWarningMsg(wrongThirdDimensionBorder / magnetfieldFactorAnalyser->getNumberOfFactors());
		numberOfWrongThirdDimensionBordersFound->warningMsg();
		if(numberOfWrongThirdDimensionBordersFound != NULL) {
			delete numberOfWrongThirdDimensionBordersFound;
			numberOfWrongThirdDimensionBordersFound = NULL;
		}

	}

#endif

	/* this sets the magnetfield factors for each station for the computing of the hough transformation */
	for (j = 0; j < (*eventData)->getNumberOfActiveStations(); j++) {
	
		_magnetfieldFactor = magnetfieldFactorAnalyser->getMagnetfieldFactor(j);

		if (averagingFactors && !isFirstEvent)
			magneticField->updateMagnetfieldFactor(j, _magnetfieldFactor);
		else
			magneticField->setMagnetfieldFactor(j, _magnetfieldFactor);

		if (analysisResultWarnings & MAGNETFIELDFACTORINFORMATION) {

			setMagnetfieldFactorWarningMsg* setMagnetfieldFactor = new setMagnetfieldFactorWarningMsg(j, magneticField->getMagnetfieldFactor(j));
			setMagnetfieldFactor->warningMsg();
			if(setMagnetfieldFactor != NULL) {
				delete setMagnetfieldFactor;
				setMagnetfieldFactor = NULL;
			}

		}

	}

}

/****************************************************************
 * method evaluates the dim3 entry range.						*
 ****************************************************************/

void analysis::evaluatePrelutRange(bool isFirstEvent, bool averagingFactors, bool chooseMainPrelutRange, bool chooseConstraintPrelutRange, std::streambuf* terminal) {

	prelutMath                      prelut;
	unsigned short                  k;
	unsigned short                  l;
	terminalSequence                statusSequenceForDim3EntryRange;
	std::list<findableTrack>        findableTracks;
	trackfinderInputTrack*          track;
	trackfinderInputHit*            hit;
	prelutHoughBorder               firstBorder;
	unsigned short                  stationIndex;
	hitsInHistogramBorderFrequency* histogramBorderFrequency;
	unsigned short                  numberOfHistogramBorderEntries;
	unsigned short                  borderIndex;
	hitsInHistogramBorderFrequency  maximumHistogramBorderFrequency;
	bitArray                        trackSignature;
	double                          evaluatedDim3StartEntry;
	double                          evaluatedDim3StopEntry;
	unsigned short                  evaluatedDim3Info;
	double                          evaluatedDim3MeanEntriesPerHit;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError();

	if (prelutRangeLayerAnalyser == NULL)
		throw cannotAccessPrelutRangeAnalyserError();

	prelutRangeLayerAnalyser->reset();

	/* This analysis can just be made for the Math version of the prelut */
	prelut.init((*luts)->getPrelutDefinition().dim3Min, (*luts)->getPrelutDefinition().dim3Max, (*luts)->getPrelutDefinition().dim3Step, 0, 0);

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForDim3EntryRange, terminal, "Analyse prelut range:\t\t\t\t", (unsigned int)(findableTracks.size() * prelutRangeLayerAnalyser->getNumberOfMinFactors() * prelutRangeLayerAnalyser->getNumberOfMaxFactors()));
	terminalInitialize(statusSequenceForDim3EntryRange);

	histogramBorderFrequency       = NULL;
	numberOfHistogramBorderEntries = 0;

	for (unsigned short i = 0; i < prelutRangeLayerAnalyser->getNumberOfMinFactors(); i++) {

		for (unsigned short j = 0; j < prelutRangeLayerAnalyser->getNumberOfMaxFactors(); j++) {

			/* the min bigger than the max value does not make sense for evaluation */
			if (prelutRangeLayerAnalyser->getMinFactor(i) > prelutRangeLayerAnalyser->getMaxFactor(j))
				continue;

			/* sets the values for the next computation */
			prelut.set(prelutRangeLayerAnalyser->getMinFactor(i), prelutRangeLayerAnalyser->getMaxFactor(j));

			for (k = 0; k < (*eventData)->getNumberOfActiveStations(); k++) {

				if ((i == 0) && (j == 0))
					prelutRangeLayerAnalyser->setRelativeValue(k, 0);

				prelutRangeLayerAnalyser->setValue(k, i, j, 0);

			}

			/* loop to evaluate the prelut goodness for all findable tracks */
			for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

				track = actualTrack->getTrack();
				if (track == NULL)
					throw cannotAccessHitsOrTracksError(ANALYSISLIB);

				track->resetHitPointer();

				/* loop to evaluate the magnetfield factor for all hits of this track */
				for (k = 0; k < track->getNumberOfHits(); k++) {

					hit = track->getHit();
					if (hit == NULL)
						throw cannotAccessHitsOrTracksError(ANALYSISLIB);

					if (hit->getStation() == NULL)
						throw cannotAccessStationError(ANALYSISLIB);
					else
						stationIndex = hit->getStation()->getIndex();

					/* compute the borders for this hit */
					prelut.evaluate(hit, &firstBorder);

					/* Accumulate the frequency of the third dimension border */
					for (l = firstBorder.start; l <= firstBorder.stop; l++) {

						if (numberOfHistogramBorderEntries == 0) {

							numberOfHistogramBorderEntries        = 1;
							histogramBorderFrequency              = (hitsInHistogramBorderFrequency*)calloc(numberOfHistogramBorderEntries, sizeof(hitsInHistogramBorderFrequency));
							histogramBorderFrequency[0].border    = l;
							histogramBorderFrequency[0].frequency = 1;

						}
						else {
					
							/* check if this border has occured until now */
							borderIndex = numberOfHistogramBorderEntries;
							for (unsigned short m = 0; m < numberOfHistogramBorderEntries; m++) {

								if (histogramBorderFrequency[m].border == l) {

									borderIndex = m;
									break;

								}

							}
					
							/* if the border has not occured */
							if (borderIndex == numberOfHistogramBorderEntries) {

								numberOfHistogramBorderEntries++;
								histogramBorderFrequency                        = (hitsInHistogramBorderFrequency*)realloc(histogramBorderFrequency, numberOfHistogramBorderEntries * sizeof(hitsInHistogramBorderFrequency));
								histogramBorderFrequency[borderIndex].border    = l;
								histogramBorderFrequency[borderIndex].frequency = 1;

							}
							/* if the border has occured */
							else
								histogramBorderFrequency[borderIndex].frequency++;

						}

					}

				}

				maximumHistogramBorderFrequency.frequency = 0;
				maximumHistogramBorderFrequency.distance  = 0;
				for (k = 0; k < numberOfHistogramBorderEntries; k++) {

					histogramBorderFrequency[k].distance = sqrt((double)sqr(histogramBorderFrequency[k].border - actualTrack->getPosition().get(DIM3)));

					if (histogramBorderFrequency[k].frequency > maximumHistogramBorderFrequency.frequency)
						maximumHistogramBorderFrequency = histogramBorderFrequency[k];
					/* This is added because if there is a equal found clone peak which is nearer to the
					 * correct cell, this peak should be used because of a better quality approximation */
					else if ((histogramBorderFrequency[k].frequency == maximumHistogramBorderFrequency.frequency) && (histogramBorderFrequency[k].distance < maximumHistogramBorderFrequency.distance))
						maximumHistogramBorderFrequency = histogramBorderFrequency[k];

				}

				if (histogramBorderFrequency != NULL) {
					free(histogramBorderFrequency);
					histogramBorderFrequency       = NULL;
					numberOfHistogramBorderEntries = 0;
				}

				trackSignature = bitArray(0);
				/* loop to evaluate the not correct layer anaylsis*/
				for (k = 0; k < track->getNumberOfHits(); k++) {

					hit = track->getHit();
					if (hit == NULL)
						throw cannotAccessHitsOrTracksError(ANALYSISLIB);

					if (hit->getStation() == NULL)
						throw cannotAccessStationError(ANALYSISLIB);
					else
						stationIndex = hit->getStation()->getIndex();

					/* compute the borders for this hit */
					prelut.evaluate(hit, &firstBorder);

					/* Prevent a track with multiple hits in one station */
					if ((trackSignature[stationIndex] == false)) {

						trackSignature[stationIndex] = true;

						/* Count the number of hits per detector station  */
						if ((i == 0) && (j == 0))
							prelutRangeLayerAnalyser->addRelativeValue(stationIndex, 1);

						/* Count the border range for the hit*/
						prelutRangeLayerAnalyser->addConstraintValue(stationIndex, i, j, firstBorder.stop - firstBorder.start + 1);

						/* Check if the third dimension border is in the correct layer range */
						if ((firstBorder.start <= actualTrack->getPosition().get(DIM3)) && (firstBorder.stop >= actualTrack->getPosition().get(DIM3)))
							prelutRangeLayerAnalyser->addCorrectValue(stationIndex, i, j, 1);

						/* Check if the third dimension border is in the layer range */
						if ((firstBorder.start <= maximumHistogramBorderFrequency.border) && (firstBorder.stop >= maximumHistogramBorderFrequency.border))
							prelutRangeLayerAnalyser->addMainValue(stationIndex, i, j, 1);

					}

				}

				terminalOverwriteWithIncrement(statusSequenceForDim3EntryRange);
			}

		}

	}

	terminalFinalize(statusSequenceForDim3EntryRange);

	/* this sets the prelut range for the computing of the hough transformation */
	if (chooseMainPrelutRange) {
		if (chooseConstraintPrelutRange)
			prelutRangeLayerAnalyser->getMainPrelutConstraintRange((*eventData)->getNumberOfActiveStations(), evaluatedDim3StartEntry, evaluatedDim3StopEntry, evaluatedDim3Info, evaluatedDim3MeanEntriesPerHit);
		else
			prelutRangeLayerAnalyser->getMainPrelutMaximumRange((*eventData)->getNumberOfActiveStations(), evaluatedDim3StartEntry, evaluatedDim3StopEntry, evaluatedDim3Info, evaluatedDim3MeanEntriesPerHit);
	}
	else {
		if (chooseConstraintPrelutRange)
			prelutRangeLayerAnalyser->getCorrectPrelutConstraintRange((*eventData)->getNumberOfActiveStations(), evaluatedDim3StartEntry, evaluatedDim3StopEntry, evaluatedDim3Info, evaluatedDim3MeanEntriesPerHit);
		else
			prelutRangeLayerAnalyser->getCorrectPrelutMaximumRange((*eventData)->getNumberOfActiveStations(), evaluatedDim3StartEntry, evaluatedDim3StopEntry, evaluatedDim3Info, evaluatedDim3MeanEntriesPerHit);
	}

	if (averagingFactors && !isFirstEvent) {

		if (evaluatedDim3StartEntry > (*luts)->getPrelutDefinition().dim3StartEntry)
			evaluatedDim3StartEntry = (*luts)->getPrelutDefinition().dim3StartEntry;

		if (evaluatedDim3StopEntry < (*luts)->getPrelutDefinition().dim3StopEntry)
			evaluatedDim3StopEntry = (*luts)->getPrelutDefinition().dim3StopEntry;

		(*luts)->setPrelutRange(evaluatedDim3StartEntry, evaluatedDim3StopEntry);

	}
	else {

		(*luts)->setPrelutRange(evaluatedDim3StartEntry, evaluatedDim3StopEntry);

	}

	if (analysisResultWarnings & PRELUTRANGEINFORMATION) {

		if (chooseConstraintPrelutRange) {

			numberOfPrelutRangesFoundWarningMsg* numberOfPrelutRangesFound = new numberOfPrelutRangesFoundWarningMsg(evaluatedDim3Info);
			numberOfPrelutRangesFound->warningMsg();
			if(numberOfPrelutRangesFound != NULL) {
				delete numberOfPrelutRangesFound;
				numberOfPrelutRangesFound = NULL;
			}

		}
		else {

			maximumPrelutRangePercentageWarningMsg* maximumPrelutRangePercentage = new maximumPrelutRangePercentageWarningMsg(evaluatedDim3Info);
			maximumPrelutRangePercentage->warningMsg();
			if(maximumPrelutRangePercentage != NULL) {
				delete maximumPrelutRangePercentage;
				maximumPrelutRangePercentage = NULL;
			}

		}

		meanPrelutRangeEntriesPerHitWarningMsg* meanPrelutRangeEntriesPerHit = new meanPrelutRangeEntriesPerHitWarningMsg(evaluatedDim3MeanEntriesPerHit);
		meanPrelutRangeEntriesPerHit->warningMsg();
		if(meanPrelutRangeEntriesPerHit != NULL) {
			delete meanPrelutRangeEntriesPerHit;
			meanPrelutRangeEntriesPerHit = NULL;
		}

		setPrelutRangeWarningMsg* setPrelutRange = new setPrelutRangeWarningMsg((*luts)->getPrelutDefinition().dim3StartEntry, (*luts)->getPrelutDefinition().dim3StopEntry);
		setPrelutRange->warningMsg();
		if(setPrelutRange != NULL) {
			delete setPrelutRange;
			setPrelutRange = NULL;
		}

	}

}

/****************************************************************
 * method evaluates the goodness of the prelut.					*
 ****************************************************************/

void analysis::evaluatePrelutGoodness(std::streambuf* terminal) {

	unsigned int                    i;
	unsigned short                  j;
	terminalSequence                statusSequenceForPrelut;
	std::list<findableTrack>        findableTracks;
	trackfinderInputTrack*          track;
	trackfinderInputHit*            hit;
	prelutHoughBorder               firstBorder;
	unsigned short                  stationIndex;
	double                          baseSlope;
	double                          actualSlope;
	double                          allowedSlope;
	unsigned short                  numberOfHitsMeetingCorrectLayer;
	unsigned int*                   numberOfTracksWithPercentCorrectHitsInLayer;
	unsigned short                  numberOfHitsMeetingCorrectSlope;
	unsigned int*                   numberOfTracksWithPercentCorrectSlope;
	hitsInHistogramBorderFrequency* histogramBorderFrequency;
	unsigned short                  numberOfHistogramBorderEntries;
	unsigned short                  borderIndex;
	hitsInHistogramBorderFrequency  maximumHistogramBorderFrequency;
	unsigned int*                   numberOfTracksWithPercentHitsInLayer;
	double                          distanceOfMaximumLayerAndCorrectLayer;
	unsigned long                   numberOfTracksWithGoodPrelutSignature;
	bitArray                        trackSignature;
	bitArray                        correctLayerSignature;
	bitArray                        bestLayerSignature;
	unsigned short                  numberOfCorrectSignaturePositions;
	double*                         yPositions;
	double*                         zPositions;
	unsigned short                  numberOfPositions;
	unsigned int*                   numberOfTracksWithPercentCorrectHitsInCell;
	unsigned int*                   numberOfTracksWithPercentHitsInCell;
	unsigned short                  actualNumberOfTracksForSignatureAcceptance;
	unsigned short                  minimalNumberOfTracksForSignatureAcceptance;
	bitArray                        actualSignatureForSignatureAcceptance;
	bitArray                        searchSignatureForSignatureAcceptance;

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	unsigned short                  actualClassForSignatureAcceptance;
	bool                            searchFoundForSignatureAcceptance;

#endif

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError();

	numberOfTracksWithPercentCorrectHitsInLayer  = new unsigned int[NUMBEROFDIFFERENTCORRECTHITS];
	if (numberOfTracksWithPercentCorrectHitsInLayer == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentCorrectSlope = new unsigned int[NUMBEROFDIFFERENTCORRECTHITS];
	if (numberOfTracksWithPercentCorrectSlope == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentHitsInLayer  = new unsigned int[NUMBEROFDIFFERENTCORRECTHITS];
	if (numberOfTracksWithPercentHitsInLayer == NULL)
		throw memoryAllocationError(ANALYSISLIB);


	numberOfTracksWithPercentCorrectHitsInCell  = new unsigned int[1 << (unsigned int)trackSignature.length()];
	if (numberOfTracksWithPercentCorrectHitsInCell == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentHitsInCell  = new unsigned int[1 << (unsigned int)trackSignature.length()];
	if (numberOfTracksWithPercentHitsInCell == NULL)
		throw memoryAllocationError(ANALYSISLIB);


	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForPrelut, terminal, "Analyse prelut goodness:\t", (unsigned int)findableTracks.size());
	terminalInitialize(statusSequenceForPrelut);

	for (i = 0; i < NUMBEROFDIFFERENTCORRECTHITS; i++) {

		numberOfTracksWithPercentCorrectHitsInLayer[i]  = 0;
		numberOfTracksWithPercentCorrectSlope[i]        = 0;
		numberOfTracksWithPercentHitsInLayer[i]         = 0;

	}

	for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++) {

		numberOfTracksWithPercentCorrectHitsInCell[i] = 0;
		numberOfTracksWithPercentHitsInCell[i]        = 0;

	}

	histogramBorderFrequency              = NULL;
	numberOfHistogramBorderEntries        = 0;
	distanceOfMaximumLayerAndCorrectLayer = 0;
	numberOfTracksWithGoodPrelutSignature = 0;

	yPositions                            = NULL;
	zPositions                            = NULL;
	numberOfPositions                     = 0;

	/* loop to evaluate the prelut goodness for all findable tracks */
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		track->resetHitPointer();

		numberOfHitsMeetingCorrectLayer = 0;
		numberOfHitsMeetingCorrectSlope = 0;
		baseSlope                       = track->getMomY() / track->getMomZ();
		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (j = 0; j < track->getNumberOfHits(); j++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluatePrelut(hit, &firstBorder);

			/* Check if the third dimension border is in the range */
			if ((firstBorder.start <= actualTrack->getPosition().get(DIM3)) && (firstBorder.stop >= actualTrack->getPosition().get(DIM3)))
				numberOfHitsMeetingCorrectLayer++;

			/* Check if the slope is in the range */
			actualSlope  = hit->getHit()->GetY() / hit->getHit()->GetZ();
			allowedSlope = STARTSLOPE + stationIndex * INCRSLOPE;

			if ((actualSlope >= baseSlope - allowedSlope) && (actualSlope <= baseSlope + allowedSlope))
				numberOfHitsMeetingCorrectSlope++;

			/* Accumulate the frequency of the third dimension border */
			for (unsigned short k = firstBorder.start; k <= firstBorder.stop; k++) {

				if (numberOfHistogramBorderEntries == 0) {

					numberOfHistogramBorderEntries        = 1;
					histogramBorderFrequency              = (hitsInHistogramBorderFrequency*)calloc(numberOfHistogramBorderEntries, sizeof(hitsInHistogramBorderFrequency));
					histogramBorderFrequency[0].border    = k;
					histogramBorderFrequency[0].frequency = 1;

				}
				else {
					
					/* check if this border has occured until now */
					borderIndex = numberOfHistogramBorderEntries;
					for (unsigned short l = 0; l < numberOfHistogramBorderEntries; l++) {

						if (histogramBorderFrequency[l].border == k) {

							borderIndex = l;
							break;

						}

					}
					
					/* if the border has not occured */
					if (borderIndex == numberOfHistogramBorderEntries) {

						numberOfHistogramBorderEntries++;
						histogramBorderFrequency                        = (hitsInHistogramBorderFrequency*)realloc(histogramBorderFrequency, numberOfHistogramBorderEntries * sizeof(hitsInHistogramBorderFrequency));
						histogramBorderFrequency[borderIndex].border    = k;
						histogramBorderFrequency[borderIndex].frequency = 1;

					}
					/* if the border has occured */
					else
						histogramBorderFrequency[borderIndex].frequency++;

				}

			}

			if (track->getTrackIndex() == 1343) {

				numberOfPositions = track->getNumberOfHits();
				if (yPositions == NULL) {
					yPositions = new double[numberOfPositions];
					if (yPositions == NULL)
						throw memoryAllocationError(ANALYSISLIB);
				}
				if (zPositions == NULL) {
					zPositions = new double[numberOfPositions];
					if (zPositions == NULL)
						throw memoryAllocationError(ANALYSISLIB);
				}

				yPositions[j] = hit->getHit()->GetY();
				zPositions[j] = hit->getHit()->GetZ();

			}

		}

#ifdef DEBUGTRACKINDEXOFTRACKSWITHPERCENTAGEOFCORRECTHITS

		/* By enabling this you can write the trackIndex of the tracks which have a defined percentage of correct hits to the standard output */
		if ((unsigned short)((((double)numberOfHitsMeetingCorrectSlope * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits())) == 6)
			std::cout << numberOfTracksWithPercentCorrectSlope[(unsigned short)((((double)numberOfHitsMeetingCorrectSlope * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))] << ": " << track->getTrackIndex() << std::endl;

#endif

		numberOfTracksWithPercentCorrectHitsInLayer[(unsigned short)((((double)numberOfHitsMeetingCorrectLayer * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))]++;
		numberOfTracksWithPercentCorrectSlope[(unsigned short)((((double)numberOfHitsMeetingCorrectSlope * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))]++;

		maximumHistogramBorderFrequency.frequency = 0;
		maximumHistogramBorderFrequency.distance  = 0;
		for (j = 0; j < numberOfHistogramBorderEntries; j++) {

			histogramBorderFrequency[j].distance = sqrt((double)sqr(histogramBorderFrequency[j].border - actualTrack->getPosition().get(DIM3)));

			if (histogramBorderFrequency[j].frequency > maximumHistogramBorderFrequency.frequency)
				maximumHistogramBorderFrequency = histogramBorderFrequency[j];
			/* This is added because if there is a equal found clone peak which is nearer to the
			 * correct cell, this peak should be used because of a better quality approximation */
			else if ((histogramBorderFrequency[j].frequency == maximumHistogramBorderFrequency.frequency) && (histogramBorderFrequency[j].distance < maximumHistogramBorderFrequency.distance))
				maximumHistogramBorderFrequency = histogramBorderFrequency[j];

		}

		numberOfTracksWithPercentHitsInLayer[(unsigned short)((((double)maximumHistogramBorderFrequency.frequency * (NUMBEROFDIFFERENTCORRECTHITS - 1)) / (double)track->getNumberOfHits()) + ((double)(track->getNumberOfHits() - 1) / (double)track->getNumberOfHits()))]++;

		distanceOfMaximumLayerAndCorrectLayer += (maximumHistogramBorderFrequency.distance / (double)findableTracks.size());

		if (histogramBorderFrequency != NULL) {
			free(histogramBorderFrequency);
			histogramBorderFrequency       = NULL;
			numberOfHistogramBorderEntries = 0;
		}

		trackSignature        = bitArray(0);
		correctLayerSignature = bitArray(0);
		bestLayerSignature    = bitArray(0);
		/* loop to evaluate the total prelut anaylsis*/
		for (j = 0; j < track->getNumberOfHits(); j++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluatePrelut(hit, &firstBorder);

			/* Check if the third dimension border is in the range */
			trackSignature[stationIndex] = true;
			if ((firstBorder.start <= actualTrack->getPosition().get(DIM3)) && (firstBorder.stop >= actualTrack->getPosition().get(DIM3)))
				correctLayerSignature[stationIndex] = true;
			if ((firstBorder.start <= maximumHistogramBorderFrequency.border) && (firstBorder.stop >= maximumHistogramBorderFrequency.border))
				bestLayerSignature[stationIndex] = true;

		}

		numberOfTracksWithPercentCorrectHitsInCell[correctLayerSignature.toULong()]++;

		numberOfTracksWithPercentHitsInCell[bestLayerSignature.toULong()]++;

		numberOfCorrectSignaturePositions = 0;
		for (unsigned short m = 0; m < trackSignature.length(); m++) {

			if (trackSignature[m] && (trackSignature[m] == bestLayerSignature[m]))
				numberOfCorrectSignaturePositions++;

		}

		if (numberOfCorrectSignaturePositions >= (unsigned short)ceil(((double)percentageOfHitsInSignature / (double)100) * (double)trackSignature.count()))
			numberOfTracksWithGoodPrelutSignature++;

		terminalOverwriteWithIncrement(statusSequenceForPrelut);

	}

	addNumberOfTracksWithGoodPrelutSignature(numberOfTracksWithGoodPrelutSignature);

	terminalFinalize(statusSequenceForPrelut);

	if (analysisResultWarnings & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		trackWithHitsInCorrectCellDistributionWarningMsg* trackWithHitsInCorrectCellDistribution = new trackWithHitsInCorrectCellDistributionWarningMsg(numberOfTracksWithPercentCorrectHitsInCell, (1 << (unsigned int)trackSignature.length()));
		trackWithHitsInCorrectCellDistribution->warningMsg();
		if(trackWithHitsInCorrectCellDistribution != NULL) {
			delete trackWithHitsInCorrectCellDistribution;
			trackWithHitsInCorrectCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINCELLDISTRIBUTION) {

		trackWithHitsInCellDistributionWarningMsg* trackWithHitsInCellDistribution = new trackWithHitsInCellDistributionWarningMsg(numberOfTracksWithPercentHitsInCell, (1 << (unsigned int)trackSignature.length()));
		trackWithHitsInCellDistribution->warningMsg();
		if(trackWithHitsInCellDistribution != NULL) {
			delete trackWithHitsInCellDistribution;
			trackWithHitsInCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINCORRECTBORDERDISTRIBUTION) {

		trackWithHitsInCorrectLayerDistributionWarningMsg* trackWithHitsInCorrectLayerDistribution = new trackWithHitsInCorrectLayerDistributionWarningMsg(numberOfTracksWithPercentCorrectHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);
		trackWithHitsInCorrectLayerDistribution->warningMsg();
		if(trackWithHitsInCorrectLayerDistribution != NULL) {
			delete trackWithHitsInCorrectLayerDistribution;
			trackWithHitsInCorrectLayerDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINBORDERDISTRIBUTION) {

		trackWithHitsInLayerDistributionWarningMsg* trackWithHitsInLayerDistribution = new trackWithHitsInLayerDistributionWarningMsg(numberOfTracksWithPercentHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);
		trackWithHitsInLayerDistribution->warningMsg();
		if(trackWithHitsInLayerDistribution != NULL) {
			delete trackWithHitsInLayerDistribution;
			trackWithHitsInLayerDistribution = NULL;
		}

	}

	if (analysisResultWarnings & MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYER) {

		middleDistanceOfMaximumAndCorrectLayerWarningMsg* middleDistanceOfMaximumAndCorrectLayer = new middleDistanceOfMaximumAndCorrectLayerWarningMsg(distanceOfMaximumLayerAndCorrectLayer);
		middleDistanceOfMaximumAndCorrectLayer->warningMsg();
		if(middleDistanceOfMaximumAndCorrectLayer != NULL) {
			delete middleDistanceOfMaximumAndCorrectLayer;
			middleDistanceOfMaximumAndCorrectLayer = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITWITHCORRECTSLOPEDISTRIBUTION) {

		trackWithHitsWithCorrectSlopeDistributionWarningMsg* trackWithHitsWithCorrectSlopeDistribution = new trackWithHitsWithCorrectSlopeDistributionWarningMsg(numberOfTracksWithPercentCorrectSlope, NUMBEROFDIFFERENTCORRECTHITS);
		trackWithHitsWithCorrectSlopeDistribution->warningMsg();
		if(trackWithHitsWithCorrectSlopeDistribution != NULL) {
			delete trackWithHitsWithCorrectSlopeDistribution;
			trackWithHitsWithCorrectSlopeDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectPrelutCellDistribution(numberOfTracksWithPercentCorrectHitsInCell, (1 << (unsigned int)trackSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHHITINCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addPrelutCellDistribution(numberOfTracksWithPercentHitsInCell, (1 << (unsigned int)trackSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHHITINCORRECTBORDERDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectLayerDistribution(numberOfTracksWithPercentCorrectHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);

	}

	if (analysisResultDisplays & TRACKWITHHITINBORDERDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addLayerDistribution(numberOfTracksWithPercentHitsInLayer, NUMBEROFDIFFERENTCORRECTHITS);

	}

	if (analysisResultDisplays & TRACKWITHHITWITHCORRECTSLOPEDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectSlopeDistribution(numberOfTracksWithPercentCorrectSlope, NUMBEROFDIFFERENTCORRECTHITS);

	}

	if (analysisResultDisplays & TRACKWITHYZPROJECTION) {

		initializeShowAnalysis();

		showAnalyser->addYZProjection(yPositions, zPositions, numberOfPositions);

	}

	actualNumberOfTracksForSignatureAcceptance  = 0;
	minimalNumberOfTracksForSignatureAcceptance = 0;
	for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++)
		minimalNumberOfTracksForSignatureAcceptance += numberOfTracksWithPercentHitsInCell[i];
	minimalNumberOfTracksForSignatureAcceptance = (unsigned short)(((double)percentageOfTracksForSignature / (double)100) * (double)minimalNumberOfTracksForSignatureAcceptance);

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	actualClassForSignatureAcceptance = (unsigned short)trackSignature.length();

#endif

	while (minimalNumberOfTracksForSignatureAcceptance > actualNumberOfTracksForSignatureAcceptance) {

		searchSignatureForSignatureAcceptance = bitArray(0);

#ifdef HIGHOCCURENCEISGOODSIGNATURE

		for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);

			if (numberOfTracksWithPercentHitsInCell[i] > numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()])
				searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
			else if (numberOfTracksWithPercentHitsInCell[i] == numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()])
				if (actualSignatureForSignatureAcceptance.count() > searchSignatureForSignatureAcceptance.count())
					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;

		}

		addGoodPrelutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
		actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()];
		numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()]  = 0;

#else

		searchFoundForSignatureAcceptance = false;

		for (i = 0; i < (unsigned int)(1 << (unsigned int)trackSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);
		
			if (actualSignatureForSignatureAcceptance.count() == actualClassForSignatureAcceptance) {

				if (numberOfTracksWithPercentHitsInCell[i] > numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()]) {

					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
					searchFoundForSignatureAcceptance     = true;

				}

			}

		}

		if (searchFoundForSignatureAcceptance) {

			addGoodPrelutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
			actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()];
			numberOfTracksWithPercentHitsInCell[searchSignatureForSignatureAcceptance.toULong()]  = 0;

		}
		else
			actualClassForSignatureAcceptance--;

#endif

	}

	if (yPositions != NULL) {
		delete yPositions;
		yPositions = NULL;
	}
	if (zPositions != NULL) {
		delete zPositions;
		zPositions = NULL;
	}

	if (numberOfTracksWithPercentCorrectHitsInLayer != NULL) {
		delete[] numberOfTracksWithPercentCorrectHitsInLayer;
		numberOfTracksWithPercentCorrectHitsInLayer = NULL;
	}
	if (numberOfTracksWithPercentCorrectSlope != NULL) {
		delete[] numberOfTracksWithPercentCorrectSlope;
		numberOfTracksWithPercentCorrectSlope = NULL;
	}
	if (numberOfTracksWithPercentHitsInLayer != NULL) {
		delete[] numberOfTracksWithPercentHitsInLayer;
		numberOfTracksWithPercentHitsInLayer = NULL;
	}
	if (numberOfTracksWithPercentCorrectHitsInCell != NULL) {
		delete[] numberOfTracksWithPercentCorrectHitsInCell;
		numberOfTracksWithPercentCorrectHitsInCell = NULL;
	}
	if (numberOfTracksWithPercentHitsInCell != NULL) {
		delete[] numberOfTracksWithPercentHitsInCell;
		numberOfTracksWithPercentHitsInCell = NULL;
	}

}

/****************************************************************
 * method evaluates the lut goodness.							*
 ****************************************************************/

void analysis::evaluateLutGoodness(std::streambuf* terminal) {

	unsigned int   i;
	bitArray       maxSignature;
	unsigned int*  numberOfTracksWithPercentCorrectHits;
	unsigned int*  numberOfTracksWithSignature;
	unsigned int*  numberOfTracksWithPercentHits;
	double         distanceOfMaximumCellAndCorrectCell;
	std::string    terminalString;
	unsigned long  numberOfTracksWithGoodLutSignature;
	unsigned short actualNumberOfTracksForSignatureAcceptance;
	unsigned short minimalNumberOfTracksForSignatureAcceptance;
	bitArray       actualSignatureForSignatureAcceptance;
	bitArray       searchSignatureForSignatureAcceptance;

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	unsigned short actualClassForSignatureAcceptance;
	bool           searchFoundForSignatureAcceptance;

#endif

	numberOfTracksWithSignature  = new unsigned int[1 << (unsigned int)maxSignature.length()];
	if (numberOfTracksWithSignature == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentCorrectHits  = new unsigned int[1 << (unsigned int)maxSignature.length()];
	if (numberOfTracksWithPercentCorrectHits == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithPercentHits  = new unsigned int[1 << (unsigned int)maxSignature.length()];
	if (numberOfTracksWithPercentHits == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (i = 0; i < (unsigned int)(1 << (unsigned int)maxSignature.length()); i++) {

		numberOfTracksWithSignature[i]          = 0;
		numberOfTracksWithPercentCorrectHits[i] = 0;
		numberOfTracksWithPercentHits[i]        = 0;

	}

	distanceOfMaximumCellAndCorrectCell = 0;
	numberOfTracksWithGoodLutSignature  = 0;
	terminalString                      = "Analyse lut goodness:\t\t";

	evaluateLutDistribution(numberOfTracksWithSignature, numberOfTracksWithPercentCorrectHits, numberOfTracksWithPercentHits, &distanceOfMaximumCellAndCorrectCell, &numberOfTracksWithGoodLutSignature, terminalString, terminal);

	addNumberOfTracksWithGoodLutSignature(numberOfTracksWithGoodLutSignature);

	if (analysisResultWarnings & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		trackWithHitsInCorrectCellDistributionWarningMsg* trackWithHitsInCorrectCellDistribution = new trackWithHitsInCorrectCellDistributionWarningMsg(numberOfTracksWithPercentCorrectHits, (1 << (unsigned int)maxSignature.length()));
		trackWithHitsInCorrectCellDistribution->warningMsg();
		if(trackWithHitsInCorrectCellDistribution != NULL) {
			delete trackWithHitsInCorrectCellDistribution;
			trackWithHitsInCorrectCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHHITINCELLDISTRIBUTION) {

		trackWithHitsInCellDistributionWarningMsg* trackWithHitsInCellDistribution = new trackWithHitsInCellDistributionWarningMsg(numberOfTracksWithPercentHits, (1 << (unsigned int)maxSignature.length()));
		trackWithHitsInCellDistribution->warningMsg();
		if(trackWithHitsInCellDistribution != NULL) {
			delete trackWithHitsInCellDistribution;
			trackWithHitsInCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELL) {

		middleDistanceOfMaximumAndCorrectCellWarningMsg* middleDistanceOfMaximumAndCorrectCell = new middleDistanceOfMaximumAndCorrectCellWarningMsg(distanceOfMaximumCellAndCorrectCell);
		middleDistanceOfMaximumAndCorrectCell->warningMsg();
		if(middleDistanceOfMaximumAndCorrectCell != NULL) {
			delete middleDistanceOfMaximumAndCorrectCell;
			middleDistanceOfMaximumAndCorrectCell = NULL;
		}

	}

	if (analysisResultWarnings & TRACKWITHSIGNATUREDISTRIBUTION) {

		trackWithSignatureDistributionWarningMsg* trackWithSignatureDistribution = new trackWithSignatureDistributionWarningMsg(numberOfTracksWithSignature, (1 << (unsigned int)maxSignature.length()));
		trackWithSignatureDistribution->warningMsg();
		if(trackWithSignatureDistribution != NULL) {
			delete trackWithSignatureDistribution;
			trackWithSignatureDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHHITINCORRECTCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addCorrectLutCellDistribution(numberOfTracksWithPercentCorrectHits, (1 << (unsigned int)maxSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHHITINCELLDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addLutCellDistribution(numberOfTracksWithPercentHits, (1 << (unsigned int)maxSignature.length()));

	}

	if (analysisResultDisplays & TRACKWITHSIGNATUREDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addSignatureDistribution(numberOfTracksWithSignature, (1 << (unsigned int)maxSignature.length()));

	}

	actualNumberOfTracksForSignatureAcceptance  = 0;
	minimalNumberOfTracksForSignatureAcceptance = 0;
	for (i = 0; i < (unsigned int)(1 << (unsigned int)maxSignature.length()); i++)
		minimalNumberOfTracksForSignatureAcceptance += numberOfTracksWithPercentHits[i];
	minimalNumberOfTracksForSignatureAcceptance = (unsigned short)(((double)percentageOfTracksForSignature / (double)100) * (double)minimalNumberOfTracksForSignatureAcceptance);

#ifndef HIGHOCCURENCEISGOODSIGNATURE

	actualClassForSignatureAcceptance = (unsigned short)maxSignature.length();

#endif

	while (minimalNumberOfTracksForSignatureAcceptance > actualNumberOfTracksForSignatureAcceptance) {

		searchSignatureForSignatureAcceptance = bitArray(0);

#ifdef HIGHOCCURENCEISGOODSIGNATURE

		for (i = 0; i < (unsigned int)(1 << (unsigned int)maxSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);

			if (numberOfTracksWithPercentHits[i] > numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()])
				searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
			else if (numberOfTracksWithPercentHits[i] == numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()])
				if (actualSignatureForSignatureAcceptance.count() > searchSignatureForSignatureAcceptance.count())
					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;

		}

		addGoodLutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
		actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()];
		numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()]  = 0;

#else

		searchFoundForSignatureAcceptance = false;

		for (i = 0; i < (1 << (unsigned int)maxSignature.length()); i++) {

			actualSignatureForSignatureAcceptance = bitArray(i);
		
			if (actualSignatureForSignatureAcceptance.count() == actualClassForSignatureAcceptance) {

				if (numberOfTracksWithPercentHits[i] > numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()]) {

					searchSignatureForSignatureAcceptance = actualSignatureForSignatureAcceptance;
					searchFoundForSignatureAcceptance     = true;

				}

			}

		}

		if (searchFoundForSignatureAcceptance) {

			addGoodLutSignature(searchSignatureForSignatureAcceptance, bitArray(1));
			actualNumberOfTracksForSignatureAcceptance                                           += numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()];
			numberOfTracksWithPercentHits[searchSignatureForSignatureAcceptance.toULong()]  = 0;

		}
		else
			actualClassForSignatureAcceptance--;

#endif

	}

	if (numberOfTracksWithSignature != NULL) {
		delete[] numberOfTracksWithSignature;
		numberOfTracksWithSignature = NULL;
	}
	if (numberOfTracksWithPercentCorrectHits != NULL) {
		delete[] numberOfTracksWithPercentCorrectHits;
		numberOfTracksWithPercentCorrectHits = NULL;
	}
	if (numberOfTracksWithPercentHits != NULL) {
		delete[] numberOfTracksWithPercentHits;
		numberOfTracksWithPercentHits = NULL;
	}

}

/****************************************************************
 * method evaluates the lut distribution.						*
 ****************************************************************/

void analysis::evaluateLutDistribution(unsigned int* numberOfTracksWithSignature, unsigned int* numberOfTracksWithPercentCorrectHits, unsigned int* numberOfTracksWithPercentHits, double* distanceOfMaximumCellAndCorrectCell, unsigned long* numberOfTracksWithGoodLutSignature, std::string terminalString, std::streambuf* terminal) {

	terminalSequence         statusSequenceForLut;
	std::list<findableTrack> findableTracks;
	trackfinderInputTrack*   track;
	trackfinderInputHit*     hit;
	lutHoughBorder           secondBorder;
	unsigned short           stationIndex;
	houghBorderPosition      actualPosition;
	bitArray                 trackSignature;
	bitArray                 correctCellSignature;
	histogramCellSignatures  cellSignatures;
	histogramCellSignature   bestCellSignature;
	unsigned short           numberOfCorrectSignaturePositions;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError(ANALYSISLIB);

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForLut, terminal, terminalString, (unsigned int)findableTracks.size());
	terminalInitialize(statusSequenceForLut);

	cellSignatures.reset();

	/* loop to evaluate the prelut goodness for all findable tracks */
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		track->resetHitPointer();

		correctCellSignature = bitArray(0);
		trackSignature       = bitArray(0);
		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (unsigned short i = 0; i < track->getNumberOfHits(); i++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			trackSignature[stationIndex] = true;

			/* compute the borders for this hit */
			(*luts)->evaluateLut(hit, &secondBorder);

			secondBorder.houghCoord.resetActiveObject();
			for (unsigned short j = 0; j < secondBorder.houghCoord.getNumberOfEntries(); j++) {

				actualPosition = secondBorder.houghCoord.readActiveObjectAndMakeNextOneActive();

				/* Check if the first and the second dimension border is in the range */
				if ((actualPosition.pos1 == actualTrack->getPosition().get(DIM1)) && (actualPosition.pos2 == actualTrack->getPosition().get(DIM2)))
					correctCellSignature[stationIndex] = true;

				/* Accumulate the frequency of the border */
				cellSignatures.add(actualPosition, stationIndex);

			}

		}

		if (numberOfTracksWithSignature != NULL)
			numberOfTracksWithSignature[trackSignature.toULong()]++;

		if (numberOfTracksWithPercentCorrectHits != NULL)
			numberOfTracksWithPercentCorrectHits[correctCellSignature.toULong()]++;

		bestCellSignature = cellSignatures.getBest(actualTrack->getPosition());

		if (numberOfTracksWithPercentHits != NULL)
			numberOfTracksWithPercentHits[bestCellSignature.value.toULong()]++;

		if (distanceOfMaximumCellAndCorrectCell != NULL)
			(*distanceOfMaximumCellAndCorrectCell) += (bestCellSignature.distance / (double)findableTracks.size());

		numberOfCorrectSignaturePositions = 0;
		for (unsigned short k = 0; k < trackSignature.length(); k++) {

			if (trackSignature[k] && (trackSignature[k] == bestCellSignature.value[k]))
				numberOfCorrectSignaturePositions++;

		}

		if (numberOfCorrectSignaturePositions >= (unsigned short)ceil(((double)percentageOfHitsInSignature / (double)100) * (double)trackSignature.count()))
			if (numberOfTracksWithGoodLutSignature != NULL)
				(*numberOfTracksWithGoodLutSignature)++;

		cellSignatures.reset();
		terminalOverwriteWithIncrement(statusSequenceForLut);

	}

	terminalFinalize(statusSequenceForLut);

}

/****************************************************************
 * method evaluates the goodness of both luts.					*
 ****************************************************************/

void analysis::evaluateBothLutsDistribution(unsigned int* numberOfTracksWithHits, std::string terminalString, std::streambuf* terminal) {

	terminalSequence         statusSequenceForBothLuts;
	std::list<findableTrack> findableTracks;
	trackfinderInputTrack*   track;
	histogramCellSignature   bestCellSignature;

	if (numberOfTracksWithHits == NULL)
		throw cannotAccessDistributionMemoryError(ANALYSISLIB);

	/* this function creates a std::list which includes pointers to all
	 * findable tracks and their corresponding position in the Hough
	 * space */
	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForBothLuts, terminal, terminalString, (unsigned int)findableTracks.size());
	terminalInitialize(statusSequenceForBothLuts);

	/* loop to evaluate the prelut goodness for all findable tracks */
	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		bestCellSignature = transformPreview(*track, actualTrack->getPosition());

		numberOfTracksWithHits[bestCellSignature.value.toULong()]++;

		terminalOverwriteWithIncrement(statusSequenceForBothLuts);

	}

	terminalFinalize(statusSequenceForBothLuts);

}

/****************************************************************
 * method evaluates the magnetfield.							*
 ****************************************************************/

void analysis::evaluateMagnetfield(std::streambuf* terminal) {

	trackfinderInputMagneticField* magneticField;
	terminalSequence               statusSequenceForMagnetfield;
	unsigned int                   totalNumberOfBins;
	unsigned int                   i;
	magnetfieldDisplayInfo*        actualDisplayInfo;
	double                         actualPositionIncrement;
	magneticFieldValue             actualFieldValue;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (magnetfieldAnalyser == NULL)
		throw cannotAccessMagnetfieldAnalyserError();

	magneticField = (*eventData)->getMagneticField();
	if (magneticField == NULL)
		throw magneticFieldNotFoundError();

	initializeMagnetfieldEvaluation();

	totalNumberOfBins = 0;
	if (magnetfieldAnalyser->isMagnetfieldXBxAnalysisEnabled())
		if (magnetfieldAnalyser->getXBxDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getXBxDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldXByAnalysisEnabled())
		if (magnetfieldAnalyser->getXByDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getXByDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldXBzAnalysisEnabled())
		if (magnetfieldAnalyser->getXBzDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getXBzDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldYBxAnalysisEnabled())
		if (magnetfieldAnalyser->getYBxDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getYBxDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldYByAnalysisEnabled())
		if (magnetfieldAnalyser->getYByDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getYByDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldYBzAnalysisEnabled())
		if (magnetfieldAnalyser->getYBzDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getYBzDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldZBxAnalysisEnabled())
		if (magnetfieldAnalyser->getZBxDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getZBxDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldZByAnalysisEnabled())
		if (magnetfieldAnalyser->getZByDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getZByDisplayEntries();
	if (magnetfieldAnalyser->isMagnetfieldZBzAnalysisEnabled())
		if (magnetfieldAnalyser->getZBzDisplayAxiRange() != NULL)
			totalNumberOfBins += (unsigned int)magnetfieldAnalyser->getZBzDisplayEntries();

	createTerminalStatusSequence(&statusSequenceForMagnetfield, terminal, "Analyse magnetfield:\t\t", (unsigned int)totalNumberOfBins);
	terminalInitialize(statusSequenceForMagnetfield);

	if (magnetfieldAnalyser->isMagnetfieldXBxAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getXBxDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getXBxDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getXBxDisplayEntries(); i++) {

			magneticField->getFieldValues(i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setXBxPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldX());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldXByAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getXByDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getXByDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getXByDisplayEntries(); i++) {

			magneticField->getFieldValues(i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setXByPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldY());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldXBzAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getXBzDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getXBzDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getXBzDisplayEntries(); i++) {

			magneticField->getFieldValues(i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setXBzPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldZ());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldYBxAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getYBxDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getYBxDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getYBxDisplayEntries(); i++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, i * actualPositionIncrement + actualDisplayInfo->min,  actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setYBxPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldX());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldYByAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getYByDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getYByDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getYByDisplayEntries(); i++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setYByPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldY());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldYBzAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getYBzDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getYBzDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getYBzDisplayEntries(); i++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, i * actualPositionIncrement + actualDisplayInfo->min, actualDisplayInfo->constValDim2, &actualFieldValue);
			magnetfieldAnalyser->setYBzPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldZ());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldZBxAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getZBxDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getZBxDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getZBxDisplayEntries(); i++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, i * actualPositionIncrement + actualDisplayInfo->min, &actualFieldValue);
			magnetfieldAnalyser->setZBxPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldX());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldZByAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getZByDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getZByDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getZByDisplayEntries(); i++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, i * actualPositionIncrement + actualDisplayInfo->min, &actualFieldValue);
			magnetfieldAnalyser->setZByPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldY());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}
	if (magnetfieldAnalyser->isMagnetfieldZBzAnalysisEnabled()) {

		actualDisplayInfo = magnetfieldAnalyser->getZBzDisplayAxiRange();
		if (actualDisplayInfo == NULL)
			throw cannotAccessDisplayInfoError();

		actualPositionIncrement = (actualDisplayInfo->max - actualDisplayInfo->min) / magnetfieldAnalyser->getZBzDisplayEntries();
		for (i = 0; i < magnetfieldAnalyser->getZBzDisplayEntries(); i++) {

			magneticField->getFieldValues(actualDisplayInfo->constValDim1, actualDisplayInfo->constValDim2, i * actualPositionIncrement + actualDisplayInfo->min, &actualFieldValue);
			magnetfieldAnalyser->setZBzPoint(i, i * actualPositionIncrement + actualDisplayInfo->min, actualFieldValue.getFieldZ());

			terminalOverwriteWithIncrement(statusSequenceForMagnetfield);

		}

	}


	terminalFinalize(statusSequenceForMagnetfield);

	finalizeMagnetfieldEvaluation();

}

/****************************************************************
 * method compares the magnetfield with the factors.			*
 ****************************************************************/

void analysis::evaluateMagnetfieldVSFactors(double constXPos, double constYPos, bool median) {

	trackfinderInputMagneticField* magneticField;
	double                         actualZPos;
	double*                        positions;
	double*                        factors;
	magneticFieldValue*            fieldValues;
	magneticFieldValue*            integratedFieldValues;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	magneticField = (*eventData)->getMagneticField();
	if (magneticField == NULL)
		throw magneticFieldNotFoundForComparissonToFactorsError();

	positions = new double[(*eventData)->getNumberOfActiveStations()];
	if (positions == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	factors = new double[(*eventData)->getNumberOfActiveStations()];
	if (factors == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	fieldValues = new magneticFieldValue[(*eventData)->getNumberOfActiveStations()];
	if (fieldValues == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	integratedFieldValues = new magneticFieldValue[(*eventData)->getNumberOfActiveStations()];
	if (integratedFieldValues == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	for (unsigned short i = 0; i < (*eventData)->getNumberOfActiveStations(); i++) {

		actualZPos   = (*eventData)->getDetector().getStationByIndex(i).getDistance();
		positions[i] = actualZPos;
		factors[i]   = magneticField->evaluateMagnetfieldFactor(i);
		magneticField->getFieldValues(constXPos, constYPos, actualZPos, &fieldValues[i]);
		magneticField->getIntegratedFieldValues(constXPos, constYPos, actualZPos, i, &integratedFieldValues[i], median);

	}


	initializeShowAnalysis();

	showAnalyser->addMagnetfieldFactorsZ(positions, factors, (*eventData)->getNumberOfActiveStations());
	showAnalyser->addMagnetfieldValuesZ(positions, fieldValues, (*eventData)->getNumberOfActiveStations());
	showAnalyser->addIntegratedMagnetfieldValuesZ(positions, integratedFieldValues, (*eventData)->getNumberOfActiveStations());

	if (positions != NULL) {
		delete [] positions;
		positions = NULL;
	}

	if (factors != NULL) {
		delete [] factors;
		factors = NULL;
	}

	if (fieldValues != NULL) {
		delete [] fieldValues;
		fieldValues = NULL;
	}

	if (integratedFieldValues != NULL) {
		delete [] integratedFieldValues;
		integratedFieldValues = NULL;
	}

}

/****************************************************************
 * method evaluates the goodness of the used analytic formula.	*
 ****************************************************************/

void analysis::evaluateHoughTransformGoodness(std::streambuf* terminal) {

	unsigned int*            numberOfTracksWithMomentaError;
	unsigned short           i;
	unsigned short           j;
	std::list<findableTrack> findableTracks;
	terminalSequence         statusSequenceForFormula;
	trackfinderInputTrack*   track;
	trackfinderInputHit*     hit;
	lutBorder                border;
	prelutHoughBorder*       firstBorder;
	lutHoughBorder*          secondBorder;
	unsigned short           stationIndex;
	houghBorderPosition      actualPosition;
	histogramCellSignatures  cellSignatures;
	bitArray                 minimumClass;
	histogramCellSignature   bestCellSignature;
	trackMomentum            momentum;
	analyticFormula          formula;
	double                   absoluteTrackMomentum;
	double                   absoluteHitMomentum;
	double                   relativeErrorMomentum;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError(ANALYSISLIB);

	numberOfTracksWithMomentaError  = new unsigned int[NUMBEROFDIFFERENTMOMENTAS];
	if (numberOfTracksWithMomentaError == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	firstBorder  = border.getPrelutHoughBorderPointer();
	secondBorder = border.getLutHoughBorderPointer();

	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForFormula, terminal, "Analyse formula goodness:\t\t\t", (unsigned int)findableTracks.size());
	terminalInitialize(statusSequenceForFormula);

	cellSignatures.reset();
	minimumClass = bitArray(0);
	for (i = 0; i < NUMBEROFDIFFERENTMOMENTAS; i++)
		numberOfTracksWithMomentaError[i]  = 0;

	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		momentum.set(track->getMomX(), PX);
		momentum.set(track->getMomY(), PY);
		momentum.set(track->getMomZ(), PZ);

		absoluteTrackMomentum = formula.evaluateAbsP(momentum);

		track->resetHitPointer();

		/* loop to evaluate the magnetfield factor for all hits of this track */
		for (i = 0; i < track->getNumberOfHits(); i++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() == NULL)
				throw cannotAccessStationError(ANALYSISLIB);
			else
				stationIndex = hit->getStation()->getIndex();

			/* compute the borders for this hit */
			(*luts)->evaluate(hit, &border);

			secondBorder->houghCoord.resetActiveObject();
			for (j = 0; j < secondBorder->houghCoord.getNumberOfEntries(); j++) {

				actualPosition = secondBorder->houghCoord.readActiveObjectAndMakeNextOneActive();

				for(unsigned short k = firstBorder->start; k <= firstBorder->stop; k++) {

					/* Accumulate the frequency of the border */
					cellSignatures.add(actualPosition, k, stationIndex);

				}

			}

		}

		bestCellSignature = cellSignatures.getBest(actualTrack->getPosition());

		if (evaluateTrackPatternCoding(bestCellSignature.value) > minimumClass) {

			formula.evaluateP(bestCellSignature.pos, *(*space), &momentum, NULL);

			absoluteHitMomentum = formula.evaluateAbsP(momentum);

			if (absoluteHitMomentum < absoluteTrackMomentum)
				relativeErrorMomentum = (1 - (absoluteHitMomentum / absoluteTrackMomentum));
			else if (absoluteHitMomentum < 2 * absoluteTrackMomentum)
				relativeErrorMomentum = ((absoluteHitMomentum / absoluteTrackMomentum) - 1);
			else
				relativeErrorMomentum = 1;

			numberOfTracksWithMomentaError[(unsigned short)(relativeErrorMomentum * (NUMBEROFDIFFERENTMOMENTAS - 1) + 0.5)]++;

		}

		cellSignatures.reset();
		terminalOverwriteWithIncrement(statusSequenceForFormula);

	}

	terminalFinalize(statusSequenceForFormula);

	if (analysisResultWarnings & TRACKWITHMOMENTAERRORDISTRIBUTION) {

		trackWithMomentaErrorDistributionWarningMsg* trackWithMomentaErrorDistribution = new trackWithMomentaErrorDistributionWarningMsg(numberOfTracksWithMomentaError, NUMBEROFDIFFERENTMOMENTAS);
		trackWithMomentaErrorDistribution->warningMsg();
		if(trackWithMomentaErrorDistribution != NULL) {
			delete trackWithMomentaErrorDistribution;
			trackWithMomentaErrorDistribution = NULL;
		}

	}

	if (analysisResultDisplays & TRACKWITHMOMENTAERRORDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addMomentaErrorDistribution(numberOfTracksWithMomentaError, NUMBEROFDIFFERENTMOMENTAS);

	}

	if (numberOfTracksWithMomentaError != NULL) {
		delete[] numberOfTracksWithMomentaError;
		numberOfTracksWithMomentaError = NULL;
	}

}

/****************************************************************
 * method evaluates the goodness of the used quatization.		*
 ****************************************************************/

void analysis::evaluateQuantizationGoodness(std::streambuf* terminal) {

	unsigned int*            numberOfTracksWithMomentumX;
	unsigned int*            numberOfTracksWithMomentumY;
	unsigned int*            numberOfTracksWithMomentumZ;
	unsigned int*            numberOfTracksWithHoughspaceDim1;
	unsigned int*            numberOfTracksWithHoughspaceDim2;
	unsigned int*            numberOfTracksWithHoughspaceDim3;
	unsigned int*            numberOfTracksWithMomentum;
	unsigned int*            numberOfTracksWithHoughspace;
	double                   momentumXMin;
	double                   momentumXMax;
	double                   momentumXIncr;
	double                   momentumYMin;
	double                   momentumYMax;
	double                   momentumYIncr;
	double                   momentumZMin;
	double                   momentumZMax;
	double                   momentumZIncr;
	unsigned short           i;
	unsigned int             j;
	std::list<findableTrack> findableTracks;
	terminalSequence         statusSequenceForQuantization;
	trackfinderInputTrack*   track;
	double                   actualMomentum;
	unsigned short           actualPosition1;
	unsigned short           actualPosition2;
	unsigned short           actualPosition3;
	unsigned long            numberOfNotFindableTracks;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	numberOfTracksWithMomentumX      = new unsigned int[(*space)->getStep(HTHETA)];
	if (numberOfTracksWithMomentumX == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithMomentumY      = new unsigned int[(*space)->getStep(HGAMMA)];
	if (numberOfTracksWithMomentumY == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithMomentumZ      = new unsigned int[(*space)->getStep(HRADIUS)];
	if (numberOfTracksWithMomentumZ == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithHoughspaceDim1 = new unsigned int[(*space)->getStep(DIM1)];
	if (numberOfTracksWithHoughspaceDim1 == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithHoughspaceDim2 = new unsigned int[(*space)->getStep(DIM2)];
	if (numberOfTracksWithHoughspaceDim2 == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithHoughspaceDim3 = new unsigned int[(*space)->getStep(DIM3)];
	if (numberOfTracksWithHoughspaceDim3 == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	numberOfTracksWithMomentum       = new unsigned int[(*space)->getStep(HTHETA) * (*space)->getStep(HGAMMA) * (*space)->getStep(HRADIUS)];
	if (numberOfTracksWithMomentum == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	numberOfTracksWithHoughspace     = new unsigned int[(*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3)];
	if (numberOfTracksWithHoughspace == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	momentumZMin  = 1 / (*space)->getMax(HRADIUS);
	momentumZMax  = 1 / (*space)->getIncr(HRADIUS);
	momentumZIncr = (momentumZMax - momentumZMin) / ((*space)->getStep(HRADIUS) - 1);
	momentumYMin  = tan((*space)->getMin(HGAMMA)) * momentumZMax;
	momentumYMax  = tan((*space)->getMax(HGAMMA)) * momentumZMax;
	momentumYIncr = (momentumYMax - momentumYMin) / ((*space)->getStep(HGAMMA) - 1);
	momentumXMin  = tan((*space)->getMin(HTHETA)) * momentumZMax;
	momentumXMax  = tan((*space)->getMax(HTHETA)) * momentumZMax;
	momentumXIncr = (momentumXMax - momentumXMin) / ((*space)->getStep(HTHETA) - 1);

	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForQuantization, terminal, "Analyse quantization goodness:\t\t\t", (unsigned int)findableTracks.size());
	terminalInitialize(statusSequenceForQuantization);

	for (i = 0; i < (*space)->getStep(HTHETA); i++)
		numberOfTracksWithMomentumX[i]       = 0;
	for (i = 0; i < (*space)->getStep(HGAMMA); i++)
		numberOfTracksWithMomentumY[i]       = 0;
	for (i = 0; i < (*space)->getStep(HRADIUS); i++)
		numberOfTracksWithMomentumZ[i]       = 0;
	for (i = 0; i < (*space)->getStep(DIM1); i++)
		numberOfTracksWithHoughspaceDim1[i]  = 0;
	for (i = 0; i < (*space)->getStep(DIM2); i++)
		numberOfTracksWithHoughspaceDim2[i]  = 0;
	for (i = 0; i < (*space)->getStep(DIM3); i++)
		numberOfTracksWithHoughspaceDim3[i]  = 0;

	for (j = 0; j < (unsigned int)(*space)->getStep(HTHETA) * (unsigned int)(*space)->getStep(HGAMMA) * (unsigned int)(*space)->getStep(HRADIUS); j++)
		numberOfTracksWithMomentum[j]        = 0;
	for (j = 0; j < (unsigned int)(*space)->getStep(DIM1) * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3); j++)
		numberOfTracksWithHoughspace[j]      = 0;

	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end(); actualTrack++) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		actualMomentum  = track->getMomX();
		if (actualMomentum <  momentumXMin)
			actualMomentum = momentumXMin;
		if (actualMomentum >  momentumXMax)
			actualMomentum = momentumXMax;
		actualPosition1 = (unsigned short)(((actualMomentum - momentumXMin) / momentumXIncr) + 0.5);
		numberOfTracksWithMomentumX[actualPosition1]++;
		actualMomentum  = track->getMomY();
		if (actualMomentum <  momentumYMin)
			actualMomentum = momentumYMin;
		if (actualMomentum >  momentumYMax)
			actualMomentum = momentumYMax;
		actualPosition2 = (unsigned short)(((actualMomentum - momentumYMin) / momentumYIncr) + 0.5);
		numberOfTracksWithMomentumY[actualPosition2]++;
		actualMomentum  = track->getMomZ();
		if (actualMomentum <  momentumZMin)
			actualMomentum = momentumZMin;
		if (actualMomentum >  momentumZMax)
			actualMomentum = momentumZMax;
		actualPosition3 = (unsigned short)(((actualMomentum - momentumZMin) / momentumZIncr) + 0.5);
		numberOfTracksWithMomentumZ[actualPosition3]++;

		if ((actualPosition1 < (*space)->getStep(HTHETA)) && (actualPosition2 < (*space)->getStep(HGAMMA)) && (actualPosition3 < (*space)->getStep(HRADIUS)))
			numberOfTracksWithMomentum[(unsigned int)actualPosition1 * (unsigned int)(*space)->getStep(HGAMMA) * (unsigned int)(*space)->getStep(HRADIUS) + (unsigned int)actualPosition2 * (unsigned int)(*space)->getStep(HRADIUS) + (unsigned int)actualPosition3]++;

		actualPosition1 = actualTrack->getPosition().get(DIM1);
		if (actualPosition1 < (*space)->getStep(DIM1))
			numberOfTracksWithHoughspaceDim1[actualPosition1]++;
		actualPosition2 = actualTrack->getPosition().get(DIM2);
		if (actualPosition2 < (*space)->getStep(DIM2))
			numberOfTracksWithHoughspaceDim2[actualPosition2]++;
		actualPosition3 = actualTrack->getPosition().get(DIM3);
		if (actualPosition3 < (*space)->getStep(DIM3))
			numberOfTracksWithHoughspaceDim3[actualPosition3]++;

		if ((actualPosition1 < (*space)->getStep(DIM1)) && (actualPosition2 < (*space)->getStep(DIM2)) && (actualPosition3 < (*space)->getStep(DIM3)))
			numberOfTracksWithHoughspace[(unsigned int)actualPosition1 * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3) + (unsigned int)actualPosition2 * (unsigned int)(*space)->getStep(DIM3) + (unsigned int)actualPosition3]++;

		terminalOverwriteWithIncrement(statusSequenceForQuantization);

	}

	numberOfNotFindableTracks = 0;
	for (j = 0; j < (unsigned int)(*space)->getStep(DIM1) * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3); j++)
		if (numberOfTracksWithHoughspace[j] > 1)
			numberOfNotFindableTracks += (numberOfTracksWithHoughspace[j] - 1);

	terminalFinalize(statusSequenceForQuantization);

	if (analysisResultWarnings & QUANTIZEDMOMENTADIMDISTRIBUTION) {

		quantizedMomentaXDistributionWarningMsg* quantizedMomentaXDistribution = new quantizedMomentaXDistributionWarningMsg(numberOfTracksWithMomentumX, (*space)->getStep(HTHETA), momentumXMin, momentumXMax);
		quantizedMomentaXDistribution->warningMsg();
		if(quantizedMomentaXDistribution != NULL) {
			delete quantizedMomentaXDistribution;
			quantizedMomentaXDistribution = NULL;
		}
		quantizedMomentaYDistributionWarningMsg* quantizedMomentaYDistribution = new quantizedMomentaYDistributionWarningMsg(numberOfTracksWithMomentumY, (*space)->getStep(HGAMMA), momentumYMin, momentumYMax);
		quantizedMomentaYDistribution->warningMsg();
		if(quantizedMomentaYDistribution != NULL) {
			delete quantizedMomentaYDistribution;
			quantizedMomentaYDistribution = NULL;
		}
		quantizedMomentaZDistributionWarningMsg* quantizedMomentaZDistribution = new quantizedMomentaZDistributionWarningMsg(numberOfTracksWithMomentumZ, (*space)->getStep(HRADIUS), momentumZMin, momentumZMax);
		quantizedMomentaZDistribution->warningMsg();
		if(quantizedMomentaZDistribution != NULL) {
			delete quantizedMomentaZDistribution;
			quantizedMomentaZDistribution = NULL;
		}

	}
	if (analysisResultWarnings & QUANTIZEDHOUGHSPACEDIMDISTRIBUTION) {

		quantizedHoughspaceDim1DistributionWarningMsg* quantizedHoughspaceDim1Distribution = new quantizedHoughspaceDim1DistributionWarningMsg(numberOfTracksWithHoughspaceDim1, (*space)->getStep(DIM1));
		quantizedHoughspaceDim1Distribution->warningMsg();
		if(quantizedHoughspaceDim1Distribution != NULL) {
			delete quantizedHoughspaceDim1Distribution;
			quantizedHoughspaceDim1Distribution = NULL;
		}
		quantizedHoughspaceDim2DistributionWarningMsg* quantizedHoughspaceDim2Distribution = new quantizedHoughspaceDim2DistributionWarningMsg(numberOfTracksWithHoughspaceDim2, (*space)->getStep(DIM2));
		quantizedHoughspaceDim2Distribution->warningMsg();
		if(quantizedHoughspaceDim2Distribution != NULL) {
			delete quantizedHoughspaceDim2Distribution;
			quantizedHoughspaceDim2Distribution = NULL;
		}
		quantizedHoughspaceDim3DistributionWarningMsg* quantizedHoughspaceDim3Distribution = new quantizedHoughspaceDim3DistributionWarningMsg(numberOfTracksWithHoughspaceDim3, (*space)->getStep(DIM3));
		quantizedHoughspaceDim3Distribution->warningMsg();
		if(quantizedHoughspaceDim3Distribution != NULL) {
			delete quantizedHoughspaceDim3Distribution;
			quantizedHoughspaceDim3Distribution = NULL;
		}

	}

	if (analysisResultWarnings & QUANTIZEDMOMENTADISTRIBUTION) {

		quantizedMomentaDistributionWarningMsg* quantizedMomentaDistribution = new quantizedMomentaDistributionWarningMsg(numberOfTracksWithMomentum, (*space)->getStep(HTHETA) * (*space)->getStep(HGAMMA) * (*space)->getStep(HRADIUS));
		quantizedMomentaDistribution->warningMsg();
		if(quantizedMomentaDistribution != NULL) {
			delete quantizedMomentaDistribution;
			quantizedMomentaDistribution = NULL;
		}

	}

	if (analysisResultWarnings & QUANTIZEDHOUGHSPACEDISTRIBUTION) {

		quantizedHoughspaceDistributionWarningMsg* quantizedHoughspaceDistribution = new quantizedHoughspaceDistributionWarningMsg(numberOfTracksWithHoughspace, (*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3));
		quantizedHoughspaceDistribution->warningMsg();
		if(quantizedHoughspaceDistribution != NULL) {
			delete quantizedHoughspaceDistribution;
			quantizedHoughspaceDistribution = NULL;
		}

	}

	if (analysisResultDisplays & QUANTIZEDMOMENTADIMDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addMomentaXDistribution(numberOfTracksWithMomentumX, (*space)->getStep(HTHETA), momentumXMin, momentumXMax);
		showAnalyser->addMomentaYDistribution(numberOfTracksWithMomentumY, (*space)->getStep(HGAMMA), momentumYMin, momentumYMax);
		showAnalyser->addMomentaZDistribution(numberOfTracksWithMomentumZ, (*space)->getStep(HRADIUS), momentumZMin, momentumZMax);

	}

	if (analysisResultDisplays & QUANTIZEDHOUGHSPACEDIMDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addHoughspaceDim1Distribution(numberOfTracksWithHoughspaceDim1, (*space)->getStep(DIM1));
		showAnalyser->addHoughspaceDim2Distribution(numberOfTracksWithHoughspaceDim2, (*space)->getStep(DIM2));
		showAnalyser->addHoughspaceDim3Distribution(numberOfTracksWithHoughspaceDim3, (*space)->getStep(DIM3));

	}

	if (analysisResultDisplays & QUANTIZEDMOMENTADISTRIBUTION) {

		if (analysisResultDisplays & QUANTIZEDHOUGHSPACEDISTRIBUTION) {

			youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg* youCannotEnableTrackWithSameMomentaAndCoordinateDistribution = new youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg();
			youCannotEnableTrackWithSameMomentaAndCoordinateDistribution->warningMsg();
			if(youCannotEnableTrackWithSameMomentaAndCoordinateDistribution != NULL) {
				delete youCannotEnableTrackWithSameMomentaAndCoordinateDistribution;
				youCannotEnableTrackWithSameMomentaAndCoordinateDistribution = NULL;
			}

		}
		else {

			initializeShowAnalysis();

			showAnalyser->addMomentaDistribution(numberOfTracksWithMomentum, (unsigned int)(*space)->getStep(HTHETA) * (unsigned int)(*space)->getStep(HGAMMA) * (unsigned int)(*space)->getStep(HRADIUS));

		}

	}
	else {

		if (analysisResultDisplays & QUANTIZEDHOUGHSPACEDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addCoordinateDistribution(numberOfTracksWithHoughspace, (unsigned int)(*space)->getStep(DIM1) * (unsigned int)(*space)->getStep(DIM2) * (unsigned int)(*space)->getStep(DIM3));

		}

	}

	if (analysisResultWarnings & SAMEHOUGHSPACECELLDISTRIBUTION) {

		sameHoughspaceCellDistributionWarningMsg* sameHoughspaceCellDistribution = new sameHoughspaceCellDistributionWarningMsg(numberOfTracksWithHoughspace, (*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3));
		sameHoughspaceCellDistribution->warningMsg();
		if(sameHoughspaceCellDistribution != NULL) {
			delete sameHoughspaceCellDistribution;
			sameHoughspaceCellDistribution = NULL;
		}

	}

	if (analysisResultWarnings & NUMBEROFTRACKSWHICHCANNOTBEFOUND) {

		numberOfTracksWhichCannotBeFoundWarningMsg* numberOfTracksWhichCannotBeFound = new numberOfTracksWhichCannotBeFoundWarningMsg(numberOfNotFindableTracks);
		numberOfTracksWhichCannotBeFound->warningMsg();
		if(numberOfTracksWhichCannotBeFound != NULL) {
			delete numberOfTracksWhichCannotBeFound;
			numberOfTracksWhichCannotBeFound = NULL;
		}

	}

	addNumberOfTracksWhichCannotBeFound(numberOfNotFindableTracks);

	if (numberOfTracksWithMomentumX != NULL) {
		delete[] numberOfTracksWithMomentumX;
		numberOfTracksWithMomentumX = NULL;
	}
	if (numberOfTracksWithMomentumY != NULL) {
		delete[] numberOfTracksWithMomentumY;
		numberOfTracksWithMomentumY = NULL;
	}
	if (numberOfTracksWithMomentumZ != NULL) {
		delete[] numberOfTracksWithMomentumZ;
		numberOfTracksWithMomentumZ = NULL;
	}
	if (numberOfTracksWithHoughspaceDim1 != NULL) {
		delete[] numberOfTracksWithHoughspaceDim1;
		numberOfTracksWithHoughspaceDim1 = NULL;
	}
	if (numberOfTracksWithHoughspaceDim2 != NULL) {
		delete[] numberOfTracksWithHoughspaceDim2;
		numberOfTracksWithHoughspaceDim2 = NULL;
	}
	if (numberOfTracksWithHoughspaceDim3 != NULL) {
		delete[] numberOfTracksWithHoughspaceDim3;
		numberOfTracksWithHoughspaceDim3 = NULL;
	}

	if (numberOfTracksWithMomentum != NULL) {
		delete[] numberOfTracksWithMomentum;
		numberOfTracksWithMomentum = NULL;
	}
	if (numberOfTracksWithHoughspace != NULL) {
		delete[] numberOfTracksWithHoughspace;
		numberOfTracksWithHoughspace = NULL;
	}

}

/****************************************************************
 * method evaluates the goodness of the used quatization.		*
 ****************************************************************/

void analysis::evaluatePeakDistanceGoodness(std::streambuf* terminal) {

	unsigned int*            dim1PeakDistanceDistribution;
	unsigned int*            dim2PeakDistanceDistribution;
	unsigned int*            dim3PeakDistanceDistribution;
	unsigned int*            accumulatedPeakDistanceDistribution;
	std::list<findableTrack> findableTracks;
	unsigned int             i;
	terminalSequence         statusSequenceForPeakDistance;
	trackfinderInputTrack*   track;
	unsigned short           actualPosition1;
	unsigned short           actualPosition2;
	unsigned short           actualPosition3;
	unsigned short           comparePosition1;
	unsigned short           comparePosition2;
	unsigned short           comparePosition3;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	dim1PeakDistanceDistribution = new unsigned int[(*space)->getStep(DIM1)];
	if (dim1PeakDistanceDistribution == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	dim2PeakDistanceDistribution = new unsigned int[(*space)->getStep(DIM2)];
	if (dim2PeakDistanceDistribution == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	dim3PeakDistanceDistribution = new unsigned int[(*space)->getStep(DIM3)];
	if (dim3PeakDistanceDistribution == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	accumulatedPeakDistanceDistribution = new unsigned int[(unsigned int)ceil(sqrt((double)sqr((*space)->getStep(DIM1)) + (double)sqr((*space)->getStep(DIM2)) + (double)sqr((*space)->getStep(DIM3))))];
	if (accumulatedPeakDistanceDistribution == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	setupFindableEstimatedTracks(&findableTracks, terminal);

	createTerminalStatusSequence(&statusSequenceForPeakDistance, terminal, "Analyse peak distances:\t\t\t\t", (unsigned int)findableTracks.size());
	terminalInitialize(statusSequenceForPeakDistance);

	for (i = 0; i < (unsigned int)(*space)->getStep(DIM1); i++)
		dim1PeakDistanceDistribution[i]        = 0;
	for (i = 0; i < (unsigned int)(*space)->getStep(DIM2); i++)
		dim2PeakDistanceDistribution[i]        = 0;
	for (i = 0; i < (unsigned int)(*space)->getStep(DIM3); i++)
		dim3PeakDistanceDistribution[i]        = 0;
	for (i = 0; i < (unsigned int)ceil(sqrt((double)sqr((*space)->getStep(DIM1)) + (double)sqr((*space)->getStep(DIM2)) + (double)sqr((*space)->getStep(DIM3)))); i++)
		accumulatedPeakDistanceDistribution[i] = 0;

	for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end();) {

		track = actualTrack->getTrack();
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		actualPosition1 = actualTrack->getPosition().get(DIM1);
		actualPosition2 = actualTrack->getPosition().get(DIM2);
		actualPosition3 = actualTrack->getPosition().get(DIM3);

		actualTrack++;

		for (std::list<findableTrack>::iterator compareTrack = actualTrack; compareTrack != findableTracks.end(); compareTrack++) {

			comparePosition1 = compareTrack->getPosition().get(DIM1);
			comparePosition2 = compareTrack->getPosition().get(DIM2);
			comparePosition3 = compareTrack->getPosition().get(DIM3);
			
			if (abs(actualPosition1 - comparePosition1) < (*space)->getStep(DIM1))
				dim1PeakDistanceDistribution[abs(actualPosition1 - comparePosition1)]++;
			if (abs(actualPosition2 - comparePosition2) < (*space)->getStep(DIM2))
				dim2PeakDistanceDistribution[abs(actualPosition2 - comparePosition2)]++;
			if (abs(actualPosition3 - comparePosition3) < (*space)->getStep(DIM3))
				dim3PeakDistanceDistribution[abs(actualPosition3 - comparePosition3)]++;
			if ((unsigned int)sqrt((double)sqr(actualPosition1 - comparePosition1) + (double)sqr(actualPosition2 - comparePosition2) + (double)sqr(actualPosition3 - comparePosition3)) < (unsigned int)ceil(sqrt((double)sqr((*space)->getStep(DIM1)) + (double)sqr((*space)->getStep(DIM2)) + (double)sqr((*space)->getStep(DIM3)))))
				accumulatedPeakDistanceDistribution[(unsigned int)sqrt((double)sqr(actualPosition1 - comparePosition1) + (double)sqr(actualPosition2 - comparePosition2) + (double)sqr(actualPosition3 - comparePosition3))]++;

		}

		terminalOverwriteWithIncrement(statusSequenceForPeakDistance);

	}

	terminalFinalize(statusSequenceForPeakDistance);

	if (analysisResultWarnings & DIMPEAKDISTANCEDISTRIBUTION) {

		dim1PeakDistanceDistributionWarningMsg* dim1PeakDistance = new dim1PeakDistanceDistributionWarningMsg(dim1PeakDistanceDistribution, (unsigned int)(*space)->getStep(DIM1));
		dim1PeakDistance->warningMsg();
		if(dim1PeakDistance != NULL) {
			delete dim1PeakDistance;
			dim1PeakDistance = NULL;
		}
		dim2PeakDistanceDistributionWarningMsg* dim2PeakDistance = new dim2PeakDistanceDistributionWarningMsg(dim2PeakDistanceDistribution, (unsigned int)(*space)->getStep(DIM2));
		dim2PeakDistance->warningMsg();
		if(dim2PeakDistance != NULL) {
			delete dim2PeakDistance;
			dim2PeakDistance = NULL;
		}
		dim3PeakDistanceDistributionWarningMsg* dim3PeakDistance = new dim3PeakDistanceDistributionWarningMsg(dim3PeakDistanceDistribution, (unsigned int)(*space)->getStep(DIM3));
		dim3PeakDistance->warningMsg();
		if(dim3PeakDistance != NULL) {
			delete dim3PeakDistance;
			dim3PeakDistance = NULL;
		}
		accumulatedPeakDistanceDistributionWarningMsg* accumulatedPeakDistance = new accumulatedPeakDistanceDistributionWarningMsg(accumulatedPeakDistanceDistribution, (unsigned int)ceil(sqrt((double)sqr((*space)->getStep(DIM1)) + (double)sqr((*space)->getStep(DIM2)) + (double)sqr((*space)->getStep(DIM3)))));
		accumulatedPeakDistance->warningMsg();
		if(accumulatedPeakDistance != NULL) {
			delete accumulatedPeakDistance;
			accumulatedPeakDistance = NULL;
		}

	}


	if (analysisResultDisplays & DIMPEAKDISTANCEDISTRIBUTION) {

		initializeShowAnalysis();

		showAnalyser->addDim1PeakDistanceDistribution(dim1PeakDistanceDistribution, (*space)->getStep(DIM1));
		showAnalyser->addDim2PeakDistanceDistribution(dim2PeakDistanceDistribution, (*space)->getStep(DIM2));
		showAnalyser->addDim3PeakDistanceDistribution(dim3PeakDistanceDistribution, (*space)->getStep(DIM3));
		showAnalyser->addAccumulatedPeakDistanceDistribution(accumulatedPeakDistanceDistribution, (unsigned int)ceil(sqrt((double)sqr((*space)->getStep(DIM1)) + (double)sqr((*space)->getStep(DIM2)) + (double)sqr((*space)->getStep(DIM3)))));

	}

	if (dim1PeakDistanceDistribution != NULL) {
		delete[] dim1PeakDistanceDistribution;
		dim1PeakDistanceDistribution = NULL;
	}
	if (dim2PeakDistanceDistribution != NULL) {
		delete[] dim2PeakDistanceDistribution;
		dim2PeakDistanceDistribution = NULL;
	}
	if (dim3PeakDistanceDistribution != NULL) {
		delete[] dim3PeakDistanceDistribution;
		dim3PeakDistanceDistribution = NULL;
	}
	if (accumulatedPeakDistanceDistribution != NULL) {
		delete[] accumulatedPeakDistanceDistribution;
		accumulatedPeakDistanceDistribution = NULL;
	}

}

/**
 * method evaluates the neccessary files for a CELL BE simulation.
 */

void analysis::evaluateCellSimulationFiles(std::string hitFileName, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal) {

	terminalSequence     statusSequenceForCellFiles;
	digitalHitAccess     digitalHitData;
	prelutAccess         prelut;
	lutAccess            lut;
	trackfinderInputHit* analogHitValue;
	digitalHit           digitalHitValue;
	lutBorder            border;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((luts == NULL) || (*luts == NULL))
		throw cannotAccessLutsError();

	createTerminalStatusSequence(&statusSequenceForCellFiles, terminal, "\nEvaluate Cell BE simulation files:\t\t", (unsigned int)(*eventData)->getNumberOfHits());
	terminalInitialize(statusSequenceForCellFiles);

	/* reset the counter for the corrections in the first and second
	 * dimension. Corrected are these values which have a difference
	 * of more than one in the first dimension according to the previous
	 * value */
	if ((*luts)->typeUsesCorrections())
		(*luts)->resetCorrectionCounter();

	if (!hitFileName.empty())
		digitalHitData.init();
	if (!prelutFileName.empty())
		prelut.init((*luts)->getPrelutDefinition().dim3Min, (*luts)->getPrelutDefinition().dim3Max, (*luts)->getPrelutDefinition().dim3Step, (*luts)->getPrelutDefinition().dim3StartEntry, (*luts)->getPrelutDefinition().dim3StopEntry);
	if (!lutFileName.empty())
		lut.init((*luts)->getLutDefinition().dim1Min, (*luts)->getLutDefinition().dim1Max, (*luts)->getLutDefinition().dim1Step, (*luts)->getLutDefinition().dim2Min, (*luts)->getLutDefinition().dim2Max, (*luts)->getLutDefinition().dim2Step);

	for (int i = 0; i < (*eventData)->getNumberOfHits(); i++) {

		analogHitValue = (*eventData)->getHitByIndex(i);
		if (analogHitValue == NULL)
			throw cannotAccessHitsOrTracksError(ANALYSISLIB);

		if (analogHitValue->getStation() != NULL) {

			if (!analogHitValue->getStation()->isMasked()) {

				digitalHitValue.setHit(analogHitValue);
				(*luts)->evaluate(analogHitValue, &border);

				if (!hitFileName.empty())
					digitalHitData.addEntry(digitalHitValue);
				if (!prelutFileName.empty())
					prelut.addEntry(border.getPrelutHoughBorder(), digitalHitValue);
				if (!lutFileName.empty())
					lut.addEntry(border.getLutHoughBorder(), digitalHitValue);

			}

		}

		terminalOverwriteWithIncrement(statusSequenceForCellFiles);

	}

	terminalFinalize(statusSequenceForCellFiles);

	if (!hitFileName.empty())
		digitalHitData.write(hitFileName, "Hits for CellBE simulation", terminal);
	if (!prelutFileName.empty())
		prelut.write(prelutFileName, "Prelut for CellBE simulation", terminal);
	if (!lutFileName.empty())
		lut.write(lutFileName, "Lut for CellBE simulation", HARDWAREFORMAT, terminal);

}
void analysis::evaluateCellSimulationFiles(std::string hitFileName, std::string prelutFileName, std::string lutFileName, unsigned int eventNumber, std::streambuf* terminal) {

	std::string modifiedHitFileName;
	std::string modifiedPrelutFileName;
	std::string modifiedLutFileName;
	char        buffer[intConversionDigits+1];
	std::string modifier;
	int         modifierPosition;

	modifiedHitFileName    = hitFileName;
	modifiedPrelutFileName = prelutFileName;
	modifiedLutFileName    = lutFileName;

	uitos(eventNumber, buffer, 10, intConversionDigits);

	modifier  = "_";
	modifier += buffer;

	if (!modifiedHitFileName.empty()) {
		
		modifierPosition = (int)modifiedHitFileName.find_last_of(".", modifiedHitFileName.length());
		modifiedHitFileName.insert(modifierPosition, modifier);

	}

	if (!modifiedPrelutFileName.empty()) {

		modifierPosition = (int)modifiedPrelutFileName.find_last_of(".", modifiedPrelutFileName.length());
		modifiedPrelutFileName.insert(modifierPosition, modifier);

	}

	if (!modifiedLutFileName.empty()) {

		modifierPosition = (int)modifiedLutFileName.find_last_of(".", modifiedLutFileName.length());
		modifiedLutFileName.insert(modifierPosition, modifier);

	}

	evaluateCellSimulationFiles(modifiedHitFileName, modifiedPrelutFileName, modifiedLutFileName, terminal);

}

/****************************************************************
 * method initializes the configuration-Analysis.				*
 ****************************************************************/

void analysis::initConfigurationAnalysis(bool enable) {

	configuration = enable;

}

/****************************************************************
 * method initializes the configuration-Analysis.				*
 ****************************************************************/

void analysis::initDetectorAnalysis(bool enable) {

	detector = enable;

}

/****************************************************************
 * method initializes the memory-Analysis.						*
 ****************************************************************/

void analysis::initMemoryAnalysis(bool enable) {

	memory = enable;

}

/****************************************************************
 * method initializes the time-Analysis.						*
 ****************************************************************/

void analysis::initTimeAnalysis(bool enable) {

	time = enable;

}

/****************************************************************
 * method initializes the event-Analysis.						*
 ****************************************************************/

void analysis::initEventAnalysis(bool enable) {

	event = enable;

}

/****************************************************************
 * method initializes the classPriority-Analysis.				*
 ****************************************************************/

void analysis::initClassPriorityAnalysis(bool enable) {

	classPriority = enable;

}

/****************************************************************
 * method initializes the type of the filter.					*
 ****************************************************************/

void analysis::initAutomaticFilterGeometryAnalysis(histogramData** histogram, bool enable) {

	if (enable) {

		if (peakfindingGeometryAnalyser == NULL)
			peakfindingGeometryAnalyser = new peakfindingGeometryAnalysis(histogram);

	}
	else {

		if (peakfindingGeometryAnalyser != NULL) {
			delete peakfindingGeometryAnalyser;
			peakfindingGeometryAnalyser = NULL;
		}

	}

}

/****************************************************************
 * method initializes the number of displays.					*
 ****************************************************************/

void analysis::initTrackPropagationNumberOfDisplays(unsigned short numberOfDisplays) {

	if (trackPropagationAnalyser != NULL)
		trackPropagationAnalyser->initNumberOfDisplays(numberOfDisplays);
	else
		throw cannotAccessTrackPropagationAnalyserError();

}

/****************************************************************
 * method initializes the track-propagation-Analysis.			*
 ****************************************************************/

void analysis::initTrackPropagationEventPointAnalysis(bool enable) {

	if (enable) {

		if (trackPropagationAnalyser == NULL)
			trackPropagationAnalyser = new trackPropagationAnalysis();

		trackPropagationAnalyser->initTrackPropagationEventPointAnalysis(enable);

	}
	else {

		if (trackPropagationAnalyser != NULL) {

			trackPropagationAnalyser->initTrackPropagationEventPointAnalysis(enable);

			if (!isTrackPropagationEventAnalysisEnabled() && !isTrackPropagationTotalAnalysisEnabled()) {
				delete trackPropagationAnalyser;
				trackPropagationAnalyser = NULL;
			}

		}

	}

}
void analysis::initTrackPropagationEventHitAnalysis(bool enable) {

	if (enable) {

		if (trackPropagationAnalyser == NULL)
			trackPropagationAnalyser = new trackPropagationAnalysis();

		trackPropagationAnalyser->initTrackPropagationEventHitAnalysis(enable);

	}
	else {

		if (trackPropagationAnalyser != NULL) {

			trackPropagationAnalyser->initTrackPropagationEventHitAnalysis(enable);

			if (!isTrackPropagationEventAnalysisEnabled() && !isTrackPropagationTotalAnalysisEnabled()) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}
void analysis::initTrackPropagationTotalPointAnalysis(bool enable) {

	if (enable) {

		if (trackPropagationAnalyser == NULL)
			trackPropagationAnalyser = new trackPropagationAnalysis();

		trackPropagationAnalyser->initTrackPropagationTotalPointAnalysis(enable);

	}
	else {

		if (trackPropagationAnalyser != NULL) {

			trackPropagationAnalyser->initTrackPropagationTotalPointAnalysis(enable);

			if (!isTrackPropagationEventAnalysisEnabled() && !isTrackPropagationTotalAnalysisEnabled()) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}
void analysis::initTrackPropagationTotalHitAnalysis(bool enable) {

	if (enable) {

		if (trackPropagationAnalyser == NULL)
			trackPropagationAnalyser = new trackPropagationAnalysis();

		trackPropagationAnalyser->initTrackPropagationTotalHitAnalysis(enable);

	}
	else {

		if (trackPropagationAnalyser != NULL) {

			trackPropagationAnalyser->initTrackPropagationTotalHitAnalysis(enable);

			if (!isTrackPropagationEventAnalysisEnabled() && !isTrackPropagationTotalAnalysisEnabled()) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initTrackPropagationAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (trackPropagationAnalyser != NULL) {
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (prelutRangeLayerAnalyser != NULL)
				justUpdate |= prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
			trackPropagationAnalyser->initTrackPropagationAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the track propagation's display.		*
 ****************************************************************/

void analysis::initTrackPropagationAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (prelutRangeLayerAnalyser != NULL) {

			if (prelutRangeLayerAnalyser->isPrelutRangeDisplayEnabled()) {

				prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (trackPropagationAnalyser == NULL)
			trackPropagationAnalyser = new trackPropagationAnalysis();

		trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(enable);

	}
	else {

		if (trackPropagationAnalyser != NULL) {

			trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(enable);

			if ((!trackPropagationAnalyser->isTrackPropagationEventAnalysisEnabled()) && (!trackPropagationAnalyser->isTrackPropagationTotalAnalysisEnabled())) {
				delete trackPropagationAnalyser;
				trackPropagationAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCEventAbsolute-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCEventAbsoluteAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCEventAbsoluteAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCEventAbsoluteAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}
		
		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCEventRelative-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCEventRelativeAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCEventRelativeAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCEventRelativeAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCTotalAbsolute-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCTotalAbsoluteAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCTotalAbsoluteAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCTotalAbsoluteAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the qualityEFGCTotalRelative-Analysis.	*
 ****************************************************************/

void analysis::initQualityEFGCTotalRelativeAnalysis(bool enable) {

	if (enable) {

		if (qualityAnalyser == NULL)
			qualityAnalyser = new qualityAnalysis();

		qualityAnalyser->initQualityEFGCTotalRelativeAnalysis(enable);

	}
	else {

		if (qualityAnalyser != NULL) {

			qualityAnalyser->initQualityEFGCTotalRelativeAnalysis(enable);

			if ((!qualityAnalyser->isQualityEFGCEventAnalysisEnabled()) && (!qualityAnalyser->isQualityEFGCTotalAnalysisEnabled())) {
				delete qualityAnalyser;
				qualityAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzE-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzF-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzG-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzC-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCEventPzEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPzEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPzEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtE-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtF-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtG-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtC-Analysis.			*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCEventPtEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCEventPtEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCEventPtEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzE-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzF-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzG-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPzEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPzEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPzEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtE-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtEAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtEAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtF-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtFAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtFAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtG-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtGAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtGAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtEFGC-Analysis.		*
 ****************************************************************/

void analysis::initMomentumEFGCTotalPtEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumEFGCTotalPtEFGCAnalysis(enable, nBins, xMin, xMax);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumEFGCTotalPtEFGCAnalysis(enable, nBins, xMin, xMax);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent12EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent12EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent12EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent13EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent13EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent13EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNEvent32EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNEvent32EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNEvent32EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal12EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal12EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal12EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal13EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal13EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal13EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32E-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32EAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32EAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32F-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32FAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32FAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32G-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32GAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32GAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32C-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32CAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32CAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32N-Analysis.		*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32NAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32NAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32EFGCN-Analysis.	*
 ****************************************************************/

void analysis::initProjectionEFGCNTotal32EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionEFGCNTotal32EFGCNAnalysis(enable, nXBins, nYBins);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionEFGCNTotal32EFGCNAnalysis(enable, nXBins, nYBins);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initMomentumAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (momentumAnalyser != NULL) {
			if (trackPropagationAnalyser != NULL)
				justUpdate |= trackPropagationAnalyser->isTrackPropagationToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			if (prelutRangeLayerAnalyser != NULL)
				justUpdate |= prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
			momentumAnalyser->initMomentumAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initProjectionAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisProjectionOutputFileNameSpecifiedWarningMsg* noAnalysisProjectionOutputFileNameSpecified = new noAnalysisProjectionOutputFileNameSpecifiedWarningMsg();
		noAnalysisProjectionOutputFileNameSpecified->warningMsg();
		if(noAnalysisProjectionOutputFileNameSpecified != NULL) {
			delete noAnalysisProjectionOutputFileNameSpecified;
			noAnalysisProjectionOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (projectionAnalyser != NULL) {
			if (trackPropagationAnalyser != NULL)
				justUpdate |= trackPropagationAnalyser->isTrackPropagationToRootEnabled();
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			if (prelutRangeLayerAnalyser != NULL)
				justUpdate |= prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
			projectionAnalyser->initProjectionAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the graphic's display.				*
 ****************************************************************/

void analysis::initMomentumAnalysisDisplay(bool enable) {

	if (enable) {

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (prelutRangeLayerAnalyser != NULL) {

			if (prelutRangeLayerAnalyser->isPrelutRangeDisplayEnabled()) {

				prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (trackPropagationAnalyser != NULL) {

			if (trackPropagationAnalyser->isTrackPropagationDisplayEnabled()) {

				trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (momentumAnalyser == NULL)
			momentumAnalyser = new momentumAnalysis();

		momentumAnalyser->initMomentumAnalysisDisplay(enable);

	}
	else {

		if (momentumAnalyser != NULL) {

			momentumAnalyser->initMomentumAnalysisDisplay(enable);

			if ((!momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled()) && (!momentumAnalyser->isMomentumDisplayEnabled())) {
				delete momentumAnalyser;
				momentumAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the graphic's display.				*
 ****************************************************************/

void analysis::initProjectionAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (prelutRangeLayerAnalyser != NULL) {

			if (prelutRangeLayerAnalyser->isPrelutRangeDisplayEnabled()) {

				prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (trackPropagationAnalyser != NULL) {

			if (trackPropagationAnalyser->isTrackPropagationDisplayEnabled()) {

				trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser == NULL)
			projectionAnalyser = new projectionAnalysis();

		projectionAnalyser->initProjectionAnalysisDisplay(enable);

	}
	else {

		if (projectionAnalyser != NULL) {

			projectionAnalyser->initProjectionAnalysisDisplay(enable);

			if ((!projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled()) && (!projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled())) {
				delete projectionAnalyser;
				projectionAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXBxAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXByAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXBzAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the										*
 * initMagnetfieldFactorAnalysis-Analysis.						*
 ****************************************************************/

void analysis::initMagnetfieldConstantAnalysis(bool enable, int nBins, double min, double max) {

	if (enable) {

		if (magnetfieldFactorAnalyser == NULL)
			magnetfieldFactorAnalyser = new magnetfieldFactorAnalysis();
		
		magnetfieldFactorAnalyser->init(nBins, min, max);

	}
	else {

		if (magnetfieldFactorAnalyser != NULL) {

			delete magnetfieldFactorAnalyser;
			magnetfieldFactorAnalyser = NULL;

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldXAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldXAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldXAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYBxAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYByAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYBzAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldYAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldYAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldYAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZBxAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZByAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZBzAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldZAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldZAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldZAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldAnalysis-Analysis.	*
 ****************************************************************/

void analysis::initMagnetfieldAnalysis(bool enable, int nBins, double xMin, double yMin, double zMin, double xMax, double yMax, double zMax, double constValDim1, double constValDim2, double constValueDim3) {

	if (enable) {

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldAnalysis(enable, nBins, xMin, yMin, zMin, xMax, yMax, zMax, constValDim1, constValDim2, constValueDim3);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldAnalysis(enable, nBins, xMin, yMin, zMin, xMax, yMax, zMax, constValDim1, constValDim2, constValueDim3);

			if (!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled()){
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initMagnetfieldAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (magnetfieldAnalyser != NULL) {
			if (trackPropagationAnalyser != NULL)
				justUpdate |= trackPropagationAnalyser->isTrackPropagationToRootEnabled();
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			if (prelutRangeLayerAnalyser != NULL)
				justUpdate |= prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
			magnetfieldAnalyser->initMagnetfieldAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the magnetfield's display.			*
 ****************************************************************/

void analysis::initMagnetfieldAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (prelutRangeLayerAnalyser != NULL) {

			if (prelutRangeLayerAnalyser->isPrelutRangeDisplayEnabled()) {

				prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (trackPropagationAnalyser != NULL) {

			if (trackPropagationAnalyser->isTrackPropagationDisplayEnabled()) {

				trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser == NULL)
			magnetfieldAnalyser = new magnetfieldAnalysis();

		magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(enable);

	}
	else {

		if (magnetfieldAnalyser != NULL) {

			magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(enable);

			if ((!magnetfieldAnalyser->isMagnetfieldAnalysisEnabled())) {
				delete magnetfieldAnalyser;
				magnetfieldAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initMagnetfieldConstantAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (magnetfieldFactorAnalyser != NULL) {
			if (trackPropagationAnalyser != NULL)
				justUpdate |= trackPropagationAnalyser->isTrackPropagationToRootEnabled();
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			if (prelutRangeLayerAnalyser != NULL)
				justUpdate |= prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
			magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the magnetfield's display.			*
 ****************************************************************/

void analysis::initMagnetfieldConstantAnalysisDisplay(bool enable) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (prelutRangeLayerAnalyser != NULL) {

			if (prelutRangeLayerAnalyser->isPrelutRangeDisplayEnabled()) {

				prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (trackPropagationAnalyser != NULL) {

			if (trackPropagationAnalyser->isTrackPropagationDisplayEnabled()) {

				trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser == NULL)
			magnetfieldFactorAnalyser = new magnetfieldFactorAnalysis();

		magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(enable);

	}
	else {

		if (magnetfieldFactorAnalyser != NULL) {

			magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(enable);

			if ((!magnetfieldFactorAnalyser->isMagnetfieldFactorAnalysisEnabled())) {
				delete magnetfieldFactorAnalyser;
				magnetfieldFactorAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the										*
 * initPrelutRangeAnalysis-Analysis.							*
 ****************************************************************/

void analysis::initPrelutRangeAnalysis(bool enable) {

	if (enable) {

		if (prelutRangeLayerAnalyser == NULL)
			prelutRangeLayerAnalyser = new prelutRangeLayerAnalysis();

	}
	else {

		if (prelutRangeLayerAnalyser != NULL) {

			delete prelutRangeLayerAnalyser;
			prelutRangeLayerAnalyser = NULL;

		}

	}

}
void analysis::initPrelutRangeAnalysis(bool enable, unsigned short numberOfDisplays) {

	if (enable) {

		if (prelutRangeLayerAnalyser == NULL)
			prelutRangeLayerAnalyser = new prelutRangeLayerAnalysis();

		prelutRangeLayerAnalyser->init(numberOfDisplays);

	}
	else {

		if (prelutRangeLayerAnalyser != NULL) {

			delete prelutRangeLayerAnalyser;
			prelutRangeLayerAnalyser = NULL;

		}

	}

}
void analysis::initPrelutRangeAnalysis(bool enable, unsigned short prelutRangeCut, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) {

	if (enable) {

		if (prelutRangeLayerAnalyser == NULL)
			prelutRangeLayerAnalyser = new prelutRangeLayerAnalysis();

		prelutRangeLayerAnalyser->init(prelutRangeCut, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

	}
	else {

		if (prelutRangeLayerAnalyser != NULL) {

			delete prelutRangeLayerAnalyser;
			prelutRangeLayerAnalyser = NULL;

		}

	}

}
void analysis::initPrelutRangeAnalysis(bool enable, unsigned short prelutRangeCut, unsigned short numberOfDisplays, unsigned int numberOfMinFactors, double factorMinMin, double factorMinMax, unsigned int numberOfMaxFactors, double factorMaxMin, double factorMaxMax) {

	if (enable) {

		if (prelutRangeLayerAnalyser == NULL)
			prelutRangeLayerAnalyser = new prelutRangeLayerAnalysis();

		prelutRangeLayerAnalyser->init(prelutRangeCut, numberOfDisplays, numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);

	}
	else {

		if (prelutRangeLayerAnalyser != NULL) {

			delete prelutRangeLayerAnalyser;
			prelutRangeLayerAnalyser = NULL;

		}

	}

}
/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void analysis::initPrelutRangeAnalysisToRoot(bool enable, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (prelutRangeLayerAnalyser != NULL) {
			if (trackPropagationAnalyser != NULL)
				justUpdate |= trackPropagationAnalyser->isTrackPropagationToRootEnabled();
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (histogramAnalyser != NULL)
				justUpdate |= histogramAnalyser->isHistogramToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			prelutRangeLayerAnalyser->initPrelutRangeAnalysisToRoot(enable, name, justUpdate);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * This method initializes the magnetfield's display.			*
 ****************************************************************/

void analysis::initPrelutRangeAnalysisDisplay(bool enable, unsigned short displayMode) {

	if (enable) {

		if (momentumAnalyser != NULL) {

			if (momentumAnalyser->isMomentumDisplayEnabled()) {

				momentumAnalyser->initMomentumAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (projectionAnalyser != NULL) {

			if (projectionAnalyser->isProjectionDisplayEnabled()) {

				projectionAnalyser->initProjectionAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldAnalyser != NULL) {

			if (magnetfieldAnalyser->isMagnetfieldDisplayEnabled()) {

				magnetfieldAnalyser->initMagnetfieldAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (magnetfieldFactorAnalyser != NULL) {

			if (magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled()) {

				magnetfieldFactorAnalyser->initMagnetfieldFactorAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (trackPropagationAnalyser != NULL) {

			if (trackPropagationAnalyser->isTrackPropagationDisplayEnabled()) {

				trackPropagationAnalyser->initTrackPropagationAnalysisDisplay(false);

				enableJustOneDisplayWarningMsg* enableJustOneDisplay = new enableJustOneDisplayWarningMsg();
				enableJustOneDisplay->warningMsg();
				if(enableJustOneDisplay != NULL) {
					delete enableJustOneDisplay;
					enableJustOneDisplay = NULL;
				}

			}

		}

		if (prelutRangeLayerAnalyser == NULL)
			prelutRangeLayerAnalyser = new prelutRangeLayerAnalysis();

		prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(enable, displayMode);

	}
	else {

		if (prelutRangeLayerAnalyser != NULL) {

			prelutRangeLayerAnalyser->initPrelutRangeAnalysisDisplay(enable, displayMode);

			if ((!prelutRangeLayerAnalyser->isPrelutRangeAnalysisEnabled())) {
				delete prelutRangeLayerAnalyser;
				prelutRangeLayerAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * This method initializes the histogram analysis.				*
 ****************************************************************/

void analysis::initHistogramAnalysis(histogramData** histogram, bool enable) {

	if (enable) {

		if (histogramAnalyser == NULL)
			histogramAnalyser = new histogramAnalysis(histogram);

	}
	else {

		if (histogramAnalyser != NULL) {

			delete histogramAnalyser;
			histogramAnalyser = NULL;

		}

	}

}

/****************************************************************
 * This method initializes the root directory for the analysis.	*
 ****************************************************************/

void analysis::initHistogramAnalysis(bool enableCreated, bool enableEncoded, bool enableFiltered,
									 bool justOneCreated, bool justOneEncoded, bool justOneFiltered,
									 bool showCreated, bool showEncoded, bool showFiltered,
									 unsigned short layer, const char* name) {

	bool justUpdate;

	if ((name == NULL) || (strcmp(name, "") == 0)) {

		noAnalysisOutputFileNameSpecifiedWarningMsg* noAnalysisMomentumOutputFileNameSpecified = new noAnalysisOutputFileNameSpecifiedWarningMsg();
		noAnalysisMomentumOutputFileNameSpecified->warningMsg();
		if(noAnalysisMomentumOutputFileNameSpecified != NULL) {
			delete noAnalysisMomentumOutputFileNameSpecified;
			noAnalysisMomentumOutputFileNameSpecified = NULL;
		}

	}
	else {

		justUpdate = false;
		if (histogramAnalyser != NULL) {
			if (trackPropagationAnalyser != NULL)
				justUpdate |= trackPropagationAnalyser->isTrackPropagationToRootEnabled();
			if (momentumAnalyser != NULL)
				justUpdate |= momentumAnalyser->isMomentumToRootEnabled();
			if (projectionAnalyser != NULL)
				justUpdate |= projectionAnalyser->isProjectionToRootEnabled();
			if (magnetfieldAnalyser != NULL)
				justUpdate |= magnetfieldAnalyser->isMagnetfieldToRootEnabled();
			if (magnetfieldFactorAnalyser != NULL)
				justUpdate |= magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
			if (prelutRangeLayerAnalyser != NULL)
				justUpdate |= prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
			histogramAnalyser->initHistogramAnalysisToRoot(enableCreated, enableEncoded, enableFiltered, justOneCreated, justOneEncoded, justOneFiltered, name, justUpdate);
			histogramAnalyser->initHistogramAnalysisToShow(showCreated, showEncoded, showFiltered);
			histogramAnalyser->initHistogramLayer(layer);
		}

#ifdef ENABLEALLWARNINGS

		else {

			cannotUseToRootBeforeInitializingWarningMsg* cannotUseToRootBeforeInitializing = new cannotUseToRootBeforeInitializingWarningMsg();
			cannotUseToRootBeforeInitializing->warningMsg();
			if(cannotUseToRootBeforeInitializing != NULL) {
				delete cannotUseToRootBeforeInitializing;
				cannotUseToRootBeforeInitializing = NULL;
			}

		}

#endif

	}

}

/****************************************************************
 * method returns true if the configuration-Analysis is enabled.*
 ****************************************************************/

bool analysis::isConfigurationAnalysisEnabled() {

	return configuration;

}

/****************************************************************
 * method returns true if the detector-Analysis is enabled.		*
 ****************************************************************/

bool analysis::isDetectorAnalysisEnabled() {

	return detector;

}

/****************************************************************
 * method returns true if the memory-Analysis is enabled.		*
 ****************************************************************/

bool analysis::isMemoryAnalysisEnabled() {

	return memory;

}

/****************************************************************
 * method returns true if the time-Analysis is enabled.			*
 ****************************************************************/

bool analysis::isTimeAnalysisEnabled() {

	return time;

}

/****************************************************************
 * method returns true if the event-Analysis is enabled.		*
 ****************************************************************/

bool analysis::isEventAnalysisEnabled() {

	return event;

}

/****************************************************************
 * method returns true if the classPriority-Analysis is enabled.*
 ****************************************************************/

bool analysis::isClassPriorityAnalysisEnabled() {

	return classPriority;

}

/****************************************************************
 * method returns true if the filterGeometry-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool analysis::isAutomaticFilterGeometryAnalysisEnabled() {

	bool returnValue;

	if (peakfindingGeometryAnalyser != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the track-propagation-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool analysis::isTrackPropagationEventAnalysisEnabled() {

	bool returnValue;

	if (trackPropagationAnalyser != NULL)
		returnValue = trackPropagationAnalyser->isTrackPropagationEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}
bool analysis::isTrackPropagationTotalAnalysisEnabled() {

	bool returnValue;

	if (trackPropagationAnalyser != NULL)
		returnValue = trackPropagationAnalyser->isTrackPropagationTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the quality-Analysis for each event	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isQualityEventAnalysisEnabled() {

	bool returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->isQualityEFGCEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the quality-Analysis over all events	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isQualityTotalAnalysisEnabled() {

	bool returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->isQualityEFGCTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumEFGC-Analysis for each	*
 * event is enabled.											*
 ****************************************************************/

bool analysis::isMomentumEFGCEventAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = (momentumAnalyser->isMomentumEFGCEventPzAnalysisEnabled() || momentumAnalyser->isMomentumEFGCEventPtAnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the projectionEFGCN-Analysis for each	*
 * event is enabled.											*
 ****************************************************************/

bool analysis::isProjectionEFGCNEventAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = (projectionAnalyser->isProjectionEFGCNEvent12AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNEvent13AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNEvent32AnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumEFGC-Analysis over all	*
 * events is enabled.											*
 ****************************************************************/

bool analysis::isMomentumEFGCTotalAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = (momentumAnalyser->isMomentumEFGCTotalPzAnalysisEnabled() || momentumAnalyser->isMomentumEFGCTotalPtAnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the projectionEFGCN-Analysis over all	*
 * events is enabled.											*
 ****************************************************************/

bool analysis::isProjectionEFGCNTotalAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = (projectionAnalyser->isProjectionEFGCNTotal12AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNTotal13AnalysisEnabled() || projectionAnalyser->isProjectionEFGCNTotal32AnalysisEnabled());
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the track-propagation-to-root-		*
 * analysis is enabled.											*
 ****************************************************************/

bool analysis::isTrackPropagationToRootEnabled() {

	bool returnValue;

	if (trackPropagationAnalyser != NULL)
		returnValue = trackPropagationAnalyser->isTrackPropagationToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * graphic-to-root-analysis is enabled.							*
 ****************************************************************/

bool analysis::isMomentumToRootEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * graphic-to-root-analysis is enabled.							*
 ****************************************************************/

bool analysis::isProjectionToRootEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * magnetfield-to-root-analysis is enabled.						*
 ****************************************************************/

bool analysis::isMagnetfieldToRootEnabled() {

	bool returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->isMagnetfieldToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * magnetfield-constant-to-root-analysis is enabled.			*
 ****************************************************************/

bool analysis::isMagnetfieldFactorToRootEnabled() {

	bool returnValue;

	if (magnetfieldFactorAnalyser != NULL)
		returnValue = magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the									*
 * prelut-range-to-root-analysis is enabled.					*
 ****************************************************************/

bool analysis::isPrelutRangeToRootEnabled() {

	bool returnValue;

	if (prelutRangeLayerAnalyser != NULL)
		returnValue = prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * qualityEFGCEvent-Analysis.									*
 ****************************************************************/

std::string analysis::getQualityEFGCEventAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getQualityEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getAbsoluteQualityEFGCEventAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getAbsoluteQualityEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getRelativeQualityEFGCEventAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getRelativeQualityEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * qualityEFGCTotal-Analysis.									*
 ****************************************************************/

std::string analysis::getQualityEFGCTotalAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getQualityEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getAbsoluteQualityEFGCTotalAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getAbsoluteQualityEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}
std::string analysis::getRelativeQualityEFGCTotalAnalysis() {

	std::string returnValue;

	if (qualityAnalyser != NULL)
		returnValue = qualityAnalyser->getRelativeQualityEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * This method returns true if the track-propagation's display	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isTrackPropagationDisplayEnabled() {

	bool returnValue;

	if (trackPropagationAnalyser != NULL)
		returnValue = trackPropagationAnalyser->isTrackPropagationDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the track-propagation-Analysis display.		*
 ****************************************************************/

void analysis::trackPropagationAnalysisUpdate() {

	if (trackPropagationAnalyser != NULL) {

		trackPropagationAnalyser->trackPropagationAnalysisUpdate();
	
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the track-propagation-Analysis display.			*
 ****************************************************************/

void analysis::trackPropagationAnalysisDraw(bitArray preventPointDraw, bitArray preventHitDraw) {

	if (trackPropagationAnalyser != NULL) {

		trackPropagationAnalyser->trackPropagationAnalysisDraw(preventPointDraw, preventHitDraw);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method returns true if the momentumAnalysis for each event	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMomentumEventAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumAnalysis over all events	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMomentumTotalAnalysisEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true if the graphic's display is enabled.*
 ****************************************************************/

bool analysis::isMomentumDisplayEnabled() {

	bool returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->isMomentumDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the graphic-Analysis display.					*
 ****************************************************************/

void analysis::momentumAnalysisUpdate() {

	if (momentumAnalyser != NULL)
		momentumAnalyser->momentumAnalysisUpdate();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the graphic-Analysis display.					*
 ****************************************************************/

void analysis::momentumAnalysisDraw() {

	if (momentumAnalyser != NULL)
		momentumAnalyser->momentumAnalysisDraw();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method returns true if the momentumAnalysis for each event	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isProjectionEventAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionEventAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true if the momentumAnalysis over all events	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isProjectionTotalAnalysisEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionTotalAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true if the graphic's display is enabled.*
 ****************************************************************/

bool analysis::isProjectionDisplayEnabled() {

	bool returnValue;

	if (projectionAnalyser != NULL)
		returnValue = projectionAnalyser->isProjectionDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the graphic-Analysis display.					*
 ****************************************************************/

void analysis::projectionAnalysisUpdate() {

	if (projectionAnalyser != NULL) {

		projectionAnalyser->controlProjectionEFGCNEvent12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNEvent13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNEvent32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNTotal32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));

		projectionAnalyser->projectionAnalysisUpdate();

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the graphic-Analysis display.					*
 ****************************************************************/

void analysis::projectionAnalysisDraw() {

	if (projectionAnalyser != NULL) {

		projectionAnalyser->controlProjectionEFGCNEvent12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNEvent13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNEvent32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal12EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		projectionAnalyser->controlProjectionEFGCNTotal13EFGCNRange((*space)->getStep(DIM1), (*space)->getStep(DIM3));
		projectionAnalyser->controlProjectionEFGCNTotal32EFGCNRange((*space)->getStep(DIM3), (*space)->getStep(DIM2));
		
		projectionAnalyser->projectionAnalysisDraw();
	
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
/****************************************************************
 * method returns true if the magnetfieldAnalysis				*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMagnetfieldAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->isMagnetfieldAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true if the magnetfield's display is		*
 * enabled.														*
 ****************************************************************/

bool analysis::isMagnetfieldDisplayEnabled() {

	bool returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->isMagnetfieldDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the magnetfield-Analysis display.				*
 ****************************************************************/

void analysis::magnetfieldAnalysisUpdate() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->magnetfieldAnalysisUpdate();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the magnetfield-Analysis display.				*
 ****************************************************************/

void analysis::magnetfieldAnalysisDraw() {

	if (magnetfieldAnalyser != NULL)
		magnetfieldAnalyser->magnetfieldAnalysisDraw();
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method returns true if the magnetfield factor's display	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isMagnetfieldFactorDisplayEnabled() {

	bool returnValue;

	if (magnetfieldFactorAnalyser != NULL)
		returnValue = magnetfieldFactorAnalyser->isMagnetfieldFactorDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the magnetfield factor-Analysis display.		*
 ****************************************************************/

void analysis::magnetfieldFactorAnalysisUpdate() {

	if (magnetfieldFactorAnalyser != NULL) {

		magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());
		magnetfieldFactorAnalyser->magnetfieldFactorAnalysisUpdate();
	
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the magnetfield factor-Analysis display.		*
 ****************************************************************/

void analysis::magnetfieldFactorAnalysisDraw(bitArray preventDraw) {

	if (magnetfieldFactorAnalyser != NULL) {

		magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());
		magnetfieldFactorAnalyser->magnetfieldFactorAnalysisDraw(preventDraw);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method returns true if the prelut range's display		*
 * is enabled.													*
 ****************************************************************/

bool analysis::isPrelutRangeDisplayEnabled() {

	bool returnValue;

	if (prelutRangeLayerAnalyser != NULL)
		returnValue = prelutRangeLayerAnalyser->isPrelutRangeDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method updates the prelut range-Analysis display.			*
 ****************************************************************/

void analysis::prelutRangeAnalysisUpdate() {

	if (prelutRangeLayerAnalyser != NULL) {

		prelutRangeLayerAnalyser->prelutRangeAnalysisUpdate();
	
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method draws the prelut range-Analysis display.				*
 ****************************************************************/

void analysis::prelutRangeAnalysisDraw(bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw) {

	if (prelutRangeLayerAnalyser != NULL) {

		prelutRangeLayerAnalyser->prelutRangeAnalysisDraw(preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the track-propagation-Analysis into			*
 * a root file.													*
 ****************************************************************/

void analysis::trackPropagationEventAnalysisWrite(int eventNumber) {

	if (trackPropagationAnalyser != NULL) {

		if (trackPropagationAnalyser->isTrackPropagationToRootEnabled())
			trackPropagationAnalyser->trackPropagationEventAnalysisWrite(eventNumber);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::trackPropagationTotalAnalysisWrite(int eventNumber) {

	if (trackPropagationAnalyser != NULL) {

		if (trackPropagationAnalyser->isTrackPropagationToRootEnabled())
			trackPropagationAnalyser->trackPropagationTotalAnalysisWrite(eventNumber);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
/****************************************************************
 * method writes the momentumEvent-Analysis for each event into	*
 * a root file.													*
 ****************************************************************/

void analysis::momentumEventAnalysisWrite(int eventNumber) {

	if (momentumAnalyser != NULL) {
		if (momentumAnalyser->isMomentumToRootEnabled())
			momentumAnalyser->momentumEventAnalysisWrite(eventNumber);
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the momentumTotal-Analysis over all events into*
 * a root file.													*
 ****************************************************************/

void analysis::momentumTotalAnalysisWrite(int eventNumber) {

	if (momentumAnalyser != NULL) {
		if (momentumAnalyser->isMomentumToRootEnabled())
			momentumAnalyser->momentumTotalAnalysisWrite(eventNumber);
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the projectionEvent-Analysis for each event	*
 * into a root file.											*
 ****************************************************************/

void analysis::projectionEventAnalysisWrite(int eventNumber) {

	if (projectionAnalyser != NULL) {
		if (projectionAnalyser->isProjectionToRootEnabled())
			projectionAnalyser->projectionEventAnalysisWrite(eventNumber);
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the projectionTotal-Analysis over all events	*
 * into a root file.											*
 ****************************************************************/

void analysis::projectionTotalAnalysisWrite(int eventNumber) {

	if (projectionAnalyser != NULL) {
		if (projectionAnalyser->isProjectionToRootEnabled())
			projectionAnalyser->projectionTotalAnalysisWrite(eventNumber);
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the magnetfield-Analysis into					*
 * a root file.													*
 ****************************************************************/

void analysis::magnetfieldAnalysisWrite() {

	if (magnetfieldAnalyser != NULL) {
		if (magnetfieldAnalyser->isMagnetfieldToRootEnabled())
			magnetfieldAnalyser->magnetfieldAnalysisWrite();
	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the magnetfieldFactor-Analysis into			*
 * a root file.													*
 ****************************************************************/

void analysis::magnetfieldFactorAnalysisWrite(int eventNumber) {

	if (magnetfieldFactorAnalyser != NULL) {

		if (magnetfieldFactorAnalyser->isMagnetfieldFactorToRootEnabled()) {
			magnetfieldFactorAnalyser->set((*eventData)->getNumberOfActiveStations());
			magnetfieldFactorAnalyser->magnetfieldFactorAnalysisWrite(eventNumber);
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the prelut range-Analysis into					*
 * a root file.													*
 ****************************************************************/

void analysis::prelutRangeAnalysisWrite(int eventNumber) {

	if (prelutRangeLayerAnalyser != NULL) {

		if (prelutRangeLayerAnalyser->isPrelutRangeToRootEnabled())
			prelutRangeLayerAnalyser->prelutRangeAnalysisWrite(eventNumber);

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method initializes the momentumEFGCEventAnalysis.			*
 ****************************************************************/

void analysis::initMomentumEventAnalysis(bool enable) {

	if (momentumAnalyser != NULL)
		momentumAnalyser->initMomentumEventAnalysis(enable);

}

/****************************************************************
 * method initializes the momentumEFGCTotalAnalysis.			*
 ****************************************************************/

void analysis::initMomentumTotalAnalysis(bool enable) {

	if (momentumAnalyser != NULL)
		momentumAnalyser->initMomentumTotalAnalysis(enable);

}

/****************************************************************
 * method initializes the projectionEFGCNEventAnalysis.			*
 ****************************************************************/

void analysis::initProjectionEventAnalysis(bool enable) {

	if (projectionAnalyser != NULL)
		projectionAnalyser->initProjectionEventAnalysis(enable);

}

/****************************************************************
 * method initializes the projectionEFGCNTotalAnalysis.			*
 ****************************************************************/

void analysis::initProjectionTotalAnalysis(bool enable) {

	if (projectionAnalyser != NULL)
		projectionAnalyser->initProjectionTotalAnalysis(enable);

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumEvent-Analysis.										*
 ****************************************************************/

std::string analysis::getMomentumEventAnalysis() {

	std::string returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->getMomentumEFGCEventAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumTotal-Analysis.										*
 ****************************************************************/

std::string analysis::getMomentumTotalAnalysis() {

	std::string returnValue;

	if (momentumAnalyser != NULL)
		returnValue = momentumAnalyser->getMomentumEFGCTotalAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * magnetfield-Analysis.										*
 ****************************************************************/

std::string analysis::getMagnetfieldAnalysis() {

	std::string returnValue;

	if (magnetfieldAnalyser != NULL)
		returnValue = magnetfieldAnalyser->getMagnetfieldAnalysis();
	else
		returnValue = "";

	return returnValue;

}

/****************************************************************
 * method returns if the created histogram layer can			*
 * be written into the file.									*
 ****************************************************************/

bool analysis::isWritingCreatedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isCreatedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the encoded histogram layer can			*
 * be written into the file.									*
 ****************************************************************/

bool analysis::isWritingEncodedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isEncodedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the filtered histogram layer can			*
 * be written into the file.									*
 ****************************************************************/

bool analysis::isWritingFilteredHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isFilteredHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if just one created histogram layer can		*
 * be accessed.													*
 ****************************************************************/

bool analysis::isWritingJustOneCreatedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isJustOneCreatedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if just one encoded histogram layer can		*
 * be accessed.													*
 ****************************************************************/

bool analysis::isWritingJustOneEncodedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isJustOneEncodedHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if just one filtered histogram layer can		*
 * be accessed.													*
 ****************************************************************/

bool analysis::isWritingJustOneFilteredHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isJustOneFilteredHistogramToRootEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the created histogram layer can			*
 * be shown.													*
 ****************************************************************/

bool analysis::isShowingCreatedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isCreatedHistogramShowingEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the encoded histogram layer can			*
 * be shown.													*
 ****************************************************************/

bool analysis::isShowingEncodedHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isEncodedHistogramShowingEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the filtered histogram layer can			*
 * be shown.													*
 ****************************************************************/

bool analysis::isShowingFilteredHistogramLayerEnabled() {

	bool returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->isFilteredHistogramShowingEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns the index of the histogram layer			*
 * which should be accessed.									*
 ****************************************************************/

unsigned short analysis::getWritingHistogramLayerIndex() {

	unsigned short returnValue;

	if (histogramAnalyser != NULL)
		returnValue = histogramAnalyser->getHistogramLayerIndexToRoot();
	else
		returnValue = 0;

	return returnValue;

}

/****************************************************************
 * method writes the actual created histogram layer as the		*
 * layer 'layer' into the file.									*
 ****************************************************************/

void analysis::createdHistogramLayerWrite(unsigned short layer) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isCreatedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->createdHistogramLayerWrite((*eventData)->getEventNumber(), layer);
	
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the actual encoded histogram layer as the		*
 * layer 'layer' into the file.									*
 ****************************************************************/

void analysis::encodedHistogramLayerWrite(unsigned short layer) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isEncodedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->encodedHistogramLayerWrite((*eventData)->getEventNumber(), layer);
	
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * method writes the actual filtered histogram layer as the		*
 * layer 'layer' into the file.									*
 ****************************************************************/

void analysis::filteredHistogramLayerWrite(unsigned short layer) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isFilteredHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->filteredHistogramLayerWrite((*eventData)->getEventNumber(), layer);
	
		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the histogram after the filtering			*
 ****************************************************************/

void analysis::createdHistogramLayerShow(unsigned short layer, std::string drawOption) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isCreatedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->createdHistogramLayerShow((*eventData)->getEventNumber(), layer, drawOption);
	
		}
		else {

			cannotShowHistogramWithoutWritingToRootWarningMsg* cannotShowHistogramWithoutWritingToRoot = new cannotShowHistogramWithoutWritingToRootWarningMsg();
			cannotShowHistogramWithoutWritingToRoot->warningMsg();
			if(cannotShowHistogramWithoutWritingToRoot != NULL) {
				delete cannotShowHistogramWithoutWritingToRoot;
				cannotShowHistogramWithoutWritingToRoot = NULL;
			}

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::createdHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (eventNumber == (*eventData)->getEventNumber())
		createdHistogramLayerShow(layer, drawOption);

}

/****************************************************************
 * This method shows the histogram after the creation			*
 ****************************************************************/

void analysis::encodedHistogramLayerShow(unsigned short layer, std::string drawOption) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isEncodedHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->encodedHistogramLayerShow((*eventData)->getEventNumber(), layer, drawOption);
	
		}
		else {

			cannotShowHistogramWithoutWritingToRootWarningMsg* cannotShowHistogramWithoutWritingToRoot = new cannotShowHistogramWithoutWritingToRootWarningMsg();
			cannotShowHistogramWithoutWritingToRoot->warningMsg();
			if(cannotShowHistogramWithoutWritingToRoot != NULL) {
				delete cannotShowHistogramWithoutWritingToRoot;
				cannotShowHistogramWithoutWritingToRoot = NULL;
			}

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::encodedHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (eventNumber == (*eventData)->getEventNumber())
		encodedHistogramLayerShow(layer, drawOption);

}

/****************************************************************
 * This method shows the histogram after the encoding			*
 ****************************************************************/

void analysis::filteredHistogramLayerShow(unsigned short layer, std::string drawOption) {

	if (histogramAnalyser != NULL) {

		if (histogramAnalyser->isFilteredHistogramToRootEnabled()) {

			if (eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);
			if (*eventData == NULL)
				throw cannotAccessEventDataError(ANALYSISLIB);

			histogramAnalyser->filteredHistogramLayerShow((*eventData)->getEventNumber(), layer, drawOption);
	
		}
		else {

			cannotShowHistogramWithoutWritingToRootWarningMsg* cannotShowHistogramWithoutWritingToRoot = new cannotShowHistogramWithoutWritingToRootWarningMsg();
			cannotShowHistogramWithoutWritingToRoot->warningMsg();
			if(cannotShowHistogramWithoutWritingToRoot != NULL) {
				delete cannotShowHistogramWithoutWritingToRoot;
				cannotShowHistogramWithoutWritingToRoot = NULL;
			}

		}

	}
	else {

		displayAnalyserNotFoundWarningMsg* displayAnalyserNotFound = new displayAnalyserNotFoundWarningMsg();
		displayAnalyserNotFound->warningMsg();
		if(displayAnalyserNotFound != NULL) {
			delete displayAnalyserNotFound;
			displayAnalyserNotFound = NULL;
		}
	
	}

}
void analysis::filteredHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if (eventNumber == (*eventData)->getEventNumber())
		filteredHistogramLayerShow(layer, drawOption);

}

/****************************************************************
 * method returns the number of active detector stations		*
 ****************************************************************/

unsigned int analysis::getNumberOfActiveStations() {

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	return (*eventData)->getDetector().getNumberOfActiveStations();

}

/****************************************************************
 * method initializes the hardwareTracksPerColumn-Analysis.		*
 ****************************************************************/

void analysis::initHardwareTracksPerColumnAnalysis(bool enable) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (enable) {

		if (hardwareAnalyser == NULL)
			hardwareAnalyser = new hardwareAnalysis();

		hardwareAnalyser->initHardwareHistogramDimensions((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		hardwareAnalyser->initHardwareTracksPerColumnAnalysis(enable);

	}
	else {

		if (hardwareAnalyser != NULL) {

			hardwareAnalyser->initHardwareTracksPerColumnAnalysis(enable);

			if (!hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled() && !hardwareAnalyser->isHitReadoutDistributionAnalysisEnabled()) {
				delete hardwareAnalyser;
				hardwareAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the hardwareTracksPerRow-Analysis.		*
 ****************************************************************/

void analysis::initHardwareTracksPerRowAnalysis(bool enable) {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	if (enable) {

		if (hardwareAnalyser == NULL)
			hardwareAnalyser = new hardwareAnalysis();

		hardwareAnalyser->initHardwareHistogramDimensions((*space)->getStep(DIM1), (*space)->getStep(DIM2));
		hardwareAnalyser->initHardwareTracksPerRowAnalysis(enable);

	}
	else {

		if (hardwareAnalyser != NULL) {

			hardwareAnalyser->initHardwareTracksPerRowAnalysis(enable);

			if (!hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllRowsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled() && !hardwareAnalyser->isHitReadoutDistributionAnalysisEnabled()) {
				delete hardwareAnalyser;
				hardwareAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the hardwareTracksPerLayer-Analysis.		*
 ****************************************************************/

void analysis::initHardwareTracksPerLayerAnalysis(bool enable) {

	if ((tracks == NULL) || (*tracks == NULL))
		throw cannotAccessTrackDataError(ANALYSISLIB);

	if (enable) {

		if (hardwareAnalyser == NULL)
			hardwareAnalyser = new hardwareAnalysis();

		hardwareAnalyser->initHardwareTracksPerLayerAnalysis((*tracks)->getNumberOfLayers(), enable);

	}
	else {

		if (hardwareAnalyser != NULL) {

			hardwareAnalyser->initHardwareTracksPerLayerAnalysis((*tracks)->getNumberOfLayers(), enable);

			if (!hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllRowsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled() && !hardwareAnalyser->isHitReadoutDistributionAnalysisEnabled()) {
				delete hardwareAnalyser;
				hardwareAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the hardwareHistogramLayerDistribution-	*
 * Analysis.													*
 ****************************************************************/

void analysis::initHardwareHitReadoutDistributionAnalysis(bool enable) {

	if (enable) {

		if (hardwareAnalyser == NULL)
			hardwareAnalyser = new hardwareAnalysis();

		hardwareAnalyser->initHardwareHitReadoutDistributionAnalysis((*tracks)->getNumberOfLayers(), enable);

	}
	else {

		if (hardwareAnalyser != NULL) {

			hardwareAnalyser->initHardwareHitReadoutDistributionAnalysis((*tracks)->getNumberOfLayers(), enable);

			if (!hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllRowsAnalysisEnabled() && !hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled() && !hardwareAnalyser->isHitReadoutDistributionAnalysisEnabled()) {
				delete hardwareAnalyser;
				hardwareAnalyser = NULL;
			}

		}

	}

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all columns of the histogram layers is				*
 * enabled or not												*
 ****************************************************************/

bool analysis::isNumberOfTracksInAllColumnsAnalysisEnabled() {

	bool returnValue;

	if (hardwareAnalyser != NULL)
		returnValue = hardwareAnalyser->isNumberOfTracksInAllColumnsAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all rows of the histogram layers is				*
 * enabled or not												*
 ****************************************************************/

bool analysis::isNumberOfTracksInAllRowsAnalysisEnabled() {

	bool returnValue;

	if (hardwareAnalyser != NULL)
		returnValue = hardwareAnalyser->isNumberOfTracksInAllRowsAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all histogram layers is enabled or not				*
 ****************************************************************/

bool analysis::isNumberOfTracksInAllLayersAnalysisEnabled() {

	bool returnValue;

	if (hardwareAnalyser != NULL)
		returnValue = hardwareAnalyser->isNumberOfTracksInAllLayersAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns if the analysis for the histogram				*
 * layer distribution is enabled or not							*
 ****************************************************************/

bool analysis::isHitReadoutDistributionAnalysisEnabled() {

	bool returnValue;

	if (hardwareAnalyser != NULL)
		returnValue = hardwareAnalyser->isHitReadoutDistributionAnalysisEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method evaluates the minimal, the maximal and			*
 * the average number of tracks in all columns of the histogram.*
 ****************************************************************/

void analysis::evaluateNumberOfTracksInAllColumns(std::streambuf* terminal) {

	if (hardwareAnalyser != NULL)
		hardwareAnalyser->evaluateNumberOfTracksInAllColumns(*tracks, terminal);
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method evaluates the minimal, the maximal and			*
 * the average number of tracks in all rows of the histogram.	*
 ****************************************************************/

void analysis::evaluateNumberOfTracksInAllRows(std::streambuf* terminal) {

	if (hardwareAnalyser != NULL)
		hardwareAnalyser->evaluateNumberOfTracksInAllRows(*tracks, terminal);
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}
/****************************************************************
 * This method evaluates the distribution of the number of		*
 * tracks in all layers of the histogram.						*
 ****************************************************************/

void analysis::evaluateNumberOfTracksInAllLayers(std::streambuf* terminal) {

	if (hardwareAnalyser != NULL)
		hardwareAnalyser->evaluateNumberOfTracksInAllLayers(*tracks, terminal);
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * The method evaluates the distribution how often a hit must	*
 * be inserted in a layer.										*
 ****************************************************************/

void analysis::evaluateHitReadoutDistribution(histogramData* histogram, std::streambuf* terminal) {

	if (hardwareAnalyser != NULL)
		hardwareAnalyser->evaluateHitReadoutDistribution(histogram, terminal);
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the minimal, the maximal and				*
 * the average number of tracks in all columns of the histogram.*
 ****************************************************************/

void analysis::showNumberOfTracksInAllColumns() {

	if (hardwareAnalyser != NULL) {

		if (analysisResultWarnings & TRACKSPERCOLUMNDISTRIBUTION) {

			numberOfMinimalTracksInAllColumnsWarningMsg* numberOfMinimalTracksInAllColumns = new numberOfMinimalTracksInAllColumnsWarningMsg(hardwareAnalyser->getMinimalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			numberOfMinimalTracksInAllColumns->warningMsg();
			if(numberOfMinimalTracksInAllColumns != NULL) {
				delete numberOfMinimalTracksInAllColumns;
				numberOfMinimalTracksInAllColumns = NULL;
			}

			numberOfAverageTracksInAllColumnsWarningMsg* numberOfAverageTracksInAllColumns = new numberOfAverageTracksInAllColumnsWarningMsg(hardwareAnalyser->getAverageNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries(), hardwareAnalyser->getNumberOfColumnAnalysis());
			numberOfAverageTracksInAllColumns->warningMsg();
			if(numberOfAverageTracksInAllColumns != NULL) {
				delete numberOfAverageTracksInAllColumns;
				numberOfAverageTracksInAllColumns = NULL;
			}

			numberOfMaximalTracksInAllColumnsWarningMsg* numberOfMaximalTracksInAllColumns = new numberOfMaximalTracksInAllColumnsWarningMsg(hardwareAnalyser->getMaximalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			numberOfMaximalTracksInAllColumns->warningMsg();
			if(numberOfMaximalTracksInAllColumns != NULL) {
				delete numberOfMaximalTracksInAllColumns;
				numberOfMaximalTracksInAllColumns = NULL;
			}

		}

		if (analysisResultWarnings & FIFOSPERCOLUMNDISTRIBUTION) {

			sizeOfColumnSeparatorFifosWarningMsg* sizeOfColumnSeparatorFifos = new sizeOfColumnSeparatorFifosWarningMsg(hardwareAnalyser->getNumberOfFifoSizesInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			sizeOfColumnSeparatorFifos->warningMsg();
			if(sizeOfColumnSeparatorFifos != NULL) {
				delete sizeOfColumnSeparatorFifos;
				sizeOfColumnSeparatorFifos = NULL;
			}

		}

		if (analysisResultDisplays & TRACKSPERCOLUMNDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addMinimalTrackColumnDistribution(hardwareAnalyser->getMinimalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());
			showAnalyser->addAverageTrackColumnDistribution(hardwareAnalyser->getAverageNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries(), hardwareAnalyser->getNumberOfColumnAnalysis());
			showAnalyser->addMaximalTrackColumnDistribution(hardwareAnalyser->getMaximalNumberOfTracksInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());

		}

		if (analysisResultDisplays & FIFOSPERCOLUMNDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addFifoForColumnDistribution(hardwareAnalyser->getNumberOfFifoSizesInAllColumnsDistribution(), hardwareAnalyser->getNumberOfTracksInAllColumnEntries());

		}

	}
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the minimal, the maximal and				*
 * the average number of tracks in all rows of the histogram.	*
 ****************************************************************/

void analysis::showNumberOfTracksInAllRows() {

	if (hardwareAnalyser != NULL) {

		if (analysisResultWarnings & TRACKSPERROWDISTRIBUTION) {

			numberOfMinimalTracksInAllRowsWarningMsg* numberOfMinimalTracksInAllRows = new numberOfMinimalTracksInAllRowsWarningMsg(hardwareAnalyser->getMinimalNumberOfTracksInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries());
			numberOfMinimalTracksInAllRows->warningMsg();
			if(numberOfMinimalTracksInAllRows != NULL) {
				delete numberOfMinimalTracksInAllRows;
				numberOfMinimalTracksInAllRows = NULL;
			}

			numberOfAverageTracksInAllRowsWarningMsg* numberOfAverageTracksInAllRows = new numberOfAverageTracksInAllRowsWarningMsg(hardwareAnalyser->getAverageNumberOfTracksInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries(), hardwareAnalyser->getNumberOfRowAnalysis());
			numberOfAverageTracksInAllRows->warningMsg();
			if(numberOfAverageTracksInAllRows != NULL) {
				delete numberOfAverageTracksInAllRows;
				numberOfAverageTracksInAllRows = NULL;
			}

			numberOfMaximalTracksInAllRowsWarningMsg* numberOfMaximalTracksInAllRows = new numberOfMaximalTracksInAllRowsWarningMsg(hardwareAnalyser->getMaximalNumberOfTracksInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries());
			numberOfMaximalTracksInAllRows->warningMsg();
			if(numberOfMaximalTracksInAllRows != NULL) {
				delete numberOfMaximalTracksInAllRows;
				numberOfMaximalTracksInAllRows = NULL;
			}

		}

		if (analysisResultWarnings & FIFOSPERROWDISTRIBUTION) {

			sizeOfRowSeparatorFifosWarningMsg* sizeOfRowSeparatorFifos = new sizeOfRowSeparatorFifosWarningMsg(hardwareAnalyser->getNumberOfFifoSizesInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries());
			sizeOfRowSeparatorFifos->warningMsg();
			if(sizeOfRowSeparatorFifos != NULL) {
				delete sizeOfRowSeparatorFifos;
				sizeOfRowSeparatorFifos = NULL;
			}

		}

		if (analysisResultDisplays & TRACKSPERROWDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addMinimalTrackRowDistribution(hardwareAnalyser->getMinimalNumberOfTracksInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries());
			showAnalyser->addAverageTrackRowDistribution(hardwareAnalyser->getAverageNumberOfTracksInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries(), hardwareAnalyser->getNumberOfRowAnalysis());
			showAnalyser->addMaximalTrackRowDistribution(hardwareAnalyser->getMaximalNumberOfTracksInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries());

		}

		if (analysisResultDisplays & FIFOSPERROWDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addFifoForRowDistribution(hardwareAnalyser->getNumberOfFifoSizesInAllRowsDistribution(), hardwareAnalyser->getNumberOfTracksInAllRowEntries());

		}

	}
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the distribution of the number of			*
 * tracks in all layers of the histogram.						*
 ****************************************************************/

void analysis::showNumberOfTracksInAllLayers() {

	if (hardwareAnalyser != NULL) {

		if (analysisResultWarnings & TRACKSPERLAYERDISTRIBUTION) {

			numberOfTracksInAllLayersWarningMsg* numberOfTracksInAllLayers = new numberOfTracksInAllLayersWarningMsg(hardwareAnalyser->getNumberOfTracksPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnalysis());
			numberOfTracksInAllLayers->warningMsg();
			if(numberOfTracksInAllLayers != NULL) {
				delete numberOfTracksInAllLayers;
				numberOfTracksInAllLayers = NULL;
			}

			numberOfTrackDensitiesInAllLayersWarningMsg* numberOfTrackDensitiesInAllLayers = new numberOfTrackDensitiesInAllLayersWarningMsg(hardwareAnalyser->getNumberOfTrackDensitiesPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnalysis());
			numberOfTrackDensitiesInAllLayers->warningMsg();
			if(numberOfTrackDensitiesInAllLayers != NULL) {
				delete numberOfTrackDensitiesInAllLayers;
				numberOfTrackDensitiesInAllLayers = NULL;
			}

		}

		if (analysisResultDisplays & TRACKSPERLAYERDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addTrackLayerDistribution(hardwareAnalyser->getNumberOfTracksPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnalysis());
			showAnalyser->addTrackDensityLayerDistribution(hardwareAnalyser->getNumberOfTrackDensitiesPerLayerDistribution(), hardwareAnalyser->getNumberOfTracksInAllLayersEntries(), hardwareAnalyser->getNumberOfLayerAnalysis());

		}

	}
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method shows the distribution of the histogram			*
 * layers for the hits.											*
 ****************************************************************/

void analysis::showHitReadoutDistribution(bool readoutColumnsInParallel) {

	if (hardwareAnalyser != NULL) {

		if (analysisResultWarnings & HITREADOUTDISTRIBUTION) {

			hitReadoutDistributionWarningMsg* hitReadoutDistribution = new hitReadoutDistributionWarningMsg(hardwareAnalyser->getHitReadoutDistribution(), hardwareAnalyser->getNumberOfHitReadoutDistributionEntries(), hardwareAnalyser->getNumberOfHitReadoutDistributionAnalysis());
			hitReadoutDistribution->warningMsg();
			if(hitReadoutDistribution != NULL) {
				delete hitReadoutDistribution;
				hitReadoutDistribution = NULL;
			}

		}

		if (analysisResultWarnings & HITREADOUTMEANDISTRIBUTION) {

			hitReadoutMeanDistributionWarningMsg* hitReadoutMeanDistribution = new hitReadoutMeanDistributionWarningMsg(hardwareAnalyser->getHitReadoutDistribution(), hardwareAnalyser->getNumberOfHitReadoutDistributionEntries(), hardwareAnalyser->getNumberOfHitReadoutDistributionAnalysis());
			hitReadoutMeanDistribution->warningMsg();
			if(hitReadoutMeanDistribution != NULL) {
				delete hitReadoutMeanDistribution;
				hitReadoutMeanDistribution = NULL;
			}

		}

		if (analysisResultWarnings & FPGAHISTOGRAMPROCESSINGTIMING) {

			if ((space == NULL) || (*space == NULL))
				throw cannotAccessHistogramSpaceError(ANALYSISLIB);

			fpgaHistogramProcessingTimeDistributionWarningMsg* fpgaHistogramProcessingTimeDistribution = new fpgaHistogramProcessingTimeDistributionWarningMsg(hardwareAnalyser->getHitReadoutDistribution(), hardwareAnalyser->getNumberOfHitReadoutDistributionEntries(), hardwareAnalyser->getNumberOfHitReadoutDistributionAnalysis(), readoutColumnsInParallel, (*space)->getStep(DIM1), (*space)->getStep(DIM2));
			fpgaHistogramProcessingTimeDistribution->warningMsg();
			if(fpgaHistogramProcessingTimeDistribution != NULL) {
				delete fpgaHistogramProcessingTimeDistribution;
				fpgaHistogramProcessingTimeDistribution = NULL;
			}

		}

		if (analysisResultDisplays & HITREADOUTDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addHitReadoutDistribution(hardwareAnalyser->getHitReadoutDistribution(), hardwareAnalyser->getNumberOfHitReadoutDistributionEntries(), hardwareAnalyser->getNumberOfHitReadoutDistributionAnalysis());

		}

		if (analysisResultDisplays & HITREADOUTMEANDISTRIBUTION) {

			initializeShowAnalysis();

			showAnalyser->addHitReadoutMeanDistribution(hardwareAnalyser->getHitReadoutDistribution(), hardwareAnalyser->getNumberOfHitReadoutDistributionEntries(), hardwareAnalyser->getNumberOfHitReadoutDistributionAnalysis());

		}

		if (analysisResultDisplays & FPGAHISTOGRAMPROCESSINGTIMING) {

			if ((space == NULL) || (*space == NULL))
				throw cannotAccessHistogramSpaceError(ANALYSISLIB);

			initializeShowAnalysis();

			showAnalyser->addFpgaHistogramProcessingTimeDistribution(hardwareAnalyser->getHitReadoutDistribution(), hardwareAnalyser->getNumberOfHitReadoutDistributionEntries(), hardwareAnalyser->getNumberOfHitReadoutDistributionAnalysis(), readoutColumnsInParallel, (*space)->getStep(DIM1), (*space)->getStep(DIM2));

		}

	}
	else {

		hardwareAnalyserNotFoundWarningMsg* hardwareAnalyserNotFound = new hardwareAnalyserNotFoundWarningMsg();
		hardwareAnalyserNotFound->warningMsg();
		if(hardwareAnalyserNotFound != NULL) {
			delete hardwareAnalyserNotFound;
			hardwareAnalyserNotFound = NULL;
		}
	
	}

}

/****************************************************************
 * This method resets the totalAnalysis object with				*
 * its actual results.											*
 ****************************************************************/

void analysis::resetTotalAnalysis() {

	initializeTotalAnalysis();

	totalAnalyser->reset();

}

/****************************************************************
 * This method adds the actual number of Hough transform		*
 * corrections for the total multi event summary to the			*
 * totalAnalysis object.										*
 ****************************************************************/

void analysis::addNumberOfHoughTransformCorrections(unsigned long numberOfCorrections, unsigned long numberOfCoordCorrections) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfCorrections(numberOfCorrections);
	totalAnalyser->addNumberOfCoordCorrections(numberOfCoordCorrections);

}

/****************************************************************
 * This method adds the actual number of tracks which cannot be	*
 * found.														*
 ****************************************************************/

void analysis::addNumberOfTracksWhichCannotBeFound(unsigned long numberOfTracksWhichCannotBeFound) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfTracksWhichCannotBeFound(numberOfTracksWhichCannotBeFound);

}

/****************************************************************
 * This method adds the actual number of tracks which have an	*
 * acceptable signature belonging to prelut.					*
 ****************************************************************/

void analysis::addNumberOfTracksWithGoodPrelutSignature(unsigned long numberOfTracksWithGoodPrelutSignature) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfTracksWithGoodPrelutSignature(numberOfTracksWithGoodPrelutSignature);

}

/****************************************************************
 * This method adds the actual number of tracks which have an	*
 * acceptable signature belonging to lut.						*
 ****************************************************************/

void analysis::addNumberOfTracksWithGoodLutSignature(unsigned long numberOfTracksWithGoodLutSignature) {

	initializeTotalAnalysis();

	totalAnalyser->addNumberOfTracksWithGoodLutSignature(numberOfTracksWithGoodLutSignature);

}

/****************************************************************
 * This method adds the actual signature to the					*
 * acceptable signatures belonging to prelut.					*
 ****************************************************************/

void analysis::addGoodPrelutSignature(bitArray goodPrelutSignature, bitArray classification) {

	initializeTotalAnalysis();

	totalAnalyser->addGoodPrelutSignature(goodPrelutSignature, classification);

}

/****************************************************************
 * This method adds the actual signature to the					*
 * acceptable signatures belonging to lut.						*
 ****************************************************************/

void analysis::addGoodLutSignature(bitArray goodLutSignature, bitArray classification) {

	initializeTotalAnalysis();

	totalAnalyser->addGoodLutSignature(goodLutSignature, classification);

}

/****************************************************************
 * This method prints the actual results of the totalAnalysis.	*
 ****************************************************************/

void analysis::printTotalAnalysis() {

	if (totalAnalyser != NULL) {

		if (analysisResultWarnings & LUTCORRECTIONCOUNTER) {

			totalAnalyser->printCorrectionCounter();

		}

		if (analysisResultWarnings & NUMBEROFTRACKSWHICHCANNOTBEFOUND) {

			totalAnalyser->printNumberOfTracksWhichCannotBeFound();

		}

		if (analysisResultWarnings & GOODSIGNATURES) {

			totalAnalyser->printNumberOfTracksWithGoodPrelutSignature();
			totalAnalyser->printNumberOfTracksWithGoodLutSignature();
			totalAnalyser->printNumberOfGoodPrelutSignatures();
			totalAnalyser->printNumberOfGoodLutSignatures();

		}

		if (analysisResultWarnings & GOODSIGNATURETABLES) {

			totalAnalyser->printGoodPrelutSignatures();
			totalAnalyser->printGoodLutSignatures();

		}

		if (analysisResultWarnings & USEDSIGNATURETABLES) {

			usedTablesWarningMsg* usedTables = new usedTablesWarningMsg((*ratings)->getInfo());
			usedTables->warningMsg();
			if(usedTables != NULL) {
				delete usedTables;
				usedTables = NULL;
			}

		}

	}

}

/****************************************************************
 * This method displays all arrays of the totalAnalysis object.	*
 ****************************************************************/

void analysis::displayTotalAnalysis() {

	if (analysisResultDisplays & LUTCORRECTIONCOUNTER) {

		initializeShowAnalysis();
	
		showAnalyser->addDistribution((double*)correctionCounterSteps, (double*)correctionCounterValues, numberOfCorrectionCounters, CORRECTIONCOUNTERNAME, CORRECTIONCOUNTERTITLE, CORRECTIONCOUNTERXAXITITLE, CORRECTIONCOUNTERYAXITITLE);

	}

	if (analysisResultDisplays & NUMBEROFTRACKSWHICHCANNOTBEFOUND) {

		initializeShowAnalysis();

		showAnalyser->addDistribution((double*)tracksWhichAreNotFindableSteps, (double*)tracksWhichAreNotFindableValues, numberOfTracksWhichArenotFindable, NOTFINDABLETRACKSNAME, NOTFINDABLETRACKSTITLE, NOTFINDABLETRACKSXAXITITLE, NOTFINDABLETRACKSYAXITITLE);
	
	}

	if (analysisResultDisplays & GOODSIGNATURES) {

		initializeShowAnalysis();

		showAnalyser->addDistribution((double*)tracksWithAGoodPrelutSignatureSteps, (double*)tracksWithAGoodPrelutSignatureValues, numberOfTracksWithAGoodPrelutSignature, TRACKSWITHGOODPRELUTSIGNATURENAME, TRACKSWITHGOODPRELUTSIGNATURETITLE, TRACKSWITHGOODPRELUTSIGNATUREXAXITITLE, TRACKSWITHGOODPRELUTSIGNATUREYAXITITLE);
		showAnalyser->addDistribution((double*)tracksWithAGoodLutSignatureSteps, (double*)tracksWithAGoodLutSignatureValues, numberOfTracksWithAGoodLutSignature, TRACKSWITHGOODLUTSIGNATURENAME, TRACKSWITHGOODLUTSIGNATURETITLE, TRACKSWITHGOODLUTSIGNATUREXAXITITLE, TRACKSWITHGOODLUTSIGNATUREYAXITITLE);
		showAnalyser->addDistribution((double*)goodPrelutSignaturesSteps, (double*)goodPrelutSignaturesValues, numberOfGoodPrelutSignatures, GOODPRELUTSIGNATURESNAME, GOODPRELUTSIGNATURESTITLE, GOODPRELUTSIGNATURESXAXITITLE, GOODPRELUTSIGNATURESYAXITITLE);
		showAnalyser->addDistribution((double*)goodLutSignaturesSteps, (double*)goodLutSignaturesValues, numberOfGoodLutSignatures, GOODLUTSIGNATURESNAME, GOODLUTSIGNATURESTITLE, GOODLUTSIGNATURESXAXITITLE, GOODLUTSIGNATURESYAXITITLE);

	}

	if (analysisResultDisplays & GOODSIGNATURETABLES) {

		initializeShowAnalysis();
		
		if (totalAnalyser != NULL) {

			showAnalyser->addTable(totalAnalyser->getGoodPrelutSignatureTable(), GOODPRELUTSIGNATURETABLENAME, GOODPRELUTSIGNATURETABLETITLE, GOODPRELUTSIGNATURETABLEXAXITITLE, GOODPRELUTSIGNATURETABLEYAXITITLE);
			showAnalyser->addTable(totalAnalyser->getGoodLutSignatureTable(), GOODLUTSIGNATURETABLENAME, GOODLUTSIGNATURETABLETITLE, GOODLUTSIGNATURETABLEXAXITITLE, GOODLUTSIGNATURETABLEYAXITITLE);

		}

	}

	if (analysisResultDisplays & USEDSIGNATURETABLES) {

		initializeShowAnalysis();


		if ((ratings != NULL) && (*ratings != NULL)) {

			showAnalyser->addTable((*ratings)->getCodingTable(), CODINGTABLENAME, CODINGTABLETITLE, CODINGTABLEXAXITITLE, CODINGTABLEYAXITITLE);
			showAnalyser->addTable((*ratings)->getGradingPTable(), GRADINGPTABLENAME, GRADINGPTABLETITLE, GRADINGPTABLEXAXITITLE, GRADINGPTABLEYAXITITLE);
			showAnalyser->addTable((*ratings)->getGradingRTable(), GRADINGRTABLENAME, GRADINGRTABLETITLE, GRADINGRTABLEXAXITITLE, GRADINGRTABLEYAXITITLE);

		}
		else
			throw cannotAccessTablesError(ANALYSISLIB);

	}

	if (analysisResultDisplays & HOUGHPICTURES) {

		if (pictures == NULL) {

			pictures = new houghPictures();

			pictures->draw();

		}

	}

}

/****************************************************************
 * method returns true if the MCTrack distribution-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool analysis::isMCTrackVisualizationAnalysisEnabled() {

	return ((analysisMoreResultDisplays & MCTRACKVISUALIZATION) | (analysisMoreResultWarnings & MCTRACKVISUALIZATION));

}

/****************************************************************
 * This method displays all MCTracks with corresponding hits.	*
 ****************************************************************/

std::string analysis::displayMCTracks(int trackIndex) {

	std::string            returnValue;
	trackfinderInputHit**  actualDisplayTrack;
	unsigned int           j;
	trackfinderInputTrack* track;
	trackfinderInputHit*   hit;
	double                 timeOfFlight;

	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	returnValue.clear();

	/* allocate memory for one hit per detector station */
	actualDisplayTrack = (trackfinderInputHit**)calloc((*eventData)->getNumberOfStations(), sizeof(trackfinderInputHit*));
	if (actualDisplayTrack == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	/* initialize visual analysis object */
	if (analysisMoreResultDisplays & MCTRACKVISUALIZATION) {
	
		if (mcTrackVisualAnalyser == NULL) {

			mcTrackVisualAnalyser = new visualAnalysis();
			if (mcTrackVisualAnalyser == NULL)
				throw memoryAllocationError(ANALYSISLIB);

		}
		else {

			mcTrackVisualAnalyser->reset();

		}

	}

	for (int i = 0; i < (*eventData)->getNumberOfTracks(); i++) {

		track = (*eventData)->getTrackByOrder(i);
		if (track == NULL)
			throw cannotAccessTrackDataError(ANALYSISLIB);

		if ((trackIndex != INVALIDTRACKINDEX) && (trackIndex != track->getTrackIndex()))
			continue;

		/* reset the hit pointers for the actual track which should be displayed */
		for (j = 0; j < (*eventData)->getNumberOfStations(); j++)
			actualDisplayTrack[j] = NULL;

		track->resetHitPointer();
		for (int k = 0; k < track->getNumberOfHits(); k++) {

			hit = track->getHit();
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			if (hit->getStation() != NULL) {

				if (actualDisplayTrack[hit->getStation()->getIndex()] == NULL) {
					
					actualDisplayTrack[hit->getStation()->getIndex()] = hit;

				}

			}

		}

		/* add the actual track to the display */
		if (analysisMoreResultDisplays & MCTRACKVISUALIZATION) {

			if ((trackIndex == INVALIDTRACKINDEX) || (trackIndex == track->getTrackIndex())) {

				mcTrackVisualAnalyser->addActualTrack(track->getTrackIndex(), track->getMomX(), track->getMomY(), track->getMomZ(), -1, -1, -1, track->getPdgCode());
			
				for (j = 0; j < (*eventData)->getNumberOfStations(); j++) {

					if (actualDisplayTrack[j] != NULL) {

						if (actualDisplayTrack[j]->getPoint() != NULL)
							timeOfFlight = actualDisplayTrack[j]->getPoint()->GetTime();
						else
							timeOfFlight = -1;

						mcTrackVisualAnalyser->addPointToActualTrack(actualDisplayTrack[j]->getHit()->GetX(), actualDisplayTrack[j]->getHit()->GetY(), actualDisplayTrack[j]->getHit()->GetZ(), timeOfFlight, actualDisplayTrack[j]->getHitIndex());

					}

				}

			}

		}

	}

	/* convert visual analysis object to string */
	if (analysisMoreResultWarnings & MCTRACKVISUALIZATION) {

		returnValue = mcTrackVisualAnalyser->toString("MCTrack Visualization", trackIndex);

	}
	/* draw visual analysis object */
	if (analysisMoreResultDisplays & MCTRACKVISUALIZATION) {

		mcTrackVisualAnalyser->setupVolumeVisibility((std::string*)volumes, numberOfVolumes);
		mcTrackVisualAnalyser->drawSetup("mcTrackVisualization", "MCTrack Visualization", trackIndex);

	}

	if (actualDisplayTrack != NULL) {
		free(actualDisplayTrack);
		actualDisplayTrack = NULL;
	}

	return returnValue;

}

/****************************************************************
 * method returns true if the found track distribution-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool analysis::isFoundTrackVisualizationAnalysisEnabled() {

	return ((analysisMoreResultDisplays & FOUNDTRACKVISUALIZATION) | (analysisMoreResultWarnings & FOUNDTRACKVISUALIZATION));

}

/****************************************************************
 * method returns true if the display of the peakfinding		*
 * geometry is enabled.											*
 ****************************************************************/

bool analysis::isPeakfindingGeometryDisplayEnabled() {

	return (analysisMoreResultDisplays & PEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the display of the projected			*
 * peakfinding geometry is enabled.								*
 ****************************************************************/

bool analysis::isProjectedPeakfindingGeometryDisplayEnabled() {

	return (analysisMoreResultDisplays & PROJECTEDPEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the display of the covered			*
 * peakfinding geometry is enabled.								*
 ****************************************************************/

bool analysis::isCoveredPeakfindingGeometryDisplayEnabled() {

	return (analysisMoreResultDisplays & COVEREDPEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the display of the covered and		*
 * projected peakfinding geometry is enabled.					*
 ****************************************************************/

bool analysis::isCoveredProjectedPeakfindingGeometryDisplayEnabled() {

	return (analysisMoreResultDisplays & COVEREDPROJECTEDPEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the message of the peakfinding		*
 * geometry is enabled.											*
 ****************************************************************/

bool analysis::isPeakfindingGeometryMessageEnabled() {

	return (analysisMoreResultWarnings & PEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the message of the projected			*
 * peakfinding geometry is enabled.								*
 ****************************************************************/

bool analysis::isProjectedPeakfindingGeometryMessageEnabled() {

	return (analysisMoreResultWarnings & PROJECTEDPEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the message of the covered			*
 * peakfinding geometry is enabled.								*
 ****************************************************************/

bool analysis::isCoveredPeakfindingGeometryMessageEnabled() {

	return (analysisMoreResultWarnings & COVEREDPEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * method returns true if the message of the covered and		*
 * projected peakfinding geometry is enabled.					*
 ****************************************************************/

bool analysis::isCoveredProjectedPeakfindingGeometryMessageEnabled() {

	return (analysisMoreResultWarnings & COVEREDPROJECTEDPEAKFINDINGGEOMETRYVISUALIZATION);

}

/****************************************************************
 * This method displays all found tracks with corresponding		*
 * hits.														*
 ****************************************************************/

std::string analysis::displayFoundTracks(int trackIndex) {

	std::string                 returnValue;
	trackfinderInputHit**       actualDisplayTrack;
	unsigned int                j;
	trackDigitalInformation     hitInfo;
	trackfinderInputHit*        hit;
	trackHitMem                 possibleTrack;
	specialListMem<trackHitMem> possibleTracks;
	bool                        acceptDisplay;
	trackMomentum               momentum;
	analyticFormula             formula;
	double                      timeOfFlight;

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	if (*tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	if (eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(ANALYSISLIB);

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(ANALYSISLIB);

	returnValue.clear();

	/* allocate memory for one hit per detector station */
	actualDisplayTrack = (trackfinderInputHit**)calloc((*eventData)->getNumberOfStations(), sizeof(trackfinderInputHit*));
	if (actualDisplayTrack == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	/* initialize visual analysis object */
	if (analysisMoreResultDisplays & FOUNDTRACKVISUALIZATION) {
	
		if (foundTrackVisualAnalyser == NULL) {

			foundTrackVisualAnalyser = new visualAnalysis();
			if (foundTrackVisualAnalyser == NULL)
				throw memoryAllocationError(ANALYSISLIB);

		}
		else {

			foundTrackVisualAnalyser->reset();

		}

	}


	/* loop over all found tracks */
	(*tracks)->resetActiveObject();
	for (unsigned short i = 0; i < (*tracks)->getNumberOfTracks(); i++) {

		/* reset the hit pointers for the actual track which should be displayed */
		for (j = 0; j < (*eventData)->getNumberOfStations(); j++)
			actualDisplayTrack[j] = NULL;

		/* read the actual hit distribution */
		(*tracks)->getNextTrackDigitalInfo(&hitInfo);

		/* reset the distribution evaluation object and build the evaluation */
		possibleTracks.clear();
		for (unsigned short k = 0; k < hitInfo.hits.getNumberOfMemories(); k++) {

			hitInfo.hits[k].resetActiveObject();
			for (unsigned short l = 0; l < hitInfo.hits[k].getNumberOfEntries(); l++) {
				
				hit = hitInfo.hits[k].readActiveObjectAndMakeNextOneActive();

				possibleTrack.removeAllHits();
				possibleTrack.addHit(hit);

				if (possibleTracks.isFound(possibleTrack, true))		/* track is old track */
					possibleTracks.getActiveObject()->addHit(hit);
				else													/* track not found, so add it; fakes have no track => they are always not found */
					possibleTracks.push(possibleTrack);

			}

		}

		/* sort the evaluation to have the highest prior object at the beginning*/
		possibleTracks.sort();
		possibleTracks.resetActiveObject();

		/* add the highest prior object hits to the display track */
		possibleTrack = possibleTracks.readActiveObjectAndMakeNextOneActive();
		possibleTrack.resetHitPointer();
		for (unsigned long m = 0; m < possibleTrack.getNumberOfHits(); m++) {
				
			hit = possibleTrack.getHitAndMakeNextActive();
			if (hit->getStation() != NULL)
				actualDisplayTrack[hit->getStation()->getIndex()] = hit;
				
		}

		/* fill the missing hit positions with pointers from less prior objects */
		for (unsigned long n = 1; n < possibleTracks.getNumberOfEntries(); n++) {

			possibleTrack = possibleTracks.readActiveObjectAndMakeNextOneActive();

			possibleTrack.resetHitPointer();
			for (unsigned long o = 0; o < possibleTrack.getNumberOfHits(); o++) {
				
				hit = possibleTrack.getHitAndMakeNextActive();
				if (hit->getStation() != NULL) {

					if (actualDisplayTrack[hit->getStation()->getIndex()] == NULL) {
					
						actualDisplayTrack[hit->getStation()->getIndex()] = hit;

					}

				}

			}

		}

		/* avoid the drawing of objects which corresponds not the given trackIndex */
		acceptDisplay = false;
		for (j = 0; j < (*eventData)->getNumberOfStations(); j++) {

			if (actualDisplayTrack[j] != NULL) {

				if (actualDisplayTrack[j]->getTrack() != NULL) {

					if ((trackIndex == INVALIDTRACKINDEX) || (trackIndex == actualDisplayTrack[j]->getTrack()->getTrackIndex())) {
			
						acceptDisplay = true;
						break;

					}

				}

			}

		}

		/* add the actual track to the display */
		if (acceptDisplay) {

			if (analysisMoreResultDisplays & FOUNDTRACKVISUALIZATION) {

				formula.evaluateP(hitInfo.position, *(*space), &momentum, NULL);
				foundTrackVisualAnalyser->addActualTrack(trackIndex, momentum.get(PX), momentum.get(PY), momentum.get(PZ), hitInfo.position.get(DIM1), hitInfo.position.get(DIM2), hitInfo.position.get(DIM3));
			
				for (j = 0; j < (*eventData)->getNumberOfStations(); j++) {

					if (actualDisplayTrack[j] != NULL) {

						if (actualDisplayTrack[j]->getPoint() != NULL)
							timeOfFlight = actualDisplayTrack[j]->getPoint()->GetTime();
						else
							timeOfFlight = -1;

						foundTrackVisualAnalyser->addPointToActualTrack(actualDisplayTrack[j]->getHit()->GetX(), actualDisplayTrack[j]->getHit()->GetY(), actualDisplayTrack[j]->getHit()->GetZ(), timeOfFlight, actualDisplayTrack[j]->getHitIndex());

					}

				}

			}

		}

	}

	/* convert visual analysis object to string */
	if (analysisMoreResultWarnings & FOUNDTRACKVISUALIZATION) {

		returnValue = foundTrackVisualAnalyser->toString("Found Track Visualization", trackIndex);

	}
	/* draw visual analysis object */
	if (analysisMoreResultDisplays & FOUNDTRACKVISUALIZATION) {

		foundTrackVisualAnalyser->setupVolumeVisibility((std::string*)volumes, numberOfVolumes);
		foundTrackVisualAnalyser->drawSetup("foundTrackVisualization", "Found Track Visualization", trackIndex);

	}

	if (actualDisplayTrack != NULL) {
		free(actualDisplayTrack);
		actualDisplayTrack = NULL;
	}

	return returnValue;

}

/****************************************************************
 * This method returns the value for the percentage of			*
 * hits which must be in a signature which is accepted			*
 ****************************************************************/

unsigned short analysis::getPercentageOfHitsInSignature() {

	return percentageOfHitsInSignature;

}

/****************************************************************
 * This method returns the value for the percentage of real		*
 * tracks which must be found with the accepted signatures		*
 ****************************************************************/

unsigned short analysis::getPercentageOfTracksForSignature() {

	return percentageOfTracksForSignature;

}

/****************************************************************
 * This method returns the actual measured time for creating	*
 * the border objects.											*
 ****************************************************************/

void analysis::borderCreationTimerReset() {

	if (isTimeAnalysisEnabled() && (borderCreationTimer == NULL))
		borderCreationTimer = new TStopwatch();

	if (borderCreationTimer != NULL)
		borderCreationTimer->Reset();

}
void analysis::borderCreationTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (borderCreationTimer == NULL))
		borderCreationTimer = new TStopwatch();

	if (borderCreationTimer != NULL)
		borderCreationTimer->Start(reset);

}
void analysis::borderCreationTimerStop() {

	if (isTimeAnalysisEnabled() && (borderCreationTimer == NULL))
		borderCreationTimer = new TStopwatch();

	if (borderCreationTimer != NULL)
		borderCreationTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for creating	*
 * the histogram layers.										*
 ****************************************************************/

void analysis::histogramCreationTimerReset() {

	if (isTimeAnalysisEnabled() && (histogramCreationTimer == NULL))
		histogramCreationTimer = new TStopwatch();

	if (histogramCreationTimer != NULL)
		histogramCreationTimer->Reset();

}
void analysis::histogramCreationTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (histogramCreationTimer == NULL))
		histogramCreationTimer = new TStopwatch();

	if (histogramCreationTimer != NULL)
		histogramCreationTimer->Start(reset);

}
void analysis::histogramCreationTimerStop() {

	if (isTimeAnalysisEnabled() && (histogramCreationTimer == NULL))
		histogramCreationTimer = new TStopwatch();

	if (histogramCreationTimer != NULL)
		histogramCreationTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for encoding	*
 * the histogram layers.										*
 ****************************************************************/

void analysis::histogramEncodingTimerReset() {

	if (isTimeAnalysisEnabled() && (histogramEncodingTimer == NULL))
		histogramEncodingTimer = new TStopwatch();

	if (histogramEncodingTimer != NULL)
		histogramEncodingTimer->Reset();

}
void analysis::histogramEncodingTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (histogramEncodingTimer == NULL))
		histogramEncodingTimer = new TStopwatch();

	if (histogramEncodingTimer != NULL)
		histogramEncodingTimer->Start(reset);

}
void analysis::histogramEncodingTimerStop() {

	if (isTimeAnalysisEnabled() && (histogramEncodingTimer == NULL))
		histogramEncodingTimer = new TStopwatch();

	if (histogramEncodingTimer != NULL)
		histogramEncodingTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for				*
 * diagonalization the histogram layers.						*
 ****************************************************************/

void analysis::histogramDiagonalizingTimerReset() {

	if (isTimeAnalysisEnabled() && (histogramDiagonalizingTimer == NULL))
		histogramDiagonalizingTimer = new TStopwatch();

	if (histogramDiagonalizingTimer != NULL)
		histogramDiagonalizingTimer->Reset();

}
void analysis::histogramDiagonalizingTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (histogramDiagonalizingTimer == NULL))
		histogramDiagonalizingTimer = new TStopwatch();

	if (histogramDiagonalizingTimer != NULL)
		histogramDiagonalizingTimer->Start(reset);

}
void analysis::histogramDiagonalizingTimerStop() {

	if (isTimeAnalysisEnabled() && (histogramDiagonalizingTimer == NULL))
		histogramDiagonalizingTimer = new TStopwatch();

	if (histogramDiagonalizingTimer != NULL)
		histogramDiagonalizingTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for peak		*
 * finding the histogram layers.								*
 ****************************************************************/

void analysis::histogramPeakfindingTimerReset() {

	if (isTimeAnalysisEnabled() && (histogramPeakfindingTimer == NULL))
		histogramPeakfindingTimer = new TStopwatch();

	if (histogramPeakfindingTimer != NULL)
		histogramPeakfindingTimer->Reset();

}
void analysis::histogramPeakfindingTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (histogramPeakfindingTimer == NULL))
		histogramPeakfindingTimer = new TStopwatch();

	if (histogramPeakfindingTimer != NULL)
		histogramPeakfindingTimer->Start(reset);

}
void analysis::histogramPeakfindingTimerStop() {

	if (isTimeAnalysisEnabled() && (histogramPeakfindingTimer == NULL))
		histogramPeakfindingTimer = new TStopwatch();

	if (histogramPeakfindingTimer != NULL)
		histogramPeakfindingTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for finalizing	*
 * the histogram layers.										*
 ****************************************************************/

void analysis::histogramFinalizingTimerReset() {

	if (isTimeAnalysisEnabled() && (histogramFinalizingTimer == NULL))
		histogramFinalizingTimer = new TStopwatch();

	if (histogramFinalizingTimer != NULL)
		histogramFinalizingTimer->Reset();

}
void analysis::histogramFinalizingTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (histogramFinalizingTimer == NULL))
		histogramFinalizingTimer = new TStopwatch();

	if (histogramFinalizingTimer != NULL)
		histogramFinalizingTimer->Start(reset);

}
void analysis::histogramFinalizingTimerStop() {

	if (isTimeAnalysisEnabled() && (histogramFinalizingTimer == NULL))
		histogramFinalizingTimer = new TStopwatch();

	if (histogramFinalizingTimer != NULL)
		histogramFinalizingTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for resetting	*
 * the histogram layers.										*
 ****************************************************************/

void analysis::histogramResettingTimerReset() {

	if (isTimeAnalysisEnabled() && (histogramResettingTimer == NULL))
		histogramResettingTimer = new TStopwatch();

	if (histogramResettingTimer != NULL)
		histogramResettingTimer->Reset();

}
void analysis::histogramResettingTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (histogramResettingTimer == NULL))
		histogramResettingTimer = new TStopwatch();

	if (histogramResettingTimer != NULL)
		histogramResettingTimer->Start(reset);

}
void analysis::histogramResettingTimerStop() {

	if (isTimeAnalysisEnabled() && (histogramResettingTimer == NULL))
		histogramResettingTimer = new TStopwatch();

	if (histogramResettingTimer != NULL)
		histogramResettingTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for peak		*
 * finding in the track candidates of neighbored layers.		*
 ****************************************************************/

void analysis::trackPeakfindingTimerReset() {

	if (isTimeAnalysisEnabled() && (trackPeakfindingTimer == NULL))
		trackPeakfindingTimer = new TStopwatch();

	if (trackPeakfindingTimer != NULL)
		trackPeakfindingTimer->Reset();

}
void analysis::trackPeakfindingTimerStart(bool reset) {

	if (isTimeAnalysisEnabled() && (trackPeakfindingTimer == NULL))
		trackPeakfindingTimer = new TStopwatch();

	if (trackPeakfindingTimer != NULL)
		trackPeakfindingTimer->Start(reset);

}
void analysis::trackPeakfindingTimerStop() {

	if (isTimeAnalysisEnabled() && (trackPeakfindingTimer == NULL))
		trackPeakfindingTimer = new TStopwatch();

	if (trackPeakfindingTimer != NULL)
		trackPeakfindingTimer->Stop();

}

/****************************************************************
 * This method returns the actual measured time for creating	*
 * the border objects.											*
 ****************************************************************/

double analysis::getBorderCreationRealTime() {

	double returnValue = 0;

	if (borderCreationTimer != NULL)
		returnValue = borderCreationTimer->RealTime();

	return returnValue;

}
double analysis::getBorderCreationCpuTime() {

	double returnValue = 0;

	if (borderCreationTimer != NULL)
		returnValue = borderCreationTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for creating	*
 * the histogram layers.										*
 ****************************************************************/

double analysis::getHistogramCreationRealTime() {

	double returnValue = 0;

	if (histogramCreationTimer != NULL)
		returnValue = histogramCreationTimer->RealTime();

	return returnValue;

}
double analysis::getHistogramCreationCpuTime() {

	double returnValue = 0;

	if (histogramCreationTimer != NULL)
		returnValue = histogramCreationTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for encoding	*
 * the histogram layers.										*
 ****************************************************************/

double analysis::getHistogramEncodingRealTime() {

	double returnValue = 0;

	if (histogramEncodingTimer != NULL)
		returnValue = histogramEncodingTimer->RealTime();

	return returnValue;

}
double analysis::getHistogramEncodingCpuTime() {

	double returnValue = 0;

	if (histogramEncodingTimer != NULL)
		returnValue = histogramEncodingTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for				*
 * diagonalization the histogram layers.						*
 ****************************************************************/

double analysis::getHistogramDiagonalizingRealTime() {

	double returnValue = 0;

	if (histogramDiagonalizingTimer != NULL)
		returnValue = histogramDiagonalizingTimer->RealTime();

	return returnValue;

}
double analysis::getHistogramDiagonalizingCpuTime() {

	double returnValue = 0;

	if (histogramDiagonalizingTimer != NULL)
		returnValue = histogramDiagonalizingTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for peak		*
 * finding the histogram layers.								*
 ****************************************************************/

double analysis::getHistogramPeakfindingRealTime() {

	double returnValue = 0;

	if (histogramPeakfindingTimer != NULL)
		returnValue = histogramPeakfindingTimer->RealTime();

	return returnValue;

}
double analysis::getHistogramPeakfindingCpuTime() {

	double returnValue = 0;

	if (histogramPeakfindingTimer != NULL)
		returnValue = histogramPeakfindingTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for finalizing	*
 * the histogram layers.										*
 ****************************************************************/

double analysis::getHistogramFinalizingRealTime() {

	double returnValue = 0;

	if (histogramFinalizingTimer != NULL)
		returnValue = histogramFinalizingTimer->RealTime();

	return returnValue;

}
double analysis::getHistogramFinalizingCpuTime() {

	double returnValue = 0;

	if (histogramFinalizingTimer != NULL)
		returnValue = histogramFinalizingTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for resetting	*
 * the histogram layers.										*
 ****************************************************************/

double analysis::getHistogramResettingRealTime() {

	double returnValue = 0;

	if (histogramResettingTimer != NULL)
		returnValue = histogramResettingTimer->RealTime();

	return returnValue;

}
double analysis::getHistogramResettingCpuTime() {

	double returnValue = 0;

	if (histogramResettingTimer != NULL)
		returnValue = histogramResettingTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method returns the actual measured time for peak		*
 * finding in the track candidates of neighbored layers.		*
 ****************************************************************/

double analysis::getTrackPeakfindingRealTime() {

	double returnValue = 0;

	if (trackPeakfindingTimer != NULL)
		returnValue = trackPeakfindingTimer->RealTime();

	return returnValue;

}
double analysis::getTrackPeakfindingCpuTime() {

	double returnValue = 0;

	if (trackPeakfindingTimer != NULL)
		returnValue = trackPeakfindingTimer->CpuTime();

	return returnValue;

}

/****************************************************************
 * This method evaluates the size of the memory for				*
 * the source data.												*
 ****************************************************************/

void analysis::evaluateSizeOfLBufferData() {

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	if (*tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);

	reservedSizeOfLBufferData  = (*tracks)->getReservedSizeOfData(0);
	allocatedSizeOfLBufferData = (*tracks)->getAllocatedSizeOfData(0);
	usedSizeOfLBufferData      = (*tracks)->getUsedSizeOfData(0);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double analysis::getReservedSizeOfLBufferData(unsigned short dimension) {

	double returnValue;

	returnValue  = (reservedSizeOfLBufferData / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double analysis::getAllocatedSizeOfLBufferData(unsigned short dimension) {

	double returnValue;

	returnValue  = (allocatedSizeOfLBufferData / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double analysis::getUsedSizeOfLBufferData(unsigned short dimension) {

	double returnValue;

	returnValue  = (usedSizeOfLBufferData / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns a formated string containing the time	*
 * analysis.													*
 ****************************************************************/

std::string analysis::formatRealTimeAnalysis() {

	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "  creating the borders: \t\t\t\t";
	dtos(getBorderCreationRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  creating the histogram layers: \t\t\t";
	dtos(getHistogramCreationRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  encoding the histogram layers: \t\t\t";
	dtos(getHistogramEncodingRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  diagonalizing the histogram layers: \t\t\t";
	dtos(getHistogramDiagonalizingRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  peak finding in the histogram layers: \t\t";
	dtos(getHistogramPeakfindingRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  finalizing the histogram layers: \t\t\t";
	dtos(getHistogramFinalizingRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  resetting the histogram layers: \t\t\t";
	dtos(getHistogramResettingRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  peak finding in the track candidates: \t\t";
	dtos(getTrackPeakfindingRealTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  \t\t\t\t\t\t\t---------";

	return returnValue;

}
std::string analysis::formatRealTimeSummaryAnalysis(double trackfinderRealTime, double eventRealTime) {

	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "  the tracking: \t\t\t\t\t";
	dtos(trackfinderRealTime, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  the event ";
	if (eventData != NULL)
		if (*eventData != NULL)
			returnValue += (*eventData)->getEventNumber();
	returnValue += ": \t\t\t\t\t\t";
	dtos(eventRealTime, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s";

	return returnValue;

}
std::string analysis::formatCpuTimeAnalysis() {

	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "  creating the borders: \t\t\t\t";
	dtos(getBorderCreationCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  creating the histogram layers: \t\t\t";
	dtos(getHistogramCreationCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  encoding the histogram layers: \t\t\t";
	dtos(getHistogramEncodingCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  diagonalizing the histogram layers: \t\t\t";
	dtos(getHistogramDiagonalizingCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  peak finding in the histogram layers: \t\t";
	dtos(getHistogramPeakfindingCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  finalizing the histogram layers: \t\t\t";
	dtos(getHistogramFinalizingCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  resetting the histogram layers: \t\t\t";
	dtos(getHistogramResettingCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  peak finding in the track candidates: \t\t";
	dtos(getTrackPeakfindingCpuTime(), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  \t\t\t\t\t\t\t---------";

	return returnValue;

}
std::string analysis::formatCpuTimeSummaryAnalysis(double trackfinderCpuTime, double eventCpuTime) {

	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "  the tracking:  \t\t\t\t\t";
	dtos(trackfinderCpuTime, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s\n";
	returnValue += "  the event ";
	if (eventData != NULL)
		if (*eventData != NULL)
			returnValue += (*eventData)->getEventNumber();
	returnValue += ":  \t\t\t\t\t";
	dtos(eventCpuTime, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " s";

	return returnValue;

}
std::string analysis::formatTimeAnalysis(double trackfinderRealTime, double eventRealTime, double trackfinderCpuTime, double eventCpuTime) {

	std::string returnValue;

	returnValue  = "Needed realtime for:\n";
	returnValue += formatRealTimeAnalysis();
	returnValue += "\n";
	returnValue += formatRealTimeSummaryAnalysis(trackfinderRealTime, eventRealTime);
	returnValue += "\n\n";
	returnValue += "Needed cputime for:\n";
	returnValue += formatCpuTimeAnalysis();
	returnValue += "\n";
	returnValue += formatCpuTimeSummaryAnalysis(trackfinderCpuTime, eventCpuTime);

	return returnValue;

}

/****************************************************************
 * This method returns a formated string containing the time	*
 * analysis.													*
 ****************************************************************/

std::string analysis::formatReservedMemoryAnalysis(histogramData* histogram, double inputReservedSize) {

	double      sizeOfActualData;
	double      sizeOfData;
	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "Size of reserved memory for:\n";
	sizeOfData   = inputReservedSize;
	if (eventData != NULL) {
		if (*eventData != NULL) {
			sizeOfActualData  = (*eventData)->getReservedSizeOfAddOnData(0);
			returnValue += "    the MC data: \t\t\t\t";
			dtos(sizeOfData - sizeOfActualData, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " B\n";
			returnValue += "    the MC data addon: \t\t\t\t";
			dtos(sizeOfActualData, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " B\n";
		}
	}
	returnValue += "  the input data: \t\t\t\t\t";
	dtos(sizeOfData, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " B\n";
	if (histogram != NULL) {
		sizeOfActualData  = histogram->getReservedSizeOfHBufferPrelutData(0);
		returnValue += "    the HBuffer::Prelut: \t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
		sizeOfActualData  = histogram->getReservedSizeOfHBufferLutData(0);
		returnValue += "    the HBuffer::Lut: \t\t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
		sizeOfActualData  = histogram->getReservedSizeOfHBufferHitData(0);
		returnValue += "    the HBuffer::Hits: \t\t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
		sizeOfActualData  = histogram->getReservedSizeOfHBufferData(0);
		sizeOfData       += sizeOfActualData;
		returnValue += "  the HBuffer unit: \t\t\t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
		sizeOfActualData  = histogram->getReservedSizeOfHistogramSignatureData(0);
		returnValue += "    the signature: \t\t\t\t";
		dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " kB\n";
		sizeOfActualData  = histogram->getReservedSizeOfHistogramHitData(0);
		returnValue += "    the hits: \t\t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
		sizeOfActualData  = histogram->getReservedSizeOfHistogramData(0);
		sizeOfData       += sizeOfActualData;
		returnValue += "  the Histogram: \t\t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
	}
	sizeOfActualData  = getReservedSizeOfLBufferData(0);
	sizeOfData       += sizeOfActualData;
	returnValue += "  the LBuffer unit: \t\t\t\t\t";
	dtos(sizeOfActualData, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " B\n";
	if (tracks != NULL) {
		if (*tracks != NULL) {
			sizeOfActualData  = (*tracks)->getReservedSizeOfData(0);
			sizeOfData       += sizeOfActualData;
			returnValue += "  the output data: \t\t\t\t\t";
			dtos(sizeOfActualData, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " B\n";
		}
	}
	returnValue += "  \t\t\t\t\t\t\t----------\n";
	returnValue += "  the tracking: \t\t\t\t\t";
	dtos(sizeOfData / (1024*1024), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " MB";

	return returnValue;

}
std::string analysis::formatAllocatedMemoryAnalysis(histogramData* histogram, double inputAllocatedSize) {

	double      sizeOfActualData;
	double      sizeOfData;
	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "Size of used allocated for:\n";
	sizeOfData   = inputAllocatedSize;
	if (eventData != NULL) {
		if (*eventData != NULL) {
			sizeOfActualData  = (*eventData)->getAllocatedSizeOfAddOnData(0);
			returnValue += "    the MC data: \t\t\t\t";
			dtos((sizeOfData - sizeOfActualData) / (1024*1024), buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " MB\n";
			returnValue += "    the MC data addon: \t\t\t\t";
			dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " kB\n";
		}
	}
	returnValue += "  the input data: \t\t\t\t\t";
	dtos(sizeOfData / (1024*1024), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " MB\n";
	if (histogram != NULL) {
		sizeOfActualData  = histogram->getAllocatedSizeOfHBufferPrelutData(0);
		returnValue += "    the HBuffer::Prelut: \t\t\t";
		dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " kB\n";
		sizeOfActualData  = histogram->getAllocatedSizeOfHBufferLutData(0);
		returnValue += "    the HBuffer::Lut: \t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
		sizeOfActualData  = histogram->getAllocatedSizeOfHBufferHitData(0);
		returnValue += "    the HBuffer::Hits: \t\t\t\t";
		dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " kB\n";
		sizeOfActualData  = histogram->getAllocatedSizeOfHBufferData(0);
		sizeOfData       += sizeOfActualData;
		returnValue += "  the HBuffer unit: \t\t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
		sizeOfActualData  = histogram->getAllocatedSizeOfHistogramSignatureData(0);
		returnValue += "    the signature: \t\t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
		sizeOfActualData  = histogram->getAllocatedSizeOfHistogramHitData(0);
		returnValue += "    the hits: \t\t\t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
		sizeOfActualData  = histogram->getAllocatedSizeOfHistogramData(0);
		sizeOfData       += sizeOfActualData;
		returnValue += "  the Histogram: \t\t\t\t\t";
		dtos(sizeOfActualData, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " B\n";
	}
	sizeOfActualData  = getAllocatedSizeOfLBufferData(0);
	sizeOfData       += sizeOfActualData;
	returnValue += "  the LBuffer unit: \t\t\t\t\t";
	dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " kB\n";
	if (tracks != NULL) {
		if (*tracks != NULL) {
			sizeOfActualData  = (*tracks)->getAllocatedSizeOfData(0);
			sizeOfData       += sizeOfActualData;
			returnValue += "  the output data: \t\t\t\t\t";
			dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " kB\n";
		}
	}
	returnValue += "  \t\t\t\t\t\t\t----------\n";
	returnValue += "  the tracking: \t\t\t\t\t";
	dtos(sizeOfData / (1024*1024), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " MB";

	return returnValue;

}
std::string analysis::formatUsedMemoryAnalysis(histogramData* histogram, double inputUsedSize) {

	double      sizeOfActualData;
	double      sizeOfData;
	char        buffer[doubleConversion+1];
	std::string returnValue;

	returnValue  = "Size of used memory for:\n";
	sizeOfData   = inputUsedSize;
	if (eventData != NULL) {
		if (*eventData != NULL) {
			sizeOfActualData  = (*eventData)->getUsedSizeOfAddOnData(0);
			returnValue += "    the MC data: \t\t\t\t";
			dtos((sizeOfData - sizeOfActualData) / (1024*1024), buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " MB\n";
			returnValue += "    the MC data addon: \t\t\t\t";
			dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " kB\n";
		}
	}
	returnValue += "  the input data: \t\t\t\t\t";
	dtos(sizeOfData / (1024*1024), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " MB\n";
	if (histogram != NULL) {
		sizeOfActualData  = histogram->getUsedSizeOfHBufferPrelutData(0);
		returnValue += "    the HBuffer::Prelut: \t\t\t";
		dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " kB\n";
		sizeOfActualData  = histogram->getUsedSizeOfHBufferLutData(0);
		returnValue += "    the HBuffer::Lut: \t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
		sizeOfActualData  = histogram->getUsedSizeOfHBufferHitData(0);
		returnValue += "    the HBuffer::Hits: \t\t\t\t";
		dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " kB\n";
		sizeOfActualData  = histogram->getUsedSizeOfHBufferData(0);
		sizeOfData       += sizeOfActualData;
		returnValue += "  the HBuffer unit: \t\t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
		sizeOfActualData  = histogram->getUsedSizeOfHistogramSignatureData(0);
		returnValue += "    the signature: \t\t\t\t";
		dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " kB\n";
		sizeOfActualData  = histogram->getUsedSizeOfHistogramHitData(0);
		returnValue += "    the hits: \t\t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
		sizeOfActualData  = histogram->getUsedSizeOfHistogramData(0);
		sizeOfData       += sizeOfActualData;
		returnValue += "  the Histogram: \t\t\t\t\t";
		dtos(sizeOfActualData / (1024*1024), buffer, doubleConversionDigits);
		returnValue += buffer;
		returnValue += " MB\n";
	}
	sizeOfActualData  = getUsedSizeOfLBufferData(0);
	sizeOfData       += sizeOfActualData;
	returnValue += "  the LBuffer unit: \t\t\t\t\t";
	dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " kB\n";
	if (tracks != NULL) {
		if (*tracks != NULL) {
			sizeOfActualData  = (*tracks)->getUsedSizeOfData(0);
			sizeOfData       += sizeOfActualData;
			returnValue += "  the output data: \t\t\t\t\t";
			dtos(sizeOfActualData / 1024, buffer, doubleConversionDigits);
			returnValue += buffer;
			returnValue += " kB\n";
		}
	}
	returnValue += "  \t\t\t\t\t\t\t----------\n";
	returnValue += "  the tracking: \t\t\t\t\t";
	dtos(sizeOfData / (1024*1024), buffer, doubleConversionDigits);
	returnValue += buffer;
	returnValue += " MB";

	return returnValue;

}
std::string analysis::formatMemoryAnalysis(histogramData* histogram, double inputReservedSize, double inputAllocatedSize, double inputUsedSize) {

	std::string returnValue;

	returnValue  = formatReservedMemoryAnalysis(histogram, inputReservedSize);
	returnValue += "\n\n";

	returnValue += formatAllocatedMemoryAnalysis(histogram, inputAllocatedSize);
	returnValue += "\n\n";

	returnValue += formatUsedMemoryAnalysis(histogram, inputUsedSize);

	return returnValue;

}
