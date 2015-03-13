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
///     - base class for warnings occuring during lut access
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:41:18 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTGENERATORWARNINGMSG_H
#define _LUTGENERATORWARNINGMSG_H


#include <string>


//#define NOLUTGENERATORWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOLUTGENERATORWARNINGMESSAGE


/* **************************************************************
 * CLASS lutGeneratorWarningMsg					 				*
 * **************************************************************/

class lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	lutGeneratorWarningMsg();

/**
 * Destructor
 */

	virtual ~lutGeneratorWarningMsg();

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


#ifndef _MAGNETICFIELDNOTDEFINEDWARNINGMSG_H
#define _MAGNETICFIELDNOTDEFINEDWARNINGMSG_H

/**
 * CLASS magneticFieldNotDefinedWarningMsg
 */

class magneticFieldNotDefinedWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	magneticFieldNotDefinedWarningMsg();

/**
 * Destructor
 */

	virtual ~magneticFieldNotDefinedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MAGNETICFIELDRENEWWARNINGMSG_H
#define _MAGNETICFIELDRENEWWARNINGMSG_H

/**
 * CLASS magneticFieldRenewWarningMsg
 */

class magneticFieldRenewWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	magneticFieldRenewWarningMsg();

/**
 * Destructor
 */

	virtual ~magneticFieldRenewWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIFFERENTPRELUTDEFINITIONASFILEDETECTEDWARNINGMSG_H
#define _DIFFERENTPRELUTDEFINITIONASFILEDETECTEDWARNINGMSG_H

/**
 * CLASS differentPrelutDefinitionAsFileDetectedWarningMsg
 */

class differentPrelutDefinitionAsFileDetectedWarningMsg : public lutGeneratorWarningMsg {

private:

	unsigned short numberOfDifferences;

public:

/**
 * Default constructor
 */

	differentPrelutDefinitionAsFileDetectedWarningMsg();

/**
 * Constructor
 */

	differentPrelutDefinitionAsFileDetectedWarningMsg(unsigned short numberOfDifferences);

/**
 * Destructor
 */

	virtual ~differentPrelutDefinitionAsFileDetectedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIFFERENTDIGITALHITLUTUSAGEASFILEDETECTEDWARNINGMSG_H
#define _DIFFERENTDIGITALHITLUTUSAGEASFILEDETECTEDWARNINGMSG_H

/**
 * CLASS differentDigitalHitLutUsageAsFileDetectedWarningMsg
 */

class differentDigitalHitLutUsageAsFileDetectedWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	differentDigitalHitLutUsageAsFileDetectedWarningMsg();

/**
 * Destructor
 */

	virtual ~differentDigitalHitLutUsageAsFileDetectedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIFFERENTPRELUTUSAGEASFILEDETECTEDWARNINGMSG_H
#define _DIFFERENTPRELUTUSAGEASFILEDETECTEDWARNINGMSG_H

/**
 * CLASS differentPrelutUsageAsFileDetectedWarningMsg
 */

class differentPrelutUsageAsFileDetectedWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	differentPrelutUsageAsFileDetectedWarningMsg();

/**
 * Destructor
 */

	virtual ~differentPrelutUsageAsFileDetectedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIFFERENTLUTDEFINITIONASFILEDETECTEDWARNINGMSG_H
#define _DIFFERENTLUTDEFINITIONASFILEDETECTEDWARNINGMSG_H

/**
 * CLASS differentLutDefinitionAsFileDetectedWarningMsg
 */

class differentLutDefinitionAsFileDetectedWarningMsg : public lutGeneratorWarningMsg {

private:

	unsigned short numberOfDifferences;

public:

/**
 * Default constructor
 */

	differentLutDefinitionAsFileDetectedWarningMsg();

/**
 * Constructor
 */

	differentLutDefinitionAsFileDetectedWarningMsg(unsigned short numberOfDifferences);

/**
 * Destructor
 */

	virtual ~differentLutDefinitionAsFileDetectedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DIFFERENTLUTUSAGEASFILEDETECTEDWARNINGMSG_H
#define _DIFFERENTLUTUSAGEASFILEDETECTEDWARNINGMSG_H

/**
 * CLASS differentLutUsageAsFileDetectedWarningMsg
 */

class differentLutUsageAsFileDetectedWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	differentLutUsageAsFileDetectedWarningMsg();

/**
 * Destructor
 */

	virtual ~differentLutUsageAsFileDetectedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITEEMPTYDIGITALHITLUTWARNINGMSG_H
#define _CANNOTWRITEEMPTYDIGITALHITLUTWARNINGMSG_H

/**
 * CLASS cannotWriteEmptyDigitalHitLutWarningMsg
 */

class cannotWriteEmptyDigitalHitLutWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	cannotWriteEmptyDigitalHitLutWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotWriteEmptyDigitalHitLutWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITEEMPTYPRELUTWARNINGMSG_H
#define _CANNOTWRITEEMPTYPRELUTWARNINGMSG_H

/**
 * CLASS cannotWriteEmptyPrelutWarningMsg
 */

class cannotWriteEmptyPrelutWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	cannotWriteEmptyPrelutWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotWriteEmptyPrelutWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTWRITEEMPTYLUTWARNINGMSG_H
#define _CANNOTWRITEEMPTYLUTWARNINGMSG_H

/**
 * CLASS cannotWriteEmptyLutWarningMsg
 */

class cannotWriteEmptyLutWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	cannotWriteEmptyLutWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotWriteEmptyLutWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRYTOACCESSMAGNETICFIELDOFFILEOBJECTWARNINGMSG_H
#define _TRYTOACCESSMAGNETICFIELDOFFILEOBJECTWARNINGMSG_H

/**
 * CLASS tryToAccessMagneticFieldOfFileObjectWarningMsg
 */

class tryToAccessMagneticFieldOfFileObjectWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	tryToAccessMagneticFieldOfFileObjectWarningMsg();

/**
 * Destructor
 */

	virtual ~tryToAccessMagneticFieldOfFileObjectWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRYTOACCESSMAGNETICFIELDFACTOROFFILEOBJECTWARNINGMSG_H
#define _TRYTOACCESSMAGNETICFIELDFACTOROFFILEOBJECTWARNINGMSG_H

/**
 * CLASS tryToAccessMagneticFieldFactorOfFileObjectWarningMsg
 */

class tryToAccessMagneticFieldFactorOfFileObjectWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	tryToAccessMagneticFieldFactorOfFileObjectWarningMsg();

/**
 * Destructor
 */

	virtual ~tryToAccessMagneticFieldFactorOfFileObjectWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRYTOACCESSLUTCORRECTIONSOFFILEOBJECTWARNINGMSG_H
#define _TRYTOACCESSLUTCORRECTIONSOFFILEOBJECTWARNINGMSG_H

/**
 * CLASS tryToAccessLutCorrectionsOfFileObjectWarningMsg
 */

class tryToAccessLutCorrectionsOfFileObjectWarningMsg : public lutGeneratorWarningMsg {

public:

/**
 * Default constructor
 */

	tryToAccessLutCorrectionsOfFileObjectWarningMsg();

/**
 * Destructor
 */

	virtual ~tryToAccessLutCorrectionsOfFileObjectWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
