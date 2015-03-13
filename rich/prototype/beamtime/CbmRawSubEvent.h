#ifndef CBMRAWSUBEVENT_H
#define CBMRAWSUBEVENT_H

#include "TObject.h" // only for swab - fix it!
#include "CbmRawEvent.h"

/*
The class describes the data format of an TRB3 subevent. A detailed description of the data format
can be found at http://jspc29.x-matter.uni-frankfurt.de/docu/trb3docu.pdf on page 96
*/


enum CbmRawSubEventIndex {
	CbmRawSubEventIndexSize,
	CbmRawSubEventIndexDecoding,
	CbmRawSubEventIndexId,
	CbmRawSubEventIndexTrigNr,
	CbmRawSubEventIndexData
};

class CbmRawSubEvent
{

public:
	void* fData;

	CbmRawSubEvent(void* data)
	: fData(data)
	{

	}

	~CbmRawSubEvent()
	{
	}

	void Print()
	{
		printf("\nSUBEVENT - size: 0x%08x  decoding: 0x%08x  id:    0x%08x  trigNr: 0x%08x trigTypeTRB3: 0x%01x\n",
			Size(), SubDecoding(), SubId(), SubTrigNr(), SubTrigTypeTrb3());

		for (int i = 0; i < SubDataSize() / sizeof(uint32_t); i++) {
			if ( i % 4 == 0) {	/* newline and the offset in the subEvt */
				if(i!=0) printf("\n");
				printf("%08lx:", i * sizeof(uint32_t));
			}
			printf("  0x%08x", SubDataValue(i));
		}
		printf("\n");
	}

   void PrintHeaderOnly()
   {
		printf("SUBEVENT - size: 0x%08x  decoding: 0x%08x  id:    0x%08x  trigNr: 0x%08x trigTypeTRB3: 0x%01x\n",
			Size(), SubDecoding(), SubId(), SubTrigNr(), SubTrigTypeTrb3());
   }

	uint32_t SubDecoding()
	{
		return (Decoding() & 0xFFFFFF0F);
	}

	uint32_t SubId()
	{
		return HdrValue(CbmRawSubEventIndexId);
	}

	uint32_t SubTrigNr()
	{
		return HdrValue(CbmRawSubEventIndexTrigNr);
	}

	uint32_t SubTrigTypeTrb3()
	{
	/* get trb3 trigger type info in bits 4...7*/
		return ((Decoding() & 0xF0) >> 4) ;
	}

	size_t SubDataSize()
	{
		return Size() - SubHdrSize();
	}

	size_t SubHdrLen()
	{
		return CbmRawSubEventIndexData;
	}

	size_t SubHdrSize(void)
	{
		return SubHdrLen() * sizeof(uint32_t);
	}

	void *SubData()
	{
		return (void *) ((char *) fData + SubHdrSize());
	}

	uint32_t SubDataValue(unsigned idx)
	{
		uint32_t val;

		switch (Alignment()) {
		case 4:
			if (IsSwapped()) {
				uint32_t tmp0;
				uint32_t tmp1;

				tmp0 = ((uint32_t *) SubData())[idx];
				((char *) &tmp1)[0] = ((char *) &tmp0)[3];
				((char *) &tmp1)[1] = ((char *) &tmp0)[2];
				((char *) &tmp1)[2] = ((char *) &tmp0)[1];
				((char *) &tmp1)[3] = ((char *) &tmp0)[0];
				val = tmp1;
			} else {
				val = ((uint32_t *) SubData())[idx];
			}
			break;
		case 2:
			if (IsSwapped()) {
				uint16_t v;
				swab(((uint16_t *) SubData()) + idx, &v, 2);
				val = v;
			} else {
				val = ((uint16_t *) SubData())[idx];
			}
			break;
		default:
			val = ((uint8_t *) SubData())[idx];
			break;
		}
		return val;
	}


	//same
	uint32_t Decoding()
	{
		return HdrValue(CbmRawEventIndexDecoding);
	}

	//same
	uint32_t Size()
	{
		return HdrValue(CbmRawEventIndexSize);
	}
	//same
	uint32_t *Hdr()
	{
		return (uint32_t *) fData;
	}
	//same
	int IsSwapped()
	{
		uint32_t *hdr = Hdr();

		return hdr[CbmRawEventIndexDecoding] > 0xffffff;
	}
	//same
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
	//same
	unsigned Alignment()
	{
		return 1 << (Decoding() >> 16 & 0xff);
	}

 private:
	CbmRawSubEvent(const CbmRawSubEvent&);
	CbmRawSubEvent operator=(const CbmRawSubEvent&);

};

#endif // CBMRAWSUBEVENT_H
