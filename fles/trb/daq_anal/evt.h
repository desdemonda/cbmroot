#ifndef EVT_H
#define EVT_H

#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "hadtu.h"
#include "subevt.h"

#define EVT_MAXSIZE (1024 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

static const int evtBlockSize = 8 * 1024;

enum EvtId {
	EvtId_data = 1,
	EvtId_runStart = 0x00010002,
	EvtId_runStop = 0x00010003
};

enum EvtDecoding {
	EvtDecoding_default = 1,
	EvtDecoding_64bitAligned = (0x03 << 16) | 0x0001
};

#define Evt_hdr HadTu_hdr
#define Evt_isSwapped HadTu_isSwapped
#define Evt_hdrValue HadTu_hdrValue
#define Evt_setHdrValue HadTu_setHdrValue
#define Evt_decoding HadTu_decoding
#define Evt_setDecoding HadTu_setDecoding
#define Evt_alignment HadTu_alignment
#define Evt_size HadTu_size
#define Evt_setSize HadTu_setSize
#define Evt_paddedSize HadTu_paddedSize
#define Evt_begin HadTu_begin
#define Evt_end HadTu_end
#define Evt_next HadTu_next

size_t Evt_hdrLen(void);
size_t Evt_hdrSize(void);
size_t Evt_dataSize(const void *my);
void *Evt_data(const void *my);
char *Evt_date2charP(const void *my);
char *Evt_time2charP(const void *my);
char *Evt_2charP(const void *my);
char *Evt_expId2charP(const void *my);
void Evt_setExpIdCharP(void *my, const char *expId);

uint32_t Evt_id(const void *my);

static uint32_t Evt_pureId(const void *my)
{
	return Evt_id(my) & 0x7fffffffUL;
}

static int Evt_dataError(const void *my)
{
	return (Evt_id(my) & 0x80000000UL) != 0;
}

void Evt_setId(void *my, uint32_t id);

static int Evt_setDataError(void *my)
{
	Evt_setId(my, Evt_id(my) | 0x80000000UL);
}

static int Evt_clrDataError(void *my)
{
	Evt_setId(my, Evt_id(my) & ~0x80000000UL);
}

uint32_t Evt_seqNr(const void *my);
void Evt_setSeqNr(void *my, uint32_t seqNr);
uint32_t Evt_date(const void *my);
void Evt_setDate(void *my, uint32_t date);
uint32_t Evt_time(const void *my);
void Evt_setTime(void *my, uint32_t time);
uint32_t Evt_runNr(const void *my);
uint32_t Evt_expId(const void *my);
void Evt_setRunNr(void *my, uint32_t runNr);
void Evt_setExpId(void *my, uint32_t expId);

void *newEvt(uint32_t decoding, uint32_t id);
void deleteEvt(void *my);
void *Evt_fget(FILE *file);
void *Evt_appendSubEvt(void *my, void *subEvt);

#ifdef __cplusplus
}
#endif
#endif
