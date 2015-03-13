/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// CbmStsCMPoint class - representation of one hit for the conformal               //
//                            mapping track algorithm.                             //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#include "CbmStsHit.h"
#include "CbmStsCMPoint.h"
#include "TMath.h"
#include <Riostream.h>

ClassImp(CbmStsCMPoint)

//________________________________________________________________
CbmStsCMPoint::CbmStsCMPoint() : CbmStsHit() {

// Sets additional values to zero.

  SetPhi(0.);
  SetEta(0.);

  SetXprime(0.);
  SetZprime(0.);
  SetXprimeerr(0.);
  SetZprimeerr(0.);
  SetIntPoint(0., 0., 0., 0., 0., 0.);
  SetShiftedCoord();
  SetDist(0., 0.);
}

//________________________________________________________________
CbmStsCMPoint::CbmStsCMPoint(CbmStsHit *point) : CbmStsHit( *point) {
//  constructor.

}

//________________________________________________________________
CbmStsCMPoint::~CbmStsCMPoint() {
// Destructor.
}

//________________________________________________________________
void CbmStsCMPoint::SetIntPoint(const Double_t in_x, const Double_t in_y, const Double_t in_z,
				const Double_t in_x_err, const Double_t in_y_err, const Double_t in_z_err) {
// Defines a new interaction point. This point is needed to calculate
// the conformal coordinates.

  SetXt(in_x);
  SetYt(in_y);
  SetZt(in_z);
  SetXterr(in_x_err);
  SetYterr(in_y_err);
  SetZterr(in_z_err);

}

//________________________________________________________________
void CbmStsCMPoint::SetAllCoord(const CbmStsCMPoint *preceding_hit) {
  // Sets the interaction point, the shifted coordinates,
  // and the conformal mapping coordinates.
  // These values are calculated from the interaction point of the given hit
  // which should be a
  // already found hit on the same track.

  if (this == preceding_hit) {
    SetIntPoint(preceding_hit->GetX(),    preceding_hit->GetY(),    preceding_hit->GetZ(),
      preceding_hit->GetXerr(), preceding_hit->GetYerr(), preceding_hit->GetZerr());
  }

  else {
    SetIntPoint(preceding_hit->GetXt(),    preceding_hit->GetYt(),    preceding_hit->GetZt(),
      preceding_hit->GetXterr(), preceding_hit->GetYterr(), preceding_hit->GetZterr());
  }

  SetShiftedCoord();
  SetConfCoord();

}


//________________________________________________________________
void CbmStsCMPoint::SetShiftedCoord() {
// Sets the coordinates with resepct to the given vertex point

  SetXv(GetX() - fXt);
  SetYv(GetY() - fYt);
  SetZv(GetZ() - fZt);
  SetXverr(TMath::Sqrt(GetXerr()*GetXerr() + fXterr*fXterr));
  SetYverr(TMath::Sqrt(GetYerr()*GetYerr() + fYterr*fYterr));
  SetZverr(TMath::Sqrt(GetZerr()*GetZerr() + fZterr*fZterr));

}

//________________________________________________________________
void CbmStsCMPoint::SetConfCoord() {
  // Calculates the conformal coordinates of one hit.
  // If the option "vertex_constraint" applies the interaction point is
  // assumed to be at (0, 0, 0). Otherwise the function will use the
  // interaction point specified by fXt and fYt.

  Double_t r2;

  if ((r2 = fXv*fXv + fZv*fZv) != 0.) {
    fXprime =  fXv / r2;
    fZprime = -fZv / r2;

    fXprimeerr = TMath::Sqrt(TMath::Power((-fXv * fXv +   fZv*fZv) * fXverr, 2) +
			     TMath::Power( 2*fXv*fZv*fZverr, 2)) /
      TMath::Power(fXv*fXv + fZv*fZv, 2);

    fZprimeerr = TMath::Sqrt(TMath::Power((-fXv * fXv - 3*fZv*fZv) * fZverr, 2) +
			     TMath::Power(-2*fXv*fZv*fXverr, 2)) /
      TMath::Power(fXv*fXv + fZv*fZv, 2);
  }

  else {

    fXprime    = 0.;
    fZprime    = 0.;
    fXprimeerr = 0.;
    fZprimeerr = 0.;
  }

}


//________________________________________________________________
//
void CbmStsCMPoint::SetAngles() {
// Calculates the angle phi and the pseudorapidity eta for each hit.
// So to say this is just a transformation of the coordinate system.

  Double_t r3dim = TMath::Sqrt(fXv*fXv + fYv*fYv + fZv*fZv);
  Double_t r2dim = TMath::Sqrt(fXv*fXv + fYv*fYv);
  //  Double_t r2deep = TMath::Sqrt(fYv*fYv + fZv*fZv);

  if (r2dim == 0.) {

// If r2dim == 0 the pseudorapidity eta cannot be calculated (division
// by zero)!  This can only happen if the point is lying on the z-axis
// and this should never be possible.
    cout << "The pseudorapidity cannot be calculated! " << endl;
    cout << "(2-dim radius is zero and set to 1.e-10.)" << endl;

    r2dim = 1.e-10;
  }

  if (fXv == 0.) {
    fPhi = (fYv > 0.) ? TMath::Pi() / 2. : - TMath::Pi() / 2.;
  }

  else {
    fPhi = (fXv > 0.) ? TMath::ASin(fYv/r2dim) : TMath::Pi() - TMath::ASin(fYv/r2dim);
  }

  if (fPhi < 0.) {
    fPhi += 2. * TMath::Pi();
  }

  fEta = 0.5 * TMath::Log((r3dim + fZv)/(r3dim - fZv));

// in the future must be changed with Alpha, Beta

  fAlpha = TMath::ATan2(fYv, fZv);

  fBeta = TMath::ATan2(fXv, fZv);

}

//________________________________________________________________
void CbmStsCMPoint::Setup(CbmStsVertex *vertex)
{
// Does the usual setup of a CbmStsCMPoint. It does it in the right order, especially.

  SetIntPoint(vertex->GetX(),    vertex->GetY(),    vertex->GetZ(),
	      vertex->GetXerr(), vertex->GetYerr(), vertex->GetZerr());
  SetShiftedCoord();
  SetConfCoord();

  // The angles are set properly if they are set after the interaction
  // point and the shifted coordinates

  SetAngles();
  SetDist(0., 0.);

}

 
