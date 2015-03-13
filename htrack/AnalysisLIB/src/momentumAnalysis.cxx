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
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisError.h"
#include "../include/momentumAnalysis.h"
#include "TStyle.h"
#include "TFile.h"


/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void momentumAnalysis::initDisplayStyle() {

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

void momentumAnalysis::initWindow() {

	if ((window == NULL) && (enableDisplay)) {

		initDisplayStyle();

		window = new TCanvas("MAWindow", "Momentum Analysis", 600, 400);

		numberOfWindowPadColumns = 0;
		numberOfWindowPadRows    = 0;

	}

}

/****************************************************************
 * This method evaluate the window division signature.			*
 ****************************************************************/

void momentumAnalysis::evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature) {

	if ((sizeOfLocalSignature > sizeOfGlobalSignature) || (globalSignature == NULL) || (localSignature == NULL))
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < sizeOfLocalSignature; i++)
		globalSignature[i] |= localSignature[i];

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void momentumAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows) {

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

	if (isMomentumEFGCTotalPzAnalysisEnabled()) {

		efgcTotalPz->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = efgcTotalPz->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		efgcTotalPz->getSignatureOfEnabledDisplays(localSignature);
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

	if (isMomentumEFGCTotalPtAnalysisEnabled()) {

		efgcTotalPt->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = efgcTotalPt->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		efgcTotalPt->getSignatureOfEnabledDisplays(localSignature);
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

	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->setWindowPadRow(globalSignature, sizeOfGlobalSignature);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->setWindowPadRow(globalSignature, sizeOfGlobalSignature);

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

void momentumAnalysis::drawWindowPads(unsigned short columnId) {

	unsigned short padId;

	if (window == NULL)
		throw windowNotFoundError();

	padId = columnId + 1;

	if (padId > numberOfWindowPadColumns)
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < numberOfWindowPadRows; i++) {

		window->cd(padId + i * numberOfWindowPadColumns);

		if (isMomentumEFGCTotalPzAnalysisEnabled())
			if (columnId == efgcTotalPz->getWindowPadColumn())
				efgcTotalPz->drawDisplay(i);

		if (isMomentumEFGCTotalPtAnalysisEnabled())
			if (columnId == efgcTotalPt->getWindowPadColumn())
				efgcTotalPt->drawDisplay(i);

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

momentumAnalysis::momentumAnalysis() 
 : window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   efgcEventPz(NULL),
   efgcEventPt(NULL),
   efgcTotalPz(NULL),
   efgcTotalPt(NULL),
   efgcEventAnalysis(false),
   efgcTotalAnalysis(false),
   outputFileName()
{
  /*
	window                   = NULL;
	numberOfWindowPadColumns = 0;
	numberOfWindowPadRows    = 0;
	enableDisplay            = false;
	efgcEventPz               = NULL;
	efgcEventPt               = NULL;
	efgcTotalPz               = NULL;
	efgcTotalPt               = NULL;
	efgcEventAnalysis         = false;
	efgcTotalAnalysis         = false;
	outputFileName.clear();
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

momentumAnalysis::~momentumAnalysis() {

	if (window != NULL) {
		delete window;
		window = NULL;
	}
	if (isMomentumEFGCEventPzAnalysisEnabled()) {
		delete efgcEventPz;
		efgcEventPz = NULL;
	}
	if (isMomentumEFGCEventPtAnalysisEnabled()) {
		delete efgcEventPt;
		efgcEventPt = NULL;
	}
	if (isMomentumEFGCTotalPzAnalysisEnabled()) {
		delete efgcTotalPz;
		efgcTotalPz = NULL;
	}
	if (isMomentumEFGCTotalPtAnalysisEnabled()) {
		delete efgcTotalPt;
		efgcTotalPt = NULL;
	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void momentumAnalysis::initMomentumAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	TFile*      file              = NULL;
	TDirectory* previousDirectory = NULL;

	outputFileName.clear();

	if (isMomentumEFGCEventPzAnalysisEnabled() || isMomentumEFGCEventPtAnalysisEnabled() || isMomentumEFGCTotalPzAnalysisEnabled() || isMomentumEFGCTotalPtAnalysisEnabled()) {

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

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->initMomentumAnalysisToRoot(enable, name);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->initMomentumAnalysisToRoot(enable, name);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->initMomentumAnalysisToRoot(enable, name);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->initMomentumAnalysisToRoot(enable, name);

}

/****************************************************************
 * This method returns true if the momentum's display is written*
 * to file.														*
 ****************************************************************/

bool momentumAnalysis::isMomentumToRootEnabled() {

	bool returnValue = false;

	if (isMomentumEFGCEventPzAnalysisEnabled())
		returnValue |= efgcEventPz->isWritingEnabled();
	if (isMomentumEFGCEventPtAnalysisEnabled())
		returnValue |= efgcEventPt->isWritingEnabled();
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		returnValue |= efgcTotalPz->isWritingEnabled();
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		returnValue |= efgcTotalPt->isWritingEnabled();

	return returnValue;

}

/****************************************************************
 * This method initializes the momentum's display.				*
 ****************************************************************/

void momentumAnalysis::initMomentumAnalysisDisplay(bool enable) {

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

bool momentumAnalysis::isMomentumDisplayEnabled() {

	return enableDisplay;

}

/****************************************************************
 * method initializes the momentumEFGCEventPzE-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPzEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPz == NULL)
			efgcEventPz = new momentumEFGCEventPzAnalysis();

		efgcEventPz->initEMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPzAnalysisEnabled()) {

			efgcEventPz->deleteEMDisplay();

			if (!efgcEventPz->isDisplayEnabled()) {
				delete efgcEventPz;
				efgcEventPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzF-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPzFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPz == NULL)
			efgcEventPz = new momentumEFGCEventPzAnalysis();

		efgcEventPz->initFMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPzAnalysisEnabled()) {

			efgcEventPz->deleteFMDisplay();

			if (!efgcEventPz->isDisplayEnabled()) {
				delete efgcEventPz;
				efgcEventPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzG-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPzGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPz == NULL)
			efgcEventPz = new momentumEFGCEventPzAnalysis();

		efgcEventPz->initGMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPzAnalysisEnabled()) {

			efgcEventPz->deleteGMDisplay();

			if (!efgcEventPz->isDisplayEnabled()) {
				delete efgcEventPz;
				efgcEventPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPzCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPz == NULL)
			efgcEventPz = new momentumEFGCEventPzAnalysis();

		efgcEventPz->initCMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPzAnalysisEnabled()) {

			efgcEventPz->deleteCMDisplay();

			if (!efgcEventPz->isDisplayEnabled()) {
				delete efgcEventPz;
				efgcEventPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPzEFGC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPzEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	initMomentumEFGCEventPzEAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCEventPzFAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCEventPzGAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCEventPzCAnalysis(enable, nBins, xMin, xMax);

}

/****************************************************************
 * method initializes the momentumEFGCEventPtE-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPtEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPt == NULL)
			efgcEventPt = new momentumEFGCEventPtAnalysis();

		efgcEventPt->initEMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPtAnalysisEnabled()) {

			efgcEventPt->deleteEMDisplay();

			if (!efgcEventPt->isDisplayEnabled()) {
				delete efgcEventPt;
				efgcEventPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtF-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPtFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPt == NULL)
			efgcEventPt = new momentumEFGCEventPtAnalysis();

		efgcEventPt->initFMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPtAnalysisEnabled()) {

			efgcEventPt->deleteFMDisplay();

			if (!efgcEventPt->isDisplayEnabled()) {
				delete efgcEventPt;
				efgcEventPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtG-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPtGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPt == NULL)
			efgcEventPt = new momentumEFGCEventPtAnalysis();

		efgcEventPt->initGMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPtAnalysisEnabled()) {

			efgcEventPt->deleteGMDisplay();

			if (!efgcEventPt->isDisplayEnabled()) {
				delete efgcEventPt;
				efgcEventPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPtCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcEventPt == NULL)
			efgcEventPt = new momentumEFGCEventPtAnalysis();

		efgcEventPt->initCMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCEventPtAnalysisEnabled()) {

			efgcEventPt->deleteCMDisplay();

			if (!efgcEventPt->isDisplayEnabled()) {
				delete efgcEventPt;
				efgcEventPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCEventPtEFGC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCEventPtEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	initMomentumEFGCEventPtEAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCEventPtFAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCEventPtGAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCEventPtCAnalysis(enable, nBins, xMin, xMax);

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzE-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPzEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPz == NULL)
			efgcTotalPz = new momentumEFGCTotalPzAnalysis();

		efgcTotalPz->initEMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPzAnalysisEnabled()) {

			efgcTotalPz->deleteEMDisplay();

			if (!efgcTotalPz->isDisplayEnabled()) {
				delete efgcTotalPz;
				efgcTotalPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzF-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPzFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPz == NULL)
			efgcTotalPz = new momentumEFGCTotalPzAnalysis();

		efgcTotalPz->initFMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPzAnalysisEnabled()) {

			efgcTotalPz->deleteFMDisplay();

			if (!efgcTotalPz->isDisplayEnabled()) {
				delete efgcTotalPz;
				efgcTotalPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzG-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPzGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPz == NULL)
			efgcTotalPz = new momentumEFGCTotalPzAnalysis();

		efgcTotalPz->initGMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPzAnalysisEnabled()) {

			efgcTotalPz->deleteGMDisplay();

			if (!efgcTotalPz->isDisplayEnabled()) {
				delete efgcTotalPz;
				efgcTotalPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPzCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPz == NULL)
			efgcTotalPz = new momentumEFGCTotalPzAnalysis();

		efgcTotalPz->initCMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPzAnalysisEnabled()) {

			efgcTotalPz->deleteCMDisplay();

			if (!efgcTotalPz->isDisplayEnabled()) {
				delete efgcTotalPz;
				efgcTotalPz = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPzEFGC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPzEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	initMomentumEFGCTotalPzEAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCTotalPzFAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCTotalPzGAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCTotalPzCAnalysis(enable, nBins, xMin, xMax);

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtE-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPtEAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPt == NULL)
			efgcTotalPt = new momentumEFGCTotalPtAnalysis();

		efgcTotalPt->initEMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPtAnalysisEnabled()) {

			efgcTotalPt->deleteEMDisplay();

			if (!efgcTotalPt->isDisplayEnabled()) {
				delete efgcTotalPt;
				efgcTotalPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtF-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPtFAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPt == NULL)
			efgcTotalPt = new momentumEFGCTotalPtAnalysis();

		efgcTotalPt->initFMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPtAnalysisEnabled()) {

			efgcTotalPt->deleteFMDisplay();

			if (!efgcTotalPt->isDisplayEnabled()) {
				delete efgcTotalPt;
				efgcTotalPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtG-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPtGAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPt == NULL)
			efgcTotalPt = new momentumEFGCTotalPtAnalysis();

		efgcTotalPt->initGMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPtAnalysisEnabled()) {

			efgcTotalPt->deleteGMDisplay();

			if (!efgcTotalPt->isDisplayEnabled()) {
				delete efgcTotalPt;
				efgcTotalPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPtCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	if (enable) {

		if (efgcTotalPt == NULL)
			efgcTotalPt = new momentumEFGCTotalPtAnalysis();

		efgcTotalPt->initCMDisplay(nBins, xMin, xMax);

	}
	else {

		if (isMomentumEFGCTotalPtAnalysisEnabled()) {

			efgcTotalPt->deleteCMDisplay();

			if (!efgcTotalPt->isDisplayEnabled()) {
				delete efgcTotalPt;
				efgcTotalPt = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the momentumEFGCTotalPtEFGC-Analysis.		*
 ****************************************************************/

void momentumAnalysis::initMomentumEFGCTotalPtEFGCAnalysis(bool enable, int nBins, int xMin, int xMax) {

	initMomentumEFGCTotalPtEAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCTotalPtFAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCTotalPtGAnalysis(enable, nBins, xMin, xMax);
	initMomentumEFGCTotalPtCAnalysis(enable, nBins, xMin, xMax);

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void momentumAnalysis::initializeEvaluation() {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->initializeEvaluation();
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->initializeEvaluation();
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->initializeEvaluation();
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->initializeEvaluation();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void momentumAnalysis::finalizeEvaluation() {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->finalizeEvaluation();
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->finalizeEvaluation();
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->finalizeEvaluation();
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->finalizeEvaluation();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::peakToFakeTrack(trackParameter& parameter, histogramSpace& space) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->peakToFakeTrack(parameter, space);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->peakToFakeTrack(parameter, space);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->peakToFakeTrack(parameter, space);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->peakToFakeTrack(parameter, space);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::peakToNoTrack(trackParameter& parameter, histogramSpace& space) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->peakToNoTrack(parameter, space);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->peakToNoTrack(parameter, space);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->peakToNoTrack(parameter, space);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->peakToNoTrack(parameter, space);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::peakToOneTrack(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->peakToOneTrack(parameter, space, isClone, isWrong);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->peakToOneTrack(parameter, space, isClone, isWrong);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->peakToOneTrack(parameter, space, isClone, isWrong);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->peakToOneTrack(parameter, space, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::peakToMoreTracks(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->peakToMoreTracks(parameter, space, isClone, isWrong);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->peakToMoreTracks(parameter, space, isClone, isWrong);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->peakToMoreTracks(parameter, space, isClone, isWrong);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->peakToMoreTracks(parameter, space, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::trackToNoPeak(trackfinderInputTrack& actualTrack) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->trackToNoPeak(actualTrack);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->trackToNoPeak(actualTrack);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->trackToNoPeak(actualTrack);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->trackToNoPeak(actualTrack);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::trackToOnePeak(trackfinderInputTrack& actualTrack) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->trackToOnePeak(actualTrack);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->trackToOnePeak(actualTrack);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->trackToOnePeak(actualTrack);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->trackToOnePeak(actualTrack);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumAnalysis::trackToMorePeaks(trackfinderInputTrack& actualTrack) {

	if (isMomentumEFGCEventPzAnalysisEnabled())
		efgcEventPz->trackToMorePeaks(actualTrack);
	if (isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventPt->trackToMorePeaks(actualTrack);
	if (isMomentumEFGCTotalPzAnalysisEnabled())
		efgcTotalPz->trackToMorePeaks(actualTrack);
	if (isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalPt->trackToMorePeaks(actualTrack);

}

/****************************************************************
 * method returns true, if the momentumEFGCEventPz-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool momentumAnalysis::isMomentumEFGCEventPzAnalysisEnabled() {

	bool returnValue;

	if (efgcEventPz != NULL)
		returnValue = efgcEventPz->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the momentumEFGCTotalPz-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool momentumAnalysis::isMomentumEFGCTotalPzAnalysisEnabled() {

	bool returnValue;

	if (efgcTotalPz != NULL)
		returnValue = efgcTotalPz->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the momentumEFGCEventPt-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool momentumAnalysis::isMomentumEFGCEventPtAnalysisEnabled() {

	bool returnValue;

	if (efgcEventPt != NULL)
		returnValue = efgcEventPt->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the momentumEFGCTotalPt-Analysis is	*
 * enabled.														*
 ****************************************************************/

bool momentumAnalysis::isMomentumEFGCTotalPtAnalysisEnabled() {

	bool returnValue;

	if (efgcTotalPt != NULL)
		returnValue = efgcTotalPt->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method updates the window.								*
 ****************************************************************/

void momentumAnalysis::momentumAnalysisUpdate() {

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
			momentumAnalysisDraw();

	}

}

/****************************************************************
 * This method draws the window.								*
 ****************************************************************/

void momentumAnalysis::momentumAnalysisDraw() {

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

		for (unsigned short i = 0; i < numberOfWindowPadColumns; i++)
			drawWindowPads(i);

		window->cd();

	}

}

/****************************************************************
 * method writes the momentumEvent-Analysis for each event into	*
 * a root file.													*
 ****************************************************************/

void momentumAnalysis::momentumEventAnalysisWrite(int eventNumber) {

	if (isMomentumEFGCEventPzAnalysisEnabled()) {

		if (efgcEventPz->isEMDisplayEnabled())
			efgcEventPz->writeEMDisplay(eventNumber, outputFileName);
		if (efgcEventPz->isFMDisplayEnabled())
			efgcEventPz->writeFMDisplay(eventNumber, outputFileName);
		if (efgcEventPz->isGMDisplayEnabled())
			efgcEventPz->writeGMDisplay(eventNumber, outputFileName);
		if (efgcEventPz->isCMDisplayEnabled())
			efgcEventPz->writeCMDisplay(eventNumber, outputFileName);

	}

	if (isMomentumEFGCEventPtAnalysisEnabled()) {

		if (efgcEventPt->isEMDisplayEnabled())
			efgcEventPt->writeEMDisplay(eventNumber, outputFileName);
		if (efgcEventPt->isFMDisplayEnabled())
			efgcEventPt->writeFMDisplay(eventNumber, outputFileName);
		if (efgcEventPt->isGMDisplayEnabled())
			efgcEventPt->writeGMDisplay(eventNumber, outputFileName);
		if (efgcEventPt->isCMDisplayEnabled())
			efgcEventPt->writeCMDisplay(eventNumber, outputFileName);

	}

}

/****************************************************************
 * method writes the momentumTotal-Analysis over all events into*
 * a root file.													*
 ****************************************************************/

void momentumAnalysis::momentumTotalAnalysisWrite(int eventNumber) {

	if (isMomentumEFGCTotalPzAnalysisEnabled()) {

		if (efgcTotalPz->isEMDisplayEnabled())
			efgcTotalPz->writeEMDisplay(eventNumber, outputFileName);
		if (efgcTotalPz->isFMDisplayEnabled())
			efgcTotalPz->writeFMDisplay(eventNumber, outputFileName);
		if (efgcTotalPz->isGMDisplayEnabled())
			efgcTotalPz->writeGMDisplay(eventNumber, outputFileName);
		if (efgcTotalPz->isCMDisplayEnabled())
			efgcTotalPz->writeCMDisplay(eventNumber, outputFileName);

	}

	if (isMomentumEFGCTotalPtAnalysisEnabled()) {

		if (efgcTotalPt->isEMDisplayEnabled())
			efgcTotalPt->writeEMDisplay(eventNumber, outputFileName);
		if (efgcTotalPt->isFMDisplayEnabled())
			efgcTotalPt->writeFMDisplay(eventNumber, outputFileName);
		if (efgcTotalPt->isGMDisplayEnabled())
			efgcTotalPt->writeGMDisplay(eventNumber, outputFileName);
		if (efgcTotalPt->isCMDisplayEnabled())
			efgcTotalPt->writeCMDisplay(eventNumber, outputFileName);

	}

}

/****************************************************************
 * method initializes the momentumEventAnalysis.				*
 ****************************************************************/

void momentumAnalysis::initMomentumEventAnalysis(bool enable) {

	if (isMomentumEFGCEventPzAnalysisEnabled() || isMomentumEFGCEventPtAnalysisEnabled())
		efgcEventAnalysis = enable;
	else
		efgcEventAnalysis = false;

}

/****************************************************************
 * method initializes the momentumTotalAnalysis.				*
 ****************************************************************/

void momentumAnalysis::initMomentumTotalAnalysis(bool enable) {

	if (isMomentumEFGCTotalPzAnalysisEnabled() || isMomentumEFGCTotalPtAnalysisEnabled())
		efgcTotalAnalysis = enable;
	else
		efgcTotalAnalysis = false;

}

/****************************************************************
 * method  returns true, if the momentumEFGCEventAnalysis		*
 * is enabled.													*
 ****************************************************************/

bool momentumAnalysis::isMomentumEventAnalysisEnabled() {

	bool returnValue;

	if (isMomentumEFGCEventPzAnalysisEnabled() || isMomentumEFGCEventPtAnalysisEnabled())
		returnValue = efgcEventAnalysis;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method  returns true, if the momentumEFGCTotalAnalysis		*
 * is enabled.													*
 ****************************************************************/

bool momentumAnalysis::isMomentumTotalAnalysisEnabled() {

	bool returnValue;

	if (isMomentumEFGCTotalPzAnalysisEnabled() || isMomentumEFGCTotalPtAnalysisEnabled())
		returnValue = efgcTotalAnalysis;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumEFGCEvent-Analysis.									*
 ****************************************************************/

std::string momentumAnalysis::getMomentumEFGCEventAnalysis() {

	std::string returnValue;
	int         percentBuffer;
	char        buffer[intConversionDigits + 1];

	returnValue = "";

	if (isMomentumEFGCEventPzAnalysisEnabled()) {

		returnValue += "\nMomentumEFGCEventPzAnalysis\n";
		returnValue += "-------------------------\n";
		if (efgcEventPz->isEMDisplayEnabled()) {
			returnValue += "Efficiency:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPz->getEMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPz->getEMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPz->getEMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getEMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPz->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPz->getEMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getEMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPz->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcEventPz->isFMDisplayEnabled()) {
			returnValue += "Fake:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPz->getFMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPz->getFMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPz->getFMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getFMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPz->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPz->getFMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getFMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPz->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcEventPz->isGMDisplayEnabled()) {
			returnValue += "Ghost:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPz->getGMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPz->getGMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPz->getGMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getGMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPz->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPz->getGMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getGMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPz->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcEventPz->isCMDisplayEnabled()) {
			returnValue += "Clone:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPz->getCMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPz->getCMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPz->getCMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getCMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPz->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPz->getCMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPz->getCMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPz->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}

	}

	if (isMomentumEFGCEventPtAnalysisEnabled()) {

		returnValue += "\nMomentumEFGCEventPtAnalysis\n";
		returnValue += "-------------------------\n";
		if (efgcEventPt->isEMDisplayEnabled()) {
			returnValue += "Efficiency:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPt->getEMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPt->getEMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPt->getEMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getEMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPt->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPt->getEMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getEMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPt->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcEventPt->isFMDisplayEnabled()) {
			returnValue += "Fake:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPt->getFMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPt->getFMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPt->getFMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getFMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPt->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPt->getFMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getFMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPt->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcEventPt->isGMDisplayEnabled()) {
			returnValue += "Ghost:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPt->getGMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPt->getGMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPt->getGMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getGMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPt->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPt->getGMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getGMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPt->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcEventPt->isCMDisplayEnabled()) {
			returnValue += "Clone:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcEventPt->getCMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcEventPt->getCMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcEventPt->getCMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getCMDisplayAxiRange().numberOfUnderflows) / ((double)efgcEventPt->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcEventPt->getCMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcEventPt->getCMDisplayAxiRange().numberOfOverflows) / ((double)efgcEventPt->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}

	}

	if ((isMomentumEFGCEventPzAnalysisEnabled()) || (isMomentumEFGCEventPtAnalysisEnabled())) {

		returnValue += "\nAll underflows are set into the first entry,\n";
		returnValue += "while all overflows are set into the last one.\n";

	}

	return returnValue;

}

/****************************************************************
 * method returns a string representing the result of the		*
 * momentumEFGCTotal-Analysis.									*
 ****************************************************************/

std::string momentumAnalysis::getMomentumEFGCTotalAnalysis() {

	std::string returnValue;
	int         percentBuffer;
	char        buffer[intConversionDigits + 1];

	returnValue = "";

	if (isMomentumEFGCTotalPzAnalysisEnabled()) {

		returnValue += "\nMomentumEFGCTotalPzAnalysis\n";
		returnValue += "-------------------------\n";
		if (efgcTotalPz->isEMDisplayEnabled()) {
			returnValue += "Efficiency:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPz->getEMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPz->getEMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPz->getEMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getEMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPz->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPz->getEMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getEMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPz->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";

		}
		if (efgcTotalPz->isFMDisplayEnabled()) {
			returnValue += "Fake:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPz->getFMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPz->getFMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPz->getFMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getFMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPz->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPz->getFMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getFMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPz->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcTotalPz->isGMDisplayEnabled()) {
			returnValue += "Ghost:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPz->getGMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPz->getGMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPz->getGMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getGMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPz->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPz->getGMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getGMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPz->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcTotalPz->isCMDisplayEnabled()) {
			returnValue += "Clone:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPz->getCMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPz->getCMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPz->getCMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getCMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPz->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPz->getCMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPz->getCMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPz->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}

	}

	if (isMomentumEFGCTotalPtAnalysisEnabled()) {

		returnValue += "\nMomentumEFGCTotalPtAnalysis\n";
		returnValue += "-------------------------\n";
		if (efgcTotalPt->isEMDisplayEnabled()) {
			returnValue += "Efficiency:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPt->getEMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPt->getEMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPt->getEMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getEMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPt->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPt->getEMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getEMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPt->getEMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcTotalPt->isFMDisplayEnabled()) {
			returnValue += "Fake:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPt->getFMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPt->getFMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPt->getFMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getFMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPt->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPt->getFMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getFMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPt->getFMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcTotalPt->isGMDisplayEnabled()) {
			returnValue += "Ghost:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPt->getGMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPt->getGMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPt->getGMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getGMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPt->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPt->getGMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getGMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPt->getGMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}
		if (efgcTotalPt->isCMDisplayEnabled()) {
			returnValue += "Clone:\n";
			returnValue += " The xaxi range is from ";
			uitos(efgcTotalPt->getCMDisplayAxiRange().min, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " to ";
			uitos(efgcTotalPt->getCMDisplayAxiRange().max, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += ".\n There are ";
			uitos(efgcTotalPt->getCMDisplayAxiRange().numberOfUnderflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getCMDisplayAxiRange().numberOfUnderflows) / ((double)efgcTotalPt->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) underflows and ";
			uitos(efgcTotalPt->getCMDisplayAxiRange().numberOfOverflows, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += " (";
			percentBuffer = (unsigned short)((double)((int)(100 * ((double)efgcTotalPt->getCMDisplayAxiRange().numberOfOverflows) / ((double)efgcTotalPt->getCMDisplayEntries()) + 0.5)));
			uitos(percentBuffer, buffer, 10, intConversionDigits);
			returnValue += buffer;
			returnValue += "%) overflows.\n";
		}

	}

	if ((isMomentumEFGCTotalPzAnalysisEnabled()) || (isMomentumEFGCTotalPtAnalysisEnabled())) {

		returnValue += "\nAll underflows are set into the first entry,\n";
		returnValue += "while all overflows are set into the last one.\n";

	}

	return returnValue;

}
