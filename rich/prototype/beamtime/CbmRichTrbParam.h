#ifndef CBMRICHTRBPARAM_H
#define CBMRICHTRBPARAM_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "FairLogger.h"

#include "CbmRichHitInfo.h"

using namespace std;

class CbmRichTrbParam {
private:
 CbmRichTrbParam() : fMap() {
		ReadMap();
	}

public:

	static CbmRichTrbParam* Instance()
	{
		static CbmRichTrbParam fInstance;
		return &fInstance;
	}

	~CbmRichTrbParam() {

	}

	CbmRichHitInfo* GetRichHitInfo(UInt_t tdcId, UInt_t leadingChannel)
	{
		return fMap[tdcId][leadingChannel];
	}

	Bool_t IsPmtTrb(UInt_t trbId) const
	{
		return  (trbId==0x8015 || trbId==0x8025 || trbId==0x8035 || trbId==0x8045 ||
               trbId==0x8055 || trbId==0x8065 || trbId==0x8075 || trbId==0x8085 ||
               trbId==0x8095 || trbId==0x80a5 || trbId==0x80b5 || trbId==0x80c5 ||
               trbId==0x80d5 || trbId==0x80e5 || trbId==0x80f5 || trbId==0x8105 || trbId == 0x8115);

	}

   UInt_t TDCidToInteger (UInt_t tdcId) {
      return ((((tdcId >> 4) & 0x00ff) - 1) * 4 + (tdcId & 0x000f));
   }

   UInt_t IntegerToTDCid (UInt_t index) {
      return (((index/4) + 1) << 4) + (index%4);
   }

   Bool_t IsSyncChannel(UInt_t ch, UInt_t tdcId=0xffff) const
   {
      if (ch==0) return kTRUE;
      return kFALSE;
   }

   Bool_t IsLeadingEdgeChannel(UInt_t ch, UInt_t tdcId=0xffff) const
   {
      if (ch%2==1) return kTRUE;
      // if (tdcId==.....) {
      // }
      return kFALSE;
   }

   UInt_t GetCorrespondingLeadingEdgeChannel(UInt_t ch, UInt_t tdcId=0xffff) const
   {
      if (tdcId == 0x0110) {
         return ch-1;
      } else if (tdcId == 0x0111) {
         if (ch == 2) return 1;
         if (ch == 4) return 1;
         if (ch == 6) return 5;
         if (ch == 8) return 5;
         if (ch == 16) return 13;
         if (ch == 14) return 13;
      } else if (tdcId == 0x0112) {
         return ch-1;
      } else if (tdcId == 0x0113) {
         if (ch == 12) return 9;
         if (ch == 10) return 9;
      } else {
         return ch-1;
      }
   }

	Bool_t IsReferenceTimeTdc(UInt_t tdcId)
	{
		return (tdcId == 0x0110);
	}

private:

	map<UInt_t, map<UInt_t, CbmRichHitInfo*> > fMap;

	void ReadMap()
	{
		string line;
		ifstream myfile ("pixel-coord_channel-register.ascii");
		if (myfile.is_open()) {
			getline (myfile,line);
			getline (myfile,line);

			while ( !myfile.eof() ){
				UInt_t pmtNum, pixelNum;
				Double_t xmm, ymm;
				UInt_t simpleX, simpleY, tdcId, chLeadingEdge, chTrailingEdge, padiwaNum, trbNum;
				string pmtType;
				myfile >> pmtNum >> pixelNum >> xmm >> ymm >> simpleX >> simpleY;
				myfile >> std::hex >> tdcId;
				myfile >> std::dec >> chLeadingEdge >> chTrailingEdge >> padiwaNum >> trbNum >> pmtType;
				LOG(DEBUG4) << pmtNum <<" " <<  pixelNum <<" " << xmm <<" " << ymm <<" " << simpleX <<" " <<
						simpleY <<" " << tdcId<<" " <<chLeadingEdge<<" " <<chTrailingEdge<<" " <<padiwaNum<<" " <<trbNum<<" " <<pmtType << FairLogger::endl;

				UInt_t pmtTypeId = GetPmtTypeIdByString(pmtType);
				fMap[tdcId][chLeadingEdge] = new CbmRichHitInfo(pmtNum, pixelNum, xmm / 10., ymm / 10., simpleX, simpleY, tdcId, chLeadingEdge, chTrailingEdge, padiwaNum, trbNum, pmtTypeId);
			}
			myfile.close();
		} else {
		   LOG(FATAL) << "[CbmRichTrbParam::ReadMap] Failed to open ASCII map file." << FairLogger::endl;
		}
	}

	UInt_t GetPmtTypeIdByString(const string& str)
	{
		if ( str == "H12700") return 1;
		else if (str == "H8500") return 2;
		else return -1;
	}

};

#endif
