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
//     - class for handling the hough-transformations memory space
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:39:20 $
// $Revision: 1.7 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/terminal.h"
#include "../../MiscLIB/include/coordinateSystem.h"
#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"
#include "../include/histogramData.h"
#include <stdio.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#ifndef CALLOC_HISTOGRAM_RUNTIME

	histogramCell* staticPCell[maxDim2];
	histogramCell  staticCell[maxDim2][maxDim1];
	borderCell     staticHitBorders[maxDim3];

#endif

/****************************************************************
 * allocates memory												*
 ****************************************************************/

void histogramData::allocateMemory(std::streambuf* terminal) {

	terminalSequence statusSequence;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

#ifndef CALLOC_HISTOGRAM_RUNTIME

	if ((*space)->getStep(DIM1)  > maxDim1)
		throw memoryAllocationError(DATAOBJECTLIB);
	if ((*space)->getStep(DIM2)  > maxDim2)
		throw memoryAllocationError(DATAOBJECTLIB);
	if (valueDimSH > maxDimSH)
		throw memoryAllocationError(DATAOBJECTLIB);

#endif

	createTerminalStatusSequence(&statusSequence, terminal, "\nAllocate histogram:\t\t\t\t", (unsigned int)(*space)->getStep(DIM2));
	terminalInitialize(statusSequence);

#ifdef CALLOC_HISTOGRAM_RUNTIME

	cell       = new histogramCell*[(*space)->getStep(DIM2)];
	if (cell == NULL)
		throw memoryAllocationError(DATAOBJECTLIB);
	hitBorders = new borderCell[(*space)->getStep(DIM3)];
	if (hitBorders == NULL)
		throw memoryAllocationError(DATAOBJECTLIB);

#else

	cell       = staticPCell;
	hitBorders = staticHitBorders;

#endif

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

		cell[i] = new histogramCell[(*space)->getStep(DIM1)];
		if (cell[i] == NULL)
			throw memoryAllocationError(DATAOBJECTLIB);

#else

		staticPCell[i]        = staticCell[i];

#endif

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

#ifndef CALLOC_HISTOGRAM_RUNTIME

	reset();

#endif

}

/****************************************************************
 * deletes allocated memory										*
 ****************************************************************/

void histogramData::deleteMemory(std::streambuf* terminal) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

	terminalSequence statusSequence;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Deallocate histogram:\t\t", (unsigned int)(*space)->getStep(DIM2));
	terminalInitialize(statusSequence);

	reset();

	if(cell != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

			if(cell[i] != NULL) {
				delete[] cell[i];
				cell[i] = NULL;
			}

			terminalOverwriteWithIncrement(statusSequence);

		}

		delete[] cell;
		cell = NULL;
	}

	if(hitBorders != NULL) {
		delete[] hitBorders;
		hitBorders = NULL;
	}

	terminalFinalize(statusSequence);

#endif

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramData::histogramData() : space(NULL), valueDimSH(0), cell(NULL), hitBorders(NULL) {

  //	space       = NULL;
  //	valueDimSH  = 0;
  //	cell        = NULL;
  //	hitBorders  = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

histogramData::histogramData(const histogramData& value) {

	/* free the old allocated space */
	deleteMemory(NULL);

	this->space       = value.space;		/* histogramData size in all dimensions */
	this->valueDimSH  = value.valueDimSH;		/* histogramData stationHit size */

	/* allocate new space */
	allocateMemory(NULL);

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*(this->space))->getStep(DIM2); i++)
		for (unsigned short j = 0; j < (*(this->space))->getStep(DIM1); j++)
			this->cell[i][j] = value.cell[i][j];
	for (unsigned short k = 0; k < (*(this->space))->getStep(DIM3); k++)
		this->hitBorders[k]  = value.hitBorders[k];

}
histogramData::histogramData(histogramSpace** _space) : space(_space), valueDimSH(0), cell(NULL), hitBorders(NULL) {

  //	this->space = _space;			/* histogramData size in all dimensions */
  //	valueDimSH  = 0;				/* histogramData stationHit size */
  //	cell        = NULL;
  //	hitBorders  = NULL;

}
histogramData::histogramData(histogramSpace** _space, unsigned short stationStep) : space(_space), valueDimSH(stationStep), cell(NULL), hitBorders(NULL) {

  //	this->space = _space;			/* histogramData size in all dimensions */
  //	valueDimSH  = stationStep;		/* histogramData stationHit size */
  //	cell        = NULL;
  //	hitBorders  = NULL;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

histogramData::~histogramData() {

	deleteMemory(NULL);

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const histogramData& histogramData::operator = (const histogramData& value) {

	/* free the old allocated space */
	deleteMemory(NULL);

	this->space       = value.space;			/* histogramData size in all dimensions */
	this->valueDimSH  = value.valueDimSH;		/* histogramData stationHit size */

	/* allocate new space */
	allocateMemory(NULL);

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*(this->space))->getStep(DIM2); i++)
		for (unsigned short j = 0; j < (*(this->space))->getStep(DIM1); j++)
			this->cell[i][j] = value.cell[i][j];
	for (unsigned short k = 0; k < (*(this->space))->getStep(DIM3); k++)
		this->hitBorders[k]  = value.hitBorders[k];

	return *this;

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void histogramData::init(std::streambuf* terminal) {

	/* free the old allocated space */
	deleteMemory(terminal);

	/* allocate new space */
	allocateMemory(terminal);

}
void histogramData::init(unsigned short stationStep, std::streambuf* terminal) {

	/* free the old allocated space */
	deleteMemory(terminal);

	/* set new parameter */
	valueDimSH = stationStep;		/* histogramData stationHit size */

	/* allocate new space */
	allocateMemory(terminal);

}
void histogramData::init(histogramSpace** _space, std::streambuf* terminal) {

	/* free the old allocated space */
	deleteMemory(terminal);

	/* set new parameter */
	this->space = _space;				/* histogramData size in all dimensions */

	/* allocate new space */
	allocateMemory(terminal);

}
void histogramData::init(histogramSpace** _space, unsigned short stationStep, std::streambuf* terminal) {

	/* free the old allocated space */
	deleteMemory(terminal);

	/* set new parameter */
	this->space = _space;				/* histogramData size in all dimensions */
	valueDimSH  = stationStep;			/* histogramData stationHit size */

	/* allocate new space */
	allocateMemory(terminal);

}

/****************************************************************
 * This method resets the object.								*
 ****************************************************************/

void histogramData::reset() {

	resetLayer();

	if (hitBorders != NULL) {

		if ((space == NULL) || (*(space) == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short k = 0; k < (*space)->getStep(DIM3); k++) {

			hitBorders[k].layer.clear();

		}

	}

}

/****************************************************************
 * This method resets the layer of the object.					*
 ****************************************************************/

void histogramData::resetLayer() {

	if (cell != NULL) {

		if ((space == NULL) || (*(space) == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

			if (cell[i] != NULL) {

				for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

					cell[i][j].reset();

				}

			}

		}

	}

}

/****************************************************************
 * This method returns the size of the dimension 1 for the		*
 * histogram.													*
 ****************************************************************/

unsigned short histogramData::getValueDim1() {

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	return (*space)->getStep(DIM1);

}

/****************************************************************
 * This method returns the size of the dimension 2 for the		*
 * histogram.													*
 ****************************************************************/

unsigned short histogramData::getValueDim2() {

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	return (*space)->getStep(DIM2);

}

/****************************************************************
 * This method returns the size of the dimension 3 for the		*
 * histogram.													*
 ****************************************************************/

unsigned short histogramData::getValueDim3() {

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	return (*space)->getStep(DIM3);

}

/****************************************************************
 * This method returns the size of the dimension SH for the		*
 * histogram.													*
 ****************************************************************/

unsigned short histogramData::getValueDimSH() {

	return valueDimSH;

}

/****************************************************************
 * This method returns the cell at position (index1, index2)	*
 * from the histogram.											*
 ****************************************************************/

histogramCell* histogramData::getCell(unsigned short index1, unsigned short index2) {

	histogramCell* returnValue;

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((index1 < (*space)->getStep(DIM1)) && (index2 < (*space)->getStep(DIM2)) && (cell != NULL) && (*cell != NULL))
		returnValue = &cell[index2][index1];
	else
		throw cannotAccessHistogramCellError(index1, index2);

	return returnValue;

}

/****************************************************************
 * This method returns an iterator for the borders at position	*
 * (index) from the histogram.									*
 ****************************************************************/

std::vector<lutBorder>::iterator histogramData::getBorderBeginIterator(unsigned short index) {

	std::vector<lutBorder>::iterator returnValue;

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((index < (*space)->getStep(DIM3)) && (hitBorders != NULL))
		returnValue = hitBorders[index].layer.begin();
	else
		throw cannotAccessHistogramBorderError(index);

	return returnValue;

}
std::vector<lutBorder>::iterator histogramData::getBorderEndIterator(unsigned short index) {

	std::vector<lutBorder>::iterator returnValue;

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((index < (*space)->getStep(DIM3)) && (hitBorders != NULL))
		returnValue = hitBorders[index].layer.end();
	else
		throw cannotAccessHistogramBorderError(index);

	return returnValue;

}

/**
 * This method returns the number of borders at position (index) from the histogram.
 */

unsigned long histogramData::getNumberOfBorders(unsigned short index) {

	unsigned long returnValue = 0;

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((index < (*space)->getStep(DIM3)) && (hitBorders != NULL))
		returnValue = hitBorders[index].layer.size();
	else
		throw cannotAccessHistogramBorderError(index);

	return returnValue;

}

/****************************************************************
 * This method sets the hit into the cell at position			*
 * (cellIndex1,	cellIndex2) for the detectorstation				*
 * (indexStation) of the histogram.								*
 ****************************************************************/

void histogramData::setStationHit(unsigned short cellIndex1, unsigned short cellIndex2, unsigned short stationIndex, bool hit) {

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((cellIndex1 < (*space)->getStep(DIM1)) && (cellIndex2 < (*space)->getStep(DIM2)) && (cell != NULL) && (*cell != NULL))
		cell[cellIndex2][cellIndex1].value[stationIndex] = hit;
	else
		throw cannotAccessHistogramCellError(cellIndex1, cellIndex2);

}

/****************************************************************
 * This method adds the hit itself into the cell at position	*
 * (cellIndex1,	cellIndex2) for the detectorstation				*
 * (indexStation) of the histogram.								*
 ****************************************************************/

bool histogramData::addStationHit(unsigned short cellIndex1, unsigned short cellIndex2, unsigned short stationIndex, trackfinderInputHit* hit) {

	bool returnValue = true;

#ifndef NOANALYSIS

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((cellIndex1 < (*space)->getStep(DIM1)) && (cellIndex2 < (*space)->getStep(DIM2)) && (cell != NULL) && (*cell != NULL)) {
		if (hit != NULL)
			returnValue = cell[cellIndex2][cellIndex1].hits[stationIndex].push(hit);
	}
	else
		throw cannotAccessHistogramCellError(cellIndex1, cellIndex2);

#endif

	return returnValue;

}

/****************************************************************
 * This method adds the border at position (index) to the		*
 * histogram.													*
 ****************************************************************/

void histogramData::addBorder(lutBorder& border) {

	int index = border.getPrelutHoughBorder().start;

	if ((space == NULL) || (*(space) == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	if ((index < (*space)->getStep(DIM3)) && (hitBorders != NULL))
		hitBorders[index].layer.push_back(border);
	else
		throw cannotAccessHistogramBorderError(index);

}

/****************************************************************
 * This method prints a layer of the histogram into a file.		*
 ****************************************************************/

void histogramData::printLayer(unsigned int maximumClass, unsigned int dim1Start, unsigned int dim1Stop, unsigned int dim2Start, unsigned int dim2Stop, unsigned int dimStationStart, unsigned int dimStationStop, bool hits, char* name) {

	FILE*                fileHandle;
	trackfinderInputHit* hit;
	unsigned int         i;
	unsigned int         j;

	if ((fileHandle = fopen(name, "w")) == NULL) {
		printf("Cannot write debug file '%s'!!!\n", name);
		fileHandle = NULL;
	}
	else {
		for (i = dim2Stop; i > dim2Start; i--) {
			for (j = dim1Start; j < dim1Stop; j++) {
#ifdef CORRECTHISTOGRAMLAYER
				fprintf(fileHandle, "[%i, %i]: %s ", i-1, j, maximumClass);
#else
				fprintf(fileHandle, "[%i, %i]: %s ", i-1, j, cell[i-1][j].value.toString().c_str());
#endif
			}
			fprintf(fileHandle, "\n");
		}

#ifndef NOANALYSIS

		if (hits) {
			fprintf(fileHandle, "\n");
			fprintf(fileHandle, "\n");
			for (i = dim2Stop; i > dim2Start; i--) {
				for (j = dim1Start; j < dim1Stop; j++) {
					fprintf(fileHandle, "[%i, %i]: ", i-1, j);
					for (unsigned int k = dimStationStart; k < dimStationStop; k++) {
						if (cell[i-1][j].hits[k].getNumberOfEntries() > 0) {
							fprintf(fileHandle, "[%i]: ", k);
							cell[i-1][j].hits[k].resetActiveObject();
							for (unsigned int l = 0; l < cell[i-1][j].hits[k].getNumberOfEntries(); l++) {
								hit = cell[i-1][j].hits[k].readActiveObjectAndMakeNextOneActive();
								if (hit != NULL)
									fprintf(fileHandle, "%i ", hit->getHitIndex());
							}
						}
					}
					fprintf(fileHandle, "\n");
				}
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
void histogramData::printLayer(unsigned int dim1Start, unsigned int dim1Stop, unsigned int dim2Start, unsigned int dim2Stop, unsigned int dimStationStart, unsigned int dimStationStop, bool hits, char* name) {

	FILE*                fileHandle;
	trackfinderInputHit* hit;
	unsigned int         i;
	unsigned int         j;

	if ((fileHandle = fopen(name, "w")) == NULL) {
		printf("Cannot write debug file '%s'!!!\n", name);
		fileHandle = NULL;
	}
	else {
		for (i = dim2Stop; i > dim2Start; i--) {
			for (j = dim1Start; j < dim1Stop; j++) {
				fprintf(fileHandle, "[%i, %i]: %s ", i-1, j, cell[i-1][j].value.toString().c_str());
			}
			fprintf(fileHandle, "\n");
		}

#ifndef NOANALYSIS

		if (hits) {
			fprintf(fileHandle, "\n");
			fprintf(fileHandle, "\n");
			for (i = dim2Stop; i > dim2Start; i--) {
				for (j = dim1Start; j < dim1Stop; j++) {
					fprintf(fileHandle, "[%i, %i]: ", i-1, j);
					for (unsigned int k = dimStationStart; k < dimStationStop; k++) {
						if (cell[i-1][j].hits[k].getNumberOfEntries() > 0) {
							fprintf(fileHandle, "[%i]: ", k);
							cell[i-1][j].hits[k].resetActiveObject();
							for (unsigned int l = 0; l < cell[i-1][j].hits[k].getNumberOfEntries(); l++) {
								hit = cell[i-1][j].hits[k].readActiveObjectAndMakeNextOneActive();
								if (hit != NULL)
									fprintf(fileHandle, "%i ", hit->getHitIndex());
							}
						}
					}
					fprintf(fileHandle, "\n");
				}
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
 * This method prints a the borders of a histogram into a file.	*
 ****************************************************************/

void histogramData::printBorder(unsigned int dim3Start, unsigned int dim3Stop, char* name) {

	FILE*               fileHandle;
	houghBorderPosition position;

	if ((fileHandle = fopen(name, "w")) == NULL) {
		printf("Cannot write debug file '%s'!!!\n", name);
		fileHandle = NULL;
	}
	else {
		for (unsigned int i = dim3Start; i < dim3Stop; i++) {
			for (unsigned int j = 0; j < hitBorders[i].layer.size(); j++) {
				fprintf(fileHandle, "[%i, %i]: ", i, j);
				fprintf(fileHandle, "%i ", hitBorders[i].layer[j].getPrelutHoughBorder().start);
				fprintf(fileHandle, "%i ", hitBorders[i].layer[j].getPrelutHoughBorder().stop);
				hitBorders[i].layer[j].getLutHoughBorder().houghCoord.resetActiveObject();
				for (unsigned int k = 0; k < hitBorders[i].layer[j].getLutHoughBorder().houghCoord.getNumberOfEntries(); k++) {
					position = hitBorders[i].layer[j].getLutHoughBorder().houghCoord.readActiveObjectAndMakeNextOneActive();
					fprintf(fileHandle, "[%i]: ", k);
					fprintf(fileHandle, "%i ", position.pos1);
					fprintf(fileHandle, "%i ", position.pos2);
				}
			}
			fprintf(fileHandle, "\n");
		}
		fprintf(fileHandle, "\n");
		fclose(fileHandle);
		fileHandle = NULL;
	}

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHBufferData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hitBorders);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHBufferData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (hitBorders != NULL) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

		if ((space == NULL) || (*space == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getReservedSizeOfData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfData(0);

		}

#else

		for (unsigned short i = 0; i < maxDim3; i++) {

			returnValue += hitBorders[i].getReservedSizeOfData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfData(0);

		}

#endif

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHBufferData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	if (hitBorders != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getUsedSizeOfData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHBufferPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHBufferPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (hitBorders != NULL) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

		if ((space == NULL) || (*space == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getReservedSizeOfPrelutData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfPrelutData(0);

		}

#else

		for (unsigned short i = 0; i < maxDim3; i++) {

			returnValue += hitBorders[i].getReservedSizeOfPrelutData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfPrelutData(0);

		}

#endif

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHBufferPrelutData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	if (hitBorders != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getUsedSizeOfPrelutData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHBufferLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHBufferLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (hitBorders != NULL) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

		if ((space == NULL) || (*space == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getReservedSizeOfLutData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfLutData(0);

		}

#else

		for (unsigned short i = 0; i < maxDim3; i++) {

			returnValue += hitBorders[i].getReservedSizeOfLutData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfLutData(0);

		}

#endif

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHBufferLutData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	if (hitBorders != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getUsedSizeOfLutData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHBufferHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHBufferHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (hitBorders != NULL) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

		if ((space == NULL) || (*space == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getReservedSizeOfHitData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfHitData(0);

		}

#else

		for (unsigned short i = 0; i < maxDim3; i++) {

			returnValue += hitBorders[i].getReservedSizeOfHitData(0);
			returnValue += hitBorders[i].getAllocatedSizeOfHitData(0);

		}
	
#endif

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHBufferHitData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	if (hitBorders != NULL) {

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getUsedSizeOfHitData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHBufferAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (hitBorders != NULL) {

#ifdef CALLOC_HISTOGRAM_RUNTIME

		if ((space == NULL) || (*space == NULL))
			throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

		for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

			returnValue += hitBorders[i].getAllocatedSizeOfAddOnData(0);

		}

#else

		for (unsigned short i = 0; i < maxDim3; i++) {

			returnValue += hitBorders[i].getAllocatedSizeOfAddOnData(0);

		}

#endif

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHistogramData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

#ifndef CALLOC_HISTOGRAM_RUNTIME

	for (unsigned short i = 0; i < maxDim2; i++) {

		for (unsigned short j = 0; j < maxDim1; j++) {

			returnValue += cell[i][j].getReservedSizeOfHistogramData(0);

		}

	}

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHistogramData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

#ifndef CALLOC_HISTOGRAM_RUNTIME

	for (unsigned short i = 0; i < maxDim2; i++) {

		for (unsigned short j = 0; j < maxDim1; j++) {

			returnValue += cell[i][j].getAllocatedSizeOfHistogramData(0);

		}

	}

#else

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

		for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

			returnValue += cell[i][j].getReservedSizeOfHistogramData(0);
			returnValue += cell[i][j].getAllocatedSizeOfHistogramData(0);

		}

	}

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHistogramData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

		for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

			returnValue += cell[i][j].getUsedSizeOfHistogramData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHistogramSignatureData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

#ifndef CALLOC_HISTOGRAM_RUNTIME

	for (unsigned short i = 0; i < maxDim2; i++) {

		for (unsigned short j = 0; j < maxDim1; j++) {

			returnValue += cell[i][j].getReservedSizeOfHistogramSignatureData(0);

		}

	}

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHistogramSignatureData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

#ifndef CALLOC_HISTOGRAM_RUNTIME

	for (unsigned short i = 0; i < maxDim2; i++) {

		for (unsigned short j = 0; j < maxDim1; j++) {

			returnValue += cell[i][j].getAllocatedSizeOfHistogramSignatureData(0);

		}

	}

#else

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

		for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

			returnValue += cell[i][j].getReservedSizeOfHistogramSignatureData(0);
			returnValue += cell[i][j].getAllocatedSizeOfHistogramSignatureData(0);

		}

	}

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHistogramSignatureData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

		for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

			returnValue += cell[i][j].getUsedSizeOfHistogramSignatureData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getReservedSizeOfHistogramHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

#ifndef CALLOC_HISTOGRAM_RUNTIME

	for (unsigned short i = 0; i < maxDim2; i++) {

		for (unsigned short j = 0; j < maxDim1; j++) {

			returnValue += cell[i][j].getReservedSizeOfHistogramHitData(0);

		}

	}

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramData::getAllocatedSizeOfHistogramHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

#ifndef CALLOC_HISTOGRAM_RUNTIME

	for (unsigned short i = 0; i < maxDim2; i++) {

		for (unsigned short j = 0; j < maxDim1; j++) {

			returnValue += cell[i][j].getAllocatedSizeOfHistogramHitData(0);

		}

	}

#else

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

		for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

			returnValue += cell[i][j].getReservedSizeOfHistogramHitData(0);
			returnValue += cell[i][j].getAllocatedSizeOfHistogramHitData(0);

		}

	}

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramData::getUsedSizeOfHistogramHitData(unsigned short dimension) {

	double returnValue;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(DATAROOTOBJECTLIB);

	returnValue  = 0;

	for (unsigned short i = 0; i < (*space)->getStep(DIM2); i++) {

		for (unsigned short j = 0; j < (*space)->getStep(DIM1); j++) {

			returnValue += cell[i][j].getUsedSizeOfHistogramHitData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
