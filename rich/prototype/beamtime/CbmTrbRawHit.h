#ifndef CBMTRBRAWHIT_H
#define CBMTRBRAWHIT_H

#include "TObject.h"
/*
 *  TrbRawHit consists of two fronts - leading and trailing.
 *  Time interval between the leading and the trailing edges indicated the value of the hit.
 */

class CbmTrbRawHit: public TObject
{
public:
	CbmTrbRawHit () :
		fTdcId (0),
		fLeadingChannel (0),
		fLeadingEpoch(0),
		fLeadingCoarseTime(0),
		fLeadingFineTime(0),
		fLeadingFineTimeCalibCorr(0.),
		fTrailingChannel(0),
		fTrailingEpoch(0),
		fTrailingCoarseTime(0),
		fTrailingFineTime(0),
		fTrailingFineTimeCalibCorr(0.)
   {
	}

	CbmTrbRawHit (UShort_t tdc,
		            UShort_t leadingChannel, UInt_t leadingEpoch, UShort_t leadingCoarseTime, UShort_t leadingFineTime,
	               UShort_t trailingChannel, UInt_t trailingEpoch, UShort_t trailingCoarseTime, UShort_t trailingFineTime) :
         fTdcId (tdc),
         fLeadingChannel(leadingChannel),
         fLeadingEpoch(leadingEpoch),
         fLeadingCoarseTime(leadingCoarseTime),
         fLeadingFineTime(leadingFineTime),
         fLeadingFineTimeCalibCorr(0.),
         fTrailingChannel(trailingChannel),
         fTrailingEpoch(trailingEpoch),
         fTrailingCoarseTime(trailingCoarseTime),
         fTrailingFineTime(trailingFineTime),
         fTrailingFineTimeCalibCorr(0.)
	{
	}

   UShort_t GetTdc() const {return this->fTdcId;}
   
   UShort_t GetLChannel() const {return this->fLeadingChannel;}
   UInt_t GetLEpoch() const {return this->fLeadingEpoch;}
   UShort_t GetLCTime() const {return this->fLeadingCoarseTime;}
   UShort_t GetLFTime() const {return this->fLeadingFineTime;}
   Double_t GetLeadingTimeCorr() const {return this->fLeadingFineTimeCalibCorr;}
   
   UShort_t GetTChannel() const {return this->fTrailingChannel;}
   UInt_t GetTEpoch() const {return this->fTrailingEpoch;}
   UShort_t GetTCTime() const {return this->fTrailingCoarseTime;}
   UShort_t GetTFTime () const {return this->fTrailingFineTime;}
   Double_t GetTrailingTimeCorr () const {return this->fTrailingFineTimeCalibCorr;}

   void SetLeadingTimeCorr (Double_t LFTimeCalibrCorr) {this->fLeadingFineTimeCalibCorr = LFTimeCalibrCorr;}
   void SetTrailingTimeCorr (Double_t TFTimeCalibrCorr){this->fTrailingFineTimeCalibCorr = TFTimeCalibrCorr;}

private:
   UShort_t fTdcId;                 // 16 bit (as max)

   UShort_t fLeadingChannel;
   UInt_t fLeadingEpoch;            // 28 bit
   UShort_t fLeadingCoarseTime;     // 11 bit
   UShort_t fLeadingFineTime;       // 10 bit
   Double_t fLeadingFineTimeCalibCorr;
   
   UShort_t fTrailingChannel;
   UInt_t fTrailingEpoch;           // 28 bit
   UShort_t fTrailingCoarseTime;    // 11 bit
   UShort_t fTrailingFineTime;      // 10 bit
   Double_t fTrailingFineTimeCalibCorr;
};

#endif // CBMTRBRAWHIT_H
