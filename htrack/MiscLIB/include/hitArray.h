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
///   file:
///     - global definition of the data type used for the hit memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:55 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HITARRAY_H
#define _HITARRAY_H


#include "../../DataRootObjectLIB/include/inputHitSpecialMem.h"
#include "../../DataRootObjectLIB/include/inputHitStlMem.h"


/**
 * This two objects are equal of functionality but different in the implementation.
 * So here you can define the one which should be used.
 * HINT: Use the one which is finally faster!!!
 */

//#define hitArray inputHitSpecialMem		/**< Definition sets all hitArray objects to inputHitSpecialMem objects. */
#define hitArray inputHitStlMem		/**< Definition sets all hitArray objects to inputHitStlMem objects. */


#endif
