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
///     - class for errors occuring while reading the events
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:16 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTERROR_H
#define _INPUTERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS inputError								 				*
 * **************************************************************/

class inputError : public errorHandling {

public:

/**
 * Default constructor
 */

	inputError();

/**
 * Default destructor
 */

	virtual ~inputError();

};

#endif


#ifndef _EVENTNOTFOUNDERROR_H
#define _EVENTNOTFOUNDERROR_H

/**
 * CLASS eventNotFoundError
 */

class eventNotFoundError : public inputError {

private:

	int eventID;

public:

/**
 * Default constructor
 */

	eventNotFoundError();

/**
 * Constructor
 */

	eventNotFoundError(int event);

/**
 * Destructor
 */

	virtual ~eventNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _ZEROTRACKSORHITSERROR_H
#define _ZEROTRACKSORHITSERROR_H

/**
 * CLASS tooManyTracksOrHitsError
 */

class zeroTracksOrHitsError : public inputError {

private:

	int numberOfTracks;
	int numberOfHits;

public:

/**
 * Default constructor
 */

	zeroTracksOrHitsError();

/**
 * Constructor
 */

	zeroTracksOrHitsError(int tracks, int hits);

/**
 * Destructor
 */

	virtual ~zeroTracksOrHitsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _DETECTORNODENOTFOUNDERROR_H
#define _DETECTORNODENOTFOUNDERROR_H

/**
 * CLASS detectorNodeNotFoundError
 */

class detectorNodeNotFoundError : public inputError {

private:

	std::string nodeName;

public:

/**
 * Default constructor
 */

	detectorNodeNotFoundError();

/**
 * Constructor
 */

	detectorNodeNotFoundError(std::string actualNodeName);

/**
 * Destructor
 */

	virtual ~detectorNodeNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOTRACKINFILEERROR_H
#define _NOTRACKINFILEERROR_H

/**
 * CLASS noTrackInFileError
 */

class noTrackInFileError : public inputError {

protected:

	std::string branchName;

public:

/**
 * Default constructor
 */

	noTrackInFileError();

/**
 * Constructor
 */

	noTrackInFileError(std::string actualBranchName);

/**
 * Destructor
 */

	virtual ~noTrackInFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOPOINTINFILEERROR_H
#define _NOPOINTINFILEERROR_H

/**
 * CLASS noPointInFileError
 */

class noPointInFileError : public inputError {

protected:

	std::string branchNamePoint;

public:

/**
 * Default constructor
 */

	noPointInFileError();

/**
 * Constructor
 */

	noPointInFileError(std::string actualBranchNamePoint);

/**
 * Destructor
 */

	virtual ~noPointInFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _DETECTORMATRIXNOTFOUNDERROR_H
#define _DETECTORMATRIXNOTFOUNDERROR_H

/**
 * CLASS detectorMatrixNotFoundError
 */

class detectorMatrixNotFoundError : public inputError {

private:

	std::string nodeName;

public:

/**
 * Default constructor
 */

	detectorMatrixNotFoundError();

/**
 * Constructor
 */

	detectorMatrixNotFoundError(std::string actualNodeName);

/**
 * Destructor
 */

	virtual ~detectorMatrixNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _DETECTORTRANSLATIONNOTFOUNDERROR_H
#define _DETECTORTRANSLATIONNOTFOUNDERROR_H

/**
 * CLASS detectorTranslationNotFoundError
 */

class detectorTranslationNotFoundError : public inputError {

private:

	std::string nodeName;

public:

/**
 * Default constructor
 */

	detectorTranslationNotFoundError();

/**
 * Constructor
 */

	detectorTranslationNotFoundError(std::string actualNodeName);

/**
 * Destructor
 */

	virtual ~detectorTranslationNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
