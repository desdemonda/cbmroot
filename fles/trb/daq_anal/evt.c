static char *rcsId = "$Header: /misc/hadesprojects/daq/cvsroot/eventbuilder/hadaq/evt.c,v 6.18 2010-01-29 18:24:02 hadaq Exp $";


#define _ANSI_C_SOURCE
#include <stddef.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <syslog.h>

#include "evt.h"
#include "subevt.h"

enum EvtIdx {
	EvtIdx_size,
	EvtIdx_decoding,
	EvtIdx_id,
	EvtIdx_seqNr,
	EvtIdx_date,
	EvtIdx_time,
	EvtIdx_runNr,
	EvtIdx_expId,
	EvtIdx_data
};

size_t Evt_hdrLen(void)
{
	return EvtIdx_data;
}

size_t Evt_hdrSize(void)
{
	return Evt_hdrLen() * sizeof(uint32_t);
}

size_t Evt_dataSize(const void *my)
{
	return Evt_size(my) - Evt_hdrSize();
}

void *Evt_data(const void *my)
{
	return (void *) ((char *) my + Evt_hdrSize());
}

uint32_t Evt_id(const void *my)
{
	return Evt_hdrValue(my, EvtIdx_id);
}

void Evt_setId(void *my, uint32_t id)
{
	Evt_setHdrValue(my, EvtIdx_id, id);
}

uint32_t Evt_seqNr(const void *my)
{
	return Evt_hdrValue(my, EvtIdx_seqNr);
}

void Evt_setSeqNr(void *my, uint32_t seqNr)
{
	Evt_setHdrValue(my, EvtIdx_seqNr, seqNr);
}

uint32_t Evt_date(const void *my)
{
	return Evt_hdrValue(my, EvtIdx_date);
}

void Evt_setDate(void *my, uint32_t date)
{
	Evt_setHdrValue(my, EvtIdx_date, date);
}

uint32_t Evt_time(const void *my)
{
	return Evt_hdrValue(my, EvtIdx_time);
}

void Evt_setTime(void *my, uint32_t time)
{
	Evt_setHdrValue(my, EvtIdx_time, time);
}

uint32_t Evt_runNr(const void *my)
{
	return Evt_hdrValue(my, EvtIdx_runNr);
}

void Evt_setRunNr(void *my, uint32_t runNr)
{
	Evt_setHdrValue(my, EvtIdx_runNr, runNr);
}

uint32_t Evt_expId(const void *my)
{
	return Evt_hdrValue(my, EvtIdx_expId);
}

void Evt_setExpId(void *my, uint32_t expId)
{
	Evt_setHdrValue(my, EvtIdx_expId, expId);
}

char *Evt_expId2charP(const void *my)
{
	static char buffer[3];
	uint32_t expId;

	expId = Evt_expId(my);

	buffer[0] = (expId >> 8) & 0xff;
	buffer[1] = expId & 0xff;
	buffer[2] = '\0';

	return buffer;
}

void Evt_setExpIdCharP(void *my, const char *expId)
{
	Evt_setExpId(my, (expId[0] << 8) | expId[1]);
}

char *Evt_date2charP(const void *my)
{
	time_t tempo;
	struct tm *gmTime;
	static char buffer[80];

	tempo = time(NULL);
	gmTime = gmtime(&tempo);
	gmTime->tm_year = (Evt_date(my) >> 16) & 0xff;
	gmTime->tm_mon = (Evt_date(my) >> 8) & 0xff;
	gmTime->tm_mday = (Evt_date(my) >> 0) & 0xff;
	strftime(buffer, 80, "%Y-%m-%d", gmTime);

	return buffer;
}

char *Evt_time2charP(const void *my)
{
	time_t tempo;
	struct tm *gmTime;
	static char buffer[80];

	tempo = time(NULL);
	gmTime = gmtime(&tempo);
	gmTime->tm_hour = (Evt_time(my) >> 16) & 0xff;
	gmTime->tm_min = (Evt_time(my) >> 8) & 0xff;
	gmTime->tm_sec = (Evt_time(my) >> 0) & 0xff;
	strftime(buffer, 80, "%H:%M:%S", gmTime);

	return buffer;
}

char *Evt_2charP(const void *my)
{
	static char buf[2 * 132];

	sprintf(buf,
			"size: 0x%08x\tdecoding: 0x%08x\tid: 0x%08x\tseqNr: 0x%08x\t"
			"date: %s\ttime: %s\trunNr: %d\texpId: %x",
			Evt_size(my), Evt_decoding(my), Evt_id(my), Evt_seqNr(my),
			Evt_date2charP(my), Evt_time2charP(my), Evt_runNr(my), Evt_expId(my)
		);

	return buf;
}

void *newEvt(uint32_t decoding, uint32_t id)
{
	void *my;
	time_t tempo;
	struct tm *gmTime;
	uint32_t dummy;

	my = malloc(EVT_MAXSIZE);

	Evt_setSize(my, Evt_hdrSize());
	Evt_setDecoding(my, decoding);
	Evt_setId(my, id);

	tempo = time(NULL);
	gmTime = gmtime(&tempo);

	dummy = 0;
	dummy |= gmTime->tm_year << 16;
	dummy |= gmTime->tm_mon << 8;
	dummy |= gmTime->tm_mday;
	Evt_setDate(my, dummy);

	dummy = 0;
	dummy |= gmTime->tm_hour << 16;
	dummy |= gmTime->tm_min << 8;
	dummy |= gmTime->tm_sec;
	Evt_setTime(my, dummy);

	return my;
}

void deleteEvt(void *my)
{
	free(my);
}

void *Evt_appendSubEvt(void *my, void *subEvt)
{
	if (SubEvt_decoding(subEvt) == SubEvtDecoding_SubEvts) {
		memcpy(Evt_end(my), SubEvt_data(subEvt), SubEvt_dataSize(subEvt));
		Evt_setSize(my, Evt_paddedSize(my) + SubEvt_dataSize(subEvt));
	} else {
		memcpy(Evt_end(my), subEvt, SubEvt_size(subEvt));
		Evt_setSize(my, Evt_paddedSize(my) + SubEvt_size(subEvt));
	}

	return my;
}

void *Evt_fget(FILE *file)
{
	static char pool[EVT_MAXSIZE];
	static void *evt = pool;

	if (Evt_hdrSize() != fread(evt, 1, Evt_hdrSize(), file)) {
		syslog(LOG_DEBUG, "%s:%d: %s", __FILE__, __LINE__, strerror(errno));
		return NULL;
	}
	/* BUGBUG quickhack for a "Evt_paddedDataSize" */
	if (Evt_paddedSize(evt) - Evt_hdrSize() != fread(Evt_data(evt), 1, Evt_paddedSize(evt) - Evt_hdrSize(), file)) {
		syslog(LOG_ERR, "%s:%d: %s", __FILE__, __LINE__, strerror(errno));
		return NULL;
	}
	return evt;
}
