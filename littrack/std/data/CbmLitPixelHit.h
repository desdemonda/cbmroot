/**
 * \file CbmLitPixelHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for pixel hits.
 **/

#ifndef CBMLITPIXELHIT_H_
#define CBMLITPIXELHIT_H_

#include "data/CbmLitHit.h"

#include <string>
#include <sstream>

/**
 * \class CbmLitPixelHit
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for pixel hits.
 **/
class CbmLitPixelHit : public CbmLitHit
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPixelHit():
      fX(0.),
      fY(0.),
      fDx(0.),
      fDy(0.),
      fDxy(0.) {
      SetHitType(kLITPIXELHIT);
   }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPixelHit() {};

   /* Getters */
   litfloat GetX() const { return fX; }
   litfloat GetY() const { return fY; }
   litfloat GetDx() const { return fDx; }
   litfloat GetDy() const { return fDy; }
   litfloat GetDxy() const { return fDxy; }

   /* Setters */
   void SetX(litfloat x) { fX = x; }
   void SetY(litfloat y) { fY = y; }
   void SetDx(litfloat dx) { fDx = dx; }
   void SetDy(litfloat dy) { fDy = dy; }
   void SetDxy(litfloat dxy) { fDxy = dxy; }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "PixelHit: pos=(" << GetX() << "," << GetY() << "," << GetZ()
         << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz() << ") "
         << " dxy=" << GetDxy()
         << " refId=" << GetRefId()
         << " hitType=" << GetType()
         << " detId=" << GetSystem()
         << " station=" << GetStation() << std::endl;
      return ss.str();
   }

private:
   litfloat fX, fY; // X and Y hit positions in [cm].
   litfloat fDx, fDy; // X and Y hit position errors in [cm].
   litfloat fDxy; // XY covariance.
};

#endif /*CBMLITPIXELHIT_H_*/
