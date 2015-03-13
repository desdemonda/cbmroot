/**
 * \file CbmLitEnums.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Define enumerations used in littrack.
 **/

#ifndef CBMLITENUMS_H_
#define CBMLITENUMS_H_

/**
 * \enum Define type of hit on detector station.
 */
enum LitHitType {
   kLITSTRIPHIT = 0, /**< Strip hit. */
   kLITPIXELHIT = 1, /**< Pixel hit. */
   kLITMIXHIT = 2 /**< Mixed type of hits, i.e. both strip and pixel hits. **/
};

/**
 * \enum Define status code.
 */
enum LitStatus {
   kLITSUCCESS = 0, /**< Success. */
   kLITERROR = 1 /**< Error. */
};

/**
 * \enum Define reconstructed track quality.
 */
enum LitTrackQa {
   kLITGOOD = 0, /**< Good track. */
   kLITBAD = 1, /**< Bad track. */
   kLITGOODMERGE = 2 /**< Correctly merged track. */
};

/**
 * \enum Define system identificator.
 */
enum LitSystemId {
   kLITMUCH = 0, /**< MUCH detector. */
   kLITTRD = 1, /**< TRD detector. */
   kLITTOF = 2, /**< TOF detector. */
   kLITMVD = 3, /**< MVD detector. */
   kLITSTS = 4 /**< STS detector. */
};

#endif /* CBMLITENUMS_H_ */
