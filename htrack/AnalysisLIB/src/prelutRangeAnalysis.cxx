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
///     - implements methods to display the prelut ranges analysis results
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:07:03 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisError.h"
#include "../include/analysisWarningMsg.h"
#include "../include/prelutRangeAnalysisDef.h"
#include "../include/prelutRangeAnalysis.h"
#include "TStyle.h"
#include "TFile.h"


/* **************************************************************
 * CLASS prelutRangeAnalysis									*
 * **************************************************************/

/* **************************************************************
 * allocates memory												*
 * **************************************************************/

void prelutRangeAnalysis::allocateMemory() {

	unsigned short i;
	std::string    name;
	std::string    title;
	char           buffer[shortConversionDigits+1];

	if (numberOfDisplays > 1) {

		name          = getDisplayName();
		title         = getDisplayTitle();
		name         += "Hits";
		title        += "(Hits)";

		relative.display = new TH1S(name.c_str(), title.c_str(), numberOfDisplays - 1, 0, numberOfDisplays - 1);
		if (relative.display == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		relative.display->SetXTitle(PRELUTRANGEHITSXAXITITLE);
		relative.display->SetYTitle(PRELUTRANGEHITSYAXITITLE);
		relative.display->SetDirectory(0);

		relative.padNumber = -1;

		constraint.displays = new TH2S*[numberOfDisplays];
		if (constraint.displays == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		constraint.padNumbers = new int[numberOfDisplays];
		if (constraint.padNumbers == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		for (i = 0; i < numberOfDisplays - 1; i++) {
			
			ustos(i, buffer, 10, shortConversionDigits);
			name          = getDisplayName();
			title         = getDisplayTitle();
			name         += "Borders_";
			name         += buffer;
			title        += "(Borders:";
			title        += buffer;
			title        += ")";

			constraint.displays[i] = new TH2S(name.c_str(), title.c_str(), numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
			if (constraint.displays[i] == NULL)
				throw memoryAllocationError(ANALYSISLIB);

			constraint.displays[i]->SetXTitle(PRELUTRANGEXAXITITLE);
			constraint.displays[i]->SetYTitle(PRELUTRANGEYAXITITLE);
			constraint.displays[i]->SetZTitle(PRELUTRANGECONSTRAINTZAXITITLE);
			constraint.displays[i]->SetDirectory(0);

			constraint.padNumbers[i] = -1;

		}

		name          = getDisplayName();
		title         = getDisplayTitle();
		name         += "Borders_";
		name         += "sum";
		title        += "(Borders: ";
		title        += "sum";
		title        += ")";

		constraint.displays[numberOfDisplays - 1] = new TH2S(name.c_str(), title.c_str(), numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
		if (constraint.displays[numberOfDisplays - 1] == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		constraint.displays[numberOfDisplays - 1]->SetXTitle(PRELUTRANGEXAXITITLE);
		constraint.displays[numberOfDisplays - 1]->SetYTitle(PRELUTRANGEYAXITITLE);
		constraint.displays[numberOfDisplays - 1]->SetZTitle(PRELUTRANGECONSTRAINTZAXITITLE);
		constraint.displays[numberOfDisplays - 1]->SetDirectory(0);

		constraint.padNumbers[numberOfDisplays - 1] = -1;

		value.displays = new TH2S*[numberOfDisplays];
		if (value.displays == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		value.padNumbers = new int[numberOfDisplays];
		if (value.padNumbers == NULL)
			throw memoryAllocationError(ANALYSISLIB);
		modifiedValueDisplays = new TH2S*[numberOfDisplays];
		if (modifiedValueDisplays == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		for (i = 0; i < numberOfDisplays - 1; i++) {
			
			ustos(i, buffer, 10, shortConversionDigits);
			name          = getDisplayName();
			title         = getDisplayTitle();
			name         += "Station_";
			name         += buffer;
			title        += "(Station:";
			title        += buffer;
			title        += ")";

			value.displays[i] = new TH2S(name.c_str(), title.c_str(), numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
			if (value.displays[i] == NULL)
				throw memoryAllocationError(ANALYSISLIB);

			value.displays[i]->SetXTitle(PRELUTRANGEXAXITITLE);
			value.displays[i]->SetYTitle(PRELUTRANGEYAXITITLE);
			value.displays[i]->SetZTitle(PRELUTRANGENORMALVALUEZAXITITLE);
			value.displays[i]->SetDirectory(0);

			value.padNumbers[i]      = -1;

			modifiedValueDisplays[i] = NULL;

		}

		name          = getDisplayName();
		title         = getDisplayTitle();
		name         += "Station_";
		name         += "sum";
		title        += "(Station: ";
		title        += "sum";
		title        += ")";

		value.displays[numberOfDisplays - 1] = new TH2S(name.c_str(), title.c_str(), numberOfMinFactors, factorMinMin, factorMinMax, numberOfMaxFactors, factorMaxMin, factorMaxMax);
		if (value.displays[numberOfDisplays - 1] == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		value.displays[numberOfDisplays - 1]->SetXTitle(PRELUTRANGEXAXITITLE);
		value.displays[numberOfDisplays - 1]->SetYTitle(PRELUTRANGEYAXITITLE);
		value.displays[numberOfDisplays - 1]->SetZTitle(PRELUTRANGENORMALVALUEZAXITITLE);
		value.displays[numberOfDisplays - 1]->SetDirectory(0);

		value.padNumbers[numberOfDisplays - 1]      = -1;

		modifiedValueDisplays[numberOfDisplays - 1] = NULL;

	}

	analysisEnabled = true;

}

/* **************************************************************
 * deletes allocated memory										*
 * **************************************************************/

void prelutRangeAnalysis::deleteMemory() {

	if (relative.display != NULL) {
		delete relative.display;
		relative.display = NULL;
	}
	if (constraint.displays != NULL) {
		for (unsigned short i = 0; i < numberOfDisplays; i++) {
			if (constraint.displays[i] != NULL) {
				delete constraint.displays[i];
				constraint.displays[i] = NULL;
			}
		}
		delete [] constraint.displays;
		constraint.displays = NULL;
	}
	if (constraint.padNumbers != NULL) {
		delete [] constraint.padNumbers;
		constraint.padNumbers = NULL;
	}
	if (value.displays != NULL) {
		for (unsigned short i = 0; i < numberOfDisplays; i++) {
			if (value.displays[i] != NULL) {
				delete value.displays[i];
				value.displays[i] = NULL;
			}
		}
		delete [] value.displays;
		value.displays = NULL;
	}
	if (value.padNumbers != NULL) {
		delete [] value.padNumbers;
		value.padNumbers = NULL;
	}
	if (modifiedValueDisplays != NULL) {
		for (unsigned short i = 0; i < numberOfDisplays; i++) {
			if (modifiedValueDisplays[i] != NULL) {
				delete modifiedValueDisplays[i];
				modifiedValueDisplays[i] = NULL;
			}
		}
		delete [] modifiedValueDisplays;
		modifiedValueDisplays = NULL;
	}

	analysisEnabled = false;

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void prelutRangeAnalysis::createPrelutRangeAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

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

void prelutRangeAnalysis::specializePrelutRangeAnalysisToRoot(bool enable) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = PRELUTRANGESAVINGDIRECTORY;
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

			if (!gDirectory->cd(PRELUTRANGEANALYSISDIR)) {

				gDirectory->mkdir(PRELUTRANGEANALYSISDIR, PRELUTRANGEANALYSISDIR);
				gDirectory->cd(PRELUTRANGEANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

		}
		else {

			if (gDirectory->cd(PRELUTRANGEANALYSISDIR))
				gDirectory->Delete(specialDirectory);

			gDirectory = previousDirectory;
			gDirectory->Delete(PRELUTRANGEANALYSISDIR);

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

/* **************************************************************
 * Method uses the relative display to convert the numbers		*
 * of the others into a percentage relation						*
 * **************************************************************/

void prelutRangeAnalysis::doRelative(TH2S* display, unsigned short relativeIndex) {

	double scale;
	double sum;

	if (relative.display != NULL) {

		if (relativeIndex < numberOfDisplays - 1) {

			scale = 100 / relative.display->GetBinContent(relativeIndex + 1);

		}
		else {

			for (unsigned short k = 0; k < numberOfDisplays - 1; k++)
				sum += relative.display->GetBinContent(k + 1);
			scale = 100 / sum;

		}

		if (display != NULL)
			display->Scale(scale);

	}
	else {

		relativeDisplayNotAccessibleWarningMsg* relativeDisplayNotAccessible = new relativeDisplayNotAccessibleWarningMsg();
		relativeDisplayNotAccessible->warningMsg();
		if(relativeDisplayNotAccessible != NULL) {
			delete relativeDisplayNotAccessible;
			relativeDisplayNotAccessible = NULL;
		}

	}

}

/* **************************************************************
 * Method uses the relative display to convert the numbers		*
 * of the others into a percentage relation. Afterwards the		*
 * set cut is applied											*
 * **************************************************************/

void prelutRangeAnalysis::doRelativeCut(TH2S* display, unsigned short relativeIndex) {

	double entry;

	doRelative(display, relativeIndex);

	for (unsigned int i = 0; i < getNumberOfMinFactors(); i++) {

		for (unsigned int j = 0; j < getNumberOfMaxFactors(); j++) {

			if (display != NULL) {
		
				entry = display->GetBinContent(i + 1, j + 1);

				if (entry > (double)prelutRangeCut)
					display->SetBinContent(i + 1, j + 1, entry - (double)prelutRangeCut);
				else
					display->SetBinContent(i + 1, j + 1, 0);

			}

		}

	}

}

/****************************************************************
 * This method writes a display into the file.					*
 ****************************************************************/

void prelutRangeAnalysis::writeDisplay(TH1S* display) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = NULL;
	specialDirectory  = NULL;

	if (display != NULL) {

		if (writeToFile) {

			previousDirectory = gDirectory;
			specialDirectory  = PRELUTRANGESAVINGDIRECTORY;

			if (!outputFileName.empty()) {

				file = new TFile(outputFileName.c_str(), "UPDATE");
		
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, outputFileName);

			}
			else
				if (gDirectory != NULL)
					gDirectory->cd();

			if (!gDirectory->cd(PRELUTRANGEANALYSISDIR)) {

				gDirectory->mkdir(PRELUTRANGEANALYSISDIR, PRELUTRANGEANALYSISDIR);
				gDirectory->cd(PRELUTRANGEANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			display->Write();

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
void prelutRangeAnalysis::writeDisplay(TH2S* display) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = NULL;
	specialDirectory  = NULL;

	if (display != NULL) {

		if (writeToFile) {

			previousDirectory = gDirectory;
			specialDirectory  = PRELUTRANGESAVINGDIRECTORY;

			if (!outputFileName.empty()) {

				file = new TFile(outputFileName.c_str(), "UPDATE");
		
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, outputFileName);

			}
			else
				if (gDirectory != NULL)
					gDirectory->cd();

			if (!gDirectory->cd(PRELUTRANGEANALYSISDIR)) {

				gDirectory->mkdir(PRELUTRANGEANALYSISDIR, PRELUTRANGEANALYSISDIR);
				gDirectory->cd(PRELUTRANGEANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			display->Write();

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

void prelutRangeAnalysis::initDisplayStyle() {

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

		gStyle->SetHistFillColor(1);
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

void prelutRangeAnalysis::initWindow() {

	if ((window == NULL) && (enableDisplay)) {

		initDisplayStyle();

		window = new TCanvas(getWindowName().c_str(), getWindowTitle().c_str(), 600, 400);

		numberOfWindowPadColumns = 0;
		numberOfWindowPadRows    = 0;

	}

}

/****************************************************************
 * This method evaluate the window division factors.			*
 ****************************************************************/

void prelutRangeAnalysis::evaluateWindowDivisionFactors(unsigned short* numberOfEnabledPadColumns, unsigned short* numberOfEnabledPadRows, bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw) {

	unsigned short numberOfEnabledDisplays;
	double         sqrtOfDisplays;

	if ((numberOfEnabledPadColumns == NULL) || (numberOfEnabledPadRows == NULL))
		throw windowDivisionError(ANALYSISLIB);

	numberOfEnabledDisplays = 0;
	for (unsigned short i = 0; i < numberOfDisplays - 1; i++)
		if (!preventStationDraw[i])
			numberOfEnabledDisplays++;
	if (!preventStationSumDraw)
		numberOfEnabledDisplays++;
	for (unsigned short i = 0; i < numberOfDisplays - 1; i++)
		if (!preventConstraintDraw[i])
			numberOfEnabledDisplays++;
	if (!preventConstraintSumDraw)
		numberOfEnabledDisplays++;
	if (!preventRelativeDraw)
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

prelutRangeAnalysis::prelutRangeAnalysis() 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   relative(),
   constraint(),
   value(),
   modifiedValueDisplays(NULL),
   numberOfDisplays(0),
   numberOfMinFactors(0),
   factorMinMin(0),
   factorMinMax(0),
   numberOfMaxFactors(0),
   factorMaxMin(0),
   factorMaxMax(0),
   prelutRangeCut(100)
{
  /*
	analysisEnabled          = false;
	window                   = NULL;
	numberOfWindowPadColumns = 0;
	numberOfWindowPadRows    = 0;
	enableDisplay            = false;
	outputFileName.clear();
	writeToFile              = false;
	relative.display         = NULL;
	constraint.displays      = NULL;
	constraint.padNumbers    = NULL;
	value.displays           = NULL;
	value.padNumbers         = NULL;
	modifiedValueDisplays    = NULL;
	numberOfDisplays         = 0;
	numberOfMinFactors       = 0;
	factorMinMin             = 0;
	factorMinMax             = 0;
	numberOfMaxFactors       = 0;
	factorMaxMin             = 0;
	factorMaxMax             = 0;
	prelutRangeCut           = 100;
  */
}

/* **************************************************************
 * Constructor													*
 * **************************************************************/

prelutRangeAnalysis::prelutRangeAnalysis(unsigned short _numberOfDisplays) 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   relative(),
   constraint(),
   value(),
   modifiedValueDisplays(NULL),
   numberOfDisplays(_numberOfDisplays + 1),
   numberOfMinFactors(0),
   factorMinMin(0),
   factorMinMax(0),
   numberOfMaxFactors(0),
   factorMaxMin(0),
   factorMaxMax(0),
   prelutRangeCut(100)
{
  /*
	this->analysisEnabled          = false;
	this->window                   = NULL;
	this->numberOfWindowPadColumns = 0;
	this->numberOfWindowPadRows    = 0;
	this->enableDisplay            = false;
	this->outputFileName.clear();
	this->writeToFile              = false;
	this->relative.display         = NULL;
	this->constraint.displays      = NULL;
	this->constraint.padNumbers    = NULL;
	this->value.displays           = NULL;
	this->value.padNumbers         = NULL;
	this->modifiedValueDisplays    = NULL;
	this->numberOfDisplays         = _numberOfDisplays + 1;
	this->numberOfMinFactors       = 0;
	this->factorMinMin             = 0;
	this->factorMinMax             = 0;
	this->numberOfMaxFactors       = 0;
	this->factorMaxMin             = 0;
	this->factorMaxMax             = 0;
	this->prelutRangeCut           = 100;
  */
}
prelutRangeAnalysis::prelutRangeAnalysis(unsigned short _prelutRangeCut, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   relative(),
   constraint(),
   value(),
   modifiedValueDisplays(NULL),
   numberOfDisplays(0),
   numberOfMinFactors(_numberOfMinFactors),
   factorMinMin(_factorMinMin),
   factorMinMax(_factorMinMax),
   numberOfMaxFactors(_numberOfMaxFactors),
   factorMaxMin(_factorMaxMin),
   factorMaxMax(_factorMaxMax),
   prelutRangeCut(_prelutRangeCut)
{
  /*
	this->analysisEnabled          = false;
	this->window                   = NULL;
	this->numberOfWindowPadColumns = 0;
	this->numberOfWindowPadRows    = 0;
	this->enableDisplay            = false;
	this->outputFileName.clear();
	this->writeToFile              = false;
	this->relative.display         = NULL;
	this->constraint.displays      = NULL;
	this->constraint.padNumbers    = NULL;
	this->value.displays           = NULL;
	this->value.padNumbers         = NULL;
	this->modifiedValueDisplays    = NULL;
	this->numberOfDisplays         = 0;
	this->numberOfMinFactors       = _numberOfMinFactors;
	this->factorMinMin             = _factorMinMin;
	this->factorMinMax             = _factorMinMax;
	this->numberOfMaxFactors       = _numberOfMaxFactors;
	this->factorMaxMin             = _factorMaxMin;
	this->factorMaxMax             = _factorMaxMax;
	this->prelutRangeCut           = _prelutRangeCut;
  */
}

prelutRangeAnalysis::prelutRangeAnalysis(unsigned short _prelutRangeCut, unsigned short _numberOfDisplays, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) 
 : analysisEnabled(false),
   window(NULL),
   numberOfWindowPadColumns(0),
   numberOfWindowPadRows(0),
   enableDisplay(false),
   outputFileName(),
   writeToFile(false),
   relative(),
   constraint(),
   value(),
   modifiedValueDisplays(NULL),
   numberOfDisplays(_numberOfDisplays + 1),
   numberOfMinFactors(_numberOfMinFactors),
   factorMinMin(_factorMinMin),
   factorMinMax(_factorMinMax),
   numberOfMaxFactors(_numberOfMaxFactors),
   factorMaxMin(_factorMaxMin),
   factorMaxMax(_factorMaxMax),
   prelutRangeCut(_prelutRangeCut)

{
  /*
	this->analysisEnabled          = false;
	this->window                   = NULL;
	this->numberOfWindowPadColumns = 0;
	this->numberOfWindowPadRows    = 0;
	this->enableDisplay            = false;
	this->outputFileName.clear();
	this->writeToFile              = false;
	this->relative.display         = NULL;
	this->constraint.displays      = NULL;
	this->constraint.padNumbers    = NULL;
	this->value.displays           = NULL;
	this->value.padNumbers         = NULL;
	this->modifiedValueDisplays    = NULL;
	this->numberOfDisplays         = _numberOfDisplays + 1;
	this->numberOfMinFactors       = _numberOfMinFactors;
	this->factorMinMin             = _factorMinMin;
	this->factorMinMax             = _factorMinMax;
	this->numberOfMaxFactors       = _numberOfMaxFactors;
	this->factorMaxMin             = _factorMaxMin;
	this->factorMaxMax             = _factorMaxMax;
	this->prelutRangeCut           = _prelutRangeCut;
  */
	allocateMemory();

}

/* **************************************************************
 * Destructor													*
 * **************************************************************/

prelutRangeAnalysis::~prelutRangeAnalysis() {

	deleteMemory();

}

/* **************************************************************
 * Method inits the variables.									*
 * **************************************************************/

void prelutRangeAnalysis::init(unsigned short _numberOfDisplays) {

	deleteMemory();

	this->numberOfDisplays         = _numberOfDisplays + 1;

	allocateMemory();

}
void prelutRangeAnalysis::init(unsigned short _prelutRangeCut, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) {

	deleteMemory();

	this->numberOfMinFactors       = _numberOfMinFactors;
	this->factorMinMin             = _factorMinMin;
	this->factorMinMax             = _factorMinMax;
	this->numberOfMaxFactors       = _numberOfMaxFactors;
	this->factorMaxMin             = _factorMaxMin;
	this->factorMaxMax             = _factorMaxMax;
	this->prelutRangeCut           = _prelutRangeCut;

	allocateMemory();

}
void prelutRangeAnalysis::init(unsigned short _prelutRangeCut, unsigned short _numberOfDisplays, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) {

	deleteMemory();

	this->numberOfDisplays         = _numberOfDisplays + 1;
	this->numberOfMinFactors       = _numberOfMinFactors;
	this->factorMinMin             = _factorMinMin;
	this->factorMinMax             = _factorMinMax;
	this->numberOfMaxFactors       = _numberOfMaxFactors;
	this->factorMaxMin             = _factorMaxMin;
	this->factorMaxMax             = _factorMaxMax;
	this->prelutRangeCut           = _prelutRangeCut;

	allocateMemory();

}

/* **************************************************************
 * Method sets the variables.									*
 * **************************************************************/

void prelutRangeAnalysis::set(unsigned short _numberOfDisplays) {

	if (this->numberOfDisplays != _numberOfDisplays + 1) {

		init(_numberOfDisplays);

	}

}
void prelutRangeAnalysis::set(unsigned short _prelutRangeCut, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) {

	if ((this->numberOfMinFactors != _numberOfMinFactors) || (this->factorMinMin != _factorMinMin) || (this->factorMinMax != _factorMinMax) || (this->numberOfMaxFactors != _numberOfMaxFactors) || (this->factorMaxMin != _factorMaxMin) || (this->factorMaxMax != _factorMaxMax)) {

		init(_prelutRangeCut, _numberOfMinFactors, _factorMinMin, _factorMinMax, _numberOfMaxFactors, _factorMaxMin, _factorMaxMax);

	}

}
void prelutRangeAnalysis::set(unsigned short _prelutRangeCut, unsigned short _numberOfDisplays, unsigned int _numberOfMinFactors, double _factorMinMin, double _factorMinMax, unsigned int _numberOfMaxFactors, double _factorMaxMin, double _factorMaxMax) {

	if ((this->numberOfDisplays != _numberOfDisplays + 1) || (this->numberOfMinFactors != _numberOfMinFactors) || (this->factorMinMin != _factorMinMin) || (this->factorMinMax != _factorMinMax) || (this->numberOfMaxFactors != _numberOfMaxFactors) || (this->factorMaxMin != factorMaxMin) || (this->factorMaxMax != factorMaxMax)) {

		init(_prelutRangeCut, _numberOfDisplays, _numberOfMinFactors, _factorMinMin, _factorMinMax, _numberOfMaxFactors, _factorMaxMin, _factorMaxMax);

	}

}

/* **************************************************************
 * returns the number of value steps							*
 * **************************************************************/

unsigned int prelutRangeAnalysis::getNumberOfMinFactors() {

	return numberOfMinFactors;

}
unsigned int prelutRangeAnalysis::getNumberOfMaxFactors() {

	return numberOfMaxFactors;

}

/* **************************************************************
 * returns the value of the index								*
 * **************************************************************/

double prelutRangeAnalysis::getMinFactor(unsigned int index) {

	double returnValue = 0;

	if (index < numberOfMinFactors)
		returnValue = ((double)index) * (factorMinMax - factorMinMin) / ((double)numberOfMinFactors - 1) + factorMinMin;
	else
		throw indexIsOutOfFactorRangeError(index, numberOfMinFactors);

	return returnValue;

}
double prelutRangeAnalysis::getMaxFactor(unsigned int index) {

	double returnValue = 0;

	if (index < numberOfMaxFactors)
		returnValue = ((double)index) * (factorMaxMax - factorMaxMin) / ((double)numberOfMaxFactors - 1) + factorMaxMin;
	else
		throw indexIsOutOfFactorRangeError(index, numberOfMaxFactors);

	return returnValue;

}

/* **************************************************************
 * resets the values in all displays							*
 * **************************************************************/

void prelutRangeAnalysis::reset() {

	if (relative.display != NULL)
		relative.display->Reset();

	for (unsigned short i = 0; i < numberOfDisplays; i++) {
	
		constraint.displays[i]->Reset();
		value.displays[i]->Reset();

	}

}

/* **************************************************************
 * returns the value in the relative display at index			*
 * **************************************************************/

double prelutRangeAnalysis::getRelative(unsigned short index) {

	double returnValue = 0;

	if (index < numberOfDisplays - 1) {

		if (relative.display != NULL)
			returnValue = relative.display->GetBinContent(index + 1);
		else {

			relativeDisplayNotAccessibleWarningMsg* relativeDisplayNotAccessible = new relativeDisplayNotAccessibleWarningMsg();
			relativeDisplayNotAccessible->warningMsg();
			if(relativeDisplayNotAccessible != NULL) {
				delete relativeDisplayNotAccessible;
				relativeDisplayNotAccessible = NULL;
			}
	
		}

	}
	else
		throw indexIsOutOfDisplayRangeError(index, numberOfDisplays - 1);

	return returnValue;

}

/* **************************************************************
 * sets the value in the relative display at index				*
 * **************************************************************/

void prelutRangeAnalysis::setRelative(unsigned short index, double weight) {

	if (index < numberOfDisplays - 1) {

		if (relative.display != NULL)
			relative.display->SetBinContent(index + 1, weight);
		else {

			relativeDisplayNotAccessibleWarningMsg* relativeDisplayNotAccessible = new relativeDisplayNotAccessibleWarningMsg();
			relativeDisplayNotAccessible->warningMsg();
			if(relativeDisplayNotAccessible != NULL) {
				delete relativeDisplayNotAccessible;
				relativeDisplayNotAccessible = NULL;
			}
	
		}

	}
	else
		throw indexIsOutOfDisplayRangeError(index, numberOfDisplays - 1);

}

/* **************************************************************
 * adds the value in the relative display at index				*
 * **************************************************************/

void prelutRangeAnalysis::addRelative(unsigned short index, double weight) {

	if (index < numberOfDisplays - 1) {

		if (relative.display != NULL)
			relative.display->AddBinContent(index + 1, weight);
		else {

			relativeDisplayNotAccessibleWarningMsg* relativeDisplayNotAccessible = new relativeDisplayNotAccessibleWarningMsg();
			relativeDisplayNotAccessible->warningMsg();
			if(relativeDisplayNotAccessible != NULL) {
				delete relativeDisplayNotAccessible;
				relativeDisplayNotAccessible = NULL;
			}

		}

	}
	else
		throw indexIsOutOfDisplayRangeError(index, numberOfDisplays - 1);

}

/* **************************************************************
 * returns the value in the constraint display at index			*
 * **************************************************************/

double prelutRangeAnalysis::getConstraint(unsigned short display, unsigned int indexMin, unsigned int indexMax) {

	double returnValue;

	if (display < numberOfDisplays) {

		if (indexMin < numberOfMinFactors) {

			if (indexMax < numberOfMaxFactors) {
		
				returnValue = constraint.displays[display]->GetBinContent(indexMin + 1, indexMax + 1);

			}
			else
				throw indexIsOutOfFactorRangeError(indexMax, numberOfMaxFactors);

		}
		else
			throw indexIsOutOfFactorRangeError(indexMin, numberOfMinFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays - 1);

	return returnValue;

}

/* **************************************************************
 * sets the value in the constraint display at index			*
 * **************************************************************/

void prelutRangeAnalysis::setConstraint(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight) {

	if (display < numberOfDisplays - 1) {

		if (indexMin < numberOfMinFactors) {

			if (indexMax < numberOfMaxFactors) {
		
				constraint.displays[display]->SetBinContent(indexMin + 1, indexMax + 1, weight);
				constraint.displays[numberOfDisplays - 1]->SetBinContent(indexMin + 1, indexMax + 1, 0);
				for (unsigned short i = 0; i < numberOfDisplays - 1; i++)
					constraint.displays[numberOfDisplays - 1]->AddBinContent(constraint.displays[numberOfDisplays - 1]->GetBin(indexMin + 1, indexMax + 1), weight);

			}
			else
				throw indexIsOutOfFactorRangeError(indexMax, numberOfMaxFactors);

		}
		else
			throw indexIsOutOfFactorRangeError(indexMin, numberOfMinFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays - 1);

}

/* **************************************************************
 * adds the value in the constraint display at index			*
 * **************************************************************/

void prelutRangeAnalysis::addConstraint(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight) {

	if (display < numberOfDisplays - 1) {

		if (indexMin < numberOfMinFactors) {

			if (indexMax < numberOfMaxFactors) {
		
				constraint.displays[display]->AddBinContent(constraint.displays[numberOfDisplays - 1]->GetBin(indexMin + 1, indexMax + 1), weight);
				constraint.displays[numberOfDisplays - 1]->AddBinContent(constraint.displays[numberOfDisplays - 1]->GetBin(indexMin + 1, indexMax + 1), weight);

			}
			else
				throw indexIsOutOfFactorRangeError(indexMax, numberOfMaxFactors);

		}
		else
			throw indexIsOutOfFactorRangeError(indexMin, numberOfMinFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays - 1);

}

/* **************************************************************
 * returns the value for the min/max combination identified by	*
 * indexMin and indexMax in a specific display					*
 * **************************************************************/

double prelutRangeAnalysis::getValue(unsigned short display, unsigned int indexMin, unsigned int indexMax) {

	double returnValue = 0;

	if (display < numberOfDisplays) {

		if (indexMin < numberOfMinFactors) {

			if (indexMax < numberOfMaxFactors) {
		
				returnValue = value.displays[display]->GetBinContent(indexMin + 1, indexMax + 1);

			}
			else
				throw indexIsOutOfFactorRangeError(indexMax, numberOfMaxFactors);

		}
		else
			throw indexIsOutOfFactorRangeError(indexMin, numberOfMinFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays - 1);

	return returnValue;

}

/* **************************************************************
 * sets the value for the min/max combination identified by		*
 * indexMin and indexMax in a specific display					*
 * **************************************************************/

void prelutRangeAnalysis::setValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight) {

	if (display < numberOfDisplays - 1) {

		if (indexMin < numberOfMinFactors) {

			if (indexMax < numberOfMaxFactors) {
		
				value.displays[display]->SetBinContent(indexMin + 1, indexMax + 1, weight);
				value.displays[numberOfDisplays - 1]->SetBinContent(indexMin + 1, indexMax + 1, 0);
				for (unsigned short i = 0; i < numberOfDisplays - 1; i++)
					value.displays[numberOfDisplays - 1]->AddBinContent(value.displays[numberOfDisplays - 1]->GetBin(indexMin + 1, indexMax + 1), weight);

			}
			else
				throw indexIsOutOfFactorRangeError(indexMax, numberOfMaxFactors);

		}
		else
			throw indexIsOutOfFactorRangeError(indexMin, numberOfMinFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays - 1);

}

/* **************************************************************
 * sets the value for the min/max combination identified by		*
 * indexMin and indexMax in a specific display					*
 * **************************************************************/

void prelutRangeAnalysis::addValue(unsigned short display, unsigned int indexMin, unsigned int indexMax, double weight) {

	if (display < numberOfDisplays - 1) {

		if (indexMin < numberOfMinFactors) {

			if (indexMax < numberOfMaxFactors) {
		
				value.displays[display]->AddBinContent(value.displays[numberOfDisplays - 1]->GetBin(indexMin + 1, indexMax + 1), weight);
				value.displays[numberOfDisplays - 1]->AddBinContent(value.displays[numberOfDisplays - 1]->GetBin(indexMin + 1, indexMax + 1), weight);

			}
			else
				throw indexIsOutOfFactorRangeError(indexMax, numberOfMaxFactors);

		}
		else
			throw indexIsOutOfFactorRangeError(indexMin, numberOfMinFactors);

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays - 1);

}

/****************************************************************
 * Method returns the best prelut range.						*
 * If the display is set to numberOfDisplays,					*
 * the best of all diatributions would be returned.				*
 ****************************************************************/

void prelutRangeAnalysis::getPrelutConstraintRange(unsigned short display, double& minimum, double& maximum, unsigned short& numberOfPrelutRanges, double& meanEntriesPerHit) {

	double relativSum;
	double threshold;
	double actualConstraint;
	double actualThreshold;
	bool   isFirstTime;

	minimum              = 0;
	maximum              = 0;
	numberOfPrelutRanges = 0;
	meanEntriesPerHit    = 0;

	if (display < numberOfDisplays) {

		if (display == numberOfDisplays - 1) {

			relativSum = 0;
			for (unsigned short k = 0; k < numberOfDisplays - 1; k++)
				relativSum += relative.display->GetBinContent(k + 1);

		}
		else {

			relativSum = relative.display->GetBinContent(display + 1);

		}

		threshold        = (((double)prelutRangeCut) / 100) * relativSum;
		isFirstTime       = true;
		actualConstraint  = 0;
		actualThreshold   = 0;

		for (unsigned int i = 0; i < numberOfMinFactors; i++) {

			for (unsigned int j = 0; j < numberOfMaxFactors; j++) {

				if (relative.display != NULL) {

					if (value.displays[display]->GetBinContent(i + 1, j + 1) >= threshold) {

						numberOfPrelutRanges++;

						if ((isFirstTime) || (constraint.displays[display]->GetBinContent(i + 1, j + 1) < actualConstraint)) {

							isFirstTime       = false;
							minimum           = getMinFactor(i);
							maximum           = getMaxFactor(j);
							actualConstraint  = constraint.displays[display]->GetBinContent(i + 1, j + 1);
							actualThreshold   = value.displays[display]->GetBinContent(i + 1, j + 1);

						}
						else {

							if ((constraint.displays[display]->GetBinContent(i + 1, j + 1) == actualConstraint) && (value.displays[display]->GetBinContent(i + 1, j + 1) > actualThreshold)) {

								isFirstTime       = false;
								minimum           = getMinFactor(i);
								maximum           = getMaxFactor(j);
								actualConstraint  = constraint.displays[display]->GetBinContent(i + 1, j + 1);
								actualThreshold   = value.displays[display]->GetBinContent(i + 1, j + 1);

							}
	
						}

					}

				}
				else {

					relativeDisplayNotAccessibleWarningMsg* relativeDisplayNotAccessible = new relativeDisplayNotAccessibleWarningMsg();
					relativeDisplayNotAccessible->warningMsg();
					if(relativeDisplayNotAccessible != NULL) {
						delete relativeDisplayNotAccessible;
						relativeDisplayNotAccessible = NULL;
					}

				}

			}

		}

		if (relativSum != 0)
			meanEntriesPerHit = actualConstraint / relativSum;

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays);

}
void prelutRangeAnalysis::getPrelutMaximumRange(unsigned short display, double& minimum, double& maximum, unsigned short& maximumPercentage, double& meanEntriesPerHit) {

	double threshold;
	double numberOfEntries;
	double relativeSum;

	threshold         = 0;
	minimum           = 0;
	maximum           = 0;
	maximumPercentage = 0;
	meanEntriesPerHit = 0;

	if (display < numberOfDisplays) {

		for (unsigned int i = 0; i < numberOfMinFactors; i++) {

			for (unsigned int j = 0; j < numberOfMaxFactors; j++) {

				if (value.displays[display]->GetBinContent(i + 1, j + 1) > threshold) {

					minimum         = getMinFactor(i);
					maximum         = getMaxFactor(j);
					threshold       = value.displays[display]->GetBinContent(i + 1, j + 1);
					numberOfEntries = constraint.displays[display]->GetBinContent(i + 1, j + 1);

				}

			}

		}

		if (relative.display != NULL) {

			if (display == numberOfDisplays - 1) {

				relativeSum = 0;
				for (unsigned short k = 0; k < numberOfDisplays - 1; k++)
					relativeSum += relative.display->GetBinContent(k + 1);

			}
			else {

				relativeSum = relative.display->GetBinContent(display + 1);

			}

			if (relativeSum != 0) {

				maximumPercentage = (unsigned short)(((threshold * 100) / relativeSum) + 0.5);
				meanEntriesPerHit = numberOfEntries / relativeSum;

			}
			else {

				maximumPercentage = 100;
				meanEntriesPerHit = 1;

			}

		}
		else {

			maximumPercentage = 100;
			meanEntriesPerHit = 1;

		}

	}
	else
		throw indexIsOutOfDisplayRangeError(display, numberOfDisplays);

}

/****************************************************************
 * This method initializes the root directory for the displays.	*
 ****************************************************************/

void prelutRangeAnalysis::initPrelutRangeAnalysisToRoot(bool enable, const char* name, bool justUpdate) {

	if (enable) {

		createPrelutRangeAnalysisToRoot(enable, name, justUpdate);

		specializePrelutRangeAnalysisToRoot(enable);

	}

}

/****************************************************************
 * This method returns true if the prelut range's display is	*
 * written to file.												*
 ****************************************************************/

bool prelutRangeAnalysis::isPrelutRangeToRootEnabled() {

	return writeToFile;

}

/****************************************************************
 * method writes the prelutRange-Analysis into					*
 * a root file.													*
 ****************************************************************/

void prelutRangeAnalysis::prelutRangeAnalysisWrite(int eventNumber) {

	unsigned short i;
	char           buffer1[shortConversionDigits+1];
	char           buffer2[intConversionDigits+1];
	std::string    oldName;
	std::string    oldTitle;
	std::string    name;
	std::string    title;

	itos(eventNumber, buffer2, 10, intConversionDigits);

	if (relative.display != NULL) {

		name  = getDisplayName();
		title = getDisplayTitle();

		name  += "Hits";
		name  += "_";
		name  += "_";
		name  += PRELUTRANGEEVENTMODIFIER;
		name  += "_";
		name  += buffer2;
		title += "(Hits)";
		title += " ";
		title += PRELUTRANGEEVENTMODIFIER;
		title += ":";
		title += buffer2;

		oldName  = relative.display->GetName();
		oldTitle = relative.display->GetTitle();

		relative.display->SetName(name.c_str());
		relative.display->SetTitle(title.c_str());

		writeDisplay(relative.display);

		relative.display->SetName(oldName.c_str());
		relative.display->SetTitle(oldTitle.c_str());

	}
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

	for (i = 0; i < numberOfDisplays; i++) {

		if (constraint.displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name   = getDisplayName();
			title  = getDisplayTitle();

			if ( i == numberOfDisplays - 1) {

				name  += "Borders_";
				name  += "sum";
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Borders:";
				title += "sum";
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}
			else {

				name  += "Borders_";
				name  += buffer1;
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Borders:";
				title += buffer1;
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}

			oldName  = constraint.displays[i]->GetName();
			oldTitle = constraint.displays[i]->GetTitle();

			constraint.displays[i]->SetName(name.c_str());
			constraint.displays[i]->SetTitle(title.c_str());

			writeDisplay(constraint.displays[i]);

			constraint.displays[i]->SetName(oldName.c_str());
			constraint.displays[i]->SetTitle(oldTitle.c_str());

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

	for (i = 0; i < numberOfDisplays; i++) {

		if (value.displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name   = getDisplayName();
			title  = getDisplayTitle();

			if ( i == numberOfDisplays - 1) {

				name  += "Station_";
				name  += "sum";
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Station:";
				title += "sum";
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}
			else {

				name  += "Station_";
				name  += buffer1;
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Station:";
				title += buffer1;
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}

			oldName  = value.displays[i]->GetName();
			oldTitle = value.displays[i]->GetTitle();

			value.displays[i]->SetName(name.c_str());
			value.displays[i]->SetTitle(title.c_str());

			writeDisplay(value.displays[i]);

			value.displays[i]->SetName(oldName.c_str());
			value.displays[i]->SetTitle(oldTitle.c_str());

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
void prelutRangeAnalysis::prelutRangeAnalysisWriteRelative(int eventNumber) {

	unsigned short i;
	char           buffer1[shortConversionDigits+1];
	char           buffer2[intConversionDigits+1];
	std::string    oldName;
	std::string    oldTitle;
	std::string    name;
	std::string    title;
	TH2S*          modifiedDisplay;

	itos(eventNumber, buffer2, 10, intConversionDigits);

	if (relative.display != NULL) {

		name  = getDisplayName();
		title = getDisplayTitle();

		name  += "Hits";
		name  += "_";
		name  += "_";
		name  += PRELUTRANGEEVENTMODIFIER;
		name  += "_";
		name  += buffer2;
		title += "(Hits)";
		title += " ";
		title += PRELUTRANGEEVENTMODIFIER;
		title += ":";
		title += buffer2;

		oldName  = relative.display->GetName();
		oldTitle = relative.display->GetTitle();

		relative.display->SetName(name.c_str());
		relative.display->SetTitle(title.c_str());

		writeDisplay(relative.display);

		relative.display->SetName(oldName.c_str());
		relative.display->SetTitle(oldTitle.c_str());

	}
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

	for (i = 0; i < numberOfDisplays; i++) {

		if (constraint.displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name   = getDisplayName();
			title  = getDisplayTitle();

			if ( i == numberOfDisplays - 1) {

				name  += "Borders_";
				name  += "sum";
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Borders:";
				title += "sum";
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}
			else {

				name  += "Borders_";
				name  += buffer1;
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Borders:";
				title += buffer1;
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}

			oldName  = constraint.displays[i]->GetName();
			oldTitle = constraint.displays[i]->GetTitle();

			constraint.displays[i]->SetName(name.c_str());
			constraint.displays[i]->SetTitle(title.c_str());

			writeDisplay(constraint.displays[i]);

			constraint.displays[i]->SetName(oldName.c_str());
			constraint.displays[i]->SetTitle(oldTitle.c_str());

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

	for (i = 0; i < numberOfDisplays; i++) {

		if (value.displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name   = getDisplayName();
			title  = getDisplayTitle();

			if ( i == numberOfDisplays - 1) {

				name  += "Station_";
				name  += "sum";
				name  += "_";
				name  += "_Relative__";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Station:";
				title += "sum";
				title += ")";
				title += "(Relative) ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}
			else {

				name  += "Station_";
				name  += buffer1;
				name  += "_";
				name  += "_Relative__";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Station:";
				title += buffer1;
				title += ")";
				title += "(Relative) ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}

			modifiedDisplay = (TH2S*)value.displays[i]->Clone(name.c_str());
			doRelative(modifiedDisplay, i);
			modifiedDisplay->SetTitle(title.c_str());

			writeDisplay(modifiedDisplay);

			delete modifiedDisplay;

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
void prelutRangeAnalysis::prelutRangeAnalysisWriteRelativeCut(int eventNumber) {

	unsigned short i;
	char           buffer1[shortConversionDigits+1];
	char           buffer2[intConversionDigits+1];
	std::string    oldName;
	std::string    oldTitle;
	std::string    name;
	std::string    title;
	TH2S*          modifiedDisplay;

	itos(eventNumber, buffer2, 10, intConversionDigits);

	if (relative.display != NULL) {

		name  = getDisplayName();
		title = getDisplayTitle();

		name  += "Hits";
		name  += "_";
		name  += "_";
		name  += PRELUTRANGEEVENTMODIFIER;
		name  += "_";
		name  += buffer2;
		title += "(Hits)";
		title += " ";
		title += PRELUTRANGEEVENTMODIFIER;
		title += ":";
		title += buffer2;

		oldName  = relative.display->GetName();
		oldTitle = relative.display->GetTitle();

		relative.display->SetName(name.c_str());
		relative.display->SetTitle(title.c_str());

		writeDisplay(relative.display);

		relative.display->SetName(oldName.c_str());
		relative.display->SetTitle(oldTitle.c_str());

	}
	else {

		cannotWriteWarningMsg* cannotWrite = new cannotWriteWarningMsg();
		cannotWrite->warningMsg();
		if(cannotWrite != NULL) {
			delete cannotWrite;
			cannotWrite = NULL;
		}

	}

	for (i = 0; i < numberOfDisplays; i++) {

		if (constraint.displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name   = getDisplayName();
			title  = getDisplayTitle();

			if ( i == numberOfDisplays - 1) {

				name  += "Borders_";
				name  += "sum";
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Borders:";
				title += "sum";
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}
			else {

				name  += "Borders_";
				name  += buffer1;
				name  += "_";
				name  += "_";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Borders:";
				title += buffer1;
				title += ")";
				title += " ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}

			oldName  = constraint.displays[i]->GetName();
			oldTitle = constraint.displays[i]->GetTitle();

			constraint.displays[i]->SetName(name.c_str());
			constraint.displays[i]->SetTitle(title.c_str());

			writeDisplay(constraint.displays[i]);

			constraint.displays[i]->SetName(oldName.c_str());
			constraint.displays[i]->SetTitle(oldTitle.c_str());

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

	for (i = 0; i < numberOfDisplays; i++) {

		if (value.displays[i] != NULL) {

			ustos(i, buffer1, 10, shortConversionDigits);

			name   = getDisplayName();
			title  = getDisplayTitle();

			if ( i == numberOfDisplays - 1) {

				name  += "Station_";
				name  += "sum";
				name  += "_";
				name  += "RelativeCut__";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Station:";
				title += "sum";
				title += ")";
				title += "(RelativeCut) ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}
			else {

				name  += "Station_";
				name  += buffer1;
				name  += "_";
				name  += "RelativeCut__";
				name  += PRELUTRANGEEVENTMODIFIER;
				name  += "_";
				name  += buffer2;
				title += "(Station:";
				title += buffer1;
				title += ")";
				title += "(RelativeCut) ";
				title += PRELUTRANGEEVENTMODIFIER;
				title += ":";
				title += buffer2;

			}

			modifiedDisplay = (TH2S*)value.displays[i]->Clone(name.c_str());
			doRelative(modifiedDisplay, i);
			modifiedDisplay->SetTitle(title.c_str());

			writeDisplay(modifiedDisplay);

			delete modifiedDisplay;

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

void prelutRangeAnalysis::initPrelutRangeAnalysisDisplay(bool enable) {

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
 * This method returns true if the prelutRange's display		*
 * is enabled.													*
 ****************************************************************/

bool prelutRangeAnalysis::isPrelutRangeDisplayEnabled() {

	return enableDisplay;

}

/****************************************************************
 * method updates the prelutRange-Analysis display.				*
 ****************************************************************/

void prelutRangeAnalysis::prelutRangeAnalysisUpdate() {

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
			prelutRangeAnalysisDraw();

	}

}
void prelutRangeAnalysis::prelutRangeAnalysisUpdateRelative() {

	std::string newName;

	if (enableDisplay) {

		if (window != NULL) {
	
			for (unsigned short i = 0; i < numberOfDisplays; i++) {

				if (value.padNumbers[i] > 0) {

					window->cd(value.padNumbers[i]);
					
					if (value.displays[i] != NULL) {

						if (modifiedValueDisplays[i] != NULL) {
							delete modifiedValueDisplays[i];
							modifiedValueDisplays[i] = NULL;
						}
						newName                   = value.displays[i]->GetName();
						newName                  += "Relative";
						modifiedValueDisplays[i]  = (TH2S*)value.displays[i]->Clone(newName.c_str());
						modifiedValueDisplays[i]->SetZTitle(PRELUTRANGERELATIVEVALUEZAXITITLE);
						modifiedValueDisplays[i]->SetDirectory(0);
						doRelative(modifiedValueDisplays[i], i);
						modifiedValueDisplays[i]->Draw("SURF2");

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
			prelutRangeAnalysisDrawRelative();

	}

}
void prelutRangeAnalysis::prelutRangeAnalysisUpdateRelativeCut() {

	std::string newName;

	if (enableDisplay) {

		if (window != NULL) {
	
			for (unsigned short i = 0; i < numberOfDisplays; i++) {

				if (value.padNumbers[i] > 0) {

					window->cd(value.padNumbers[i]);
					
					if (value.displays[i] != NULL) {

						if (modifiedValueDisplays[i] != NULL) {
							delete modifiedValueDisplays[i];
							modifiedValueDisplays[i] = NULL;
						}
						newName                   = value.displays[i]->GetName();
						newName                  += "RelativeCut";
						modifiedValueDisplays[i]  = (TH2S*)value.displays[i]->Clone(newName.c_str());
						modifiedValueDisplays[i]->SetZTitle(PRELUTRANGERELATIVEVALUEZAXITITLE);
						modifiedValueDisplays[i]->SetDirectory(0);
						doRelativeCut(modifiedValueDisplays[i], i);
						modifiedValueDisplays[i]->Draw("SURF2");

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
			prelutRangeAnalysisDrawRelativeCut();

	}

}

/****************************************************************
 * method draws the prelutRange-Analysis display.				*
 ****************************************************************/

void prelutRangeAnalysis::prelutRangeAnalysisDraw(bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw) {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;
	unsigned short numberOfEnabledDisplay;

	if (enableDisplay) {

		evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows, preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);

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

		numberOfEnabledDisplay = 1;
		for (unsigned short i = 0; i < numberOfDisplays - 1; i++) {

			if (!preventStationDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (value.displays[i] != NULL) {

					value.displays[i]->Draw("SURF2");
					value.padNumbers[i] = numberOfEnabledDisplay;

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

			if (!preventConstraintDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (constraint.displays[i] != NULL) {

					constraint.displays[i]->Draw("SURF2");
					constraint.padNumbers[i] = numberOfEnabledDisplay;

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

		if (!preventStationSumDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (value.displays[numberOfDisplays - 1] != NULL) {

				value.displays[numberOfDisplays - 1]->Draw("SURF2");
				value.padNumbers[numberOfDisplays - 1] = numberOfEnabledDisplay;

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

		if (!preventConstraintSumDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (constraint.displays[numberOfDisplays - 1] != NULL) {

				constraint.displays[numberOfDisplays - 1]->Draw("SURF2");
				constraint.padNumbers[numberOfDisplays - 1] = numberOfEnabledDisplay;

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

		if (!preventRelativeDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (relative.display != NULL) {

				relative.display->SetBarOffset(0.15);
				relative.display->SetBarWidth(0.7);
				relative.display->GetXaxis()->CenterLabels(true);
				relative.display->Draw("bar");
				relative.padNumber = numberOfEnabledDisplay;

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

		window->cd();

	}

}
void prelutRangeAnalysis::prelutRangeAnalysisDrawRelative(bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw) {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;
	unsigned short numberOfEnabledDisplay;
	std::string    newName;

	if (enableDisplay) {

		evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows, preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);

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

		numberOfEnabledDisplay = 1;
		for (unsigned short i = 0; i < numberOfDisplays - 1; i++) {

			if (!preventStationDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (value.displays[i] != NULL) {

					if (modifiedValueDisplays[i] != NULL) {
						delete modifiedValueDisplays[i];
						modifiedValueDisplays[i] = NULL;
					}
					newName                   = value.displays[i]->GetName();
					newName                  += "Relative";
					modifiedValueDisplays[i]  = (TH2S*)value.displays[i]->Clone(newName.c_str());
					modifiedValueDisplays[i]->SetZTitle(PRELUTRANGERELATIVEVALUEZAXITITLE);
					modifiedValueDisplays[i]->SetDirectory(0);
					doRelative(modifiedValueDisplays[i], i);
					modifiedValueDisplays[i]->Draw("SURF2");
					value.padNumbers[i]       = numberOfEnabledDisplay;

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

			if (!preventConstraintDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (constraint.displays[i] != NULL) {

					constraint.displays[i]->Draw("SURF2");
					constraint.padNumbers[i] = numberOfEnabledDisplay;

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

		if (!preventStationSumDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (value.displays[numberOfDisplays - 1] != NULL) {

				if (modifiedValueDisplays[numberOfDisplays - 1] != NULL) {
					delete modifiedValueDisplays[numberOfDisplays - 1];
					modifiedValueDisplays[numberOfDisplays - 1] = NULL;
				}
				newName                                      = value.displays[numberOfDisplays - 1]->GetName();
				newName                                     += "Relative";
				modifiedValueDisplays[numberOfDisplays - 1]  = (TH2S*)value.displays[numberOfDisplays - 1]->Clone(newName.c_str());
				modifiedValueDisplays[numberOfDisplays - 1]->SetZTitle(PRELUTRANGERELATIVEVALUEZAXITITLE);
				modifiedValueDisplays[numberOfDisplays - 1]->SetDirectory(0);
				doRelative(modifiedValueDisplays[numberOfDisplays - 1], numberOfDisplays - 1);
				modifiedValueDisplays[numberOfDisplays - 1]->Draw("SURF2");
				value.padNumbers[numberOfDisplays - 1]       = numberOfEnabledDisplay;

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

		if (!preventConstraintSumDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (constraint.displays[numberOfDisplays - 1] != NULL) {

				constraint.displays[numberOfDisplays - 1]->Draw("SURF2");
				constraint.padNumbers[numberOfDisplays - 1] = numberOfEnabledDisplay;

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

		if (!preventRelativeDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (relative.display != NULL) {

				relative.display->SetBarOffset(0.15);
				relative.display->SetBarWidth(0.7);
				relative.display->GetXaxis()->CenterLabels(true);
				relative.display->Draw("bar");
				relative.padNumber = numberOfEnabledDisplay;

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

		window->cd();

	}

}
void prelutRangeAnalysis::prelutRangeAnalysisDrawRelativeCut(bitArray preventStationDraw, bool preventStationSumDraw, bitArray preventConstraintDraw, bool preventConstraintSumDraw, bool preventRelativeDraw) {

	unsigned short numberOfEnabledPadColumns;
	unsigned short numberOfEnabledPadRows;
	unsigned short numberOfEnabledDisplay;
	std::string    newName;

	if (enableDisplay) {

		evaluateWindowDivisionFactors(&numberOfEnabledPadColumns, &numberOfEnabledPadRows, preventStationDraw, preventStationSumDraw, preventConstraintDraw, preventConstraintSumDraw, preventRelativeDraw);

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

		numberOfEnabledDisplay = 1;
		for (unsigned short i = 0; i < numberOfDisplays - 1; i++) {

			if (!preventStationDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (value.displays[i] != NULL) {

					if (modifiedValueDisplays[i] != NULL) {
						delete modifiedValueDisplays[i];
						modifiedValueDisplays[i] = NULL;
					}
					newName                   = value.displays[i]->GetName();
					newName                  += "RelativeCut";
					modifiedValueDisplays[i]  = (TH2S*)value.displays[i]->Clone(newName.c_str());
					modifiedValueDisplays[i]->SetZTitle(PRELUTRANGERELATIVEVALUEZAXITITLE);
					modifiedValueDisplays[i]->SetDirectory(0);
					doRelativeCut(modifiedValueDisplays[i], i);
					modifiedValueDisplays[i]->Draw("SURF2");
					value.padNumbers[i] = numberOfEnabledDisplay;

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

			if (!preventConstraintDraw[i]) {

				window->cd(numberOfEnabledDisplay);
					
				if (constraint.displays[i] != NULL) {

					constraint.displays[i]->Draw("SURF2");
					constraint.padNumbers[i] = numberOfEnabledDisplay;

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

		if (!preventStationSumDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (value.displays[numberOfDisplays - 1] != NULL) {

				if (modifiedValueDisplays[numberOfDisplays - 1] != NULL) {
					delete modifiedValueDisplays[numberOfDisplays - 1];
					modifiedValueDisplays[numberOfDisplays - 1] = NULL;
				}
				newName                                      = value.displays[numberOfDisplays - 1]->GetName();
				newName                                     += "Relative";
				modifiedValueDisplays[numberOfDisplays - 1]  = (TH2S*)value.displays[numberOfDisplays - 1]->Clone(newName.c_str());
				modifiedValueDisplays[numberOfDisplays - 1]->SetZTitle(PRELUTRANGERELATIVEVALUEZAXITITLE);
				modifiedValueDisplays[numberOfDisplays - 1]->SetDirectory(0);
				doRelativeCut(modifiedValueDisplays[numberOfDisplays - 1], numberOfDisplays - 1);
				modifiedValueDisplays[numberOfDisplays - 1]->Draw("SURF2");
				value.padNumbers[numberOfDisplays - 1] = numberOfEnabledDisplay;

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

		if (!preventConstraintSumDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (constraint.displays[numberOfDisplays - 1] != NULL) {

				constraint.displays[numberOfDisplays - 1]->Draw("SURF2");
				constraint.padNumbers[numberOfDisplays - 1] = numberOfEnabledDisplay;

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

		if (!preventRelativeDraw) {

			window->cd(numberOfEnabledDisplay);
					
			if (relative.display != NULL) {

				relative.display->SetBarOffset(0.15);
				relative.display->SetBarWidth(0.7);
				relative.display->GetXaxis()->CenterLabels(true);
				relative.display->Draw("bar");
				relative.padNumber = numberOfEnabledDisplay;

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

		window->cd();

	}

}

/****************************************************************
 * This method returns true if the prelutRange's				*
 * analysis is enabled.											*
 ****************************************************************/

bool prelutRangeAnalysis::isPrelutRangeAnalysisEnabled() {

	return isPrelutRangeDisplayEnabled() || isPrelutRangeToRootEnabled() || analysisEnabled;

}
