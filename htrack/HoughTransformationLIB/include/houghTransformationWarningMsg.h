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
///     - base class for warnings occuring during hough transformation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:54 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HOUGHTRANSFORMATIONWARNINGMSG_H
#define _HOUGHTRANSFORMATIONWARNINGMSG_H


#include <string>


//#define NOHOUGHTRANSFORMATIONWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOHOUGHTRANSFORMATIONWARNINGMESSAGE


/* **************************************************************
 * CLASS houghTransformationWarningMsg			 				*
 * **************************************************************/

class houghTransformationWarningMsg {

public:

/**
 * Default constructor
 */

	houghTransformationWarningMsg();

/**
 * Destructor
 */

	virtual ~houghTransformationWarningMsg();

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


#ifndef _TOOMUCHHITENTRIESINHISTCELLWARNINGMSG_H
#define _TOOMUCHHITENTRIESINHISTCELLWARNINGMSG_H

/**
 * CLASS tooMuchHitEntriesInHistCellWarningMsg
 */

class tooMuchHitEntriesInHistCellWarningMsg : public houghTransformationWarningMsg {

private:

	int indexDim1;		/**< index of the first dimension of the histogramcell */
	int indexDim2;		/**< index of the second dimension of the histogramcell */
	int indexDim3;		/**< index of the layer of the histogram */
	int indexDim4;		/**< index of the station in the histogramcell */

public:

/**
 * Default constructor
 */

	tooMuchHitEntriesInHistCellWarningMsg();

/**
 * Constructor
 * @param indexDim1 is the index of the first dimension of the histogramcell
 * @param indexDim2 is the index of the second dimension of the histogramcell
 * @param indexDim3 is the index of the layer of the histogram
 * @param indexDim4 is the index of the station in the histogramcell
 */

	tooMuchHitEntriesInHistCellWarningMsg(int index1, int index2, int index3, int index4);

/**
 * Destructor
 */

	virtual ~tooMuchHitEntriesInHistCellWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
