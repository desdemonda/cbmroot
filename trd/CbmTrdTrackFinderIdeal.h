// -------------------------------------------------------------------------
// -----                 CbmTrdTrackFinderIdeal header file            -----
// -----                  Created 28/11/05  by D. Kresan               -----
// -----               according to the CbmStsTrackFinderIdeal         -----
// -------------------------------------------------------------------------


/** CbmStsTrackFinderIdeal
 *@author D.Kresan <D.Kresan@gsi.de>
 **
 ** Ideal track finder in the TRD for simulated data.
 ** For each MCTrack having requested number of TrdPoints,
 ** a TrdTrack is created and the corresponding TrdHits
 ** are attached using the correspondence between TrdHit and TrdPoint.
 **/


#ifndef CBMTRDTRACKFINDERIDEAL_H
#define CBMTRDTRACKFINDERIDEAL_H


#include "CbmTrdTrackFinder.h"

class TClonesArray;


class CbmTrdTrackFinderIdeal : public CbmTrdTrackFinder
{

 public:

  /** Default constructor **/
  CbmTrdTrackFinderIdeal();


  /** Destructor **/
  virtual ~CbmTrdTrackFinderIdeal();


  /** Initialisation **/
  virtual void Init();


  /** Track finding algorithm
   ** This just reads MC truth (MCTracks and MCPoints), creates
   ** one TrdTrack for each MCTrack and attaches the hits according
   ** to the MCTrack of the corresponding MCPoint
   **
   *@param hitArray    Array of TRD hits
   *@param trackArray  Array of CbmTrdTrack
   **
   *@value Number of tracks created
   **/
  virtual Int_t DoFind(TClonesArray* hitArray,
		      TClonesArray* trackArray);

private:

  CbmTrdTrackFinderIdeal& operator=(const CbmTrdTrackFinderIdeal&);
  CbmTrdTrackFinderIdeal(const CbmTrdTrackFinderIdeal&);

  TClonesArray* fMcTracks;
  TClonesArray* fTrdPoints;
  TClonesArray* fTrdHitMatches;// this is needed for clustering

  std::string fTrdHitProducerType; //smearing or digi

  Int_t fEventNum;

  ClassDef(CbmTrdTrackFinderIdeal,1);

};

#endif
