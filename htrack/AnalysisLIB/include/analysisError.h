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
///     - derived from errorHandling
///     - class for errors occuring while doing some analysis operations
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:07:01 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ANALYSISERROR_H
#define _ANALYSISERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS analysisError											*
 * **************************************************************/

class analysisError : public errorHandling {

public:

/**
 * Default constructor
 */

	analysisError();

/**
 * Default destructor
 */

	virtual ~analysisError();

};

#endif


#ifndef _WINDOWNOTFOUNDERROR_H
#define _WINDOWNOTFOUNDERROR_H

/**
 * CLASS windowNotFoundError
 */

class windowNotFoundError : public analysisError {

public:

/**
 * Default constructor
 */

	windowNotFoundError();

/**
 * Destructor
 */

	virtual ~windowNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _FAKEPASSINGGRADINGRERROR_H
#define _FAKEPASSINGGRADINGRERROR_H

/**
 * CLASS fakePassingGradingRError
 */

class fakePassingGradingRError : public analysisError {

public:

/**
 * Default constructor
 */

	fakePassingGradingRError();

/**
 * Destructor
 */

	virtual ~fakePassingGradingRError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MAGNETICFIELDNOTFOUNDERROR_H
#define _MAGNETICFIELDNOTFOUNDERROR_H

/**
 * CLASS magneticFieldNotFoundError
 */

class magneticFieldNotFoundError : public analysisError {

public:

/**
 * Default constructor
 */

	magneticFieldNotFoundError();

/**
 * Destructor
 */

	virtual ~magneticFieldNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETFIELDANALYSERERROR_H
#define _CANNOTACCESSMAGNETFIELDANALYSERERROR_H

/**
 * CLASS cannotAccessMagnetfieldAnalyserError
 */

class cannotAccessMagnetfieldAnalyserError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessMagnetfieldAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessMagnetfieldAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSDISPLAYINFOERROR_H
#define _CANNOTACCESSDISPLAYINFOERROR_H

/**
 * CLASS cannotAccessDisplayInfoError
 */

class cannotAccessDisplayInfoError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessDisplayInfoError();

/**
 * Destructor
 */

	virtual ~cannotAccessDisplayInfoError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSFINDABLETRACKSMEMORYERROR_H
#define _CANNOTACCESSFINDABLETRACKSMEMORYERROR_H

/**
 * CLASS cannotAccessFindableTracksMemoryError
 */

class cannotAccessFindableTracksMemoryError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessFindableTracksMemoryError();

/**
 * Destructor
 */

	virtual ~cannotAccessFindableTracksMemoryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSFIRSTANALYSISLUTERROR_H
#define _CANNOTACCESSFIRSTANALYSISLUTERROR_H

/**
 * CLASS cannotAccessFirstAnalysisLutError
 */

class cannotAccessFirstAnalysisLutError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessFirstAnalysisLutError();

/**
 * Destructor
 */

	virtual ~cannotAccessFirstAnalysisLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSSECONDANALYSISLUTERROR_H
#define _CANNOTACCESSSECONDANALYSISLUTERROR_H

/**
 * CLASS cannotAccessSecondAnalysisLutError
 */

class cannotAccessSecondAnalysisLutError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessSecondAnalysisLutError();

/**
 * Destructor
 */

	virtual ~cannotAccessSecondAnalysisLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSALLMAGNETFIELDDISTRIBUTIONSERROR_H
#define _CANNOTACCESSALLMAGNETFIELDDISTRIBUTIONSERROR_H

/**
 * CLASS cannotAccessAllMagnetfieldFactorDistributionsError
 */

class cannotAccessAllMagnetfieldFactorDistributionsError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessAllMagnetfieldFactorDistributionsError();

/**
 * Destructor
 */

	virtual ~cannotAccessAllMagnetfieldFactorDistributionsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETFIELDFACTORANALYSERERROR_H
#define _CANNOTACCESSMAGNETFIELDFACTORANALYSERERROR_H

/**
 * CLASS cannotAccessMagnetfieldFactorAnalyserError
 */

class cannotAccessMagnetfieldFactorAnalyserError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessMagnetfieldFactorAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessMagnetfieldFactorAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSPRELUTRANGEANALYSERERROR_H
#define _CANNOTACCESSPRELUTRANGEANALYSERERROR_H

/**
 * CLASS cannotAccessPrelutRangeAnalyserError
 */

class cannotAccessPrelutRangeAnalyserError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessPrelutRangeAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessPrelutRangeAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _INDEXISOUTOFDISPLAYRANGEERROR_H
#define _INDEXISOUTOFDISPLAYRANGEERROR_H

/**
 * CLASS indexIsOutOfDisplayRangeError
 */

class indexIsOutOfDisplayRangeError : public analysisError {

protected:

	unsigned short index;
	unsigned short numberOfDisplays;

public:

/**
 * Default constructor
 */

	indexIsOutOfDisplayRangeError();

/**
 * Constructor
 */

	indexIsOutOfDisplayRangeError(unsigned short index, unsigned short numberOfDisplays);

/**
 * Destructor
 */

	virtual ~indexIsOutOfDisplayRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _INDEXISOUTOFFACTORRANGEERROR_H
#define _INDEXISOUTOFFACTORRANGEERROR_H

/**
 * CLASS indexIsOutOfFactorRangeError
 */

class indexIsOutOfFactorRangeError : public analysisError {

protected:

	unsigned short index;
	unsigned short numberOfFactors;

public:

/**
 * Default constructor
 */

	indexIsOutOfFactorRangeError();

/**
 * Constructor
 */

	indexIsOutOfFactorRangeError(unsigned short index, unsigned short numberOfFactors);

/**
 * Destructor
 */

	virtual ~indexIsOutOfFactorRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOMANYACTIVESTATIONSERROR_H
#define _TOOMANYACTIVESTATIONSERROR_H

/**
 * CLASS tooManyActiveStationsError
 */

class tooManyActiveStationsError : public analysisError {

protected:

	unsigned short actualNumberOfStations;
	unsigned short maximumNumberOfStations;

public:

/**
 * Default constructor
 */

	tooManyActiveStationsError();

/**
 * Constructor
 */

	tooManyActiveStationsError(unsigned short actualNumberOfStations, unsigned short maximumNumberOfStations);

/**
 * Destructor
 */

	virtual ~tooManyActiveStationsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MAGNETICFIELDNOTFOUNDFORCOMPARISSONTOFACTORSERROR_H
#define _MAGNETICFIELDNOTFOUNDFORCOMPARISSONTOFACTORSERROR_H

/**
 * CLASS magneticFieldNotFoundForComparissonToFactorsError
 */

class magneticFieldNotFoundForComparissonToFactorsError : public analysisError {

public:

/**
 * Default constructor
 */

	magneticFieldNotFoundForComparissonToFactorsError();

/**
 * Destructor
 */

	virtual ~magneticFieldNotFoundForComparissonToFactorsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKCOORDINATESERROR_H
#define _CANNOTACCESSTRACKCOORDINATESERROR_H

/**
 * CLASS cannotAccessTrackCoordinatesError
 */

class cannotAccessTrackCoordinatesError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessTrackCoordinatesError();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackCoordinatesError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _INDEXISOUTOFTRACKRANGEERROR_H
#define _INDEXISOUTOFTRACKRANGEERROR_H

/**
 * CLASS indexIsOutOfTrackRangeError
 */

class indexIsOutOfTrackRangeError : public analysisError {

protected:

	unsigned int index;
	unsigned int numberOfTracks;

public:

/**
 * Default constructor
 */

	indexIsOutOfTrackRangeError();

/**
 * Constructor
 */

	indexIsOutOfTrackRangeError(unsigned int index, unsigned int numberOfTracks);

/**
 * Destructor
 */

	virtual ~indexIsOutOfTrackRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WRONGRELATIVEVALUEFOUNDERROR_H
#define _WRONGRELATIVEVALUEFOUNDERROR_H

/**
 * CLASS wrongRelativeValueFoundError
 */

class wrongRelativeValueFoundError : public analysisError {

public:

/**
 * Default constructor
 */

	wrongRelativeValueFoundError();

/**
 * Destructor
 */

	virtual ~wrongRelativeValueFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WRONGCONSTRAINTVALUEFOUNDERROR_H
#define _WRONGCONSTRAINTVALUEFOUNDERROR_H

/**
 * CLASS wrongConstraintValueFoundError
 */

class wrongConstraintValueFoundError : public analysisError {

public:

/**
 * Default constructor
 */

	wrongConstraintValueFoundError();

/**
 * Destructor
 */

	virtual ~wrongConstraintValueFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _INITHISTOGRAMDIMENSIONSFIRSTERROR_H
#define _INITHISTOGRAMDIMENSIONSFIRSTERROR_H

/**
 * CLASS initHistogramDimensionsFirstError
 */

class initHistogramDimensionsFirstError : public analysisError {

public:

/**
 * Default constructor
 */

	initHistogramDimensionsFirstError();

/**
 * Destructor
 */

	virtual ~initHistogramDimensionsFirstError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _cannotAccessPeakfindingGeometryAnalyserError_H
#define _cannotAccessPeakfindingGeometryAnalyserError_H

/**
 * CLASS cannotAccessPeakfindingGeometryAnalyserError
 */

class cannotAccessPeakfindingGeometryAnalyserError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessPeakfindingGeometryAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessPeakfindingGeometryAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKPROPAGATIONANALYSERERROR_H
#define _CANNOTACCESSTRACKPROPAGATIONANALYSERERROR_H


/**
 * CLASS cannotAccessTrackPropagationAnalyserError
 */

class cannotAccessTrackPropagationAnalyserError : public analysisError {

public:

/**
 * Default constructor
 */

	cannotAccessTrackPropagationAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessTrackPropagationAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
