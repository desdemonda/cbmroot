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
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _OUTPUTTRACK_H
#define _OUTPUTTRACK_H


#include "../../DataRootObjectLIB/include/trackData.h"
#include "../../DataRootObjectLIB/include/trackfinderOutputData.h"
#include "../../DataObjectLIB/include/analyticFormula.h"


#define STSFOLDER      "Sts"			/**< Defines the name of the folder which is used to find the STS data */
#define STSTRACKBRANCH "StsTrack"		/**< Defines the name of the branch in the ROOT tree to write the computed tracks of STS */


/**
 * This defines the adding-process of the hits from the
 * histogram to the corresponding found track. Because of
 * having more than one hit per station, this can be a problem
 * for the global analysis and for the following trackfitter.
 * So the defined number decides the about the adding-process:
 * 1) All hits are added.
 * 2) Just one hit per layer is added, which occurs most
 *    frequently.
 * 3) The hit per layer is added, which occurs most frequently.
 *    If there are more hits with the same frequence, all of
 *    these are added.
 */

#define HITSTOTRACKVERSION 2


/* **************************************************************
 * CLASS outputTrack							 				*
 * **************************************************************/

class outputTrack {

 private:

  outputTrack(const outputTrack&);
  outputTrack& operator=(const outputTrack&);

protected:

	trackData**           tracks;				/**< Object to access the computed tracks. */
	analyticFormula       formula;				/**< Object to compute the momentum. */
	trackfinderOutputData data;					/**< Object to store the tracks in the form of the output file. */

/**
 * Method inits the variables.
 * @param name is the name of the output file to place the computed tracks
 * @param tracks is the object to access the computed tracks
 */

	void initialize();
	void initialize(trackData** tracks);
	void initialize(const char* name, trackData** tracks);

/**
 * writes the evaluated tracks into the root file
 * @param terminal is a buffer to place the process information
 */

	void writeTracks(std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	outputTrack();

/**
 * Constructor
 * @param name is the name of the output file to place the computed tracks
 * @param tracks is the object to access the computed tracks
 */

	outputTrack(trackData** tracks);
	outputTrack(const char* name, trackData** tracks);

/**
 * Destructor
 */

	virtual ~outputTrack();

/**
 * Method inits the variables.
 * @param name is the name of the output file to place the computed tracks
 * @param tracks is the object to access the computed tracks
 */

	virtual void init() = 0;
	void init(trackData** tracks);
	void init(const char* name, trackData** tracks);

/**
 * writes the evaluated tracks into the root file
 * @param terminal is a buffer to place the process information
 * @param trackArray is the array to write the tracks to
 */

	virtual void write(std::streambuf* terminal = NULL, TClonesArray* trackArray = NULL) = 0;

};

#endif
