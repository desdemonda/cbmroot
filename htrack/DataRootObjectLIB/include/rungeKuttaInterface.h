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
///     - class to implement the Runge-Kutta approach by using the
///       GEANE package
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:59:22 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _RUNGEKUTTAAPPROACH_H
#define _RUNGEKUTTAAPPROACH_H


#include "../../DataObjectLIB/include/trackMomentum.h"
#include "../include/trackfinderInputHit.h"
#include "../include/trackfinderInputDetector.h"
#include <list>

#if (ARCHITECTURE == CBMROOT)

#include "FairGeanePro.h"

#endif


/* **************************************************************
 * CLASS rungeKuttaInterface					 				*
 * **************************************************************/

class rungeKuttaInterface {

private:

	trackfinderInputDetector* detector;			/**< Object to store the detector for this event. */

#if (ARCHITECTURE == CBMROOT)

	FairGeanePro*              geane;			/**< Object to implement the GEANE track propagation */

#endif

	rungeKuttaInterface(const rungeKuttaInterface&);
	rungeKuttaInterface& operator=(const rungeKuttaInterface&);

public:

/**
 * Default constructor
 */

	rungeKuttaInterface();

/**
 * Constructor
 * @param detector is an object which contains information about each detector station
 */

	rungeKuttaInterface(trackfinderInputDetector* detector);

/**
 * Destructor
 */

	virtual ~rungeKuttaInterface();

/**
 * method initializes the object
 * @param detector is an object which contains information about each detector station
 */

	void init(trackfinderInputDetector* detector);

/**
 * method evaluates all hits in the detector stations for a
 * given track parameter
 */

	std::list<trackfinderInputHit> evaluate(trackMomentum& momentum, double charge, int pdgCode = 13);

};

#endif
