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
//     - implements methods to display the track propagation's quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2009-01-13 11:48:12 $
// $Revision: 1.0 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisWarningMsg.h"
#include "../include/trackPropagationAnalysisDef.h"
#include "../include/trackPropagationDAnalysis.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"


/****************************************************************
 * resets the displays											*
 ****************************************************************/

void trackPropagationDAnalysis::resetDisplay(unsigned short index) {

	if (index < getNumberOfDisplays()) {

		if (isDistanceDisplayEnabled(index)) {

			distanceDisplaysInfo[index]->numberOfUnderflows = 0;
			distanceDisplaysInfo[index]->numberOfOverflows  = 0;
			distanceDisplays[index]->Reset();

		}

	}

}
void trackPropagationDAnalysis::resetDisplays() {

	for (unsigned short i = 0; i < getNumberOfDisplays(); i++) {

		resetDisplay(i);

	}

}

/****************************************************************
 * This method writes a display into the file.					*
 ****************************************************************/

void trackPropagationDAnalysis::writeDisplay(TH1D* display, trackPropagationDisplayInfo* info, int eventNumber, std::string outputFileName) {

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
			modifiedDisplayName += TRACKPROPAGATIONEVENTMODIFIER;
			modifiedDisplayName += "_";
			modifiedDisplayName += buffer;

			modifiedDisplayTitle  = display->GetTitle();
			modifiedDisplayTitle += " ";
			modifiedDisplayTitle += TRACKPROPAGATIONEVENTMODIFIER;
			modifiedDisplayTitle += ":";
			modifiedDisplayTitle += buffer;

			display->SetName(modifiedDisplayName.c_str());
			display->SetTitle(modifiedDisplayTitle.c_str());

			if (!gDirectory->cd(TRACKPROPAGATIONSTSANALYSISDIR)) {

				gDirectory->mkdir(TRACKPROPAGATIONSTSANALYSISDIR, TRACKPROPAGATIONSTSANALYSISDIR);
				gDirectory->cd(TRACKPROPAGATIONSTSANALYSISDIR);

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

trackPropagationDAnalysis::trackPropagationDAnalysis() 
  : numberOfDisplays(0),
    distanceDisplaysInfo(NULL),
    distanceDisplays(NULL),
    windowPadColumn(0),
    writeToFile(false)

{
  /*
	numberOfDisplays                       = 0;
	distanceDisplaysInfo                   = NULL;
	distanceDisplays                       = NULL;
	windowPadColumn                        = 0;
	writeToFile                            = false;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPropagationDAnalysis::~trackPropagationDAnalysis() {

	deleteDistanceDisplays();

}

/****************************************************************
 * method initializes the evaluation.							*
 ****************************************************************/

void trackPropagationDAnalysis::initializeEvaluation() {

	reset();

}

/****************************************************************
 * method finalizes the evaluation.								*
 ****************************************************************/

void trackPropagationDAnalysis::finalizeEvaluation() {

}

/****************************************************************
 * This method initializes the distanceDisplay.					*
 ****************************************************************/

void trackPropagationDAnalysis::initDistanceDisplay(unsigned short index, int nBins, int xMin, int xMax) {

	unsigned short actualDisplayIndex;
	bool           allocateNewDisplay;

	if (index < getNumberOfDisplays()) {

		actualDisplayIndex = index;

		if (isDistanceDisplayEnabled(actualDisplayIndex))
			deleteDistanceDisplay(actualDisplayIndex);

		if (getNumberOfDisplays() == 0) {
			numberOfDisplays++;
			allocateNewDisplay = true;
		}
		else
			allocateNewDisplay = false;

	}
	else {

		actualDisplayIndex = getNumberOfDisplays();
		numberOfDisplays++;
		allocateNewDisplay = true;

	}

	if (allocateNewDisplay) {

		if (distanceDisplaysInfo == NULL)
			distanceDisplaysInfo = (trackPropagationDisplayInfo**)calloc(numberOfDisplays, sizeof(trackPropagationDisplayInfo*));
		else
			distanceDisplaysInfo = (trackPropagationDisplayInfo**)realloc(distanceDisplaysInfo, numberOfDisplays * sizeof(trackPropagationDisplayInfo*));
		if (distanceDisplays == NULL)
			distanceDisplays = (TH1D**)calloc(numberOfDisplays, sizeof(TH1D*));
		else
			distanceDisplays = (TH1D**)realloc(distanceDisplays, numberOfDisplays * sizeof(TH1D*));

	}

	distanceDisplaysInfo[actualDisplayIndex] = new trackPropagationDisplayInfo();

	distanceDisplaysInfo[actualDisplayIndex]->min                = xMin;
	distanceDisplaysInfo[actualDisplayIndex]->max                = xMax;
	distanceDisplaysInfo[actualDisplayIndex]->numberOfBins       = nBins;
	distanceDisplaysInfo[actualDisplayIndex]->numberOfUnderflows = 0;
	distanceDisplaysInfo[actualDisplayIndex]->numberOfOverflows  = 0;
	distanceDisplaysInfo[actualDisplayIndex]->baseName           = getNameOfDistanceDisplay(actualDisplayIndex);
	distanceDisplaysInfo[actualDisplayIndex]->baseTitle          = getTitleOfDistanceDisplay(actualDisplayIndex);
	distanceDisplaysInfo[actualDisplayIndex]->subDirName         = "";
	distanceDisplaysInfo[actualDisplayIndex]->windowPadRow       = (unsigned short)-1;

	distanceDisplays[actualDisplayIndex]     = new TH1D(distanceDisplaysInfo[actualDisplayIndex]->baseName.c_str(), distanceDisplaysInfo[actualDisplayIndex]->baseTitle.c_str(), distanceDisplaysInfo[actualDisplayIndex]->numberOfBins, distanceDisplaysInfo[actualDisplayIndex]->min, distanceDisplaysInfo[actualDisplayIndex]->max);

	if (distanceDisplays[actualDisplayIndex] == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	distanceDisplays[actualDisplayIndex]->SetDirectory(0);
	distanceDisplays[actualDisplayIndex]->GetXaxis()->SetTitle(DISTANCEXAXITITLE);
	distanceDisplays[actualDisplayIndex]->GetYaxis()->SetTitle(DISTANCEYAXITITLE);

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void trackPropagationDAnalysis::initTrackPropagationAnalysisToRoot(bool enable, const char* name) {

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

			if ((!gDirectory->cd(TRACKPROPAGATIONSTSANALYSISDIR)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(TRACKPROPAGATIONSTSANALYSISDIR, TRACKPROPAGATIONSTSANALYSISDIR);
				gDirectory->cd(TRACKPROPAGATIONSTSANALYSISDIR);

			}

			if ((!gDirectory->cd(specialDirectory)) && ((isDisplayEnabled()))) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (getSavingInSubDirectory()) {

				for (unsigned short i = 0; i < getNumberOfDisplays(); i++) {

					preSubDirectory = gDirectory;
					if (isDistanceDisplayEnabled(i)) {

						if (!gDirectory->cd(DISTANCESAVINGSUBDIRECTORY))
							gDirectory->mkdir(DISTANCESAVINGSUBDIRECTORY, DISTANCESAVINGSUBDIRECTORY);
						distanceDisplaysInfo[i]->subDirName = DISTANCESAVINGSUBDIRECTORY;

					}

				}

			}

		}
		else {

			for (unsigned short i = 0; i < getNumberOfDisplays(); i++)
				distanceDisplaysInfo[i]->subDirName.clear();

			if (gDirectory->cd(TRACKPROPAGATIONSTSANALYSISDIR)) {

				preSubDirectory = gDirectory;
				if (gDirectory->cd(specialDirectory)) {

					gDirectory->Delete(DISTANCESAVINGSUBDIRECTORY);
	
				}

				gDirectory = preSubDirectory;
				gDirectory->Delete(specialDirectory);

			}

			gDirectory = previousDirectory;
			gDirectory->Delete(TRACKPROPAGATIONSTSANALYSISDIR);

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
 * This method deletes the distanceDisplays.					*
 ****************************************************************/

void trackPropagationDAnalysis::deleteDistanceDisplay(unsigned short index) {

	unsigned short actualNumberOfDisplays;

	if (isDistanceDisplayEnabled(index)) {

		if (distanceDisplaysInfo[index] != NULL) {
			delete distanceDisplaysInfo[index];
			distanceDisplaysInfo[index] = NULL;
		}
		if (distanceDisplays[index] != NULL) {
			delete distanceDisplays[index];
			distanceDisplays[index]     = NULL;
		}
	
	}

	actualNumberOfDisplays = getNumberOfDisplays();

	for (unsigned short i = actualNumberOfDisplays; i > 0 ; i--) {

		if (!isDistanceDisplayEnabled(i - 1)) {

			numberOfDisplays--;

		}

	}

	if (actualNumberOfDisplays != getNumberOfDisplays()) {

		if (getNumberOfDisplays() == 0) {

			if (distanceDisplaysInfo != NULL) {
				free(distanceDisplaysInfo);
				distanceDisplaysInfo = NULL;
			}
			if (distanceDisplays != NULL) {
				free(distanceDisplays);
				distanceDisplays = NULL;
			}

		}
		else {

			distanceDisplaysInfo = (trackPropagationDisplayInfo**)realloc(distanceDisplaysInfo, getNumberOfDisplays() * sizeof(trackPropagationDisplayInfo*));
			distanceDisplays     = (TH1D**)realloc(distanceDisplays, getNumberOfDisplays() * sizeof(TH1D*));

		}

	}

}
void trackPropagationDAnalysis::deleteDistanceDisplays() {

	for (unsigned short i = 0; i < getNumberOfDisplays(); i++)
		deleteDistanceDisplay(i);

	if (distanceDisplaysInfo != NULL) {
		free(distanceDisplaysInfo);
		distanceDisplaysInfo = NULL;
	}

	if (distanceDisplays != NULL) {
		free(distanceDisplays);
		distanceDisplays = NULL;
	}

}

/****************************************************************
 * This method paints the display.								*
 ****************************************************************/

void trackPropagationDAnalysis::drawDistanceDisplay(unsigned short index, unsigned int option) {

	if (index < getNumberOfDisplays()) {

		if (isDistanceDisplayEnabled(index)) {

			switch (option) {

				case 0:
					distanceDisplays[index]->Draw("C");				/* draws a smoothed curve */
					break;
				case 1:
					distanceDisplays[index]->Draw("LF2");			/* draws a simple line between each point with a filled area below */
					break;
				default:
					distanceDisplays[index]->Draw("L");				/* draws a simple line between each point */
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

}

/****************************************************************
 * This method draws the display corresponding to the drawId.	*
 ****************************************************************/

void trackPropagationDAnalysis::drawDisplay(unsigned short drawId, unsigned int option) {

	for (unsigned short i = 0; i < getNumberOfDisplays(); i++) {

		if (isDistanceDisplayEnabled(i))
			if (drawId == distanceDisplaysInfo[i]->windowPadRow)
				drawDistanceDisplay(i, option);

	}

}

/****************************************************************
 * This method writes the data into a file.						*
 ****************************************************************/

void trackPropagationDAnalysis::writeDistanceDisplay(unsigned short index, int eventNumber, std::string outputFileName) {

	if (index < getNumberOfDisplays()) {

		if (isDistanceDisplayEnabled(index))
			writeDisplay(distanceDisplays[index], distanceDisplaysInfo[index], eventNumber, outputFileName);
		else {

			cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
			cannotWrite->warningMsg();
			if(cannotWrite != NULL) {
				delete cannotWrite;
				cannotWrite = NULL;
			}

		}

	}

}
void trackPropagationDAnalysis::writeDistanceDisplays(int eventNumber, std::string outputFileName) {

	for (unsigned short i = 0; i < getNumberOfDisplays(); i++)
		writeDistanceDisplay(i, eventNumber, outputFileName);

}

/****************************************************************
 * This method returns true, if the emDisplay is enabled.		*
 ****************************************************************/

bool trackPropagationDAnalysis::isDistanceDisplayEnabled(unsigned short index) {

	bool returnValue;

	if (index < getNumberOfDisplays()) {

		if ((distanceDisplays[index] != NULL) && (distanceDisplaysInfo[index] != NULL))
			returnValue = true;
		else
			returnValue = false;

	}
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * enabled.														*
 ****************************************************************/

bool trackPropagationDAnalysis::isDisplayEnabled() {

	bool returnValue;

	returnValue = false;

	for (unsigned short i = 0; i < getNumberOfDisplays(); i++)
		returnValue |= isDistanceDisplayEnabled(i);

	return returnValue;

}

/****************************************************************
 * This method returns true, if at least one display is			*
 * written to file.												*
 ****************************************************************/

bool trackPropagationDAnalysis::isWritingEnabled() {

	return (writeToFile & isDisplayEnabled());

}

/****************************************************************
 * This method returns the number of all displays.				*
 ****************************************************************/

unsigned short trackPropagationDAnalysis::getNumberOfDisplays() {

	return numberOfDisplays;

}

/****************************************************************
 * This method returns the signature of the enabled displays.	*
 ****************************************************************/

void trackPropagationDAnalysis::getSignatureOfEnabledDisplays(bool* signature) {

	if (signature != NULL) {

		for (unsigned short i = 0; i < getNumberOfDisplays(); i++)
			signature[i] = isDistanceDisplayEnabled(i);

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

unsigned short trackPropagationDAnalysis::getWindowPadColumn() {

	return windowPadColumn;

}

/****************************************************************
 * sets the windowPadColumn										*
 ****************************************************************/

void trackPropagationDAnalysis::setWindowPadColumn(unsigned short column) {

	windowPadColumn = column;

}

/****************************************************************
 * sets the windowPadRow										*
 ****************************************************************/

void trackPropagationDAnalysis::setWindowPadRow(bool* localSignature, unsigned short sizeOfLocalSignature, bool* globalSignature, unsigned short sizeOfGlobalSignature) {

	unsigned short windowPadCounter;
	unsigned short counterMax;

	if (getNumberOfDisplays() > sizeOfGlobalSignature) {

		notAllDisplaysCanBeDrawnWarningMsg* notAllDisplaysCanBeDrawn = new notAllDisplaysCanBeDrawnWarningMsg();
		notAllDisplaysCanBeDrawn->warningMsg();
		if(notAllDisplaysCanBeDrawn != NULL) {
			delete notAllDisplaysCanBeDrawn;
			notAllDisplaysCanBeDrawn = NULL;
		}

		if (sizeOfGlobalSignature > sizeOfLocalSignature)
			counterMax = sizeOfLocalSignature;
		else
			counterMax = sizeOfGlobalSignature;

	}
	else
		counterMax = getNumberOfDisplays();

	windowPadCounter = 0;
	for (unsigned short i = 0; i < counterMax; i++) {

		if (globalSignature[i]) {

			if (isDistanceDisplayEnabled(i) && (localSignature[i]))
				distanceDisplaysInfo[i]->windowPadRow = windowPadCounter;

			windowPadCounter++;

		}

	}

}

/****************************************************************
 * Function wich is called from evaluate in a special			*
 * switch case.													*
 ****************************************************************/

void trackPropagationDAnalysis::addDistance(unsigned short index, double value) {

	if (index < getNumberOfDisplays()) {

		if (isDistanceDisplayEnabled(index)) {

			if (value < distanceDisplaysInfo[index]->min) {
				distanceDisplaysInfo[index]->numberOfUnderflows++;
				distanceDisplays[index]->Fill(((double)distanceDisplaysInfo[index]->min) + ((double)(distanceDisplaysInfo[index]->max - distanceDisplaysInfo[index]->min)) / ((double)distanceDisplaysInfo[index]->numberOfBins), 1);
			}
			else if (value > distanceDisplaysInfo[index]->max) {
				distanceDisplaysInfo[index]->numberOfOverflows++;
				distanceDisplays[index]->Fill(((double)distanceDisplaysInfo[index]->max) - ((double)(distanceDisplaysInfo[index]->max - distanceDisplaysInfo[index]->min)) / ((double)distanceDisplaysInfo[index]->numberOfBins), 1);
			}
			else
				distanceDisplays[index]->Fill(value, 1);

		}

	}

}

/****************************************************************
 * This method returns the distanceDisplay's axi range.			*
 ****************************************************************/

trackPropagationDisplayInfo trackPropagationDAnalysis::getDistanceDisplayAxiRange(unsigned short index) {

	trackPropagationDisplayInfo returnValue;

	returnValue.min                = 0;
	returnValue.max                = 0;
	returnValue.numberOfBins       = 0;
	returnValue.numberOfUnderflows = 0;
	returnValue.numberOfOverflows  = 0;
	returnValue.baseName           = "";
	returnValue.baseTitle          = "";
	returnValue.subDirName         = "";
	returnValue.windowPadRow       = (unsigned short)-1;

	if (index < getNumberOfDisplays())
		if (isDistanceDisplayEnabled(index))
			returnValue = *distanceDisplaysInfo[index];
	
	return returnValue;;

}

/****************************************************************
 * This method returns the number of distanceDisplay entries.	*
 ****************************************************************/

unsigned int trackPropagationDAnalysis::getDistanceDisplayEntries(unsigned short index) {

	unsigned int returnValue;

	returnValue = 1; /* set to 1 because of the usage for a division, to avoid x/0. */

	if (index < getNumberOfDisplays()) {

		if (isDistanceDisplayEnabled(index))
			returnValue = (unsigned int)distanceDisplays[index]->GetEntries();
		else {

			cannotGetDisplayEntriesWarningMsg* cannotGetDisplayEntries = new cannotGetDisplayEntriesWarningMsg();
			cannotGetDisplayEntries->warningMsg();
			if(cannotGetDisplayEntries != NULL) {
				delete cannotGetDisplayEntries;
				cannotGetDisplayEntries = NULL;
			}

		}

	}

	return returnValue;

}
