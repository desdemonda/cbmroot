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
// $Revision: 1.7 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisWarningMsg.h"
#include "../include/magnetfieldAnalysisDef.h"
#include "../include/magnetfieldDirectionAnalysis.h"
#include "TStyle.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"


/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setAutomaticAxisRangeOfDisplay(TGraph* display) {

	double* xValues;
	double* yValues;
	double  xMin;
	double  xMax;
	double  yMin;
	double  yMax;

	if (display != NULL) {

		xValues = display->GetX();
		yValues = display->GetY();

		xMin = xValues[0];
		xMax = xValues[0];
		yMin = yValues[0];
		yMax = yValues[0];

		for (int j = 1; j < display->GetN(); j++) {

			if (xValues[j] < xMin)
				xMin = xValues[j];
			if (xValues[j] > xMax)
				xMax = xValues[j];
			if (yValues[j] < yMin)
				yMin = yValues[j];
			if (yValues[j] > yMax)
				yMax = yValues[j];

		}

		/*
		 * SetLimits sets the minimal and maximal values for the axis.
		 * Smaller values are in the underflow bin and higher values
		 * in the overflow bin.
		 *
		 * SetRangeUser sets the zoom of the axis. Here the zoom is
		 * set to one.
		 */
		display->GetXaxis()->SetLimits(0.95 * xMin, 1.05 * xMax);
		display->GetXaxis()->SetRangeUser(0.95 * xMin, 1.05 * xMax);
		display->GetYaxis()->SetLimits(0.95 * yMin, 1.05 * yMax);
		display->GetYaxis()->SetRangeUser(0.95 * yMin, 1.05 * yMax);

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
 * resets the profiles											*
 ****************************************************************/

void magnetfieldDirectionAnalysis::reset() {

	int i;

	if (isBxDisplayEnabled()) {
		for (i = 0; i < bxDisplay->GetN(); i++)
			bxDisplay->SetPoint(i, 0, 0);
	}
	bxMagnetfieldDisplayInfo.numberOfUnderflows = 0;
	bxMagnetfieldDisplayInfo.numberOfOverflows  = 0;

	if (isByDisplayEnabled()) {
		for (i = 0; i < byDisplay->GetN(); i++)
			byDisplay->SetPoint(i, 0, 0);
	}

	byMagnetfieldDisplayInfo.numberOfUnderflows = 0;
	byMagnetfieldDisplayInfo.numberOfOverflows  = 0;

	if (isBzDisplayEnabled()) {
		for (i = 0; i < bzDisplay->GetN(); i++)
			bzDisplay->SetPoint(i, 0, 0);
	}

	bzMagnetfieldDisplayInfo.numberOfUnderflows = 0;
	bzMagnetfieldDisplayInfo.numberOfOverflows  = 0;

}

/****************************************************************
 * This method writes a display into the file.					*
 ****************************************************************/

void magnetfieldDirectionAnalysis::writeDisplay(TGraph* display, magnetfieldDisplayInfo* info, std::string outputFileName) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;
	char        buffer[doubleConversion+1];
	std::string modifiedDisplayName;
	std::string modifiedDisplayTitle;

	file              = NULL;
	previousDirectory = NULL;
	specialDirectory  = NULL;

	if (display != NULL) {

		if (writeToFile) {

			previousDirectory = gDirectory;
			specialDirectory  = MAGNETFIELDSAVINGDIRECTORY;

			if (!outputFileName.empty()) {

				file = new TFile(outputFileName.c_str(), "UPDATE");
		
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, outputFileName);

			}
			else
				if (gDirectory != NULL)
					gDirectory->cd();

			modifiedDisplayName   = display->GetName();
			modifiedDisplayTitle  = display->GetTitle();
			modifiedDisplayTitle += " (";
			modifiedDisplayTitle += getDim1Modifier();
			modifiedDisplayTitle += "=";
			dtos(info->constValDim1, buffer, doubleConversionDigits);
			modifiedDisplayTitle += buffer;
			modifiedDisplayTitle += ", ";
			modifiedDisplayTitle += getDim2Modifier();
			modifiedDisplayTitle += "=";
			dtos(info->constValDim2, buffer, doubleConversionDigits);
			modifiedDisplayTitle += buffer;
			modifiedDisplayTitle += ")";

			display->SetName(modifiedDisplayName.c_str());
			display->SetTitle(modifiedDisplayTitle.c_str());

			if (!gDirectory->cd(STSMAGNETFIELDANALYSISDIR)) {

				gDirectory->mkdir(STSMAGNETFIELDANALYSISDIR, STSMAGNETFIELDANALYSISDIR);
				gDirectory->cd(STSMAGNETFIELDANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (!info->subDirName.empty())
				gDirectory->cd(info->subDirName.c_str());

			display->Write();
			/* Workaround to show the TGraph with the TBrowser*/
			display->GetHistogram()->Write();

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

magnetfieldDirectionAnalysis::magnetfieldDirectionAnalysis() 
 : bxDisplay(NULL),
   bxMagnetfieldDisplayInfo(),
   byDisplay(NULL),
   byMagnetfieldDisplayInfo(),
   bzDisplay(NULL),
   bzMagnetfieldDisplayInfo(),
   windowPadColumn(0),
   bxWindowPadRow((unsigned short)-1),
   byWindowPadRow((unsigned short)-1),
   bzWindowPadRow((unsigned short)-1),
   writeToFile(false)
{

	bxDisplay                                   = NULL;
	bxMagnetfieldDisplayInfo.constValDim1       = 0;
	bxMagnetfieldDisplayInfo.constValDim2       = 0;
	bxMagnetfieldDisplayInfo.min                = 0;
	bxMagnetfieldDisplayInfo.max                = 0;
	bxMagnetfieldDisplayInfo.numberOfUnderflows = 0;
	bxMagnetfieldDisplayInfo.numberOfOverflows  = 0;
	bxMagnetfieldDisplayInfo.baseName           = "";
	bxMagnetfieldDisplayInfo.baseTitle          = "";
	bxMagnetfieldDisplayInfo.subDirName         = "";
	byDisplay                                   = NULL;
	byMagnetfieldDisplayInfo.constValDim1       = 0;
	byMagnetfieldDisplayInfo.constValDim2       = 0;
	byMagnetfieldDisplayInfo.min                = 0;
	byMagnetfieldDisplayInfo.max                = 0;
	byMagnetfieldDisplayInfo.numberOfUnderflows = 0;
	byMagnetfieldDisplayInfo.numberOfOverflows  = 0;
	byMagnetfieldDisplayInfo.baseName           = "";
	byMagnetfieldDisplayInfo.baseTitle          = "";
	byMagnetfieldDisplayInfo.subDirName         = "";
	bzDisplay                                   = NULL;
	bzMagnetfieldDisplayInfo.constValDim1       = 0;
	bzMagnetfieldDisplayInfo.constValDim2       = 0;
	bzMagnetfieldDisplayInfo.min                = 0;
	bzMagnetfieldDisplayInfo.max                = 0;
	bzMagnetfieldDisplayInfo.numberOfUnderflows = 0;
	bzMagnetfieldDisplayInfo.numberOfOverflows  = 0;
	bzMagnetfieldDisplayInfo.baseName           = "";
	bzMagnetfieldDisplayInfo.baseTitle          = "";
	bzMagnetfieldDisplayInfo.subDirName         = "";
	windowPadColumn                             = 0;
	bxWindowPadRow                              = (unsigned short)-1;
	byWindowPadRow                              = (unsigned short)-1;
	bzWindowPadRow                              = (unsigned short)-1;
	writeToFile                                 = false;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magnetfieldDirectionAnalysis::~magnetfieldDirectionAnalysis() {

	deleteBxDisplay();
	deleteByDisplay();
	deleteBzDisplay();

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::initializeEvaluation() {

	reset();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void magnetfieldDirectionAnalysis::finalizeEvaluation() {

}

/****************************************************************
 * This method initializes the bxDisplay.						*
 ****************************************************************/

void magnetfieldDirectionAnalysis::initBxDisplay(int nBins, double min, double max, double constValDim1, double constValDim2) {

	deleteBxDisplay();

	bxMagnetfieldDisplayInfo.constValDim1 = constValDim1;
	bxMagnetfieldDisplayInfo.constValDim2 = constValDim2;
	bxMagnetfieldDisplayInfo.min          = min;
	bxMagnetfieldDisplayInfo.max          = max;
	bxMagnetfieldDisplayInfo.baseName     = getNameOfBxDisplay();
	bxMagnetfieldDisplayInfo.baseTitle    = getTitleOfBxDisplay();

	bxDisplay = new TGraph(nBins);

}

/****************************************************************
 * This method initializes the byDisplay.						*
 ****************************************************************/

void magnetfieldDirectionAnalysis::initByDisplay(int nBins, double min, double max, double constValDim1, double constValDim2) {

	deleteByDisplay();

	byMagnetfieldDisplayInfo.constValDim1 = constValDim1;
	byMagnetfieldDisplayInfo.constValDim2 = constValDim2;
	byMagnetfieldDisplayInfo.min          = min;
	byMagnetfieldDisplayInfo.max          = max;
	byMagnetfieldDisplayInfo.baseName     = getNameOfByDisplay();
	byMagnetfieldDisplayInfo.baseTitle    = getTitleOfByDisplay();

	byDisplay = new TGraph(nBins);

}

/****************************************************************
 * This method initializes the bzDisplay.						*
 ****************************************************************/

void magnetfieldDirectionAnalysis::initBzDisplay(int nBins, double min, double max, double constValDim1, double constValDim2) {

	deleteBzDisplay();

	bzMagnetfieldDisplayInfo.constValDim1 = constValDim1;
	bzMagnetfieldDisplayInfo.constValDim2 = constValDim2;
	bzMagnetfieldDisplayInfo.min          = min;
	bzMagnetfieldDisplayInfo.max          = max;
	bzMagnetfieldDisplayInfo.baseName     = getNameOfBzDisplay();
	bzMagnetfieldDisplayInfo.baseTitle    = getTitleOfBzDisplay();

	bzDisplay = new TGraph(nBins);

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void magnetfieldDirectionAnalysis::initMagnetfieldAnalysisToRoot(bool enable, const char* name) {

	TFile*      file;
	TDirectory* previousDirectory;
	TDirectory* preSubDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = MAGNETFIELDSAVINGDIRECTORY;
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

			if ((!gDirectory->cd(STSMAGNETFIELDANALYSISDIR)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(STSMAGNETFIELDANALYSISDIR, STSMAGNETFIELDANALYSISDIR);
				gDirectory->cd(STSMAGNETFIELDANALYSISDIR);

			}

			if ((!gDirectory->cd(specialDirectory)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (getSavingInSubDirectory()) {

				preSubDirectory = gDirectory;

				if (isBxDisplayEnabled() || isByDisplayEnabled() || isBzDisplayEnabled()) {
					if (!gDirectory->cd(getNameOfSavingSubDirectory()))
						gDirectory->mkdir(getNameOfSavingSubDirectory(), getNameOfSavingSubDirectory());
				}
				if (isBxDisplayEnabled())
					bxMagnetfieldDisplayInfo.subDirName = getNameOfSavingSubDirectory();
				if (isByDisplayEnabled())
					byMagnetfieldDisplayInfo.subDirName = getNameOfSavingSubDirectory();
				if (isBzDisplayEnabled())
					bzMagnetfieldDisplayInfo.subDirName = getNameOfSavingSubDirectory();

			}

		}
		else {

			bxMagnetfieldDisplayInfo.subDirName.clear();
			byMagnetfieldDisplayInfo.subDirName.clear();
			bzMagnetfieldDisplayInfo.subDirName.clear();

			if (gDirectory->cd(STSMAGNETFIELDANALYSISDIR)) {

				preSubDirectory = gDirectory;
				if (gDirectory->cd(specialDirectory))
					gDirectory->Delete(getNameOfSavingSubDirectory());

				gDirectory = preSubDirectory;
				gDirectory->Delete(specialDirectory);

			}

			gDirectory = previousDirectory;
			gDirectory->Delete(STSMAGNETFIELDANALYSISDIR);

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
 * This method deletes the bxDisplay.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::deleteBxDisplay() {

	if (isBxDisplayEnabled()) {
		delete bxDisplay;
		bxDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the byDisplay.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::deleteByDisplay() {

	if (isByDisplayEnabled()) {
		delete byDisplay;
		byDisplay = NULL;
	}

}

/****************************************************************
 * This method deletes the bzDisplay.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::deleteBzDisplay() {

	if (isBzDisplayEnabled()) {
		delete bzDisplay;
		bzDisplay = NULL;
	}

}

/****************************************************************
 * This method sets the values for x and y at the given point	*
 * at position index in the bxDisplay.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setBxPoint(int index, double xValue, double yValue) {

	if (isBxDisplayEnabled() && (index >= 0) && (index < bxDisplay->GetN())) {

		bxDisplay->SetPoint(index, xValue, yValue);

	}
	else {

		cannotSetPointWarningMsg* cannotSetPoint = new cannotSetPointWarningMsg(index);
		cannotSetPoint->warningMsg();
		if(cannotSetPoint != NULL) {
			delete cannotSetPoint;
			cannotSetPoint = NULL;
		}

	}

}

/****************************************************************
 * This method sets the values for x and y at the given point	*
 * at position index in the byDisplay.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setByPoint(int index, double xValue, double yValue) {

	if (isByDisplayEnabled() && (index >= 0) && (index < byDisplay->GetN())) {

		byDisplay->SetPoint(index, xValue, yValue);

	}
	else {

		cannotSetPointWarningMsg* cannotSetPoint = new cannotSetPointWarningMsg(index);
		cannotSetPoint->warningMsg();
		if(cannotSetPoint != NULL) {
			delete cannotSetPoint;
			cannotSetPoint = NULL;
		}

	}

}

/****************************************************************
 * This method sets the values for x and y at the given point	*
 * at position index in the bzDisplay.							*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setBzPoint(int index, double xValue, double yValue) {

	if (isBzDisplayEnabled() && (index >= 0) && (index < bzDisplay->GetN())) {

		bzDisplay->SetPoint(index, xValue, yValue);

	}
	else {

		cannotSetPointWarningMsg* cannotSetPoint = new cannotSetPointWarningMsg(index);
		cannotSetPoint->warningMsg();
		if(cannotSetPoint != NULL) {
			delete cannotSetPoint;
			cannotSetPoint = NULL;
		}

	}

}

/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setAutomaticBxAxisRange() {

	if (bxDisplay != NULL)
		setAutomaticAxisRangeOfDisplay(bxDisplay);

}

/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setAutomaticByAxisRange() {

	if (byDisplay != NULL)
		setAutomaticAxisRangeOfDisplay(byDisplay);

}

/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setAutomaticBzAxisRange() {

	if (bzDisplay != NULL)
		setAutomaticAxisRangeOfDisplay(bzDisplay);

}

/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setAutomaticAxisRange() {

	setAutomaticBxAxisRange();
	setAutomaticByAxisRange();
	setAutomaticBzAxisRange();

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void magnetfieldDirectionAnalysis::drawBxDisplay() {

	if (isBxDisplayEnabled()) {

		bxDisplay->SetName(bxMagnetfieldDisplayInfo.baseName.c_str());
		bxDisplay->SetTitle(bxMagnetfieldDisplayInfo.baseTitle.c_str());
		bxDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
		bxDisplay->GetYaxis()->SetTitle(BXYAXITITLE);
		bxDisplay->Draw("AC*");		/* draws without errors */

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

void magnetfieldDirectionAnalysis::drawByDisplay() {

	if (isByDisplayEnabled()) {

		byDisplay->SetName(byMagnetfieldDisplayInfo.baseName.c_str());
		byDisplay->SetTitle(byMagnetfieldDisplayInfo.baseTitle.c_str());
		byDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
		byDisplay->GetYaxis()->SetTitle(BYYAXITITLE);
		byDisplay->Draw("AC*");		/* draws without errors */

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

void magnetfieldDirectionAnalysis::drawBzDisplay() {

	if (isBzDisplayEnabled()) {

		bzDisplay->SetName(bzMagnetfieldDisplayInfo.baseName.c_str());
		bzDisplay->SetTitle(bzMagnetfieldDisplayInfo.baseTitle.c_str());
		bzDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
		bzDisplay->GetYaxis()->SetTitle(BZYAXITITLE);
		bzDisplay->Draw("AC*");		/* draws without errors */

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

void magnetfieldDirectionAnalysis::drawDisplay(unsigned short drawId) {

	if (drawId == bxWindowPadRow)
		drawBxDisplay();
	if (drawId == byWindowPadRow)
		drawByDisplay();
	if (drawId == bzWindowPadRow)
		drawBzDisplay();

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void magnetfieldDirectionAnalysis::writeBxDisplay(std::string outputFileName) {

	if (isBxDisplayEnabled()) {
		bxDisplay->SetName(bxMagnetfieldDisplayInfo.baseName.c_str());
		bxDisplay->SetTitle(bxMagnetfieldDisplayInfo.baseTitle.c_str());
		bxDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
		bxDisplay->GetYaxis()->SetTitle(BXYAXITITLE);
		writeDisplay(bxDisplay, &bxMagnetfieldDisplayInfo, outputFileName);
	}
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

void magnetfieldDirectionAnalysis::writeByDisplay(std::string outputFileName) {

	if (isByDisplayEnabled()) {
		byDisplay->SetName(byMagnetfieldDisplayInfo.baseName.c_str());
		byDisplay->SetTitle(byMagnetfieldDisplayInfo.baseTitle.c_str());
		byDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
		byDisplay->GetYaxis()->SetTitle(BYYAXITITLE);
		writeDisplay(byDisplay, &byMagnetfieldDisplayInfo, outputFileName);
	}
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

void magnetfieldDirectionAnalysis::writeBzDisplay(std::string outputFileName) {

	if (isBzDisplayEnabled()) {
		bzDisplay->SetName(bzMagnetfieldDisplayInfo.baseName.c_str());
		bzDisplay->SetTitle(bzMagnetfieldDisplayInfo.baseTitle.c_str());
		bzDisplay->GetXaxis()->SetTitle(getTitleOfXAxi());
		bzDisplay->GetYaxis()->SetTitle(BZYAXITITLE);
		writeDisplay(bzDisplay, &bzMagnetfieldDisplayInfo, outputFileName);
	}
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
 * This method returns true, if the bxDisplay is enabled.		*
 ****************************************************************/

bool magnetfieldDirectionAnalysis::isBxDisplayEnabled() {

	bool returnValue;

	if (bxDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the byDisplay is enabled.		*
 ****************************************************************/

bool magnetfieldDirectionAnalysis::isByDisplayEnabled() {

	bool returnValue;

	if (byDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if the bzDisplay is enabled.		*
 ****************************************************************/

bool magnetfieldDirectionAnalysis::isBzDisplayEnabled() {

	bool returnValue;

	if (bzDisplay != NULL)
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * enabled.														*
 ****************************************************************/

bool magnetfieldDirectionAnalysis::isDisplayEnabled() {

	return (isBxDisplayEnabled() || isByDisplayEnabled() || isBzDisplayEnabled());

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * written to file.												*
 ****************************************************************/

bool magnetfieldDirectionAnalysis::isWritingEnabled() {

	return (writeToFile & isDisplayEnabled());

}

/****************************************************************
 * This method returns the number of all displays.				*
 ****************************************************************/

unsigned short magnetfieldDirectionAnalysis::getNumberOfDisplays() {

	return NUMBEROFMAGNETFIELDDISPLAYS;

}

/****************************************************************
 * This method returns the signature of the enabled displays.	*
 ****************************************************************/

void magnetfieldDirectionAnalysis::getSignatureOfEnabledDisplays(bool* signature) {

	if (signature != NULL) {

		signature[0] = isBxDisplayEnabled();
		signature[1] = isByDisplayEnabled();
		signature[2] = isBzDisplayEnabled();

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

unsigned short magnetfieldDirectionAnalysis::getWindowPadColumn() {

	return windowPadColumn;

}

/****************************************************************
 * sets the windowPadColumn										*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setWindowPadColumn(unsigned short column) {

	windowPadColumn = column;

}

/****************************************************************
 * sets the windowPadRow										*
 ****************************************************************/

void magnetfieldDirectionAnalysis::setWindowPadRow(bool* globalSignature, unsigned short sizeOfGlobalSignature) {

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
		bxWindowPadRow = counter;
		counter++;
	}
	if (globalSignature[1]) {
		byWindowPadRow = counter;
		counter++;
	}
	if (globalSignature[2]) {
		bzWindowPadRow = counter;
		counter++;
	}

}

/****************************************************************
 * This method returns the bxDisplay's axi range.				*
 ****************************************************************/

magnetfieldDisplayInfo& magnetfieldDirectionAnalysis::getBxDisplayAxiRange() {

	return bxMagnetfieldDisplayInfo;

}

/****************************************************************
 * This method returns the byDisplay's axi range.				*
 ****************************************************************/

magnetfieldDisplayInfo& magnetfieldDirectionAnalysis::getByDisplayAxiRange() {

	return byMagnetfieldDisplayInfo;

}

/****************************************************************
 * This method returns the bzDisplay's axi range.				*
 ****************************************************************/

magnetfieldDisplayInfo& magnetfieldDirectionAnalysis::getBzDisplayAxiRange() {

	return bzMagnetfieldDisplayInfo;

}

/****************************************************************
 * This method returns the number of bxDisplay entries.			*
 ****************************************************************/

unsigned int magnetfieldDirectionAnalysis::getBxDisplayEntries() {

	unsigned int returnValue;

	if (isBxDisplayEnabled())
		returnValue = (unsigned int)bxDisplay->GetN();
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
 * This method returns the number of byDisplay entries.			*
 ****************************************************************/

unsigned int magnetfieldDirectionAnalysis::getByDisplayEntries() {

	unsigned int returnValue;

	if (isByDisplayEnabled())
		returnValue = (unsigned int)byDisplay->GetN();
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
 * This method returns the number of bzDisplay entries.			*
 ****************************************************************/

unsigned int magnetfieldDirectionAnalysis::getBzDisplayEntries() {

	unsigned int returnValue;

	if (isBzDisplayEnabled())
		returnValue = (unsigned int)bzDisplay->GetN();
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
