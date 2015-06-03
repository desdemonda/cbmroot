#ifndef CBMTRBOUTPUTHIT_H
#define CBMTRBOUTPUTHIT_H

#include "TObject.h"

class CbmTrbOutputHit : public TObject
{
public:

   CbmTrbOutputHit():
      fTdcId(0),
      fLeadingChannel(0),
      fTrailingChannel(0),
      fLeadingFullTime(0.),
      fTrailingFullTime(0.)
   {

   }

   CbmTrbOutputHit (UShort_t tdc, UShort_t lChannel, Double_t lFullTime, UShort_t tChannel, Double_t tFullTime) :
      fTdcId(tdc),
      fLeadingChannel(lChannel),
      fTrailingChannel(tChannel),
      fLeadingFullTime(lFullTime),
      fTrailingFullTime(tFullTime)
   {

   }
   UShort_t GetTdc() const {return fTdcId;}
   UShort_t GetLChannel() const {return fLeadingChannel;}
   Double_t GetLFullTime () const { return fLeadingFullTime;}
   UShort_t GetTChannel() const {return fTrailingChannel;}
   Double_t GetTFullTime () const { return fTrailingFullTime;}
   Double_t GetDeltaT() const {return fTrailingFullTime - fLeadingFullTime;}

   void SetLeadingFullTime (Double_t fullTime) { fLeadingFullTime = fullTime; }

private:

   UShort_t fTdcId;
   UShort_t fLeadingChannel;
   UShort_t fTrailingChannel;
   Double_t fLeadingFullTime; // Full time stamp in ns
   Double_t fTrailingFullTime; // Full time stamp in ns

};



class CbmTrbOutputHitLeadingFullTimeComparatorLess:
   public std::binary_function<
      const CbmTrbOutputHit*,
      const CbmTrbOutputHit*,
      bool>
{
   public:

   virtual ~CbmTrbOutputHitLeadingFullTimeComparatorLess(){;}

   bool operator()(
      const CbmTrbOutputHit* hit1,
      const CbmTrbOutputHit* hit2) const
   {
      return hit1->GetLFullTime() < hit2->GetLFullTime();
   }
};

#endif

