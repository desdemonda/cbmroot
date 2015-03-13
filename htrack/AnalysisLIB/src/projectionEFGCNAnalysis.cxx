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


#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisWarningMsg.h"
#include "../include/projectionEFGCNAnalysisDef.h"
#include "../include/projectionEFGCNAnalysis.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"


/****************************************************************
 * resets the displays											*
 ****************************************************************/

void projectionEFGCNAnalysis::resetDisplays() {

	if (isEDisplayEnabled())
		eDisplay->Reset();

	if (isFDisplayEnabled())
		fDisplay->Reset();

	if (isGDisplayEnabled())
		gDisplay->Reset();

	if (isCDisplayEnabled())
		cDisplay->Reset();

	if (isCDisplayEnabled())
		nDisplay->Reset();

}

/****************************************************************
 * This method writes a display into the file.					*
 ****************************************************************/

void projectionEFGCNAnalysis::writeDisplay(TH2S* display, projectionEFGCNDisplayInfo* info, int eventNumber, std::string outputFileName) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;
	char        buffer[intConversionDigits+1];
	std::string modifiedDisplayName;
	std::string modifiedDisplayTitle;

	file              = NULL;
	previousDirectory = NULL;
	specialDirectory  = NULL;

	if (display != NULL) {

		if (writeToFile) {

			previousDirectory = gDirectory;
			specialDirectory  = getNameOfSavingDirectory();

			if (!outputFileName.empty()) {

				file = new TFile(outputFileName.c_str(), "UPDATE");
		
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, outputFileName);

			}
			else
				if (gDirectory != NULL)
					gDirectory->cd();

			itos(eventNumber, buffer, 10, intConversionDigits);

			modifiedDisplayName  = display->GetName();
			modifiedDisplayName += "_";
			modifiedDisplayName += PROJECTIONEFGCNEVENTMODIFIER;
			modifiedDisplayName += "_";
			modifiedDisplayName += buffer;

			modifiedDisplayTitle  = display->GetTitle();
			modifiedDisplayTitle += " ";
			modifiedDisplayTitle += PROJECTIONEFGCNEVENTMODIFIER;
			modifiedDisplayTitle += ":";
			modifiedDisplayTitle += buffer;

			display->SetName(modifiedDisplayName.c_str());
			display->SetTitle(modifiedDisplayTitle.c_str());

			if (!gDirectory->cd(PROJECTIONEFGCNSTSANALYSISDIR)) {

				gDirectory->mkdir(PROJECTIONEFGCNSTSANALYSISDIR, PROJECTIONEFGCNSTSANALYSISDIR);
				gDirectory->cd(PROJECTIONEFGCNSTSANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (!info->subDirName.empty())
				gDirectory->cd(info->subDirName.c_str());

			display->Write();

			display->SetName(info->baseName.c_str());
			display->SetTitle(info->baseTitle.c_str());

			gDirectory = previousDirectory;

			if (file != NULL) {
				file->Close();
				if (file->IsOpen())
					throw cannotCloseFileError(ANALYSISLIB, outputFileName);
				delete file;
				file = NULL;
			}

		}
		else {

			displayCannotBeWrittenToFileWarningMsg* displayCannotBeWrittenToFile = new displayCannotBeWrittenToFileWarningMsg();
			displayCannotBeWrittenToFile->warningMsg();
			if(displayCannotBeWrittenToFile != NULL) {
				delete displayCannotBeWrittenToFile;
				displayCannotBeWrittenToFile = NULL;
			}

		}

	}
	else {

		displayNotFoundWarningMsg* displayNotFound = new displayNotFoundWarningMsg();
		displayNotFound->warningMsg();
		if(displayNotFound != NULL) {
			delete displayNotFound;
			displayNotFound = NULL;
		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionEFGCNAnalysis::projectionEFGCNAnalysis() 
  : eDisplay(NULL),
    eGraphicEFGCNDisplayInfo(),
    fDisplay(NULL),
    fGraphicEFGCNDisplayInfo(),
    gDisplay(NULL),
    gGraphicEFGCNDisplayInfo(),
    cDisplay(NULL),
    cGraphicEFGCNDisplayInfo(),
    nDisplay(NULL),
    nGraphicEFGCNDisplayInfo(),
    windowPadColumn(0),
    eWindowPadRow((unsigned short)-1),
    fWindowPadRow((unsigned short)-1),
    gWindowPadRow((unsigned short)-1),
    cWindowPadRow((unsigned short)-1),
    nWindowPadRow((unsigned short)-1),
    writeToFile(false)
{
  /*
	eDisplay                                    = NULL;
	eGraphicEFGCNDisplayInfo.numberOfXBins      = 0;
	eGraphicEFGCNDisplayInfo.numberOfYBins      = 0;
	eGraphicEFGCNDisplayInfo.baseName           = "";
	eGraphicEFGCNDisplayInfo.baseTitle          = "";
	eGraphicEFGCNDisplayInfo.subDirName         = "";
	fDisplay                                    = NULL;
	fGraphicEFGCNDisplayInfo.numberOfXBins      = 0;
	fGraphicEFGCNDisplayInfo.numberOfYBins      = 0;
	fGraphicEFGCNDisplayInfo.baseName           = "";
	fGraphicEFGCNDisplayInfo.baseTitle          = "";
	fGraphicEFGCNDisplayInfo.subDirName         = "";
	gDisplay                                    = NULL;
	gGraphicEFGCNDisplayInfo.numberOfXBins      = 0;
	gGraphicEFGCNDisplayInfo.numberOfYBins      = 0;
	gGraphicEFGCNDisplayInfo.baseName           = "";
	gGraphicEFGCNDisplayInfo.baseTitle          = "";
	gGraphicEFGCNDisplayInfo.subDirName         = "";
	cDisplay                                    = NULL;
	cGraphicEFGCNDisplayInfo.numberOfXBins      = 0;
	cGraphicEFGCNDisplayInfo.numberOfYBins      = 0;
	cGraphicEFGCNDisplayInfo.baseName           = "";
	cGraphicEFGCNDisplayInfo.baseTitle          = "";
	cGraphicEFGCNDisplayInfo.subDirName         = "";
	nDisplay                                    = NULL;
	nGraphicEFGCNDisplayInfo.numberOfXBins      = 0;
	nGraphicEFGCNDisplayInfo.numberOfYBins      = 0;
	nGraphicEFGCNDisplayInfo.baseName           = "";
	nGraphicEFGCNDisplayInfo.baseTitle          = "";
	nGraphicEFGCNDisplayInfo.subDirName         = "";
	windowPadColumn                             = 0;
	eWindowPadRow                               = (unsigned short)-1;
	fWindowPadRow                               = (unsigned short)-1;
	gWindowPadRow                               = (unsigned short)-1;
	cWindowPadRow                               = (unsigned short)-1;
	nWindowPadRow                               = (unsigned short)-1;
	writeToFile                                 = false;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionEFGCNAnalysis::~projectionEFGCNAnalysis() {

	deleteEDisplay();
	deleteFDisplay();
	deleteGDisplay();
	deleteCDisplay();
	deleteNDisplay();

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void projectionEFGCNAnalysis::initializeEvaluation() {

	reset();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void projectionEFGCNAnalysis::finalizeEvaluation() {

}

/****************************************************************
 * This method initializes the eDisplay.						*
 ****************************************************************/

void projectionEFGCNAnalysis::initEDisplay(int nXBins, int nYBins) {

	if (isEDisplayEnabled()) {
		delete eDisplay;
		eDisplay = NULL;
	}

	eGraphicEFGCNDisplayInfo.numberOfXBins = nXBins;
	eGraphicEFGCNDisplayInfo.numberOfYBins = nYBins;
	eGraphicEFGCNDisplayInfo.baseName      = getNameOfEDisplay();
	eGraphicEFGCNDisplayInfo.baseTitle     = getTitleOfEDisplay();

	eDisplay = new TH2S(eGraphicEFGCNDisplayInfo.baseName.c_str(), eGraphicEFGCNDisplayInfo.baseTitle.c_str(), eGraphicEFGCNDisplayInfo.numberOfXBins, 0, eGraphicEFGCNDisplayInfo.numberOfXBins - 1, eGraphicEFGCNDisplayInfo.numberOfYBins, 0, eGraphicEFGCNDisplayInfo.numberOfYBins - 1);

	if (eDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	eDisplay->SetXTitle(getTitleOfXAxi());
	eDisplay->SetYTitle(getTitleOfYAxi());
	eDisplay->SetDirectory(0);

}

/****************************************************************
 * This method initializes the fDisplay.						*
 ****************************************************************/

void projectionEFGCNAnalysis::initFDisplay(int nXBins, int nYBins) {

	if (isFDisplayEnabled()) {
		delete fDisplay;
		fDisplay = NULL;
	}

	fGraphicEFGCNDisplayInfo.numberOfXBins = nXBins;
	fGraphicEFGCNDisplayInfo.numberOfYBins = nYBins;
	fGraphicEFGCNDisplayInfo.baseName      = getNameOfFDisplay();
	fGraphicEFGCNDisplayInfo.baseTitle     = getTitleOfFDisplay();

	fDisplay = new TH2S(fGraphicEFGCNDisplayInfo.baseName.c_str(), fGraphicEFGCNDisplayInfo.baseTitle.c_str(), fGraphicEFGCNDisplayInfo.numberOfXBins, 0, fGraphicEFGCNDisplayInfo.numberOfXBins - 1, fGraphicEFGCNDisplayInfo.numberOfYBins, 0, fGraphicEFGCNDisplayInfo.numberOfYBins - 1);

	if (fDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	fDisplay->SetXTitle(getTitleOfXAxi());
	fDisplay->SetYTitle(getTitleOfYAxi());
	fDisplay->SetDirectory(0);

}

/****************************************************************
 * This method initializes the gDisplay.						*
 ****************************************************************/

void projectionEFGCNAnalysis::initGDisplay(int nXBins, int nYBins) {

	if (isGDisplayEnabled()) {
		delete gDisplay;
		gDisplay = NULL;
	}

	gGraphicEFGCNDisplayInfo.numberOfXBins = nXBins;
	gGraphicEFGCNDisplayInfo.numberOfYBins = nYBins;
	gGraphicEFGCNDisplayInfo.baseName      = getNameOfGDisplay();
	gGraphicEFGCNDisplayInfo.baseTitle     = getTitleOfGDisplay();

	gDisplay = new TH2S(gGraphicEFGCNDisplayInfo.baseName.c_str(), gGraphicEFGCNDisplayInfo.baseTitle.c_str(), gGraphicEFGCNDisplayInfo.numberOfXBins, 0, gGraphicEFGCNDisplayInfo.numberOfXBins - 1, gGraphicEFGCNDisplayInfo.numberOfYBins, 0, gGraphicEFGCNDisplayInfo.numberOfYBins - 1);

	if (gDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	gDisplay->SetXTitle(getTitleOfXAxi());
	gDisplay->SetYTitle(getTitleOfYAxi());
	gDisplay->SetDirectory(0);

}

/****************************************************************
 * This method initializes the cDisplay.						*
 ****************************************************************/

void projectionEFGCNAnalysis::initCDisplay(int nXBins, int nYBins) {

	if (isCDisplayEnabled()) {
		delete cDisplay;
		cDisplay = NULL;
	}

	cGraphicEFGCNDisplayInfo.numberOfXBins = nXBins;
	cGraphicEFGCNDisplayInfo.numberOfYBins = nYBins;
	cGraphicEFGCNDisplayInfo.baseName      = getNameOfCDisplay();
	cGraphicEFGCNDisplayInfo.baseTitle     = getTitleOfCDisplay();

	cDisplay = new TH2S(cGraphicEFGCNDisplayInfo.baseName.c_str(), cGraphicEFGCNDisplayInfo.baseTitle.c_str(), cGraphicEFGCNDisplayInfo.numberOfXBins, 0, cGraphicEFGCNDisplayInfo.numberOfXBins - 1, cGraphicEFGCNDisplayInfo.numberOfYBins, 0, cGraphicEFGCNDisplayInfo.numberOfYBins - 1);

	if (cDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	cDisplay->SetXTitle(getTitleOfXAxi());
	cDisplay->SetYTitle(getTitleOfYAxi());
	cDisplay->SetDirectory(0);

}

/****************************************************************
 * This method initializes the nDisplay.						*
 ****************************************************************/

void projectionEFGCNAnalysis::initNDisplay(int nXBins, int nYBins) {

	if (isNDisplayEnabled()) {
		delete nDisplay;
		nDisplay = NULL;
	}

	nGraphicEFGCNDisplayInfo.numberOfXBins = nXBins;
	nGraphicEFGCNDisplayInfo.numberOfYBins = nYBins;
	nGraphicEFGCNDisplayInfo.baseName      = getNameOfNDisplay();
	nGraphicEFGCNDisplayInfo.baseTitle     = getTitleOfNDisplay();

	nDisplay = new TH2S(nGraphicEFGCNDisplayInfo.baseName.c_str(), nGraphicEFGCNDisplayInfo.baseTitle.c_str(), nGraphicEFGCNDisplayInfo.numberOfXBins, 0, nGraphicEFGCNDisplayInfo.numberOfXBins - 1, nGraphicEFGCNDisplayInfo.numberOfYBins, 0, nGraphicEFGCNDisplayInfo.numberOfYBins - 1);

	if (nDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	nDisplay->SetXTitle(getTitleOfXAxi());
	nDisplay->SetYTitle(getTitleOfYAxi());
	nDisplay->SetDirectory(0);

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void projectionEFGCNAnalysis::initProjectionAnalysisToRoot(bool enable, const char* name) {

	TFile*      file;
	TDirectory* previousDirectory;
	TDirectory* preSubDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = getNameOfSavingDirectory();
	writeToFile       = false;

	if ((name != NULL) && (strcmp(name, "") != 0)) {

		file = new TFile(name, "UPDATE");
	
		if (file->IsZombie())
			throw cannotOpenFileError(ANALYSISLIB, name);

	}
	else
		if (gDirectory != NULL)
			gDirectory->cd();

	if (gDirectory != NULL) {

		if (enable) {
			
			writeToFile = true;

			if ((!gDirectory->cd(PROJECTIONEFGCNSTSANALYSISDIR)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(PROJECTIONEFGCNSTSANALYSISDIR, PROJECTIONEFGCNSTSANALYSISDIR);
				gDirectory->cd(PROJECTIONEFGCNSTSANALYSISDIR);

			}

			if ((!gDirectory->cd(specialDirectory)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (getSavingInSubDirectory()) {

				preSubDirectory = gDirectory;
				if (isEDisplayEnabled()) {

					if (!gDirectory->cd(ESAVINGSUBDIRECTORY))
						gDirectory->mkdir(ESAVINGSUBDIRECTORY, ESAVINGSUBDIRECTORY);
					eGraphicEFGCNDisplayInfo.subDirName = ESAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isFDisplayEnabled()) {

					if (!gDirectory->cd(FSAVINGSUBDIRECTORY))
						gDirectory->mkdir(FSAVINGSUBDIRECTORY, FSAVINGSUBDIRECTORY);
					fGraphicEFGCNDisplayInfo.subDirName = FSAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isGDisplayEnabled()) {

					if (!gDirectory->cd(GSAVINGSUBDIRECTORY))
						gDirectory->mkdir(GSAVINGSUBDIRECTORY, GSAVINGSUBDIRECTORY);
					gGraphicEFGCNDisplayInfo.subDirName = GSAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isCDisplayEnabled()) {

					if (!gDirectory->cd(CSAVINGSUBDIRECTORY))
						gDirectory->mkdir(CSAVINGSUBDIRECTORY, CSAVINGSUBDIRECTORY);
					cGraphicEFGCNDisplayInfo.subDirName = CSAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isNDisplayEnabled()) {

					if (!gDirectory->cd(NSAVINGSUBDIRECTORY))
						gDirectory->mkdir(NSAVINGSUBDIRECTORY, NSAVINGSUBDIRECTORY);
					nGraphicEFGCNDisplayInfo.subDirName = NSAVINGSUBDIRECTORY;

				}

			}

		}
		else {

			eGraphicEFGCNDisplayInfo.subDirName.clear();
			fGraphicEFGCNDisplayInfo.subDirName.clear();
			gGraphicEFGCNDisplayInfo.subDirName.clear();
			cGraphicEFGCNDisplayInfo.subDirName.clear();

			if (gDirectory->cd(PROJECTIONEFGCNSTSANALYSISDIR)) {

				preSubDirectory = gDirectory;
				if (gDirectory->cd(specialDirectory)) {

					gDirectory->Delete(ESAVINGSUBDIRECTORY);
					gDirectory->Delete(FSAVINGSUBDIRECTORY);
					gDirectory->Delete(GSAVINGSUBDIRECTORY);
					gDirectory->Delete(CSAVINGSUBDIRECTORY);
	
				}

				gDirectory = preSubDirectory;
				gDirectory->Delete(specialDirectory);

			}

			gDirectory = previousDirectory;
			gDirectory->Delete(PROJECTIONEFGCNSTSANALYSISDIR);

		}

	}
	else {

		gDirectoryNotFoundWarningMsg* gDirectoryNotFound = new gDirectoryNotFoundWarningMsg(ANALYSISLIB);
		gDirectoryNotFound->warningMsg();
		if(gDirectoryNotFound != NULL) {
			delete gDirectoryNotFound;
			gDirectoryNotFound = NULL;
		}

	}

	gDirectory = previousDirectory;

	if (file != NULL) {
		file->Close();
		if (file->IsOpen())
			throw cannotCloseFileError(ANALYSISLIB, name);
		delete file;
		file = NULL;
	}

}

/****************************************************************
 * This method deletes the eDisplay.							*
 ****************************************************************/

void projectionEFGCNAnalysis::deleteEDisplay() {

	if (isEDisplayEnabled()) {
		delete eDisplay;
		eDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the fDisplay.							*
 ****************************************************************/

void projectionEFGCNAnalysis::deleteFDisplay() {

	if (isFDisplayEnabled()) {
		delete fDisplay;
		fDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the gDisplay.							*
 ****************************************************************/

void projectionEFGCNAnalysis::deleteGDisplay() {

	if (isGDisplayEnabled()) {
		delete gDisplay;
		gDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the cDisplay.							*
 ****************************************************************/

void projectionEFGCNAnalysis::deleteCDisplay() {

	if (isCDisplayEnabled()) {
		delete cDisplay;
		cDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the nDisplay.							*
 ****************************************************************/

void projectionEFGCNAnalysis::deleteNDisplay() {

	if (isNDisplayEnabled()) {
		delete nDisplay;
		nDisplay = NULL;
	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void projectionEFGCNAnalysis::drawEDisplay(std::string drawOption) {

	if (isEDisplayEnabled()) {

		eDisplay->Draw(drawOption.c_str());

	}
	else {

		cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(ANALYSISLIB);
		cannotDraw->warningMsg();
		if(cannotDraw != NULL) {
			delete cannotDraw;
			cannotDraw = NULL;
		}

	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void projectionEFGCNAnalysis::drawFDisplay(std::string drawOption) {

	if (isFDisplayEnabled()) {

		fDisplay->Draw(drawOption.c_str());

	}
	else {

		cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(ANALYSISLIB);
		cannotDraw->warningMsg();
		if(cannotDraw != NULL) {
			delete cannotDraw;
			cannotDraw = NULL;
		}

	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void projectionEFGCNAnalysis::drawGDisplay(std::string drawOption) {

	if (isGDisplayEnabled()) {

		gDisplay->Draw(drawOption.c_str());

	}
	else {

		cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(ANALYSISLIB);
		cannotDraw->warningMsg();
		if(cannotDraw != NULL) {
			delete cannotDraw;
			cannotDraw = NULL;
		}

	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void projectionEFGCNAnalysis::drawCDisplay(std::string drawOption) {

	if (isCDisplayEnabled()) {

		cDisplay->Draw(drawOption.c_str());

	}
	else {

		cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(ANALYSISLIB);
		cannotDraw->warningMsg();
		if(cannotDraw != NULL) {
			delete cannotDraw;
			cannotDraw = NULL;
		}

	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void projectionEFGCNAnalysis::drawNDisplay(std::string drawOption) {

	if (isNDisplayEnabled()) {

		nDisplay->Draw(drawOption.c_str());

	}
	else {

		cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(ANALYSISLIB);
		cannotDraw->warningMsg();
		if(cannotDraw != NULL) {
			delete cannotDraw;
			cannotDraw = NULL;
		}

	}

}

/****************************************************************
 * This method draws the display corresponding to the drawId.	*
 ****************************************************************/

void projectionEFGCNAnalysis::drawDisplay(unsigned short drawId, std::string drawOption) {

	if (drawId == eWindowPadRow)
		drawEDisplay(drawOption);
	if (drawId == fWindowPadRow)
		drawFDisplay(drawOption);
	if (drawId == gWindowPadRow)
		drawGDisplay(drawOption);
	if (drawId == cWindowPadRow)
		drawCDisplay(drawOption);
	if (drawId == nWindowPadRow)
		drawNDisplay(drawOption);

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void projectionEFGCNAnalysis::writeEDisplay(int eventNumber, std::string outputFileName) {

	if (isEDisplayEnabled())
		writeDisplay(eDisplay, &eGraphicEFGCNDisplayInfo, eventNumber, outputFileName);
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void projectionEFGCNAnalysis::writeFDisplay(int eventNumber, std::string outputFileName) {

	if (isFDisplayEnabled())
		writeDisplay(fDisplay, &fGraphicEFGCNDisplayInfo, eventNumber, outputFileName);
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void projectionEFGCNAnalysis::writeGDisplay(int eventNumber, std::string outputFileName) {

	if (isGDisplayEnabled())
		writeDisplay(gDisplay, &gGraphicEFGCNDisplayInfo, eventNumber, outputFileName);
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void projectionEFGCNAnalysis::writeCDisplay(int eventNumber, std::string outputFileName) {

	if (isCDisplayEnabled())
		writeDisplay(cDisplay, &cGraphicEFGCNDisplayInfo, eventNumber, outputFileName);
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void projectionEFGCNAnalysis::writeNDisplay(int eventNumber, std::string outputFileName) {

	if (isNDisplayEnabled())
		writeDisplay(nDisplay, &nGraphicEFGCNDisplayInfo, eventNumber, outputFileName);
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

}

/****************************************************************
 * This method returns true, if the eDisplay is enabled.		*
 ****************************************************************/

bool projectionEFGCNAnalysis::isEDisplayEnabled() {

	bool returnValue;

	if (eDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the fDisplay is enabled.		*
 ****************************************************************/

bool projectionEFGCNAnalysis::isFDisplayEnabled() {

	bool returnValue;

	if (fDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the gDisplay is enabled.		*
 ****************************************************************/

bool projectionEFGCNAnalysis::isGDisplayEnabled() {

	bool returnValue;

	if (gDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the cDisplay is enabled.		*
 ****************************************************************/

bool projectionEFGCNAnalysis::isCDisplayEnabled() {

	bool returnValue;

	if (cDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the cDisplay is enabled.		*
 ****************************************************************/

bool projectionEFGCNAnalysis::isNDisplayEnabled() {

	bool returnValue;

	if (nDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * enabled.														*
 ****************************************************************/

bool projectionEFGCNAnalysis::isDisplayEnabled() {

	return (isEDisplayEnabled() || isFDisplayEnabled() || isGDisplayEnabled() || isCDisplayEnabled() || isNDisplayEnabled());

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * written to file.												*
 ****************************************************************/

bool projectionEFGCNAnalysis::isWritingEnabled() {

	return (writeToFile & isDisplayEnabled());

}

/****************************************************************
 * This method returns the number of all displays.				*
 ****************************************************************/

unsigned short projectionEFGCNAnalysis::getNumberOfDisplays() {

	return NUMBEROFGRAPHICEFGCNDISPLAYS;

}

/****************************************************************
 * This method returns the signature of the enabled displays.	*
 ****************************************************************/

void projectionEFGCNAnalysis::getSignatureOfEnabledDisplays(bool* signature) {

	if (signature != NULL) {

		signature[0] = isEDisplayEnabled();
		signature[1] = isFDisplayEnabled();
		signature[2] = isGDisplayEnabled();
		signature[3] = isCDisplayEnabled();
		signature[4] = isNDisplayEnabled();

	}
	else {

		cannotCreateSignatureWarningMsg* cannotCreateSignature = new cannotCreateSignatureWarningMsg();
		cannotCreateSignature->warningMsg();
		if(cannotCreateSignature != NULL) {
			delete cannotCreateSignature;
			cannotCreateSignature = NULL;
		}

	}

}

/****************************************************************
 * gets the windowPadColumn										*
 ****************************************************************/

unsigned short projectionEFGCNAnalysis::getWindowPadColumn() {

	return windowPadColumn;

}

/****************************************************************
 * sets the windowPadColumn										*
 ****************************************************************/

void projectionEFGCNAnalysis::setWindowPadColumn(unsigned short column) {

	windowPadColumn = column;

}

/****************************************************************
 * sets the windowPadRow										*
 ****************************************************************/

void projectionEFGCNAnalysis::setWindowPadRow(bool* globalSignature, unsigned short sizeOfGlobalSignature) {

	unsigned short counter;

	if (getNumberOfDisplays() > sizeOfGlobalSignature) {

		notAllDisplaysCanBeDrawnWarningMsg* notAllDisplaysCanBeDrawn = new notAllDisplaysCanBeDrawnWarningMsg();
		notAllDisplaysCanBeDrawn->warningMsg();
		if(notAllDisplaysCanBeDrawn != NULL) {
			delete notAllDisplaysCanBeDrawn;
			notAllDisplaysCanBeDrawn = NULL;
		}

	}

	counter = 0;
	if (globalSignature[0]) {
		eWindowPadRow = counter;
		counter++;
	}
	if (globalSignature[1]) {
		fWindowPadRow = counter;
		counter++;
	}
	if (globalSignature[2]) {
		gWindowPadRow = counter;
		counter++;
	}
	if (globalSignature[3]) {
		cWindowPadRow = counter;
		counter++;
	}
	if (globalSignature[4]) {
		nWindowPadRow = counter;
		counter++;
	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionEFGCNAnalysis::peakToFakeTrack(trackCoordinates& coordinates) {

	int xEntry;
	int yEntry;

	if (isFDisplayEnabled()) {
		
		xEntry = getEntryOfXAxi(coordinates);
		yEntry = getEntryOfYAxi(coordinates);

		if ((xEntry >= 0) && (xEntry < fGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < fGraphicEFGCNDisplayInfo.numberOfYBins))
			fDisplay->SetBinContent(xEntry + 1, yEntry + 1, fDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionEFGCNAnalysis::peakToNoTrack(trackCoordinates& coordinates) {

	int xEntry;
	int yEntry;

	if (isGDisplayEnabled()) {
		
		xEntry = getEntryOfXAxi(coordinates);
		yEntry = getEntryOfYAxi(coordinates);

		if ((xEntry >= 0) && (xEntry < gGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < gGraphicEFGCNDisplayInfo.numberOfYBins))
			gDisplay->SetBinContent(xEntry + 1, yEntry + 1, gDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionEFGCNAnalysis::peakToOneTrack(trackCoordinates& coordinates, bool isClone, bool isWrong) {

	int xEntry;
	int yEntry;

	if ((isCDisplayEnabled()) || (isEDisplayEnabled())) {

		xEntry = getEntryOfXAxi(coordinates);
		yEntry = getEntryOfYAxi(coordinates);

	}

	if (isClone) {

		if ((!isWrong) && (isCDisplayEnabled())) {

			if ((xEntry >= 0) && (xEntry < cGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < cGraphicEFGCNDisplayInfo.numberOfYBins))
				cDisplay->SetBinContent(xEntry + 1, yEntry + 1, cDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

		}

	}
	else {

		if ((!isWrong) && (isEDisplayEnabled())) {

			if ((xEntry >= 0) && (xEntry < eGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < eGraphicEFGCNDisplayInfo.numberOfYBins))
				eDisplay->SetBinContent(xEntry + 1, yEntry + 1, eDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

		}

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionEFGCNAnalysis::peakToMoreTracks(trackCoordinates& coordinates, bool isClone, bool isWrong) {

	int xEntry;
	int yEntry;

	if ((isCDisplayEnabled()) || (isEDisplayEnabled())) {

		xEntry = getEntryOfXAxi(coordinates);
		yEntry = getEntryOfYAxi(coordinates);

	}

	if (isClone) {

		if ((!isWrong) && (isCDisplayEnabled())) {

			if ((xEntry >= 0) && (xEntry < cGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < cGraphicEFGCNDisplayInfo.numberOfYBins))
				cDisplay->SetBinContent(xEntry + 1, yEntry + 1, cDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

		}

	}
	else {

		if ((!isWrong) && (isEDisplayEnabled())) {

			if ((xEntry >= 0) && (xEntry < eGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < eGraphicEFGCNDisplayInfo.numberOfYBins))
				eDisplay->SetBinContent(xEntry + 1, yEntry + 1, eDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

		}

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void projectionEFGCNAnalysis::trackToNoPeak(trackCoordinates& coordinates) {

	int xEntry;
	int yEntry;

	if (isNDisplayEnabled()) {
		
		xEntry = getEntryOfXAxi(coordinates);
		yEntry = getEntryOfYAxi(coordinates);

		if ((xEntry >= 0) && (xEntry < nGraphicEFGCNDisplayInfo.numberOfXBins) && (yEntry >= 0) && (yEntry < nGraphicEFGCNDisplayInfo.numberOfYBins))
			nDisplay->SetBinContent(xEntry + 1, yEntry + 1, nDisplay->GetBinContent(xEntry + 1, yEntry + 1) + 1);

	}

}

/****************************************************************
 * This method returns the eDisplay's axi range.				*
 ****************************************************************/

projectionEFGCNDisplayInfo projectionEFGCNAnalysis::getEDisplayAxiRange() {

	return eGraphicEFGCNDisplayInfo;

}

/****************************************************************
 * This method returns the fDisplay's axi range.				*
 ****************************************************************/

projectionEFGCNDisplayInfo projectionEFGCNAnalysis::getFDisplayAxiRange() {

	return fGraphicEFGCNDisplayInfo;

}

/****************************************************************
 * This method returns the gDisplay's axi range.				*
 ****************************************************************/

projectionEFGCNDisplayInfo projectionEFGCNAnalysis::getGDisplayAxiRange() {

	return gGraphicEFGCNDisplayInfo;

}

/****************************************************************
 * This method returns the cDisplay's axi range.				*
 ****************************************************************/

projectionEFGCNDisplayInfo projectionEFGCNAnalysis::getCDisplayAxiRange() {

	return cGraphicEFGCNDisplayInfo;

}

/****************************************************************
 * This method returns the nDisplay's axi range.				*
 ****************************************************************/

projectionEFGCNDisplayInfo projectionEFGCNAnalysis::getNDisplayAxiRange() {

	return nGraphicEFGCNDisplayInfo;

}

/****************************************************************
 * This method returns the number of eDisplay entries.			*
 ****************************************************************/

unsigned int projectionEFGCNAnalysis::getEDisplayEntries() {

	unsigned int returnValue;

	if (isEDisplayEnabled())
		returnValue = (unsigned int)eDisplay->GetEntries();
	else {

		cannotGetDisplayEntriesWarningMsg* cannotGetDisplayEntries = new cannotGetDisplayEntriesWarningMsg();
		cannotGetDisplayEntries->warningMsg();
		if(cannotGetDisplayEntries != NULL) {
			delete cannotGetDisplayEntries;
			cannotGetDisplayEntries = NULL;
		}

		returnValue = 1; /* set to 1 because of the usage for a division, to avoid x/0. */

	}

	return returnValue;

}

/****************************************************************
 * This method returns the number of fDisplay entries.			*
 ****************************************************************/

unsigned int projectionEFGCNAnalysis::getFDisplayEntries() {

	unsigned int returnValue;

	if (isFDisplayEnabled())
		returnValue = (unsigned int)fDisplay->GetEntries();
	else {

		cannotGetDisplayEntriesWarningMsg* cannotGetDisplayEntries = new cannotGetDisplayEntriesWarningMsg();
		cannotGetDisplayEntries->warningMsg();
		if(cannotGetDisplayEntries != NULL) {
			delete cannotGetDisplayEntries;
			cannotGetDisplayEntries = NULL;
		}

		returnValue = 1; /* set to 1 because of the usage for a division, to avoid x/0. */

	}

	return returnValue;

}

/****************************************************************
 * This method returns the number of gDisplay entries.			*
 ****************************************************************/

unsigned int projectionEFGCNAnalysis::getGDisplayEntries() {

	unsigned int returnValue;

	if (isGDisplayEnabled())
		returnValue = (unsigned int)gDisplay->GetEntries();
	else {

		cannotGetDisplayEntriesWarningMsg* cannotGetDisplayEntries = new cannotGetDisplayEntriesWarningMsg();
		cannotGetDisplayEntries->warningMsg();
		if(cannotGetDisplayEntries != NULL) {
			delete cannotGetDisplayEntries;
			cannotGetDisplayEntries = NULL;
		}

		returnValue = 1; /* set to 1 because of the usage for a division, to avoid x/0. */

	}

	return returnValue;

}

/****************************************************************
 * This method returns the number of cDisplay entries.			*
 ****************************************************************/

unsigned int projectionEFGCNAnalysis::getCDisplayEntries() {

	unsigned int returnValue;

	if (isCDisplayEnabled())
		returnValue = (unsigned int)cDisplay->GetEntries();
	else {

		cannotGetDisplayEntriesWarningMsg* cannotGetDisplayEntries = new cannotGetDisplayEntriesWarningMsg();
		cannotGetDisplayEntries->warningMsg();
		if(cannotGetDisplayEntries != NULL) {
			delete cannotGetDisplayEntries;
			cannotGetDisplayEntries = NULL;
		}

		returnValue = 1; /* set to 1 because of the usage for a division, to avoid x/0. */

	}

	return returnValue;

}

/****************************************************************
 * This method returns the number of cDisplay entries.			*
 ****************************************************************/

unsigned int projectionEFGCNAnalysis::getNDisplayEntries() {

	unsigned int returnValue;

	if (isNDisplayEnabled())
		returnValue = (unsigned int)nDisplay->GetEntries();
	else {

		cannotGetDisplayEntriesWarningMsg* cannotGetDisplayEntries = new cannotGetDisplayEntriesWarningMsg();
		cannotGetDisplayEntries->warningMsg();
		if(cannotGetDisplayEntries != NULL) {
			delete cannotGetDisplayEntries;
			cannotGetDisplayEntries = NULL;
		}

		returnValue = 1; /* set to 1 because of the usage for a division, to avoid x/0. */

	}

	return returnValue;

}
