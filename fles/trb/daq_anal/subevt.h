#ifndef SUBEVT_H
#define SUBEVT_H

#include <stdint.h>

#include "hadtu.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SubEvtId {
	SubEvtId_invalid = 0,
	SubEvtId_test1 = 1,
	SubEvtId_slow = 2,
	SubEvtId_test2 = 3,
	SubEvtId_mdc = 6,
	SubEvtId_shower = 7,
	SubEvtId_rich = 8,
	SubEvtId_tof = 9,
	SubEvtId_start = 10,
	SubEvtId_veto = 11,
	SubEvtId_richPed = 12,
	SubEvtId_tofTest = 400,
	SubEvtId_trigCode = 500,
	SubEvtId_trigInfo = 501,
	SubEvtId_slowTest = 600
};

enum SubEvtDecoding {
	SubEvtDecoding_8bitData = 1,
	SubEvtDecoding_16bitData = (1 << 16) | 1,
	SubEvtDecoding_32bitData = (2 << 16) | 1,
	SubEvtDecoding_text = 2,
	SubEvtDecoding_SubEvts = (3 << 16) | 3
};

#define SubEvt_hdr HadTu_hdr
#define SubEvt_isSwapped HadTu_isSwapped
#define SubEvt_hdrValue HadTu_hdrValue
#define SubEvt_setHdrValue HadTu_setHdrValue
/*#define SubEvt_decoding HadTu_decoding JAM: replaced by function that also masks the trb3 trigger type info*/
/* #define SubEvt_setDecoding HadTu_setDecoding */
#define SubEvt_alignment HadTu_alignment
#define SubEvt_size HadTu_size
#define SubEvt_setSize HadTu_setSize
#define SubEvt_paddedSize HadTu_paddedSize
#define SubEvt_begin HadTu_begin
#define SubEvt_end HadTu_end
#define SubEvt_next HadTu_next


enum SubEvtIdx {
	SubEvtIdx_size,
	SubEvtIdx_decoding,
	SubEvtIdx_id,
	SubEvtIdx_trigNr,
	SubEvtIdx_data
};

static size_t SubEvt_hdrLen(void)
{
	return SubEvtIdx_data;
}

static size_t SubEvt_hdrSize(void)
{
	return SubEvt_hdrLen() * sizeof(uint32_t);
}

static size_t SubEvt_dataSize(const void *my)
{
	return SubEvt_size(my) - SubEvt_hdrSize();
}

static void *SubEvt_data(const void *my)
{
	return (void *) ((char *) my + SubEvt_hdrSize());
}

uint32_t SubEvt_dataValue(const void *my, unsigned idx);
char *SubEvt_2charP(const void *my);


static uint32_t SubEvt_id(const void *my)
{
	return SubEvt_hdrValue(my, SubEvtIdx_id);
}

static uint32_t SubEvt_pureId(const void *my)
{
	return SubEvt_id(my) & 0x7fffffffUL;
}

static int SubEvt_dataError(const void *my)
{
	return (SubEvt_id(my) & 0x80000000UL) != 0;
}

static void SubEvt_setId(void *my, uint32_t id)
{
	SubEvt_setHdrValue(my, SubEvtIdx_id, id);
}

static int SubEvt_setDataError(void *my)
{
	SubEvt_setId(my, SubEvt_id(my) | 0x80000000UL);
}

static int SubEvt_clrDataError(void *my)
{
	SubEvt_setId(my, SubEvt_id(my) & ~0x80000000UL);
}

static uint32_t SubEvt_trigNr(const void *my)
{
	return SubEvt_hdrValue(my, SubEvtIdx_trigNr);
}

static void SubEvt_setTrigNr(void *my, uint32_t trigNr)
{
	SubEvt_setHdrValue(my, SubEvtIdx_trigNr, trigNr);
}

static uint32_t SubEvt_trigType_trb3(const void *my)
{
/* get trb3 trigger type info in bits 4...7*/
	return ((HadTu_decoding(my) & 0xF0) >> 4) ;
}


static uint32_t SubEvt_decoding(const void *my)
{
/* JAM: mask out the trb3 trigger type info in bits 4...7*/
	return (HadTu_decoding(my) & 0xFFFFFF0F);
}

static void SubEvt_setDecoding(void *my, uint32_t decoding)
{
	/* JAM: save and restore trb3 trigger type info in bits 4...7*/
	uint32_t oldtriggertype= HadTu_decoding(my) & 0xF0;
	 HadTu_setDecoding(my, (decoding | oldtriggertype));
}


void *newSubEvt(uint32_t decoding, uint32_t id, uint32_t trigNr);
void deleteSubEvt(void *my);
void *SubEvt_appendData(void *my, const void *data, size_t size);

#ifdef __cplusplus
}
#endif
#endif
