#ifndef CBMRICHTRBTIMEMESSAGE_H
#define CBMRICHTRBTIMEMESSAGE_H

#include <ostream>
#include <stdint.h>

using namespace std;

class CbmRichTrbTimeMessage
{
public:
	CbmRichTrbTimeMessage():
		fChannelNum(0),
		fEpochCounter(0),
		fCoarseTime(0),
		fFineTime(0)
	{

	}

	CbmRichTrbTimeMessage(UInt_t chNum, UInt_t epoch, UInt_t coarse, UInt_t fine):
		fChannelNum(chNum),
		fEpochCounter(epoch),
		fCoarseTime(coarse),
		fFineTime(fine)
		{
			;
		}

	~CbmRichTrbTimeMessage()
	{

	}

	void SetParams(UInt_t chNum, UInt_t epoch, UInt_t coarse, UInt_t fine)
	{
		fChannelNum = chNum;
		fEpochCounter = epoch;
		fCoarseTime = coarse;
		fFineTime = fine;
	}

	UInt_t fChannelNum;
	UInt_t fEpochCounter;
	UInt_t fCoarseTime;
	UInt_t fFineTime;
};

#endif
