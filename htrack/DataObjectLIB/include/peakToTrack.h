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
///     - class for holding the peakToTrack analysis information
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PEAKTOTRACK_H
#define _PEAKTOTRACK_H


/* **************************************************************
 * CLASS peakToTrack							 				*
 * **************************************************************/

class peakToTrack {

public:

	unsigned int peakToFakeTrack;			/**< Variable to store the number of peaks which corresond to a fake track. */
	unsigned int peakToNoTrack;				/**< Variable to store the number of peaks which correspond to no track. */
	unsigned int peakToOneTrack;			/**< Variable to store the number of peaks which correspond to one track. */
	unsigned int peakToMoreTracks;			/**< Variable to store the number of peaks which correspond to more than track. */
	unsigned int numberOfPeaks;				/**< Variable to store the number of found peaks. */
	unsigned int numberOfIdentifiedPeaks;	/**< Variable to store the number of peaks which corresponds to a track in some way. */
	unsigned int numberOfClonePeaks;		/**< Variable to store the number of peaks which corresponds to a track which is already found by another peak. */
	unsigned int numberOfWrongPeaks;		/**< Variable to store the number of peaks which should be found. */

/**
 * Default constructor
 */

	peakToTrack();

/**
 * Constructor
 */

	peakToTrack(const peakToTrack& value);

/**
 * Destructor
 */

	virtual ~peakToTrack();

/**
 * operator = ()
 */

	const peakToTrack& operator = (const peakToTrack& value);

};

#endif
