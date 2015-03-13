
//  ----------------------------------------------------------------------
//  -----                         CbmKFTrdHit                        -----
//  -----               Created 12/12/2005  by D.Kresan              -----
//  ----------------------------------------------------------------------


/** CbmKFTrdHit
 *@author D.Kresan <D.Kresan@gsi.de>
 ** Interface class, representing CbmTrdHit in Kalman Filter.
 **/


#ifndef CBMKFTRDHIT
#define CBMKFTRDHIT 1

#include "CbmKFHit.h"
#include "CbmKFUMeasurement.h"

class CbmKFWall;
class CbmTrdHit;

class CbmKFTrdHit : public CbmKFHit {

public:

    /** Default constructor **/
    CbmKFTrdHit(): FitPoint(), wall(0) {};

    /** Destructor **/
    ~CbmKFTrdHit() {};

    /** Measurement point **/
//    CbmKFPixelMeasurement FitPoint;
    CbmKFUMeasurement FitPoint;

    /** Material **/
    CbmKFWall* wall;

    /** Copy data from CbmTrdHit **/
    void Create(CbmTrdHit* hit);

    /** Filter **/
    Int_t Filter(CbmKFTrackInterface& track, Bool_t downstream, Double_t& QP0);

  const CbmKFTrdHit& operator=(const CbmKFTrdHit& a) {
    wall = a.wall;
    FitPoint = a.FitPoint;
    return *this;
  };

  CbmKFTrdHit(const CbmKFTrdHit& a):
    FitPoint(a.FitPoint),
    wall(a.wall)    
  {};
  
  ClassDef(CbmKFTrdHit, 1);

};


#endif


