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
///     - base class for holding the definition of the lut
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTDEFINITION_H
#define _LUTDEFINITION_H


/* **************************************************************
 * CLASS lutDefinition							 				*
 * **************************************************************/

class lutDefinition {

public:

	double dim1Min;			/**< Variable to store the minimal value for the first dimension to compute the prelut value. */
	double dim1Max;			/**< Variable to store the maximal value for the first dimension to compute the prelut value. */
	int    dim1Step;		/**< Variable to store the stepwidth of the first dimension. */
	double dim2Min;			/**< Variable to store the minimal value for the second dimension to compute the prelut value. */
	double dim2Max;			/**< Variable to store the maximal value for the second dimension to compute the prelut value. */
	int    dim2Step;		/**< Variable to store the stepwidth of the second dimension. */

/**
 * Default constructor
 */

	lutDefinition();

/**
 * Constructor
 * @param angleMin is the minimal angle to compute the prelut value
 * @param angleMax is the maximal angle to compute the prelut value
 * @param angleStep is the stepwidth of the angle
 * @param radiusMin is the minimal radius to compute the prelut value
 * @param radiusMax is the maximal radius to compute the prelut value
 * @param radiusStep is the stepwidth of the radius
 */

	lutDefinition(const lutDefinition& value);
	lutDefinition(double dim2Min, double dim2Max, int dim2Step, double dim1Min, double dim1Max, int dim1Step);

/**
 * Destructor
 */

	virtual ~lutDefinition();

/**
 * operator = ()
 */

	const lutDefinition& operator = (const lutDefinition& value);

/**
 * operator << (stream)
 */

	friend std::ostream& operator << (std::ostream& os, lutDefinition& anyObject);

/**
 * This method converts the object into a string representation.
 */
	
	std::string toString();

};

#endif
