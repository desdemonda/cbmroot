//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    CbmStsHit class - representation of one hit in the silicon  stations. //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#include "CbmStsHit.h"
#include "CbmStsPoint.h"

#include "Riostream.h"
#include "iomanip.h"

ClassImp(CbmStsHit)

//________________________________________________________________
CbmStsHit::CbmStsHit()
{
  // Default constructor.

  SetUsage(kFALSE);
  SetHitNumber(-1);
  SetTrackID(-1);

  SetStation(0);

  SetX(0);
  SetY(0);
  SetZ(0);

  SetXerr(0);
  SetYerr(0);
  SetZerr(0);

}

//________________________________________________________________
CbmStsHit::CbmStsHit(CbmStsPoint *point)
{

  SetUsage(kFALSE);
  SetHitNumber(-1);
  SetTrackID(-1);

  SetStation((Long_t) point->GetDetectorID());

  SetX((Double_t) point->GetXOut());
  SetY((Double_t) point->GetYOut());
  SetZ((Double_t) point->GetZOut());

  Float_t error = 0.0;
  SetXerr(error);
  SetYerr(error);
  SetZerr(error);
    
}

//________________________________________________________________
CbmStsHit::CbmStsHit(Double_t *x, Int_t stn)
{
  // Constructor which takes the coodrinates and the station number.

  SetUsage(kFALSE);
  SetHitNumber(-1);

  SetTrackID(-1);

  SetStation(stn);

  SetX(x[0]);
  SetY(x[1]);
  SetZ(x[2]);

  SetXerr(0.);
  SetYerr(0.);
  SetZerr(0.);

}

//________________________________________________________________
CbmStsHit::~CbmStsHit() 
{
  // ---
}


//________________________________________________________________
void CbmStsHit::Print()
{

      cout << flush;
      cout << " " << fTrackID ;
      cout << " " << fStation ;

      cout.setf(ios::right, ios::floatfield);
      cout.precision(3);
      cout << " " << setw(7) << fCoord.X();
      cout << " " << setw(7) << fCoord.Y();
      cout << " " << setw(5) << fCoord.Z();
      cout.setf(ios::scientific);
      cout.precision(5);
      cout << " " << setw(12) << fError.X();
      cout << " " << setw(12) << fError.Y();
      cout << " " << setw(12) << fError.Z();
      cout << endl;
      cout.setf(ios::right, ios::floatfield);

}
