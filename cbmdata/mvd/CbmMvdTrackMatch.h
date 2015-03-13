// -------------------------------------------------------------------------
// -----                   CbmMvdTrackMatch header file                -----
// -----                  Derived from CbmStsTrackMatch by V. Friese   -----
// -----                  16.06.09 M.Deveaux
// -------------------------------------------------------------------------


/** CbmMvdTrackMatch.h
 *@author M.Deveaux <m.deveaux@gsi.de>
 **
 ** Data structure describing the matching of a reconstructed CbmStsTrack
 ** with a Monte Carlo CbmMCTrack in the MVD.
 **/


#ifndef CBMMVDTRACKMATCH_H
#define CBMMVDTRACKMATCH_H 1


#include "TObject.h"



class CbmMvdTrackMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmMvdTrackMatch();


  /** Standard constructor 
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@param nTracks     Number of MCTracks with common points
  **/
  CbmMvdTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, 
		   Int_t nFake, Int_t nTracks);


  /** Destructor **/
  virtual ~CbmMvdTrackMatch();


  /** Accessors **/
  Int_t GetMCTrackId()    { return fMCTrackId;    };
  Int_t GetNofTrueHits()  { return fNofTrueHits;  };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits()  { return fNofFakeHits;  };
  Int_t GetNofMCTracks()  { return fNofMCTracks;  };


 private:

  /** Best matching CbmMCTrack  **/
  Int_t fMCTrackId;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /** Number of MCTrackx with common points **/
  Int_t fNofMCTracks;


  ClassDef(CbmMvdTrackMatch,1);

};


#endif
				 
