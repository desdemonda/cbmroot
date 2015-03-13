// -------------------------------------------------------------------------
// -----                   CbmTrdTrackFinder header file               -----
// -----                  Created 25/11/05  by D. Kresan               -----
// -----                according to the CbmStsTrackFinder             -----
// -------------------------------------------------------------------------


/** CbmTrdTrackFinder
 *@author D.Kresan <D.Kresan@gsi.de>
 **
 ** Abstract base class for concrete TRD track finding algorithm.
 ** Each derived class must implement the method DoFind. This has
 ** to operate on the TClonesArrays of TRD hits and to fill
 ** the CbmTrdTrackArray.
 **/

#ifndef CBMTRDTRACKFINDER
#define CBMTRDTRACKFINDER 1

#include "TObject.h"

class TClonesArray;

class CbmTrdTrackFinder : public TObject
{

 public:

  /** Default constructor **/
  CbmTrdTrackFinder() { }; 


  /** Destructor **/
  virtual ~CbmTrdTrackFinder() { };


  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init() { };


  /** Abstract method DoFind. To be implemented in the concrete class.
   ** Task: Read the hit array and fill the track array,
   ** pointers to which are given as arguments
   **
   *@param hitArray   Array of TRD hits
   *@param trackArray  Array of CbmTrdTrack
   **
   *@value Number of tracks created
   **/
  virtual Int_t DoFind(TClonesArray* hitArray,
		       TClonesArray* trackArray) = 0;


  ClassDef(CbmTrdTrackFinder,1);

};

#endif
