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
///     - implements methods to display the track propagation's quality
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2009-01-13 11:33:51 $
/// $Revision: 1.0 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKPROPAGATIONDANALYSIS_H
#define _TRACKPROPAGATIONDANALYSIS_H


#include "TH1D.h"
#include <string>


/**
 * struct stores the basic information for one display
 */
struct trackPropagationDisplayInfo {

trackPropagationDisplayInfo() : min(0), max(0), numberOfBins(), numberOfUnderflows(0), numberOfOverflows(0), baseName(), baseTitle(), subDirName(), windowPadRow(0) {};
	int            min;					/**< Variable stores the minimal value in x-dimension */
	int            max;					/**< Variable stores the maximal value in x-dimension */
	int            numberOfBins;		/**< Variable stores the steps of the grid in x-dimension */
	unsigned short numberOfUnderflows;	/**< Variable stores the number of inputs which are smaller than the minimal value in x-dimension */
	unsigned short numberOfOverflows;	/**< Variable stores the number of inputs which are bigger than the maximal value in x-dimension */
	std::string    baseName;			/**< Variable stores the basic name for the display which is constant over all events */
	std::string    baseTitle;			/**< Variable stores the basic title for the display which is constant over all events */
	std::string    subDirName;			/**< Variable stores the name of the subdirectory to store the analysis results */
	unsigned short windowPadRow;		/**< Variable to store the number of the row pad of the global window to place the track propagation display */

};


/* **************************************************************
 * CLASS trackPropagationDAnalysis		 						*
 * **************************************************************/

class trackPropagationDAnalysis {

 private:
  trackPropagationDAnalysis(const trackPropagationDAnalysis&);
  trackPropagationDAnalysis& operator=(const trackPropagationDAnalysis&);

protected:

	unsigned short                numberOfDisplays;          /**< Variable to store the number of displays */
	trackPropagationDisplayInfo** distanceDisplaysInfo;		/**< Struct for the track propagation display information */
	TH1D**                        distanceDisplays;			/**< Object for the track propagation display */
	unsigned short                windowPadColumn;			/**< Variable to store the number of the column pad of the global window to place the displays */
	bool                          writeToFile;				/**< Variable to store if writing into the file is enabled */

/**
 * resets the displays
 * @param index represents the order index of the display which should be identified
 */

	void resetDisplay(unsigned short index);
	void resetDisplays();

/**
 * This method writes a display into the file.
 * @param display is the display to save
 * @param info is the information-struct of the display to save
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeDisplay(TH1D* display, trackPropagationDisplayInfo* info, int eventNumber, std::string outputFileName);

/**
 * resets the info structs for event and nothing for total
 */

	virtual void reset() = 0;

/**
 * returns the corresponding string for the name identification.
 * @param index represents the order index of the display which should be identified
 */

	virtual std::string getNameOfDistanceDisplay(unsigned short index) = 0;

/**
 * returns the corresponding string for the title.
 * @param index represents the order index of the display which should be identified
 */

	virtual std::string getTitleOfDistanceDisplay(unsigned short index) = 0;

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

	trackPropagationDAnalysis();

/**
 * Destructor
 */

	virtual ~trackPropagationDAnalysis();

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * This method initializes the distanceDisplay.
 * @param index represents the order index of the display which should be identified. If index is not existing a new one is added with the end index.
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initDistanceDisplay(unsigned short index = (unsigned short)-1, int nBins = 100, int xMin = 0, int xMax = 10);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initTrackPropagationAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method deletes the distanceDisplays.
 * @param index represents the order index of the display which should be identified
 */

	void deleteDistanceDisplay(unsigned short index);
	void deleteDistanceDisplays();

/**
 * This method draws the display.
 * @param index represents the order index of the display which should be identified
 * @param option represents the look of the display's content
 */

	void drawDistanceDisplay(unsigned short index, unsigned int option = 0);

/**
 * This method draws the display corresponding to the drawId.
 * @param drawId selects one of the displays (NUMBEROFGRAPHICEFGCDISPLAYS) to be drawn
 * @param option represents the look of the display's content
 */

	void drawDisplay(unsigned short drawId, unsigned int option = 0);

/**
 * This method writes the data into a file.
 * @param index represents the order index of the display which should be identified
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeDistanceDisplay(unsigned short index, int eventNumber, std::string outputFileName);
	void writeDistanceDisplays(int eventNumber, std::string outputFileName);

/**
 * This method returns true, if the emDisplay is enabled.
 * @param index represents the order index of the display which should be identified
 */

	bool isDistanceDisplayEnabled(unsigned short index);

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
 * @param localSignature is an array with an element for each display.
 *        An one enables the display's pad sets it
 * @param sizeOfLocalSignature is the size of localSignature
 * @param globalSignature is an array with an element for each display.
 *        An one enables the display's pad row counter
 * @param sizeOfGlobalSignature is the size of globalSignature
 */

	void setWindowPadRow(bool* localSignature, unsigned short sizeOfLocalSignature, bool* globalSignature, unsigned short sizeOfGlobalSignature);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param index represents the order index of the display which should be identified
 * @param value is the value which should be inserted
 */

	void addDistance(unsigned short index, double value);

/**
 * This method returns the emDisplay's axi range.
 * @param index represents the order index of the display which should be identified
 */

	trackPropagationDisplayInfo getDistanceDisplayAxiRange(unsigned short index);

/**
 * This method returns the number of emDisplay entries.
 * @param index represents the order index of the display which should be identified
 */

	unsigned int getDistanceDisplayEntries(unsigned short index);

};

#endif
