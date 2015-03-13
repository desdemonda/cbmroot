static char *rcsId = "$Header: /misc/hadesprojects/daq/cvsroot/eventbuilder/hadaq/subevt.c,v 6.16 2010-07-06 15:35:51 hadaq Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include "subevt.h"

uint32_t SubEvt_dataValue(const void *my, unsigned idx)
{
	uint32_t val;

	switch (SubEvt_alignment(my)) {
	case 4:
		if (SubEvt_isSwapped(my)) {
			uint32_t tmp0;
			uint32_t tmp1;

			tmp0 = ((uint32_t *) SubEvt_data(my))[idx];
			((char *) &tmp1)[0] = ((char *) &tmp0)[3];
			((char *) &tmp1)[1] = ((char *) &tmp0)[2];
			((char *) &tmp1)[2] = ((char *) &tmp0)[1];
			((char *) &tmp1)[3] = ((char *) &tmp0)[0];
			val = tmp1;
		} else {
			val = ((uint32_t *) SubEvt_data(my))[idx];
		}
		break;
	case 2:
		if (SubEvt_isSwapped(my)) {
			uint16_t v;
			swab(((uint16_t *) SubEvt_data(my)) + idx, &v, 2);
			val = v;
		} else {
			val = ((uint16_t *) SubEvt_data(my))[idx];
		}
		break;
	default:
		val = ((uint8_t *) SubEvt_data(my))[idx];
		break;
	}
	return val;
}

unsigned SubEvt_nrOfDataWords(const void *my)
{
	unsigned i;

	if (SubEvt_decoding(my) == SubEvtDecoding_32bitData) {
		i = SubEvt_dataSize(my) / sizeof(uint32_t) - 1;
	} else if (SubEvt_decoding(my) == SubEvtDecoding_16bitData) {
		i = SubEvt_dataSize(my) / sizeof(uint16_t) - 1;
	} else if (SubEvt_decoding(my) == SubEvtDecoding_8bitData) {
		i = SubEvt_dataSize(my) / sizeof(uint8_t) - 1;
	} else if (SubEvt_decoding(my) == SubEvtDecoding_SubEvts) {
		fprintf(stderr, "ERROR: Crate event are not supported any longer, decoding: %x\n", SubEvt_decoding(my));
	} else {
		fprintf(stderr, "ERROR: Unknown decoding: %i\n", SubEvt_decoding(my));
	}

	return i;
}

uint32_t SubEvt_errBit(const void *my)
{
	unsigned i;
	uint32_t val;

	i = SubEvt_nrOfDataWords(my);

	val = SubEvt_dataValue(my, i);

	return val;
}

uint32_t SubEvt_debugWord(const void *my)
{
	unsigned i;
	uint32_t val;
	unsigned wordNr = -1;

	/* Define debug word for the RICH */
	if (SubEvt_id(my) == 0x00008300UL || SubEvt_id(my) == 0x00008310UL || SubEvt_id(my) == 0x00008320UL) {
		wordNr = 2;
	}

	i = SubEvt_nrOfDataWords(my);

	if (wordNr == -1) {
		val = 0;
	} else if (i > wordNr) {
		val = SubEvt_dataValue(my, wordNr - 1);
	} else {
		val = 0;
	}

	return val;
}

uint32_t SubEvt_trigType(const void *my, uint32_t wordNr, uint32_t bitmask, uint32_t bitshift)
{
	unsigned i;
	uint32_t trigType;
	uint32_t val;

	i = SubEvt_nrOfDataWords(my);

	if (i > wordNr) {
		val = SubEvt_dataValue(my, wordNr - 1);
	} else {
		return 0;
	}

	return (val & bitmask) >> bitshift;
}

char *SubEvt_2charP(const void *my)
{
	static char buf[132];

	sprintf(buf, "size: 0x%08x\tdecoding: 0x%08x\tid: 0x%08x\ttrigNr: 0x%08x",
			SubEvt_size(my), SubEvt_decoding(my), SubEvt_id(my), SubEvt_trigNr(my));

	return buf;
}

void *newSubEvt(uint32_t decoding, uint32_t id, uint32_t trigNr)
{
	void *my;

	my = malloc(SubEvt_hdrSize());

	SubEvt_setSize(my, SubEvt_hdrSize());
	SubEvt_setDecoding(my, decoding);
	SubEvt_setId(my, id);
	SubEvt_setTrigNr(my, trigNr);

	return my;
}

void deleteSubEvt(void *my)
{
	free(my);
}

void *SubEvt_appendData(void *my, const void *data, size_t size)
{

	my = realloc(my, SubEvt_paddedSize(my) + size);
	memcpy(SubEvt_end(my), data, size);
	SubEvt_setSize(my, SubEvt_paddedSize(my) + size);

	return my;
}
