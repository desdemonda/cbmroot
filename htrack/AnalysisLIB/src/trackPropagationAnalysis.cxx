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
//     - implements methods to display the track propagation quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2009-01-12 17:44:02 $
// $Revision: 1.0 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisError.h"
#include "../include/trackPropagationAnalysis.h"

#include "TStyle.h"
#include "TFile.h"

#include <cstdlib>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void trackPropagationAnalysis::initDisplayStyle() {

	/* setting some globals for the display */

	if (gStyle != NULL) {

		gStyle->SetCanvasBorderMode(-1);
		gStyle->SetCanvasBorderSize(1);
		gStyle->SetCanvasColor(10);

		gStyle->SetFrameFillColor(10);
		gStyle->SetFrameBorderSize(1);
		gStyle->SetFrameBorderMode(-1);
		gStyle->SetFrameLineWidth(2);
		gStyle->SetFrameLineColor(1);

		gStyle->SetHistFillColor(0);
		gStyle->SetHistLineWidth(2);
		gStyle->SetHistLineColor(1);

		gStyle->SetPadColor(10);
		gStyle->SetPadBorderSize(1);
		gStyle->SetPadBorderMode(-1);

		gStyle->SetStatColor(10);
		gStyle->SetStatFont(22);

		gStyle->SetTitleColor(1, "X");
		gStyle->SetTitleOffset(1.0, "X");
		gStyle->SetTitleFont(22, "X");
		gStyle->SetLabelFont(22, "X");

		gStyle->SetTitleColor(1, "Y");
		gStyle->SetTitleOffset(0.8, "Y");
		gStyle->SetTitleFont(22, "Y");
		gStyle->SetLabelFont(22, "Y");

		gStyle->SetOptDate(0);
		gStyle->SetOptTitle(1);
		gStyle->SetOptStat(0);
		gStyle->SetOptFit(1);

	}
	else {

		gStyleNotFoundWarningMsg* gStyleNotFound = new gStyleNotFoundWarningMsg(ANALYSISLIB);
		gStyleNotFound->warningMsg();
		if(gStyleNotFound != NULL) {
			delete gStyleNotFound;
			gStyleNotFound = NULL;
		}

	}

}

/****************************************************************
 * This method initializes the window.							*
 ****************************************************************/

void trackPropagationAnalysis::initWindow() {

	if ((window == NULL) && (enableDisplay)) {

		initDisplayStyle();

		window = new TCanvas("TPWindow", "Track Propagation Analysis", 600, 400);

		numberOfWindowPadColumns = 0;
		numberOfWindowPadRows    = 0;

	}

}

/****************************************************************
 * This method evaluate the window division signature.			*
 ****************************************************************/

void trackPropagationAnalysis::evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature) {

	if ((sizeOfLocalSignature > sizeOfGlobalSignature) || (globalSignature == NULL) || (localSignature == NULL))
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < sizeOfLocalSignature; i++)
		globalSignature[i] |= localSignature[i];

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void trackPropagationAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, bitArray preventPointDraw, bitArray preventHitDraw) {

	bool*          localSignature;
	unsigned short sizeOfLocalSignature;
	bool*          globalSignature;
	unsigned short sizeOfGlobalSignature;
	unsigned short i;

	if ((numberOfEnabledPadColumns == NULL) || (numberOfEnabledPadRows == NULL))
		throw windowDivisionError(ANALYSISLIB);

	(*numberOfEnabledPadColumns) = 0;
	(*numberOfEnabledPadRows)    = 0;

	sizeOfLocalSignature  = 1;
	localSignature        = (bool*)calloc(sizeOfLocalSignature, sizeof(bool));
	if (localSignature == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	sizeOfGlobalSignature = 1;
	globalSignature       = (bool*)calloc(sizeOfGlobalSignature, sizeof(bool));
	if (globalSignature == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	if (isTrackPropagationTotalPointAnalysisEnabled()) {

		trackPropagationTotalPoint->setWindowPadColumn(*numberOfEnabledPadColumns);
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = trackPropagationTotalPoint->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		trackPropagationTotalPoint->getSignatureOfEnabledDisplays(localSignature);
		for (i = 0; i < sizeOfLocalSignature; i++)
			localSignature[i] = (localSignature[i] & !(preventPointDraw[i]));
		if (sizeOfLocalSignature > sizeOfGlobalSignature) {
			globalSignature = (bool*)realloc(globalSignature, sizeOfLocalSignature * sizeof(bool));
			if (globalSignature == NULL)
				throw memoryAllocationError(ANALYSISLIB);
			for (unsigned short _i = sizeOfGlobalSignature; _i < sizeOfLocalSignature; _i++)
				globalSignature[_i] = false;
			sizeOfGlobalSignature = sizeOfLocalSignature;
		}
		evaluateWindowDivisionSignatures(globalSignature, sizeOfGlobalSignature, localSignature, sizeOfLocalSignature);
	
	}

	if (isTrackPropagationTotalHitAnalysisEnabled()) {

		trackPropagationTotalHit->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = trackPropagationTotalHit->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		trackPropagationTotalHit->getSignatureOfEnabledDisplays(localSignature);
		for (i = 0; i < sizeOfLocalSignature; i++)
			localSignature[i] = (localSignature[i] & !(preventHitDraw[i]));
		if (sizeOfLocalSignature > sizeOfGlobalSignature) {
			globalSignature = (bool*)realloc(globalSignature, sizeOfLocalSignature * sizeof(bool));
			if (globalSignature == NULL)
				throw memoryAllocationError(ANALYSISLIB);
			for (unsigned short _i = sizeOfGlobalSignature; _i < sizeOfLocalSignature; _i++)
				globalSignature[_i] = false;
			sizeOfGlobalSignature = sizeOfLocalSignature;
		}
		evaluateWindowDivisionSignatures(globalSignature, sizeOfGlobalSignature, localSignature, sizeOfLocalSignature);

	}

	if (isTrackPropagationTotalPointAnalysisEnabled()) {
		sizeOfLocalSignature = trackPropagationTotalHit->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		for (i = 0; i < sizeOfLocalSignature; i++)
			localSignature[i] = !(preventPointDraw[i]);
		trackPropagationTotalPoint->setWindowPadRow(localSignature, sizeOfLocalSignature, globalSignature, sizeOfGlobalSignature);
	}
	if (isTrackPropagationTotalHitAnalysisEnabled()) {
		sizeOfLocalSignature = trackPropagationTotalHit->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		for (i = 0; i < sizeOfLocalSignature; i++)
			localSignature[i] = !(preventHitDraw[i]);
		trackPropagationTotalHit->setWindowPadRow(localSignature, sizeOfLocalSignature, globalSignature, sizeOfGlobalSignature);
	}

	for (unsigned short j = 0; j < sizeOfGlobalSignature; j++)
		if (globalSignature[j])
			(*numberOfEnabledPadRows)++;

	if (localSignature != NULL) {
		free(localSignature);
		localSignature = NULL;
	}
	if (globalSignature != NULL) {
		free(globalSignature);
		globalSignature = NULL;
	}

}

/****************************************************************
 * This method draws all window pads corresponding to the		*
 * columnId.													*
 ****************************************************************/

void trackPropagationAnalysis::drawWindowPads(unsigned short columnId) {

	unsigned short padId;

	if (window == NULL)
		throw windowNotFoundError();

	padId = columnId + 1;

	if (padId > numberOfWindowPadColumns)
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < numberOfWindowPadRows; i++) {

		window->cd(padId + i * numberOfWindowPadColumns);

		if (isTrackPropagationTotalPointAnalysisEnabled())
			if (columnId == trackPropagationTotalPoint->getWindowPadColumn())
				trackPropagationTotalPoint->drawDisplay(i);

		if (isTrackPropagationTotalHitAnalysisEnabled())
			if (columnId == trackPropagationTotalHit->getWindowPadColumn())
				trackPropagationTotalHit->drawDisplay(i);

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackPropagationAnalysis::trackPropagationAnalysis() 
 : window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   trackPropagationEventPoint(NULL),
   trackPropagationEventHit(NULL),
   trackPropagationTotalPoint(NULL),
   trackPropagationTotalHit(NULL),
   outputFileName()
{
  /*
	window                     = NULL;
	numberOfWindowPadColumns   = 0;
	numberOfWindowPadRows      = 0;
	enableDisplay              = false;
	trackPropagationEventPoint = NULL;
	trackPropagationEventHit   = NULL;
	trackPropagationTotalPoint = NULL;
	trackPropagationTotalHit   = NULL;
	outputFileName.clear();
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPropagationAnalysis::~trackPropagationAnalysis() {

	if (window != NULL) {
		delete window;
		window = NULL;
	}
	if (isTrackPropagationEventPointAnalysisEnabled()) {
		delete trackPropagationEventPoint;
		trackPropagationEventPoint = NULL;
	}
	if (isTrackPropagationEventHitAnalysisEnabled()) {
		delete trackPropagationEventHit;
		trackPropagationEventHit = NULL;
	}
	if (isTrackPropagationTotalPointAnalysisEnabled()) {
		delete trackPropagationTotalPoint;
		trackPropagationTotalPoint = NULL;
	}
	if (isTrackPropagationTotalHitAnalysisEnabled()) {
		delete trackPropagationTotalHit;
		trackPropagationTotalHit = NULL;
	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void trackPropagationAnalysis::initTrackPropagationAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	TFile*      file              = NULL;
	TDirectory* previousDirectory = NULL;

	outputFileName.clear();

	if (isTrackPropagationEventAnalysisEnabled() || isTrackPropagationTotalAnalysisEnabled()) {

		if ((name != NULL) && (strcmp(name, "") != 0)) {

			if (enable) {

				previousDirectory = gDirectory;
	
				if (justUpdate)
					file = new TFile(name, "UPDATE");
				else
					file = new TFile(name, "RECREATE");
	
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, name);

				outputFileName = name;

			}

		}

	}

	if (previousDirectory != NULL)
		gDirectory = previousDirectory;

	if (file != NULL) {
		file->Close();
		if (file->IsOpen())
			throw cannotCloseFileError(ANALYSISLIB, name);
		delete file;
		file = NULL;
	}

	if (isTrackPropagationEventPointAnalysisEnabled())
		trackPropagationEventPoint->initTrackPropagationAnalysisToRoot(enable, name);
	if (isTrackPropagationEventHitAnalysisEnabled())
		trackPropagationEventHit->initTrackPropagationAnalysisToRoot(enable, name);
	if (isTrackPropagationTotalPointAnalysisEnabled())
		trackPropagationTotalPoint->initTrackPropagationAnalysisToRoot(enable, name);
	if (isTrackPropagationTotalHitAnalysisEnabled())
		trackPropagationTotalHit->initTrackPropagationAnalysisToRoot(enable, name);

}

/****************************************************************
 * This method returns true if the momentum's display is written*
 * to file.														*
 ****************************************************************/

bool trackPropagationAnalysis::isTrackPropagationToRootEnabled() {

	bool returnValue = false;

	if (isTrackPropagationEventPointAnalysisEnabled())
		returnValue |= trackPropagationEventPoint->isWritingEnabled();
	if (isTrackPropagationEventHitAnalysisEnabled())
		returnValue |= trackPropagationEventHit->isWritingEnabled();
	if (isTrackPropagationTotalPointAnalysisEnabled())
		returnValue |= trackPropagationTotalPoint->isWritingEnabled();
	if (isTrackPropagationTotalHitAnalysisEnabled())
		returnValue |= trackPropagationTotalHit->isWritingEnabled();

	return returnValue;

}

/****************************************************************
 * This method initializes the momentum's display.				*
 ****************************************************************/

void trackPropagationAnalysis::initTrackPropagationAnalysisDisplay(bool enable) {

	enableDisplay = enable;

	if (enable) {

		if (window == NULL)
			initWindow();

	}
	else {

		if (window != NULL) {
			delete window;
			window = NULL;
		}

	}

}

/****************************************************************
 * This method returns true if the momentum's display is enabled*
 ****************************************************************/

bool trackPropagationAnalysis::isTrackPropagationDisplayEnabled() {

	return enableDisplay;

}

/****************************************************************
 * method initializes the number of displays.					*
 ****************************************************************/

void trackPropagationAnalysis::initNumberOfDisplays(unsigned short numberOfDisplays) {

	unsigned short i;

	for (i = 0; i < numberOfDisplays; i++) {

		if (isTrackPropagationEventPointAnalysisEnabled())
			initTrackPropagationEventPointAnalysis(true, i);
		if (isTrackPropagationEventHitAnalysisEnabled())
			initTrackPropagationEventHitAnalysis(true, i);
		if (isTrackPropagationTotalPointAnalysisEnabled())
			initTrackPropagationTotalPointAnalysis(true, i);
		if (isTrackPropagationTotalHitAnalysisEnabled())
			initTrackPropagationTotalHitAnalysis(true, i);

	}

	if (isTrackPropagationEventPointAnalysisEnabled()) {

		for (i = numberOfDisplays; i < trackPropagationEventPoint->getNumberOfDisplays(); i++)
			initTrackPropagationEventPointAnalysis(false, i);

	}

	if (isTrackPropagationEventHitAnalysisEnabled()) {

		for (i = numberOfDisplays; i < trackPropagationEventHit->getNumberOfDisplays(); i++)
			initTrackPropagationEventHitAnalysis(false, i);

	}

	if (isTrackPropagationTotalPointAnalysisEnabled()) {

		for (i = numberOfDisplays; i < trackPropagationTotalPoint->getNumberOfDisplays(); i++)
			initTrackPropagationTotalPointAnalysis(false, i);

	}

	if (isTrackPropagationTotalHitAnalysisEnabled()) {

		for (i = numberOfDisplays; i < trackPropagationTotalHit->getNumberOfDisplays(); i++)
			initTrackPropagationTotalHitAnalysis(false, i);

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzE-Analysis.		*
 ****************************************************************/

void trackPropagationAnalysis::initTrackPropagationEventPointAnalysis(bool enable, unsigned short index, int nBins, int xMin, int xMax) {

	if (enable) {

		if (trackPropagationEventPoint == NULL)
			trackPropagationEventPoint = new trackPropagationEventPointAnalysis();

		trackPropagationEventPoint->initDistanceDisplay(index, nBins, xMin, xMax);

	}
	else {

		if (isTrackPropagationEventPointAnalysisEnabled()) {

			trackPropagationEventPoint->deleteDistanceDisplay(index);

			if (!trackPropagationEventPoint->isDisplayEnabled()) {
				delete trackPropagationEventPoint;
				trackPropagationEventPoint = NULL;
			}

		}

	}

}
void trackPropagationAnalysis::initTrackPropagationEventHitAnalysis(bool enable, unsigned short index, int nBins, int xMin, int xMax) {

	if (enable) {

		if (trackPropagationEventHit == NULL)
			trackPropagationEventHit = new trackPropagationEventHitAnalysis();

		trackPropagationEventHit->initDistanceDisplay(index, nBins, xMin, xMax);

	}
	else {

		if (isTrackPropagationEventHitAnalysisEnabled()) {

			trackPropagationEventHit->deleteDistanceDisplay(index);

			if (!trackPropagationEventHit->isDisplayEnabled()) {
				delete trackPropagationEventHit;
				trackPropagationEventHit = NULL;
			}

		}

	}

}
void trackPropagationAnalysis::initTrackPropagationTotalPointAnalysis(bool enable, unsigned short index, int nBins, int xMin, int xMax) {

	if (enable) {

		if (trackPropagationTotalPoint == NULL)
			trackPropagationTotalPoint = new trackPropagationTotalPointAnalysis();

		trackPropagationTotalPoint->initDistanceDisplay(index, nBins, xMin, xMax);

	}
	else {

		if (isTrackPropagationTotalPointAnalysisEnabled()) {

			trackPropagationTotalPoint->deleteDistanceDisplay(index);

			if (!trackPropagationTotalPoint->isDisplayEnabled()) {
				delete trackPropagationTotalPoint;
				trackPropagationTotalPoint = NULL;
			}

		}

	}

}
void trackPropagationAnalysis::initTrackPropagationTotalHitAnalysis(bool enable, unsigned short index, int nBins, int xMin, int xMax) {

	if (enable) {

		if (trackPropagationTotalHit == NULL)
			trackPropagationTotalHit = new trackPropagationTotalHitAnalysis();

		trackPropagationTotalHit->initDistanceDisplay(index, nBins, xMin, xMax);

	}
	else {

		if (isTrackPropagationTotalHitAnalysisEnabled()) {

			trackPropagationTotalHit->deleteDistanceDisplay(index);

			if (!trackPropagationTotalHit->isDisplayEnabled()) {
				delete trackPropagationTotalHit;
				trackPropagationTotalHit = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void trackPropagationAnalysis::initializeEvaluation() {

	if (isTrackPropagationEventPointAnalysisEnabled())
		trackPropagationEventPoint->initializeEvaluation();
	if (isTrackPropagationEventHitAnalysisEnabled())
		trackPropagationEventHit->initializeEvaluation();
	if (isTrackPropagationTotalPointAnalysisEnabled())
		trackPropagationTotalPoint->initializeEvaluation();
	if (isTrackPropagationTotalHitAnalysisEnabled())
		trackPropagationTotalHit->initializeEvaluation();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void trackPropagationAnalysis::finalizeEvaluation() {

	if (isTrackPropagationEventPointAnalysisEnabled())
		trackPropagationEventPoint->finalizeEvaluation();
	if (isTrackPropagationEventHitAnalysisEnabled())
		trackPropagationEventHit->finalizeEvaluation();
	if (isTrackPropagationTotalPointAnalysisEnabled())
		trackPropagationTotalPoint->finalizeEvaluation();
	if (isTrackPropagationTotalHitAnalysisEnabled())
		trackPropagationTotalHit->finalizeEvaluation();

}

/****************************************************************
 * method adds a point distance.								*
 ****************************************************************/

void trackPropagationAnalysis::addPointDistance(unsigned short index, double distance) {

	if (isTrackPropagationEventPointAnalysisEnabled())
		trackPropagationEventPoint->addDistance(index, distance);
	if (isTrackPropagationTotalPointAnalysisEnabled())
		trackPropagationTotalPoint->addDistance(index, distance);

}

/****************************************************************
 * method adds a hit distance.									*
 ****************************************************************/

void trackPropagationAnalysis::addHitDistance(unsigned short index, double distance) {

	if (isTrackPropagationEventHitAnalysisEnabled())
		trackPropagationEventHit->addDistance(index, distance);
	if (isTrackPropagationTotalHitAnalysisEnabled())
		trackPropagationTotalHit->addDistance(index, distance);

}

/****************************************************************
 * method returns true, if the momentumEFGCEventPz-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool trackPropagationAnalysis::isTrackPropagationEventPointAnalysisEnabled() {

	bool returnValue;

	if (trackPropagationEventPoint != NULL)
		returnValue = trackPropagationEventPoint->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the momentumEFGCTotalPz-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool trackPropagationAnalysis::isTrackPropagationEventHitAnalysisEnabled() {

	bool returnValue;

	if (trackPropagationEventHit != NULL)
		returnValue = trackPropagationEventHit->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the momentumEFGCEventPt-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool trackPropagationAnalysis::isTrackPropagationTotalPointAnalysisEnabled() {

	bool returnValue;

	if (trackPropagationTotalPoint != NULL)
		returnValue = trackPropagationTotalPoint->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the momentumEFGCTotalPt-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool trackPropagationAnalysis::isTrackPropagationTotalHitAnalysisEnabled() {

	bool returnValue;

	if (trackPropagationTotalHit != NULL)
		returnValue = trackPropagationTotalHit->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}
bool trackPropagationAnalysis::isTrackPropagationEventAnalysisEnabled() {

	return (isTrackPropagationEventPointAnalysisEnabled() || isTrackPropagationEventHitAnalysisEnabled());

}
bool trackPropagationAnalysis::isTrackPropagationTotalAnalysisEnabled() {

	return (isTrackPropagationTotalPointAnalysisEnabled() || isTrackPropagationTotalHitAnalysisEnabled());

}

/****************************************************************
 * This method updates the window.								*
 ****************************************************************/

void trackPropagationAnalysis::trackPropagationAnalysisUpdate() {

	if (enableDisplay) {

		if (window != NULL) {

			window->Update();
			window->Paint();
			/* 
			 * Because of double buffering the window must be updated two times
			 * to show the actual update of the data.
			 * The method window->setDoubleBuffer(0) is not working until now.
			 */
			window->Update();
			window->Paint();

		}
		else
			trackPropagationAnalysisDraw();

	}

}

/****************************************************************
 * This method draws the window.								*
 ****************************************************************/

void trackPropagationAnalysis::trackPropagationAnalysisDraw(bitArray preventPointDraw, bitArray preventHitDraw) {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;

	if (enableDisplay) {

		evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows, preventPointDraw, preventHitDraw);

		if ((numberOfWindowPadColumns != 0) || (numberOfWindowPadRows != 0)) {

			if ((numberOfWindowPadColumns != numberOfEnabledPadColumns) || (numberOfWindowPadRows != numberOfEnabledPadRows)) {

				if (window != NULL) {
					delete window;
					window = NULL;
				}

			}

		}

		if (window == NULL)
			initWindow();

		if ((numberOfWindowPadColumns == 0) && (numberOfWindowPadRows == 0)) {

			numberOfWindowPadColumns = numberOfEnabledPadColumns;
			numberOfWindowPadRows    = numberOfEnabledPadRows;
			window->Divide(numberOfWindowPadColumns, numberOfWindowPadRows, 0, 0, 0);
	
		}

		for (unsigned short i = 0; i < numberOfWindowPadColumns; i++)
			drawWindowPads(i);

		window->cd();

	}

}

/****************************************************************
 * method writes the momentumEvent-Analysis for each event into	*
 * a root file.													*
 ****************************************************************/

void trackPropagationAnalysis::trackPropagationEventAnalysisWrite(int eventNumber) {

	if (isTrackPropagationEventPointAnalysisEnabled())
		if (trackPropagationEventPoint->isDisplayEnabled())
			trackPropagationEventPoint->writeDistanceDisplays(eventNumber, outputFileName);

	if (isTrackPropagationEventHitAnalysisEnabled())
		if (trackPropagationEventHit->isDisplayEnabled())
			trackPropagationEventHit->writeDistanceDisplays(eventNumber, outputFileName);

}

/****************************************************************
 * method writes the momentumTotal-Analysis over all events into*
 * a root file.													*
 ****************************************************************/

void trackPropagationAnalysis::trackPropagationTotalAnalysisWrite(int eventNumber) {

	if (isTrackPropagationTotalPointAnalysisEnabled())
		if (trackPropagationTotalPoint->isDisplayEnabled())
			trackPropagationTotalPoint->writeDistanceDisplays(eventNumber, outputFileName);

	if (isTrackPropagationTotalHitAnalysisEnabled())
		if (trackPropagationTotalHit->isDisplayEnabled())
			trackPropagationTotalHit->writeDistanceDisplays(eventNumber, outputFileName);

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumEFGCEvent-Analysis.									*
 ****************************************************************/

std::string trackPropagationAnalysis::getTrackPropagationEventAnalysis() {

	std::string    returnValue;
	unsigned short i;
	int            percentBuffer;
	char           buffer[intConversionDigits + 1];

	returnValue = "";

	if (isTrackPropagationEventPointAnalysisEnabled()) {

		returnValue += "\nTrackPropagationEventPointAnalysis\n";
		returnValue += "-------------------------\n";
		for (i = 0; i < trackPropagationEventPoint->getNumberOfDisplays(); i++) {
			if (trackPropagationEventPoint->isDistanceDisplayEnabled(i)) {
				returnValue += "Track propagation for event input point data of station ";
				ustos(i, buffer, 10, shortConversionDigits);
				returnValue += buffer;
				returnValue += ":\n";
				returnValue += " The xaxi range is from ";
				uitos(trackPropagationEventPoint->getDistanceDisplayAxiRange(i).min, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " to ";
				uitos(trackPropagationEventPoint->getDistanceDisplayAxiRange(i).max, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += ".\n There are ";
				uitos(trackPropagationEventPoint->getDistanceDisplayAxiRange(i).numberOfUnderflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationEventPoint->getDistanceDisplayAxiRange(i).numberOfUnderflows) / ((double)trackPropagationEventPoint->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) underflows and ";
				uitos(trackPropagationEventPoint->getDistanceDisplayAxiRange(i).numberOfOverflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationEventPoint->getDistanceDisplayAxiRange(i).numberOfOverflows) / ((double)trackPropagationEventPoint->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) overflows.\n";
			}
		}

	}

	if (isTrackPropagationEventHitAnalysisEnabled()) {

		returnValue += "\nTrackPropagationEventHitAnalysis\n";
		returnValue += "-------------------------\n";
		for (i = 0; i < trackPropagationEventHit->getNumberOfDisplays(); i++) {
			if (trackPropagationEventHit->isDistanceDisplayEnabled(i)) {
				returnValue += "Track propagation for event input hit data of station ";
				ustos(i, buffer, 10, shortConversionDigits);
				returnValue += buffer;
				returnValue += ":\n";
				returnValue += " The xaxi range is from ";
				uitos(trackPropagationEventHit->getDistanceDisplayAxiRange(i).min, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " to ";
				uitos(trackPropagationEventHit->getDistanceDisplayAxiRange(i).max, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += ".\n There are ";
				uitos(trackPropagationEventHit->getDistanceDisplayAxiRange(i).numberOfUnderflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationEventHit->getDistanceDisplayAxiRange(i).numberOfUnderflows) / ((double)trackPropagationEventHit->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) underflows and ";
				uitos(trackPropagationEventHit->getDistanceDisplayAxiRange(i).numberOfOverflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationEventHit->getDistanceDisplayAxiRange(i).numberOfOverflows) / ((double)trackPropagationEventHit->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) overflows.\n";
			}
		}

	}

	if ((isTrackPropagationEventPointAnalysisEnabled()) || (isTrackPropagationEventHitAnalysisEnabled())) {

		returnValue += "\nAll underflows are set into the first entry,\n";
		returnValue += "while all overflows are set into the last one.\n";

	}

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumEFGCTotal-Analysis.									*
 ****************************************************************/

std::string trackPropagationAnalysis::getTrackPropagationTotalAnalysis() {

	std::string    returnValue;
	unsigned short i;
	int            percentBuffer;
	char           buffer[intConversionDigits + 1];

	returnValue = "";

	if (isTrackPropagationTotalPointAnalysisEnabled()) {

		returnValue += "\nTrackPropagationTotalPointAnalysis\n";
		returnValue += "-------------------------\n";
		for (i = 0; i < trackPropagationTotalPoint->getNumberOfDisplays(); i++) {
			if (trackPropagationTotalPoint->isDistanceDisplayEnabled(i)) {
				returnValue += "Track propagation for total input point data of station ";
				ustos(i, buffer, 10, shortConversionDigits);
				returnValue += buffer;
				returnValue += ":\n";
				returnValue += " The xaxi range is from ";
				uitos(trackPropagationTotalPoint->getDistanceDisplayAxiRange(i).min, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " to ";
				uitos(trackPropagationTotalPoint->getDistanceDisplayAxiRange(i).max, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += ".\n There are ";
				uitos(trackPropagationTotalPoint->getDistanceDisplayAxiRange(i).numberOfUnderflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationTotalPoint->getDistanceDisplayAxiRange(i).numberOfUnderflows) / ((double)trackPropagationTotalPoint->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) underflows and ";
				uitos(trackPropagationTotalPoint->getDistanceDisplayAxiRange(i).numberOfOverflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationTotalPoint->getDistanceDisplayAxiRange(i).numberOfOverflows) / ((double)trackPropagationTotalPoint->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) overflows.\n";
			}
		}

	}

	if (isTrackPropagationTotalHitAnalysisEnabled()) {

		returnValue += "\nTrackPropagationTotalHitAnalysis\n";
		returnValue += "-------------------------\n";
		for (i = 0; i < trackPropagationTotalHit->getNumberOfDisplays(); i++) {
			if (trackPropagationTotalHit->isDistanceDisplayEnabled(i)) {
				returnValue += "Track propagation for total input hit data of station ";
				ustos(i, buffer, 10, shortConversionDigits);
				returnValue += buffer;
				returnValue += ":\n";
				returnValue += " The xaxi range is from ";
				uitos(trackPropagationTotalHit->getDistanceDisplayAxiRange(i).min, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " to ";
				uitos(trackPropagationTotalHit->getDistanceDisplayAxiRange(i).max, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += ".\n There are ";
				uitos(trackPropagationTotalHit->getDistanceDisplayAxiRange(i).numberOfUnderflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationTotalHit->getDistanceDisplayAxiRange(i).numberOfUnderflows) / ((double)trackPropagationTotalHit->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) underflows and ";
				uitos(trackPropagationTotalHit->getDistanceDisplayAxiRange(i).numberOfOverflows, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += " (";
				percentBuffer = (unsigned short)((double)((int)(100 * ((double)trackPropagationTotalHit->getDistanceDisplayAxiRange(i).numberOfOverflows) / ((double)trackPropagationTotalHit->getDistanceDisplayEntries(i)) + 0.5)));
				uitos(percentBuffer, buffer, 10, intConversionDigits);
				returnValue += buffer;
				returnValue += "%) overflows.\n";
			}
		}

	}

	if ((isTrackPropagationTotalPointAnalysisEnabled()) || (isTrackPropagationTotalHitAnalysisEnabled())) {

		returnValue += "\nAll underflows are set into the first entry,\n";
		returnValue += "while all overflows are set into the last one.\n";

	}

	return returnValue;

}
