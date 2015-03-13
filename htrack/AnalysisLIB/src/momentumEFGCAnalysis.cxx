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
// $Date: 2008-09-11 14:07:03 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisWarningMsg.h"
#include "../include/momentumEFGCAnalysisDef.h"
#include "../include/momentumEFGCAnalysis.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"


/****************************************************************
 * resets the displays											*
 ****************************************************************/

void momentumEFGCAnalysis::resetDisplays() {

	if (isEMDisplayEnabled())
		emDisplay->Reset();
	emGraphicEFGCDisplayInfo.numberOfUnderflows = 0;
	emGraphicEFGCDisplayInfo.numberOfOverflows  = 0;

	if (isFMDisplayEnabled())
		fmDisplay->Reset();
	fmGraphicEFGCDisplayInfo.numberOfUnderflows = 0;
	fmGraphicEFGCDisplayInfo.numberOfOverflows  = 0;

	if (isGMDisplayEnabled())
		gmDisplay->Reset();
	gmGraphicEFGCDisplayInfo.numberOfUnderflows = 0;
	gmGraphicEFGCDisplayInfo.numberOfOverflows  = 0;

	if (isCMDisplayEnabled())
		cmDisplay->Reset();
	cmGraphicEFGCDisplayInfo.numberOfUnderflows = 0;
	cmGraphicEFGCDisplayInfo.numberOfOverflows  = 0;

}

/****************************************************************
 * This method writes a display into the file.					*
 ****************************************************************/

void momentumEFGCAnalysis::writeDisplay(TProfile* display, momentumEFGCDisplayInfo* info, int eventNumber, std::string outputFileName) {

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
			modifiedDisplayName += MOMENTUMEFGCEVENTMODIFIER;
			modifiedDisplayName += "_";
			modifiedDisplayName += buffer;

			modifiedDisplayTitle  = display->GetTitle();
			modifiedDisplayTitle += " ";
			modifiedDisplayTitle += MOMENTUMEFGCEVENTMODIFIER;
			modifiedDisplayTitle += ":";
			modifiedDisplayTitle += buffer;

			display->SetName(modifiedDisplayName.c_str());
			display->SetTitle(modifiedDisplayTitle.c_str());

			if (!gDirectory->cd(MOMENTUMEFGCSTSANALYSISDIR)) {

				gDirectory->mkdir(MOMENTUMEFGCSTSANALYSISDIR, MOMENTUMEFGCSTSANALYSISDIR);
				gDirectory->cd(MOMENTUMEFGCSTSANALYSISDIR);

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

momentumEFGCAnalysis::momentumEFGCAnalysis() 
  : emDisplay(NULL),
    emGraphicEFGCDisplayInfo(),
    fmDisplay(NULL),
    fmGraphicEFGCDisplayInfo(),
    gmDisplay(NULL),
    gmGraphicEFGCDisplayInfo(),
    cmDisplay(NULL),
    cmGraphicEFGCDisplayInfo(),
    formula(),
    windowPadColumn(0),
    emWindowPadRow((unsigned short)-1),
    fmWindowPadRow((unsigned short)-1),
    gmWindowPadRow((unsigned short)-1),
    cmWindowPadRow((unsigned short)-1),
    writeToFile(false)
{

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

momentumEFGCAnalysis::~momentumEFGCAnalysis() {

	deleteEMDisplay();
	deleteFMDisplay();
	deleteGMDisplay();
	deleteCMDisplay();

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void momentumEFGCAnalysis::initializeEvaluation() {

	reset();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void momentumEFGCAnalysis::finalizeEvaluation() {

}

/****************************************************************
 * This method initializes the emDisplay.						*
 ****************************************************************/

void momentumEFGCAnalysis::initEMDisplay(int nBins, int xMin, int xMax) {

	if (isEMDisplayEnabled()) {
		delete emDisplay;
		emDisplay = NULL;
	}

	emGraphicEFGCDisplayInfo.min          = xMin;
	emGraphicEFGCDisplayInfo.max          = xMax;
	emGraphicEFGCDisplayInfo.numberOfBins = nBins;
	emGraphicEFGCDisplayInfo.baseName     = getNameOfEMDisplay();
	emGraphicEFGCDisplayInfo.baseTitle    = getTitleOfEMDisplay();

	emDisplay = new TProfile(emGraphicEFGCDisplayInfo.baseName.c_str(), emGraphicEFGCDisplayInfo.baseTitle.c_str(), emGraphicEFGCDisplayInfo.numberOfBins, emGraphicEFGCDisplayInfo.min, emGraphicEFGCDisplayInfo.max, YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE, "i");

	if (emDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	emDisplay->SetDirectory(0);
	emDisplay->GetYaxis()->SetRangeUser(YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE);
	emDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
	emDisplay->GetYaxis()->SetTitle(EMYAXITITLE);

}

/****************************************************************
 * This method initializes the fmDisplay.						*
 ****************************************************************/

void momentumEFGCAnalysis::initFMDisplay(int nBins, int xMin, int xMax) {

	if (isFMDisplayEnabled()) {
		delete fmDisplay;
		fmDisplay = NULL;
	}

	fmGraphicEFGCDisplayInfo.min          = xMin;
	fmGraphicEFGCDisplayInfo.max          = xMax;
	fmGraphicEFGCDisplayInfo.numberOfBins = nBins;
	fmGraphicEFGCDisplayInfo.baseName     = getNameOfFMDisplay();
	fmGraphicEFGCDisplayInfo.baseTitle    = getTitleOfFMDisplay();

	fmDisplay = new TProfile(fmGraphicEFGCDisplayInfo.baseName.c_str(), fmGraphicEFGCDisplayInfo.baseTitle.c_str(), fmGraphicEFGCDisplayInfo.numberOfBins, fmGraphicEFGCDisplayInfo.min, fmGraphicEFGCDisplayInfo.max, YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE, "i");

	if (fmDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	fmDisplay->SetDirectory(0);
	fmDisplay->GetYaxis()->SetRangeUser(YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE);
	fmDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
	fmDisplay->GetYaxis()->SetTitle(FMYAXITITLE);

}

/****************************************************************
 * This method initializes the gmDisplay.						*
 ****************************************************************/

void momentumEFGCAnalysis::initGMDisplay(int nBins, int xMin, int xMax) {

	if (isGMDisplayEnabled()) {
		delete gmDisplay;
		gmDisplay = NULL;
	}

	gmGraphicEFGCDisplayInfo.min          = xMin;
	gmGraphicEFGCDisplayInfo.max          = xMax;
	gmGraphicEFGCDisplayInfo.numberOfBins = nBins;
	gmGraphicEFGCDisplayInfo.baseName     = getNameOfGMDisplay();
	gmGraphicEFGCDisplayInfo.baseTitle    = getTitleOfGMDisplay();

	gmDisplay = new TProfile(gmGraphicEFGCDisplayInfo.baseName.c_str(), gmGraphicEFGCDisplayInfo.baseTitle.c_str(), gmGraphicEFGCDisplayInfo.numberOfBins, gmGraphicEFGCDisplayInfo.min, gmGraphicEFGCDisplayInfo.max, YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE, "i");

	if (gmDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	gmDisplay->SetDirectory(0);
	gmDisplay->GetYaxis()->SetRangeUser(YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE);
	gmDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
	gmDisplay->GetYaxis()->SetTitle(GMYAXITITLE);

}

/****************************************************************
 * This method initializes the gmDisplay.						*
 ****************************************************************/

void momentumEFGCAnalysis::initCMDisplay(int nBins, int xMin, int xMax) {

	if (isCMDisplayEnabled()) {
		delete cmDisplay;
		cmDisplay = NULL;
	}

	cmGraphicEFGCDisplayInfo.min          = xMin;
	cmGraphicEFGCDisplayInfo.max          = xMax;
	cmGraphicEFGCDisplayInfo.numberOfBins = nBins;
	cmGraphicEFGCDisplayInfo.baseName     = getNameOfCMDisplay();
	cmGraphicEFGCDisplayInfo.baseTitle    = getTitleOfCMDisplay();

	cmDisplay = new TProfile(cmGraphicEFGCDisplayInfo.baseName.c_str(), cmGraphicEFGCDisplayInfo.baseTitle.c_str(), cmGraphicEFGCDisplayInfo.numberOfBins, cmGraphicEFGCDisplayInfo.min, cmGraphicEFGCDisplayInfo.max, YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE, "i");

	if (cmDisplay == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	cmDisplay->SetDirectory(0);
	cmDisplay->GetYaxis()->SetRangeUser(YAXIMIN-YAXIOVERSIZE, YAXIMAX+YAXIOVERSIZE);
	cmDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
	cmDisplay->GetYaxis()->SetTitle(CMYAXITITLE);

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void momentumEFGCAnalysis::initMomentumAnalysisToRoot(bool enable, const char* name) {

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

			if ((!gDirectory->cd(MOMENTUMEFGCSTSANALYSISDIR)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(MOMENTUMEFGCSTSANALYSISDIR, MOMENTUMEFGCSTSANALYSISDIR);
				gDirectory->cd(MOMENTUMEFGCSTSANALYSISDIR);

			}

			if ((!gDirectory->cd(specialDirectory)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (getSavingInSubDirectory()) {

				preSubDirectory = gDirectory;
				if (isEMDisplayEnabled()) {

					if (!gDirectory->cd(EMSAVINGSUBDIRECTORY))
						gDirectory->mkdir(EMSAVINGSUBDIRECTORY, EMSAVINGSUBDIRECTORY);
					emGraphicEFGCDisplayInfo.subDirName = EMSAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isFMDisplayEnabled()) {

					if (!gDirectory->cd(FMSAVINGSUBDIRECTORY))
						gDirectory->mkdir(FMSAVINGSUBDIRECTORY, FMSAVINGSUBDIRECTORY);
					fmGraphicEFGCDisplayInfo.subDirName = FMSAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isGMDisplayEnabled()) {

					if (!gDirectory->cd(GMSAVINGSUBDIRECTORY))
						gDirectory->mkdir(GMSAVINGSUBDIRECTORY, GMSAVINGSUBDIRECTORY);
					gmGraphicEFGCDisplayInfo.subDirName = GMSAVINGSUBDIRECTORY;

				}

				gDirectory = preSubDirectory;
				if (isCMDisplayEnabled()) {

					if (!gDirectory->cd(CMSAVINGSUBDIRECTORY))
						gDirectory->mkdir(CMSAVINGSUBDIRECTORY, CMSAVINGSUBDIRECTORY);
					cmGraphicEFGCDisplayInfo.subDirName = CMSAVINGSUBDIRECTORY;

				}

			}

		}
		else {

			emGraphicEFGCDisplayInfo.subDirName.clear();
			fmGraphicEFGCDisplayInfo.subDirName.clear();
			gmGraphicEFGCDisplayInfo.subDirName.clear();
			cmGraphicEFGCDisplayInfo.subDirName.clear();

			if (gDirectory->cd(MOMENTUMEFGCSTSANALYSISDIR)) {

				preSubDirectory = gDirectory;
				if (gDirectory->cd(specialDirectory)) {

					gDirectory->Delete(EMSAVINGSUBDIRECTORY);
					gDirectory->Delete(FMSAVINGSUBDIRECTORY);
					gDirectory->Delete(GMSAVINGSUBDIRECTORY);
					gDirectory->Delete(CMSAVINGSUBDIRECTORY);
	
				}

				gDirectory = preSubDirectory;
				gDirectory->Delete(specialDirectory);

			}

			gDirectory = previousDirectory;
			gDirectory->Delete(MOMENTUMEFGCSTSANALYSISDIR);

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
 * This method deletes the emDisplay.							*
 ****************************************************************/

void momentumEFGCAnalysis::deleteEMDisplay() {

	if (isEMDisplayEnabled()) {
		delete emDisplay;
		emDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the fmDisplay.							*
 ****************************************************************/

void momentumEFGCAnalysis::deleteFMDisplay() {

	if (isFMDisplayEnabled()) {
		delete fmDisplay;
		fmDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the gmDisplay.							*
 ****************************************************************/

void momentumEFGCAnalysis::deleteGMDisplay() {

	if (isGMDisplayEnabled()) {
		delete gmDisplay;
		gmDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the cmDisplay.							*
 ****************************************************************/

void momentumEFGCAnalysis::deleteCMDisplay() {

	if (isCMDisplayEnabled()) {
		delete cmDisplay;
		cmDisplay = NULL;
	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void momentumEFGCAnalysis::drawEMDisplay(unsigned int error) {

	if (isEMDisplayEnabled()) {

		switch (error) {

			case 0:
				emDisplay->Draw("9HIST");		/* draws without errors */
				break;
			default:
				emDisplay->Draw("9E0");			/* draws with default error */
				break;

		}

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

void momentumEFGCAnalysis::drawFMDisplay(unsigned int error) {

	if (isFMDisplayEnabled()) {

		switch (error) {

			case 0:
				fmDisplay->Draw("9HIST");		/* draws without errors */
				break;
			default:
				fmDisplay->Draw("9E0");			/* draws with default error */
				break;

		}

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

void momentumEFGCAnalysis::drawGMDisplay(unsigned int error) {

	if (isGMDisplayEnabled()) {

		switch (error) {

			case 0:
				gmDisplay->Draw("9HIST");		/* draws without errors */
				break;
			default:
				gmDisplay->Draw("9E0");			/* draws with default error */
				break;

		}

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

void momentumEFGCAnalysis::drawCMDisplay(unsigned int error) {

	if (isCMDisplayEnabled()) {

		switch (error) {

			case 0:
				cmDisplay->Draw("9HIST");		/* draws without errors */
				break;
			default:
				cmDisplay->Draw("9E0");			/* draws with default error */
				break;

		}

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

void momentumEFGCAnalysis::drawDisplay(unsigned short drawId, unsigned int error) {

	if (drawId == emWindowPadRow)
		drawEMDisplay(error);
	if (drawId == fmWindowPadRow)
		drawFMDisplay(error);
	if (drawId == gmWindowPadRow)
		drawGMDisplay(error);
	if (drawId == cmWindowPadRow)
		drawCMDisplay(error);

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void momentumEFGCAnalysis::writeEMDisplay(int eventNumber, std::string outputFileName) {

	if (isEMDisplayEnabled())
		writeDisplay(emDisplay, &emGraphicEFGCDisplayInfo, eventNumber, outputFileName);
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

void momentumEFGCAnalysis::writeFMDisplay(int eventNumber, std::string outputFileName) {

	if (isFMDisplayEnabled())
		writeDisplay(fmDisplay, &fmGraphicEFGCDisplayInfo, eventNumber, outputFileName);
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

void momentumEFGCAnalysis::writeGMDisplay(int eventNumber, std::string outputFileName) {

	if (isGMDisplayEnabled())
		writeDisplay(gmDisplay, &gmGraphicEFGCDisplayInfo, eventNumber, outputFileName);
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

void momentumEFGCAnalysis::writeCMDisplay(int eventNumber, std::string outputFileName) {

	if (isCMDisplayEnabled())
		writeDisplay(cmDisplay, &cmGraphicEFGCDisplayInfo, eventNumber, outputFileName);
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
 * This method returns true, if the emDisplay is enabled.		*
 ****************************************************************/

bool momentumEFGCAnalysis::isEMDisplayEnabled() {

	bool returnValue;

	if (emDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the fmDisplay is enabled.		*
 ****************************************************************/

bool momentumEFGCAnalysis::isFMDisplayEnabled() {

	bool returnValue;

	if (fmDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the gmDisplay is enabled.		*
 ****************************************************************/

bool momentumEFGCAnalysis::isGMDisplayEnabled() {

	bool returnValue;

	if (gmDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the cmDisplay is enabled.		*
 ****************************************************************/

bool momentumEFGCAnalysis::isCMDisplayEnabled() {

	bool returnValue;

	if (cmDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * enabled.														*
 ****************************************************************/

bool momentumEFGCAnalysis::isDisplayEnabled() {

	return (isEMDisplayEnabled() || isFMDisplayEnabled() || isGMDisplayEnabled() || isCMDisplayEnabled());

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * written to file.												*
 ****************************************************************/

bool momentumEFGCAnalysis::isWritingEnabled() {

	return (writeToFile & isDisplayEnabled());

}

/****************************************************************
 * This method returns the number of all displays.				*
 ****************************************************************/

unsigned short momentumEFGCAnalysis::getNumberOfDisplays() {

	return NUMBEROFGRAPHICEFGCDISPLAYS;

}

/****************************************************************
 * This method returns the signature of the enabled displays.	*
 ****************************************************************/

void momentumEFGCAnalysis::getSignatureOfEnabledDisplays(bool* signature) {

	if (signature != NULL) {

		signature[0] = isEMDisplayEnabled();
		signature[1] = isFMDisplayEnabled();
		signature[2] = isGMDisplayEnabled();
		signature[3] = isCMDisplayEnabled();

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

unsigned short momentumEFGCAnalysis::getWindowPadColumn() {

	return windowPadColumn;

}

/****************************************************************
 * sets the windowPadColumn										*
 ****************************************************************/

void momentumEFGCAnalysis::setWindowPadColumn(unsigned short column) {

	windowPadColumn = column;

}

/****************************************************************
 * sets the windowPadRow										*
 ****************************************************************/

void momentumEFGCAnalysis::setWindowPadRow(bool* globalSignature, unsigned short sizeOfGlobalSignature) {

	unsigned short counter;
	unsigned short counterMax;

	if (getNumberOfDisplays() > sizeOfGlobalSignature) {

		notAllDisplaysCanBeDrawnWarningMsg* notAllDisplaysCanBeDrawn = new notAllDisplaysCanBeDrawnWarningMsg();
		notAllDisplaysCanBeDrawn->warningMsg();
		if(notAllDisplaysCanBeDrawn != NULL) {
			delete notAllDisplaysCanBeDrawn;
			notAllDisplaysCanBeDrawn = NULL;
		}

		counterMax = sizeOfGlobalSignature;

	}
	else
		counterMax = getNumberOfDisplays();

	counter = 0;
	if (counter < counterMax) {

		if (globalSignature[counter]) {
			emWindowPadRow = counter;
			counter++;
		}

	}

	if (counter < counterMax) {

		if (globalSignature[counter]) {
			fmWindowPadRow = counter;
			counter++;
		}

	}

	if (counter < counterMax) {

		if (globalSignature[counter]) {
			gmWindowPadRow = counter;
			counter++;
		}

	}

	if (counter < counterMax) {

		if (globalSignature[counter]) {
			cmWindowPadRow = counter;
			counter++;
		}

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::peakToFakeTrack(trackParameter& parameter, histogramSpace& space) {

	trackMomentum momentum;
	double        value;

	if ((isFMDisplayEnabled()) || (isGMDisplayEnabled()) || (isCMDisplayEnabled())) {
		
		formula.evaluateP(parameter, space, &momentum, NULL);
		value = getEntryOfXAxi(momentum);

	}
	else
		value = 0;

	if (isFMDisplayEnabled()) {

		if (value < fmGraphicEFGCDisplayInfo.min) {
			fmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.min) + ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else if (value > fmGraphicEFGCDisplayInfo.max) {
			fmGraphicEFGCDisplayInfo.numberOfOverflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.max) - ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else
			fmDisplay->Fill(value, YAXIMAX);

	}

	if (isGMDisplayEnabled()) {

		if (value < gmGraphicEFGCDisplayInfo.min) {
			gmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.min) + ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > gmGraphicEFGCDisplayInfo.max) {
			gmGraphicEFGCDisplayInfo.numberOfOverflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.max) - ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			gmDisplay->Fill(value, YAXIMIN);

	}

	if (isCMDisplayEnabled()) {

		if (value < cmGraphicEFGCDisplayInfo.min) {
			cmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.min) + ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > cmGraphicEFGCDisplayInfo.max) {
			cmGraphicEFGCDisplayInfo.numberOfOverflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.max) - ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			cmDisplay->Fill(value, YAXIMIN);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::peakToNoTrack(trackParameter& parameter, histogramSpace& space) {

	trackMomentum momentum;
	double        value;

	if ((isFMDisplayEnabled()) || (isGMDisplayEnabled()) || (isCMDisplayEnabled())) {
		
		formula.evaluateP(parameter, space, &momentum, NULL);
		value = getEntryOfXAxi(momentum);

	}
	else
		value = 0;

	if (isFMDisplayEnabled()) {

		if (value < fmGraphicEFGCDisplayInfo.min) {
			fmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.min) + ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > fmGraphicEFGCDisplayInfo.max) {
			fmGraphicEFGCDisplayInfo.numberOfOverflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.max) - ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			fmDisplay->Fill(value, YAXIMIN);

	}

	if (isGMDisplayEnabled()) {

		if (value < gmGraphicEFGCDisplayInfo.min) {
			gmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.min) + ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else if (value > gmGraphicEFGCDisplayInfo.max) {
			gmGraphicEFGCDisplayInfo.numberOfOverflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.max) - ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else
			gmDisplay->Fill(value, YAXIMAX);

	}

	if (isCMDisplayEnabled()) {

		if (value < cmGraphicEFGCDisplayInfo.min) {
			cmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.min) + ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > cmGraphicEFGCDisplayInfo.max) {
			cmGraphicEFGCDisplayInfo.numberOfOverflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.max) - ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			cmDisplay->Fill(value, YAXIMIN);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::peakToOneTrack(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong) {

	trackMomentum momentum;
	double        value;
	int           entry;

	if ((isFMDisplayEnabled()) || (isGMDisplayEnabled()) || (isCMDisplayEnabled())) {
		
		formula.evaluateP(parameter, space, &momentum, NULL);
		value = getEntryOfXAxi(momentum);

	}
	else
		value = 0;

	if (isFMDisplayEnabled()) {

		if (value < fmGraphicEFGCDisplayInfo.min) {
			fmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.min) + ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > fmGraphicEFGCDisplayInfo.max) {
			fmGraphicEFGCDisplayInfo.numberOfOverflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.max) - ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			fmDisplay->Fill(value, YAXIMIN);

	}

	if (isGMDisplayEnabled()) {

		if (value < gmGraphicEFGCDisplayInfo.min) {
			gmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.min) + ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > gmGraphicEFGCDisplayInfo.max) {
			gmGraphicEFGCDisplayInfo.numberOfOverflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.max) - ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			gmDisplay->Fill(value, YAXIMIN);

	}

	if (isCMDisplayEnabled()) {

		if ((isClone) && (!isWrong))
			entry = YAXIMAX;
		else
			entry = YAXIMIN;

		if (value < cmGraphicEFGCDisplayInfo.min) {
			cmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.min) + ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), entry);
		}
		else if (value > cmGraphicEFGCDisplayInfo.max) {
			cmGraphicEFGCDisplayInfo.numberOfOverflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.max) - ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), entry);
		}
		else
			cmDisplay->Fill(value, entry);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special	*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::peakToMoreTracks(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong) {

	trackMomentum momentum;
	double        value;
	int           entry;

	if ((isFMDisplayEnabled()) || (isGMDisplayEnabled()) || (isCMDisplayEnabled())) {
		
		formula.evaluateP(parameter, space, &momentum, NULL);
		value = getEntryOfXAxi(momentum);

	}
	else
		value = 0;

	if (isFMDisplayEnabled()) {

		if (value < fmGraphicEFGCDisplayInfo.min) {
			fmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.min) + ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > fmGraphicEFGCDisplayInfo.max) {
			fmGraphicEFGCDisplayInfo.numberOfOverflows++;
			fmDisplay->Fill(((double)fmGraphicEFGCDisplayInfo.max) - ((double)(fmGraphicEFGCDisplayInfo.max - fmGraphicEFGCDisplayInfo.min)) / ((double)fmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			fmDisplay->Fill(value, YAXIMIN);

	}

	if (isGMDisplayEnabled()) {

		if (value < gmGraphicEFGCDisplayInfo.min) {
			gmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.min) + ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > gmGraphicEFGCDisplayInfo.max) {
			gmGraphicEFGCDisplayInfo.numberOfOverflows++;
			gmDisplay->Fill(((double)gmGraphicEFGCDisplayInfo.max) - ((double)(gmGraphicEFGCDisplayInfo.max - gmGraphicEFGCDisplayInfo.min)) / ((double)gmGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			gmDisplay->Fill(value, YAXIMIN);
	
	}

	if (isCMDisplayEnabled()) {

		if ((isClone) && (!isWrong))
			entry = YAXIMAX;
		else
			entry = YAXIMIN;

		if (value < cmGraphicEFGCDisplayInfo.min) {
			cmGraphicEFGCDisplayInfo.numberOfUnderflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.min) + ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), entry);
		}
		else if (value > cmGraphicEFGCDisplayInfo.max) {
			cmGraphicEFGCDisplayInfo.numberOfOverflows++;
			cmDisplay->Fill(((double)cmGraphicEFGCDisplayInfo.max) - ((double)(cmGraphicEFGCDisplayInfo.max - cmGraphicEFGCDisplayInfo.min)) / ((double)cmGraphicEFGCDisplayInfo.numberOfBins), entry);
		}
		else
			cmDisplay->Fill(value, entry);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::trackToNoPeak(trackfinderInputTrack& actualTrack) {

	trackMomentum mcMomentum;
	double        value;

	if (isEMDisplayEnabled()) {

		mcMomentum.set(actualTrack.getMomX(), PX);
		mcMomentum.set(actualTrack.getMomY(), PY);
		mcMomentum.set(actualTrack.getMomZ(), PZ);

		value = getEntryOfXAxi(mcMomentum);

		if (value < emGraphicEFGCDisplayInfo.min) {
			emGraphicEFGCDisplayInfo.numberOfUnderflows++;
			emDisplay->Fill(((double)emGraphicEFGCDisplayInfo.min) + ((double)(emGraphicEFGCDisplayInfo.max - emGraphicEFGCDisplayInfo.min)) / ((double)emGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else if (value > emGraphicEFGCDisplayInfo.max) {
			emGraphicEFGCDisplayInfo.numberOfOverflows++;
			emDisplay->Fill(((double)emGraphicEFGCDisplayInfo.max) - ((double)(emGraphicEFGCDisplayInfo.max - emGraphicEFGCDisplayInfo.min)) / ((double)emGraphicEFGCDisplayInfo.numberOfBins), YAXIMIN);
		}
		else
			emDisplay->Fill(value, YAXIMIN);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::trackToOnePeak(trackfinderInputTrack& actualTrack) {

	trackMomentum mcMomentum;
	double        value;

	if (isEMDisplayEnabled()) {

		mcMomentum.set(actualTrack.getMomX(), PX);
		mcMomentum.set(actualTrack.getMomY(), PY);
		mcMomentum.set(actualTrack.getMomZ(), PZ);

		value = getEntryOfXAxi(mcMomentum);

		if (value < emGraphicEFGCDisplayInfo.min) {
			emGraphicEFGCDisplayInfo.numberOfUnderflows++;
			emDisplay->Fill(((double)emGraphicEFGCDisplayInfo.min) + ((double)(emGraphicEFGCDisplayInfo.max - emGraphicEFGCDisplayInfo.min)) / ((double)emGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else if (value > emGraphicEFGCDisplayInfo.max) {
			emGraphicEFGCDisplayInfo.numberOfOverflows++;
			emDisplay->Fill(((double)emGraphicEFGCDisplayInfo.max) - ((double)(emGraphicEFGCDisplayInfo.max - emGraphicEFGCDisplayInfo.min)) / ((double)emGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else
			emDisplay->Fill(value, YAXIMAX);

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void momentumEFGCAnalysis::trackToMorePeaks(trackfinderInputTrack& actualTrack) {

	trackMomentum mcMomentum;
	double        value;

	if (isEMDisplayEnabled()) {

		mcMomentum.set(actualTrack.getMomX(), PX);
		mcMomentum.set(actualTrack.getMomY(), PY);
		mcMomentum.set(actualTrack.getMomZ(), PZ);

		value = getEntryOfXAxi(mcMomentum);

		if (value < emGraphicEFGCDisplayInfo.min) {
			emGraphicEFGCDisplayInfo.numberOfUnderflows++;
			emDisplay->Fill(((double)emGraphicEFGCDisplayInfo.min) + ((double)(emGraphicEFGCDisplayInfo.max - emGraphicEFGCDisplayInfo.min)) / ((double)emGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else if (value > emGraphicEFGCDisplayInfo.max) {
			emGraphicEFGCDisplayInfo.numberOfOverflows++;
			emDisplay->Fill(((double)emGraphicEFGCDisplayInfo.max) - ((double)(emGraphicEFGCDisplayInfo.max - emGraphicEFGCDisplayInfo.min)) / ((double)emGraphicEFGCDisplayInfo.numberOfBins), YAXIMAX);
		}
		else
			emDisplay->Fill(value, YAXIMAX);

	}

}

/****************************************************************
 * This method returns the emDisplay's axi range.				*
 ****************************************************************/

momentumEFGCDisplayInfo momentumEFGCAnalysis::getEMDisplayAxiRange() {

	return emGraphicEFGCDisplayInfo;

}

/****************************************************************
 * This method returns the fmDisplay's axi range.				*
 ****************************************************************/

momentumEFGCDisplayInfo momentumEFGCAnalysis::getFMDisplayAxiRange() {

	return fmGraphicEFGCDisplayInfo;

}

/****************************************************************
 * This method returns the gmDisplay's axi range.				*
 ****************************************************************/

momentumEFGCDisplayInfo momentumEFGCAnalysis::getGMDisplayAxiRange() {

	return gmGraphicEFGCDisplayInfo;

}

/****************************************************************
 * This method returns the cmDisplay's axi range.				*
 ****************************************************************/

momentumEFGCDisplayInfo momentumEFGCAnalysis::getCMDisplayAxiRange() {

	return cmGraphicEFGCDisplayInfo;

}

/****************************************************************
 * This method returns the number of emDisplay entries.			*
 ****************************************************************/

unsigned int momentumEFGCAnalysis::getEMDisplayEntries() {

	unsigned int returnValue;

	if (isEMDisplayEnabled())
		returnValue = (unsigned int)emDisplay->GetEntries();
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
 * This method returns the number of fmDisplay entries.			*
 ****************************************************************/

unsigned int momentumEFGCAnalysis::getFMDisplayEntries() {

	unsigned int returnValue;

	if (isFMDisplayEnabled())
		returnValue = (unsigned int)fmDisplay->GetEntries();
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
 * This method returns the number of gmDisplay entries.			*
 ****************************************************************/

unsigned int momentumEFGCAnalysis::getGMDisplayEntries() {

	unsigned int returnValue;

	if (isGMDisplayEnabled())
		returnValue = (unsigned int)gmDisplay->GetEntries();
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
 * This method returns the number of cmDisplay entries.			*
 ****************************************************************/

unsigned int momentumEFGCAnalysis::getCMDisplayEntries() {

	unsigned int returnValue;

	if (isCMDisplayEnabled())
		returnValue = (unsigned int)cmDisplay->GetEntries();
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
