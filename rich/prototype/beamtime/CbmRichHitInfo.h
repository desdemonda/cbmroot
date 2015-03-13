#ifndef CBMRICHHITINFO_H
#define CBMRICHHITINFO_H

#include "TObject.h"

class CbmRichHitInfo : public TObject
{
public:
	CbmRichHitInfo():
			fPmtNum(0),
			fPixelNum(0),
			fX(0),
			fY(0),
			fXPixel(0),
			fYPixel(0),
			fTdcId(0),
			fLeadingChannel(0),
			fTrailingChannel(0),
			fPadiwaNum(0),
			fTrbNum(0),
			fPmtTypeId(0)
		{

		}

	CbmRichHitInfo(UInt_t pmtNum, UInt_t pixelNum, Double_t x, Double_t y, UInt_t xPixel, UInt_t yPixel,
			UInt_t tdcId, UInt_t leadingChanel, UInt_t trailingChanel, UInt_t padiwaNum, UInt_t trbNum, UInt_t pmtTypeId):
		fPmtNum(pmtNum),
		fPixelNum(pixelNum),
		fX(x),
		fY(y),
		fXPixel(xPixel),
		fYPixel(yPixel),
		fTdcId(tdcId),
		fLeadingChannel(leadingChanel),
		fTrailingChannel(trailingChanel),
		fPadiwaNum(padiwaNum),
		fTrbNum(trbNum),
		fPmtTypeId(pmtTypeId)
	{

	}

	virtual ~CbmRichHitInfo()
	{

	}

	void Copy(CbmRichHitInfo* hitInfo)
	{
		fPmtNum = hitInfo->GetPmtNum();
		fPixelNum = hitInfo->GetPixelNum();
		fX = hitInfo->GetX();
		fY = hitInfo->GetY();
		fXPixel = hitInfo->GetXPixel();
		fYPixel = hitInfo->GetYPixel();
		fTdcId = hitInfo->GetTdcId();
		fLeadingChannel = hitInfo->GetLeadingChannel();
		fTrailingChannel = hitInfo->GetTrailingChannel();
		fPadiwaNum = hitInfo->GetPadiwaNum();
		fTrbNum	= hitInfo->GetTrbNum();
		fPmtTypeId = hitInfo->GetPmtTypeId();
	}

	UInt_t GetPmtNum() const {return fPmtNum;}
	UInt_t GetPixelNum() const {return fPixelNum;}
	Double_t GetX() const {return fX;}
	Double_t GetY() const {return fY;}
	UInt_t GetXPixel() const {return fXPixel;}
	UInt_t GetYPixel() const {return fYPixel;}
	UInt_t GetTdcId() const {return fTdcId;}
	UInt_t GetLeadingChannel() const {return fLeadingChannel;}
	UInt_t GetTrailingChannel() const {return fTrailingChannel;}
	UInt_t GetPadiwaNum() const {return fPadiwaNum;}
	UInt_t GetTrbNum() const {return fTrbNum;}
	UInt_t GetPmtTypeId() const {return fPmtTypeId;}

private:
	UInt_t fPmtNum; // PMT number
	UInt_t fPixelNum; // Pixel number
	Double_t fX; // X coordinate in cm
	Double_t fY; // Y coordinate in cm
	UInt_t fXPixel; // X coordinate in pixels
	UInt_t fYPixel; // Y coordinate in pixels
	UInt_t fTdcId; // TDC ID
	UInt_t fLeadingChannel; // leading channel number
	UInt_t fTrailingChannel; // trailing channel number
	UInt_t fPadiwaNum; // PADIWA number
	UInt_t fTrbNum; // TRB number
	UInt_t fPmtTypeId; // type Id of PMT PMT-Type

	ClassDef(CbmRichHitInfo, 1)
};

#endif
