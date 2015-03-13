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
/// $Date: 2008-08-14 12:29:24 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMANALYSIS_H
#define _HISTOGRAMANALYSIS_H


#include "../../DataRootObjectLIB/include/histogramData.h"
#include "TH2S.h"
#include "TCanvas.h"
#include <string>


/* **************************************************************
 * CLASS histogramAnalysis				 						*
 * **************************************************************/

class histogramAnalysis {

 private:
  histogramAnalysis(const histogramAnalysis&);
  histogramAnalysis& operator=(const histogramAnalysis&);

protected:

	histogramData** histogram;
	bool            writeCreated;
	bool            writeEncoded;
	bool            writeFiltered;
	bool            justOneCreated;
	bool            justOneEncoded;
	bool            justOneFiltered;
	bool            showCreated;
	bool            showEncoded;
	bool            showFiltered;
	unsigned short  layerIndex;
	std::string     outputFileName;
	bool            writeToFile;
	TH2S*           displayToFile;
	TCanvas**       window;
	unsigned short  numberOfWindows;
	TH2S**          displaysFromFile;
	unsigned short  numberOfDisplays;

/**
 * This method initializes the global style for each display.
 */

	void initWindowStyle();

/**
 * This method searches for the window with name and title.
 * @param name is the name of the window
 * @param title is the title of the window
 */

	TCanvas* findWindow(std::string name, std::string title);

/**
 * This method initializes the window with a name and a title.
 * @param name is the name of the window
 * @param title is the title of the window
 */

	TCanvas* addWindow(std::string name, std::string title);

/**
 * This method searches for the display with name and title.
 * @param name is the name of the display
 * @param title is the title of the display
 */

	TH2S* findDisplayFromFile(std::string name, std::string title);

/**
 * This method initializes an object for the display read from
 * file.
 */

	TH2S* addDisplayFromFile(TH2S* displayFromFile);

/**
 * This method initializes the root directory for the analysis.
 */

	void createHistogramAnalysisToRoot(const char* name, bool justUpdate);

/**
 * This method initializes the root directory for the analysis.
 */

	void specializeHistogramAnalysisToRoot();

/**
 * This method initializes the root graphics for the analysis.
 */

	void initDisplayToFile();

/**
 * This method sets up some identifying names.
 */

	void setupNames(std::string* nameOfDisplay, std::string* titleOfDisplay, std::string* nameOfEventFolder, std::string* nameOfLayerFolder, unsigned int eventNumber, unsigned short layer);

/**
 * This method fills the root graphics for the analysis.
 */

	void fillDisplay();

/**
 * This method writes the root graphics into the file.
 */

	void writeDisplay(std::string& eventFolder, std::string& layerFolder);

public:

/**
 * Default constructor
 */

	histogramAnalysis();

/**
 * Constructor
 */

	histogramAnalysis(histogramData** histogram);

/**
 * Destructor
 */

	virtual ~histogramAnalysis();

/**
 * This method initializes the object.
 */

	void init(histogramData** histogram);

/**
 * This method initializes the root directory for the analysis.
 * @param enableCreated is the enabling-flag for the histogram after the creation
 * @param enableEncoded is the enabling-flag for the histogram after the encoding
 * @param enableFiltered is the enabling-flag for the histogram after the filtering
 * @param justOneCreated is the enabling-flag for one layer of the histogram after the creation
 * @param justOneEncoded is the enabling-flag for one layer of the histogram after the encoding
 * @param justOneFiltered is the enabling-flag for one layer of the histogram after the filtering
 * @param name is the name of the file. If null is supplied it is written to the standard root output file
 * @param justUpdate just updates the file instead of recreating
 */

	void initHistogramAnalysisToRoot(bool enableCreated = true, bool enableEncoded = true, bool enableFiltered = true, bool justOneCreated = false, bool justOneEncoded = false, bool justOneFiltered = false, const char* name = NULL, bool justUpdate = false);

/**
 * This method initializes the layer for the justOne-analysis.
 */

	void initHistogramLayer(unsigned short layer);

/**
 * This method initializes the root directory for the analysis.
 * @param showCreated is the enabling-flag to show the histogram layer after the creation
 * @param showEncoded is the enabling-flag to show the histogram layer after the encoding
 * @param showFiltered is the enabling-flag to show the histogram layer after the filtering
 * @see initHistogramLayer(unsigned short layer)
 */

	void initHistogramAnalysisToShow(bool showCreated = false, bool showEncoded = false, bool showFiltered = false);

/**
 * This method returns true if the analysis is written to file.
 */

	bool isHistogramToRootEnabled();

/**
 * This method returns true if the histogram after the creation
 * is written to file.
 */

	bool isCreatedHistogramToRootEnabled();

/**
 * This method returns true if the histogram after the encoding
 * is written to file.
 */

	bool isEncodedHistogramToRootEnabled();

/**
 * This method returns true if the histogram after the filtering
 * is written to file.
 */

	bool isFilteredHistogramToRootEnabled();

/**
 * This method returns true if just one histogram after the creation
 * is written to file.
 */

	bool isJustOneCreatedHistogramToRootEnabled();

/**
 * This method returns true if just one histogram after the encoding
 * is written to file.
 */

	bool isJustOneEncodedHistogramToRootEnabled();

/**
 * This method returns true if just one histogram after the filtering
 * is written to file.
 */

	bool isJustOneFilteredHistogramToRootEnabled();

/**
 * This method returns true if the histogram after the creation
 * is shown.
 */

	bool isCreatedHistogramShowingEnabled();

/**
 * This method returns true if the histogram after the encoding
 * is shown.
 */

	bool isEncodedHistogramShowingEnabled();

/**
 * This method returns true if the histogram after the filtering
 * is shown.
 */

	bool isFilteredHistogramShowingEnabled();

/**
 * This method returns the index of the histogram layer
 * which should be accessed.
 */

	unsigned short getHistogramLayerIndexToRoot();

/**
 * This method writes the histogram after the creation
 * to file.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 */

	void createdHistogramLayerWrite(unsigned int eventNumber, unsigned short layer);

/**
 * This method writes the histogram after the encoding
 * to file.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 */

	void encodedHistogramLayerWrite(unsigned int eventNumber, unsigned short layer);

/**
 * This method writes the histogram after the filtering
 * to file.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 */

	void filteredHistogramLayerWrite(unsigned int eventNumber, unsigned short layer);

/**
 * This method shows the histogram after the filtering
 * @warning To use this function the corresponding ...Write function must be used first.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 * @param drawOption sets the options for drawing the TH2S object
 */

	void createdHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption = "TEXT");

/**
 * This method shows the histogram after the creation
 * @warning To use this function the corresponding ...Write function must be used first.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 * @param drawOption sets the options for drawing the TH2S object
 */

	void encodedHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption = "TEXT");

/**
 * This method shows the histogram after the encoding
 * @warning To use this function the corresponding ...Write function must be used first.
 * @param eventNumber is the number of the event to identify the object in the file
 * @param layer is the number of the layer to identify the object in the file
 * @param drawOption sets the options for drawing the TH2S object
 */

	void filteredHistogramLayerShow(unsigned int eventNumber, unsigned short layer, std::string drawOption = "TEXT");

};

#endif
