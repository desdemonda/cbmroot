// -------------------------------------------------------------------------
// -----                   CbmSttTrackMatch header file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -----          Adapted for STT 13/03/08  by A. Zinchenko            -----
// -------------------------------------------------------------------------

/** CbmSttTrackMatch.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data structure describing the matching of a reconstructed CbmSttTrack
 ** with a Monte Carlo CbmMCTrack.
 **/

#ifndef CBMSTTTRACKMATCH_H
#define CBMSTTTRACKMATCH_H 1

#include "TObject.h"

class CbmSttTrackMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmSttTrackMatch();

  /** Standard constructor 
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@param nTracks     Number of MCTracks with common points
  **/
  CbmSttTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, 
		   Int_t nFake, Int_t nTracks);

  /** Destructor **/
  virtual ~CbmSttTrackMatch();

  /** Accessors **/
  Int_t GetMCTrackID()    { return fMCTrackID;    };
  Int_t GetNofTrueHits()  { return fNofTrueHits;  };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits()  { return fNofFakeHits;  };
  Int_t GetNofMCTracks()  { return fNofMCTracks;  };

 private:

  /** Best matching CbmMCTrack  **/
  Int_t fMCTrackID;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /** Number of MCTrackx with common points **/
  Int_t fNofMCTracks;

  ClassDef(CbmSttTrackMatch,1);

};

#endif
				 
