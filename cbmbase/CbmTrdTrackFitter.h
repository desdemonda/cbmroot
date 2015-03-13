// -------------------------------------------------------------------------
// -----                   CbmTrdTrackFitter header file               -----
// -----                  Created 28/11/05  by D. Kresan               -----
// -----                according to the CbmStsTrackFitter             -----
// -------------------------------------------------------------------------


/** CbmTrdTrackFitter
 *@author D.Kresan <D.Kresan@gsi.de>
 **
 ** Abstract base class for concrete TRD track fitting algorithm.
 ** Each derived class must implement the method DoFit. This has
 ** to operate on an object of type CbmTrdTrack and fill the
 ** parameters fPidHypo, fParamFirst, fParamLast, fFlag and fChi2.
 **/

#ifndef CBMTRDTRACKFITTER
#define CBMTRDTRACKFITTER 1

#include "TObject.h"

class CbmTrdTrack;


class CbmTrdTrackFitter : public TObject
{

 public:

  /** Default constructor **/
  CbmTrdTrackFitter() { };


  /** Destructor **/
  virtual ~CbmTrdTrackFitter() { };


  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init() { };


  /** Abstract method DoFit. To be implemented in the concrete class.
   ** Task: Make a fit to the hits attached to the track by the track
   ** finder. Fill the track parameter member variables.
   **
   *@param pTrack      Pointer to CbmTrdTrack
   **/
  virtual Int_t DoFit(CbmTrdTrack* pTrack) = 0;


  ClassDef(CbmTrdTrackFitter,1);

};

#endif
