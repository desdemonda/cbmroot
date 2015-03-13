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
//     - implements methods to visualize tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-10-19 14:32:13 $
// $Revision: 1.4 $
//
// *******************************************************************/


#ifndef _VISUALANALYSIS_H
#define _VISUALANALYSIS_H


#include "../../DataObjectLIB/include/trackMomentum.h"
#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "../../MiscLIB/include/defs.h"
#include "TGeoManager.h"
#include "TGeoTrack.h"
#include "TCanvas.h"
#include <string>


typedef struct {

	TGeoTrack*        track;
	int*              hitIndex;
	trackMomentum*    momentum;
	trackCoordinates* coordinates;

} visualTrackInformation;


/* **************************************************************
 * CLASS visualAnalysis					 						*
 * **************************************************************/

class visualAnalysis {

 private:
  visualAnalysis(const visualAnalysis&);
  visualAnalysis& operator=(const visualAnalysis&);

protected:

	TCanvas*                window;				/**< Object to handle the window for the display. */
	visualTrackInformation* tracks;				/**< Object to store the track information. */
	unsigned int            numberOfTracks;		/**< Variable to store the number of tracks in the array. */
	visualTrackInformation* actualTrack;		/**< Pointer to the actual track. */

/**
 * This method initializes the global style for each display.
 */

	void initWindowStyle();

public:

/**
 * Default constructor
 */

	visualAnalysis();

/**
 * Destructor
 */

	virtual ~visualAnalysis();

/**
 * method sets the visibility for the TGeoManager volumes to display
 * @param name is the name identifier of the stations which have to be displayed. All others will be discarded.
 */

	void setupVolumeVisibility(std::string* volumes, unsigned short numberOfVolumes);

/**
 * method resets the object
 */

	void reset();

/**
 * method sets the track for actual track
 * @param index is the index of the track to be set for actual track
 */

	void setActualTrack(unsigned int index);

/**
 * method adds the track and sets it for actual track
 * @param trackIndex is the index of the track which the hit corresponds to in reality
 * @param px is the coordinate in the x-dimension
 * @param py is the coordinate in the y-dimension
 * @param pz is the coordinate in the z-dimension
 * @param dim1 is the histogram coordinate in the first dimension
 * @param dim2 is the histogram coordinate in the second dimension
 * @param dim3 is the histogram coordinate in the third dimension
 * @param pdgCode is the particle identification code
 */

	void addActualTrack(int trackIndex = INVALIDTRACKINDEX, double px = 0, double py = 0, double pz = 0, int dim1 = -1, int dim2 = -1, int dim3 = -1, int pdgCode = 2212);

/**
 * method adds a point to the actual track
 * @param x is the coordinate in the x-dimension
 * @param y is the coordinate in the y-dimension
 * @param z is the coordinate in the z-dimension
 * @param time is the arrival time at the given coordinate (must be in rising order)
 */

	void addPointToActualTrack(double x, double y, double z, double time = -1, int hitIndex = -1);

/**
 * Method draws all tracks including the visible volumes.
 * If no track is set, just the visible volumes would be drawn.
 */

	void drawSetup(std::string name, std::string title, int trackIndex = INVALIDTRACKINDEX);

/**
 * Method converts all tracks into a string representation.
 */

	std::string toString(std::string title, int trackIndex = INVALIDTRACKINDEX);

};

#endif
