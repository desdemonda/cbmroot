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
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DATAOBJECTWARNINGMSG_H
#define _DATAOBJECTWARNINGMSG_H


#include <string>


//#define NODATAOBJECTWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NODATAOBJECTWARNINGMESSAGE


/* **************************************************************
 * CLASS dataObjectWarningMsg					 				*
 * **************************************************************/

class dataObjectWarningMsg {

public:

/**
 * Default constructor
 */

	dataObjectWarningMsg();

/**
 * Destructor
 */

	virtual ~dataObjectWarningMsg();

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


#ifndef _CANNOTDOQUINEMCCLUSKYWITHDIFFERENTLENGTHWARNINGMSG_H
#define _CANNOTDOQUINEMCCLUSKYWITHDIFFERENTLENGTHWARNINGMSG_H


/**
 * CLASS cannotDoQuineMcCluskyWithDifferentLengthWarningMsg
 */

class cannotDoQuineMcCluskyWithDifferentLengthWarningMsg : public dataObjectWarningMsg {

private:

	unsigned int length1;
	unsigned int length2;

public:

/**
 * Default constructor
 */

	cannotDoQuineMcCluskyWithDifferentLengthWarningMsg();

/**
 * Constructor
 */

	cannotDoQuineMcCluskyWithDifferentLengthWarningMsg(unsigned int length1, unsigned int length2);

/**
 * Destructor
 */

	virtual ~cannotDoQuineMcCluskyWithDifferentLengthWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTDOQUINEMCCLUSKYWITHWRONGFORMATWARNINGMSG_H
#define _CANNOTDOQUINEMCCLUSKYWITHWRONGFORMATWARNINGMSG_H


/**
 * CLASS cannotDoQuineMcCluskyWithWrongFormatWarningMsg
 */

class cannotDoQuineMcCluskyWithWrongFormatWarningMsg : public dataObjectWarningMsg {

public:

/**
 * Default constructor
 */

	cannotDoQuineMcCluskyWithWrongFormatWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotDoQuineMcCluskyWithWrongFormatWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _UNKNOWNVALUEINCLASSIFICATIONFOUNDWARNINGMSG_H
#define _UNKNOWNVALUEINCLASSIFICATIONFOUNDWARNINGMSG_H


/**
 * CLASS unknownValueInClassificationFoundWarningMsg
 */

class unknownValueInClassificationFoundWarningMsg : public dataObjectWarningMsg {

public:

/**
 * Default constructor
 */

	unknownValueInClassificationFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~unknownValueInClassificationFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MINCLASSISBIGGERTHANMAXCLASSWARNINGMSG_H
#define _MINCLASSISBIGGERTHANMAXCLASSWARNINGMSG_H


/**
 * CLASS minClassIsBiggerThanMaxClassWarningMsg
 */

class minClassIsBiggerThanMaxClassWarningMsg : public dataObjectWarningMsg {

public:

/**
 * Default constructor
 */

	minClassIsBiggerThanMaxClassWarningMsg();

/**
 * Destructor
 */

	virtual ~minClassIsBiggerThanMaxClassWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _RANGELUTHOUGHBORDERWARNINGMSG_H
#define _RANGELUTHOUGHBORDERWARNINGMSG_H


/**
 * CLASS rangeLutHoughBorderWarningMsg
 */

class rangeLutHoughBorderWarningMsg : public dataObjectWarningMsg {

private:

	unsigned short maxFirstDim;
	unsigned short maxSecondDim;
	unsigned short actualFirstDim;
	unsigned short actualSecondDim;

public:

/**
 * Default constructor
 */

	rangeLutHoughBorderWarningMsg(unsigned short maxFirstDim, unsigned short maxSecondDim, unsigned short actualFirstDim, unsigned short actualSecondDim);

/**
 * Constructor
 */

	rangeLutHoughBorderWarningMsg();

/**
 * Destructor
 */

	virtual ~rangeLutHoughBorderWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _OVERFLOWINSUMMATIONOFCLASSIFICATIONWARNINGMSG_H
#define _OVERFLOWINSUMMATIONOFCLASSIFICATIONWARNINGMSG_H


/**
 * CLASS overflowByAddingClassificationsFoundWarningMsg
 */

class overflowByAddingClassificationsFoundWarningMsg : public dataObjectWarningMsg {

public:

/**
 * Default constructor
 */

	overflowByAddingClassificationsFoundWarningMsg();

/**
 * Destructor
 */

	virtual ~overflowByAddingClassificationsFoundWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TOOBIGVALUEFORATOLONGCONVERSIONWARNINGMSG_H
#define _TOOBIGVALUEFORATOLONGCONVERSIONWARNINGMSG_H


/**
 * CLASS tooBigValueForAToLongConversionWarningMsg
 */

class tooBigValueForAToLongConversionWarningMsg : public dataObjectWarningMsg {

private:

	std::string value;

public:

/**
 * Default constructor
 */

	tooBigValueForAToLongConversionWarningMsg();

/**
 * Constructor
 */

	tooBigValueForAToLongConversionWarningMsg(std::string value);

/**
 * Destructor
 */

	virtual ~tooBigValueForAToLongConversionWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
