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
//     - implements methods to display the histogram's occupancy
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-09-11 14:07:03 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../include/analysisError.h"
#include "../include/projectionAnalysis.h"

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

void projectionAnalysis::initDisplayStyle() {

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

void projectionAnalysis::initWindow() {

	if ((window == NULL) && (enableDisplay)) {

		initDisplayStyle();

		window = new TCanvas("PAWindow", "Projection Analysis", 600, 400);

		numberOfWindowPadColumns = 0;
		numberOfWindowPadRows    = 0;

	}

}

/****************************************************************
 * This method evaluate the window division signature.			*
 ****************************************************************/

void projectionAnalysis::evaluateWindowDivisionSignatures(bool* globalSignature, unsigned short sizeOfGlobalSignature, bool* localSignature, unsigned short sizeOfLocalSignature) {

	if ((sizeOfLocalSignature > sizeOfGlobalSignature) || (globalSignature == NULL) || (localSignature == NULL))
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < sizeOfLocalSignature; i++)
		globalSignature[i] |= localSignature[i];

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void projectionAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows) {

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

	if (isProjectionEFGCNTotal12AnalysisEnabled()) {

		efgcnTotal12->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = efgcnTotal12->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		efgcnTotal12->getSignatureOfEnabledDisplays(localSignature);
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

	if (isProjectionEFGCNTotal13AnalysisEnabled()) {

		efgcnTotal13->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = efgcnTotal13->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		efgcnTotal13->getSignatureOfEnabledDisplays(localSignature);
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

	if (isProjectionEFGCNTotal32AnalysisEnabled()) {

		efgcnTotal32->setWindowPadColumn((*numberOfEnabledPadColumns));
		(*numberOfEnabledPadColumns)++;
		sizeOfLocalSignature = efgcnTotal32->getNumberOfDisplays();
		localSignature = (bool*)realloc(localSignature, sizeOfLocalSignature * sizeof(bool));
		if (localSignature == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		efgcnTotal32->getSignatureOfEnabledDisplays(localSignature);
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

	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->setWindowPadRow(globalSignature, sizeOfGlobalSignature);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->setWindowPadRow(globalSignature, sizeOfGlobalSignature);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->setWindowPadRow(globalSignature, sizeOfGlobalSignature);

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

void projectionAnalysis::drawWindowPads(unsigned short columnId) {

	unsigned short padId;

	if (window == NULL)
		throw windowNotFoundError();

	padId = columnId + 1;

	if (padId > numberOfWindowPadColumns)
		throw windowDivisionError(ANALYSISLIB);

	for (unsigned short i = 0; i < numberOfWindowPadRows; i++) {

		window->cd(padId + i * numberOfWindowPadColumns);

		if (isProjectionEFGCNTotal12AnalysisEnabled())
			if (columnId == efgcnTotal12->getWindowPadColumn())
				efgcnTotal12->drawDisplay(i);

		if (isProjectionEFGCNTotal13AnalysisEnabled())
			if (columnId == efgcnTotal13->getWindowPadColumn())
				efgcnTotal13->drawDisplay(i);

		if (isProjectionEFGCNTotal32AnalysisEnabled())
			if (columnId == efgcnTotal32->getWindowPadColumn())
				efgcnTotal32->drawDisplay(i);

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionAnalysis::projectionAnalysis() 
 : window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   efgcnEvent12(NULL),
   efgcnEvent13(NULL),
   efgcnEvent32(NULL),
   efgcnTotal12(NULL),
   efgcnTotal13(NULL),
   efgcnTotal32(NULL),
   efgcnEventAnalysis(false),
   efgcnTotalAnalysis(false),
   outputFileName()
{
  /*
	window                    = NULL;
	numberOfWindowPadColumns  = 0;
	numberOfWindowPadRows     = 0;
	enableDisplay             = false;
	efgcnEvent12              = NULL;
	efgcnEvent13              = NULL;
	efgcnEvent32              = NULL;
	efgcnTotal12              = NULL;
	efgcnTotal13              = NULL;
	efgcnTotal32              = NULL;
	efgcnEventAnalysis        = false;
	efgcnTotalAnalysis        = false;
	outputFileName.clear();
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionAnalysis::~projectionAnalysis() {

	if (window != NULL) {
		delete window;
		window = NULL;
	}
	if (isProjectionEFGCNEvent12AnalysisEnabled()) {
		delete efgcnEvent12;
		efgcnEvent12 = NULL;
	}
	if (isProjectionEFGCNEvent13AnalysisEnabled()) {
		delete efgcnEvent13;
		efgcnEvent13 = NULL;
	}
	if (isProjectionEFGCNEvent32AnalysisEnabled()) {
		delete efgcnEvent32;
		efgcnEvent32 = NULL;
	}
	if (isProjectionEFGCNTotal12AnalysisEnabled()) {
		delete efgcnTotal12;
		efgcnTotal12 = NULL;
	}
	if (isProjectionEFGCNTotal13AnalysisEnabled()) {
		delete efgcnTotal13;
		efgcnTotal13 = NULL;
	}
	if (isProjectionEFGCNTotal32AnalysisEnabled()) {
		delete efgcnTotal32;
		efgcnTotal32 = NULL;
	}

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void projectionAnalysis::initProjectionAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	TFile*      file              = NULL;
	TDirectory* previousDirectory = NULL;

	outputFileName.clear();

	if (isProjectionEFGCNEvent12AnalysisEnabled() || isProjectionEFGCNEvent13AnalysisEnabled() || isProjectionEFGCNEvent32AnalysisEnabled() || isProjectionEFGCNTotal12AnalysisEnabled() || isProjectionEFGCNTotal13AnalysisEnabled() || isProjectionEFGCNTotal32AnalysisEnabled()) {

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

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->initProjectionAnalysisToRoot(enable, name);
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->initProjectionAnalysisToRoot(enable, name);
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->initProjectionAnalysisToRoot(enable, name);
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->initProjectionAnalysisToRoot(enable, name);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->initProjectionAnalysisToRoot(enable, name);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->initProjectionAnalysisToRoot(enable, name);

}

/****************************************************************
 * This method returns true if the projection's display is		*
 * written to file.												*
 ****************************************************************/

bool projectionAnalysis::isProjectionToRootEnabled() {

	bool returnValue = false;

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		returnValue |= efgcnEvent12->isWritingEnabled();
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		returnValue |= efgcnEvent13->isWritingEnabled();
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		returnValue |= efgcnEvent32->isWritingEnabled();
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		returnValue |= efgcnTotal12->isWritingEnabled();
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		returnValue |= efgcnTotal13->isWritingEnabled();
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		returnValue |= efgcnTotal32->isWritingEnabled();

	return returnValue;

}

/****************************************************************
 * This method initializes the projection's display.			*
 ****************************************************************/

void projectionAnalysis::initProjectionAnalysisDisplay(bool enable) {

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
 * This method returns true if the projection's display is enabled.*
 ****************************************************************/

bool projectionAnalysis::isProjectionDisplayEnabled() {

	return enableDisplay;

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12E-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent12EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent12 == NULL)
			efgcnEvent12 = new projectionEFGCNEvent12Analysis();

		efgcnEvent12->initEDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent12AnalysisEnabled()) {

			efgcnEvent12->deleteEDisplay();

			if (!efgcnEvent12->isDisplayEnabled()) {
				delete efgcnEvent12;
				efgcnEvent12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12F-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent12FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent12 == NULL)
			efgcnEvent12 = new projectionEFGCNEvent12Analysis();

		efgcnEvent12->initFDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent12AnalysisEnabled()) {

			efgcnEvent12->deleteFDisplay();

			if (!efgcnEvent12->isDisplayEnabled()) {
				delete efgcnEvent12;
				efgcnEvent12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12G-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent12GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent12 == NULL)
			efgcnEvent12 = new projectionEFGCNEvent12Analysis();

		efgcnEvent12->initGDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent12AnalysisEnabled()) {

			efgcnEvent12->deleteGDisplay();

			if (!efgcnEvent12->isDisplayEnabled()) {
				delete efgcnEvent12;
				efgcnEvent12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12C-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent12CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent12 == NULL)
			efgcnEvent12 = new projectionEFGCNEvent12Analysis();

		efgcnEvent12->initCDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent12AnalysisEnabled()) {

			efgcnEvent12->deleteCDisplay();

			if (!efgcnEvent12->isDisplayEnabled()) {
				delete efgcnEvent12;
				efgcnEvent12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12N-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent12NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent12 == NULL)
			efgcnEvent12 = new projectionEFGCNEvent12Analysis();

		efgcnEvent12->initNDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent12AnalysisEnabled()) {

			efgcnEvent12->deleteNDisplay();

			if (!efgcnEvent12->isDisplayEnabled()) {
				delete efgcnEvent12;
				efgcnEvent12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12EFGCN-Analysis.	*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent12EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	initProjectionEFGCNEvent12EAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent12FAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent12GAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent12CAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent12NAnalysis(enable, nXBins, nYBins);

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13E-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent13EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent13 == NULL)
			efgcnEvent13 = new projectionEFGCNEvent13Analysis();

		efgcnEvent13->initEDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent13AnalysisEnabled()) {

			efgcnEvent13->deleteEDisplay();

			if (!efgcnEvent13->isDisplayEnabled()) {
				delete efgcnEvent13;
				efgcnEvent13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13F-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent13FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent13 == NULL)
			efgcnEvent13 = new projectionEFGCNEvent13Analysis();

		efgcnEvent13->initFDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent13AnalysisEnabled()) {

			efgcnEvent13->deleteFDisplay();

			if (!efgcnEvent13->isDisplayEnabled()) {
				delete efgcnEvent13;
				efgcnEvent13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13G-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent13GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent13 == NULL)
			efgcnEvent13 = new projectionEFGCNEvent13Analysis();

		efgcnEvent13->initGDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent13AnalysisEnabled()) {

			efgcnEvent13->deleteGDisplay();

			if (!efgcnEvent13->isDisplayEnabled()) {
				delete efgcnEvent13;
				efgcnEvent13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13C-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent13CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent13 == NULL)
			efgcnEvent13 = new projectionEFGCNEvent13Analysis();

		efgcnEvent13->initCDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent13AnalysisEnabled()) {

			efgcnEvent13->deleteCDisplay();

			if (!efgcnEvent13->isDisplayEnabled()) {
				delete efgcnEvent13;
				efgcnEvent13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13N-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent13NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent13 == NULL)
			efgcnEvent13 = new projectionEFGCNEvent13Analysis();

		efgcnEvent13->initNDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent13AnalysisEnabled()) {

			efgcnEvent13->deleteNDisplay();

			if (!efgcnEvent13->isDisplayEnabled()) {
				delete efgcnEvent13;
				efgcnEvent13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent13EFGCN-Analysis.	*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent13EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	initProjectionEFGCNEvent13EAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent13FAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent13GAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent13CAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent13NAnalysis(enable, nXBins, nYBins);

}

/****************************************************************
 * method initializes the projectionEFGCNEvent12E-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent32EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent32 == NULL)
			efgcnEvent32 = new projectionEFGCNEvent32Analysis();

		efgcnEvent32->initEDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent32AnalysisEnabled()) {

			efgcnEvent32->deleteEDisplay();

			if (!efgcnEvent32->isDisplayEnabled()) {
				delete efgcnEvent32;
				efgcnEvent32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32F-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent32FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent32 == NULL)
			efgcnEvent32 = new projectionEFGCNEvent32Analysis();

		efgcnEvent32->initFDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent32AnalysisEnabled()) {

			efgcnEvent32->deleteFDisplay();

			if (!efgcnEvent32->isDisplayEnabled()) {
				delete efgcnEvent32;
				efgcnEvent32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32G-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent32GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent32 == NULL)
			efgcnEvent32 = new projectionEFGCNEvent32Analysis();

		efgcnEvent32->initGDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent32AnalysisEnabled()) {

			efgcnEvent32->deleteGDisplay();

			if (!efgcnEvent32->isDisplayEnabled()) {
				delete efgcnEvent32;
				efgcnEvent32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32C-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent32CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent32 == NULL)
			efgcnEvent32 = new projectionEFGCNEvent32Analysis();

		efgcnEvent32->initCDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent32AnalysisEnabled()) {

			efgcnEvent32->deleteCDisplay();

			if (!efgcnEvent32->isDisplayEnabled()) {
				delete efgcnEvent32;
				efgcnEvent32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32N-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent32NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnEvent32 == NULL)
			efgcnEvent32 = new projectionEFGCNEvent32Analysis();

		efgcnEvent32->initNDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNEvent32AnalysisEnabled()) {

			efgcnEvent32->deleteNDisplay();

			if (!efgcnEvent32->isDisplayEnabled()) {
				delete efgcnEvent32;
				efgcnEvent32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNEvent32EFGCN-Analysis.	*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNEvent32EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	initProjectionEFGCNEvent32EAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent32FAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent32GAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent32CAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNEvent32NAnalysis(enable, nXBins, nYBins);

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12E-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal12EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal12 == NULL)
			efgcnTotal12 = new projectionEFGCNTotal12Analysis();

		efgcnTotal12->initEDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal12AnalysisEnabled()) {

			efgcnTotal12->deleteEDisplay();

			if (!efgcnTotal12->isDisplayEnabled()) {
				delete efgcnTotal12;
				efgcnTotal12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12F-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal12FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal12 == NULL)
			efgcnTotal12 = new projectionEFGCNTotal12Analysis();

		efgcnTotal12->initFDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal12AnalysisEnabled()) {

			efgcnTotal12->deleteFDisplay();

			if (!efgcnTotal12->isDisplayEnabled()) {
				delete efgcnTotal12;
				efgcnTotal12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12G-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal12GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal12 == NULL)
			efgcnTotal12 = new projectionEFGCNTotal12Analysis();

		efgcnTotal12->initGDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal12AnalysisEnabled()) {

			efgcnTotal12->deleteGDisplay();

			if (!efgcnTotal12->isDisplayEnabled()) {
				delete efgcnTotal12;
				efgcnTotal12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12C-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal12CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal12 == NULL)
			efgcnTotal12 = new projectionEFGCNTotal12Analysis();

		efgcnTotal12->initCDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal12AnalysisEnabled()) {

			efgcnTotal12->deleteCDisplay();

			if (!efgcnTotal12->isDisplayEnabled()) {
				delete efgcnTotal12;
				efgcnTotal12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12N-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal12NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal12 == NULL)
			efgcnTotal12 = new projectionEFGCNTotal12Analysis();

		efgcnTotal12->initNDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal12AnalysisEnabled()) {

			efgcnTotal12->deleteNDisplay();

			if (!efgcnTotal12->isDisplayEnabled()) {
				delete efgcnTotal12;
				efgcnTotal12 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12EFGCN-Analysis.	*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal12EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	initProjectionEFGCNTotal12EAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal12FAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal12GAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal12CAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal12NAnalysis(enable, nXBins, nYBins);

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13E-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal13EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal13 == NULL)
			efgcnTotal13 = new projectionEFGCNTotal13Analysis();

		efgcnTotal13->initEDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal13AnalysisEnabled()) {

			efgcnTotal13->deleteEDisplay();

			if (!efgcnTotal13->isDisplayEnabled()) {
				delete efgcnTotal13;
				efgcnTotal13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13F-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal13FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal13 == NULL)
			efgcnTotal13 = new projectionEFGCNTotal13Analysis();

		efgcnTotal13->initFDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal13AnalysisEnabled()) {

			efgcnTotal13->deleteFDisplay();

			if (!efgcnTotal13->isDisplayEnabled()) {
				delete efgcnTotal13;
				efgcnTotal13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13G-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal13GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal13 == NULL)
			efgcnTotal13 = new projectionEFGCNTotal13Analysis();

		efgcnTotal13->initGDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal13AnalysisEnabled()) {

			efgcnTotal13->deleteGDisplay();

			if (!efgcnTotal13->isDisplayEnabled()) {
				delete efgcnTotal13;
				efgcnTotal13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13C-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal13CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal13 == NULL)
			efgcnTotal13 = new projectionEFGCNTotal13Analysis();

		efgcnTotal13->initCDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal13AnalysisEnabled()) {

			efgcnTotal13->deleteCDisplay();

			if (!efgcnTotal13->isDisplayEnabled()) {
				delete efgcnTotal13;
				efgcnTotal13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13N-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal13NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal13 == NULL)
			efgcnTotal13 = new projectionEFGCNTotal13Analysis();

		efgcnTotal13->initNDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal13AnalysisEnabled()) {

			efgcnTotal13->deleteNDisplay();

			if (!efgcnTotal13->isDisplayEnabled()) {
				delete efgcnTotal13;
				efgcnTotal13 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal13EFGCN-Analysis.	*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal13EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	initProjectionEFGCNTotal13EAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal13FAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal13GAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal13CAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal13NAnalysis(enable, nXBins, nYBins);

}

/****************************************************************
 * method initializes the projectionEFGCNTotal12E-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal32EAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal32 == NULL)
			efgcnTotal32 = new projectionEFGCNTotal32Analysis();

		efgcnTotal32->initEDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal32AnalysisEnabled()) {

			efgcnTotal32->deleteEDisplay();

			if (!efgcnTotal32->isDisplayEnabled()) {
				delete efgcnTotal32;
				efgcnTotal32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32F-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal32FAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal32 == NULL)
			efgcnTotal32 = new projectionEFGCNTotal32Analysis();

		efgcnTotal32->initFDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal32AnalysisEnabled()) {

			efgcnTotal32->deleteFDisplay();

			if (!efgcnTotal32->isDisplayEnabled()) {
				delete efgcnTotal32;
				efgcnTotal32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32G-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal32GAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal32 == NULL)
			efgcnTotal32 = new projectionEFGCNTotal32Analysis();

		efgcnTotal32->initGDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal32AnalysisEnabled()) {

			efgcnTotal32->deleteGDisplay();

			if (!efgcnTotal32->isDisplayEnabled()) {
				delete efgcnTotal32;
				efgcnTotal32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32C-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal32CAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal32 == NULL)
			efgcnTotal32 = new projectionEFGCNTotal32Analysis();

		efgcnTotal32->initCDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal32AnalysisEnabled()) {

			efgcnTotal32->deleteCDisplay();

			if (!efgcnTotal32->isDisplayEnabled()) {
				delete efgcnTotal32;
				efgcnTotal32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32N-Analysis.		*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal32NAnalysis(bool enable, int nXBins, int nYBins) {

	if (enable) {

		if (efgcnTotal32 == NULL)
			efgcnTotal32 = new projectionEFGCNTotal32Analysis();

		efgcnTotal32->initNDisplay(nXBins, nYBins);

	}
	else {

		if (isProjectionEFGCNTotal32AnalysisEnabled()) {

			efgcnTotal32->deleteNDisplay();

			if (!efgcnTotal32->isDisplayEnabled()) {
				delete efgcnTotal32;
				efgcnTotal32 = NULL;
			}

		}

	}

}

/****************************************************************
 * method initializes the projectionEFGCNTotal32EFGCN-Analysis.	*
 ****************************************************************/

void projectionAnalysis::initProjectionEFGCNTotal32EFGCNAnalysis(bool enable, int nXBins, int nYBins) {

	initProjectionEFGCNTotal32EAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal32FAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal32GAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal32CAnalysis(enable, nXBins, nYBins);
	initProjectionEFGCNTotal32NAnalysis(enable, nXBins, nYBins);

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void projectionAnalysis::initializeEvaluation() {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->initializeEvaluation();
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->initializeEvaluation();
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->initializeEvaluation();
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->initializeEvaluation();
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->initializeEvaluation();
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->initializeEvaluation();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void projectionAnalysis::finalizeEvaluation() {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->finalizeEvaluation();
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->finalizeEvaluation();
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->finalizeEvaluation();
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->finalizeEvaluation();
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->finalizeEvaluation();
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->finalizeEvaluation();

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionAnalysis::peakToFakeTrack(trackCoordinates& coordinates) {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->peakToFakeTrack(coordinates);
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->peakToFakeTrack(coordinates);
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->peakToFakeTrack(coordinates);
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->peakToFakeTrack(coordinates);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->peakToFakeTrack(coordinates);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->peakToFakeTrack(coordinates);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionAnalysis::peakToNoTrack(trackCoordinates& coordinates) {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->peakToNoTrack(coordinates);
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->peakToNoTrack(coordinates);
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->peakToNoTrack(coordinates);
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->peakToNoTrack(coordinates);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->peakToNoTrack(coordinates);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->peakToNoTrack(coordinates);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionAnalysis::peakToOneTrack(trackCoordinates& coordinates, bool isClone, bool isWrong) {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->peakToOneTrack(coordinates, isClone, isWrong);
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->peakToOneTrack(coordinates, isClone, isWrong);
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->peakToOneTrack(coordinates, isClone, isWrong);
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->peakToOneTrack(coordinates, isClone, isWrong);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->peakToOneTrack(coordinates, isClone, isWrong);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->peakToOneTrack(coordinates, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionAnalysis::peakToMoreTracks(trackCoordinates& coordinates, bool isClone, bool isWrong) {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->peakToMoreTracks(coordinates, isClone, isWrong);
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->peakToMoreTracks(coordinates, isClone, isWrong);
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->peakToMoreTracks(coordinates, isClone, isWrong);
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->peakToMoreTracks(coordinates, isClone, isWrong);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->peakToMoreTracks(coordinates, isClone, isWrong);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->peakToMoreTracks(coordinates, isClone, isWrong);

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionAnalysis::trackToNoPeak(trackCoordinates& coordinates) {

	if (isProjectionEFGCNEvent12AnalysisEnabled())
		efgcnEvent12->trackToNoPeak(coordinates);
	if (isProjectionEFGCNEvent13AnalysisEnabled())
		efgcnEvent13->trackToNoPeak(coordinates);
	if (isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEvent32->trackToNoPeak(coordinates);
	if (isProjectionEFGCNTotal12AnalysisEnabled())
		efgcnTotal12->trackToNoPeak(coordinates);
	if (isProjectionEFGCNTotal13AnalysisEnabled())
		efgcnTotal13->trackToNoPeak(coordinates);
	if (isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotal32->trackToNoPeak(coordinates);

}

/****************************************************************
 * method returns true, if the projectionEFGCNEvent12-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEFGCNEvent12AnalysisEnabled() {

	bool returnValue;

	if (efgcnEvent12 != NULL)
		returnValue = efgcnEvent12->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the projectionEFGCNEvent13-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEFGCNEvent13AnalysisEnabled() {

	bool returnValue;

	if (efgcnEvent13 != NULL)
		returnValue = efgcnEvent13->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the projectionEFGCNEvent32-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEFGCNEvent32AnalysisEnabled() {

	bool returnValue;

	if (efgcnEvent32 != NULL)
		returnValue = efgcnEvent32->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the projectionEFGCNTotal12-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEFGCNTotal12AnalysisEnabled() {

	bool returnValue;

	if (efgcnTotal12 != NULL)
		returnValue = efgcnTotal12->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns true, if the projectionEFGCNTotal13-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEFGCNTotal13AnalysisEnabled() {

	bool returnValue;

	if (efgcnTotal13 != NULL)
		returnValue = efgcnTotal13->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}
/****************************************************************
 * method returns true, if the projectionEFGCNTotal32-Analysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEFGCNTotal32AnalysisEnabled() {

	bool returnValue;

	if (efgcnTotal32 != NULL)
		returnValue = efgcnTotal32->isDisplayEnabled();
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method updates the window.								*
 ****************************************************************/

void projectionAnalysis::projectionAnalysisUpdate() {

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
			projectionAnalysisDraw();

	}

}

/****************************************************************
 * This method draws the window.								*
 ****************************************************************/

void projectionAnalysis::projectionAnalysisDraw() {

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
 * method writes the projectionEvent-Analysis for each event	*
 * into a root file.											*
 ****************************************************************/

void projectionAnalysis::projectionEventAnalysisWrite(int eventNumber) {

	if (isProjectionEFGCNEvent12AnalysisEnabled()) {

		if (efgcnEvent12->isEDisplayEnabled())
			efgcnEvent12->writeEDisplay(eventNumber, outputFileName);
		if (efgcnEvent12->isFDisplayEnabled())
			efgcnEvent12->writeFDisplay(eventNumber, outputFileName);
		if (efgcnEvent12->isGDisplayEnabled())
			efgcnEvent12->writeGDisplay(eventNumber, outputFileName);
		if (efgcnEvent12->isCDisplayEnabled())
			efgcnEvent12->writeCDisplay(eventNumber, outputFileName);
		if (efgcnEvent12->isNDisplayEnabled())
			efgcnEvent12->writeNDisplay(eventNumber, outputFileName);

	}

	if (isProjectionEFGCNEvent13AnalysisEnabled()) {

		if (efgcnEvent13->isEDisplayEnabled())
			efgcnEvent13->writeEDisplay(eventNumber, outputFileName);
		if (efgcnEvent13->isFDisplayEnabled())
			efgcnEvent13->writeFDisplay(eventNumber, outputFileName);
		if (efgcnEvent13->isGDisplayEnabled())
			efgcnEvent13->writeGDisplay(eventNumber, outputFileName);
		if (efgcnEvent13->isCDisplayEnabled())
			efgcnEvent13->writeCDisplay(eventNumber, outputFileName);
		if (efgcnEvent13->isNDisplayEnabled())
			efgcnEvent13->writeNDisplay(eventNumber, outputFileName);

	}

	if (isProjectionEFGCNEvent32AnalysisEnabled()) {

		if (efgcnEvent32->isEDisplayEnabled())
			efgcnEvent32->writeEDisplay(eventNumber, outputFileName);
		if (efgcnEvent32->isFDisplayEnabled())
			efgcnEvent32->writeFDisplay(eventNumber, outputFileName);
		if (efgcnEvent32->isGDisplayEnabled())
			efgcnEvent32->writeGDisplay(eventNumber, outputFileName);
		if (efgcnEvent32->isCDisplayEnabled())
			efgcnEvent32->writeCDisplay(eventNumber, outputFileName);
		if (efgcnEvent32->isNDisplayEnabled())
			efgcnEvent32->writeNDisplay(eventNumber, outputFileName);

	}

}

/****************************************************************
 * method writes the projectionTotal-Analysis over all events	*
 * into a root file.											*
 ****************************************************************/

void projectionAnalysis::projectionTotalAnalysisWrite(int eventNumber) {

	if (isProjectionEFGCNTotal12AnalysisEnabled()) {

		if (efgcnTotal12->isEDisplayEnabled())
			efgcnTotal12->writeEDisplay(eventNumber, outputFileName);
		if (efgcnTotal12->isFDisplayEnabled())
			efgcnTotal12->writeFDisplay(eventNumber, outputFileName);
		if (efgcnTotal12->isGDisplayEnabled())
			efgcnTotal12->writeGDisplay(eventNumber, outputFileName);
		if (efgcnTotal12->isCDisplayEnabled())
			efgcnTotal12->writeCDisplay(eventNumber, outputFileName);
		if (efgcnTotal12->isNDisplayEnabled())
			efgcnTotal12->writeNDisplay(eventNumber, outputFileName);

	}

	if (isProjectionEFGCNTotal13AnalysisEnabled()) {

		if (efgcnTotal13->isEDisplayEnabled())
			efgcnTotal13->writeEDisplay(eventNumber, outputFileName);
		if (efgcnTotal13->isFDisplayEnabled())
			efgcnTotal13->writeFDisplay(eventNumber, outputFileName);
		if (efgcnTotal13->isGDisplayEnabled())
			efgcnTotal13->writeGDisplay(eventNumber, outputFileName);
		if (efgcnTotal13->isCDisplayEnabled())
			efgcnTotal13->writeCDisplay(eventNumber, outputFileName);
		if (efgcnTotal13->isNDisplayEnabled())
			efgcnTotal13->writeNDisplay(eventNumber, outputFileName);

	}

	if (isProjectionEFGCNTotal32AnalysisEnabled()) {

		if (efgcnTotal32->isEDisplayEnabled())
			efgcnTotal32->writeEDisplay(eventNumber, outputFileName);
		if (efgcnTotal32->isFDisplayEnabled())
			efgcnTotal32->writeFDisplay(eventNumber, outputFileName);
		if (efgcnTotal32->isGDisplayEnabled())
			efgcnTotal32->writeGDisplay(eventNumber, outputFileName);
		if (efgcnTotal32->isCDisplayEnabled())
			efgcnTotal32->writeCDisplay(eventNumber, outputFileName);
		if (efgcnTotal32->isNDisplayEnabled())
			efgcnTotal32->writeNDisplay(eventNumber, outputFileName);

	}

}

/****************************************************************
 * method initializes the projectionEventAnalysis.				*
 ****************************************************************/

void projectionAnalysis::initProjectionEventAnalysis(bool enable) {

	if (isProjectionEFGCNEvent12AnalysisEnabled() || isProjectionEFGCNEvent13AnalysisEnabled() || isProjectionEFGCNEvent32AnalysisEnabled())
		efgcnEventAnalysis = enable;
	else
		efgcnEventAnalysis = false;

}

/****************************************************************
 * method initializes the projectionTotalAnalysis.				*
 ****************************************************************/

void projectionAnalysis::initProjectionTotalAnalysis(bool enable) {

	if (isProjectionEFGCNTotal12AnalysisEnabled() || isProjectionEFGCNTotal13AnalysisEnabled() || isProjectionEFGCNTotal32AnalysisEnabled())
		efgcnTotalAnalysis = enable;
	else
		efgcnTotalAnalysis = false;

}

/****************************************************************
 * method  returns true, if the projectionEFGCNEventAnalysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionEventAnalysisEnabled() {

	bool returnValue;

	if (isProjectionEFGCNEvent12AnalysisEnabled() || isProjectionEFGCNEvent13AnalysisEnabled() || isProjectionEFGCNEvent32AnalysisEnabled())
		returnValue = efgcnEventAnalysis;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method  returns true, if the projectionEFGCNTotalAnalysis	*
 * is enabled.													*
 ****************************************************************/

bool projectionAnalysis::isProjectionTotalAnalysisEnabled() {

	bool returnValue;

	if (isProjectionEFGCNTotal12AnalysisEnabled() || isProjectionEFGCNTotal13AnalysisEnabled() || isProjectionEFGCNTotal32AnalysisEnabled())
		returnValue = efgcnTotalAnalysis;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method  controls the projectionEFGCN12EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent12ERange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent12 != NULL) {

		if (efgcnEvent12->isEDisplayEnabled()) {

			displayInfo = efgcnEvent12->getEDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnEvent12->initEDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent12FRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent12 != NULL) {

		if (efgcnEvent12->isFDisplayEnabled()) {

			displayInfo = efgcnEvent12->getFDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnEvent12->initFDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent12GRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent12 != NULL) {

		if (efgcnEvent12->isGDisplayEnabled()) {

			displayInfo = efgcnEvent12->getGDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnEvent12->initGDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent12CRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent12 != NULL) {

		if (efgcnEvent12->isCDisplayEnabled()) {

			displayInfo = efgcnEvent12->getCDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnEvent12->initCDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent12NRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent12 != NULL) {

		if (efgcnEvent12->isNDisplayEnabled()) {

			displayInfo = efgcnEvent12->getNDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnEvent12->initNDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent12EFGCNRange(unsigned short range1, unsigned short range2) {

	if (efgcnEvent12 != NULL) {

		controlProjectionEFGCNEvent12ERange(range1, range2);
		controlProjectionEFGCNEvent12FRange(range1, range2);
		controlProjectionEFGCNEvent12GRange(range1, range2);
		controlProjectionEFGCNEvent12CRange(range1, range2);
		controlProjectionEFGCNEvent12NRange(range1, range2);

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent13ERange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent13 != NULL) {

		if (efgcnEvent13->isEDisplayEnabled()) {

			displayInfo = efgcnEvent13->getEDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnEvent13->initEDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent13FRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent13 != NULL) {

		if (efgcnEvent13->isFDisplayEnabled()) {

			displayInfo = efgcnEvent13->getFDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnEvent13->initFDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent13GRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent13 != NULL) {

		if (efgcnEvent13->isGDisplayEnabled()) {

			displayInfo = efgcnEvent13->getGDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnEvent13->initGDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent13CRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent13 != NULL) {

		if (efgcnEvent13->isCDisplayEnabled()) {

			displayInfo = efgcnEvent13->getCDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnEvent13->initCDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent13NRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent13 != NULL) {

		if (efgcnEvent13->isNDisplayEnabled()) {

			displayInfo = efgcnEvent13->getNDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnEvent13->initNDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent13EFGCNRange(unsigned short range1, unsigned short range3) {

	if (efgcnEvent13 != NULL) {

		controlProjectionEFGCNEvent13ERange(range1, range3);
		controlProjectionEFGCNEvent13FRange(range1, range3);
		controlProjectionEFGCNEvent13GRange(range1, range3);
		controlProjectionEFGCNEvent13CRange(range1, range3);
		controlProjectionEFGCNEvent13NRange(range1, range3);

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent32ERange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent32 != NULL) {

		if (efgcnEvent32->isEDisplayEnabled()) {

			displayInfo = efgcnEvent32->getEDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnEvent32->initEDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent32FRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent32 != NULL) {

		if (efgcnEvent32->isFDisplayEnabled()) {

			displayInfo = efgcnEvent32->getFDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnEvent32->initFDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent32GRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent32 != NULL) {

		if (efgcnEvent32->isGDisplayEnabled()) {

			displayInfo = efgcnEvent32->getGDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnEvent32->initGDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent32CRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent32 != NULL) {

		if (efgcnEvent32->isCDisplayEnabled()) {

			displayInfo = efgcnEvent32->getCDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnEvent32->initCDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent32NRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnEvent32 != NULL) {

		if (efgcnEvent32->isNDisplayEnabled()) {

			displayInfo = efgcnEvent32->getNDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnEvent32->initNDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32EventAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNEvent32EFGCNRange(unsigned short range3, unsigned short range2) {

	if (efgcnEvent32 != NULL) {

		controlProjectionEFGCNEvent32ERange(range3, range2);
		controlProjectionEFGCNEvent32FRange(range3, range2);
		controlProjectionEFGCNEvent32GRange(range3, range2);
		controlProjectionEFGCNEvent32CRange(range3, range2);
		controlProjectionEFGCNEvent32NRange(range3, range2);

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal12ERange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal12 != NULL) {

		if (efgcnTotal12->isEDisplayEnabled()) {

			displayInfo = efgcnTotal12->getEDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnTotal12->initEDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal12FRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal12 != NULL) {

		if (efgcnTotal12->isFDisplayEnabled()) {

			displayInfo = efgcnTotal12->getFDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnTotal12->initFDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal12GRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal12 != NULL) {

		if (efgcnTotal12->isGDisplayEnabled()) {

			displayInfo = efgcnTotal12->getGDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnTotal12->initGDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal12CRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal12 != NULL) {

		if (efgcnTotal12->isCDisplayEnabled()) {

			displayInfo = efgcnTotal12->getCDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnTotal12->initCDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal12NRange(unsigned short range1, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal12 != NULL) {

		if (efgcnTotal12->isNDisplayEnabled()) {

			displayInfo = efgcnTotal12->getNDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range2))
				efgcnTotal12->initNDisplay(range1, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN12TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal12EFGCNRange(unsigned short range1, unsigned short range2) {

	if (efgcnTotal12 != NULL) {

		controlProjectionEFGCNTotal12ERange(range1, range2);
		controlProjectionEFGCNTotal12FRange(range1, range2);
		controlProjectionEFGCNTotal12GRange(range1, range2);
		controlProjectionEFGCNTotal12CRange(range1, range2);
		controlProjectionEFGCNTotal12NRange(range1, range2);

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal13ERange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal13 != NULL) {

		if (efgcnTotal13->isEDisplayEnabled()) {

			displayInfo = efgcnTotal13->getEDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnTotal13->initEDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal13FRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal13 != NULL) {

		if (efgcnTotal13->isFDisplayEnabled()) {

			displayInfo = efgcnTotal13->getFDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnTotal13->initFDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal13GRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal13 != NULL) {

		if (efgcnTotal13->isGDisplayEnabled()) {

			displayInfo = efgcnTotal13->getGDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnTotal13->initGDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal13CRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal13 != NULL) {

		if (efgcnTotal13->isCDisplayEnabled()) {

			displayInfo = efgcnTotal13->getCDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnTotal13->initCDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal13NRange(unsigned short range1, unsigned short range3) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal13 != NULL) {

		if (efgcnTotal13->isNDisplayEnabled()) {

			displayInfo = efgcnTotal13->getNDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range1) || (displayInfo.numberOfYBins != range3))
				efgcnTotal13->initNDisplay(range1, range3);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN13TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal13EFGCNRange(unsigned short range1, unsigned short range3) {

	if (efgcnTotal13 != NULL) {

		controlProjectionEFGCNTotal13ERange(range1, range3);
		controlProjectionEFGCNTotal13FRange(range1, range3);
		controlProjectionEFGCNTotal13GRange(range1, range3);
		controlProjectionEFGCNTotal13CRange(range1, range3);
		controlProjectionEFGCNTotal13NRange(range1, range3);

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal32ERange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal32 != NULL) {

		if (efgcnTotal32->isEDisplayEnabled()) {

			displayInfo = efgcnTotal32->getEDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnTotal32->initEDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal32FRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal32 != NULL) {

		if (efgcnTotal32->isFDisplayEnabled()) {

			displayInfo = efgcnTotal32->getFDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnTotal32->initFDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal32GRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal32 != NULL) {

		if (efgcnTotal32->isGDisplayEnabled()) {

			displayInfo = efgcnTotal32->getGDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnTotal32->initGDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal32CRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal32 != NULL) {

		if (efgcnTotal32->isCDisplayEnabled()) {

			displayInfo = efgcnTotal32->getCDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnTotal32->initCDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal32NRange(unsigned short range3, unsigned short range2) {

	projectionEFGCNDisplayInfo displayInfo;

	if (efgcnTotal32 != NULL) {

		if (efgcnTotal32->isNDisplayEnabled()) {

			displayInfo = efgcnTotal32->getNDisplayAxiRange();

			if ((displayInfo.numberOfXBins != range3) || (displayInfo.numberOfYBins != range2))
				efgcnTotal32->initNDisplay(range3, range2);

		}

	}

}

/****************************************************************
 * method  controls the projectionEFGCN32TotalAnalysis for the	*
 * range.														*
 ****************************************************************/

void projectionAnalysis::controlProjectionEFGCNTotal32EFGCNRange(unsigned short range3, unsigned short range2) {

	if (efgcnTotal32 != NULL) {

		controlProjectionEFGCNTotal32ERange(range3, range2);
		controlProjectionEFGCNTotal32FRange(range3, range2);
		controlProjectionEFGCNTotal32GRange(range3, range2);
		controlProjectionEFGCNTotal32CRange(range3, range2);
		controlProjectionEFGCNTotal32NRange(range3, range2);

	}

}
