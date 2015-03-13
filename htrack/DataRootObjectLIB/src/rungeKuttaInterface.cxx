/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
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
//     - class to implement the Runge-Kutta approach by using the
//       GEANE package
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:59:23 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/dataRootObjectError.h"
#include "../include/rungeKuttaInterface.h"

#if (ARCHITECTURE == CBMROOT)

#include "TVector3.h"
#include "FairTrackParP.h"

#endif


/****************************************************************
 * CLASS rungeKuttaInterface					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

#if (ARCHITECTURE == CBMROOT)
rungeKuttaInterface::rungeKuttaInterface() : detector(NULL), geane(NULL) 
{
}
#else
rungeKuttaInterface::rungeKuttaInterface() : detector(NULL)
{
}
#endif
/****************************************************************
 * Constructor													*
 ****************************************************************/

#if (ARCHITECTURE == CBMROOT)
rungeKuttaInterface::rungeKuttaInterface(trackfinderInputDetector* _detector) 
  : detector(_detector), geane(new FairGeanePro())
{
}
#else
rungeKuttaInterface::rungeKuttaInterface(trackfinderInputDetector* _detector) 
  : detector(_detector)
{
}
#endif

/****************************************************************
 * Destructor													*
 ****************************************************************/

rungeKuttaInterface::~rungeKuttaInterface() {

#if (ARCHITECTURE == CBMROOT)

	if (geane != NULL) {
		delete geane;
		geane = NULL;
	}

#endif

}

/****************************************************************
 * method initializes the object								*
 ****************************************************************/

void rungeKuttaInterface::init(trackfinderInputDetector* _detector) {

	this->detector = _detector;

#if (ARCHITECTURE == CBMROOT)

	if (geane != NULL) {
		delete geane;
		geane = NULL;
	}

	geane          = new FairGeanePro();

#endif

}

/****************************************************************
 * method evaluates all hits in the detector stations for a		*
 * given track parameter and combines the values				*
 ****************************************************************/

std::list<trackfinderInputHit> rungeKuttaInterface::evaluate(trackMomentum& momentum, double charge, int pdgCode) {

	std::list<trackfinderInputHit> returnValue;

#if (ARCHITECTURE == CBMROOT)

	TVector3                       startPlane;
	FairTrackParP                   startParameter;
	trackfinderInputStation*       actualStation;
	TVector3                       stopPlane;
	FairTrackParP                   stopParameter;
	trackfinderInputHit            hit;

#endif

	returnValue.clear();

#if (ARCHITECTURE == CBMROOT)

	if (detector == NULL)
		throw detectorIsNotDefinedError();

	if (geane == NULL)
		throw trackPropagationEngineIsNotDefinedError();

	/* The startPlane consists of the position of the starting plane for the propagation */
	startPlane         = TVector3(0, 0, 0);
	/* The geaneStartParameter consists of the parameters for the propagation at the starting plane */
	startParameter = FairTrackParP(startPlane, TVector3(momentum.get(PX), momentum.get(PY), momentum.get(PZ)), TVector3(0,0,0), TVector3(0,0,0), charge, startPlane, TVector3(1,0,0), TVector3(0,1,0));

	for (unsigned short i = 0; i < detector->getNumberOfActiveStations(); i++) {

		actualStation  = detector->getStationPointerByIndex(i);

		if (actualStation != NULL) {

			/* The stopPlane consists of the position of the stopping plane for the propagation */
			stopPlane      = TVector3(0, 0, actualStation->getDistance());
			/* The following propagation function sets the mode of the propagation in combination with the planes */
			geane->PropagateFromPlane(startPlane, stopPlane);
			/* The following propagation function does the propagation */
			geane->Propagate(&startParameter, &stopParameter, pdgCode);
			
			/* The following line creates the resulting hit for the propagation */
			hit.init(actualStation->getId(), stopParameter.GetX(), stopParameter.GetY(), stopParameter.GetZ(), 0, 0, -1, 0, -1);
			hit.setStation(actualStation);
			/* The following line stores the resulting hit for the propagation */
			returnValue.push_back(hit);

			/* The startPlane consists of the position of the starting plane for the propagation */
			startPlane     = stopPlane;
			/* The geaneStartParameter consists of the parameters for the propagation at the starting plane */
			startParameter = stopParameter;

		}

	}

#endif

	return returnValue;

}
