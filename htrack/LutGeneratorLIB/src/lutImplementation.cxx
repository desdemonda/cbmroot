/////////////////////////////////////////////////////////////////////
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
//     - class for the implementation of both look-up-tables
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 14:01:50 $
// $Revision: 1.5 $
//
/////////////////////////////////////////////////////////////////////


#include "../../DataRootObjectLIB/include/rungeKuttaInterface.h"
#include "../../DataObjectLIB/include/analyticFormula.h"
#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/terminal.h"
#include "../include/prelutMath.h"
#include "../include/prelutAccess.h"
#include "../include/lutMath.h"
#include "../include/lutAccess.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/lutImplementation.h"


/****************************************************************
 * deletes allocated and initialized memory						*
 ****************************************************************/

void lutImplementation::deleteSpace() {

	if (isSpaceLocal) {
		if (space != NULL) {
			if ((*space) != NULL) {
				delete (*space);
				(*space) = NULL;
			}
			free(space);
			space = NULL;
		}
	}

}
void lutImplementation::deleteLuts(identifiedPrelut* first, identifiedLut* second) {

	if (first != NULL) {
		if (first->lutObject != NULL) {
			switch(first->type) {
				case ANALYTICFORMULALUT:
					delete ((prelutMath*)first->lutObject);
					break;
				case FILELUT:
					delete ((prelutAccess*)first->lutObject);
					break;
				default:
					delete first->lutObject;
					break;
			}
			first->lutObject = NULL;
			first->type      = NOLUT;
		}
	}
	if (second != NULL) {
		if (second->lutObject != NULL) {
			switch(second->type) {
				case ANALYTICFORMULALUT:
					delete ((lutMath*)second->lutObject);
					break;
				case FILELUT:
					delete ((lutAccess*)second->lutObject);
					break;
				default:
					delete second->lutObject;
					break;
			}
			second->lutObject = NULL;
			second->type      = NOLUT;
		}
	}

}
void lutImplementation::deleteMemory() {

	deleteSpace();
	deleteLuts(&firstLut, &secondLut);

}

/****************************************************************
 * allocates memory for the histogramSpace object				*
 ****************************************************************/

void lutImplementation::allocateSpace(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step) {

	deleteSpace();

	space        = (histogramSpace**)calloc(1, sizeof(histogramSpace*));
	if (space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);

	(*space)     = new histogramSpace();
	if (*space == NULL)
		throw memoryAllocationError(LUTGENERATORLIB);

	(*space)->setMin(dim1Min, DIM1);
	(*space)->setMin(dim2Min, DIM2);
	(*space)->setMin(dim3Min, DIM3);
	(*space)->setMax(dim1Max, DIM1);
	(*space)->setMax(dim2Max, DIM2);
	(*space)->setMax(dim3Max, DIM3);
	(*space)->setStep(dim1Step, DIM1);
	(*space)->setStep(dim2Step, DIM2);
	(*space)->setStep(dim3Step, DIM3);

	isSpaceLocal = true;

}
void lutImplementation::allocateSpace(histogramSpace** _space) {

	deleteSpace();

	this->space  = _space;

	isSpaceLocal = false;

}

/****************************************************************
 * allocates memory for the look-up-table object				*
 ****************************************************************/

void lutImplementation::allocateMathLuts(identifiedPrelut* first, identifiedLut* second, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField) {

	deleteLuts(first, second);

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (first != NULL) {

		first->lutObject  = new prelutMath((*space)->getMin(DIM3), (*space)->getMax(DIM3), (*space)->getStep(DIM3), dim3StartEntry, dim3StopEntry);
		if (first->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		first->type       = ANALYTICFORMULALUT;

	}

	if (second != NULL) {

		second->lutObject = new lutMath((*space)->getMin(DIM1), (*space)->getMax(DIM1), (*space)->getStep(DIM1), (*space)->getMin(DIM2), (*space)->getMax(DIM2), (*space)->getStep(DIM2));
		if (second->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		second->type      = ANALYTICFORMULALUT;

		if (magneticField != NULL)
			second->lutObject->setMagneticField(magneticField);

	}

}
void lutImplementation::allocateFileLuts(identifiedPrelut* first, identifiedLut* second, double dim3StartEntry, double dim3StopEntry) {

	deleteLuts(first, second);

	if ((this->space == NULL) || (*(this->space) == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (first != NULL) {

		first->lutObject  = new prelutAccess((*space)->getMin(DIM3), (*space)->getMax(DIM3), (*space)->getStep(DIM3), dim3StartEntry, dim3StopEntry);
		if (first->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		first->type       = FILELUT;

	}

	if (second != NULL) {

		second->lutObject = new lutAccess((*space)->getMin(DIM1), (*space)->getMax(DIM1), (*space)->getStep(DIM1), (*space)->getMin(DIM2), (*space)->getMax(DIM2), (*space)->getStep(DIM2));
		if (second->lutObject == NULL)
			throw memoryAllocationError(LUTGENERATORLIB);
		second->type      = FILELUT;

	}

}

/****************************************************************
 * Method reads the file look-up-tables.						*
 ****************************************************************/

void lutImplementation::readFileLuts(std::string prelutFileName, std::string lutFileName, double dim3StartEntry, double dim3StopEntry, std::streambuf* terminal) {

	allocateFileLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry);

	if ((firstLut.type != FILELUT) ||(secondLut.type != FILELUT))
		throw wrongLutTypeForUsedFunctionalityDetectedError();

	if (!prelutFileName.empty())
		((prelutAccess*)(firstLut.lutObject))->read(prelutFileName, terminal);
	if (!lutFileName.empty())
		((lutAccess*)(secondLut.lutObject))->read(lutFileName, terminal);

}

/****************************************************************
 * Method generates the analytic formula file look-up-tables.	*
 ****************************************************************/

void lutImplementation::generateAnalyticFormulaFileLuts(std::string prelutFileName, std::string prelutName, std::string lutFileName, std::string lutName, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	identifiedPrelut    firstLutMath;
	identifiedLut       secondLutMath;
	terminalSequence    statusSequence;
	digitalHit          digitalHitValue;
	trackfinderInputHit analogHitValue;
	prelutHoughBorder   actualPrelutBorder;
	lutHoughBorder      actualLutBorder;

	firstLutMath.lutObject  = NULL;
	firstLutMath.type       = NOLUT;
	secondLutMath.lutObject = NULL;
	secondLutMath.type      = NOLUT;

	allocateMathLuts(&firstLutMath, &secondLutMath, dim3StartEntry, dim3StopEntry, magneticField);

	allocateFileLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry);

	if ((firstLutMath.type != ANALYTICFORMULALUT) || (secondLutMath.type != ANALYTICFORMULALUT) || (firstLut.type != FILELUT) || (secondLut.type != FILELUT))
		throw wrongLutTypeForUsedFunctionalityDetectedError();

	createTerminalStatusSequence(&statusSequence, terminal, "Generate Analytic Formula file LUTs:\t\t\t", (unsigned int)(digitalHitValue.getMaxNumberOfHitIndizes()));
	terminalInitialize(statusSequence);

	((prelutAccess*)(firstLut.lutObject))->clear();
	((lutAccess*)(secondLut.lutObject))->clear();
	for (unsigned long i = 0; i < digitalHitValue.getMaxNumberOfHitIndizes(); i++) {

		digitalHitValue.setHitIndex(i);
		analogHitValue = digitalHitValue.getHit();
		
		((prelutMath*)(firstLutMath.lutObject))->evaluate(&analogHitValue, &actualPrelutBorder);
		((lutMath*)(secondLutMath.lutObject))->evaluate(&analogHitValue, &actualLutBorder);

		((prelutAccess*)(firstLut.lutObject))->addEntry(actualPrelutBorder, digitalHitValue);
		((lutAccess*)(secondLut.lutObject))->addEntry(actualLutBorder, digitalHitValue);

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

	if (!prelutFileName.empty())
		((prelutAccess*)(firstLut.lutObject))->write(prelutFileName, prelutName, terminal);
	if (!lutFileName.empty())
		((lutAccess*)(secondLut.lutObject))->write(lutFileName, lutName, HARDWAREFORMAT, terminal);

	deleteLuts(&firstLutMath, &secondLutMath);

}

/****************************************************************
 * Method generates the Runge-Kutta list.						*
 ****************************************************************/

void lutImplementation::generateRungeKuttaList(specialList<rungeKuttaListEntry, true, true, true, true, true>* rungeKuttaList, trackfinderInputDetector* detector, std::streambuf* terminal) {

	terminalSequence               statusSequence;
	trackCoordinates               actualDigitalTrack;
	trackMomentum                  momentum;
	double                         charge;
	analyticFormula                basicFormula;
	rungeKuttaInterface            computationFormula;
	std::list<trackfinderInputHit> actualTrackHits;
	rungeKuttaListEntry            actualRungeKuttaListEntry;
	trackfinderInputHit            actualAnalogHit;
	digitalHit                     actualDigitalHit;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (rungeKuttaList == NULL)
		throw cannotAccessRungeKuttaListError();

	rungeKuttaList->clear();
	computationFormula.init(detector);

	/* Build the list of all possible tracks identified by the actual digitized parameter settings with their correspondent digitized detector hit coordinates */
	createTerminalStatusSequence(&statusSequence, terminal, "Generate Runge-Kutta list:\t\t\t", (unsigned int)((*space)->getStep(DIM1) * (*space)->getStep(DIM2) * (*space)->getStep(DIM3)));
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < (*space)->getStep(DIM3); i++) {

		actualDigitalTrack.set(i, DIM3);

		for (unsigned short j = 0; i < (*space)->getStep(DIM2); i++) {

			actualDigitalTrack.set(j, DIM2);

			for (unsigned short k = 0; i < (*space)->getStep(DIM1); i++) {

				actualDigitalTrack.set(k, DIM1);

				basicFormula.evaluateP(actualDigitalTrack, *(*space), &momentum, &charge);
				actualTrackHits = computationFormula.evaluate(momentum, charge);

				actualRungeKuttaListEntry.setPosition(actualDigitalTrack);
				actualRungeKuttaListEntry.clearHits();
				while (!actualTrackHits.empty()) {

					actualAnalogHit = actualTrackHits.back(); 
					actualTrackHits.pop_back();
					actualDigitalHit.setHit(&actualAnalogHit);
					actualRungeKuttaListEntry.addHit(actualDigitalHit);

				}

				rungeKuttaList->push(actualRungeKuttaListEntry);

				terminalOverwriteWithIncrement(statusSequence);

			}

		}

	}

	terminalFinalize(statusSequence);

}

/****************************************************************
 * Method generates the inverted Runge-Kutta list.				*
 ****************************************************************/

void lutImplementation::generateRungeKuttaInvertedList(specialList<rungeKuttaListEntry, true, true, true, true, true>* rungeKuttaList, specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal) {

	terminalSequence            statusSequence;
	rungeKuttaListEntry         actualRungeKuttaListEntry;
	trackCoordinates            actualDigitalTrack;
	rungeKuttaInvertedListEntry actualRungeKuttaInvertedListEntry;
	digitalHit                  actualDigitalHit;

	if (rungeKuttaList == NULL)
		throw cannotAccessRungeKuttaListError();

	if (rungeKuttaInvertedList == NULL)
		throw cannotAccessRungeKuttaInvertedListError();

	rungeKuttaInvertedList->clear();

	/* Build the list of digitized hits with their correspondent digitized track parameters by inverting the actually created list */
	createTerminalStatusSequence(&statusSequence, terminal, "Generate Runge-Kutta inverted list:\t\t", (unsigned int)(rungeKuttaList->getNumberOfEntries()));
	terminalInitialize(statusSequence);

	while (!rungeKuttaList->isEmpty()) {

		actualRungeKuttaListEntry = rungeKuttaList->pop();

		actualDigitalTrack = actualRungeKuttaListEntry.getPosition();
		actualRungeKuttaInvertedListEntry.clearPositions();
		actualRungeKuttaInvertedListEntry.addPosition(actualDigitalTrack);

		actualRungeKuttaListEntry.resetActiveHitObject();
		for (unsigned long i = 0; i < actualRungeKuttaListEntry.getNumberOfHits(); i++) {

			actualDigitalHit = actualRungeKuttaListEntry.readActiveHitObjectAndMakeNextOneActive();

			actualRungeKuttaInvertedListEntry.setHit(actualDigitalHit);

			if (rungeKuttaInvertedList->isFound(actualRungeKuttaInvertedListEntry, true))
				rungeKuttaInvertedList->getActiveObject()->addPosition(actualDigitalTrack);
			else
				rungeKuttaInvertedList->push(actualRungeKuttaInvertedListEntry);

		}

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

}

/****************************************************************
 * Method interpolates the inverted Runge-Kutta list.			*
 ****************************************************************/

void lutImplementation::interpolateRungeKuttaInvertedList(specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal) {

	terminalSequence            statusSequence;
	trackCoordinates            actualDigitalTrack;
	rungeKuttaInvertedListEntry actualRungeKuttaInvertedListEntry;
	digitalHit                  actualDigitalHit;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (rungeKuttaInvertedList == NULL)
		throw cannotAccessRungeKuttaInvertedListError();

	/* Interpolate the actually inverted list with regard to the neccessary hit indizes */
	createTerminalStatusSequence(&statusSequence, terminal, "Interpolate Runge-Kutta inverted list:\t\t", (unsigned int)(actualDigitalHit.getMaxNumberOfHitIndizes()));
	terminalInitialize(statusSequence);

	actualDigitalTrack.set(0, DIM1);
	actualDigitalTrack.set((*space)->getStep(DIM2) - 1, DIM2);
	actualDigitalTrack.set(0, DIM3);
	actualRungeKuttaInvertedListEntry.clearPositions();
	actualRungeKuttaInvertedListEntry.addPosition(actualDigitalTrack);
	for (unsigned long i = 0; i < actualDigitalHit.getMaxNumberOfHitIndizes(); i++) {

		actualDigitalHit.setHitIndex(i);

		actualRungeKuttaInvertedListEntry.setHit(actualDigitalHit);

		if (!rungeKuttaInvertedList->isFound(actualRungeKuttaInvertedListEntry, false))
			rungeKuttaInvertedList->push(actualRungeKuttaInvertedListEntry);

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

}

/****************************************************************
 * Method makes the inverted Runge-Kutta list conform to the	*
 * actual LUTVERSION.											*
 ****************************************************************/

void lutImplementation::makeLutversionConformRungeKuttaInvertedList(specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal) {

	unsigned short                                              j;
	terminalSequence                                            statusSequence;
	specialList<trackCoordinates, true, true, true, true, true> addLutversionConformPositions;
	rungeKuttaInvertedListEntry*                                actualRungeKuttaInvertedListEntry;
	trackCoordinates                                            previousLutversionConformPosition;
	trackCoordinates                                            actualLutversionConformPosition;
	trackCoordinates                                            addLutversionConformPosition;
	double                                                      lutversionConformPositionSlope12;
	double                                                      lutversionConformPositionSlope32;

	if ((space == NULL) || (*space == NULL))
		throw cannotAccessHistogramSpaceError(LUTGENERATORLIB);

	if (rungeKuttaInvertedList == NULL)
		throw cannotAccessRungeKuttaInvertedListError();

	/* Make the actually interpolated list conform to the used LUTVERSION */
	createTerminalStatusSequence(&statusSequence, terminal, "Make Runge-Kutta inverted list conform:\t\t", (unsigned int)(rungeKuttaInvertedList->getNumberOfEntries()));
	terminalInitialize(statusSequence);

	rungeKuttaInvertedList->resetActiveObject();
	for (unsigned long i = 0; i < rungeKuttaInvertedList->getNumberOfEntries(); i++) {

		actualRungeKuttaInvertedListEntry = rungeKuttaInvertedList->getActiveObject();
		if (actualRungeKuttaInvertedListEntry == NULL)
			throw wrongEntryInListError();

		if (actualRungeKuttaInvertedListEntry->getNumberOfPositions() > 0) {
	
			addLutversionConformPositions.clear();
			actualRungeKuttaInvertedListEntry->resetActivePositionObject();
			previousLutversionConformPosition   = actualRungeKuttaInvertedListEntry->readActivePositionObjectAndMakeNextOneActive();

			addLutversionConformPosition.set(previousLutversionConformPosition.get(DIM1), DIM1);
			addLutversionConformPosition.set(previousLutversionConformPosition.get(DIM3), DIM3);
			for (j = 0; j < previousLutversionConformPosition.get(DIM2); j++) {
			
				addLutversionConformPosition.set(j, DIM2);
				addLutversionConformPositions.push(addLutversionConformPosition);
			
			}

			for (unsigned long k = 1; k < actualRungeKuttaInvertedListEntry->getNumberOfPositions(); k++) {

				actualLutversionConformPosition = actualRungeKuttaInvertedListEntry->readActivePositionObject();

				if (actualLutversionConformPosition.get(DIM1) < previousLutversionConformPosition.get(DIM1))
					actualLutversionConformPosition.set(previousLutversionConformPosition.get(DIM1), DIM1);

				if (actualLutversionConformPosition.get(DIM2) < previousLutversionConformPosition.get(DIM2))
					actualLutversionConformPosition.set(previousLutversionConformPosition.get(DIM2), DIM2);

				lutversionConformPositionSlope12 = (double)(actualLutversionConformPosition.get(DIM1) - previousLutversionConformPosition.get(DIM1)) / (double)(actualLutversionConformPosition.get(DIM2) - previousLutversionConformPosition.get(DIM2));
				lutversionConformPositionSlope32 = (double)(actualLutversionConformPosition.get(DIM3) - previousLutversionConformPosition.get(DIM3)) / (double)(actualLutversionConformPosition.get(DIM2) - previousLutversionConformPosition.get(DIM2));

#if (LUTVERSION > 2)

				if (lutversionConformPositionSlope12 > 1) {
					
					lutversionConformPositionSlope12 = 1;
					actualLutversionConformPosition.set((unsigned short)(lutversionConformPositionSlope12 * (actualLutversionConformPosition.get(DIM2) - previousLutversionConformPosition.get(DIM2))) + previousLutversionConformPosition.get(DIM1), DIM1);
					actualRungeKuttaInvertedListEntry->updateActivePositionObject(actualLutversionConformPosition);

				}

#endif

#if (LUTVERSION == 1)

				for (j = previousLutversionConformPosition.get(DIM2); j < actualLutversionConformPosition.get(DIM2); j++) {

					for (unsigned short l = (unsigned short)(lutversionConformPositionSlope12 * (j - previousLutversionConformPosition.get(DIM2))) + previousLutversionConformPosition.get(DIM1); l <= (unsigned short)(lutversionConformPositionSlope12 * (j + 1 - previousLutversionConformPosition.get(DIM2))) + previousLutversionConformPosition.get(DIM1); l++) {

						addLutversionConformPosition.set(l, DIM1);
						addLutversionConformPosition.set(j, DIM2);
						addLutversionConformPosition.set((unsigned short)(lutversionConformPositionSlope32 * (j - previousLutversionConformPosition.get(DIM2))) + previousLutversionConformPosition.get(DIM3), DIM3);
						addLutversionConformPositions.push(addLutversionConformPosition);

					}

				}

#else

				for (j = previousLutversionConformPosition.get(DIM2) + 1; j < actualLutversionConformPosition.get(DIM2); j++) {

					addLutversionConformPosition.set((unsigned short)(lutversionConformPositionSlope12 * (j - previousLutversionConformPosition.get(DIM2))) + previousLutversionConformPosition.get(DIM1), DIM1);
					addLutversionConformPosition.set(j, DIM2);
					addLutversionConformPosition.set((unsigned short)(lutversionConformPositionSlope32 * (j - previousLutversionConformPosition.get(DIM2))) + previousLutversionConformPosition.get(DIM3), DIM3);
					addLutversionConformPositions.push(addLutversionConformPosition);

				}

#endif

				actualRungeKuttaInvertedListEntry->makeNextPositionObjectActive();
				previousLutversionConformPosition = actualLutversionConformPosition;

			}

			if (previousLutversionConformPosition.get(DIM1) + 1 < (*space)->getStep(DIM1)) {
			
				addLutversionConformPosition.set(previousLutversionConformPosition.get(DIM1) + 1, DIM1);
				addLutversionConformPosition.set(previousLutversionConformPosition.get(DIM3), DIM3);

#if (LUTVERSION == 1)

				for (j = previousLutversionConformPosition.get(DIM2); j < (*space)->getStep(DIM2); j++) {

#else

				for (j = previousLutversionConformPosition.get(DIM2) + 1; j < (*space)->getStep(DIM2); j++) {

#endif

					addLutversionConformPosition.set(j, DIM2);
					addLutversionConformPositions.push(addLutversionConformPosition);
			
				}

			}

			while(!addLutversionConformPositions.isEmpty()) {

				addLutversionConformPosition = addLutversionConformPositions.pop();
				actualRungeKuttaInvertedListEntry->addPosition(addLutversionConformPosition);

			}

		}

		rungeKuttaInvertedList->makeNextOneActive();

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

}

/****************************************************************
 * Method decomposes the inverted Runge-Kutta list.				*
 ****************************************************************/

void lutImplementation::decomposeRungeKuttaInvertedList(double dim3StartEntry, double dim3StopEntry, specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal) {
	
	terminalSequence            statusSequence;
	rungeKuttaInvertedListEntry actualRungeKuttaInvertedListEntry;
	trackCoordinates            actualDigitalTrack;
	digitalHit                  actualDigitalHit;
	prelutHoughBorder           actualPrelutBorder;
	lutHoughBorder              actualLutBorder;
	houghBorderPosition         actualHoughCoord;

	if (rungeKuttaInvertedList == NULL)
		throw cannotAccessRungeKuttaInvertedListError();

	/* Decompose the actually interpolated list into the needed LUTs */
	allocateFileLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry);

	if ((firstLut.type != FILELUT) || (secondLut.type != FILELUT))
		throw wrongLutTypeForUsedFunctionalityDetectedError();

	createTerminalStatusSequence(&statusSequence, terminal, "Decompose Runge-Kutta inverted conform list:\t", (unsigned int)(rungeKuttaInvertedList->getNumberOfEntries()));
	terminalInitialize(statusSequence);

	((prelutAccess*)(firstLut.lutObject))->clear();
	((lutAccess*)(secondLut.lutObject))->clear();

	while (!rungeKuttaInvertedList->isEmpty()) {

		actualRungeKuttaInvertedListEntry = rungeKuttaInvertedList->pop();

		actualDigitalHit                  = actualRungeKuttaInvertedListEntry.getHit();

		actualPrelutBorder.start          = actualRungeKuttaInvertedListEntry.getMinLayer();
		actualPrelutBorder.stop           = actualRungeKuttaInvertedListEntry.getMaxLayer();

		actualLutBorder.houghCoord.clear();
		actualRungeKuttaInvertedListEntry.resetActivePositionObject();
		for (unsigned long i = 0; i < actualRungeKuttaInvertedListEntry.getNumberOfPositions(); i++) {

			actualDigitalTrack    = actualRungeKuttaInvertedListEntry.readActivePositionObjectAndMakeNextOneActive();

			actualHoughCoord.pos1 = actualDigitalTrack.get(DIM1);
			actualHoughCoord.pos2 = actualDigitalTrack.get(DIM2);

			actualLutBorder.houghCoord.push(actualHoughCoord);

		}

		((prelutAccess*)(firstLut.lutObject))->addEntry(actualPrelutBorder, actualDigitalHit);
		((lutAccess*)(secondLut.lutObject))->addEntry(actualLutBorder, actualDigitalHit);

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

}

/****************************************************************
 * Method generates the Runge-Kutta file look-up-tables.		*
 ****************************************************************/

void lutImplementation::generateRungeKuttaFileLuts(std::string prelutFileName, std::string prelutName, std::string lutFileName, std::string lutName, double dim3StartEntry, double dim3StopEntry, trackfinderInputDetector* detector, std::streambuf* terminal) {

	specialList<rungeKuttaListEntry, true, true, true, true, true>         rungeKuttaList;
	specialList<rungeKuttaInvertedListEntry, true, true, true, true, true> rungeKuttaInvertedList;

	rungeKuttaList.clear();
	rungeKuttaInvertedList.clear();

	generateRungeKuttaList(&rungeKuttaList, detector, terminal);

	generateRungeKuttaInvertedList(&rungeKuttaList, &rungeKuttaInvertedList, terminal);

	interpolateRungeKuttaInvertedList(&rungeKuttaInvertedList, terminal);

#if (LUTVERSION > 0)

	makeLutversionConformRungeKuttaInvertedList(&rungeKuttaInvertedList, terminal);

#endif

	decomposeRungeKuttaInvertedList(dim3StartEntry, dim3StopEntry, &rungeKuttaInvertedList, terminal);

	/* Write both look-up-tables into a specified file */
	if (!prelutFileName.empty())
		((prelutAccess*)(firstLut.lutObject))->write(prelutFileName, prelutName, terminal);
	if (!lutFileName.empty())
		((lutAccess*)(secondLut.lutObject))->write(lutFileName, lutName, HARDWAREFORMAT, terminal);

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutImplementation::lutImplementation() 
  : space(NULL),
  isSpaceLocal(false),
  firstLut(),
  secondLut()
{

  //	space               = NULL;
  //	isSpaceLocal        = false;
	firstLut.lutObject  = NULL;
	firstLut.type       = NOLUT;
	secondLut.lutObject = NULL;
	secondLut.type      = NOLUT;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutImplementation::~lutImplementation() {

	deleteMemory();

}

/****************************************************************
 * Method initializes the object.								*
 ****************************************************************/

void lutImplementation::initAnalyticFormulaLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	allocateMathLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry, magneticField);

}
void lutImplementation::initAnalyticFormulaLuts(histogramSpace** _space, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField) {

	allocateSpace(_space);

	allocateMathLuts(&firstLut, &secondLut, dim3StartEntry, dim3StopEntry, magneticField);

}
void lutImplementation::initFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	readFileLuts(prelutFileName, lutFileName, dim3StartEntry, dim3StopEntry, terminal);

}
void lutImplementation::initFileLuts(histogramSpace** _space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal) {

	allocateSpace(_space);

	readFileLuts(prelutFileName, lutFileName, dim3StartEntry, dim3StopEntry, terminal);

}
void lutImplementation::initAnalyticFormulaFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	generateAnalyticFormulaFileLuts(prelutFileName, "Prelut generated by the analytic formula", lutFileName, "Lut generated by the analytic formula", dim3StartEntry, dim3StopEntry, magneticField, terminal);

}
void lutImplementation::initAnalyticFormulaFileLuts(histogramSpace** _space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField, std::streambuf* terminal) {

	allocateSpace(_space);

	generateAnalyticFormulaFileLuts(prelutFileName, "Prelut generated by the analytic formula", lutFileName, "Lut generated by the analytic formula", dim3StartEntry, dim3StopEntry, magneticField, terminal);

}
void lutImplementation::initRungeKuttaFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputDetector* detector, std::streambuf* terminal) {

	allocateSpace(dim1Min, dim1Max, dim1Step, dim2Min, dim2Max, dim2Step, dim3Min, dim3Max, dim3Step);

	generateRungeKuttaFileLuts(prelutFileName, "Prelut generated by the Runge-Kutta approach", lutFileName, "Lut generated by the Runge-Kutta approach", dim3StartEntry, dim3StopEntry, detector, terminal);

}
void lutImplementation::initRungeKuttaFileLuts(histogramSpace** _space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputDetector* detector, std::streambuf* terminal) {

	allocateSpace(_space);

	generateRungeKuttaFileLuts(prelutFileName, "Prelut generated by the Runge-Kutta approach", lutFileName, "Lut generated by the Runge-Kutta approach", dim3StartEntry, dim3StopEntry, detector, terminal);

}

/****************************************************************
 * This method returns true if the magnetism					*
 * functions can be used with the actual LUT type.				*
 ****************************************************************/

bool lutImplementation::typeUsesMagnetism() {

	return (secondLut.type == ANALYTICFORMULALUT);

}

/****************************************************************
 * This method returns true if the correction					*
 * functions can be used with the actual LUT type.				*
 ****************************************************************/

bool lutImplementation::typeUsesCorrections() {

	return (secondLut.type == ANALYTICFORMULALUT);

}

/****************************************************************
 * This method returns the double pointer to the				*
 * histogramSpace object.										*
 ****************************************************************/

histogramSpace** lutImplementation::getSpacePointer() {

	return space;

}

/****************************************************************
 * This method returns the Definition of the prelut object.		*
 ****************************************************************/

prelutDefinition lutImplementation::getPrelutDefinition() {

	prelutDefinition returnValue;

	if (firstLut.lutObject != NULL)
		returnValue = firstLut.lutObject->getPrelutDefinition();
	else
		throw cannotGetDefinitionOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the Definition of the lut object.		*
 ****************************************************************/

lutDefinition lutImplementation::getLutDefinition() {

	lutDefinition returnValue;

	if (secondLut.lutObject != NULL)
		returnValue = secondLut.lutObject->getLutDefinition();
	else
		throw cannotGetDefinitionOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the prelut border.						*
 ****************************************************************/

prelutHoughBorder lutImplementation::getPrelutBorder() {

	prelutHoughBorder returnValue;

	if (firstLut.lutObject != NULL)
		returnValue = firstLut.lutObject->getBorder();
	else
		throw cannotGetBorderOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns the lut border.							*
 ****************************************************************/

lutHoughBorder lutImplementation::getLutBorder() {

	lutHoughBorder returnValue;

	if (secondLut.lutObject != NULL)
		returnValue = secondLut.lutObject->getBorder();
	else
		throw cannotGetBorderOfUndefinedObjectError();

	return returnValue;

}

/****************************************************************
 * This method returns both borders.							*
 ****************************************************************/

lutBorder lutImplementation::getBorder() {

	prelutHoughBorder actualPrelutBorder;
	lutHoughBorder    actualLutBorder;
	lutBorder         returnValue;

	actualPrelutBorder = getPrelutBorder();
	actualLutBorder    = getLutBorder();

	returnValue.setPrelutHoughBorder(actualPrelutBorder);
	returnValue.setLutHoughBorder(actualLutBorder);

	return returnValue;

}

/****************************************************************
 * This method clears the border.								*
 ****************************************************************/

void lutImplementation::clear(lutHoughBorder* borderPointer) {

	if (secondLut.lutObject != NULL)
		secondLut.lutObject->clear(borderPointer);
	else
		throw cannotClearBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method sets the prelut range to use.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setPrelutRange(double rangeStart, double rangeStop) {

	if (firstLut.lutObject != NULL)
		firstLut.lutObject->set(rangeStart, rangeStop);
	else
		throw cannotSetPrelutRangeOfUndefinedObjectError();

}

/****************************************************************
 * This method returns the magnetic field to use.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

trackfinderInputMagneticField* lutImplementation::getMagneticField() {

	trackfinderInputMagneticField* returnValue = NULL;

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getMagneticField();
		else
			throw cannotGetMagneticFieldOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldOfFileObjectWarningMsg* tryToAccessMagneticFieldOfFileObject = new tryToAccessMagneticFieldOfFileObjectWarningMsg();
		tryToAccessMagneticFieldOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldOfFileObject != NULL) {
			delete tryToAccessMagneticFieldOfFileObject;
			tryToAccessMagneticFieldOfFileObject = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method returns the magnetic field factor to use instead	*
 * of the magneticField.										*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

double lutImplementation::getMagneticFieldFactor() {

	double returnValue = 0.0;

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getMagneticFieldFactor();
		else
			throw cannotGetMagneticFieldFactorOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldFactorOfFileObjectWarningMsg* tryToAccessMagneticFieldFactorOfFileObject = new tryToAccessMagneticFieldFactorOfFileObjectWarningMsg();
		tryToAccessMagneticFieldFactorOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldFactorOfFileObject != NULL) {
			delete tryToAccessMagneticFieldFactorOfFileObject;
			tryToAccessMagneticFieldFactorOfFileObject = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method sets the magnetic field to use.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setMagneticField(trackfinderInputMagneticField* magneticField) {

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			secondLut.lutObject->setMagneticField(magneticField);
		else
			throw cannotSetMagneticFieldOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldOfFileObjectWarningMsg* tryToAccessMagneticFieldOfFileObject = new tryToAccessMagneticFieldOfFileObjectWarningMsg();
		tryToAccessMagneticFieldOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldOfFileObject != NULL) {
			delete tryToAccessMagneticFieldOfFileObject;
			tryToAccessMagneticFieldOfFileObject = NULL;
		}

	}

}

/****************************************************************
 * This method sets the magnetic field factor to use instead of	*
 * the magneticField.											*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::setMagneticFieldFactor(double magneticFieldFactor) {

	if (typeUsesMagnetism()) {

		if (secondLut.lutObject != NULL)
			secondLut.lutObject->setMagneticFieldFactor(magneticFieldFactor);
		else
			throw cannotSetMagneticFieldFactorOfUndefinedObjectError();

	}
	else {

		tryToAccessMagneticFieldFactorOfFileObjectWarningMsg* tryToAccessMagneticFieldFactorOfFileObject = new tryToAccessMagneticFieldFactorOfFileObjectWarningMsg();
		tryToAccessMagneticFieldFactorOfFileObject->warningMsg();
		if(tryToAccessMagneticFieldFactorOfFileObject != NULL) {
			delete tryToAccessMagneticFieldFactorOfFileObject;
			tryToAccessMagneticFieldFactorOfFileObject = NULL;
		}

	}

}

/****************************************************************
 * This method returns the number of corrections.				*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

unsigned long lutImplementation::getNumberOfCorrections() {

	unsigned long returnValue;

	if (typeUsesCorrections()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getNumberOfCorrections();
		else
			throw cannotAccessCorrectionsOfUndefinedObjectError();

	}
	else {

		tryToAccessLutCorrectionsOfFileObjectWarningMsg* tryToAccessLutCorrectionsOfFileObject = new tryToAccessLutCorrectionsOfFileObjectWarningMsg();
		tryToAccessLutCorrectionsOfFileObject->warningMsg();
		if(tryToAccessLutCorrectionsOfFileObject != NULL) {
			delete tryToAccessLutCorrectionsOfFileObject;
			tryToAccessLutCorrectionsOfFileObject = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method returns the number of coord corrections.			*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

unsigned long lutImplementation::getNumberOfCoordCorrections() {

	unsigned long returnValue;

	if (typeUsesCorrections()) {

		if (secondLut.lutObject != NULL)
			returnValue = secondLut.lutObject->getNumberOfCoordCorrections();
		else
			throw cannotAccessCorrectionsOfUndefinedObjectError();

	}
	else {

		tryToAccessLutCorrectionsOfFileObjectWarningMsg* tryToAccessLutCorrectionsOfFileObject = new tryToAccessLutCorrectionsOfFileObjectWarningMsg();
		tryToAccessLutCorrectionsOfFileObject->warningMsg();
		if(tryToAccessLutCorrectionsOfFileObject != NULL) {
			delete tryToAccessLutCorrectionsOfFileObject;
			tryToAccessLutCorrectionsOfFileObject = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method resets both correction counters.					*
 * Caution: This function here has no useful					*
 * implementations in some subclasses. So be					*
 * careful when using.											*
 ****************************************************************/

void lutImplementation::resetCorrectionCounter() {

	if (typeUsesCorrections()) {

		if (secondLut.lutObject != NULL)
			secondLut.lutObject->resetCorrectionCounter();
		else
			throw cannotAccessCorrectionsOfUndefinedObjectError();

	}
	else {

		tryToAccessLutCorrectionsOfFileObjectWarningMsg* tryToAccessLutCorrectionsOfFileObject = new tryToAccessLutCorrectionsOfFileObjectWarningMsg();
		tryToAccessLutCorrectionsOfFileObject->warningMsg();
		if(tryToAccessLutCorrectionsOfFileObject != NULL) {
			delete tryToAccessLutCorrectionsOfFileObject;
			tryToAccessLutCorrectionsOfFileObject = NULL;
		}

	}

}

/****************************************************************
 * This method evaluates the prelut table.						*
 ****************************************************************/

void lutImplementation::evaluatePrelut(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	if (firstLut.lutObject != NULL)
		firstLut.lutObject->evaluate(hit, borderPointer);
	else
		throw cannotEvaluateBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method evaluates the lut table.							*
 ****************************************************************/

void lutImplementation::evaluateLut(trackfinderInputHit* hit, lutHoughBorder* borderPointer) {

	if (secondLut.lutObject != NULL)
		secondLut.lutObject->evaluate(hit, borderPointer);
	else
		throw cannotEvaluateBorderOfUndefinedObjectError();

}

/****************************************************************
 * This method evaluates the prelut table.						*
 ****************************************************************/

void lutImplementation::evaluate(trackfinderInputHit* hit, lutBorder* borderPointer) {

	if (borderPointer != NULL) {

		evaluatePrelut(hit, borderPointer->getPrelutHoughBorderPointer());
		evaluateLut(hit, borderPointer->getLutHoughBorderPointer());

	}
	else {

		evaluatePrelut(hit, NULL);
		evaluateLut(hit, NULL);

	}

}
