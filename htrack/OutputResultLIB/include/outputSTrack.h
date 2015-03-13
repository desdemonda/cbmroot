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
///     - writes the evaluated tracks into a root-file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:37:34 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _OUTPUTSTRACK_H
#define _OUTPUTSTRACK_H


#include "outputTrack.h"


/* **************************************************************
 * CLASS outputTrack							 				*
 * **************************************************************/

class outputSTrack : public outputTrack {

public:

/**
 * Default constructor
 */

	outputSTrack();

/**
 * Constructor
 * @param name is the name of the output file to place the computed tracks
 * @param tracks is the object to access the computed tracks
 */

	outputSTrack(trackData** tracks);
	outputSTrack(const char* name, trackData** tracks);

/**
 * Destructor
 */

	virtual ~outputSTrack();

/**
 * Method inits the variables.
 */

	void init();

/**
 * writes the evaluated tracks into the root file
 * @param terminal is a buffer to place the process information
 * @param trackArray is the array to write the tracks to
 */

	void write(std::streambuf* terminal = NULL, TClonesArray* trackArray = NULL);

};

#endif
