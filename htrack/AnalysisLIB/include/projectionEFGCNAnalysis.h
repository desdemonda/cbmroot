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
///     - implements methods to display the histogram's occupancy
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:25 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PROJECTIONEFGCNANALYSIS_H
#define _PROJECTIONEFGCNANALYSIS_H


#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "TH2S.h"
#include <string>


/**
 * This defines the number of displays for this class.
 * In this case, there is one display for:
 * - the efficiency
 * - the fakes
 * - the ghosts
 * - the clones
 * - the not founds
 * So there are four displays.
 */
#define NUMBEROFGRAPHICEFGCNDISPLAYS 5


/**
 * struct stores the basic information for one display
 */
struct projectionEFGCNDisplayInfo{

projectionEFGCNDisplayInfo() : numberOfXBins(0), numberOfYBins(0), baseName(), baseTitle(), subDirName() {};
	int            numberOfXBins;		/**< Variable stores the steps of the grid in x-dimension */
	int            numberOfYBins;		/**< Variable stores the steps of the grid in y-dimension */
	std::string    baseName;			/**< Variable stores the basic name for the display which is constant over all events */
	std::string    baseTitle;			/**< Variable stores the basic title for the display which is constant over all events */
	std::string    subDirName;			/**< Variable stores the name of the subdirectory to store the analysis results */

};


/* **************************************************************
 * CLASS projectionEFGCNAnalysis		 						*
 * **************************************************************/

class projectionEFGCNAnalysis {

 private:
  projectionEFGCNAnalysis(const projectionEFGCNAnalysis&);
  projectionEFGCNAnalysis& operator=(const projectionEFGCNAnalysis&);

protected:

	TH2S*                      eDisplay;					/**< Object for the efficiency display */
	projectionEFGCNDisplayInfo eGraphicEFGCNDisplayInfo;	/**< Struct for the efficiency display information */
	TH2S*                      fDisplay;					/**< Object for the fakes display */
	projectionEFGCNDisplayInfo fGraphicEFGCNDisplayInfo;	/**< Struct for the fakes display information */
	TH2S*                      gDisplay;					/**< Object for the ghosts display */
	projectionEFGCNDisplayInfo gGraphicEFGCNDisplayInfo;	/**< Struct for the ghosts display information */
	TH2S*                      cDisplay;					/**< Object for the clones display */
	projectionEFGCNDisplayInfo cGraphicEFGCNDisplayInfo;	/**< Struct for the clones display information */
	TH2S*                      nDisplay;					/**< Object for the not founds display */
	projectionEFGCNDisplayInfo nGraphicEFGCNDisplayInfo;	/**< Struct for the not founds display information */
	unsigned short             windowPadColumn;				/**< Variable to store the number of the column pad of the global window to place the displays */
	unsigned short             eWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the efficiency display */
	unsigned short             fWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the fakes display */
	unsigned short             gWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the ghosts display */
	unsigned short             cWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the clones display */
	unsigned short             nWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the not founds display */
	bool                       writeToFile;					/**< Variable to store if writing into the file is enabled */

/**
 * resets the displays
 */

	void resetDisplays();

/**
 * This method writes a display into the file.
 * @param display is the display to save
 * @param info is the information-struct of the display to save
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeDisplay(TH2S* display, projectionEFGCNDisplayInfo* info, int eventNumber, std::string outputFileName);

/**
 * resets the info structs for event and nothing for total
 */

	virtual void reset() = 0;

/**
 * returns the corresponding entry for the x-axi.
 */

	virtual int getEntryOfXAxi(trackCoordinates& coordinates) = 0;

/**
 * returns the corresponding entry for the y-axi.
 */

	virtual int getEntryOfYAxi(trackCoordinates& coordinates) = 0;

/**
 * returns the corresponding string, which is drawn on the x-axi.
 */

	virtual const char* getTitleOfXAxi() = 0;

/**
 * returns the corresponding string, which is drawn on the y-axi.
 */

	virtual const char* getTitleOfYAxi() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfEDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfEDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfFDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfFDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfGDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfGDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfCDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfCDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfNDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfNDisplay() = 0;

/**
 * returns the corresponding string for the saving directory.
 */

	virtual const char* getNameOfSavingDirectory() = 0;

/**
 * returns true, if the saving should be in subdirectories.
 */

	virtual bool getSavingInSubDirectory() = 0;

public:

/**
 * Default constructor
 */

	projectionEFGCNAnalysis();

/**
 * Destructor
 */

	virtual ~projectionEFGCNAnalysis();

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * This method initializes the eDisplay.
 * @param nXBins sets the binning in the x-dimension for this ROOT-histogram
 * @param nYBins sets the binning in the y-dimension for this ROOT-histogram
 */

	void initEDisplay(int nXBins, int nYBins);

/**
 * This method initializes the fDisplay.
 * @param nXBins sets the binning in the x-dimension for this ROOT-histogram
 * @param nYBins sets the binning in the y-dimension for this ROOT-histogram
 */

	void initFDisplay(int nXBins, int nYBins);

/**
 * This method initializes the gDisplay.
 * @param nXBins sets the binning in the x-dimension for this ROOT-histogram
 * @param nYBins sets the binning in the y-dimension for this ROOT-histogram
 */

	void initGDisplay(int nXBins, int nYBins);

/**
 * This method initializes the cDisplay.
 * @param nXBins sets the binning in the x-dimension for this ROOT-histogram
 * @param nYBins sets the binning in the y-dimension for this ROOT-histogram
 */

	void initCDisplay(int nXBins, int nYBins);

/**
 * This method initializes the cDisplay.
 * @param nXBins sets the binning in the x-dimension for this ROOT-histogram
 * @param nYBins sets the binning in the y-dimension for this ROOT-histogram
 */

	void initNDisplay(int nXBins, int nYBins);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the projections
 */

	void initProjectionAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method deletes the eDisplay.
 */

	void deleteEDisplay();

/**
 * This method deletes the fDisplay.
 */

	void deleteFDisplay();

/**
 * This method deletes the gDisplay.
 */

	void deleteGDisplay();

/**
 * This method deletes the cDisplay.
 */

	void deleteCDisplay();

/**
 * This method deletes the cDisplay.
 */

	void deleteNDisplay();

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawEDisplay(std::string drawOption = "TEXT");

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawFDisplay(std::string drawOption = "TEXT");

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawGDisplay(std::string drawOption = "TEXT");

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawCDisplay(std::string drawOption = "TEXT");

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawNDisplay(std::string drawOption = "TEXT");

/**
 * This method draws the display corresponding to the drawId.
 * @param drawId selects one of the displays (NUMBEROFGRAPHICEFGCNDISPLAYS) to be drawn
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawDisplay(unsigned short drawId, std::string drawOption = "TEXT");

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeEDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeFDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeGDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeCDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeNDisplay(int eventNumber, std::string outputFileName);

/**
 * This method returns true, if the eDisplay is enabled.
 */

	bool isEDisplayEnabled();

/**
 * This method returns true, if the gDisplay is enabled.
 */

	bool isFDisplayEnabled();

/**
 * This method returns true, if the gDisplay is enabled.
 */

	bool isGDisplayEnabled();

/**
 * This method returns true, if the cDisplay is enabled.
 */

	bool isCDisplayEnabled();

/**
 * This method returns true, if the cDisplay is enabled.
 */

	bool isNDisplayEnabled();

/**
 * This method returns true, if at least one display is	
 * enabled.
 */

	bool isDisplayEnabled();

/**
 * This method returns true, if at least one display is	
 * written to file.
 */

	bool isWritingEnabled();

/**
 * This method returns the number of all displays.
 */

	unsigned short getNumberOfDisplays();

/**
 * This method returns the signature of the enabled displays.
 * @return signature is a bool array with one at the position where the
 *         corresponding display is enabled
 */

	void getSignatureOfEnabledDisplays(bool* signature);

/**
 * gets the windowPadColumn
 */

	unsigned short getWindowPadColumn();

/**
 * sets the windowPadColumn
 * @param column is the number of the column pad of the global window to place the displays 
 */

	void setWindowPadColumn(unsigned short column);

/**
 * sets the windowPadRow
 * @param globalSignature is an array with an element for each display .
 *        An one enables the display's pad row counter and sets it
 * @param sizeOfGlobalSignature is the size of globalSignature
 */

	void setWindowPadRow(bool* globalSignature, unsigned short sizeOfGlobalSignature);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        found track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToFakeTrack(trackCoordinates& coordinates);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        found track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToNoTrack(trackCoordinates& coordinates);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        found track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToOneTrack(trackCoordinates& coordinates, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        found track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToMoreTracks(trackCoordinates& coordinates, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param coordinates is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToNoPeak(trackCoordinates& coordinates);

/**
 * This method returns the eDisplay's axi range.
 */

	projectionEFGCNDisplayInfo getEDisplayAxiRange();

/**
 * This method returns the gDisplay's axi range.
 */

	projectionEFGCNDisplayInfo getFDisplayAxiRange();

/**
 * This method returns the gDisplay's axi range.
 */

	projectionEFGCNDisplayInfo getGDisplayAxiRange();

/**
 * This method returns the cDisplay's axi range.
 */

	projectionEFGCNDisplayInfo getCDisplayAxiRange();

/**
 * This method returns the nDisplay's axi range.
 */

	projectionEFGCNDisplayInfo getNDisplayAxiRange();

/**
 * This method returns the number of eDisplay entries.
 */

	unsigned int getEDisplayEntries();

/**
 * This method returns the number of gDisplay entries.
 */

	unsigned int getFDisplayEntries();

/**
 * This method returns the number of gDisplay entries.
 */

	unsigned int getGDisplayEntries();

/**
 * This method returns the number of cDisplay entries.
 */

	unsigned int getCDisplayEntries();

/**
 * This method returns the number of cDisplay entries.
 */

	unsigned int getNDisplayEntries();

};

#endif

