/**
 * \file CbmLitStripHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for strip hits.
 **/

#ifndef CBMLITSTRIPHIT_H_
#define CBMLITSTRIPHIT_H_

#include "data/CbmLitHit.h"

#include <string>
#include <sstream>

/**
 * \class CbmLitStripHit
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for strip hits.
 **/
class CbmLitStripHit : public CbmLitHit
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitStripHit():
      fU(0.),
      fDu(0.),
      fPhi(0.),
      fCosPhi(0.),
      fSinPhi(0.),
      fSegment(0) {
      SetHitType(kLITSTRIPHIT);
   }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitStripHit() {};

   /* Getters */
   litfloat GetU() const { return fU; }
   litfloat GetDu() const { return fDu; }
   litfloat GetPhi() const { return fPhi; }
   litfloat GetCosPhi() const { return fCosPhi; }
   litfloat GetSinPhi() const { return fSinPhi; }
   int GetSegment() const { return fSegment; }

   /* Setters */
   void SetU(litfloat u) { fU = u; }
   void SetDu(litfloat du) { fDu = du; }
   void SetPhi(litfloat phi) { fPhi = phi; }
   void SetCosPhi(litfloat cosPhi) { fCosPhi = cosPhi; }
   void SetSinPhi(litfloat sinPhi) { fSinPhi = sinPhi; }
   void SetSegment(int segment) { fSegment = segment; }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "StripHit: pos=(" << GetU() << "," << GetZ()
         << ") err=(" << GetDu() << "," << GetDz() << ") "
         << " phi=" << GetPhi()
         << " cosPhi=" << GetCosPhi() << " sinPhi=" << GetSinPhi()
         << " refId=" << GetRefId()
         << " hitType=" << GetType()
         << " station=" << GetStation() << std::endl;
      return ss.str();
   }

protected:
   litfloat fU; // U measurement of the hit in [cm].
   litfloat fDu; // U measurement error in [cm].
   litfloat fPhi; // Strip rotation angle in [rad].
   litfloat fCosPhi; // Cosine of strip rotation angle.
   litfloat fSinPhi; // Sine of strip rotation angle.
   int fSegment; // Up or down segment of straw tube.
};

#endif /*CBMLITSTRIPHIT_H_*/
