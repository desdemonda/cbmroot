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
///     - base class for holding the information from the look-up-tables
///       for one hit
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTBORDER_H
#define _LUTBORDER_H


#include "../../DataRootObjectLIB/include/trackLayerAccess.h"
#include "../../DataObjectLIB/include/prelutHoughBorder.h"
#include "../../DataObjectLIB/include/lutHoughBorder.h"
#include "trackfinderInputHit.h"


/* **************************************************************
 * CLASS lutBorder								 				*
 * **************************************************************/

class lutBorder {

protected:

	prelutHoughBorder    firstBorder;		/**< Object to store the border in the third dimension which comes from the prelut object. */
	lutHoughBorder       secondBorder;		/**< Object to store the border in the first and second dimension which comes from the lut object. */
	trackfinderInputHit* hit;				/**< Pointer to the hit which is responsible for this border object*/

public:

/**
 * Default constructor
 */

	lutBorder();

/**
 * Copy constructor
 */

	lutBorder(const lutBorder& value);

/**
 * Destructor
 */

	virtual ~lutBorder();

/**
 * operator = ()
 */

	const lutBorder& operator = (const lutBorder& value);

/**
 * method returns the prelut border
 */

	prelutHoughBorder& getPrelutHoughBorder();

/**
 * method returns the lut border
 */

	lutHoughBorder& getLutHoughBorder();

/**
 * method returns the hit corresponding to the borders
 */

	trackfinderInputHit* getHit();

/**
 * method sets the prelut border
 */

	prelutHoughBorder* getPrelutHoughBorderPointer();

/**
 * method sets the lut border
 */

	lutHoughBorder* getLutHoughBorderPointer();

/**
 * method sets the prelut border
 */

	void setPrelutHoughBorder(prelutHoughBorder& border);

/**
 * method sets the lut border
 */

	void setLutHoughBorder(lutHoughBorder& border);

/**
 * method sets the hit corresponding to the borders
 */

	void setHit(trackfinderInputHit* hit);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfPrelutData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfPrelutData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfPrelutData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfLutData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfLutData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfLutData(unsigned short dimension = 0);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfHitData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfHitData(unsigned short dimension = 0);

};

#endif
