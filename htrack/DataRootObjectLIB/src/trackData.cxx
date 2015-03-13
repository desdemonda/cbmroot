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
//     - class for handling the histogram-transformations memory space
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.9 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/trackData.h"
#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"


/****************************************************************
 * CLASS trackData								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackData::trackData() 
  : space(NULL), tracks(NULL), accessor(), accessLayer(0) 
{

  //	space       = NULL;
  //	tracks      = NULL;
  //	accessLayer = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackData::trackData(const trackData& value) 
  : space(value.space), tracks(NULL), accessor(value.accessor),
    accessLayer(value.accessLayer) 
{

  //	this->space  = value.space;
  //	this->tracks = NULL;

	if ((this->space != NULL) && (*(this->space) != NULL)) {
		if ((*(this->space))->getStep(DIM3) > 0) {
			this->tracks = new trackLayer[(*(this->space))->getStep(DIM3)];
			reset();
			for (unsigned short i = 0; i < (*(this->space))->getStep(DIM3); i++)
				this->tracks[i] = value.tracks[i];
		}
	}

	//	this->accessor    = value.accessor;
	//	this->accessLayer = value.accessLayer;

}
trackData::trackData(histogramSpace** _space) 
  : space(NULL), tracks(NULL), accessor(), accessLayer(0) 
{

  //	this->space        = NULL;
  //	this->tracks       = NULL;
  //	this->accessLayer  = 0;
	init(_space);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackData::~trackData() {

	if (tracks != NULL) {
		if ((space == NULL) || (*space == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);
		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++)
			tracks[i].clear();
		delete[] tracks;
		tracks = NULL;
	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackData& trackData::operator = (const trackData& value) {

	unsigned short i;

	if (this->tracks != NULL) {
		if ((this->space == NULL) || (*(this->space) == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);
		for (i = 0; i < (*(this->space))->getStep(DIM3); i++)
			this->tracks[i].clear();
		delete[] this->tracks;
		this->tracks = NULL;
	}

	this->space  = value.space;

	if ((this->space != NULL) && (*(this->space) != NULL)) {
		if ((*(this->space))->getStep(DIM3) > 0) {
			this->tracks = new trackLayer[(*(this->space))->getStep(DIM3)];
			reset();
			for (i = 0; i < (*(this->space))->getStep(DIM3); i++)
				this->tracks[i] = value.tracks[i];
		}
	}

	this->accessor    = value.accessor;
	this->accessLayer = value.accessLayer;

	return *this;

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void trackData::init(histogramSpace** _space) {

	if (tracks != NULL) {
		if ((this->space == NULL) || (*(this->space) == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);
		for (unsigned short i = 0; i < (*(this->space))->getStep(DIM3); i++)
			tracks[i].clear();
		delete[] tracks;
		tracks = NULL;
	}

	this->space = _space;

	if ((space != NULL) && (*space != NULL)) {
		if ((*(this->space))->getStep(DIM3) > 0) {
			tracks = new trackLayer[(*space)->getStep(DIM3)];
			reset();
		}
	}

}

/****************************************************************
 * This method returns a reference to the object				*
 * representing the histogramSpace.								*
 ****************************************************************/

histogramSpace& trackData::getHistogramSpace() {

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	return *(*space);

}

/****************************************************************
 * This method resets the object.								*
 ****************************************************************/

void trackData::reset() {

	if (tracks == NULL)
		throw trackDataMemoryIsNullError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++)
		tracks[i].clear();

	resetActiveObject();

}

/****************************************************************
 * This method resets the active object.						*
 ****************************************************************/

void trackData::resetActiveObject() {

	if (tracks == NULL)
		throw trackDataMemoryIsNullError();

	accessLayer = 0;
	accessor    = tracks[accessLayer].begin();

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * histogram.													*
 ****************************************************************/

double trackData::getUsedSizeOfTrackData(unsigned short dimension) {

	double returnValue = 0;

	if (tracks == NULL)
		throw trackDataMemoryIsNullError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = sizeof(trackData);
	for(unsigned short i = 0; i < (*space)->getStep(DIM3); i++)
		returnValue += tracks[i].size() * sizeof(trackDigitalInformation);

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * histogram.													*
 ****************************************************************/

double trackData::getAllocatedSizeOfTrackData(unsigned short dimension) {

	return getUsedSizeOfTrackData(dimension);

}

/****************************************************************
 * This method returns the number of layers. This value must be	*
 * equal to the layer dimension of the histogram.				*
 ****************************************************************/

unsigned short trackData::getNumberOfLayers() {

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	return (*space)->getStep(DIM3);

}

/****************************************************************
 * This method adds a track into the memory.					*
 ****************************************************************/

void trackData::addTrack(unsigned short dim1, unsigned short dim2, unsigned short dim3, histogramCell& cell) {

	trackDigitalInformation actualTrack;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if (dim3 < (*space)->getStep(DIM3)) {

		actualTrack.position.set(dim1, DIM1);
		actualTrack.position.set(dim2, DIM2);
		actualTrack.position.set(dim3, DIM3);
		actualTrack.value         = cell.value;

#ifndef NOANALYSIS

		actualTrack.hits          = cell.hits;

#endif

		tracks[dim3].push_back(actualTrack);

	}
	else {

		trackWithTooBigLayerWarningMsg* trackWithTooBigLayer = new trackWithTooBigLayerWarningMsg();
		trackWithTooBigLayer->warningMsg();
		if(trackWithTooBigLayer != NULL) {
			delete trackWithTooBigLayer;
			trackWithTooBigLayer = NULL;
		}

	}

}

/****************************************************************
 * This method removes a track from the memory.					*
 ****************************************************************/

trackLayer::iterator trackData::removeTrack(trackLayer::iterator position, unsigned short layer) {

	trackLayer::iterator actualIterator;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if (layer < (*space)->getStep(DIM3)) {

		actualIterator = tracks[layer].erase(position);

	}
	else {

		trackWithTooBigLayerWarningMsg* trackWithTooBigLayer = new trackWithTooBigLayerWarningMsg();
		trackWithTooBigLayer->warningMsg();
		if(trackWithTooBigLayer != NULL) {
			delete trackWithTooBigLayer;
			trackWithTooBigLayer = NULL;
		}

	}

	return actualIterator;

}

/****************************************************************
 * This method returns the number of tracks.					*
 ****************************************************************/

unsigned short trackData::getNumberOfTracks() {

	unsigned short returnValue = 0;

	if (tracks == NULL)
		throw trackDataMemoryIsNullError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++)
		returnValue += (unsigned short)tracks[i].size();

	return returnValue;

}

/****************************************************************
 * This method returns the each trackInfo in an order.			*
 ****************************************************************/

void trackData::getNextTrackDigitalInfo(trackDigitalInformation* value) {

	bool wrapAround;

	if (value == NULL)
		throw cannotAccessTrackInformationError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	wrapAround = false;
	while (1 == 1) {

		if (accessor == tracks[accessLayer].end()) {

			accessLayer++;
			if (accessLayer == (*space)->getStep(DIM3)) {
				accessLayer = 0;
				if (wrapAround) {
					accessor = tracks[accessLayer].begin();
					break;
				}
				wrapAround  = true;
			}
			accessor = tracks[accessLayer].begin();

		}
		else {

			*value = *accessor;
			accessor++;
			break;

		}
	
	}

}

/****************************************************************
 * This method returns the each trackAnalogInformation in an	*
 * order.														*
 ****************************************************************/

void trackData::getNextTrackAnalogInfo(trackAnalogInformation* value) {

	bool wrapAround;

	if (value == NULL)
		throw cannotAccessTrackParameterError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	wrapAround = false;
	while (1 == 1) {

		if (accessor == tracks[accessLayer].end()) {

			accessLayer++;
			if (accessLayer == (*space)->getStep(DIM3)) {
				accessLayer = 0;
				if (wrapAround) {
					accessor = tracks[accessLayer].begin();
					break;
				}
				wrapAround  = true;
			}
			accessor = tracks[accessLayer].begin();

		}
		else {

			getTrackAnalogInfoFromTrackDigitalInfo(*accessor, value);
			accessor++;
			break;

		}
	
	}

}

/**
 * This method returns the hit information.
 */

void trackData::getNextHitInfo(hitArray* value) {

#ifndef NOANALYSIS

	bool     wrapAround;

	if (value == NULL)
		throw cannotAccessTrackParameterError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	wrapAround = false;
	while (1 == 1) {

		if (accessor == tracks[accessLayer].end()) {

			accessLayer++;
			if (accessLayer == (*space)->getStep(DIM3)) {
				accessLayer = 0;
				if (wrapAround) {
					accessor = tracks[accessLayer].begin();
					break;
				}
				wrapAround  = true;
			}
			accessor = tracks[accessLayer].begin();

		}
		else {

			*value = accessor->hits;
			accessor++;
			break;

		}
	
	}

#else

	value->reset();

#endif

}

/****************************************************************
 * This method returns the each trackAnalogInformation in an	*
 * order.														*
 ****************************************************************/

void trackData::getTrackAnalogInfoFromTrackDigitalInfo(trackDigitalInformation digital, trackAnalogInformation* analog) {

	if (analog == NULL)
		throw cannotAccessTrackParameterError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	analog->position.set((*space)->getAnalogFromCell(digital.position.get(DIM1), DIM1), DIM1);
	analog->position.set((*space)->getAnalogFromCell(digital.position.get(DIM2), DIM2), DIM2);
	analog->position.set((*space)->getAnalogFromCell(digital.position.get(DIM3), DIM3), DIM3);

#ifndef NOANALYSIS

	analog->hits = digital.hits;

#endif

}

/****************************************************************
 * This method returns the each trackParameter in an order.		*
 ****************************************************************/

void trackData::getTrackParameterFromTrackDigitalInfo(trackDigitalInformation digital, trackParameter* parameter) {

	if (parameter == NULL)
		throw cannotAccessTrackParameterError();

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	parameter->set((*space)->getAnalogFromCell(digital.position.get(DIM1), DIM1), DIM1);
	parameter->set((*space)->getAnalogFromCell(digital.position.get(DIM2), DIM2), DIM2);
	parameter->set((*space)->getAnalogFromCell(digital.position.get(DIM3), DIM3), DIM3);

}

/****************************************************************
 * This method returns the number of tracks.					*
 ****************************************************************/

unsigned short trackData::getNumberOfTracksOfHistogramLayer(unsigned short layer) {

	unsigned short returnValue = 0;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if (layer < (*space)->getStep(DIM3)) {

		returnValue = (unsigned short)tracks[layer].size();

	}
	else {

		tooBigLayerWarningMsg* tooBigLayer = new tooBigLayerWarningMsg();
		tooBigLayer->warningMsg();
		if(tooBigLayer != NULL) {
			delete tooBigLayer;
			tooBigLayer = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method returns an iterator to the begin of tracks in	*
 * one histogram layer.											*
 ****************************************************************/

trackLayer::iterator trackData::getTrackIteratorBeginOfHistogramLayer(unsigned short layer) {

	trackLayer::iterator returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if (layer < (*space)->getStep(DIM3)) {

		returnValue = tracks[layer].begin();

	}
	else {

		tooBigLayerWarningMsg* tooBigLayer = new tooBigLayerWarningMsg();
		tooBigLayer->warningMsg();
		if(tooBigLayer != NULL) {
			delete tooBigLayer;
			tooBigLayer = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method returns an iterator to the end of tracks in		*
 * one histogram layer.											*
 ****************************************************************/

trackLayer::iterator trackData::getTrackIteratorEndOfHistogramLayer(unsigned short layer) {

	trackLayer::iterator returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if (layer < (*space)->getStep(DIM3)) {

		returnValue = tracks[layer].end();

	}
	else {

		tooBigLayerWarningMsg* tooBigLayer = new tooBigLayerWarningMsg();
		tooBigLayer->warningMsg();
		if(tooBigLayer != NULL) {
			delete tooBigLayer;
			tooBigLayer = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method prints a the borders of a histogram into a file.	*
 ****************************************************************/

void trackData::printTracks(unsigned int maximumClass, unsigned int layerStart, unsigned int layerStop, unsigned int stationStart, unsigned int stationStop, bool hits, char* name) {

	FILE*                fileHandle;
	unsigned int         i;
	trackfinderInputHit* hit;

	if ((fileHandle = fopen(name, "w")) == NULL) {
		printf("Cannot write debug file '%s'!!!\n", name);
		fileHandle = NULL;
	}
	else {
		for (i = layerStart; i < layerStop; i++) {
			fprintf(fileHandle, "[%i]: ", i);
			for (trackLayer::iterator j = tracks[i].begin(); j != tracks[i].end(); j++) {
				fprintf(fileHandle, "([%i ", j->position.get(DIM1));
				fprintf(fileHandle, "%i ", j->position.get(DIM2));
				fprintf(fileHandle, "%i]: ", j->position.get(DIM3));
#ifdef CORRECTTRACK
				fprintf(fileHandle, "%i) ", maximumClass);
#else
				fprintf(fileHandle, "%s) ", j->value.toString(10).c_str());
#endif
			}
			fprintf(fileHandle, "\n");
		}

#ifndef NOANALYSIS

		if (hits) {
			fprintf(fileHandle, "\n");
			fprintf(fileHandle, "\n");
			for (i = layerStart; i < layerStop; i++) {
				for (trackLayer::iterator k = tracks[i].begin(); k != tracks[i].end(); k++) {
					fprintf(fileHandle, "[%i, %i, %i]: ", k->position.get(DIM1), k->position.get(DIM2), k->position.get(DIM3));
					for (unsigned int l = stationStart; l < stationStop; l++) {
						if (k->hits[l].getNumberOfEntries() > 0) {
							fprintf(fileHandle, "([%i]: ", l);
							k->hits[l].resetActiveObject();
							for (unsigned int m = 0; m < k->hits[l].getNumberOfEntries(); m++) {
								hit = k->hits[l].readActiveObjectAndMakeNextOneActive();
								if (hit != NULL)
									fprintf(fileHandle, "%i ", hit->getHitIndex());
							}
							fprintf(fileHandle, ") ");
						}
					}
					fprintf(fileHandle, "\n");
				}
				fprintf(fileHandle, "\n");
				fprintf(fileHandle, "\n");
			}
		}

#else

		hitAnalysisNotSupportedWarningMsg* hitAnalysisNotSupported = new hitAnalysisNotSupportedWarningMsg();
		hitAnalysisNotSupported->warningMsg();
		if(hitAnalysisNotSupported != NULL) {
			delete hitAnalysisNotSupported;
			hitAnalysisNotSupported = NULL;
		}

#endif

		fclose(fileHandle);
		fileHandle = NULL;
	}

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackData::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(space);
	returnValue += sizeof(tracks);
	returnValue += sizeof(accessor);
	returnValue += sizeof(accessLayer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackData::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;
	if (tracks != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			for (trackLayer::iterator j = tracks[i].begin(); j != tracks[i].end(); j++) {

				returnValue += j->getReservedSizeOfData(0);
				returnValue += j->getAllocatedSizeOfData(0);

			}

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackData::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(space);
	returnValue += sizeof(tracks);

	if (tracks != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			for (trackLayer::iterator j = tracks[i].begin(); j != tracks[i].end(); j++) {

				returnValue += j->getUsedSizeOfData(0);

			}

		}

	}

	returnValue += sizeof(accessor);
	returnValue += sizeof(accessLayer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
