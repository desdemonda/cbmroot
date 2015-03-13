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
///     - implements methods to display the trackfinding algorithm's quality
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:24 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MOMENTUMEFGCANALYSIS_H
#define _MOMENTUMEFGCANALYSIS_H


#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include "../../DataObjectLIB/include/analyticFormula.h"
#include "TProfile.h"
#include <string>


/**
 * This defines the number of displays for this class.
 * In this case, there is one display for:
 * - the efficiency
 * - the fakes
 * - the ghosts
 * - the clones
 * So there are four displays.
 */
#define NUMBEROFGRAPHICEFGCDISPLAYS 4


/**
 * struct stores the basic information for one display
 */
struct momentumEFGCDisplayInfo {
momentumEFGCDisplayInfo() : min(0), max(0), numberOfBins(0), numberOfUnderflows(0), numberOfOverflows(0), baseName(), baseTitle(), subDirName() {};
	int            min;					/**< Variable stores the minimal value in x-dimension */
	int            max;					/**< Variable stores the maximal value in x-dimension */
	int            numberOfBins;		/**< Variable stores the steps of the grid in x-dimension */
	unsigned short numberOfUnderflows;	/**< Variable stores the number of inputs which are smaller than the minimal value in x-dimension */
	unsigned short numberOfOverflows;	/**< Variable stores the number of inputs which are bigger than the maximal value in x-dimension */
	std::string    baseName;			/**< Variable stores the basic name for the display which is constant over all events */
	std::string    baseTitle;			/**< Variable stores the basic title for the display which is constant over all events */
	std::string    subDirName;			/**< Variable stores the name of the subdirectory to store the analysis results */

};


/* **************************************************************
 * CLASS momentumEFGCAnalysis			 						*
 * **************************************************************/

class momentumEFGCAnalysis {

 private:
  momentumEFGCAnalysis(const momentumEFGCAnalysis&);
  momentumEFGCAnalysis& operator=(const momentumEFGCAnalysis&);

protected:

	TProfile*               emDisplay;					/**< Object for the efficiency display */
	momentumEFGCDisplayInfo emGraphicEFGCDisplayInfo;	/**< Struct for the efficiency display information */
	TProfile*               fmDisplay;					/**< Object for the fakes display */
	momentumEFGCDisplayInfo fmGraphicEFGCDisplayInfo;	/**< Struct for the fakes display information */
	TProfile*               gmDisplay;					/**< Object for the ghosts display */
	momentumEFGCDisplayInfo gmGraphicEFGCDisplayInfo;	/**< Struct for the ghosts display information */
	TProfile*               cmDisplay;					/**< Object for the clones display */
	momentumEFGCDisplayInfo cmGraphicEFGCDisplayInfo;	/**< Struct for the clones display information */
	analyticFormula         formula;					/**< Object to compute the actual momentum */
	unsigned short          windowPadColumn;			/**< Variable to store the number of the column pad of the global window to place the displays */
	unsigned short          emWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the efficiency display */
	unsigned short          fmWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the fakes display */
	unsigned short          gmWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the ghosts display */
	unsigned short          cmWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the clones display */
	bool                    writeToFile;				/**< Variable to store if writing into the file is enabled */

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

	void writeDisplay(TProfile* display, momentumEFGCDisplayInfo* info, int eventNumber, std::string outputFileName);

/**
 * resets the info structs for event and nothing for total
 */

	virtual void reset() = 0;

/**
 * returns the corresponding impulse for each display entry.
 */

	virtual double getEntryOfXAxi(trackMomentum& momentum) = 0;

/**
 * returns the corresponding string, which is drawn on that axi,
 * where the impulse is shown.
 */

	virtual const char* getTitleOfXAxi() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfEMDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfEMDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfFMDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfFMDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfGMDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfGMDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfCMDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfCMDisplay() = 0;

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

	momentumEFGCAnalysis();

/**
 * Destructor
 */

	virtual ~momentumEFGCAnalysis();

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * This method initializes the emDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initEMDisplay(int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * This method initializes the fmDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initFMDisplay(int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * This method initializes the gmDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initGMDisplay(int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * This method initializes the cmDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param xMin sets the minimal value in x-dimension for this ROOT-histogram
 * @param xMax sets the maximal value in x-dimension for this ROOT-histogram
 */

	void initCMDisplay(int nBins = 100, int xMin = 0, int xMax = 25);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initMomentumAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method deletes the emDisplay.
 */

	void deleteEMDisplay();

/**
 * This method deletes the fmDisplay.
 */

	void deleteFMDisplay();

/**
 * This method deletes the gmDisplay.
 */

	void deleteGMDisplay();

/**
 * This method deletes the cmDisplay.
 */

	void deleteCMDisplay();

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawEMDisplay(unsigned int error = 1);

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawFMDisplay(unsigned int error = 1);

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawGMDisplay(unsigned int error = 1);

/**
 * This method draws the display.
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawCMDisplay(unsigned int error = 1);

/**
 * This method draws the display corresponding to the drawId.
 * @param drawId selects one of the displays (NUMBEROFGRAPHICEFGCDISPLAYS) to be drawn
 * @param error is 0, if no error should be drawn, else the default error would be drawn
 */

	void drawDisplay(unsigned short drawId, unsigned int error = 1);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeEMDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeFMDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeGMDisplay(int eventNumber, std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param eventNumber is the number of the event (Needed to identify each display eventwise)
 * @param outputFileName is the name of the file to place the output
 */

	void writeCMDisplay(int eventNumber, std::string outputFileName);

/**
 * This method returns true, if the emDisplay is enabled.
 */

	bool isEMDisplayEnabled();

/**
 * This method returns true, if the gmDisplay is enabled.
 */

	bool isFMDisplayEnabled();

/**
 * This method returns true, if the gmDisplay is enabled.
 */

	bool isGMDisplayEnabled();

/**
 * This method returns true, if the cmDisplay is enabled.
 */

	bool isCMDisplayEnabled();

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
 * @param parameter is the actual processing parameter of the
 *        found track
 * @param space is the actual object containing the Hough dimensions
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToFakeTrack(trackParameter& parameter, histogramSpace& space);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param parameter is the actual processing parameter of the
 *        found track
 * @param space is the actual object containing the Hough dimensions
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToNoTrack(trackParameter& parameter, histogramSpace& space);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param parameter is the actual processing parameter of the
 *        found track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @param space is the actual object containing the Hough dimensions
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToOneTrack(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param parameter is the actual processing parameter of the
 *        found track
 * @param isClone is true if the track is a clone
 * @param isWrong is true if the track is wrong
 * @param space is the actual object containing the Hough dimensions
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void peakToMoreTracks(trackParameter& parameter, histogramSpace& space, bool isClone, bool isWrong);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToNoPeak(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToOnePeak(trackfinderInputTrack& actualTrack);

/**
 * Function wich is called from evaluate in a special
 * switch case.
 * @param actualTrack is the actual processing information of the
 *        input track
 * @see analysis::evaluate(std::streambuf* terminal = NULL)
 */

	void trackToMorePeaks(trackfinderInputTrack& actualTrack);

/**
 * This method returns the emDisplay's axi range.
 */

	momentumEFGCDisplayInfo getEMDisplayAxiRange();

/**
 * This method returns the gmDisplay's axi range.
 */

	momentumEFGCDisplayInfo getFMDisplayAxiRange();

/**
 * This method returns the gmDisplay's axi range.
 */

	momentumEFGCDisplayInfo getGMDisplayAxiRange();

/**
 * This method returns the cmDisplay's axi range.
 */

	momentumEFGCDisplayInfo getCMDisplayAxiRange();

/**
 * This method returns the number of emDisplay entries.
 */

	unsigned int getEMDisplayEntries();

/**
 * This method returns the number of gmDisplay entries.
 */

	unsigned int getFMDisplayEntries();

/**
 * This method returns the number of gmDisplay entries.
 */

	unsigned int getGMDisplayEntries();

/**
 * This method returns the number of cmDisplay entries.
 */

	unsigned int getCMDisplayEntries();

};

#endif

