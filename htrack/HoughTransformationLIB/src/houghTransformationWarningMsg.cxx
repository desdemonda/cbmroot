/* *******************************************************************
// (C)opyright 2004
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
//     - base class for warnings occuring during hough transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:35:54 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/houghTransformationWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS houghTransformationWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghTransformationWarningMsg::houghTransformationWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghTransformationWarningMsg::~houghTransformationWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void houghTransformationWarningMsg::printMsg(std::string message) {

#ifndef NOHOUGHTRANSFORMATIONWARNINGMESSAGE 

	std::cerr << std::endl;
	
	std::cerr << HOUGHTRANSFORMATIONLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS tooMuchHitEntriesInHistCellWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooMuchHitEntriesInHistCellWarningMsg::tooMuchHitEntriesInHistCellWarningMsg() 
  : houghTransformationWarningMsg(), 
    indexDim1(0),
    indexDim2(0),
    indexDim3(0),
    indexDim4(0)
{
  /*
	indexDim1 = 0;
	indexDim2 = 0;
	indexDim3 = 0;
	indexDim4 = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooMuchHitEntriesInHistCellWarningMsg::tooMuchHitEntriesInHistCellWarningMsg(int index1, int index2, int index3, int index4) 
  : houghTransformationWarningMsg(), 
    indexDim1(index1),
    indexDim2(index2),
    indexDim3(index3),
    indexDim4(index4)
{
  /*
	indexDim1 = index1;
	indexDim2 = index2;
	indexDim3 = index3;
	indexDim4 = index4;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooMuchHitEntriesInHistCellWarningMsg::~tooMuchHitEntriesInHistCellWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tooMuchHitEntriesInHistCellWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp = "There is not enough space to add the trackID to histogramcell[";
	itos(indexDim1, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += "][";
	itos(indexDim2, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += "] of the layer ";
	itos(indexDim3, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " in station ";
	itos(indexDim4, buffer, 10, intConversionDigits);
	temp += buffer;
	printMsg(temp);

}
