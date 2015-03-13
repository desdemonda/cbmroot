/**
 * \file CbmLitHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for hits.
 **/

#ifndef CBMLITHIT_H_
#define CBMLITHIT_H_

#include "base/CbmLitEnums.h"
#include "base/CbmLitFloat.h"

#include "TObject.h"

#include <functional>
#include <string>
using std::string;

/**
 * \class CbmLitHit
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for hits.
 **/
class CbmLitHit
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitHit():
      fRefId(0),
      fHitType(kLITPIXELHIT),
      fZ(0.),
      fDz(0.),
      fDetectorId(0) { }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitHit() {}

   /* Getters */
   Int_t GetRefId() const { return fRefId; }
   LitHitType GetType() const { return fHitType; }
   litfloat GetZ() const { return fZ; }
   litfloat GetDz() const { return fDz; }
   LitSystemId GetSystem() const {
	   return LitSystemId((fDetectorId & (WL_SYSTEM << SB_SYSTEM)) >> SB_SYSTEM);
   }
   Int_t GetStation() const {
	   return (fDetectorId & (WL_STATION << SB_STATION)) >> SB_STATION;
   }

   /* Setters */
   void SetRefId(Int_t refId) { fRefId = refId; }
   void SetHitType(LitHitType hitType) { fHitType = hitType; }
   void SetZ(litfloat z) { fZ = z; }
   void SetDz(litfloat dz) { fDz = dz; }
   void SetDetectorId(LitSystemId sysId, Int_t station) {
	   fDetectorId = (sysId << SB_SYSTEM) | (station << SB_STATION);
   }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual string ToString() const {return "";}

private:
   Int_t fRefId; // reference to MC
   LitHitType fHitType; // type of the hit (strip, pixel, etc). Used to safely cast to the proper type.
   litfloat fZ; // Z position of the hit [cm]
   litfloat fDz; // Z position error of the hit [cm]

   // The detector ID consists of:
   // system ID            (0-15),    bits 0-3
   // station number       (0-31),    bits 4-9
   Int_t fDetectorId; // Unique detector ID

   // Length of the index of the corresponding volume
   static const Int_t WL_SYSTEM = 15;
   static const Int_t WL_STATION = 31;
   // Start bit for each volume
   static const Int_t SB_SYSTEM = 0;
   static const Int_t SB_STATION = 4;
};

/**
 * \class Comparator for STL sorting algorithms.
 */
class CompareHitPtrZLess:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetZ() < hit2->GetZ();
   }
};

/**
 * \class Comparator for STL sorting algorithms.
 */
class CompareHitPtrZMore:
   public std::binary_function<
   const CbmLitHit*,
   const CbmLitHit*,
   bool>
{
public:
   bool operator()(const CbmLitHit* hit1, const CbmLitHit* hit2) const {
      return hit1->GetZ() > hit2->GetZ();
   }
};

#endif /*CBMLITHIT_H_*/
