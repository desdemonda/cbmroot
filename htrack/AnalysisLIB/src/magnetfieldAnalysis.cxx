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
//     - implements methods to display the trackfinding algorithm's quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-09-11 14:07:02 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisError.h"
#include "../include/magnetfieldAnalysis.h"

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

void magnetfieldAnalysis::initDisplayStyle() {

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

void magnetfieldAnalysis::initWindow() {

	if ((window == NULL) && (enableDisplay)) {

		initDisplayStyle();

		window = new TCanvas("MFWindow", "Magnetfield Analysis", 600, 400);

		numberOfWindowPadColumns = 0;
		numberOfWindowPadRows    = 0;

	}

}

/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldAnalysis::setAutomaticAxisRange() {

	if (magnetfieldX != NULL)
		magnetfieldX->setAutomaticAxisRange();
	if (magnetfieldY != NULL)
		magnetfieldY->setAutomaticAxisRange();
	if (magnetfieldZ != NULL)
		magnetfieldZ->setAutomaticAxisRange();

}

/****************************************************************
 * This method evaluate the window division signature.			*
 ****************************************************************/

void magnetfieldAnalysis::evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature) {

	if ((sizeOfLocalSignature > sizeOfGlobalSignature) || (globalSignature == NULL) || (localSignature == NULL))
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < sizeOfLocalSignature; i++)
		globalSignature[i] |= localSignature[i];

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void magnetfieldAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows) {

	bool*          localSignature;
	unsigned short sizeOfLocalSignature;
	bool*          globalSignature;
	unsigned short sizeOfGlobalSignature;

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

	if (isMagnetfieldXAnalysisEnabled()) {

		magnetfieldX->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = magnetfieldX->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		magnetfieldX->getSignatureOfEnabledDisplays(localSignature);
		if (sizeOfLocalSignature > sizeOfGlobalSignature) {
			globalSignature = (bool*)realloc(globalSignature, sizeOfLocalSignature * sizeof(bool));
			if (globalSignature == NULL)
				throw memoryAllocationError(ANALYSISLIB);
			for (unsigned short i = sizeOfGlobalSignature; i < sizeOfLocalSignature; i++)
				globalSignature[i] = false;
			sizeOfGlobalSignature = sizeOfLocalSignature;
		}
		evaluateWindowDivisionSignatures(globalSignature, sizeOfGlobalSignature, localSignature, sizeOfLocalSignature);

	}

	if (isMagnetfieldYAnalysisEnabled()) {

		magnetfieldY->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = magnetfieldY->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		magnetfieldY->getSignatureOfEnabledDisplays(localSignature);
		if (sizeOfLocalSignature > sizeOfGlobalSignature) {
			globalSignature = (bool*)realloc(globalSignature, sizeOfLocalSignature * sizeof(bool));
			if (globalSignature == NULL)
				throw memoryAllocationError(ANALYSISLIB);
			for (unsigned short i = sizeOfGlobalSignature; i < sizeOfLocalSignature; i++)
				globalSignature[i] = false;
			sizeOfGlobalSignature = sizeOfLocalSignature;
		}
		evaluateWindowDivisionSignatures(globalSignature, sizeOfGlobalSignature, localSignature, sizeOfLocalSignature);
	
	}

	if (isMagnetfieldZAnalysisEnabled()) {

		magnetfieldZ->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = magnetfieldZ->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		magnetfieldZ->getSignatureOfEnabledDisplays(localSignature);
		if (sizeOfLocalSignature > sizeOfGlobalSignature) {
			globalSignature = (bool*)realloc(globalSignature, sizeOfLocalSignature * sizeof(bool));
			if (globalSignature == NULL)
				throw memoryAllocationError(ANALYSISLIB);
			for (unsigned short i = sizeOfGlobalSignature; i < sizeOfLocalSignature; i++)
				globalSignature[i] = false;
			sizeOfGlobalSignature = sizeOfLocalSignature;
		}
		evaluateWindowDivisionSignatures(globalSignature, sizeOfGlobalSignature, localSignature, sizeOfLocalSignature);
	
	}

	if (isMagnetfieldXAnalysisEnabled())
		magnetfieldX->setWindowPadRow(globalSignature, sizeOfGlobalSignature);
	if (isMagnetfieldYAnalysisEnabled())
		magnetfieldY->setWindowPadRow(globalSignature, sizeOfGlobalSignature);
	if (isMagnetfieldZAnalysisEnabled())
		magnetfieldZ->setWindowPadRow(globalSignature, sizeOfGlobalSignature);

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

void magnetfieldAnalysis::drawWindowPads(unsigned short columnId) {

	unsigned short padId;

	if (window == NULL)
		throw windowNotFoundError();

	padId = columnId + 1;

	if (padId > numberOfWindowPadColumns)
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < numberOfWindowPadRows; i++) {

		window->cd(padId + i * numberOfWindowPadColumns);

		if (isMagnetfieldXAnalysisEnabled())
			if (columnId == magnetfieldX->getWindowPadColumn())
				magnetfieldX->drawDisplay(i);

		if (isMagnetfieldYAnalysisEnabled())
			if (columnId == magnetfieldY->getWindowPadColumn())
				magnetfieldY->drawDisplay(i);

		if (isMagnetfieldZAnalysisEnabled())
			if (columnId == magnetfieldZ->getWindowPadColumn())
				magnetfieldZ->drawDisplay(i);

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magnetfieldAnalysis::magnetfieldAnalysis() 
 : window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   magnetfieldX(NULL),
   magnetfieldY(NULL),
   magnetfieldZ(NULL),
   outputFileName()
{
  /*
	window                   = NULL;
	numberOfWindowPadColumns = 0;
	numberOfWindowPadRows    = 0;
	enableDisplay            = false;
	magnetfieldX             = NULL;
	magnetfieldY             = NULL;
	magnetfieldZ             = NULL;
	outputFileName.clear();
*/
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magnetfieldAnalysis::~magnetfieldAnalysis() {

	if (window != NULL) {
		delete window;
		window = NULL;
	}
	if (isMagnetfieldXAnalysisEnabled()) {
		delete magnetfieldX;
		magnetfieldX = NULL;
	}
	if (isMagnetfieldYAnalysisEnabled()) {
		delete magnetfieldY;
		magnetfieldY = NULL;
	}
	if (isMagnetfieldZAnalysisEnabled()) {
		delete magnetfieldZ;
		magnetfieldZ = NULL;
	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	TFile*      file              = NULL;
	TDirectory* previousDirectory = NULL;

	outputFileName.clear();

	if (isMagnetfieldXAnalysisEnabled() || isMagnetfieldYAnalysisEnabled() || isMagnetfieldZAnalysisEnabled()) {

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

	if (isMagnetfieldXAnalysisEnabled())
		magnetfieldX->initMagnetfieldAnalysisToRoot(enable, name);
	if (isMagnetfieldYAnalysisEnabled())
		magnetfieldY->initMagnetfieldAnalysisToRoot(enable, name);
	if (isMagnetfieldZAnalysisEnabled())
		magnetfieldZ->initMagnetfieldAnalysisToRoot(enable, name);

}

/****************************************************************
 * This method returns true if the magnetfield's display is		*
 * written to file.												*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldToRootEnabled() {

	bool returnValue = false;

	if (isMagnetfieldXAnalysisEnabled())
		returnValue |= magnetfieldX->isWritingEnabled();
	if (isMagnetfieldYAnalysisEnabled())
		returnValue |= magnetfieldY->isWritingEnabled();
	if (isMagnetfieldZAnalysisEnabled())
		returnValue |= magnetfieldZ->isWritingEnabled();

	return returnValue;

}

/****************************************************************
 * This method initializes the graphic's display.				*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldAnalysisDisplay(bool enable) {

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
 * This method returns true if the graphic's display is enabled.*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldDisplayEnabled() {

	return enableDisplay;

}

/****************************************************************
 * method initializes the initMagnetfieldXBxAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldXBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldX == NULL)
			magnetfieldX = new magnetfieldXAnalysis();

		magnetfieldX->initBxDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldXAnalysisEnabled()) {

			magnetfieldX->deleteBxDisplay();

			if (!magnetfieldX->isDisplayEnabled()) {
				delete magnetfieldX;
				magnetfieldX = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXByAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldXByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldX == NULL)
			magnetfieldX = new magnetfieldXAnalysis();

		magnetfieldX->initByDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldXAnalysisEnabled()) {

			magnetfieldX->deleteByDisplay();

			if (!magnetfieldX->isDisplayEnabled()) {
				delete magnetfieldX;
				magnetfieldX = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXBzAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldXBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldX == NULL)
			magnetfieldX = new magnetfieldXAnalysis();

		magnetfieldX->initBzDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldXAnalysisEnabled()) {

			magnetfieldX->deleteBzDisplay();

			if (!magnetfieldX->isDisplayEnabled()) {
				delete magnetfieldX;
				magnetfieldX = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldXAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldXAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	initMagnetfieldXBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);
	initMagnetfieldXByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);
	initMagnetfieldXBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

}

/****************************************************************
 * method initializes the initMagnetfieldYBxAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldYBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldY == NULL)
			magnetfieldY = new magnetfieldYAnalysis();

		magnetfieldY->initBxDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldYAnalysisEnabled()) {

			magnetfieldY->deleteBxDisplay();

			if (!magnetfieldY->isDisplayEnabled()) {
				delete magnetfieldY;
				magnetfieldY = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYByAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldYByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldY == NULL)
			magnetfieldY = new magnetfieldYAnalysis();

		magnetfieldY->initByDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldYAnalysisEnabled()) {

			magnetfieldY->deleteByDisplay();

			if (!magnetfieldY->isDisplayEnabled()) {
				delete magnetfieldY;
				magnetfieldY = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYBzAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldYBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldY == NULL)
			magnetfieldY = new magnetfieldYAnalysis();

		magnetfieldY->initBzDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldYAnalysisEnabled()) {

			magnetfieldY->deleteBzDisplay();

			if (!magnetfieldY->isDisplayEnabled()) {
				delete magnetfieldY;
				magnetfieldY = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldYAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldYAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	initMagnetfieldYBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);
	initMagnetfieldYByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);
	initMagnetfieldYBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

}

/****************************************************************
 * method initializes the initMagnetfieldZBxAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldZBxAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldZ == NULL)
			magnetfieldZ = new magnetfieldZAnalysis();

		magnetfieldZ->initBxDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldZAnalysisEnabled()) {

			magnetfieldZ->deleteBxDisplay();

			if (!magnetfieldZ->isDisplayEnabled()) {
				delete magnetfieldZ;
				magnetfieldZ = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZByAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldZByAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldZ == NULL)
			magnetfieldZ = new magnetfieldZAnalysis();

		magnetfieldZ->initByDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldZAnalysisEnabled()) {

			magnetfieldZ->deleteByDisplay();

			if (!magnetfieldZ->isDisplayEnabled()) {
				delete magnetfieldZ;
				magnetfieldZ = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZBzAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldZBzAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	if (enable) {

		if (magnetfieldZ == NULL)
			magnetfieldZ = new magnetfieldZAnalysis();

		magnetfieldZ->initBzDisplay(nBins, min, max, constValDim1, constValDim2);

	}
	else {

		if (isMagnetfieldZAnalysisEnabled()) {

			magnetfieldZ->deleteBzDisplay();

			if (!magnetfieldZ->isDisplayEnabled()) {
				delete magnetfieldZ;
				magnetfieldZ = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the initMagnetfieldZAnalysis-Analysis.	*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldZAnalysis(bool enable, int nBins, double min, double max, double constValDim1, double constValDim2) {

	initMagnetfieldZBxAnalysis(enable, nBins, min, max, constValDim1, constValDim2);
	initMagnetfieldZByAnalysis(enable, nBins, min, max, constValDim1, constValDim2);
	initMagnetfieldZBzAnalysis(enable, nBins, min, max, constValDim1, constValDim2);

}

/****************************************************************
 * method initializes the initMagnetfieldAnalysis-Analysis.		*
 ****************************************************************/

void magnetfieldAnalysis::initMagnetfieldAnalysis(bool enable, int nBins, double xMin, double yMin, double zMin, double xMax, double yMax, double zMax, double constValDim1, double constValDim2, double constValDim3) {

	initMagnetfieldXAnalysis(enable, nBins, xMin, xMax, constValDim2, constValDim3);
	initMagnetfieldYAnalysis(enable, nBins, yMin, yMax, constValDim1, constValDim3);
	initMagnetfieldZAnalysis(enable, nBins, zMin, zMax, constValDim1, constValDim2);

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void magnetfieldAnalysis::initializeEvaluation() {

	if (isMagnetfieldXAnalysisEnabled())
		magnetfieldX->initializeEvaluation();
	if (isMagnetfieldYAnalysisEnabled())
		magnetfieldY->initializeEvaluation();
	if (isMagnetfieldZAnalysisEnabled())
		magnetfieldZ->initializeEvaluation();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void magnetfieldAnalysis::finalizeEvaluation() {

	if (isMagnetfieldXAnalysisEnabled())
		magnetfieldX->finalizeEvaluation();
	if (isMagnetfieldYAnalysisEnabled())
		magnetfieldY->finalizeEvaluation();
	if (isMagnetfieldZAnalysisEnabled())
		magnetfieldZ->finalizeEvaluation();

}

/****************************************************************
 * method returns true, if the magnetfieldXBx-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldXBxAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->isBxDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldXBy-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldXByAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->isByDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldXBz-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldXBzAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->isBzDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldX-Analysis is			*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldXAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldYBx-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldYBxAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->isBxDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldYBy-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldYByAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->isByDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldYBz-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldYBzAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->isBzDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldY-Analysis is			*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldYAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldZBx-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldZBxAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->isBxDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the magnetfieldZBy-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldZByAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->isByDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}
/****************************************************************
 * method returns true, if the magnetfieldZBz-Analysis is		*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldZBzAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->isBzDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}
/****************************************************************
 * method returns true, if the magnetfieldZ-Analysis is			*
 * enabled.														*
 ****************************************************************/

bool magnetfieldAnalysis::isMagnetfieldZAnalysisEnabled() {

	bool returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/**
 * This method returns the xbxDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getXBxDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldX != NULL)
		returnValue = &(magnetfieldX->getBxDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the xbyDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getXByDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldX != NULL)
		returnValue = &(magnetfieldX->getByDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the xbzDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getXBzDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldX != NULL)
		returnValue = &(magnetfieldX->getBzDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the ybxDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getYBxDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldY != NULL)
		returnValue = &(magnetfieldY->getBxDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the ybyDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getYByDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldY != NULL)
		returnValue = &(magnetfieldY->getByDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the ybzDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getYBzDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldY != NULL)
		returnValue = &(magnetfieldY->getBzDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the zbxDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getZBxDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldZ != NULL)
		returnValue = &(magnetfieldZ->getBxDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the zbyDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getZByDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldZ != NULL)
		returnValue = &(magnetfieldZ->getByDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the zbzDisplay's axi range.
 */

magnetfieldDisplayInfo* magnetfieldAnalysis::getZBzDisplayAxiRange() {

	magnetfieldDisplayInfo* returnValue;

	if (magnetfieldZ != NULL)
		returnValue = &(magnetfieldZ->getBzDisplayAxiRange());
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * This method returns the number of xBxDisplay entries.
 */

unsigned int magnetfieldAnalysis::getXBxDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->getBxDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of xByDisplay entries.
 */

unsigned int magnetfieldAnalysis::getXByDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->getByDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of xBzDisplay entries.
 */

unsigned int magnetfieldAnalysis::getXBzDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldX != NULL)
		returnValue = magnetfieldX->getBzDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of yBxDisplay entries.
 */

unsigned int magnetfieldAnalysis::getYBxDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->getBxDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of yByDisplay entries.
 */

unsigned int magnetfieldAnalysis::getYByDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->getByDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of yBzDisplay entries.
 */

unsigned int magnetfieldAnalysis::getYBzDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldY != NULL)
		returnValue = magnetfieldY->getBzDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of zBxDisplay entries.
 */

unsigned int magnetfieldAnalysis::getZBxDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->getBxDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of zByDisplay entries.
 */

unsigned int magnetfieldAnalysis::getZByDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->getByDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method returns the number of zBzDisplay entries.
 */

unsigned int magnetfieldAnalysis::getZBzDisplayEntries() {

	unsigned int returnValue;

	if (magnetfieldZ != NULL)
		returnValue = magnetfieldZ->getBzDisplayEntries();
	else
		returnValue = 0;

	return returnValue;

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the xbxDisplay.
 */

void magnetfieldAnalysis::setXBxPoint(int index, double xValue, double yValue) {

	if (magnetfieldX != NULL)
		magnetfieldX->setBxPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the xbyDisplay.
 */

void magnetfieldAnalysis::setXByPoint(int index, double xValue, double yValue) {

	if (magnetfieldX != NULL)
		magnetfieldX->setByPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the xbzDisplay.
 */

void magnetfieldAnalysis::setXBzPoint(int index, double xValue, double yValue) {

	if (magnetfieldX != NULL)
		magnetfieldX->setBzPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the ybxDisplay.
 */

void magnetfieldAnalysis::setYBxPoint(int index, double xValue, double yValue) {

	if (magnetfieldY != NULL)
		magnetfieldY->setBxPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the ybyDisplay.
 */

void magnetfieldAnalysis::setYByPoint(int index, double xValue, double yValue) {

	if (magnetfieldY != NULL)
		magnetfieldY->setByPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the ybzDisplay.
 */

void magnetfieldAnalysis::setYBzPoint(int index, double xValue, double yValue) {

	if (magnetfieldY != NULL)
		magnetfieldY->setBzPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the zbxDisplay.
 */

void magnetfieldAnalysis::setZBxPoint(int index, double xValue, double yValue) {

	if (magnetfieldZ != NULL)
		magnetfieldZ->setBxPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the zbyDisplay.
 */

void magnetfieldAnalysis::setZByPoint(int index, double xValue, double yValue) {

	if (magnetfieldZ != NULL)
		magnetfieldZ->setByPoint(index, xValue, yValue);

}

/**
 * This method sets the values for x and y at the given point
 * at position index in the zbzDisplay.
 */

void magnetfieldAnalysis::setZBzPoint(int index, double xValue, double yValue) {

	if (magnetfieldZ != NULL)
		magnetfieldZ->setBzPoint(index, xValue, yValue);

}

/****************************************************************
 * This method updates the window.								*
 ****************************************************************/

void magnetfieldAnalysis::magnetfieldAnalysisUpdate() {

	if (enableDisplay) {

		if (window != NULL) {

			setAutomaticAxisRange();
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
			magnetfieldAnalysisDraw();

	}

}

/****************************************************************
 * This method draws the window.								*
 ****************************************************************/

void magnetfieldAnalysis::magnetfieldAnalysisDraw() {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;

	if (enableDisplay) {

		evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows);

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

		setAutomaticAxisRange();

		for (unsigned short i = 0; i < numberOfWindowPadColumns; i++)
			drawWindowPads(i);

		window->cd();

	}

}

/****************************************************************
 * method writes the magnetfield-Analysis into					*
 * a root file.													*
 ****************************************************************/

void magnetfieldAnalysis::magnetfieldAnalysisWrite() {

	setAutomaticAxisRange();

	if (isMagnetfieldXAnalysisEnabled()) {

		if (magnetfieldX->isBxDisplayEnabled())
			magnetfieldX->writeBxDisplay(outputFileName);
		if (magnetfieldX->isByDisplayEnabled())
			magnetfieldX->writeByDisplay(outputFileName);
		if (magnetfieldX->isBzDisplayEnabled())
			magnetfieldX->writeBzDisplay(outputFileName);

	}

	if (isMagnetfieldYAnalysisEnabled()) {

		if (magnetfieldY->isBxDisplayEnabled())
			magnetfieldY->writeBxDisplay(outputFileName);
		if (magnetfieldY->isByDisplayEnabled())
			magnetfieldY->writeByDisplay(outputFileName);
		if (magnetfieldY->isBzDisplayEnabled())
			magnetfieldY->writeBzDisplay(outputFileName);

	}

	if (isMagnetfieldZAnalysisEnabled()) {

		if (magnetfieldZ->isBxDisplayEnabled())
			magnetfieldZ->writeBxDisplay(outputFileName);
		if (magnetfieldZ->isByDisplayEnabled())
			magnetfieldZ->writeByDisplay(outputFileName);
		if (magnetfieldZ->isBzDisplayEnabled())
			magnetfieldZ->writeBzDisplay(outputFileName);

	}

}

/**
 * method  returns true, if the magnetfield-Analysis
 * is enabled.
 */

bool magnetfieldAnalysis::isMagnetfieldAnalysisEnabled() {

	bool returnValue;

	if (isMagnetfieldXAnalysisEnabled() || isMagnetfieldYAnalysisEnabled() || isMagnetfieldZAnalysisEnabled())
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * magnetfield-Analysis.										*
 ****************************************************************/

std::string magnetfieldAnalysis::getMagnetfieldAnalysis() {

	std::string returnValue;
	int         percentBuffer;
	char        intBuffer[intConversionDigits + 1];
	char        doubleBuffer[doubleConversion + 1];

	returnValue = "";

	if (isMagnetfieldXAnalysisEnabled()) {

		returnValue += "\nMagnetfieldXAnalysis\n";
		returnValue += "-------------------------\n";
		if (magnetfieldX->isBxDisplayEnabled()) {
			returnValue += "B_x:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldX->getBxDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldX->getBxDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldX->getBxDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldX->getBxDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldX->getBxDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldX->getBxDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldX->getBxDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldX->getBxDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldX->getBxDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldX->getBxDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}
		if (magnetfieldX->isByDisplayEnabled()) {
			returnValue += "B_y:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldX->getByDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldX->getByDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldX->getByDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldX->getByDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldX->getByDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldX->getByDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldX->getByDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldX->getByDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldX->getByDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldX->getByDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}
		if (magnetfieldX->isBzDisplayEnabled()) {
			returnValue += "B_z:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldX->getBzDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldX->getBzDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldX->getBzDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldX->getBzDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldX->getBzDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldX->getBzDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldX->getBzDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldX->getBzDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldX->getBzDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldX->getBzDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}

	}

	if (isMagnetfieldYAnalysisEnabled()) {

		returnValue += "\nMagnetfieldYAnalysis\n";
		returnValue += "-------------------------\n";
		if (magnetfieldY->isBxDisplayEnabled()) {
			returnValue += "B_x:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldY->getBxDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldY->getBxDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldY->getBxDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldY->getBxDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldY->getBxDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldY->getBxDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldY->getBxDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldY->getBxDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldY->getBxDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldY->getBxDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}
		if (magnetfieldY->isByDisplayEnabled()) {
			returnValue += "B_y:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldY->getByDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldY->getByDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldY->getByDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldY->getByDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldY->getByDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldY->getByDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldY->getByDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldY->getByDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldY->getByDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldY->getByDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}
		if (magnetfieldY->isBzDisplayEnabled()) {
			returnValue += "B_z:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldY->getBzDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldY->getBzDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldY->getBzDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldY->getBzDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldY->getBzDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldY->getBzDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldY->getBzDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldY->getBzDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldY->getBzDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldY->getBzDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}

	}

	if (isMagnetfieldZAnalysisEnabled()) {

		returnValue += "\nMagnetfieldZAnalysis\n";
		returnValue += "-------------------------\n";
		if (magnetfieldZ->isBxDisplayEnabled()) {
			returnValue += "B_x:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldZ->getBxDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldZ->getBxDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldZ->getBxDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldZ->getBxDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldZ->getBxDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldZ->getBxDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldZ->getBxDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldZ->getBxDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldZ->getBxDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldZ->getBxDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}
		if (magnetfieldZ->isByDisplayEnabled()) {
			returnValue += "B_y:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldZ->getByDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldZ->getByDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldZ->getByDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldZ->getByDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldZ->getByDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldZ->getByDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldZ->getByDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldZ->getByDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldZ->getByDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldZ->getByDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}
		if (magnetfieldZ->isBzDisplayEnabled()) {
			returnValue += "B_z:\n";
			returnValue += " The xaxi range is from ";
			dtos(magnetfieldZ->getBzDisplayAxiRange().min, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " to ";
			dtos(magnetfieldZ->getBzDisplayAxiRange().max, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n There are ";
			uitos(magnetfieldZ->getBzDisplayAxiRange().numberOfUnderflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldZ->getBzDisplayAxiRange().numberOfUnderflows) / ((double)magnetfieldZ->getBzDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) underflows and ";
			uitos(magnetfieldZ->getBzDisplayAxiRange().numberOfOverflows, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)magnetfieldZ->getBzDisplayAxiRange().numberOfOverflows) / ((double)magnetfieldZ->getBzDisplayEntries()) + 0.5)));
			uitos(percentBuffer, intBuffer, 10, intConversionDigits);
			returnValue += intBuffer;
			returnValue += "%) overflows.\n";
			returnValue += " The constant value for the first axi is ";
			dtos(magnetfieldZ->getBzDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += " and the one for the second is ";
			dtos(magnetfieldZ->getBzDisplayAxiRange().constValDim1, doubleBuffer, doubleConversionDigits);
			returnValue += doubleBuffer;
			returnValue += ".\n";
		}

	}

	if ((isMagnetfieldXAnalysisEnabled()) || (isMagnetfieldYAnalysisEnabled()) || (isMagnetfieldZAnalysisEnabled())) {

		returnValue += "\nAll underflows are set into the first entry,\n";
		returnValue += "while all overflows are set into the last one.\n";

	}

	return returnValue;

}
