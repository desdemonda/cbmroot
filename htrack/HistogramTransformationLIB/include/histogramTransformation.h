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
///     - class for evaluating the histogram
///     - class computes the tracks
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMTRANSFORMATION_H
#define _HISTOGRAMTRANSFORMATION_H


#include "../../MiscLIB/include/terminal.h"
#include "../../DataRootObjectLIB/include/histogramData.h"
#include "../../DataRootObjectLIB/include/trackData.h"
#include "../../DataRootObjectLIB/include/tables.h"


//#define PRINTORIGINALHISTOGRAMLAYERSTOFILE		/**< Definition enables the printing of all histogram layers before the encoding step into a file */
#undef PRINTORIGINALHISTOGRAMLAYERSTOFILE

//#define PRINTENCODEDHISTOGRAMLAYERSTOFILE			/**< Definition enables the printing of all histogram layers after the encoding step into a file */
#undef PRINTENCODEDHISTOGRAMLAYERSTOFILE

//#define PRINTDIAGONALIZEDHISTOGRAMLAYERSTOFILE	/**< Definition enables the printing of all histogram layers after the diagonalization step into a file */
#undef PRINTDIAGONALIZEDHISTOGRAMLAYERSTOFILE

//#define PRINTFILTEREDHISTOGRAMLAYERSTOFILE		/**< Definition enables the printing of all histogram layers after the first filter step into a file */
#undef PRINTFILTEREDHISTOGRAMLAYERSTOFILE

//#define PRINTFINALIZEDHISTOGRAMLAYERSTOFILE		/**< Definition enables the printing of all histogram layers after the finalizing step (conversion to track-objects) into a file */
#undef PRINTFINALIZEDHISTOGRAMLAYERSTOFILE

//#define PRINTPREFILTEREDTRACKLAYERSTOFILE			/**< Definition enables the printing of all tracks of all layers which are left before the second filter step into a file */
#undef PRINTPREFILTEREDTRACKLAYERSTOFILE

//#define PRINTFILTEREDTRACKLAYERSTOFILE			/**< Definition enables the printing of all tracks of all layers which are left after the second filter step into a file */
#undef PRINTFILTEREDTRACKLAYERSTOFILE


#define SKIPFIRSTHISTOGRAMCOLUMN					/**< Definition enables the skipping of the histogram's first column because there is the startup of many lines and there should not be found the track */

#define DIAGONALIZATIONSUMMAND 1					/**< This definition sets the summand which is used in the diagonalize function */


/* **************************************************************
 * CLASS histogramTransformation				 				*
 * **************************************************************/

class histogramTransformation {

 private:

  histogramTransformation(const histogramTransformation&);
  histogramTransformation& operator=(const histogramTransformation&);

protected:

	histogramData**       histogram;								/**< Object to access the histogram. */
	trackData**           tracks;									/**< Object to access the computed tracks. */
	tables**              ratings;                                  /**< Object to access the tables to do the ratings. */

/**
 * This method filters the peakclasses in a speciallized manner.
 */

	virtual void filteringHistogramLayer() = 0;

/**
 * This method filters the tracks in a speciallized manner
 * again.
 * @param terminal is a buffer to place the process information
 */

	virtual void filteringHistogram(std::streambuf* terminal = NULL) = 0;

public:

/**
 * Default constructor
 */

	histogramTransformation();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 */

	histogramTransformation(histogramData** histogram, trackData** tracks, tables** ratings);

/**
 * Default destructor
 */

	virtual ~histogramTransformation();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 */

	void init(histogramData** histogram, trackData** tracks, tables** ratings);

/**
 * This method resets the tracks object.
 */

	void resetTracks();

/**
 * This method encodes the histogram value to a special class
 * value.
 */

	void encodeHistogramLayer(unsigned short layer);

/**
 * This method handles the diagonles in the histogramPeaks
 * array to	perform a better quality with doubled peaks.
 */

	void diagonalizeHistogramLayer(unsigned short layer);

/**
 * This method filters the peakclasses in a speciallized manner.
 */

	void filterHistogramLayer(unsigned short layer);

/**
 * This method converts the filtered histogramPeaks array to
 * the correspondance to be equal to the eraser-object result.
 */

	void serializeHistogramLayer(unsigned short layer);

/**
 * This method filters the tracks in a speciallized manner
 * again.
 * @param terminal is a buffer to place the process information
 */

	void filterHistogram(std::streambuf* terminal = NULL);

};

#endif
