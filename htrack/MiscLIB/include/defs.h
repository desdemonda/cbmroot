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
///     - global definition of structs, defines and enums for project
///     - use this file with the CBMROOT framework version
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 14:02:13 $
/// $Revision: 1.13 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DEFS_H
#define _DEFS_H


/**
 * This definition makes the library code suitable to run in a standalone context,
 * in the CBMROOT framework or on a Playstation 3.
 */

#define STANDALONE   1
#define CBMROOT      2
#define PS3          3


#define ARCHITECTURE CBMROOT


/**
 * This definition enables that the hits of just one track can enter the histogram
 */

//#define DEBUGJUSTONEGOODTRACK
#undef DEBUGJUSTONEGOODTRACK

/**
 * This definition determines the index of the good track which should be debugged
 */

#define INDEXOFGOODTRACK 2


/**
 * This definition is just used in combination with DEBUGJUSTONEGOODTRACKINDEX
 * to workaround the displaying of all tracks
 */

#define INVALIDTRACKINDEX -1


/**
 * By enabling this you can write the trackIndex of the tracks
 * which have a defined percentage of correct hits to the standard output.
 */

#undef DEBUGTRACKINDEXOFTRACKSWITHPERCENTAGEOFCORRECTHITS


/**
 * This definition enables all warning Messages which are
 * not neccessary anymore. This messages was used in some
 * older versions of the program.
 */

//#define ENABLEALLWARNINGS
#undef ENABLEALLWARNINGS


/**
 * For the standalone version one have to setup the correct version of the root
 * framework.
 */

#if (ARCHITECTURE == STANDALONE)

	#define ROOTFRAMEWORK 51000

#else

	#undef ROOTFRAMEWORK

#endif


/**
 * This definition makes the library code suitable to run in a
 * very old version of the the CBMROOT framework context
 * belonging to the input access (hits, points).
 */

#if (ARCHITECTURE == STANDALONE)

	#undef INPUTCOMPATIBILITY

#elif (ARCHITECTURE == CBMROOT)

	#undef INPUTCOMPATIBILITY
//	#define INPUTCOMPATIBILITY

#else

	#undef INPUTCOMPATIBILITY

#endif

/**
 * This definition makes the library code suitable to run in an
 * old version of the the CBMROOT framework context belonging to
 * the hit access.
 */

#if (ARCHITECTURE == STANDALONE)

	#define HITCOMPATIBILITY

#elif (ARCHITECTURE == CBMROOT)

	#undef HITCOMPATIBILITY
	//#define HITCOMPATIBILITY

#else

	#undef HITCOMPATIBILITY

#endif


/**
 * LUTVERSION defines the version of the lut object which is
 * used. Version 1 has the following rules:
 * - The pos2 value is always not smaller than the pos2
 *   value before.
 * - The pos1 value is always equal or bigger than the pos1
 *   value before.
 * Version 2 has the following additional rules:
 * - The pos2 value is always 1 bigger than the pos2 value
 *   before.
 * - No warning is printed
 * Version 3 has the following additional rules:
 * - The pos1 value is always equal or 1 bigger than the
 *   pos1 value before.
 * - No warning is printed
 * Version 4 has the following additional rules:
 * - A warning would be printed just if the rule is hitted once or more
 * Version 5:
 * - A warning is printed all the time
 */

#if ((ARCHITECTURE == STANDALONE) ||(ARCHITECTURE == CBMROOT))

	#define LUTVERSION 3

#else

	#define LUTVERSION 3

#endif


/* 
 * If defined, then no analysis of trackfinding would be done.
 * To this it is also not possible to do some analysis because
 * there are just the same objects than in hardware. That means
 * that the hits for a peak can also not be delivered.
 */

#if ((ARCHITECTURE == STANDALONE) ||(ARCHITECTURE == CBMROOT))

//	#define NOANALYSIS
	#undef NOANALYSIS

#else

	#define NOANALYSIS

#endif


#endif
