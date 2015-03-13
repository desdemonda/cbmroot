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
///     - base class for warnings occuring during input
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:16 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTWARNINGMSG_H
#define _INPUTWARNINGMSG_H


#include <string>


//#define NOINPUTWARNINGMESSAGE		/**< If this is defined, no message would be written to the standard output. */
#undef NOINPUTWARNINGMESSAGE


/* **************************************************************
 * CLASS inputWarningMsg						 				*
 * **************************************************************/

class inputWarningMsg {

public:

/**
 * Default constructor
 */

	inputWarningMsg();

/**
 * Destructor
 */

	virtual ~inputWarningMsg();

/**
 * This method prints a message.
 */

	void printMsg(std::string message);

/**
 * This method displays a warning message.
 */

	virtual void warningMsg() = 0;

};

#endif


#ifndef _NOHITWARNINGMSG_H
#define _NOHITWARNINGMSG_H

/**
 * CLASS noHitWarningMsg
 */

class noHitWarningMsg : public inputWarningMsg {

private:

	std::string branchNameHit;
	std::string branchNamePoint;

public:

/**
 * Default constructor
 */

	noHitWarningMsg();

/**
 * Constructor
 */

	noHitWarningMsg(std::string actualBranchNameHit, std::string actualBranchNamePoint);

/**
 * Destructor
 */

	virtual ~noHitWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _STATIONSHAPENOTFOUNDWARNINGMSG_H
#define _STATIONSHAPENOTFOUNDWARNINGMSG_H

/**
 * CLASS stationShapeNotFoundWarningMsg
 */

class stationShapeNotFoundWarningMsg : public inputWarningMsg {

private:

	std::string stationName;

public:

/**
 * Default constructor
 */

	stationShapeNotFoundWarningMsg();

/**
 * Constructor
 */

	stationShapeNotFoundWarningMsg(std::string actualStationName);

/**
 * Destructor
 */

	virtual ~stationShapeNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _STATIONMATERIALNOTFOUNDWARNINGMSG_H
#define _STATIONMATERIALNOTFOUNDWARNINGMSG_H

/**
 * CLASS stationMaterialNotFoundWarningMsg
 */

class stationMaterialNotFoundWarningMsg : public inputWarningMsg {

private:

	std::string stationName;

public:

/**
 * Default constructor
 */

	stationMaterialNotFoundWarningMsg();

/**
 * Constructor
 */

	stationMaterialNotFoundWarningMsg(std::string actualStationName);

/**
 * Destructor
 */

	virtual ~stationMaterialNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ASCIIFILESNOTFOUNDWARNINGMSG_H
#define _ASCIIFILESNOTFOUNDWARNINGMSG_H

/**
 * CLASS asciiFilesNotFoundWarningMsg
 */

class asciiFilesNotFoundWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	asciiFilesNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~asciiFilesNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ASCIIFILENOTFOUNDWARNINGMSG_H
#define _ASCIIFILENOTFOUNDWARNINGMSG_H

/**
 * CLASS asciiFileNotFoundWarningMsg
 */

class asciiFileNotFoundWarningMsg : public inputWarningMsg {

private:

	std::string fileName;

public:

/**
 * Default constructor
 */

	asciiFileNotFoundWarningMsg();

/**
 * Constructor
 */

	asciiFileNotFoundWarningMsg(std::string fileName);

/**
 * Destructor
 */

	virtual ~asciiFileNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CBMROOTENVIRONMENTVARIABLENOTFOUNDWARNINGMSG_H
#define _CBMROOTENVIRONMENTVARIABLENOTFOUNDWARNINGMSG_H

/**
 * CLASS cbmrootEnvironmentVariableNotFoundWarningMsg
 */

class cbmrootEnvironmentVariableNotFoundWarningMsg : public inputWarningMsg {

private:

	std::string defaultValue;

public:

/**
 * Default constructor
 */

	cbmrootEnvironmentVariableNotFoundWarningMsg();

/**
 * Constructor
 */

	cbmrootEnvironmentVariableNotFoundWarningMsg(std::string actualDefaultValue);

/**
 * Destructor
 */

	virtual ~cbmrootEnvironmentVariableNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITSFROMFILEREADWARNINGMSG_H
#define _HITSFROMFILEREADWARNINGMSG_H

/**
 * CLASS hitsFromFileReadWarningMsg
 */

class hitsFromFileReadWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	hitsFromFileReadWarningMsg();

/**
 * Destructor
 */

	virtual ~hitsFromFileReadWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITSFROMMEMORYREADWARNINGMSG_H
#define _HITSFROMMEMORYREADWARNINGMSG_H

/**
 * CLASS hitsFromMemoryReadWarningMsg
 */

class hitsFromMemoryReadWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	hitsFromMemoryReadWarningMsg();

/**
 * Destructor
 */

	virtual ~hitsFromMemoryReadWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITWITHWRONGSTATIONWARNINGMSG_H
#define _HITWITHWRONGSTATIONWARNINGMSG_H

/**
 * CLASS hitWithWrongStationFoundWarningMsg
 */

class hitWithWrongStationFoundWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	hitWithWrongStationFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~hitWithWrongStationFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETRUNANAINSTANCEWARNINGMSG_H
#define _CANNOTGETRUNANAINSTANCEWARNINGMSG_H

/**
 * CLASS cannotGetRunAnaInstanceWarningMsg
 */

class cannotGetRunAnaInstanceWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetRunAnaInstanceWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetRunAnaInstanceWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETRUNTIMEDBWARNINGMSG_H
#define _CANNOTGETRUNTIMEDBWARNINGMSG_H

/**
 * CLASS cannotGetRuntimeDbWarningMsg
 */

class cannotGetRuntimeDbWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetRuntimeDbWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetRuntimeDbWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETSTSPARAMETERCONTAINERWARNINGMSG_H
#define _CANNOTGETSTSPARAMETERCONTAINERWARNINGMSG_H

/**
 * CLASS cannotGetStsParameterContainerWarningMsg
 */

class cannotGetStsParameterContainerWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetStsParameterContainerWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetStsParameterContainerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETMVDPARAMETERCONTAINERWARNINGMSG_H
#define _CANNOTGETMVDPARAMETERCONTAINERWARNINGMSG_H

/**
 * CLASS cannotGetMvdParameterContainerWarningMsg
 */

class cannotGetMvdParameterContainerWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetMvdParameterContainerWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetMvdParameterContainerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETSTSGEOSENSITIVEVODESWARNINGMSG_H
#define _CANNOTGETSTSGEOSENSITIVEVODESWARNINGMSG_H

/**
 * CLASS cannotGetStsGeoSensitiveNodesWarningMsg
 */

class cannotGetStsGeoSensitiveNodesWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetStsGeoSensitiveNodesWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetStsGeoSensitiveNodesWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETMVDGEOSENSITIVEVODESWARNINGMSG_H
#define _CANNOTGETMVDGEOSENSITIVEVODESWARNINGMSG_H

/**
 * CLASS cannotGetMvdGeoSensitiveNodesWarningMsg
 */

class cannotGetMvdGeoSensitiveNodesWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetMvdGeoSensitiveNodesWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetMvdGeoSensitiveNodesWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETLABTRANSFORMWARNINGMSG_H
#define _CANNOTGETLABTRANSFORMWARNINGMSG_H

/**
 * CLASS cannotGetLabTransformWarningMsg
 */

class cannotGetLabTransformWarningMsg : public inputWarningMsg {

private:

	int stationId;

public:

/**
 * Default constructor
 */

	cannotGetLabTransformWarningMsg();

/**
 * Constructor
 */

	cannotGetLabTransformWarningMsg(int stationId);

/**
 * Destructor
 */

	virtual ~cannotGetLabTransformWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTAUTOMATICREADDETECTORWARNINGMSG_H
#define _CANNOTAUTOMATICREADDETECTORWARNINGMSG_H

/**
 * CLASS cannotAutomaticReadDetectorWarningMsg
 */

class cannotAutomaticReadDetectorWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAutomaticReadDetectorWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAutomaticReadDetectorWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISABLEAUTOMATICREADHITSWARNINGMSG_H
#define _DISABLEAUTOMATICREADHITSWARNINGMSG_H

/**
 * CLASS disableAutomaticReadHitsWarningMsg
 */

class disableAutomaticReadHitsWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	disableAutomaticReadHitsWarningMsg();

/**
 * Destructor
 */

	virtual ~disableAutomaticReadHitsWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TAKEPOINTSASHITSWARNINGMSG_H
#define _TAKEPOINTSASHITSWARNINGMSG_H

/**
 * CLASS takePointsAsHitsWarningMsg
 */

class takePointsAsHitsWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	takePointsAsHitsWarningMsg();

/**
 * Destructor
 */

	virtual ~takePointsAsHitsWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TAKEMYHITPRODUCERWARNINGMSG_H
#define _TAKEMYHITPRODUCERWARNINGMSG_H

/**
 * CLASS takeMyHitProducerWarningMsg
 */

class takeMyHitProducerWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	takeMyHitProducerWarningMsg();

/**
 * Destructor
 */

	virtual ~takeMyHitProducerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITARRAYPOINTERISNOTSUPPORTEDINASCIIWARNINGMSG_H
#define _HITARRAYPOINTERISNOTSUPPORTEDINASCIIWARNINGMSG_H

/**
 * CLASS hitArrayPointerIsNotSupportedInAsciiWarningMsg
 */

class hitArrayPointerIsNotSupportedInAsciiWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	hitArrayPointerIsNotSupportedInAsciiWarningMsg();

/**
 * Destructor
 */

	virtual ~hitArrayPointerIsNotSupportedInAsciiWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOHITTOPOINTWARNINGMSG_H
#define _NOHITTOPOINTWARNINGMSG_H

/**
 * CLASS noHitToPointWarningMsg
 */

class noHitToPointWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	noHitToPointWarningMsg();

/**
 * Destructor
 */

	virtual ~noHitToPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIGISCHEMEISNOTSUPPORTEDINASCIIWARNINGMSG_H
#define _DIGISCHEMEISNOTSUPPORTEDINASCIIWARNINGMSG_H

/**
 * CLASS digiSchemeIsNotSupportedInAsciiWarningMsg
 */

class digiSchemeIsNotSupportedInAsciiWarningMsg : public inputWarningMsg {

public:

/**
 * Default constructor
 */

	digiSchemeIsNotSupportedInAsciiWarningMsg();

/**
 * Destructor
 */

	virtual ~digiSchemeIsNotSupportedInAsciiWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETSTSSTATIONWITHINDEXWARNINGMSG_H
#define _CANNOTGETSTSSTATIONWITHINDEXWARNINGMSG_H

/**
 * CLASS cannotGetStsStationWithIndexWarningMsg
 */

class cannotGetStsStationWithIndexWarningMsg : public inputWarningMsg {

private:

	int stationId;

public:

/**
 * Default constructor
 */

	cannotGetStsStationWithIndexWarningMsg();

/**
 * Constructor
 */

	cannotGetStsStationWithIndexWarningMsg(int stationId);

/**
 * Destructor
 */

	virtual ~cannotGetStsStationWithIndexWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETMVDSTATIONWITHINDEXWARNINGMSG_H
#define _CANNOTGETMVDSTATIONWITHINDEXWARNINGMSG_H

/**
 * CLASS cannotGetMvdStationWithIndexWarningMsg
 */

class cannotGetMvdStationWithIndexWarningMsg : public inputWarningMsg {

private:

	int stationId;

public:

/**
 * Default constructor
 */

	cannotGetMvdStationWithIndexWarningMsg();

/**
 * Constructor
 */

	cannotGetMvdStationWithIndexWarningMsg(int stationId);

/**
 * Destructor
 */

	virtual ~cannotGetMvdStationWithIndexWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
