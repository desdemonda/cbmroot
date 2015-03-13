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
///     - implements methods to analyze the histogram
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:07:02 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/globalWarningMsg.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/analysisWarningMsg.h"
#include "../include/histogramAnalysisDef.h"
#include "../include/histogramAnalysis.h"
#include "TStyle.h"
#include "TFile.h"


/* **************************************************************
 * CLASS histogramAnalysis				 						*
 * **************************************************************/

/****************************************************************
 * This method initializes the global style for each display.	*
 ****************************************************************/

void histogramAnalysis::initWindowStyle() {

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
 * This method searches for the window with name and title.		*
 ****************************************************************/

TCanvas* histogramAnalysis::findWindow(std::string name, std::string title) {

	TCanvas*    returnValue;
	std::string actualName;
	std::string actualTitle;

	returnValue = NULL;
	
	for (unsigned short i = 0; i < numberOfWindows; i++) {

		actualName  = window[i]->GetName();
		actualTitle = window[i]->GetTitle();
		
		if ((name == actualName) && (title == actualTitle)) {

			returnValue = window[i];
			break;

		}

	}

	return returnValue;

}

/****************************************************************
 * This method initializes the window with a name and a title.	*
 ****************************************************************/

TCanvas* histogramAnalysis::addWindow(std::string name, std::string title) {

	numberOfWindows++;
	if (window == NULL)
		window = (TCanvas**)calloc(numberOfWindows, sizeof(TCanvas*));
	else
		window = (TCanvas**)realloc(window, numberOfWindows * sizeof(TCanvas*));

	initWindowStyle();
	window[numberOfWindows - 1] = new TCanvas(name.c_str(), title.c_str(), 600, 400);

	return window[numberOfWindows - 1];

}

/****************************************************************
 * This method searches for the display with name and title.	*
 ****************************************************************/

TH2S* histogramAnalysis::findDisplayFromFile(std::string name, std::string title) {

	TH2S*       returnValue;
	std::string actualName;
	std::string actualTitle;

	returnValue = NULL;
	
	for (unsigned short i = 0; i < numberOfDisplays; i++) {

		actualName  = displaysFromFile[i]->GetName();
		actualTitle = displaysFromFile[i]->GetTitle();
		
		if ((name == actualName) && (title == actualTitle)) {

			returnValue = displaysFromFile[i];
			break;

		}

	}

	return returnValue;

}

/****************************************************************
 * This method initializes an object for the display read from	*
 * file.														*
 ****************************************************************/

TH2S* histogramAnalysis::addDisplayFromFile(TH2S* displayFromFile) {

	TH2S* returnValue;

	if (displayFromFile != NULL) {

		numberOfDisplays++;
		if (displaysFromFile == NULL)
			displaysFromFile = (TH2S**)calloc(numberOfDisplays, sizeof(TH2S*));
		else
			displaysFromFile = (TH2S**)realloc(displaysFromFile, numberOfDisplays * sizeof(TH2S*));

		displaysFromFile[numberOfDisplays - 1] = new TH2S(*displayFromFile);
		
		displaysFromFile[numberOfDisplays - 1]->SetDirectory(0);

		returnValue = displaysFromFile[numberOfDisplays - 1];

	}
	else {

		returnValue = NULL;

		displayNotAccessibleWarningMsg* displayNotAccessible = new displayNotAccessibleWarningMsg();
		displayNotAccessible->warningMsg();
		if(displayNotAccessible != NULL) {
			delete displayNotAccessible;
			displayNotAccessible = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method initializes the root directory for the analysis.	*
 ****************************************************************/

void histogramAnalysis::createHistogramAnalysisToRoot(const char* name, bool justUpdate) {

	TFile*      file              = NULL;
	TDirectory* previousDirectory = NULL;

	outputFileName.clear();

	if ((name != NULL) && (strcmp(name, "") != 0)) {

		if (writeCreated | writeEncoded | writeFiltered) {

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
 * This method initializes the root directory for the analysis.	*
 ****************************************************************/

void histogramAnalysis::specializeHistogramAnalysisToRoot() {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;
	bool        enable;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = HISTOGRAMSAVINGDIRECTORY;
	enable            = writeCreated | writeEncoded | writeFiltered;
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

			if (!gDirectory->cd(STSHISTOGRAMANALYSISDIR)) {

				gDirectory->mkdir(STSHISTOGRAMANALYSISDIR, STSHISTOGRAMANALYSISDIR);
				gDirectory->cd(STSHISTOGRAMANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

		}
		else {

			if (gDirectory->cd(STSHISTOGRAMANALYSISDIR)) {

				gDirectory->Delete(specialDirectory);

			}

			gDirectory = previousDirectory;
			gDirectory->Delete(STSHISTOGRAMANALYSISDIR);

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
 * This method initializes the root graphics for the analysis.	*
 * **************************************************************/

void histogramAnalysis::initDisplayToFile() {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(ANALYSISLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(ANALYSISLIB);

	if (displayToFile != NULL) {

		if ((displayToFile->GetNbinsX() != (*histogram)->getValueDim1()) || (displayToFile->GetNbinsY() != (*histogram)->getValueDim2())) {

			delete displayToFile;
			displayToFile = NULL;

		}

	}

	if (displayToFile == NULL) {

		displayToFile = new TH2S("Histogram", "Histogram", (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim1() - 1, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDim2() - 1);

		displayToFile->SetXTitle(HISTOGRAMXAXITITLE);
		displayToFile->SetYTitle(HISTOGRAMYAXITITLE);
		displayToFile->SetDirectory(0);

	}

}

/* **************************************************************
 * This method sets up some identifying names.					*
 * **************************************************************/

void histogramAnalysis::setupNames(std::string* nameOfDisplay, std::string* titleOfDisplay, std::string* nameOfEventFolder, std::string* nameOfLayerFolder, unsigned int eventNumber, unsigned short layer) {

	char intBuffer[intConversionDigits+1];
	char shortBuffer[shortConversionDigits+1];

	itos(eventNumber, intBuffer, 10, intConversionDigits);
	ustos(layer, shortBuffer, 10, shortConversionDigits);

	if (nameOfEventFolder != NULL) {

		*nameOfEventFolder  = HISTOGRAMEVENTMODIFIER;
		*nameOfEventFolder += "_";
		*nameOfEventFolder += intBuffer;

	}

	if (nameOfLayerFolder != NULL) {

		*nameOfLayerFolder  = LAYERMODIFIER;
		*nameOfLayerFolder += "_";
		*nameOfLayerFolder += shortBuffer;

	}

	if (nameOfDisplay != NULL) {

		if (nameOfEventFolder != NULL) {

			*nameOfDisplay  = "_";
			*nameOfDisplay  += *nameOfEventFolder;

		}

		if (nameOfLayerFolder != NULL) {

			*nameOfDisplay  += "_";
			*nameOfDisplay  += *nameOfLayerFolder;

		}

	}

	if (titleOfDisplay != NULL) {

		*titleOfDisplay += "(";
		*titleOfDisplay += HISTOGRAMEVENTMODIFIER;
		*titleOfDisplay += ":";
		*titleOfDisplay += intBuffer;
		*titleOfDisplay += ", ";
		*titleOfDisplay += LAYERMODIFIER;
		*titleOfDisplay += ":";
		*titleOfDisplay += shortBuffer;
		*titleOfDisplay += ")";

	}

}

/* **************************************************************
 * This method fills the root graphics for the analysis.		*
 * **************************************************************/

void histogramAnalysis::fillDisplay() {

	if (displayToFile != NULL) {

		if (histogram == NULL)
			throw cannotAccessHistogramDataError(ANALYSISLIB);
		if (*histogram == NULL)
			throw cannotAccessHistogramDataError(ANALYSISLIB);

		if ((displayToFile->GetNbinsX() != (*histogram)->getValueDim1()) || (displayToFile->GetNbinsY() != (*histogram)->getValueDim2()))
			initDisplayToFile();

		for (unsigned short i = 0; i < (*histogram)->getValueDim1(); i++) {

			for (unsigned short j = 0; j < (*histogram)->getValueDim2(); j++) {

				displayToFile->SetBinContent(i, j, (*histogram)->getCell(i, j)->value.toULong());

			}

		}

	}
	else {

		cannotFillNotExistingDisplayWarningMsg* cannotFillNotExistingDisplay = new cannotFillNotExistingDisplayWarningMsg();
		cannotFillNotExistingDisplay->warningMsg();
		if(cannotFillNotExistingDisplay != NULL) {
			delete cannotFillNotExistingDisplay;
			cannotFillNotExistingDisplay = NULL;
		}

	}

}

/* **************************************************************
 * This method writes the root graphics into the file.			*
 * **************************************************************/

void histogramAnalysis::writeDisplay(std::string& eventFolder, std::string& layerFolder) {

	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;

	file              = NULL;
	previousDirectory = NULL;
	specialDirectory  = NULL;

	if (displayToFile != NULL) {

		if (writeToFile) {

			previousDirectory = gDirectory;
			specialDirectory  = HISTOGRAMSAVINGDIRECTORY;

			if (!outputFileName.empty()) {

				file = new TFile(outputFileName.c_str(), "UPDATE");
		
				if (file->IsZombie())
					throw cannotOpenFileError(ANALYSISLIB, outputFileName);

			}
			else
				if (gDirectory != NULL)
					gDirectory->cd();

			if (!gDirectory->cd(STSHISTOGRAMANALYSISDIR)) {

				gDirectory->mkdir(STSHISTOGRAMANALYSISDIR, STSHISTOGRAMANALYSISDIR);
				gDirectory->cd(STSHISTOGRAMANALYSISDIR);

			}

			if (!gDirectory->cd(specialDirectory)) {

				gDirectory->mkdir(specialDirectory, specialDirectory);
				gDirectory->cd(specialDirectory);

			}

			if (!eventFolder.empty()) {

				if (!gDirectory->cd(eventFolder.c_str())) {
			
					gDirectory->mkdir(eventFolder.c_str(), eventFolder.c_str());
					gDirectory->cd(eventFolder.c_str());

				}

			}

			if (!layerFolder.empty()) {

				if (!gDirectory->cd(layerFolder.c_str())) {
			
					gDirectory->mkdir(layerFolder.c_str(), layerFolder.c_str());
					gDirectory->cd(layerFolder.c_str());

				}

			}

			displayToFile->Write();

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

/* **************************************************************
 * Default constructor											*
 * **************************************************************/

histogramAnalysis::histogramAnalysis() 
 : histogram(NULL),
   writeCreated(false),
   writeEncoded(false),
   writeFiltered(false),
   justOneCreated(false),
   justOneEncoded(false),
   justOneFiltered(false),
   showCreated(false),
   showEncoded(false),
   showFiltered(false),
   layerIndex(0),
   outputFileName(),
   writeToFile(false),
   displayToFile(NULL),
   window(NULL),
   numberOfWindows(0),
   displaysFromFile(NULL),
   numberOfDisplays(0)
{
  /*
	histogram        = NULL;
	writeCreated     = false;
	writeEncoded     = false;
	writeFiltered    = false;
	justOneCreated   = false;
	justOneEncoded   = false;
	justOneFiltered  = false;
	showCreated      = false;
	showEncoded      = false;
	showFiltered     = false;
	layerIndex       = 0;
	outputFileName.clear();
	writeToFile      = false;
	displayToFile    = NULL;
	window           = NULL;
	numberOfWindows  = 0;
	displaysFromFile = NULL;
	numberOfDisplays = 0;
  */
}

/* **************************************************************
 * Constructor													*
 * **************************************************************/

histogramAnalysis::histogramAnalysis(histogramData** _histogram) 
 : histogram(_histogram),
   writeCreated(false),
   writeEncoded(false),
   writeFiltered(false),
   justOneCreated(false),
   justOneEncoded(false),
   justOneFiltered(false),
   showCreated(false),
   showEncoded(false),
   showFiltered(false),
   layerIndex(0),
   outputFileName(),
   writeToFile(false),
   displayToFile(NULL),
   window(NULL),
   numberOfWindows(0),
   displaysFromFile(NULL),
   numberOfDisplays(0)
{
  /*
	this->histogram  = _histogram;
	writeCreated     = false;
	writeEncoded     = false;
	writeFiltered    = false;
	justOneCreated   = false;
	justOneEncoded   = false;
	justOneFiltered  = false;
	showCreated      = false;
	showEncoded      = false;
	showFiltered     = false;
	layerIndex       = 0;
	outputFileName.clear();
	writeToFile      = false;
	displayToFile    = NULL;
	window           = NULL;
	numberOfWindows  = 0;
	displaysFromFile = NULL;
	numberOfDisplays = 0;
  */
}

/* **************************************************************
 * Destructor													*
 * **************************************************************/

histogramAnalysis::~histogramAnalysis() {

	if (displayToFile != NULL) {

		delete displayToFile;
		displayToFile = NULL;

	}

	if (window != NULL) {

		for (unsigned short i = 0; i < numberOfWindows; i++) {

			if (window[i] != NULL) {

				delete window[i];
				window[i] = NULL;

			}

		}

		delete window;
		window = NULL;

	}

	if (displaysFromFile != NULL) {

		for (unsigned short i = 0; i < numberOfDisplays; i++) {

			if (displaysFromFile[i] != NULL) {

				delete displaysFromFile[i];
				displaysFromFile[i] = NULL;

			}

		}

		delete displaysFromFile;
		displaysFromFile = NULL;

	}

}

/* **************************************************************
 * This method initializes the object.							*
 * **************************************************************/

void histogramAnalysis::init(histogramData** _histogram) {

	this->histogram = _histogram;

}

/* **************************************************************
 * This method initializes the root directory for the analysis.	*
 * **************************************************************/

void histogramAnalysis::initHistogramAnalysisToRoot(bool enableCreated, bool enableEncoded, bool enableFiltered, bool _justOneCreated, bool _justOneEncoded, bool _justOneFiltered, const char* name, bool justUpdate) {

	this->writeCreated    = enableCreated;
	this->writeEncoded    = enableEncoded;
	this->writeFiltered   = enableFiltered;
	this->justOneCreated  = _justOneCreated;
	this->justOneEncoded  = _justOneEncoded;
	this->justOneFiltered = _justOneFiltered;

	if (writeCreated || writeEncoded || writeFiltered) {

		createHistogramAnalysisToRoot(name, justUpdate);

		specializeHistogramAnalysisToRoot();

	}

}

/* **************************************************************
 * This method initializes the layer for the justOne-analysis.	*
 * **************************************************************/

void histogramAnalysis::initHistogramLayer(unsigned short layer) {

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(ANALYSISLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramDataError(ANALYSISLIB);

	/* due to unsigned short, layer is always bigger than zero */
	if (layer < (*histogram)->getValueDim3()) {

		this->layerIndex = layer;

	}
	else {

		enableLayerIsOutOfRangeWarningMsg* enableLayerIsOutOfRange = new enableLayerIsOutOfRangeWarningMsg(layer, 0, (*histogram)->getValueDim3());
		enableLayerIsOutOfRange->warningMsg();
		if(enableLayerIsOutOfRange != NULL) {
			delete enableLayerIsOutOfRange;
			enableLayerIsOutOfRange = NULL;
		}

	}

}

/* **************************************************************
 * This method initializes the root directory for the analysis.	*
 * **************************************************************/

void histogramAnalysis::initHistogramAnalysisToShow(bool _showCreated, bool _showEncoded, bool _showFiltered) {

	this->showCreated  = _showCreated;
	this->showEncoded  = _showEncoded;
	this->showFiltered = _showFiltered;

}

/* **************************************************************
 * This method returns true if the analysis is written to file.	*
 * **************************************************************/

bool histogramAnalysis::isHistogramToRootEnabled() {

	return ((writeCreated || writeEncoded || writeFiltered) && writeToFile);

}

/* **************************************************************
 * This method returns true if the histogram after the creation	*
 * is written to file.											*
 * **************************************************************/

bool histogramAnalysis::isCreatedHistogramToRootEnabled() {

	return (writeCreated && writeToFile);

}

/* **************************************************************
 * This method returns true if the histogram after the encoding	*
 * is written to file.											*
 * **************************************************************/

bool histogramAnalysis::isEncodedHistogramToRootEnabled() {

	return (writeEncoded && writeToFile);

}

/* **************************************************************
 * This method returns true if the histogram after the filtering*
 * is written to file.											*
 * **************************************************************/

bool histogramAnalysis::isFilteredHistogramToRootEnabled() {

	return (writeFiltered && writeToFile);

}

/* **************************************************************
 * This method returns true if just one histogram after the		*
 * creation	is written to file.									*
 * **************************************************************/

bool histogramAnalysis::isJustOneCreatedHistogramToRootEnabled() {

	return (justOneCreated & isCreatedHistogramToRootEnabled());

}

/* **************************************************************
 * This method returns true if just one histogram after the		*
  *encoding is written to file.									*
 * **************************************************************/

bool histogramAnalysis::isJustOneEncodedHistogramToRootEnabled() {

	return (justOneEncoded & isEncodedHistogramToRootEnabled());

}

/* **************************************************************
 * This method returns true if just one histogram after the		*
 * filtering is written to file.								*
 * **************************************************************/

bool histogramAnalysis::isJustOneFilteredHistogramToRootEnabled() {

	return (justOneFiltered & isFilteredHistogramToRootEnabled());

}

/* **************************************************************
 * This method returns true if the histogram after the creation	*
 * is shown.													*
 * **************************************************************/

bool histogramAnalysis::isCreatedHistogramShowingEnabled() {

	return (showCreated & isCreatedHistogramToRootEnabled());

}

/* **************************************************************
 * This method returns true if the histogram after the encoding	*
 * is shown.													*
 * **************************************************************/

bool histogramAnalysis::isEncodedHistogramShowingEnabled() {

	return (showEncoded & isEncodedHistogramToRootEnabled());

}

/* **************************************************************
 * This method returns true if the histogram after the			*
 * filtering is shown.											*
 * **************************************************************/

bool histogramAnalysis::isFilteredHistogramShowingEnabled() {

	return (showFiltered & isFilteredHistogramToRootEnabled());

}

/* **************************************************************
 * This method returns the index of the histogram layer			*
 * which should be accessed.									*
 * **************************************************************/

unsigned short histogramAnalysis::getHistogramLayerIndexToRoot() {

	return layerIndex;

}

/* **************************************************************
 * This method writes the histogram after the creation			*
 * to file.														*
 * **************************************************************/

void histogramAnalysis::createdHistogramLayerWrite(unsigned int eventNumber, unsigned short layer) {

	std::string eventFolder;
	std::string layerFolder;
	std::string nameModifier;
	std::string titleModifier;
	std::string name;
	std::string title;

	initDisplayToFile();

	setupNames(&nameModifier, &titleModifier, &eventFolder, &layerFolder, eventNumber, layer);

	name   = createdDisplayName;
	title  = createdDisplayTitle;

	name  += nameModifier;
	title += titleModifier;

	displayToFile->SetName(name.c_str());
	displayToFile->SetTitle(title.c_str());

	fillDisplay();

#ifndef SAVINGINEVENTSUBDIRECTORY

	eventFolder.clear();

#endif

#ifndef SAVINGINLAYERSUBDIRECTORY

	layerFolder.clear();

#endif

	if (isCreatedHistogramToRootEnabled()) {

		if (isJustOneCreatedHistogramToRootEnabled()) {

			if (layer == layerIndex) {

				writeDisplay(eventFolder, layerFolder);

			}
			else {

				cannotWriteNotEnabledLayerWarningMsg* cannotWriteNotEnabledLayer = new cannotWriteNotEnabledLayerWarningMsg(layer, layerIndex);
				cannotWriteNotEnabledLayer->warningMsg();
				if(cannotWriteNotEnabledLayer != NULL) {
					delete cannotWriteNotEnabledLayer;
					cannotWriteNotEnabledLayer = NULL;
				}

			}

		}
		else {

			writeDisplay(eventFolder, layerFolder);

		}

	}

}

/* **************************************************************
 * This method writes the histogram after the encoding			*
 * to file.														*
 * **************************************************************/

void histogramAnalysis::encodedHistogramLayerWrite(unsigned int eventNumber, unsigned short layer) {

	std::string eventFolder;
	std::string layerFolder;
	std::string nameModifier;
	std::string titleModifier;
	std::string name;
	std::string title;

	initDisplayToFile();

	setupNames(&nameModifier, &titleModifier, &eventFolder, &layerFolder, eventNumber, layer);

	name   = encodedDisplayName;
	title  = encodedDisplayTitle;

	name  += nameModifier;
	title += titleModifier;

	displayToFile->SetName(name.c_str());
	displayToFile->SetTitle(title.c_str());

	fillDisplay();

#ifndef SAVINGINEVENTSUBDIRECTORY

	eventFolder.clear();

#endif

#ifndef SAVINGINLAYERSUBDIRECTORY

	layerFolder.clear();

#endif

	if (isEncodedHistogramToRootEnabled()) {

		if (isJustOneEncodedHistogramToRootEnabled()) {

			if (layer == layerIndex) {

				writeDisplay(eventFolder, layerFolder);

			}
			else {

				cannotWriteNotEnabledLayerWarningMsg* cannotWriteNotEnabledLayer = new cannotWriteNotEnabledLayerWarningMsg(layer, layerIndex);
				cannotWriteNotEnabledLayer->warningMsg();
				if(cannotWriteNotEnabledLayer != NULL) {
					delete cannotWriteNotEnabledLayer;
					cannotWriteNotEnabledLayer = NULL;
				}

			}

		}
		else {

			writeDisplay(eventFolder, layerFolder);

		}

	}

}

/* **************************************************************
 * This method writes the histogram after the filtering			*
 * to file.														*
 * **************************************************************/

void histogramAnalysis::filteredHistogramLayerWrite(unsigned int eventNumber, unsigned short layer) {

	std::string eventFolder;
	std::string layerFolder;
	std::string nameModifier;
	std::string titleModifier;
	std::string name;
	std::string title;

	initDisplayToFile();

	setupNames(&nameModifier, &titleModifier, &eventFolder, &layerFolder, eventNumber, layer);

	name   = filteredDisplayName;
	title  = filteredDisplayTitle;

	name  += nameModifier;
	title += titleModifier;

	displayToFile->SetName(name.c_str());
	displayToFile->SetTitle(title.c_str());

	fillDisplay();

#ifndef SAVINGINEVENTSUBDIRECTORY

	eventFolder.clear();

#endif

#ifndef SAVINGINLAYERSUBDIRECTORY

	layerFolder.clear();

#endif

	if (isFilteredHistogramToRootEnabled()) {

		if (isJustOneFilteredHistogramToRootEnabled()) {

			if (layer == layerIndex) {

				writeDisplay(eventFolder, layerFolder);

			}
			else {

				cannotWriteNotEnabledLayerWarningMsg* cannotWriteNotEnabledLayer = new cannotWriteNotEnabledLayerWarningMsg(layer, layerIndex);
				cannotWriteNotEnabledLayer->warningMsg();
				if(cannotWriteNotEnabledLayer != NULL) {
					delete cannotWriteNotEnabledLayer;
					cannotWriteNotEnabledLayer = NULL;
				}

			}

		}
		else {

			writeDisplay(eventFolder, layerFolder);

		}

	}

}

/* **************************************************************
 * This method shows the histogram after the filtering			*
 * **************************************************************/

void histogramAnalysis::createdHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	std::string eventFolder;
	std::string layerFolder;
	std::string nameModifier;
	std::string titleModifier;
	std::string name;
	std::string title;
	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;
	TH2S*       actualDisplayFromFile;
	TH2S*       actualDisplayInMemory;
	TCanvas*    actualWindow;

	setupNames(&nameModifier, &titleModifier, &eventFolder, &layerFolder, eventNumber, layer);

	name   = createdDisplayName;
	title  = createdDisplayTitle;

	name  += nameModifier;
	title += titleModifier;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = HISTOGRAMSAVINGDIRECTORY;

	actualDisplayInMemory = findDisplayFromFile(name, title);

	if (actualDisplayInMemory == NULL) {

		if (!outputFileName.empty()) {

			file = new TFile(outputFileName.c_str(), "READ");
		
			if (file->IsZombie())
				throw cannotOpenFileError(ANALYSISLIB, outputFileName);

		}
		else
			if (gDirectory != NULL)
				gDirectory->cd();

		gDirectory->cd(STSHISTOGRAMANALYSISDIR);
		gDirectory->cd(specialDirectory);

#ifndef SAVINGINEVENTSUBDIRECTORY

		eventFolder.clear();

#endif

#ifndef SAVINGINLAYERSUBDIRECTORY

		layerFolder.clear();

#endif

		if (!eventFolder.empty())
			gDirectory->cd(eventFolder.c_str());

		if (!layerFolder.empty())
			gDirectory->cd(layerFolder.c_str());

		actualDisplayFromFile = (TH2S*)gDirectory->Get(name.c_str());

		if (actualDisplayFromFile != NULL) {

			actualDisplayInMemory = addDisplayFromFile(actualDisplayFromFile);
			actualDisplayInMemory->SetDirectory(NULL);

		}
		else {

			actualDisplayInMemory = NULL;

			displayCannotBeReadFromFileWarningMsg* displayCannotBeReadFromFile = new displayCannotBeReadFromFileWarningMsg(name , title, outputFileName);
			displayCannotBeReadFromFile->warningMsg();
			if(displayCannotBeReadFromFile != NULL) {
				delete displayCannotBeReadFromFile;
				displayCannotBeReadFromFile = NULL;
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

	if (isJustOneCreatedHistogramToRootEnabled()) {

		if (layer != layerIndex) {

			cannotShowNotEnabledLayerWarningMsg* cannotShowNotEnabledLayer = new cannotShowNotEnabledLayerWarningMsg(layer, layerIndex);
			cannotShowNotEnabledLayer->warningMsg();
			if(cannotShowNotEnabledLayer != NULL) {
				delete cannotShowNotEnabledLayer;
				cannotShowNotEnabledLayer = NULL;
			}

		}

	}

	if (actualDisplayInMemory != NULL) {

		actualWindow = findWindow(name, title);

		if (actualWindow == NULL)
			actualWindow = addWindow(name, title);
		else
			actualWindow->Clear();

		actualWindow->cd();

		actualDisplayInMemory->Draw(drawOption.c_str());

	}

}

/* **************************************************************
 * This method shows the histogram after the creation			*
 * **************************************************************/

void histogramAnalysis::encodedHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	std::string eventFolder;
	std::string layerFolder;
	std::string nameModifier;
	std::string titleModifier;
	std::string name;
	std::string title;
	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;
	TH2S*       actualDisplayFromFile;
	TH2S*       actualDisplayInMemory;
	TCanvas*    actualWindow;

	setupNames(&nameModifier, &titleModifier, &eventFolder, &layerFolder, eventNumber, layer);

	name   = encodedDisplayName;
	title  = encodedDisplayTitle;

	name  += nameModifier;
	title += titleModifier;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = HISTOGRAMSAVINGDIRECTORY;

	actualDisplayInMemory = findDisplayFromFile(name, title);

	if (actualDisplayInMemory == NULL) {

		if (!outputFileName.empty()) {

			file = new TFile(outputFileName.c_str(), "READ");
		
			if (file->IsZombie())
				throw cannotOpenFileError(ANALYSISLIB, outputFileName);

		}
		else
			if (gDirectory != NULL)
				gDirectory->cd();

		gDirectory->cd(STSHISTOGRAMANALYSISDIR);
		gDirectory->cd(specialDirectory);

#ifndef SAVINGINEVENTSUBDIRECTORY

		eventFolder.clear();

#endif

#ifndef SAVINGINLAYERSUBDIRECTORY

		layerFolder.clear();

#endif

		if (!eventFolder.empty())
			gDirectory->cd(eventFolder.c_str());

		if (!layerFolder.empty())
			gDirectory->cd(layerFolder.c_str());

		actualDisplayFromFile = (TH2S*)gDirectory->Get(name.c_str());

		if (actualDisplayFromFile != NULL) {

			actualDisplayInMemory = addDisplayFromFile(actualDisplayFromFile);
			actualDisplayInMemory->SetDirectory(NULL);

		}
		else {

			actualDisplayInMemory = NULL;

			displayCannotBeReadFromFileWarningMsg* displayCannotBeReadFromFile = new displayCannotBeReadFromFileWarningMsg(name , title, outputFileName);
			displayCannotBeReadFromFile->warningMsg();
			if(displayCannotBeReadFromFile != NULL) {
				delete displayCannotBeReadFromFile;
				displayCannotBeReadFromFile = NULL;
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

	if (isJustOneEncodedHistogramToRootEnabled()) {

		if (layer != layerIndex) {

			cannotShowNotEnabledLayerWarningMsg* cannotShowNotEnabledLayer = new cannotShowNotEnabledLayerWarningMsg(layer, layerIndex);
			cannotShowNotEnabledLayer->warningMsg();
			if(cannotShowNotEnabledLayer != NULL) {
				delete cannotShowNotEnabledLayer;
				cannotShowNotEnabledLayer = NULL;
			}

		}

	}

	if (actualDisplayInMemory != NULL) {

		actualWindow = findWindow(name, title);

		if (actualWindow == NULL)
			actualWindow = addWindow(name, title);
		else
			actualWindow->Clear();

		actualWindow->cd();

		actualDisplayInMemory->Draw(drawOption.c_str());

	}

}

/* **************************************************************
 * This method shows the histogram after the encoding			*
 * **************************************************************/

void histogramAnalysis::filteredHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption) {

	std::string eventFolder;
	std::string layerFolder;
	std::string nameModifier;
	std::string titleModifier;
	std::string name;
	std::string title;
	TFile*      file;
	TDirectory* previousDirectory;
	const char* specialDirectory;
	TH2S*       actualDisplayFromFile;
	TH2S*       actualDisplayInMemory;
	TCanvas*    actualWindow;

	setupNames(&nameModifier, &titleModifier, &eventFolder, &layerFolder, eventNumber, layer);

	name   = filteredDisplayName;
	title  = filteredDisplayTitle;

	name  += nameModifier;
	title += titleModifier;

	file              = NULL;
	previousDirectory = gDirectory;
	specialDirectory  = HISTOGRAMSAVINGDIRECTORY;

	actualDisplayInMemory = findDisplayFromFile(name, title);

	if (actualDisplayInMemory == NULL) {

		if (!outputFileName.empty()) {

			file = new TFile(outputFileName.c_str(), "READ");
		
			if (file->IsZombie())
				throw cannotOpenFileError(ANALYSISLIB, outputFileName);

		}
		else
			if (gDirectory != NULL)
				gDirectory->cd();

		gDirectory->cd(STSHISTOGRAMANALYSISDIR);
		gDirectory->cd(specialDirectory);

#ifndef SAVINGINEVENTSUBDIRECTORY

		eventFolder.clear();

#endif

#ifndef SAVINGINLAYERSUBDIRECTORY

		layerFolder.clear();

#endif

		if (!eventFolder.empty())
			gDirectory->cd(eventFolder.c_str());

		if (!layerFolder.empty())
			gDirectory->cd(layerFolder.c_str());

		actualDisplayFromFile = (TH2S*)gDirectory->Get(name.c_str());

		if (actualDisplayFromFile != NULL) {

			actualDisplayInMemory = addDisplayFromFile(actualDisplayFromFile);
			actualDisplayInMemory->SetDirectory(NULL);

		}
		else {

			actualDisplayInMemory = NULL;

			displayCannotBeReadFromFileWarningMsg* displayCannotBeReadFromFile = new displayCannotBeReadFromFileWarningMsg(name , title, outputFileName);
			displayCannotBeReadFromFile->warningMsg();
			if(displayCannotBeReadFromFile != NULL) {
				delete displayCannotBeReadFromFile;
				displayCannotBeReadFromFile = NULL;
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

	if (isJustOneFilteredHistogramToRootEnabled()) {

		if (layer != layerIndex) {

			cannotShowNotEnabledLayerWarningMsg* cannotShowNotEnabledLayer = new cannotShowNotEnabledLayerWarningMsg(layer, layerIndex);
			cannotShowNotEnabledLayer->warningMsg();
			if(cannotShowNotEnabledLayer != NULL) {
				delete cannotShowNotEnabledLayer;
				cannotShowNotEnabledLayer = NULL;
			}

		}

	}

	if (actualDisplayInMemory != NULL) {

		actualWindow = findWindow(name, title);

		if (actualWindow == NULL)
			actualWindow = addWindow(name, title);
		else
			actualWindow->Clear();

		actualWindow->cd();

		actualDisplayInMemory->Draw(drawOption.c_str());

	}

}
