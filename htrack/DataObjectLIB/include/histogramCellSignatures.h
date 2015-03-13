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
///     - class for holding the histogram cell signatures' analysis
///       information
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMCELLSIGNATURES_H
#define _HISTOGRAMCELLSIGNATURES_H


#include "../../MiscLIB/include/bitArray.h"
#include "houghBorderPosition.h"
#include "trackCoordinates.h"


struct histogramCellSignature {
  
histogramCellSignature() : pos(), value(), distance(0.) {};

	trackCoordinates pos;
	bitArray         value;
	double           distance;

};


/* **************************************************************
 * CLASS histogramCellSignatures				 				*
 * **************************************************************/

class histogramCellSignatures {

protected:

	unsigned short          numberOfSignatures;
	histogramCellSignature* signatures;
	bool                    just2DFlag;

public:

/**
 * Default constructor
 */

	histogramCellSignatures();

/**
 * Constructor
 */

	histogramCellSignatures(const histogramCellSignatures& value);

/**
 * Destructor
 */

	virtual ~histogramCellSignatures();

/**
 * operator = ()
 */

	const histogramCellSignatures& operator = (const histogramCellSignatures& value);

/**
 * method resets all signatures
 */

	void reset();

/**
 * method sets the 2D-flag which is used to find the best signature. If
 * it is true, the comparisson is jused done in the first and the second
 * dimension. If it is false, the third dimension is also taken into
 * account.
 */

	void set2DFlag(bool flag);

/**
 * method for adding a signature
 */

	void add(houghBorderPosition& pos, unsigned short stationIndex);
	void add(houghBorderPosition& pos, unsigned short pos3, unsigned short stationIndex);

/**
 * method returns the 2D-flag which is used to find the best signature.
 */

	bool get2DFlag();

/**
 * method for getting the best signature
 */

	histogramCellSignature getBest(trackCoordinates& pos);

};

#endif
