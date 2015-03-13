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
///     - implements methods to display the magnetfield analysis
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:29:24 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAGNETFIELDDIRECTIONANALYSIS_H
#define _MAGNETFIELDDIRECTIONANALYSIS_H


#include "TGraph.h"
#include <string>


/**
 * This defines the number of displays for this class.
 * In this case, there is one display for:
 * - the B_x
 * - the B_y
 * - the B_z
 * So there are three displays.
 */
#define NUMBEROFMAGNETFIELDDISPLAYS 3


/**
 * struct stores the basic information for one display
 */
struct magnetfieldDisplayInfo {

magnetfieldDisplayInfo() : constValDim1(0.), constValDim2(0.), min(0.), max(0.),  numberOfUnderflows(0), numberOfOverflows(0), baseName(), baseTitle(), subDirName() {};

	double         constValDim1;		/**< Variable stores the constant value in the first constant dimension */
	double         constValDim2;		/**< Variable stores the constant value in the second constant dimension */
	double         min;					/**< Variable stores the minimal value in x-dimension */
	double         max;					/**< Variable stores the maximal value in x-dimension */
	unsigned short numberOfUnderflows;	/**< Variable stores the number of inputs which are smaller than the minimal value in x-dimension */
	unsigned short numberOfOverflows;	/**< Variable stores the number of inputs which are bigger than the maximal value in x-dimension */
	std::string    baseName;			/**< Variable stores the basic name for the display which is constant over all events */
	std::string    baseTitle;			/**< Variable stores the basic title for the display which is constant over all events */
	std::string    subDirName;			/**< Variable stores the name of the subdirectory to store the analysis results */

};


/* **************************************************************
 * CLASS magnetfieldDirectionAnalysis	 						*
 * **************************************************************/

class magnetfieldDirectionAnalysis {

 private:
  magnetfieldDirectionAnalysis(const magnetfieldDirectionAnalysis&);
  magnetfieldDirectionAnalysis& operator=(const magnetfieldDirectionAnalysis&);

protected:

	TGraph*                   bxDisplay;					/**< Object for the B_x display */
	magnetfieldDisplayInfo    bxMagnetfieldDisplayInfo;		/**< Struct for the B_x display information */
	TGraph*                   byDisplay;					/**< Object for the B_y display */
	magnetfieldDisplayInfo    byMagnetfieldDisplayInfo;		/**< Struct for the B_y display information */
	TGraph*                   bzDisplay;					/**< Object for the B_z display */
	magnetfieldDisplayInfo    bzMagnetfieldDisplayInfo;		/**< Struct for the B_z display information */
	unsigned short            windowPadColumn;				/**< Variable to store the number of the column pad of the global window to place the displays */
	unsigned short            bxWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the B_x display */
	unsigned short            byWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the B_y display */
	unsigned short            bzWindowPadRow;				/**< Variable to store the number of the row pad of the global window to place the B_z display */
	bool                      writeToFile;					/**< Variable to store if writing into the file is enabled */

/**
 * This method sets the range of the axis for each display.
 */

	void setAutomaticAxisRangeOfDisplay(TGraph* display);

/**
 * resets the info structs
 */

	void reset();

/**
 * This method writes a display into the file.
 * @param display is the display to save
 * @param info is the information-struct of the display to save
 * @param outputFileName is the name of the file to place the output
 */

	void writeDisplay(TGraph* display, magnetfieldDisplayInfo* info, std::string outputFileName);

/**
 * returns the corresponding string, which is drawn on that axi,
 * where the impulse is shown.
 */

	virtual const char* getTitleOfXAxi() = 0;

/**
 * returns the corresponding modifier for the first constant
 * dimension.
 */

	virtual const char* getDim1Modifier() = 0;

/**
 * returns the corresponding modifier for the second constant
 * dimension.
 */

	virtual const char* getDim2Modifier() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfBxDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfBxDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfByDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfByDisplay() = 0;

/**
 * returns the corresponding string for the name identification.
 */

	virtual const char* getNameOfBzDisplay() = 0;

/**
 * returns the corresponding string for the title.
 */

	virtual const char* getTitleOfBzDisplay() = 0;

/**
 * returns true, if the saving should be in subdirectories.
 */

	virtual bool getSavingInSubDirectory() = 0;

/**
 * returns the corresponding string for the saving subdirectory.
 */

	virtual const char* getNameOfSavingSubDirectory() = 0;

public:

/**
 * Default constructor
 */

	magnetfieldDirectionAnalysis();

/**
 * Destructor
 */

	virtual ~magnetfieldDirectionAnalysis();

/**
 * method initializes the evaluation.
 */

	void initializeEvaluation();

/**
 * method finalizes the evaluation.
 */

	void finalizeEvaluation();

/**
 * This method initializes the bxDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initBxDisplay(int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * This method initializes the byDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initByDisplay(int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * This method initializes the bzDisplay.
 * @param nBins sets the binning for this ROOT-histogram
 * @param min sets the minimal value in variable dimension for this ROOT-histogram
 * @param max sets the maximal value in variable dimension for this ROOT-histogram
 * @param constValDim1 sets the constant value in the first constant dimension for this ROOT-histogram
 * @param constValDim2 sets the constant value in the second constant dimension for this ROOT-histogram
 */

	void initBzDisplay(int nBins = 100, double min = 0, double max = 100, double constValDim1 = 0, double constValDim2 = 0);

/**
 * This method initializes the root directory for the displays.
 * @param enable enables or disables this analysis
 * @param name is the name of the file to place the graphics
 */

	void initMagnetfieldAnalysisToRoot(bool enable = true, const char* name = NULL);

/**
 * This method deletes the bxDisplay.
 */

	void deleteBxDisplay();

/**
 * This method deletes the byDisplay.
 */

	void deleteByDisplay();

/**
 * This method deletes the bzDisplay.
 */

	void deleteBzDisplay();

/**
 * This method sets the values for x and y at the given point
 * at position index in the bxDisplay.
 */

	void setBxPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the byDisplay.
 */

	void setByPoint(int index, double xValue, double yValue);

/**
 * This method sets the values for x and y at the given point
 * at position index in the bzDisplay.
 */

	void setBzPoint(int index, double xValue, double yValue);

/**
 * This method sets the range of the axis for each display.
 */

	void setAutomaticBxAxisRange();

/**
 * This method sets the range of the axis for each display.
 */

	void setAutomaticByAxisRange();

/**
 * This method sets the range of the axis for each display.
 */

	void setAutomaticBzAxisRange();

/**
 * This method sets the range of the axis for each display.
 */

	void setAutomaticAxisRange();

/**
 * This method draws the display.
 */

	void drawBxDisplay();

/**
 * This method draws the display.
 */

	void drawByDisplay();

/**
 * This method draws the display.
 */

	void drawBzDisplay();

/**
 * This method draws the display corresponding to the drawId.
 * @param drawId selects one of the displays (NUMBEROFMAGNETFIELDDISPLAYS) to be drawn
 */

	void drawDisplay(unsigned short drawId);

/**
 * This method writes the data into a file.
 * @param outputFileName is the name of the file to place the output
 */

	void writeBxDisplay(std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param outputFileName is the name of the file to place the output
 */

	void writeByDisplay(std::string outputFileName);

/**
 * This method writes the data into a file.
 * @param outputFileName is the name of the file to place the output
 */

	void writeBzDisplay(std::string outputFileName);

/**
 * This method returns true, if the bxDisplay is enabled.
 */

	bool isBxDisplayEnabled();

/**
 * This method returns true, if the byDisplay is enabled.
 */

	bool isByDisplayEnabled();

/**
 * This method returns true, if the bzDisplay is enabled.
 */

	bool isBzDisplayEnabled();

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
 * This method returns the bxDisplay's axi range.
 */

	magnetfieldDisplayInfo& getBxDisplayAxiRange();

/**
 * This method returns the byDisplay's axi range.
 */

	magnetfieldDisplayInfo& getByDisplayAxiRange();

/**
 * This method returns the bzDisplay's axi range.
 */

	magnetfieldDisplayInfo& getBzDisplayAxiRange();

/**
 * This method returns the number of bxDisplay entries.
 */

	unsigned int getBxDisplayEntries();

/**
 * This method returns the number of byDisplay entries.
 */

	unsigned int getByDisplayEntries();

/**
 * This method returns the number of bzDisplay entries.
 */

	unsigned int getBzDisplayEntries();

};

#endif
