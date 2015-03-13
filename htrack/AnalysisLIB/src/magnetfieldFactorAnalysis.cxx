//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - implements methods to display the magnetfield factors
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:07:02 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisError.h"
#include "../include/analysisWarningMsg.h"
#include "../include/magnetfieldFactorAnalysisDef.h"
#include "../include/magnetfieldFactorAnalysis.h"
#include "TStyle.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TFile.h"


/* **************************************************************
 * CLASS magnetfieldFactorAnalysis								*
 * **************************************************************/

/* **************************************************************
 * allocates memory												*
 * **************************************************************/

void magnetfieldFactorAnalysis::allocateMemory() {

	std::string name;
	std::string title;
	char        buffer[shortConversionDigits+1];
	double      factorIncr;

	if (numberOfDisplays > 0) {

		displays = new TGraph*[numberOfDisplays];
		if (displays == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		distances = new double*[numberOfDisplays];
		if (distances == NULL)
			throw memoryAllocationError(ANALYSISLIB);
	
		for (unsigned short i = 0; i < numberOfDisplays; i++) {
			
			displays[i] = new TGraph(numberOfFactors);
			if (displays[i] == NULL)
				throw memoryAllocationError(ANALYSISLIB);

			factors       = displays[i]->GetX();
			distances[i]  = displays[i]->GetY();
			ustos(i, buffer, 10, shortConversionDigits);
			name          = displayName;
			title         = displayTitle;
			name         += "B_";
			name         += buffer;
			title        += "B(";
			title        += buffer;
			title        += ")";
			displays[i]->SetName(name.c_str());
			displays[i]->SetTitle(title.c_str());
			displays[i]->GetXaxis()->SetTitle(MAGNETFIELDFACTORXAXITITLE);
			displays[i]->GetYaxis()->SetTitle(MAGNETFIELDFACTORYAXITITLE);
			factorIncr    = (factorMax - factorMin) / (numberOfFactors - 1);
			
			for (unsigned int j = 0; j < numberOfFactors; j++)
				factors[j] = j * factorIncr + factorMin;

		}

	}

	analysisEnabled = true;

}

/* **************************************************************
 * deletes allocated memory										*
 * **************************************************************/

void magnetfieldFactorAnalysis::deleteMemory() {

	if (displays != NULL) {
		for (unsigned short i = 0; i < numberOfDisplays; i++) {
			if (displays[i] != NULL) {
				delete displays[i];
				displays[i] = NULL;
			}
		}
		delete [] displays;
		displays = NULL;
	}
	if (distances != NULL) {
		delete [] distances;
		distances = NULL;
	}

	analysisEnabled = false;

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void magnetfieldFactorAnalysis::createMagnetfieldFactorAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	TFile*      file              = NULL;
	TDirectory* previousDirectory = NULL;

	outputFileName.clear();

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

	if (previousDirectory != NULL)
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
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void magnetfieldFactorAnalysis::specializeMagnetfieldFactorAnalysisToRoot(bool enable) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = MAGNETFIELDFACTORSAVINGDIRECTORY;
	writeToFile       = false;

	if (!outputFileName.empty()) {

		file = new TFile(outputFileName.c_str(), "UPDATE");
	
		if (file->IsZombie())
			throw cannotOpenFileError(ANALYSISLIB, outputFileName);

	}
	else
		if (gDirectory != NULL)
			gDirectory->cd();

	if (gDirectory != NULL) {

		if (enable) {
			
			writeToFile = true;

			if (!gDirectory->cd(STSMAGNETFIELDFACTORANALYSISDIR)) {

				gDirectory->mkdir(STSMAGNETFIELDFACTORANALYSISDIR, STSMAGNETFIELDFACTORANALYSISDIR);
				gDirectory->cd(STSMAGNETFIELDFACTORANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

		}
		else {

			if (gDirectory->cd(STSMAGNETFIELDFACTORANALYSISDIR))
				gDirectory->Delete(specialDirectory);

			gDirectory = previousDirectory;
			gDirectory->Delete(STSMAGNETFIELDFACTORANALYSISDIR);

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
			throw cannotCloseFileError(ANALYSISLIB, outputFileName);
		delete file;
		file = NULL;
	}

}

/****************************************************************
 * This method writes a display into the file.					*
 ****************************************************************/

void magnetfieldFactorAnalysis::writeDisplay(TGraph* display) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = NULL;
	specialDirectory  = NULL;

	if (display != NULL) {

		if (writeToFile) {

			previousDirectory = gDirectory;
			specialDirectory  = MAGNETFIELDFACTORSAVINGDIRECTORY;

			if (!outputFileName.empty()) {

				file = new TFile(outputFileName.c_str(), "UPDATE");
		
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, outputFileName);

			}
			else
				if (gDirectory != NULL)
					gDirectory->cd();

			if (!gDirectory->cd(STSMAGNETFIELDFACTORANALYSISDIR)) {

				gDirectory->mkdir(STSMAGNETFIELDFACTORANALYSISDIR, STSMAGNETFIELDFACTORANALYSISDIR);
				gDirectory->cd(STSMAGNETFIELDFACTORANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			display->Write();
			/* Workaround to show the TGraph with the TBrowser*/
			display->GetHistogram()->Write();

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
 * This method initializes the global style for each display.	*
 ****************************************************************/

void magnetfieldFactorAnalysis::initDisplayStyle() {

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

void magnetfieldFactorAnalysis::initWindow() {

	if ((window == NULL) && (enableDisplay)) {

		initDisplayStyle();

		window = new TCanvas("MFAWindow", "Magnetfield Factor Analysis", 600, 400);

		numberOfWindowPadColumns = 0;
		numberOfWindowPadRows    = 0;

	}

}

/****************************************************************
 * This method sets the range of the axis for each display.		*
 ****************************************************************/

void magnetfieldFactorAnalysis::setAutomaticAxisRange() {

	double* xValues;
	double* yValues;
	double  xMin;
	double  xMax;
	double  yMin;
	double  yMax;

	for (unsigned short i = 0; i < numberOfDisplays; i++) {

		if (displays[i] != NULL) {

			xValues = displays[i]->GetX();
			yValues = displays[i]->GetY();

			xMin = xValues[0];
			xMax = xValues[0];
			yMin = yValues[0];
			yMax = yValues[0];

			for (int j = 1; j < displays[i]->GetN(); j++) {

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
			displays[i]->GetXaxis()->SetLimits(0.95 * xMin, 1.05 * xMax);
			displays[i]->GetXaxis()->SetRangeUser(0.95 * xMin, 1.05 * xMax);
			displays[i]->GetYaxis()->SetLimits(0.95 * yMin, 1.05 * yMax);
			displays[i]->GetYaxis()->SetRangeUser(0.95 * yMin, 1.05 * yMax);

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

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void magnetfieldFactorAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, bitArray preventDraw) {

	unsigned short numberOfEnabledDisplays;
	double         sqrtOfDisplays;

	if ((numberOfEnabledPadColumns == NULL) || (numberOfEnabledPadRows == NULL))
		throw windowDivisionError(ANALYSISLIB);

	numberOfEnabledDisplays = 0;
	for (unsigned short i = 0; i < numberOfDisplays; i++)
		if (!preventDraw[i])
			numberOfEnabledDisplays++;

	sqrtOfDisplays = sqrt((double)numberOfEnabledDisplays);

	if ((unsigned short)sqrtOfDisplays * (unsigned short)sqrtOfDisplays >= numberOfEnabledDisplays) {

		(*numberOfEnabledPadColumns) = (unsigned short)sqrtOfDisplays;
		(*numberOfEnabledPadRows)    = (unsigned short)sqrtOfDisplays;

	}
	else if (((unsigned short)sqrtOfDisplays + 1) * (unsigned short)sqrtOfDisplays >= numberOfEnabledDisplays) {

		(*numberOfEnabledPadColumns) = (unsigned short)(sqrtOfDisplays + 1);
		(*numberOfEnabledPadRows)    = (unsigned short)sqrtOfDisplays;

	}
	else {

		(*numberOfEnabledPadColumns) = (unsigned short)(sqrtOfDisplays + 1);
		(*numberOfEnabledPadRows)    = (unsigned short)(sqrtOfDisplays + 1);

	}

}

/* **************************************************************
 * Default constructor											*
 * **************************************************************/

magnetfieldFactorAnalysis::magnetfieldFactorAnalysis() 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   displays(NULL),
   distances(NULL),
   factors(NULL),
   numberOfDisplays(0),
   numberOfFactors(0),
   factorMin(0),
   factorMax(0)
{
  /*
	analysisEnabled          = false;
	window                   = NULL;
	numberOfWindowPadColumns = 0;
	numberOfWindowPadRows    = 0;
	enableDisplay            = false;
	outputFileName.clear();
	writeToFile              = false;
	displays                 = NULL;
	distances                = NULL;
	factors                  = NULL;
	numberOfDisplays         = 0;
	numberOfFactors          = 0;
	factorMin                = 0;
	factorMax                = 0;
  */
}

/* **************************************************************
 * Constructor													*
 * **************************************************************/

magnetfieldFactorAnalysis::magnetfieldFactorAnalysis(unsigned short _numberOfDisplays) 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   displays(NULL),
   distances(NULL),
   factors(NULL),
   numberOfDisplays(_numberOfDisplays),
   numberOfFactors(0),
   factorMin(0),
   factorMax(0)
{
  /*
	this->analysisEnabled          = false;
	this->window                   = NULL;
	this->numberOfWindowPadColumns = 0;
	this->numberOfWindowPadRows    = 0;
	this->enableDisplay            = false;
	this->outputFileName.clear();
	this->writeToFile              = false;
	this->displays                 = NULL;
	this->distances                = NULL;
	this->factors                  = NULL;
	this->numberOfDisplays         = _numberOfDisplays;
	this->numberOfFactors          = 0;
	this->factorMin                = 0;
	this->factorMax                = 0;
  */
}

magnetfieldFactorAnalysis::magnetfieldFactorAnalysis(unsigned int _numberOfFactors, double _factorMin, double _factorMax) 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   displays(NULL),
   distances(NULL),
   factors(NULL),
   numberOfDisplays(0),
   numberOfFactors(_numberOfFactors),
   factorMin(_factorMin),
   factorMax(_factorMax)
{
  /*
	this->analysisEnabled          = false;
	this->window                   = NULL;
	this->numberOfWindowPadColumns = 0;
	this->numberOfWindowPadRows    = 0;
	this->enableDisplay            = false;
	this->outputFileName.clear();
	this->writeToFile              = false;
	this->displays                 = NULL;
	this->distances                = NULL;
	this->factors                  = NULL;
	this->numberOfDisplays         = 0;
	this->numberOfFactors          = _numberOfFactors;
	this->factorMin                = _factorMin;
	this->factorMax                = _factorMax;
  */
}

magnetfieldFactorAnalysis::magnetfieldFactorAnalysis(unsigned short _numberOfDisplays, unsigned int _numberOfFactors, double _factorMin, double _factorMax) 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   displays(NULL),
   distances(NULL),
   factors(NULL),
   numberOfDisplays(_numberOfDisplays),
   numberOfFactors(_numberOfFactors),
   factorMin(_factorMin),
   factorMax(_factorMax)
{
  /*
	this->analysisEnabled          = false;
	this->window                   = NULL;
	this->numberOfWindowPadColumns = 0;
	this->numberOfWindowPadRows    = 0;
	this->enableDisplay            = false;
	this->outputFileName.clear();
	this->writeToFile              = false;
	this->displays                 = NULL;
	this->distances                = NULL;
	this->factors                  = NULL;
	this->numberOfDisplays         = _numberOfDisplays;
	this->numberOfFactors          = _numberOfFactors;
	this->factorMin                = _factorMin;
	this->factorMax                = _factorMax;
  */
	allocateMemory();

}

/* **************************************************************
 * Destructor													*
 * **************************************************************/

magnetfieldFactorAnalysis::~magnetfieldFactorAnalysis() {

	deleteMemory();

	if (window != NULL) {
		delete window;
		window = NULL;
	}

}

/* **************************************************************
 * Method inits the variables.									*
 * **************************************************************/

void magnetfieldFactorAnalysis::init(unsigned short _numberOfDisplays) {

	deleteMemory();

	this->numberOfDisplays = _numberOfDisplays;

	allocateMemory();

}
void magnetfieldFactorAnalysis::init(unsigned int _numberOfFactors, double _factorMin, double _factorMax) {

	deleteMemory();

	this->numberOfFactors = _numberOfFactors;
	this->factorMin       = _factorMin;
	this->factorMax       = _factorMax;

	allocateMemory();

}
void magnetfieldFactorAnalysis::init(unsigned short _numberOfDisplays, unsigned int _numberOfFactors, double _factorMin, double _factorMax) {

	deleteMemory();

	this->numberOfDisplays = _numberOfDisplays;
	this->numberOfFactors  = _numberOfFactors;
	this->factorMin        = _factorMin;
	this->factorMax        = _factorMax;

	allocateMemory();

}

/* **************************************************************
 * Method sets the variables.									*
 * **************************************************************/

void magnetfieldFactorAnalysis::set(unsigned short _numberOfDisplays) {

	if (this->numberOfDisplays != _numberOfDisplays) {

		init(_numberOfDisplays);

	}

}
void magnetfieldFactorAnalysis::set(unsigned int _numberOfFactors, double _factorMin, double _factorMax) {

	if ((this->numberOfFactors != _numberOfFactors) || (this->factorMin != _factorMin) || (this->factorMax != _factorMax)) {

		init(_numberOfFactors, _factorMin, _factorMax);

	}

}
void magnetfieldFactorAnalysis::set(unsigned short _numberOfDisplays, unsigned int _numberOfFactors, double _factorMin, double _factorMax) {

	if ((this->numberOfDisplays != _numberOfDisplays) || (this->numberOfFactors != _numberOfFactors) || (this->factorMin != _factorMin) || (this->factorMax != _factorMax)) {

		init(_numberOfDisplays, _numberOfFactors, _factorMin, _factorMax);

	}

}

/* **************************************************************
 * returns the number of magnetfield factor steps				*
 * **************************************************************/

unsigned int magnetfieldFactorAnalysis::getNumberOfFactors() {

	return numberOfFactors;

}

/* **************************************************************
 * returns the magnetfield factor of step number				*
 * **************************************************************/

double magnetfieldFactorAnalysis::getFactor(unsigned int index) {

	double returnValue = 0;

	if (index < numberOfFactors)
		returnValue = factors[index];
	else
		throw indexIsOutOfFactorRangeError(index, numberOfFactors);

	return returnValue;

}

/* **************************************************************
 * sets the distance for the index of a magnetfield factor of a	*
 * specific display												*
 * **************************************************************/

void magnetfieldFactorAnalysis::setDistance(unsigned short display, unsigned int index, double value) {

	if (display < numberOfDisplays) {

		if (index < numberOfFactors) {

			distances[display][index] = value;

		}
		else
			throw indexIsOutOfFactorRangeError(index, numberOfFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays);

}

/* **************************************************************
 * sets the distance for the index of a magnetfield factor of a	*
 * specific display												*
 * **************************************************************/

void magnetfieldFactorAnalysis::addDistance(unsigned short display, unsigned int index, double value) {

	if (display < numberOfDisplays) {

		if (index < numberOfFactors) {

			distances[display][index] += value;

		}
		else
			throw indexIsOutOfFactorRangeError(index, numberOfFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays);

}

/****************************************************************
 * method returns the magnetfield factors for the hough			*
 * transformation.												*
 ****************************************************************/

double magnetfieldFactorAnalysis::getMagnetfieldFactor(unsigned short display) {

	double minimum;
	int    index;
	double magnetfieldFactor = 0;

	if (display < numberOfDisplays) {

		for (unsigned short i = 0; i < numberOfFactors; i++) {

			if (i == 0) {
			
				minimum = distances[display][i];
				index   = 0;
			
			}
			else {
				
				if (distances[display][i] < minimum) {

					minimum = distances[display][i];
					index   = i;

				}

			}

		}

		magnetfieldFactor = ((factorMax - factorMin) / (numberOfFactors - 1)) * index + factorMin;

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays);

	return magnetfieldFactor;

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void magnetfieldFactorAnalysis::initMagnetfieldFactorAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	if (enable) {

		createMagnetfieldFactorAnalysisToRoot(enable, name, justUpdate);

		specializeMagnetfieldFactorAnalysisToRoot(enable);

	}

}

/****************************************************************
 * This method returns true if the magnetfield's display is		*
 * written to file.												*
 ****************************************************************/

bool magnetfieldFactorAnalysis::isMagnetfieldFactorToRootEnabled() {

	return writeToFile;

}

/****************************************************************
 * method writes the magnetfieldFactor-Analysis into			*
 * a root file.													*
 ****************************************************************/

void magnetfieldFactorAnalysis::magnetfieldFactorAnalysisWrite(int eventNumber) {

	char        buffer1[shortConversionDigits+1];
	char        buffer2[intConversionDigits+1];
	std::string oldName;
	std::string oldTitle;
	std::string name;
	std::string title;

	setAutomaticAxisRange();

	itos(eventNumber, buffer2, 10, intConversionDigits);

	for (unsigned short i = 0; i < numberOfDisplays; i++) {

		if (displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name  = displayName;
			title = displayTitle;

			name  += "B_";
			name  += buffer1;
			name  += "_";
			name  += "_";
			name  += MAGNETFIELDFACTOREVENTMODIFIER;
			name  += "_";
			name  += buffer2;
			title += "B(";
			title += buffer1;
			title += ")";
			title += " ";
			title += MAGNETFIELDFACTOREVENTMODIFIER;
			title += ":";
			title += buffer2;

			oldName  = displays[i]->GetName();
			oldTitle = displays[i]->GetTitle();

			displays[i]->SetName(name.c_str());
			displays[i]->SetTitle(title.c_str());

			writeDisplay(displays[i]);

			displays[i]->SetName(oldName.c_str());
			displays[i]->SetTitle(oldTitle.c_str());

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

}

/****************************************************************
 * This method initializes the magnetfield factor's display.	*
 ****************************************************************/

void magnetfieldFactorAnalysis::initMagnetfieldFactorAnalysisDisplay(bool enable) {

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
 * This method returns true if the magnetfield factor's display	*
 * is enabled.													*
 ****************************************************************/

bool magnetfieldFactorAnalysis::isMagnetfieldFactorDisplayEnabled() {

	return enableDisplay;

}

/****************************************************************
 * method updates the magnetfield factor-Analysis display.		*
 ****************************************************************/

void magnetfieldFactorAnalysis::magnetfieldFactorAnalysisUpdate() {

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
			magnetfieldFactorAnalysisDraw();

	}

}

/****************************************************************
 * method draws the magnetfield factor-Analysis display.		*
 ****************************************************************/

void magnetfieldFactorAnalysis::magnetfieldFactorAnalysisDraw(bitArray preventDraw) {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;
	unsigned short numberOfEnabledDisplay;

	if (enableDisplay) {

		evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows, preventDraw);

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

		numberOfEnabledDisplay = 1;
		for (unsigned short i = 0; i < numberOfDisplays; i++) {

			if (!preventDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (displays[i] != NULL) {

					displays[i]->Draw("AC*");

					setAutomaticAxisRange();

				}
				else {

					cannotDrawWarningMsg* cannotDraw = new cannotDrawWarningMsg(ANALYSISLIB);
					cannotDraw->warningMsg();
					if(cannotDraw != NULL) {
						delete cannotDraw;
						cannotDraw = NULL;
					}
			
				}

				numberOfEnabledDisplay++;

			}

		}

		window->cd();

	}

}

/****************************************************************
 * This method returns true if the magnetfield factor's			*
 * analysis is enabled.											*
 ****************************************************************/

bool magnetfieldFactorAnalysis::isMagnetfieldFactorAnalysisEnabled() {

	return isMagnetfieldFactorDisplayEnabled() || isMagnetfieldFactorToRootEnabled() || analysisEnabled;

}
