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
///     - global definition of the data type used for bit memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:55 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _BITARRAY_H
#define _BITARRAY_H


#include "../../DataObjectLIB/include/bitfield.h"
#include "../../DataObjectLIB/include/bytefield.h"


/**
 * These two objects are equal of functionality but different in the implementation.
 * So here you can define the one which should be used.
 * HINT: Use the one which is finally faster!!!
 *       With the firstFilterGeometrie 6, the secondFilterGeometrie 3 and the
 *       two basicFilters being 1, bytefield is about 2.38% faster. But
 *       bitfield is more tested and so more secure to produce the correct output.
 */

#define bitArray bitfield		/**< Definition sets all bitArray Objects to bitfield Objects. */
//#define bitArray bytefield	/**< Definition sets all bitArray Objects to bytefield Objects. */


#endif
