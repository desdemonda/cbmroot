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
///     - class for defining the trackfinders memory dimensions for
///		 using static memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:55 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MEMORYDEF_H
#define _MEMORYDEF_H


//#define CALLOC_HISTOGRAM_RUNTIME		/**< Definition enables allocation of trackfinder memory at runtime */
#undef CALLOC_HISTOGRAM_RUNTIME


#ifndef CALLOC_HISTOGRAM_RUNTIME

/*
 * This definitions set the maximal number of data
 * for the histogram, if memory allocation is done
 * before runtime. During runtime smaller values
 * for this parameters are accepted when configuring.
 * Bigger values lead to some errors. This is a programmed
 * software restriction.
 * If the allocation is done at runtime, no software
 * restriction occurs.
 */

/**/
#define maxDimTheta   383		/**< Actual maximal size of the theta dimension of the histogram layer */
#define maxDimRadius  127		/**< Actual maximal size of the radius dimension of the histogram layer */
#define maxDimGamma   191		/**< Actual maximal size of the gamma dimension of the histogram layer */
/**/
/**/
//#define maxDimTheta   191		/**< Actual maximal size of the theta dimension of the histogram layer */
//#define maxDimRadius   63		/**< Actual maximal size of the radius dimension of the histogram layer */
//#define maxDimGamma   191		/**< Actual maximal size of the gamma dimension of the histogram layer */
/**/
/**/
//#define maxDimTheta    95		/**< Actual maximal size of the theta dimension of the histogram layer */
//#define maxDimRadius   31		/**< Actual maximal size of the radius dimension of the histogram layer */
//#define maxDimGamma   255		/**< Actual maximal size of the gamma dimension of the histogram layer */
/**/
//#define maxDimTheta    95		/**< Actual maximal size of the theta dimension of the histogram layer */
//#define maxDimRadius   31		/**< Actual maximal size of the radius dimension of the histogram layer */
//#define maxDimGamma   191		/**< Actual maximal size of the gamma dimension of the histogram layer */
/**/


#endif


#define maxDimSH   8			/**< Actual maximal number of the detector stations in one cell of the histogram layer */


#endif
