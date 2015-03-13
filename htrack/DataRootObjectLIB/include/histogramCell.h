//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
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
///     - base class for handling the created borders of one layer
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMCELL_H
#define _HISTOGRAMCELL_H


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../../MiscLIB/include/hitArray.h"


/* **************************************************************
 * CLASS histogramCell							 				*
 * **************************************************************/

class histogramCell {

public:

	bitArray value;		/**< Object to store the value of the peak computed for this cell. */ 

#ifndef NOANALYSIS

	hitArray hits;		/**< Object to store all hits computed to belong to this cell. */

#endif

/**
 * Default constructor
 */

	histogramCell();

/**
 * Constructor
 */

	histogramCell(const histogramCell& value);

/**
 * Destructor
 */

	virtual ~histogramCell();

/**
 * operator = ()
 */

	const histogramCell& operator = (const histogramCell& value);

/**
 * Method resets the object.
 */

	void reset();

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHistogramData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHistogramData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHistogramData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHistogramSignatureData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHistogramSignatureData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHistogramSignatureData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHistogramHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHistogramHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHistogramHitData(unsigned short dimension = 0);

};

#endif
