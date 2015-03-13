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
///     - class for the implementation of both look-up-tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 14:01:49 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTIMPLEMENTATION_H
#define _LUTIMPLEMENTATION_H


#include "../../DataRootObjectLIB/include/rungeKuttaListEntry.h"
#include "../../DataRootObjectLIB/include/rungeKuttaInvertedListEntry.h"
#include "../../DataObjectLIB/include/specialList.h"
#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataRootObjectLIB/include/lutBorder.h"
#include "prelut.h"
#include "lut.h"


/**
 * Defines all possible versions for the look-up-tables
 */

enum {NOLUT, ANALYTICFORMULALUT, RUNGEKUTTAFILELUT, ANALYTICFORMULAFILELUT, FILELUT};


/**
 * struct consists of the needed
 * information for the type of the
 * look-up-table
 */

typedef struct {

	prelut*        lutObject;			/**< Object to implement the first look-up-table of type prelut. */
	unsigned short type;				/**< Variable to store the type of the object. */

} identifiedPrelut;

typedef struct {

	lut*           lutObject;			/**< Object to implement the second look-up-table of type lut. */
	unsigned short type;				/**< Variable to store the type of the object. */

} identifiedLut;


/* **************************************************************
 * CLASS lutImplementation						 				*
 * **************************************************************/

class lutImplementation {


 private:
  lutImplementation(const lutImplementation&);
  lutImplementation& operator=(const lutImplementation&);

protected:

	histogramSpace** space;				/**< Object to store the needed values to compute the trackAnalogInformation object based on the trackDigitalInformation object. */
	bool             isSpaceLocal;		/**< Variable to store if the space object is allocated locally or not. */
	identifiedPrelut firstLut;			/**< Object to implement the first look-up-table of type prelut in combination with the saving of the type. */
	identifiedLut    secondLut;			/**< Object to implement the second look-up-table of type lut in combination with the saving of the type. */

/**
 * deletes allocated and initialized memory
 * @param first is an object for the first look-up-table
 * @param second is an object for the second look-up-table
 */

	void deleteSpace();
	void deleteLuts(identifiedPrelut* first, identifiedLut* second);
	void deleteMemory();

/**
 * allocates memory for the histogramSpace object
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param space is the size of the histogram in the first three dimensions
 */

	void allocateSpace(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step);
	void allocateSpace(histogramSpace** space);

/**
 * allocates memory for the look-up-table object
 * @param first is an object for the first look-up-table
 * @param second is an object for the second look-up-table
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param magneticField is the object representing the actually used magnetic field
 */

	void allocateMathLuts(identifiedPrelut* first, identifiedLut* second, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField = NULL);
	void allocateFileLuts(identifiedPrelut* first, identifiedLut* second, double dim3StartEntry, double dim3StopEntry);

/**
 * Method reads the file look-up-tables.
 * @param prelutFileName is the name of the file for the prelut look-up-table
 * @param lutFileName is the name of the file for the lut look-up-table
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param terminal is a buffer to place the process information
 */

	void readFileLuts(std::string prelutFileName, std::string lutFileName, double dim3StartEntry, double dim3StopEntry, std::streambuf* terminal = NULL);

/**
 * Method generates the analytic formula file look-up-tables.
 * @param prelutFileName is the name of the file for the prelut look-up-table
 * @param prelutName is the name of the prelut look-up-table
 * @param lutFileName is the name of the file for the lut look-up-table
 * @param lutName is the name of the lut look-up-table
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param magneticField is the object representing the actually used magnetic field
 * @param terminal is a buffer to place the process information
 */

	void generateAnalyticFormulaFileLuts(std::string prelutFileName, std::string prelutName, std::string lutFileName, std::string lutName, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField = NULL, std::streambuf* terminal = NULL);

/**
 * Method generates the Runge-Kutta list.
 * @param rungeKuttaList is the object which is filled by this function with entries like: TrackCoordinate(1, 2, 3) -> digitalHit1, digitalHit15, digitalHit37, ...
 * @param magneticField is the object representing the actually used magnetic field
 * @param terminal is a buffer to place the process information
 */

	void generateRungeKuttaList(specialList<rungeKuttaListEntry, true, true, true, true, true>* rungeKuttaList, trackfinderInputDetector* detector, std::streambuf* terminal = NULL);

/**
 * Method generates the inverted Runge-Kutta list.
 * @param rungeKuttaList is the object which is the input to this function with entries like: TrackCoordinate(1, 2, 3) -> digitalHit1, digitalHit15, digitalHit37, ...
 * @param invertedRungeKuttaList is the object which is filled by this function with entries like: digitalHit15 -> TrackCoordinate(1, 2, 3), TrackCoordinate(14, 21, 33), ...
 * @param terminal is a buffer to place the process information
 */

	void generateRungeKuttaInvertedList(specialList<rungeKuttaListEntry, true, true, true, true, true>* rungeKuttaList, specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal = NULL);

/**
 * Method interpolates the inverted Runge-Kutta list.
 * @param invertedRungeKuttaList is the object which is filled by this function with entries like: digitalHit15 -> TrackCoordinate(1, 2, 3), TrackCoordinate(14, 21, 33), ...
 * @param terminal is a buffer to place the process information
 */

	void interpolateRungeKuttaInvertedList(specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal = NULL);

/**
 * Method makes the inverted Runge-Kutta list conform to the actual LUTVERSION.
 * @param invertedRungeKuttaList is the object which is filled by this function with entries like: digitalHit15 -> TrackCoordinate(1, 2, 3), TrackCoordinate(14, 21, 33), ...
 * @param terminal is a buffer to place the process information
 */

	void makeLutversionConformRungeKuttaInvertedList(specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal = NULL);

/**
 * Method decomposes the inverted Runge-Kutta list.
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param invertedRungeKuttaList is the object which is filled by this function with entries like: digitalHit15 -> TrackCoordinate(1, 2, 3), TrackCoordinate(14, 21, 33), ...
 * @param terminal is a buffer to place the process information
 */

	void decomposeRungeKuttaInvertedList(double dim3StartEntry, double dim3StopEntry, specialList<rungeKuttaInvertedListEntry, true, true, true, true, true>* rungeKuttaInvertedList, std::streambuf* terminal = NULL);

/**
 * Method generates the Runge-Kutta file look-up-tables.
 * @param prelutFileName is the name of the file for the prelut look-up-table
 * @param prelutName is the name of the prelut look-up-table
 * @param lutFileName is the name of the file for the lut look-up-table
 * @param lutName is the name of the lut look-up-table
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param terminal is a buffer to place the process information
 */

	void generateRungeKuttaFileLuts(std::string prelutFileName, std::string prelutName, std::string lutFileName, std::string lutName, double dim3StartEntry, double dim3StopEntry, trackfinderInputDetector* detector, std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	lutImplementation();

/**
 * Destructor
 */

	virtual ~lutImplementation();

/**
 * Method initializes the object.
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 * @param space is the size of the histogram in the first three dimensions
 * @param prelutFileName is the name of the file for the prelut look-up-table
 * @param lutFileName is the name of the file for the lut look-up-table
 * @param magneticField is the object representing the actually used magnetic field
 * @param terminal is a buffer to place the process information
 */

	void initAnalyticFormulaLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField = NULL);
	void initAnalyticFormulaLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, trackfinderInputMagneticField* magneticField = NULL);
	void initFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal = NULL);
	void initFileLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, std::streambuf* terminal = NULL);
	void initAnalyticFormulaFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField = NULL, std::streambuf* terminal = NULL);
	void initAnalyticFormulaFileLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputMagneticField* magneticField = NULL, std::streambuf* terminal = NULL);
	void initRungeKuttaFileLuts(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step, double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputDetector* detector, std::streambuf* terminal = NULL);
	void initRungeKuttaFileLuts(histogramSpace** space, double dim3StartEntry, double dim3StopEntry, std::string prelutFileName, std::string lutFileName, trackfinderInputDetector* detector, std::streambuf* terminal = NULL);

/**
 * This method returns true if the magnetism
 * functions can be used with the actual LUT type.
 */

	bool typeUsesMagnetism();

/**
 * This method returns true if the correction
 * functions can be used with the actual LUT type.
 */

	bool typeUsesCorrections();

/**
 * This method returns the double pointer to the
 * histogramSpace object.
 */

	histogramSpace** getSpacePointer();

/**
 * This method returns the Definition of the prelut object.
 */

	prelutDefinition getPrelutDefinition();

/**
 * This method returns the Definition of the lut object.
 */

	lutDefinition getLutDefinition();

/**
 * This method returns the prelut border.
 */

	prelutHoughBorder getPrelutBorder();

/**
 * This method returns the lut border.
 */

	lutHoughBorder getLutBorder();

/**
 * This method returns both borders.
 */

	lutBorder getBorder();

/**
 * This method clears the border.
 */

	void clear(lutHoughBorder* borderPointer = NULL);

/**
 * This method sets the prelut range to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void setPrelutRange(double rangeStart, double rangeStop);

/**
 * This method returns the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	trackfinderInputMagneticField* getMagneticField();

/**
 * This method returns the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	double getMagneticFieldFactor();

/**
 * This method sets the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void setMagneticField(trackfinderInputMagneticField* magneticField);

/**
 * This method sets the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void setMagneticFieldFactor(double magneticFieldFactor);

/**
 * This method returns the number of corrections.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	unsigned long getNumberOfCorrections();

/**
 * This method returns the number of coord corrections.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	unsigned long getNumberOfCoordCorrections();

/**
 * This method resets both correction counters.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	void resetCorrectionCounter();

/**
 * This method evaluates the prelut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluatePrelut(trackfinderInputHit* hit, prelutHoughBorder* borderPointer);

/**
 * This method evaluates the lut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluateLut(trackfinderInputHit* hit, lutHoughBorder* borderPointer);

/**
 * This method evaluates both tables.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

	void evaluate(trackfinderInputHit* hit, lutBorder* borderPointer);

};

#endif
