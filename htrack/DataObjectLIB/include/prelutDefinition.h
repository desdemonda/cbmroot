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
///     - base class for holding the definition of the prelut
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTDEFINITION_H
#define _PRELUTDEFINITION_H


#include <iostream>


/* **************************************************************
 * CLASS prelutDefinition						 				*
 * **************************************************************/

class prelutDefinition {

public:

	double dim3Min;				/**< Variable to store the minimal value for the third dimension to compute the prelut value. */
	double dim3Max;				/**< Variable to store the maximal value for the third dimension to compute the prelut value. */
	int    dim3Step;			/**< Variable to store the stepwidth of the third dimension. */
	double dim3StartEntry;		/**< Variable to store the minimal starting entry in the third dimension. */
	double dim3StopEntry;		/**< Variable to store the maximal stop entry in the third dimension. */

/**
 * Default constructor
 */

	prelutDefinition();

/**
 * Constructor
 * @param angleMin is the minimal angle to compute the prelut value
 * @param angleMax is the maximal angle to compute the prelut value
 * @param angleStep is the stepwidth of the angle
 * @param radiusMin is the minimal radius to compute the prelut value
 * @param radiusMax is the maximal radius to compute the prelut value
 */

	prelutDefinition(const prelutDefinition& value);
	prelutDefinition(double angleMin, double angleMax, int angleStep, double radiusMin, double radiusMax);

/**
 * Destructor
 */

	virtual ~prelutDefinition();

/**
 * operator = ()
 */

	const prelutDefinition& operator = (const prelutDefinition& value);

/**
 * operator << (stream)
 */

	friend std::ostream& operator << (std::ostream& os, prelutDefinition& anyObject);

/**
 * This method converts the object into a string representation.
 */
	
	std::string toString();

};

#endif
