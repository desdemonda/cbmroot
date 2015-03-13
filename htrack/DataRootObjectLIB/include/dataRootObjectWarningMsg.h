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
///     - base class for warnings occuring during dataObject-access
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:59:22 $
/// $Revision: 1.10 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DATAROOTOBJECTWARNINGMSG_H
#define _DATAROOTOBJECTWARNINGMSG_H


#include "../../MiscLIB/include/defs.h"
#include <string>


//#define NODATAROOTOBJECTWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NODATAROOTOBJECTWARNINGMESSAGE


/* **************************************************************
 * CLASS dataRootObjectWarningMsg				 				*
 * **************************************************************/

class dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	dataRootObjectWarningMsg();

/**
 * Destructor
 */

	virtual ~dataRootObjectWarningMsg();

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


#ifndef _NOTRACKWITHIDFOUNDWARNINGMSG_H
#define _NOTRACKWITHIDFOUNDWARNINGMSG_H


#if (ARCHITECTURE != PS3)

#include "trackfinderInputHit.h"

#endif

/**
 * CLASS noTrackWithIdFoundWarningMsg
 */

class noTrackWithIdFoundWarningMsg : public dataRootObjectWarningMsg {

private:

  int                  trackId;

#if (ARCHITECTURE != PS3)

  trackfinderInputHit* hit;

#endif

  noTrackWithIdFoundWarningMsg(const noTrackWithIdFoundWarningMsg&);
  noTrackWithIdFoundWarningMsg& operator=(const noTrackWithIdFoundWarningMsg&);

public:

/**
 * Default constructor
 */

	noTrackWithIdFoundWarningMsg();

/**
 * Constructor
 */

	noTrackWithIdFoundWarningMsg(int actualTrackId);

#if (ARCHITECTURE != PS3)

	noTrackWithIdFoundWarningMsg(int actualTrackId, trackfinderInputHit* actualHit);

#endif

/**
 * Destructor
 */

	virtual ~noTrackWithIdFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();


};

#endif


#ifndef _TRACKSWITHNOIDWARNINGMSG_H
#define _TRACKSWITHNOIDWARNINGMSG_H

/**
 * CLASS tracksWithNoIdWarningMsg
 */

class tracksWithNoIdWarningMsg : public dataRootObjectWarningMsg {

private:

	unsigned int quantum;

public:

/**
 * Default constructor
 */

	tracksWithNoIdWarningMsg();

/**
 * Constructor
 */

	tracksWithNoIdWarningMsg(int actualQuantum);

/**
 * Destructor
 */

	virtual ~tracksWithNoIdWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _WRONGHITDEFINITIONWARNINGMSG_H
#define _WRONGHITDEFINITIONWARNINGMSG_H

/**
 * CLASS wrongHitDefinitionWarningMsg
 */

class wrongHitDefinitionWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	wrongHitDefinitionWarningMsg();

/**
 * Destructor
 */

	virtual ~wrongHitDefinitionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _WRONGSTATIONDEFINITIONWARNINGMSG_H
#define _WRONGSTATIONDEFINITIONWARNINGMSG_H

/**
 * CLASS wrongDetectorDefinitionWarningMsg
 */

class wrongStationDefinitionWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	wrongStationDefinitionWarningMsg();

/**
 * Destructor
 */

	virtual ~wrongStationDefinitionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _INDEXOFMASKEDSTATIONWARNINGMSG_H
#define _INDEXOFMASKEDSTATIONWARNINGMSG_H

/**
 * CLASS indexOfMaskedStationWarningMsg
 */

class indexOfMaskedStationWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	indexOfMaskedStationWarningMsg();

/**
 * Destructor
 */

	virtual ~indexOfMaskedStationWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHTOOBIGLAYERWARNINGMSG_H
#define _TRACKWITHTOOBIGLAYERWARNINGMSG_H

/**
 * CLASS trackWithTooBigLayerWarningMsg
 */

class trackWithTooBigLayerWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	trackWithTooBigLayerWarningMsg();

/**
 * Destructor
 */

	virtual ~trackWithTooBigLayerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TOOBIGLAYERWARNINGMSG_H
#define _TOOBIGLAYERWARNINGMSG_H

/**
 * CLASS tooBigLayerWarningMsg
 */

class tooBigLayerWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	tooBigLayerWarningMsg();

/**
 * Destructor
 */

	virtual ~tooBigLayerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSRUNANAWARNINGMSG_H
#define _CANNOTACCESSRUNANAWARNINGMSG_H

/**
 * CLASS cannotAccessRunAnaWarningMsg
 */

class cannotAccessRunAnaWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessRunAnaWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessRunAnaWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSRUNTIMEDBWARNINGMSG_H
#define _CANNOTACCESSRUNTIMEDBWARNINGMSG_H

/**
 * CLASS cannotAccessRuntimeDbWarningMsg
 */

class cannotAccessRuntimeDbWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessRuntimeDbWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessRuntimeDbWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSFIELDPARWARNINGMSG_H
#define _CANNOTACCESSFIELDPARWARNINGMSG_H

/**
 * CLASS cannotAccessFieldParWarningMsg
 */

class cannotAccessFieldParWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessFieldParWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessFieldParWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _UNKNOWNFIELDTYPEWARNINGMSG_H
#define _UNKNOWNFIELDTYPEWARNINGMSG_H

/**
 * CLASS unknownFieldTypeWarningMsg
 */

class unknownFieldTypeWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	unknownFieldTypeWarningMsg();

/**
 * Destructor
 */

	virtual ~unknownFieldTypeWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETMAGNETICFIELDWARNINGMSG_H
#define _CANNOTGETMAGNETICFIELDWARNINGMSG_H

/**
 * CLASS cannotGetMagneticFieldWarningMsg
 */

class cannotGetMagneticFieldWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetMagneticFieldWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetMagneticFieldWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _STATIONSETMAPSSTATIONTYPEWARNINGMSG_H
#define _STATIONSETMAPSSTATIONTYPEWARNINGMSG_H

/**
 * CLASS stationSetMapsTypeWarningMsg
 */

class stationSetMapsTypeWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int  stationId;
	bool isHybridType;
	bool isStripType;

public:

/**
 * Default constructor
 */

	stationSetMapsTypeWarningMsg();

/**
 * C1onstructor
 */

	stationSetMapsTypeWarningMsg(int stationId, bool isHybridType, bool isStripType);

/**
 * Destructor
 */

	virtual ~stationSetMapsTypeWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _STATIONSETHYBRIDSTATIONTYPEWARNINGMSG_H
#define _STATIONSETHYBRIDSTATIONTYPEWARNINGMSG_H

/**
 * CLASS stationSetHybridTypeWarningMsg
 */

class stationSetHybridTypeWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int  stationId;
	bool isMapsType;
	bool isStripType;

public:

/**
 * Default constructor
 */

	stationSetHybridTypeWarningMsg();

/**
 * C1onstructor
 */

	stationSetHybridTypeWarningMsg(int stationId, bool isMapsType, bool isStripType);

/**
 * Destructor
 */

	virtual ~stationSetHybridTypeWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _STATIONSETSTRIPSTATIONTYPEWARNINGMSG_H
#define _STATIONSETSTRIPSTATIONTYPEWARNINGMSG_H

/**
 * CLASS stationSetStripTypeWarningMsg
 */

class stationSetStripTypeWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int  stationId;
	bool isMapsType;
	bool isHybridType;

public:

/**
 * Default constructor
 */

	stationSetStripTypeWarningMsg();

/**
 * C1onstructor
 */

	stationSetStripTypeWarningMsg(int stationId, bool isMapsType, bool isHybridType);

/**
 * Destructor
 */

	virtual ~stationSetStripTypeWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHNOPOINTWARNINGMSG_H
#define _TRACKWITHNOPOINTWARNINGMSG_H

/**
 * CLASS trackWithNoPointWarningMsg
 */

class trackWithNoPointWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int trackIndex;

public:

/**
 * Default constructor
 */

	trackWithNoPointWarningMsg();

/**
 * C1onstructor
 */

	trackWithNoPointWarningMsg(int trackIndex);

/**
 * Destructor
 */

	virtual ~trackWithNoPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHNOHITWARNINGMSG_H
#define _TRACKWITHNOHITWARNINGMSG_H

/**
 * CLASS trackWithNoHitWarningMsg
 */

class trackWithNoHitWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int trackIndex;

public:

/**
 * Default constructor
 */

	trackWithNoHitWarningMsg();

/**
 * C1onstructor
 */

	trackWithNoHitWarningMsg(int trackIndex);

/**
 * Destructor
 */

	virtual ~trackWithNoHitWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHWRONGNUMBEROFHITSTOPOINTSWARNINGMSG_H
#define _TRACKWITHWRONGNUMBEROFHITSTOPOINTSWARNINGMSG_H

/**
 * CLASS trackWithWrongNumberOfHitsToPointsWarningMsg
 */

class trackWithWrongNumberOfHitsToPointsWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int trackIndex;
	int numberOfPoints;
	int numberOfHits;

public:

/**
 * Default constructor
 */

	trackWithWrongNumberOfHitsToPointsWarningMsg();

/**
 * C1onstructor
 */

	trackWithWrongNumberOfHitsToPointsWarningMsg(int trackIndex, int numberOfPoints, int numberOfHits);

/**
 * Destructor
 */

	virtual ~trackWithWrongNumberOfHitsToPointsWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKSWITHNOPOINTWARNINGMSG_H
#define _TRACKSWITHNOPOINTWARNINGMSG_H

/**
 * CLASS tracksWithNoPointWarningMsg
 */

class tracksWithNoPointWarningMsg : public dataRootObjectWarningMsg {

private:
	
	unsigned short tracksWithNoPoints;

public:

/**
 * Default constructor
 */

	tracksWithNoPointWarningMsg();

/**
 * C1onstructor
 */

	tracksWithNoPointWarningMsg(unsigned short tracksWithNoPoints);

/**
 * Destructor
 */

	virtual ~tracksWithNoPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKSWITHNOHITWARNINGMSG_H
#define _TRACKSWITHNOHITWARNINGMSG_H

/**
 * CLASS tracksWithNoHitWarningMsg
 */

class tracksWithNoHitWarningMsg : public dataRootObjectWarningMsg {

private:
	
	unsigned short tracksWithNoHits;

public:

/**
 * Default constructor
 */

	tracksWithNoHitWarningMsg();

/**
 * C1onstructor
 */

	tracksWithNoHitWarningMsg(unsigned short tracksWithNoHits);

/**
 * Destructor
 */

	virtual ~tracksWithNoHitWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKSWITHWRONGNUMBEROFHITSTOPOINTSWARNINGMSG_H
#define _TRACKSWITHWRONGNUMBEROFHITSTOPOINTSWARNINGMSG_H

/**
 * CLASS tracksWithWrongNumberOfHitsToPointsWarningMsg
 */

class tracksWithWrongNumberOfHitsToPointsWarningMsg : public dataRootObjectWarningMsg {

private:
	
	unsigned short trackWithWrongNumberOfHitsToPoints;

public:

/**
 * Default constructor
 */

	tracksWithWrongNumberOfHitsToPointsWarningMsg();

/**
 * C1onstructor
 */

	tracksWithWrongNumberOfHitsToPointsWarningMsg(unsigned short trackWithWrongNumberOfHitsToPoints);

/**
 * Destructor
 */

	virtual ~tracksWithWrongNumberOfHitsToPointsWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _STATIONINDEXISBIGGERTHANLASTSTATIONINDEXWARNINGMSG_H
#define _STATIONINDEXISBIGGERTHANLASTSTATIONINDEXWARNINGMSG_H

/**
 * CLASS stationIndexIsBiggerThanLastStationIndexWarningMsg
 */

class stationIndexIsBiggerThanLastStationIndexWarningMsg : public dataRootObjectWarningMsg {

private:
	
	unsigned short stationIndex;
	unsigned short lastStationIndex;

public:

/**
 * Default constructor
 */

	stationIndexIsBiggerThanLastStationIndexWarningMsg();

/**
 * C1onstructor
 */

	stationIndexIsBiggerThanLastStationIndexWarningMsg(unsigned short stationIndex, unsigned short lastStationIndex);

/**
 * Destructor
 */

	virtual ~stationIndexIsBiggerThanLastStationIndexWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _INTEGRATIONRANGEOFZEROWARNINGMSG_H
#define _INTEGRATIONRANGEOFZEROWARNINGMSG_H

/**
 * CLASS integrationRangeOfZeroWarningMsg
 */

class integrationRangeOfZeroWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	integrationRangeOfZeroWarningMsg();

/**
 * Destructor
 */

	virtual ~integrationRangeOfZeroWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ANALYSERISNOTACCESSIBLEWARNINGMSG_H
#define _ANALYSERISNOTACCESSIBLEWARNINGMSG_H

/**
 * CLASS analyserIsNotAccessibleWarningMsg
 */

class analyserIsNotAccessibleWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	analyserIsNotAccessibleWarningMsg();

/**
 * Destructor
 */

	virtual ~analyserIsNotAccessibleWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITANALYSISNOTSUPPORTEDWARNINGMSG_H
#define _HITANALYSISNOTSUPPORTEDWARNINGMSG_H

/**
 * CLASS hitAnalysisNotSupportedWarningMsg
 */

class hitAnalysisNotSupportedWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	hitAnalysisNotSupportedWarningMsg();

/**
 * Destructor
 */

	virtual ~hitAnalysisNotSupportedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITWITHNOPOINTWARNINGMSG_H
#define _HITWITHNOPOINTWARNINGMSG_H

/**
 * CLASS hitWithNoPointWarningMsg
 */

class hitWithNoPointWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int hitIndex;

public:

/**
 * Default constructor
 */

	hitWithNoPointWarningMsg();

/**
 * C1onstructor
 */

	hitWithNoPointWarningMsg(int hitIndex);

/**
 * Destructor
 */

	virtual ~hitWithNoPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITWITHNOTRACKWARNINGMSG_H
#define _HITWITHNOTRACKWARNINGMSG_H

/**
 * CLASS hitWithNoTrackWarningMsg
 */

class hitWithNoTrackWarningMsg : public dataRootObjectWarningMsg {

private:
	
	int hitIndex;

public:

/**
 * Default constructor
 */

	hitWithNoTrackWarningMsg();

/**
 * C1onstructor
 */

	hitWithNoTrackWarningMsg(int hitIndex);

/**
 * Destructor
 */

	virtual ~hitWithNoTrackWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITSWITHNOPOINTWARNINGMSG_H
#define _HITSWITHNOPOINTWARNINGMSG_H

/**
 * CLASS hitsWithNoPointWarningMsg
 */

class hitsWithNoPointWarningMsg : public dataRootObjectWarningMsg {

private:
	
	unsigned short hitsWithNoPoint;

public:

/**
 * Default constructor
 */

	hitsWithNoPointWarningMsg();

/**
 * C1onstructor
 */

	hitsWithNoPointWarningMsg(unsigned short hitsWithNoPoint);

/**
 * Destructor
 */

	virtual ~hitsWithNoPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITSWITHNOTRACKWARNINGMSG_H
#define _HITSWITHNOTRACKWARNINGMSG_H

/**
 * CLASS hitsWithNoTrackWarningMsg
 */

class hitsWithNoTrackWarningMsg : public dataRootObjectWarningMsg {

private:
	
	unsigned short hitsWithNoTrack;

public:

/**
 * Default constructor
 */

	hitsWithNoTrackWarningMsg();

/**
 * C1onstructor
 */

	hitsWithNoTrackWarningMsg(unsigned short hitsWithNoTrack);

/**
 * Destructor
 */

	virtual ~hitsWithNoTrackWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _REREADMAGNETICFIELDWARNINGMSG_H
#define _REREADMAGNETICFIELDWARNINGMSG_H

/**
 * CLASS reReadMagneticFieldWarningMsg
 */

class reReadMagneticFieldWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	reReadMagneticFieldWarningMsg();

/**
 * Destructor
 */

	virtual ~reReadMagneticFieldWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _LUTGOODNESSNOTPOSSIBLEFORGRADINGPWARNINGMSG_H
#define _LUTGOODNESSNOTPOSSIBLEFORGRADINGPWARNINGMSG_H

/**
 * CLASS lutGoodnessNotPossibleForGradingPWarningMsg
 */

class lutGoodnessNotPossibleForGradingPWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	lutGoodnessNotPossibleForGradingPWarningMsg();

/**
 * Destructor
 */

	virtual ~lutGoodnessNotPossibleForGradingPWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MISSINGORIGINWARNINGMSG_H
#define _MISSINGORIGINWARNINGMSG_H

/**
 * CLASS missingOriginWarningMsg
 */

class missingOriginWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	missingOriginWarningMsg();

/**
 * Destructor
 */

	virtual ~missingOriginWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOENTRYINLISTWARNINGMSG_H
#define _NOENTRYINLISTWARNINGMSG_H

/**
 * CLASS noEntryInListWarningMsg
 */

class noEntryInListWarningMsg : public dataRootObjectWarningMsg {

public:

/**
 * Default constructor
 */

	noEntryInListWarningMsg();

/**
 * Destructor
 */

	virtual ~noEntryInListWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
