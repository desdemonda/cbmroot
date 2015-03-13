/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - implements methods to show a total analysis summary
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-06-06 14:18:40 $
// $Revision: 1.4 $
//
// *******************************************************************/


#ifndef _TOTALANALYSIS_H
#define _TOTALANALYSIS_H


#include "../../DataObjectLIB/include/table.h"
#include "totalAnalysisDef.h"


/* **************************************************************
 * CLASS totalAnalysis					 						*
 * **************************************************************/

class totalAnalysis {

protected:

	unsigned long numberOfCorrections;						/**< Variable stores the number of corrections of the Hough transform, if at least one coordinate in is corrected. */
	unsigned long numberOfCoordCorrections;					/**< Variable stores the number of all coordinate corrections of the Hough transform. */
	unsigned long numberOfTracksWhichCannotBeFound;			/**< Variable stores the number of tracks which cannot be found because of the too small quatization of the Hough space. */
	unsigned long numberOfTracksWithGoodPrelutSignature;	/**< Variable stores the number of signatures generated with the prelut which are accepted. */
	unsigned long numberOfTracksWithGoodLutSignature;		/**< Variable stores the number of signatures generated with the lut which are accepted. */
	table         prelutSignatures;							/**< Object stores the good prelut signatures which are accepted. */
	table         lutSignatures;							/**< Object stores the good lut signatures which are accepted. */

public:

/**
 * Default constructor
 */

	totalAnalysis();

/**
 * Destructor
 */

	virtual ~totalAnalysis();

/**
 * This method resets the whole object.
 */

	void reset();

/**
 * This method returns the number of corrections.
 */

	unsigned long getNumberOfCorrections();

/**
 * This method returns the number of coord corrections.
 */

	unsigned long getNumberOfCoordCorrections();

/**
 * This method returns the number of tracks which cannot be
 * found.
 */

	unsigned long getNumberOfTracksWhichCannotBeFound();

/**
 * This method returns the number of tracks which have a
 * good prelut signature.
 */

	unsigned long getNumberOfTracksWithGoodPrelutSignature();

/**
 * This method returns the number of tracks which have a
 * good lut signature.
 */

	unsigned long getNumberOfTracksWithGoodLutSignature();

/**
 * This method returns the number of
 * good prelut signatures.
 */

	unsigned long getNumberOfGoodPrelutSignatures();

/**
 * This method returns the table containing the
 * good prelut signatures.
 */

	table& getGoodPrelutSignatureTable();

/**
 * This method returns the number of
 * good lut signatures.
 */

	unsigned long getNumberOfGoodLutSignatures();

/**
 * This method returns the table containing the
 * good lut signatures.
 */

	table& getGoodLutSignatureTable();

/**
 * This method sets the number of corrections.
 */

	void setNumberOfCorrections(unsigned long value);

/**
 * This method sets the number of coord corrections.
 */

	void setNumberOfCoordCorrections(unsigned long value);

/**
 * This method sets the number of tracks which cannot be found.
 */

	void setNumberOfTracksWhichCannotBeFound(unsigned long value);

/**
 * This method sets the number of tracks which have a
 * good prelut signature.
 */

	void setNumberOfTracksWithGoodPrelutSignature(unsigned long value);

/**
 * This method sets the number of tracks which have a
 * good lut signature.
 */

	void setNumberOfTracksWithGoodLutSignature(unsigned long value);

/**
 * This method adds the number of corrections.
 */

	void addNumberOfCorrections(unsigned long value);

/**
 * This method adds the number of coord corrections.
 */

	void addNumberOfCoordCorrections(unsigned long value);

/**
 * This method adds the number of tracks which cannot be found.
 */

	void addNumberOfTracksWhichCannotBeFound(unsigned long value);

/**
 * This method adds the number of tracks which have a
 * good prelut signature.
 */

	void addNumberOfTracksWithGoodPrelutSignature(unsigned long value);

/**
 * This method adds the number of tracks which have a
 * good lut signature.
 */

	void addNumberOfTracksWithGoodLutSignature(unsigned long value);

/**
 * This method adds a good prelut signature.
 */

	void addGoodPrelutSignature(bitArray signature, bitArray classification);

/**
 * This method adds a good lut signature.
 */

	void addGoodLutSignature(bitArray signature, bitArray classification);

/**
 * This method resets both correction counters.
 */

	void resetCorrectionCounter();

/**
 * This method resets the number of tracks which cannot be found.
 */

	void resetNumberOfTracksWhichCannotBeFound();

/**
 * This method resets the number of tracks which have a
 * good prelut signature.
 */

	void resetNumberOfTracksWithGoodPrelutSignature();

/**
 * This method resets the number of tracks which have a
 * good lut signature.
 */

	void resetNumberOfTracksWithGoodLutSignature();

/**
 * This method resets the good prelut signatures.
 */

	void resetGoodPrelutSignatures();

/**
 * This method resets the good lut signatures.
 */

	void resetGoodLutSignatures();

/**
 * This method prints both correction counters.
 */

	void printCorrectionCounter();

/**
 * This method prints the number of tracks which cannot be found.
 */

	void printNumberOfTracksWhichCannotBeFound();

/**
 * This method prints the number of tracks which have a
 * good prelut signature.
 */

	void printNumberOfTracksWithGoodPrelutSignature();

/**
 * This method prints the number of tracks which have a
 * good lut signature.
 */

	void printNumberOfTracksWithGoodLutSignature();

/**
 * This method prints the number of good prelut signatures.
 */

	void printNumberOfGoodPrelutSignatures();

/**
 * This method prints the number of good lut signatures.
 */

	void printNumberOfGoodLutSignatures();

/**
 * This method prints the good prelut signatures.
 */

	void printGoodPrelutSignatures();

/**
 * This method prints the good lut signatures.
 */

	void printGoodLutSignatures();

};

#endif
