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
//     - class for handling the hough-transformation
//     - class computes the histogram
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:41:00 $
// $Revision: 1.17 $
//
// *******************************************************************/


//#define PRINTBORDERSTOFILE						/**< This definition enables the printing of all borders into a file */
#undef PRINTBORDERSTOFILE

//#define PRINTCREATEDHISTOGRAMLAYERSTOFILE			/**< This definition enables the printing of all histogram layers into a file */
#undef PRINTCREATEDHISTOGRAMLAYERSTOFILE

/**
 * If this define is set, a warning would occur if it is not
 * enough space to add the hit to the hit-list of the station
 * in the histogram. If this define is unset, no warning would
 * be shown.
 */

#define CHECKNUMBEROFHITENTRIESINHISTCELL


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../include/houghTransformationError.h"
#include "../include/houghTransformationWarningMsg.h"
#include "../include/houghTransformation.h"

#ifdef PRINTCREATEDHISTOGRAMLAYERSTOFILE

#include "../../MiscLIB/include/conversionRoutines.h"

#endif


/****************************************************************
 * This method does the entries for one layer of the histogram	*
 ****************************************************************/

void houghTransformation::doHistogramLayerEntry(std::vector<lutBorder>::iterator border) {

	doHistogramLayerEntry(border->getLutHoughBorder(), border->getHit());

}
void houghTransformation::doHistogramLayerEntry(lutBorder* border) {
	
	doHistogramLayerEntry(border->getLutHoughBorder(), border->getHit());

}
void houghTransformation::doHistogramLayerEntry(lutHoughBorder& secondBorder, trackfinderInputHit* hit) {

	unsigned short                         stationIndex;
	houghBorderPosition                    actualHoughCoord;
	tooMuchHitEntriesInHistCellWarningMsg* tooMuchHitEntriesInHistCell;

	tooMuchHitEntriesInHistCell = NULL;

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	if (hit != NULL)
		if (hit->getStation() != NULL)
			stationIndex = hit->getStation()->getIndex();
		else
			throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);
	else
		throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);

	if (!hit->getStation()->isMasked()) {

		secondBorder.houghCoord.resetActiveObject();
		for (unsigned int k = 0; k < secondBorder.houghCoord.getNumberOfEntries(); k++) {

			actualHoughCoord = secondBorder.houghCoord.readActiveObjectAndMakeNextOneActive();

			/* Zuordnung von Detektorhit (1 im Binärbild) zur quantisierten Funktion im Houghraum (h: p=x*cos+y*sin) */
			(*histogram)->setStationHit(actualHoughCoord.pos1, actualHoughCoord.pos2, stationIndex);

#ifndef NOANALYSIS

#ifdef CHECKNUMBEROFHITENTRIESINHISTCELL

			if (!(*histogram)->addStationHit(actualHoughCoord.pos1, actualHoughCoord.pos2, stationIndex, hit)) {
					
				tooMuchHitEntriesInHistCell = new tooMuchHitEntriesInHistCellWarningMsg(actualHoughCoord.pos1, actualHoughCoord.pos2, actualLayer, stationIndex);
				tooMuchHitEntriesInHistCell->warningMsg();
				if(tooMuchHitEntriesInHistCell != NULL) {
					delete tooMuchHitEntriesInHistCell;
					tooMuchHitEntriesInHistCell = NULL;
				}

			}

#else

			(*histogram)->addStationHit(actualHoughCoord.pos1, actualHoughCoord.pos2, stationIndex, hit);

#endif

#endif

		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghTransformation::houghTransformation() 
  : eventData(NULL),
    histogram(NULL),
    lut(NULL),
    actualLayer(0),
    dynamicLayerMemory(),
    debugTrackIndex(INVALIDTRACKINDEX)
{
  /*
	eventData       = NULL;
	histogram       = NULL;
	lut             = NULL;
	actualLayer     = 0;
	dynamicLayerMemory.clear();
	debugTrackIndex = INVALIDTRACKINDEX;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

houghTransformation::houghTransformation(trackfinderInputData** _eventData, histogramData** _histogram, lutImplementation** _lut) 
  : eventData(_eventData),
    histogram(_histogram),
    lut(_lut),
    actualLayer(0),
    dynamicLayerMemory(),
    debugTrackIndex(INVALIDTRACKINDEX)
{
  /*
	this->eventData = _eventData;
	this->histogram = _histogram;
	this->lut       = _lut;

	actualLayer     = 0;
	dynamicLayerMemory.clear();
	debugTrackIndex = INVALIDTRACKINDEX;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghTransformation::~houghTransformation() {

	dynamicLayerMemory.clear();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void houghTransformation::init(trackfinderInputData** _eventData, histogramData** _histogram, lutImplementation** _lut) {

	/* allocate new space */
	this->eventData = _eventData;
	this->histogram = _histogram;
	this->lut       = _lut;

	dynamicLayerMemory.clear();
	actualLayer = 0;

}

/****************************************************************
 * This method creates the borders for each hit					*
 ****************************************************************/

void houghTransformation::createBorders(trackfinderInputTrack* track) {

	trackfinderInputHit* hit;
	lutBorder            border;

	if (track == NULL)
		throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);
	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (lut == NULL)
		throw cannotAccessLutsError(HOUGHTRANSFORMATIONLIB);
	if (*lut == NULL)
		throw cannotAccessLutsError(HOUGHTRANSFORMATIONLIB);

	track->resetHitPointer();
	if ((*lut)->typeUsesCorrections())
		(*lut)->resetCorrectionCounter();
	hit = NULL;

	for(unsigned short i = 0; i < track->getNumberOfHits(); i++) {

		hit = track->getHit();
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);

		(*lut)->evaluate(hit, &border);
		border.setHit(hit);

		(*histogram)->addBorder(border);

	}

	(*lut)->clear();

}

#ifndef NOANALYSIS

void houghTransformation::createBorders(analysis* analyser, std::streambuf* terminal) {

#else

void houghTransformation::createBorders(std::streambuf* terminal) {

#endif

	trackfinderInputHit* hit;
	lutBorder            border;
	terminalSequence     statusSequence;

#ifdef DEBUGJUSTONEGOODTRACK

	int                  goodTrack[INDEXOFGOODTRACK];
	int                  numberOfGoodTracks;
	bool                 isAcceptableTrack;
	bool                 goodTrackIsFound;

	for (unsigned int a = 0; a < INDEXOFGOODTRACK; a++)
		goodTrack[a] = -1;

#endif

	if (eventData == NULL)
		throw cannotAccessEventDataError(HOUGHTRANSFORMATIONLIB);
	if (*eventData == NULL)
		throw cannotAccessEventDataError(HOUGHTRANSFORMATIONLIB);
	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (lut == NULL)
		throw cannotAccessLutsError(HOUGHTRANSFORMATIONLIB);
	if (*lut == NULL)
		throw cannotAccessLutsError(HOUGHTRANSFORMATIONLIB);

	if ((*lut)->typeUsesCorrections())
		(*lut)->resetCorrectionCounter();

	hit = NULL;
	createTerminalStatusSequence(&statusSequence, terminal, "\nCreate borders:\t\t\t\t\t", (unsigned int)(*eventData)->getNumberOfHits());
	terminalInitialize(statusSequence);

#ifdef DEBUGJUSTONEGOODTRACK

	numberOfGoodTracks = 0;

#endif

	for(int i = 0; i < (*eventData)->getNumberOfHits(); i++) {

		hit = (*eventData)->getHitByIndex(i);
		if (hit == NULL)
			throw cannotAccessHitsOrTracksError(HOUGHTRANSFORMATIONLIB);

#ifdef DEBUGJUSTONEGOODTRACK

		if (hit->getTrack() == NULL)
			continue;

		if (numberOfGoodTracks < INDEXOFGOODTRACK) {

#ifndef NOANALYSIS

			if (analyser != NULL) {

				isAcceptableTrack = analyser->isFindableStandardTrack(hit->getTrack());

			}
			else {

#endif

				if (hit->getTrack()->getNumberOfHits() >= (*eventData)->getNumberOfActiveStations())
					isAcceptableTrack = true;
				else
					isAcceptableTrack = false;

#ifndef NOANALYSIS

			}

#endif

			if (isAcceptableTrack) {

				goodTrackIsFound = false;
				for (int search = 0; search < numberOfGoodTracks; search++) {
					if (goodTrack[search] == hit->getTrack()->getTrackIndex())
						goodTrackIsFound = true;
				}

				if (!goodTrackIsFound) {
					goodTrack[numberOfGoodTracks] = hit->getTrack()->getTrackIndex();
					numberOfGoodTracks++;
				}

			}

		}

		if ((hit->getTrack()->getTrackIndex() != goodTrack[INDEXOFGOODTRACK - 1]) || (numberOfGoodTracks != INDEXOFGOODTRACK)) {
	
			terminalOverwrite(statusSequence, (unsigned int)(i + 1));
			continue;
		
		}

#endif

		(*lut)->evaluate(hit, &border);
		border.setHit(hit);

		(*histogram)->addBorder(border);

		terminalOverwrite(statusSequence, (unsigned int)(i + 1));

	}

	(*lut)->clear();
	terminalFinalize(statusSequence);

#ifdef DEBUGJUSTONEGOODTRACK

	if (numberOfGoodTracks != INDEXOFGOODTRACK)
		throw noGoodTrackFoundError();
	else {
		debugTrackIndex = goodTrack[INDEXOFGOODTRACK - 1];
		if ((*eventData)->getTrackByIndex(debugTrackIndex) != NULL) {
			std::cout << "Correct momentum of the MCTrack (px, py, pz): (";
			std::cout << (*eventData)->getTrackByIndex(debugTrackIndex)->getMomX();
			std::cout << ", ";
			std::cout << (*eventData)->getTrackByIndex(debugTrackIndex)->getMomY();
			std::cout << ", ";
			std::cout << (*eventData)->getTrackByIndex(debugTrackIndex)->getMomZ();
			std::cout << ")" << std::endl;
		}
		else
			std::cout << "MCTrack with index: " << debugTrackIndex << " is not found!!!" << std::endl;
	}

#endif

#if (LUTVERSION == 4)

	if ((*lut)->typeUsesCorrections() {
		if ((*lut)->getNumberOfCoordCorrections() > 0) {
			houghLutCorrectionWarningMsg* houghLutCorrection = new houghLutCorrectionWarningMsg("HOUGHTRANSFORMATIONLIB", (*lut)->getNumberOfCorrections(), (*lut)->getNumberOfCoordCorrections());
			houghLutCorrection->warningMsg();
			if(houghLutCorrection != NULL) {
				delete houghLutCorrection;
				houghLutCorrection = NULL;
			}
		}
	}

#else

#if (LUTVERSION == 5)

	if ((*lut)->typeUsesCorrections() {
		houghLutCorrectionWarningMsg* houghLutCorrection = new houghLutCorrectionWarningMsg("HOUGHTRANSFORMATIONLIB", (*lut)->getNumberOfCorrections(), (*lut)->getNumberOfCoordCorrections());
		houghLutCorrection->warningMsg();
		if(houghLutCorrection != NULL) {
			delete houghLutCorrection;
			houghLutCorrection = NULL;
		}
	}

#endif

#endif

#ifdef PRINTBORDERSTOFILE

	(*histogram)->printBorder(0, (*histogram)->getValueDim3(), "borders.txt");

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isHitReadoutDistributionAnalysisEnabled())
			analyser->evaluateHitReadoutDistribution((*histogram), terminal);

#endif

}

/****************************************************************
 * This method creates one layer of the histogram				*
 ****************************************************************/

void houghTransformation::createHistogramLayer(unsigned short layer) {

	lutBorder*  transferBorder;

#ifdef PRINTCREATEDHISTOGRAMLAYERSTOFILE

	std::string file;
	std::string id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (layer >= (*histogram)->getValueDim3())
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	/* erase the content of the dynamic memory because the layer is the first one */
	/* erase the content of the dynamic memory and fill it with the correct content */
	if ((layer == 0) || (layer != actualLayer + 1)) {

		dynamicLayerMemory.clear();

		for (unsigned short i = 0; i < layer; i++) {

			for (std::vector<lutBorder>::iterator j = (*histogram)->getBorderBeginIterator(i); j != (*histogram)->getBorderEndIterator(i); j++) {

				if (j->getPrelutHoughBorder().stop >= layer) {
					transferBorder = &(*j);
					dynamicLayerMemory.push_back(transferBorder);
				}

			}

		}

	}

	/* set the layer */
	actualLayer = layer;

	/* do the dynamic memory entries */
	for (std::list<lutBorder*>::iterator k = dynamicLayerMemory.begin(); k != dynamicLayerMemory.end(); ) {

		transferBorder = *k;
		doHistogramLayerEntry(transferBorder);

		if (transferBorder->getPrelutHoughBorder().stop <= actualLayer)
			k = dynamicLayerMemory.erase(k);
		else
			k++;

	}

	/* do the static memory entries */
	for (std::vector<lutBorder>::iterator l = (*histogram)->getBorderBeginIterator(actualLayer); l != (*histogram)->getBorderEndIterator(actualLayer); l++) {

		doHistogramLayerEntry(l);

		/* do the entry into the dynamic memory if the border is also valid for the next layer */
		if (l->getPrelutHoughBorder().stop > actualLayer) {
		
			transferBorder = &(*l);
			dynamicLayerMemory.push_back(transferBorder);
		
		}

	}

#ifdef PRINTCREATEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "createdHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method resets the layer of the histogram object.		*
 ****************************************************************/

void houghTransformation::resetHistogramLayer() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	(*histogram)->resetLayer();

}

/****************************************************************
 * This method resets the histogram object.						*
 ****************************************************************/

void houghTransformation::resetHistogram() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	(*histogram)->reset();

}

/****************************************************************
 * This method returns the number of the histogram layers for	*
 * full processing.												*
 ****************************************************************/

unsigned short houghTransformation::getNumberOfHistogramLayers() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(HOUGHTRANSFORMATIONLIB);

	return (*histogram)->getValueDim3();

}

/****************************************************************
 * This method returns the trackIndex of the debugged track,	*
 * if DEBUGJUSTONEGOODTRACK is enabled.							*
 ****************************************************************/

int houghTransformation::getDebugTrackIndex() {

	return debugTrackIndex;

}
