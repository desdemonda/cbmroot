/** CbmMuchPoint.cxx
 *
 * @author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 * @version 1.0
 * @since   21.03.07
 *
 *  Class for Monte Carlo points in MUon CHambers detector
 *
 */

#include "CbmMuchPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMuchPoint::CbmMuchPoint() 
  : FairMCPoint(), 
    fX_out(0.),
    fY_out(0.),
    fZ_out(0.),
    fPx_out(0.),
    fPy_out(0.),
    fPz_out(0.)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchPoint::CbmMuchPoint(Int_t trackID, Int_t detID, TVector3 posIn,
			   TVector3 posOut, TVector3 momIn, TVector3 momOut,
			   Double_t tof, Double_t length, Double_t eLoss,
			   Int_t eventId)
  : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss, eventId),
  fX_out(posOut.X()),
  fY_out(posOut.Y()),
  fZ_out(posOut.Z()),
  fPx_out(momOut.Px()),
  fPy_out(momOut.Py()),
  fPz_out(momOut.Pz())
{
}
// -------------------------------------------------------------------------



// -----   Copy constructor with event and epoch time   --------------------
CbmMuchPoint::CbmMuchPoint(const CbmMuchPoint& point, Int_t eventId,
			   Double_t eventTime, Double_t epochTime) 
  : FairMCPoint(point),
    fX_out(point.fX_out),
    fY_out(point.fY_out),
    fZ_out(point.fZ_out),
    fPx_out(point.fPx_out),
    fPy_out(point.fPy_out),
    fPz_out(point.fPz_out)
{
  //  *this = point;
  if ( eventId > 0 ) fEventId = eventId; 
  fTime = point.GetTime() + eventTime - epochTime;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchPoint::~CbmMuchPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void CbmMuchPoint::Print(const Option_t* opt) const {
  cout << "-I- CbmMuchPoint: MUCH Point for track " << fTrackID
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



// -----   Point x coordinate from linear extrapolation   ------------------
Double_t CbmMuchPoint::GetX(Double_t z) const {
  Double_t dz = fZ_out - fZ;
  if ( TMath::Abs(dz) < 1.e-3 ) return (fX_out+fX)/2.;
  return ( fX + (z-fZ) / dz * (fX_out-fX) );
}
// -------------------------------------------------------------------------



// -----   Point y coordinate from linear extrapolation   ------------------
Double_t CbmMuchPoint::GetY(Double_t z) const {
  Double_t dz = fZ_out - fZ;
  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
  return ( fY + (z-fZ) / dz * (fY_out-fY) );
}
// -------------------------------------------------------------------------



// -----   Public method IsUsable   ----------------------------------------
Bool_t CbmMuchPoint::IsUsable() const {
  Double_t dz = fZ_out - fZ;
  if ( TMath::Abs(dz) < 1.e-4 ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchPoint)











