#ifndef CBMRAWEVENT_H
#define CBMRAWEVENT_H

#include <ostream>
#include <stdint.h>

/*
The class describes the data format of an TRB3 event. A detailed description of the data format
can be found at http://jspc29.x-matter.uni-frankfurt.de/docu/trb3docu.pdf on page 96
*/

using namespace std;

enum CbmRawEventIndex {
	CbmRawEventIndexSize = 0,
	CbmRawEventIndexDecoding = 1,
	CbmRawEventIndexId = 2,
	CbmRawEventIndexSeqNr = 3,
	CbmRawEventIndexDate = 4,
	CbmRawEventIndexTime = 5,
	CbmRawEventIndexRunNr = 6,
	CbmRawEventIndexExpId = 7,
	CbmRawEventIndexData = 8
};

class CbmRawEvent
{
public:
	void* fData;

	CbmRawEvent(void* data)
	: fData(data)
	{

	}

	~CbmRawEvent()
	{
	}

	void Print()
	{
	   printf("-----------------------Raw event-----------------------");
		printf("HdrSize = %ld \n", HdrSize() );
		printf("HdrLen = %ld \n", HdrLen());
		printf("PaddedSize = %ld \n", PaddedSize());

		printf("EVENT - size: 0x%08x  decoding: 0x%08x  id:    0x%08x  seqNr:  0x%08x\n"
					   "date: %s  time:     %s    runNr: 0x%08x  expId:  %s\n",
					   Size(), Decoding(), Id(), SeqNr(),
					   Date2charP(), Time2charP(), RunNr(), ExpId2charP()
					);
	}


	size_t HdrLen(void)
	{
		return CbmRawEventIndexData;
	}

	size_t HdrSize(void)
	{
		return HdrLen() * sizeof(uint32_t);
	}

	void *Data()
	{
		return (void *) ((char *) fData + HdrSize());
	}

	void* End()
	{
		return (void *) ((char *) fData + PaddedSize());
	}

	size_t DataSize()
	{
		return Size() - HdrSize();
	}

	uint32_t Size()
	{
		return HdrValue(CbmRawEventIndexSize);
	}

	uint32_t Decoding()
	{
		return HdrValue(CbmRawEventIndexDecoding);
	}

	uint32_t Id()
	{
		return HdrValue(CbmRawEventIndexId);
	}

	uint32_t SeqNr()
	{
		return HdrValue(CbmRawEventIndexSeqNr);
	}

	uint32_t Date()
	{
		return HdrValue(CbmRawEventIndexDate);
	}

	uint32_t Time()
	{
		return HdrValue(CbmRawEventIndexTime);
	}

	uint32_t RunNr()
	{
		return HdrValue(CbmRawEventIndexRunNr);
	}

	uint32_t ExpId()
	{
		return HdrValue(CbmRawEventIndexExpId);
	}

	uint32_t HdrValue(unsigned idx)
	{
		uint32_t *hdr = Hdr();

		if (IsSwapped()) {
			uint32_t tmp0;
			uint32_t tmp1;

			tmp0 = hdr[idx];
			((char *) &tmp1)[0] = ((char *) &tmp0)[3];
			((char *) &tmp1)[1] = ((char *) &tmp0)[2];
			((char *) &tmp1)[2] = ((char *) &tmp0)[1];
			((char *) &tmp1)[3] = ((char *) &tmp0)[0];
			return tmp1;
		} else {
			return hdr[idx];
		}
	}

	uint32_t *Hdr()
	{
		return (uint32_t *) fData;
	}

	int IsSwapped()
	{
		uint32_t *hdr = Hdr();

		return hdr[CbmRawEventIndexDecoding] > 0xffffff;
	}

	size_t PaddedSize()
	{
		uint32_t size = Size();
		uint32_t alignment = Alignment();
		return (size & (alignment - 1))
			? (size & ~(alignment - 1)) + alignment : size;
	}

	unsigned Alignment()
	{
		return 1 << (Decoding() >> 16 & 0xff);
	}

	char *Date2charP()
	{
		time_t tempo;
		struct tm *gmTime;
		static char buffer[80];

		tempo = time(NULL);
		gmTime = gmtime(&tempo);
		gmTime->tm_year = (Date() >> 16) & 0xff;
		gmTime->tm_mon = (Date() >> 8) & 0xff;
		gmTime->tm_mday = (Date() >> 0) & 0xff;
		strftime(buffer, 80, "%Y-%m-%d", gmTime);

		return buffer;
	}

	char *Time2charP()
	{
		time_t tempo;
		struct tm *gmTime;
		static char buffer[80];

		tempo = time(NULL);
		gmTime = gmtime(&tempo);
		gmTime->tm_hour = (Time() >> 16) & 0xff;
		gmTime->tm_min = (Time() >> 8) & 0xff;
		gmTime->tm_sec = (Time() >> 0) & 0xff;
		strftime(buffer, 80, "%H:%M:%S", gmTime);

		return buffer;
	}

	char *ExpId2charP()
	{
		static char buffer[3];
		uint32_t expId;

		expId = ExpId();

		buffer[0] = (expId >> 8) & 0xff;
		buffer[1] = expId & 0xff;
		buffer[2] = '\0';

		return buffer;
	}

 private:
	CbmRawEvent(const CbmRawEvent&);
	CbmRawEvent operator=(const CbmRawEvent&);
};

#endif // CBMRAWEVENT_H
