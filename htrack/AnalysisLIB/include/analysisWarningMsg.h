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
///     - base class for warnings occuring during analysis
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-10 13:45:47 $
/// $Revision: 1.18 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSISWARNINGMSG_H
#define _ANALYSISWARNINGMSG_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include <string>


//#define NOANALYSISWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOANALYSISWARNINGMESSAGE


/* **************************************************************
 * CLASS analysisWarningMsg						 				*
 * **************************************************************/

class analysisWarningMsg {

public:

/**
 * Default constructor
 */

	analysisWarningMsg();

/**
 * Destructor
 */

	virtual ~analysisWarningMsg();

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


#ifndef _TRACKNOTFOUNDWARNINGMSG_H
#define _TRACKNOTFOUNDWARNINGMSG_H

/**
 * CLASS trackNotFoundWarningMsg
 */

class trackNotFoundWarningMsg : public analysisWarningMsg {

private:

	trackfinderInputTrack* track;

        trackNotFoundWarningMsg(const trackNotFoundWarningMsg&);
        trackNotFoundWarningMsg& operator=(const trackNotFoundWarningMsg&);
public:

/**
 * Default constructor
 */

	trackNotFoundWarningMsg();

/**
 * Constructor
 */

	trackNotFoundWarningMsg(trackfinderInputTrack* actualTrack);

/**
 * Destructor
 */

	virtual ~trackNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYANALYSERNOTFOUNDWARNINGMSG_H
#define _DISPLAYANALYSERNOTFOUNDWARNINGMSG_H

/**
 * CLASS displayAnalyserNotFoundWarningMsg
 */

class displayAnalyserNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayAnalyserNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~displayAnalyserNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HARDWAREANALYSERNOTFOUNDWARNINGMSG_H
#define _HARDWAREANALYSERNOTFOUNDWARNINGMSG_H

/**
 * CLASS hardwareAnalyserNotFoundWarningMsg
 */

class hardwareAnalyserNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	hardwareAnalyserNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~hardwareAnalyserNotFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYCANNOTBEWRITTENWARNINGMSG_H
#define _DISPLAYCANNOTBEWRITTENWARNINGMSG_H

/**
 * CLASS displayCannotBeWrittenToFileWarningMsg
 */

class displayCannotBeWrittenToFileWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayCannotBeWrittenToFileWarningMsg();

/**
 * Destructor
 */

	virtual ~displayCannotBeWrittenToFileWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYNOTFOUNDWARNINGMSG_H
#define _DISPLAYNOTFOUNDWARNINGMSG_H

/**
 * CLASS displayNotFoundWarningMsg
 */

class displayNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~displayNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTCREATESIGNATUREWARNINGMSG_H
#define _CANNOTCREATESIGNATUREWARNINGMSG_H

/**
 * CLASS cannotCreateSignatureWarningMsg
 */

class cannotCreateSignatureWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotCreateSignatureWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotCreateSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITEWARNINGMSG_H
#define _CANNOTWRITEWARNINGMSG_H

/**
 * CLASS cannotWriteWarningMsg
 */

class cannotWriteWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotWriteWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotWriteWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOTALLDISPLAYSCANBEDRAWNWARNINGMSG_H
#define _NOTALLDISPLAYSCANBEDRAWNWARNINGMSG_H

/**
 * CLASS notAllDisplaysCanBeDrawnWarningMsg
 */

class notAllDisplaysCanBeDrawnWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	notAllDisplaysCanBeDrawnWarningMsg();

/**
 * Destructor
 */

	virtual ~notAllDisplaysCanBeDrawnWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSPARAMETERWARNINGMSG_H
#define _CANNOTACCESSPARAMETERWARNINGMSG_H

/**
 * CLASS cannotAccessParameterWarningMsg
 */

class cannotAccessParameterWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessParameterWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessParameterWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKWARNINGMSG_H
#define _CANNOTACCESSTRACKWARNINGMSG_H

/**
 * CLASS cannotAccessTrackWarningMsg
 */

class cannotAccessTrackWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotAccessTrackWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTGETDISPLAYENTRIESWARNINGMSG_H
#define _CANNOTGETDISPLAYENTRIESWARNINGMSG_H

/**
 * CLASS cannotGetDisplayEntriesWarningMsg
 */

class cannotGetDisplayEntriesWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotGetDisplayEntriesWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotGetDisplayEntriesWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOHITINDETECTORSTATIONWARNINGMSG_H
#define _NOHITINDETECTORSTATIONWARNINGMSG_H

/**
 * CLASS noHitInDetectorStationWarningMsg
 */

class noHitInDetectorStationWarningMsg : public analysisWarningMsg {

private:

	unsigned short index;

public:

/**
 * Default constructor
 */

	noHitInDetectorStationWarningMsg();

/**
 * Constructor
 */

	noHitInDetectorStationWarningMsg(unsigned short actualIndex);

/**
 * Destructor
 */

	virtual ~noHitInDetectorStationWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOANALYSISMOMENTUMOUTPUTFILENAMESPECIFIEDWARNINGMSG_H
#define _NOANALYSISMOMENTUMOUTPUTFILENAMESPECIFIEDWARNINGMSG_H

/**
 * CLASS noAnalysisOutputFileNameSpecifiedWarningMsg
 */

class noAnalysisOutputFileNameSpecifiedWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	noAnalysisOutputFileNameSpecifiedWarningMsg();

/**
 * Destructor
 */

	virtual ~noAnalysisOutputFileNameSpecifiedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NOANALYSISPROJECTIONOUTPUTFILENAMESPECIFIEDWARNINGMSG_H
#define _NOANALYSISPROJECTIONOUTPUTFILENAMESPECIFIEDWARNINGMSG_H

/**
 * CLASS noAnalysisProjectionOutputFileNameSpecifiedWarningMsg
 */

class noAnalysisProjectionOutputFileNameSpecifiedWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	noAnalysisProjectionOutputFileNameSpecifiedWarningMsg();

/**
 * Destructor
 */

	virtual ~noAnalysisProjectionOutputFileNameSpecifiedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ENABLEJUSTONEDISPLAYWARNINGMSG_H
#define _ENABLEJUSTONEDISPLAYWARNINGMSG_H

/**
 * CLASS enableJustOneDisplayWarningMsg
 */

class enableJustOneDisplayWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	enableJustOneDisplayWarningMsg();

/**
 * Destructor
 */

	virtual ~enableJustOneDisplayWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTSETPOINTWARNINGMSG_H
#define _CANNOTSETPOINTWARNINGMSG_H

/**
 * CLASS cannotSetPointWarningMsg
 */

class cannotSetPointWarningMsg : public analysisWarningMsg {

private:

	int index;

public:

/**
 * Default constructor
 */

	cannotSetPointWarningMsg();

/**
 * Constructor
 */

	cannotSetPointWarningMsg(int index);

/**
 * Destructor
 */

	virtual ~cannotSetPointWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _POSITIONISOUTOFSPACEWARNINGMSG_H
#define _POSITIONISOUTOFSPACEWARNINGMSG_H

/**
 * CLASS positionIsOutOfSpaceWarningMsg
 */

class positionIsOutOfSpaceWarningMsg : public analysisWarningMsg {

private:

	histogramSpace*   space;
	trackCoordinates* coordinates;

        positionIsOutOfSpaceWarningMsg(const positionIsOutOfSpaceWarningMsg&);
        positionIsOutOfSpaceWarningMsg& operator=(const positionIsOutOfSpaceWarningMsg&);
public:

/**
 * Default constructor
 */

	positionIsOutOfSpaceWarningMsg();

/**
 * Constructor
 */

	positionIsOutOfSpaceWarningMsg(histogramSpace* space, trackCoordinates* coordinates);

/**
 * Destructor
 */

	virtual ~positionIsOutOfSpaceWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFWRONGTHIRDDIMENSIONBORDERSFOUNDWARNINGMSG_H
#define _NUMBEROFWRONGTHIRDDIMENSIONBORDERSFOUNDWARNINGMSG_H

/**
 * CLASS numberOfWrongThirdDimensionBordersFoundWarningMsg
 */

class numberOfWrongThirdDimensionBordersFoundWarningMsg : public analysisWarningMsg {

private:

	int number;

public:

/**
 * Default constructor
 */

	numberOfWrongThirdDimensionBordersFoundWarningMsg();

/**
 * Constructor
 */

	numberOfWrongThirdDimensionBordersFoundWarningMsg(int number);

/**
 * Destructor
 */

	virtual ~numberOfWrongThirdDimensionBordersFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SETMAGNETFIELDFACTORWARNINGMSG_H
#define _SETMAGNETFIELDFACTORWARNINGMSG_H

/**
 * CLASS setMagnetfieldFactorWarningMsg
 */

class setMagnetfieldFactorWarningMsg : public analysisWarningMsg {

private:

	unsigned short stationIndex;
	double         magnetfieldFactor;

public:

/**
 * Default constructor
 */

	setMagnetfieldFactorWarningMsg();

/**
 * Constructor
 */

	setMagnetfieldFactorWarningMsg(unsigned short stationIndex, double magnetfieldFactor);

/**
 * Destructor
 */

	virtual ~setMagnetfieldFactorWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SETPRELUTRANGEWARNINGMSG_H
#define _SETPRELUTRANGEWARNINGMSG_H

/**
 * CLASS setPrelutRangeWarningMsg
 */

class setPrelutRangeWarningMsg : public analysisWarningMsg {

private:

	double rangeStart;
	double rangeStop;

public:

/**
 * Default constructor
 */

	setPrelutRangeWarningMsg();

/**
 * Constructor
 */

	setPrelutRangeWarningMsg(double rangeStart, double rangeStop);

/**
 * Destructor
 */

	virtual ~setPrelutRangeWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ANALYSISLUTCORRECTIONWARNINGMSG_H
#define _ANALYSISLUTCORRECTIONWARNINGMSG_H

/**
 * CLASS analysisLutCorrectionWarningMsg
 */

class analysisLutCorrectionWarningMsg : public analysisWarningMsg {

private:

	unsigned long borderCorrections;
	unsigned long borderPosCorrections;	

public:

/**
 * Default constructor
 */

	analysisLutCorrectionWarningMsg();

/**
 * Constructor
 */

	analysisLutCorrectionWarningMsg(unsigned long corrections, unsigned long posCorrections);

/**
 * Destructor
 */

	virtual ~analysisLutCorrectionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ROOTHISTOGRAMNOTFOUNDWARNINGMSG_H
#define _ROOTHISTOGRAMNOTFOUNDWARNINGMSG_H

/**
 * CLASS rootHistogramNotFoundWarningMsg
 */

class rootHistogramNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	rootHistogramNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~rootHistogramNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTUSETOROOTBEFOREINITIALIZINGWARNINGMSG_H
#define _CANNOTUSETOROOTBEFOREINITIALIZINGWARNINGMSG_H

/**
 * CLASS cannotUseToRootBeforeInitializingWarningMsg
 */

class cannotUseToRootBeforeInitializingWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotUseToRootBeforeInitializingWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotUseToRootBeforeInitializingWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINCORRECTLAYERDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINCORRECTLAYERDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInCorrectLayerDistributionWarningMsg
 */

class trackWithHitsInCorrectLayerDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithHitsInCorrectLayerDistributionWarningMsg(const trackWithHitsInCorrectLayerDistributionWarningMsg&);
        trackWithHitsInCorrectLayerDistributionWarningMsg& operator=(const trackWithHitsInCorrectLayerDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithHitsInCorrectLayerDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInCorrectLayerDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInCorrectLayerDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSWITHCORRECTSLOPEDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSWITHCORRECTSLOPEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsWithCorrectSlopeDistributionWarningMsg
 */

class trackWithHitsWithCorrectSlopeDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithHitsWithCorrectSlopeDistributionWarningMsg(const trackWithHitsWithCorrectSlopeDistributionWarningMsg&);
        trackWithHitsWithCorrectSlopeDistributionWarningMsg& operator=(const trackWithHitsWithCorrectSlopeDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithHitsWithCorrectSlopeDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsWithCorrectSlopeDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsWithCorrectSlopeDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINLAYERDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINLAYERDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInLayerDistributionWarningMsg
 */

class trackWithHitsInLayerDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithHitsInLayerDistributionWarningMsg(const trackWithHitsInLayerDistributionWarningMsg&);
        trackWithHitsInLayerDistributionWarningMsg& operator=(const trackWithHitsInLayerDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithHitsInLayerDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInLayerDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInLayerDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ARRAYTODISPLAYISNOTACCESSIBLEWARNINGMSG_H
#define _ARRAYTODISPLAYISNOTACCESSIBLEWARNINGMSG_H

/**
 * CLASS arrayToDisplayIsNotAccessibleWarningMsg
 */

class arrayToDisplayIsNotAccessibleWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	arrayToDisplayIsNotAccessibleWarningMsg();

/**
 * Destructor
 */

	virtual ~arrayToDisplayIsNotAccessibleWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHSIGNATUREDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHSIGNATUREDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithSignatureDistributionWarningMsg
 */

class trackWithSignatureDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithSignatureDistributionWarningMsg(const trackWithSignatureDistributionWarningMsg&);
        trackWithSignatureDistributionWarningMsg& operator=(const trackWithSignatureDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithSignatureDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithSignatureDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithSignatureDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINCORRECTCELLDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINCORRECTCELLDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInCorrectCellDistributionWarningMsg
 */

class trackWithHitsInCorrectCellDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithHitsInCorrectCellDistributionWarningMsg(const trackWithHitsInCorrectCellDistributionWarningMsg&);
        trackWithHitsInCorrectCellDistributionWarningMsg& operator=(const trackWithHitsInCorrectCellDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithHitsInCorrectCellDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInCorrectCellDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInCorrectCellDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHHITSINCELLDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHHITSINCELLDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithHitsInCellDistributionWarningMsg
 */

class trackWithHitsInCellDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithHitsInCellDistributionWarningMsg(const trackWithHitsInCellDistributionWarningMsg&);
        trackWithHitsInCellDistributionWarningMsg& operator=(const trackWithHitsInCellDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithHitsInCellDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithHitsInCellDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithHitsInCellDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELLWARNINGMSG_H
#define _MIDDLEDISTANCEOFMAXIMUMANDCORRECTCELLWARNINGMSG_H

/**
 * CLASS middleDistanceOfMaximumAndCorrectCellWarningMsg
 */

class middleDistanceOfMaximumAndCorrectCellWarningMsg : public analysisWarningMsg {

private:

	double distance;

public:

/**
 * Default constructor
 */

	middleDistanceOfMaximumAndCorrectCellWarningMsg();

/**
 * Constructor
 */

	middleDistanceOfMaximumAndCorrectCellWarningMsg(double distance);

/**
 * Destructor
 */

	virtual ~middleDistanceOfMaximumAndCorrectCellWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYERWARNINGMSG_H
#define _MIDDLEDISTANCEOFMAXIMUMANDCORRECTLAYERWARNINGMSG_H

/**
 * CLASS middleDistanceOfMaximumAndCorrectLayerWarningMsg
 */

class middleDistanceOfMaximumAndCorrectLayerWarningMsg : public analysisWarningMsg {

private:

	double distance;

public:

/**
 * Default constructor
 */

	middleDistanceOfMaximumAndCorrectLayerWarningMsg();

/**
 * Constructor
 */

	middleDistanceOfMaximumAndCorrectLayerWarningMsg(double distance);

/**
 * Destructor
 */

	virtual ~middleDistanceOfMaximumAndCorrectLayerWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTFILLNOTEXISTINGDISPLAYWARNINGMSG_H
#define _CANNOTFILLNOTEXISTINGDISPLAYWARNINGMSG_H

/**
 * CLASS cannotFillNotExistingDisplayWarningMsg
 */

class cannotFillNotExistingDisplayWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotFillNotExistingDisplayWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotFillNotExistingDisplayWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYCANNOTBEREADFROMFILEWARNINGMSG_H
#define _DISPLAYCANNOTBEREADFROMFILEWARNINGMSG_H

/**
 * CLASS displayCannotBeReadFromFileWarningMsg
 */

class displayCannotBeReadFromFileWarningMsg : public analysisWarningMsg {

private:

	std::string name;
	std::string title;
	std::string fileName;

public:

/**
 * Default constructor
 */

	displayCannotBeReadFromFileWarningMsg();

/**
 * Constructor
 */

	displayCannotBeReadFromFileWarningMsg(std::string name, std::string title, std::string fileName);

/**
 * Destructor
 */

	virtual ~displayCannotBeReadFromFileWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DISPLAYNOTACCESSIBLEWARNINGMSG_H
#define _DISPLAYNOTACCESSIBLEWARNINGMSG_H

/**
 * CLASS displayNotAccessibleWarningMsg
 */

class displayNotAccessibleWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	displayNotAccessibleWarningMsg();

/**
 * Destructor
 */

	virtual ~displayNotAccessibleWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITENOTENABLEDLAYERWARNINGMSG_H
#define _CANNOTWRITENOTENABLEDLAYERWARNINGMSG_H

/**
 * CLASS cannotWriteNotEnabledLayerWarningMsg
 */

class cannotWriteNotEnabledLayerWarningMsg : public analysisWarningMsg {

private:

	unsigned short layerToWrite;
	unsigned short enabledLayer;

public:

/**
 * Default constructor
 */

	cannotWriteNotEnabledLayerWarningMsg();

/**
 * Constructor
 */

	cannotWriteNotEnabledLayerWarningMsg(unsigned short layerToWrite, unsigned short enabledLayer);

/**
 * Destructor
 */

	virtual ~cannotWriteNotEnabledLayerWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTSHOWNOTENABLEDLAYERWARNINGMSG_H
#define _CANNOTSHOWNOTENABLEDLAYERWARNINGMSG_H

/**
 * CLASS cannotShowNotEnabledLayerWarningMsg
 */

class cannotShowNotEnabledLayerWarningMsg : public analysisWarningMsg {

private:

	unsigned short layerToShow;
	unsigned short enabledLayer;

public:

/**
 * Default constructor
 */

	cannotShowNotEnabledLayerWarningMsg();

/**
 * Constructor
 */

	cannotShowNotEnabledLayerWarningMsg(unsigned short layerToShow, unsigned short enabledLayer);

/**
 * Destructor
 */

	virtual ~cannotShowNotEnabledLayerWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ENABLELAYERISOUTOFRANGEWARNINGMSG_H
#define _ENABLELAYERISOUTOFRANGEWARNINGMSG_H

/**
 * CLASS enableLayerIsOutOfRangeWarningMsg
 */

class enableLayerIsOutOfRangeWarningMsg : public analysisWarningMsg {

private:

	unsigned short layer;
	unsigned short layerMin;
	unsigned short layerMax;

public:

/**
 * Default constructor
 */

	enableLayerIsOutOfRangeWarningMsg();

/**
 * Constructor
 */

	enableLayerIsOutOfRangeWarningMsg(unsigned short layer, unsigned short LayerMin, unsigned short LayerMax);

/**
 * Destructor
 */

	virtual ~enableLayerIsOutOfRangeWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTSHOWHISTOGRAMWITHOUTWRITINGTOROOTWARNINGMSG_H
#define _CANNOTSHOWHISTOGRAMWITHOUTWRITINGTOROOTWARNINGMSG_H

/**
 * CLASS cannotShowHistogramWithoutWritingToRootWarningMsg
 */

class cannotShowHistogramWithoutWritingToRootWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	cannotShowHistogramWithoutWritingToRootWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotShowHistogramWithoutWritingToRootWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKWITHMOMENTAERRORDISTRIBUTIONWARNINGMSG_H
#define _TRACKWITHMOMENTAERRORDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS trackWithMomentaErrorDistributionWarningMsg
 */

class trackWithMomentaErrorDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        trackWithMomentaErrorDistributionWarningMsg(const trackWithMomentaErrorDistributionWarningMsg&);
        trackWithMomentaErrorDistributionWarningMsg& operator=(const trackWithMomentaErrorDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	trackWithMomentaErrorDistributionWarningMsg();

/**
 * Constructor
 */

	trackWithMomentaErrorDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~trackWithMomentaErrorDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDMOMENTAXDISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDMOMENTAXDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedMomentaXDistributionWarningMsg
 */

class quantizedMomentaXDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;
	double         minimum;
	double         maximum;

        quantizedMomentaXDistributionWarningMsg(const quantizedMomentaXDistributionWarningMsg&);
        quantizedMomentaXDistributionWarningMsg& operator=(const quantizedMomentaXDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedMomentaXDistributionWarningMsg();

/**
 * Constructor
 */

	quantizedMomentaXDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max);

/**
 * Destructor
 */

	virtual ~quantizedMomentaXDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDMOMENTAYDISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDMOMENTAYDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedMomentaYDistributionWarningMsg
 */

class quantizedMomentaYDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;
	double         minimum;
	double         maximum;

        quantizedMomentaYDistributionWarningMsg(const quantizedMomentaYDistributionWarningMsg&);
        quantizedMomentaYDistributionWarningMsg& operator=(const quantizedMomentaYDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedMomentaYDistributionWarningMsg();

/**
 * Constructor
 */

	quantizedMomentaYDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max);

/**
 * Destructor
 */

	virtual ~quantizedMomentaYDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDMOMENTAZDISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDMOMENTAZDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedMomentaZDistributionWarningMsg
 */

class quantizedMomentaZDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;
	double         minimum;
	double         maximum;

        quantizedMomentaZDistributionWarningMsg(const quantizedMomentaZDistributionWarningMsg&);
        quantizedMomentaZDistributionWarningMsg& operator=(const quantizedMomentaZDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedMomentaZDistributionWarningMsg();

/**
 * Constructor
 */

	quantizedMomentaZDistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries, double min, double max);

/**
 * Destructor
 */

	virtual ~quantizedMomentaZDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDHOUGHSPACEDIM1DISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDHOUGHSPACEDIM1DISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedHoughspaceDim1DistributionWarningMsg
 */

class quantizedHoughspaceDim1DistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        quantizedHoughspaceDim1DistributionWarningMsg(const quantizedHoughspaceDim1DistributionWarningMsg&);
        quantizedHoughspaceDim1DistributionWarningMsg& operator=(const quantizedHoughspaceDim1DistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedHoughspaceDim1DistributionWarningMsg();

/**
 * Constructor
 */

	quantizedHoughspaceDim1DistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~quantizedHoughspaceDim1DistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDHOUGHSPACEDIM2DISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDHOUGHSPACEDIM2DISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedHoughspaceDim2DistributionWarningMsg
 */

class quantizedHoughspaceDim2DistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        quantizedHoughspaceDim2DistributionWarningMsg(const quantizedHoughspaceDim2DistributionWarningMsg&);
        quantizedHoughspaceDim2DistributionWarningMsg& operator=(const quantizedHoughspaceDim2DistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedHoughspaceDim2DistributionWarningMsg();

/**
 * Constructor
 */

	quantizedHoughspaceDim2DistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~quantizedHoughspaceDim2DistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDHOUGHSPACEDIM3DISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDHOUGHSPACEDIM3DISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedHoughspaceDim3DistributionWarningMsg
 */

class quantizedHoughspaceDim3DistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned short numberOfEntries;

        quantizedHoughspaceDim3DistributionWarningMsg(const quantizedHoughspaceDim3DistributionWarningMsg&);
        quantizedHoughspaceDim3DistributionWarningMsg& operator=(const quantizedHoughspaceDim3DistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedHoughspaceDim3DistributionWarningMsg();

/**
 * Constructor
 */

	quantizedHoughspaceDim3DistributionWarningMsg(unsigned int* distribution, unsigned short numberOfEntries);

/**
 * Destructor
 */

	virtual ~quantizedHoughspaceDim3DistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDMOMENTADISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDMOMENTADISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedMomentaDistributionWarningMsg
 */

class quantizedMomentaDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        quantizedMomentaDistributionWarningMsg(const quantizedMomentaDistributionWarningMsg&);
        quantizedMomentaDistributionWarningMsg& operator=(const quantizedMomentaDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedMomentaDistributionWarningMsg();

/**
 * Constructor
 */

	quantizedMomentaDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~quantizedMomentaDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _QUANTIZEDHOUGHSPACEDISTRIBUTIONWARNINGMSG_H
#define _QUANTIZEDHOUGHSPACEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS quantizedHoughspaceDistributionWarningMsg
 */

class quantizedHoughspaceDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        quantizedHoughspaceDistributionWarningMsg(const quantizedHoughspaceDistributionWarningMsg&);
        quantizedHoughspaceDistributionWarningMsg& operator=(const quantizedHoughspaceDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	quantizedHoughspaceDistributionWarningMsg();

/**
 * Constructor
 */

	quantizedHoughspaceDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~quantizedHoughspaceDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSWHICHCANNOTBEFOUNDWARNINGMSG_H
#define _NUMBEROFTRACKSWHICHCANNOTBEFOUNDWARNINGMSG_H

/**
 * CLASS numberOfTracksWhichCannotBeFoundWarningMsg
 */

class numberOfTracksWhichCannotBeFoundWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfTracks;

public:

/**
 * Default constructor
 */

	numberOfTracksWhichCannotBeFoundWarningMsg();

/**
 * Constructor
 */

	numberOfTracksWhichCannotBeFoundWarningMsg(unsigned long numberOfTracks);

/**
 * Destructor
 */

	virtual ~numberOfTracksWhichCannotBeFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIM1PEAKDISTANCEDISTRIBUTIONWARNINGMSG_H
#define _DIM1PEAKDISTANCEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS dim1PeakDistanceDistributionWarningMsg
 */

class dim1PeakDistanceDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        dim1PeakDistanceDistributionWarningMsg(const dim1PeakDistanceDistributionWarningMsg&);
        dim1PeakDistanceDistributionWarningMsg& operator=(const dim1PeakDistanceDistributionWarningMsg&);
        
public:

/**
 * Default constructor
 */

	dim1PeakDistanceDistributionWarningMsg();

/**
 * Constructor
 */

	dim1PeakDistanceDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~dim1PeakDistanceDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIM2PEAKDISTANCEDISTRIBUTIONWARNINGMSG_H
#define _DIM2PEAKDISTANCEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS dim2PeakDistanceDistributionWarningMsg
 */

class dim2PeakDistanceDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        dim2PeakDistanceDistributionWarningMsg(const dim2PeakDistanceDistributionWarningMsg&);
        dim2PeakDistanceDistributionWarningMsg& operator=(const dim2PeakDistanceDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	dim2PeakDistanceDistributionWarningMsg();

/**
 * Constructor
 */

	dim2PeakDistanceDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~dim2PeakDistanceDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIM3PEAKDISTANCEDISTRIBUTIONWARNINGMSG_H
#define _DIM3PEAKDISTANCEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS dim3PeakDistanceDistributionWarningMsg
 */

class dim3PeakDistanceDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        dim3PeakDistanceDistributionWarningMsg(const dim3PeakDistanceDistributionWarningMsg&);
        dim3PeakDistanceDistributionWarningMsg& operator=(const dim3PeakDistanceDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	dim3PeakDistanceDistributionWarningMsg();

/**
 * Constructor
 */

	dim3PeakDistanceDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~dim3PeakDistanceDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _ACCUMULATEDPEAKDISTANCEDISTRIBUTIONWARNINGMSG_H
#define _ACCUMULATEDPEAKDISTANCEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS accumulatedPeakDistanceDistributionWarningMsg
 */

class accumulatedPeakDistanceDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        accumulatedPeakDistanceDistributionWarningMsg(const accumulatedPeakDistanceDistributionWarningMsg&);
        accumulatedPeakDistanceDistributionWarningMsg& operator=(const accumulatedPeakDistanceDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	accumulatedPeakDistanceDistributionWarningMsg();

/**
 * Constructor
 */

	accumulatedPeakDistanceDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~accumulatedPeakDistanceDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSWITHGOODPRELUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFTRACKSWITHGOODPRELUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfTracksWithGoodPrelutSignatureWarningMsg
 */

class numberOfTracksWithGoodPrelutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfTracks;

public:

/**
 * Default constructor
 */

	numberOfTracksWithGoodPrelutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfTracksWithGoodPrelutSignatureWarningMsg(unsigned long numberOfTracks);

/**
 * Destructor
 */

	virtual ~numberOfTracksWithGoodPrelutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSWITHGOODLUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFTRACKSWITHGOODLUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfTracksWithGoodLutSignatureWarningMsg
 */

class numberOfTracksWithGoodLutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfTracks;

public:

/**
 * Default constructor
 */

	numberOfTracksWithGoodLutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfTracksWithGoodLutSignatureWarningMsg(unsigned long numberOfTracks);

/**
 * Destructor
 */

	virtual ~numberOfTracksWithGoodLutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFGOODPRELUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFGOODPRELUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfGoodPrelutSignatureWarningMsg
 */

class numberOfGoodPrelutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfSignatures;

public:

/**
 * Default constructor
 */

	numberOfGoodPrelutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfGoodPrelutSignatureWarningMsg(unsigned long numberOfSignatures);

/**
 * Destructor
 */

	virtual ~numberOfGoodPrelutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFGOODLUTSIGNATUREWARNINGMSG_H
#define _NUMBEROFGOODLUTSIGNATUREWARNINGMSG_H

/**
 * CLASS numberOfGoodLutSignatureWarningMsg
 */

class numberOfGoodLutSignatureWarningMsg : public analysisWarningMsg {

private:

	unsigned long numberOfSignatures;

public:

/**
 * Default constructor
 */

	numberOfGoodLutSignatureWarningMsg();

/**
 * Constructor
 */

	numberOfGoodLutSignatureWarningMsg(unsigned long numberOfSignatures);

/**
 * Destructor
 */

	virtual ~numberOfGoodLutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _GOODPRELUTSIGNATUREWARNINGMSG_H
#define _GOODPRELUTSIGNATUREWARNINGMSG_H

/**
 * CLASS goodPrelutSignatureWarningMsg
 */

class goodPrelutSignatureWarningMsg : public analysisWarningMsg {

private:

	std::string stringTable;

public:

/**
 * Default constructor
 */

	goodPrelutSignatureWarningMsg();

/**
 * Constructor
 */

	goodPrelutSignatureWarningMsg(std::string stringTable);

/**
 * Destructor
 */

	virtual ~goodPrelutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _GOODLUTSIGNATUREWARNINGMSG_H
#define _GOODLUTSIGNATUREWARNINGMSG_H

/**
 * CLASS goodLutSignatureWarningMsg
 */

class goodLutSignatureWarningMsg : public analysisWarningMsg {

private:

	std::string stringTable;

public:

/**
 * Default constructor
 */

	goodLutSignatureWarningMsg();

/**
 * Constructor
 */

	goodLutSignatureWarningMsg(std::string stringTable);

/**
 * Destructor
 */

	virtual ~goodLutSignatureWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _USEDTABLESWARNINGMSG_H
#define _USEDTABLESWARNINGMSG_H

/**
 * CLASS usedTablesWarningMsg
 */

class usedTablesWarningMsg : public analysisWarningMsg {

private:

	std::string stringTable;

public:

/**
 * Default constructor
 */

	usedTablesWarningMsg();

/**
 * Constructor
 */

	usedTablesWarningMsg(std::string stringTable);

/**
 * Destructor
 */

	virtual ~usedTablesWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SAMEHOUGHSPACECELLDISTRIBUTIONWARNINGMSG_H
#define _SAMEHOUGHSPACECELLDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS sameHoughspaceCellDistributionWarningMsg
 */

class sameHoughspaceCellDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        sameHoughspaceCellDistributionWarningMsg(const sameHoughspaceCellDistributionWarningMsg&);
        sameHoughspaceCellDistributionWarningMsg& operator=(const sameHoughspaceCellDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	sameHoughspaceCellDistributionWarningMsg();

/**
 * Constructor
 */

	sameHoughspaceCellDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~sameHoughspaceCellDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _VOLUMENOTFOUNDWARNINGMSG_H
#define _VOLUMENOTFOUNDWARNINGMSG_H

/**
 * CLASS volumeNotFoundWarningMsg
 */

class volumeNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	volumeNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~volumeNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _LISTOFVOLUMESNOTFOUNDWARNINGMSG_H
#define _LISTOFVOLUMESNOTFOUNDWARNINGMSG_H

/**
 * CLASS listOfVolumesNotFoundWarningMsg
 */

class listOfVolumesNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	listOfVolumesNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~listOfVolumesNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CURRENTTRACKNOTFOUNDWARNINGMSG_H
#define _CURRENTTRACKNOTFOUNDWARNINGMSG_H

/**
 * CLASS actualTrackNotFoundWarningMsg
 */

class actualTrackNotFoundWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	actualTrackNotFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~actualTrackNotFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TIMEVIOLATIONWARNINGMSG_H
#define _TIMEVIOLATIONWARNINGMSG_H

/**
 * CLASS timeViolationWarningMsg
 */

class timeViolationWarningMsg : public analysisWarningMsg {

private:

	double time1;
	double time2;

public:

/**
 * Default constructor
 */

	timeViolationWarningMsg();

/**
 * Constructor
 */

	timeViolationWarningMsg(double time1, double time2);

/**
 * Destructor
 */

	virtual ~timeViolationWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKSINALLLAYERSWARNINGMSG_H
#define _NUMBEROFTRACKSINALLLAYERSWARNINGMSG_H

/**
 * CLASS numberOfTracksInAllLayersWarningMsg
 */

class numberOfTracksInAllLayersWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;
	unsigned int  numberOfSummedEventsToAverage;

        numberOfTracksInAllLayersWarningMsg(const numberOfTracksInAllLayersWarningMsg&);
        numberOfTracksInAllLayersWarningMsg& operator=(const numberOfTracksInAllLayersWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfTracksInAllLayersWarningMsg();

/**
 * Constructor
 */

	numberOfTracksInAllLayersWarningMsg(unsigned int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfTracksInAllLayersWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFTRACKDENSITIESINALLLAYERSWARNINGMSG_H
#define _NUMBEROFTRACKDENSITIESINALLLAYERSWARNINGMSG_H

/**
 * CLASS numberOfTrackDensitiesInAllLayersWarningMsg
 */

class numberOfTrackDensitiesInAllLayersWarningMsg : public analysisWarningMsg {

private:

	int*          distribution;
	unsigned int  numberOfEntries;
	unsigned int  numberOfSummedEventsToAverage;

        numberOfTrackDensitiesInAllLayersWarningMsg(const numberOfTrackDensitiesInAllLayersWarningMsg&);
        numberOfTrackDensitiesInAllLayersWarningMsg& operator=(const numberOfTrackDensitiesInAllLayersWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfTrackDensitiesInAllLayersWarningMsg();

/**
 * Constructor
 */

	numberOfTrackDensitiesInAllLayersWarningMsg(int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfTrackDensitiesInAllLayersWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFMINIMALTRACKSINALLCOLUMNSWARNINGMSG_H
#define _NUMBEROFMINIMALTRACKSINALLCOLUMNSWARNINGMSG_H

/**
 * CLASS numberOfMinimalTracksInAllColumnsWarningMsg
 */

class numberOfMinimalTracksInAllColumnsWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        numberOfMinimalTracksInAllColumnsWarningMsg(const numberOfMinimalTracksInAllColumnsWarningMsg&);
        numberOfMinimalTracksInAllColumnsWarningMsg& operator=(const numberOfMinimalTracksInAllColumnsWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfMinimalTracksInAllColumnsWarningMsg();

/**
 * Constructor
 */

	numberOfMinimalTracksInAllColumnsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~numberOfMinimalTracksInAllColumnsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFAVERAGETRACKSINALLCOLUMNSWARNINGMSG_H
#define _NUMBEROFAVERAGETRACKSINALLCOLUMNSWARNINGMSG_H

/**
 * CLASS numberOfAverageTracksInAllColumnsWarningMsg
 */

class numberOfAverageTracksInAllColumnsWarningMsg : public analysisWarningMsg {

private:

	double*      distribution;
	unsigned int numberOfEntries;
	unsigned int numberOfSummedEventsToAverage;

        numberOfAverageTracksInAllColumnsWarningMsg(const numberOfAverageTracksInAllColumnsWarningMsg&);
        numberOfAverageTracksInAllColumnsWarningMsg& operator=(const numberOfAverageTracksInAllColumnsWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfAverageTracksInAllColumnsWarningMsg();

/**
 * Constructor
 */

	numberOfAverageTracksInAllColumnsWarningMsg(double* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfAverageTracksInAllColumnsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFMAXIMALTRACKSINALLCOLUMNSWARNINGMSG_H
#define _NUMBEROFMAXIMALTRACKSINALLCOLUMNSWARNINGMSG_H

/**
 * CLASS numberOfMaximalTracksInAllColumnsWarningMsg
 */

class numberOfMaximalTracksInAllColumnsWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        numberOfMaximalTracksInAllColumnsWarningMsg(const numberOfMaximalTracksInAllColumnsWarningMsg&);
        numberOfMaximalTracksInAllColumnsWarningMsg& operator=(const numberOfMaximalTracksInAllColumnsWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfMaximalTracksInAllColumnsWarningMsg();

/**
 * Constructor
 */

	numberOfMaximalTracksInAllColumnsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~numberOfMaximalTracksInAllColumnsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SIZEOFCOLUMNSEPARATORFIFOSWARNINGMSG_H
#define _SIZEOFCOLUMNSEPARATORFIFOSWARNINGMSG_H

/**
 * CLASS sizeOfColumnSeparatorFifosWarningMsg
 */

class sizeOfColumnSeparatorFifosWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        sizeOfColumnSeparatorFifosWarningMsg(const sizeOfColumnSeparatorFifosWarningMsg&);
        sizeOfColumnSeparatorFifosWarningMsg& operator=(const sizeOfColumnSeparatorFifosWarningMsg&);
public:

/**
 * Default constructor
 */

	sizeOfColumnSeparatorFifosWarningMsg();

/**
 * Constructor
 */

	sizeOfColumnSeparatorFifosWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~sizeOfColumnSeparatorFifosWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFMINIMALTRACKSINALLROWSWARNINGMSG_H
#define _NUMBEROFMINIMALTRACKSINALLROWSWARNINGMSG_H

/**
 * CLASS numberOfMinimalTracksInAllRowsWarningMsg
 */

class numberOfMinimalTracksInAllRowsWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;
       
        numberOfMinimalTracksInAllRowsWarningMsg(const numberOfMinimalTracksInAllRowsWarningMsg&);
        numberOfMinimalTracksInAllRowsWarningMsg& operator=(const numberOfMinimalTracksInAllRowsWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfMinimalTracksInAllRowsWarningMsg();

/**
 * Constructor
 */

	numberOfMinimalTracksInAllRowsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~numberOfMinimalTracksInAllRowsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFAVERAGETRACKSINALLROWSWARNINGMSG_H
#define _NUMBEROFAVERAGETRACKSINALLROWSWARNINGMSG_H

/**
 * CLASS numberOfAverageTracksInAllRowsWarningMsg
 */

class numberOfAverageTracksInAllRowsWarningMsg : public analysisWarningMsg {

private:

	double*      distribution;
	unsigned int numberOfEntries;
	unsigned int numberOfSummedEventsToAverage;

        numberOfAverageTracksInAllRowsWarningMsg(const numberOfAverageTracksInAllRowsWarningMsg&);
        numberOfAverageTracksInAllRowsWarningMsg& operator=(const numberOfAverageTracksInAllRowsWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfAverageTracksInAllRowsWarningMsg();

/**
 * Constructor
 */

	numberOfAverageTracksInAllRowsWarningMsg(double* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~numberOfAverageTracksInAllRowsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFMAXIMALTRACKSINALLROWSWARNINGMSG_H
#define _NUMBEROFMAXIMALTRACKSINALLROWSWARNINGMSG_H

/**
 * CLASS numberOfMaximalTracksInAllRowsWarningMsg
 */

class numberOfMaximalTracksInAllRowsWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        numberOfMaximalTracksInAllRowsWarningMsg(const numberOfMaximalTracksInAllRowsWarningMsg&);
        numberOfMaximalTracksInAllRowsWarningMsg& operator=(const numberOfMaximalTracksInAllRowsWarningMsg&);
public:

/**
 * Default constructor
 */

	numberOfMaximalTracksInAllRowsWarningMsg();

/**
 * Constructor
 */

	numberOfMaximalTracksInAllRowsWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~numberOfMaximalTracksInAllRowsWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _SIZEOFROWSEPARATORFIFOSWARNINGMSG_H
#define _SIZEOFROWSEPARATORFIFOSWARNINGMSG_H

/**
 * CLASS sizeOfRowSeparatorFifosWarningMsg
 */

class sizeOfRowSeparatorFifosWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;

        sizeOfRowSeparatorFifosWarningMsg(const sizeOfRowSeparatorFifosWarningMsg&);
        sizeOfRowSeparatorFifosWarningMsg& operator=(const sizeOfRowSeparatorFifosWarningMsg&);
public:

/**
 * Default constructor
 */

	sizeOfRowSeparatorFifosWarningMsg();

/**
 * Constructor
 */

	sizeOfRowSeparatorFifosWarningMsg(unsigned int* distribution, unsigned int numberOfEntries);

/**
 * Destructor
 */

	virtual ~sizeOfRowSeparatorFifosWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _YOUCANNOTENABLETRACKWITHSAMEMOMENTAANDCOORDINATEDISTRIBUTIONWARNINGMSG_H
#define _YOUCANNOTENABLETRACKWITHSAMEMOMENTAANDCOORDINATEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg
 */

class youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg();

/**
 * Destructor
 */

	virtual ~youCannotEnableTrackWithSameMomentaAndCoordinateDistributionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _RELATIVEDISPLAYNOTACCESSIBLEWARNINGMSG_H
#define _RELATIVEDISPLAYNOTACCESSIBLEWARNINGMSG_H

/**
 * CLASS relativeDisplayNotAccessibleWarningMsg
 */

class relativeDisplayNotAccessibleWarningMsg : public analysisWarningMsg {

public:

/**
 * Default constructor
 */

	relativeDisplayNotAccessibleWarningMsg();

/**
 * Destructor
 */

	virtual ~relativeDisplayNotAccessibleWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _NUMBEROFPRELUTRANGESFOUNDWARNINGMSG_H
#define _NUMBEROFPRELUTRANGESFOUNDWARNINGMSG_H

/**
 * CLASS numberOfPrelutRangesFoundWarningMsg
 */

class numberOfPrelutRangesFoundWarningMsg : public analysisWarningMsg {

private:

	unsigned short numberofPrelutRanges;

public:

/**
 * Default constructor
 */

	numberOfPrelutRangesFoundWarningMsg();

/**
 * Constructor
 */

	numberOfPrelutRangesFoundWarningMsg(unsigned short numberofPrelutRanges);

/**
 * Destructor
 */

	virtual ~numberOfPrelutRangesFoundWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MAXIMUMPRELUTRANGEPERCENTAGEWARNINGMSG_H
#define _MAXIMUMPRELUTRANGEPERCENTAGEWARNINGMSG_H

/**
 * CLASS maximumPrelutRangePercentageWarningMsg
 */

class maximumPrelutRangePercentageWarningMsg : public analysisWarningMsg {

private:

	unsigned short percentage;

public:

/**
 * Default constructor
 */

	maximumPrelutRangePercentageWarningMsg();

/**
 * Constructor
 */

	maximumPrelutRangePercentageWarningMsg(unsigned short percentage);

/**
 * Destructor
 */

	virtual ~maximumPrelutRangePercentageWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MEANPRELUTRANGEENTRIESPERHITWARNINGMSG_H
#define _MEANPRELUTRANGEENTRIESPERHITWARNINGMSG_H

/**
 * CLASS meanPrelutRangeEntriesPerHitWarningMsg
 */

class meanPrelutRangeEntriesPerHitWarningMsg : public analysisWarningMsg {

private:

	double mean;

public:

/**
 * Default constructor
 */

	meanPrelutRangeEntriesPerHitWarningMsg();

/**
 * Constructor
 */

	meanPrelutRangeEntriesPerHitWarningMsg(double mean);

/**
 * Destructor
 */

	virtual ~meanPrelutRangeEntriesPerHitWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITREADOUTDISTRIBUTIONWARNINGMSG_H
#define _HITREADOUTDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS hitReadoutDistributionWarningMsg
 */

class hitReadoutDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;
	unsigned int  numberOfSummedEventsToAverage;

        hitReadoutDistributionWarningMsg(const hitReadoutDistributionWarningMsg&);
        hitReadoutDistributionWarningMsg& operator=(const hitReadoutDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	hitReadoutDistributionWarningMsg();

/**
 * Constructor
 */

	hitReadoutDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~hitReadoutDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _HITREADOUTMEANDISTRIBUTIONDISTRIBUTIONWARNINGMSG_H
#define _HITREADOUTMEANDISTRIBUTIONDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS hitReadoutMeanDistributionWarningMsg
 */

class hitReadoutMeanDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int* distribution;
	unsigned int  numberOfEntries;
	unsigned int  numberOfSummedEventsToAverage;

        hitReadoutMeanDistributionWarningMsg(const hitReadoutMeanDistributionWarningMsg&);
        hitReadoutMeanDistributionWarningMsg& operator=(const hitReadoutMeanDistributionWarningMsg&);
public:

/**
 * Default constructor
 */

	hitReadoutMeanDistributionWarningMsg();

/**
 * Constructor
 */

	hitReadoutMeanDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage);

/**
 * Destructor
 */

	virtual ~hitReadoutMeanDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _FPGAHISTOGRAMPROCESSINGTIMEDISTRIBUTIONWARNINGMSG_H
#define _FPGAHISTOGRAMPROCESSINGTIMEDISTRIBUTIONWARNINGMSG_H

/**
 * CLASS fpgaHistogramProcessingTimeDistributionWarningMsg
 */

class fpgaHistogramProcessingTimeDistributionWarningMsg : public analysisWarningMsg {

private:

	unsigned int*  distribution;
	unsigned int   numberOfEntries;
	unsigned int   numberOfSummedEventsToAverage;
	bool           readoutColumnsInParallel;
	unsigned short histogramDim1;
	unsigned short histogramDim2;

        fpgaHistogramProcessingTimeDistributionWarningMsg(const fpgaHistogramProcessingTimeDistributionWarningMsg&);
	fpgaHistogramProcessingTimeDistributionWarningMsg& operator=(const fpgaHistogramProcessingTimeDistributionWarningMsg&);

public:

/**
 * Default constructor
 */

	fpgaHistogramProcessingTimeDistributionWarningMsg();

/**
 * Constructor
 */

	fpgaHistogramProcessingTimeDistributionWarningMsg(unsigned int* distribution, unsigned int numberOfEntries, unsigned int numberOfSummedEventsToAverage, bool readoutColumnsInParallel, unsigned short histogramDim1, unsigned short histogramDim2);

/**
 * Destructor
 */

	virtual ~fpgaHistogramProcessingTimeDistributionWarningMsg();

/**
 * This method display a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MEANVALUEWARNINGMSG_H
#define _MEANVALUEWARNINGMSG_H

/**
 * CLASS meanValueWarningMsg
 */

class meanValueWarningMsg : public analysisWarningMsg {

private:

	double      mean;
	std::string dimension;

public:

/**
 * Default constructor
 */

	meanValueWarningMsg();

/**
 * Constructor
 */

	meanValueWarningMsg(double mean, std::string dimension);

/**
 * Destructor
 */

	virtual ~meanValueWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
