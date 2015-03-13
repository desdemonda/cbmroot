static char *rcsId = "$Header: /misc/hadesprojects/daq/cvsroot/eventbuilder/hadaq/showevt.c,v 6.11 2003-05-14 13:54:45 muench Exp $";


#define _ANSI_C_SOURCE
#include <stddef.h>

#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stdlib.h>

#include "hldread.h"

typedef struct OurArgsS {
	int noPrint;
	int debug;
} OurArgs;
static OurArgs ourArgsS = { 0 }, *ourArgs = &ourArgsS;

int startAnalysis(int argc, char **argv)
{
	int i;
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "noprint") == 0) {
			ourArgs->noPrint = 1;
		}
		if (strcmp(argv[i], "debug") == 0) {
			ourArgs->debug = 1;
		}
	}
	if (ourArgs->debug) {
		fprintf(stderr, "startAnalysis called\n");
	}
	return 0;
}

int analyseEvt(void *evt)
{
	int i;
	void *subEvt;

	if (ourArgs->debug) {
		fprintf(stderr, "analyseEvt called\n");
	}

	if (!ourArgs->noPrint) {
		/* print the event header */
		printf("size: 0x%08x  decoding: 0x%08x  id:    0x%08x  seqNr:  0x%08x\n"
			   "date: %s  time:     %s    runNr: 0x%08x  expId:  %s\n",
			   Evt_size(evt), Evt_decoding(evt), Evt_id(evt), Evt_seqNr(evt),
			   Evt_date2charP(evt), Evt_time2charP(evt), Evt_runNr(evt), Evt_expId2charP(evt)
			);
	}
	if (Evt_size(evt) < Evt_hdrSize()) {
		fprintf(stderr, "   evt: %s\n", Evt_2charP(evt));
		fprintf(stderr, "Evt_size(evt) < Evt_hdrSize(), abort\n");
		abort();
	}

	i = 0;
	/* loop over all subevents */
	for (subEvt = Evt_data(evt); subEvt < Evt_end(evt); subEvt = Evt_next(evt, subEvt)) {
		if (!ourArgs->noPrint) {
			printf("size: 0x%08x  decoding: 0x%08x  id:    0x%08x  trigNr: 0x%08x trigTypeTRB3: 0x%01x\n",
				   SubEvt_size(subEvt), SubEvt_decoding(subEvt), SubEvt_id(subEvt), SubEvt_trigNr(subEvt), SubEvt_trigType_trb3(subEvt));
			if (SubEvt_decoding(subEvt) == SubEvtDecoding_32bitData) {
				for (i = 0; i < SubEvt_dataSize(subEvt) / sizeof(uint32_t); i++) {
					if (i % 4 == 0) {	/* newline and the offset in the subEvt */
						printf("\n%08x:", i * sizeof(uint32_t));
					}
					printf("  0x%08x", SubEvt_dataValue(subEvt, i));
				}
			} else if (SubEvt_decoding(subEvt) == SubEvtDecoding_16bitData) {
				for (i = 0; i < SubEvt_dataSize(subEvt) / sizeof(uint16_t); i++) {
					if (i % 8 == 0) {	/* newline and the offset in the subEvt */
						printf("\n%08x:", i * sizeof(uint16_t));
					}
					printf("  0x%04x", SubEvt_dataValue(subEvt, i));
				}
			} else if (SubEvt_decoding(subEvt) == SubEvtDecoding_8bitData) {
				for (i = 0; i < SubEvt_dataSize(subEvt) / sizeof(uint8_t); i++) {
					if (i % 16 == 0) {	/* newline and the offset in the subEvt */
						printf("\n%08x:", i);
					}
					printf("  0x%02x", SubEvt_dataValue(subEvt, i));
				}
			} else if (SubEvt_decoding(subEvt) == SubEvtDecoding_text) {
				char *data;

				printf("%c", '\n');
				data = (char *) SubEvt_data(subEvt);
				for (i = 0; i < SubEvt_dataSize(subEvt); i++) {
					printf("%c", data[i]);
				}
			}
			printf("%c", '\n');
		}
		if (SubEvt_size(subEvt) < SubEvt_hdrSize()) {
			fprintf(stderr, "   evt: %s\n", Evt_2charP(evt));
			fprintf(stderr, "subEvt: %s\n", SubEvt_2charP(subEvt));
			fprintf(stderr, "SubEvt_size(subEvt) < SubEvt_hdrSize(), skip rest of event\n");
			break;
		} else if (SubEvt_size(subEvt) > Evt_size(evt) - Evt_hdrSize()) {
			fprintf(stderr, "   evt: %s\n", Evt_2charP(evt));
			fprintf(stderr, "subEvt: %s\n", SubEvt_2charP(subEvt));
			fprintf(stderr, "SubEvt_size(subEvt) < Evt_size(evt) - Evt_hdrSize(), skip rest of event\n");
			break;
		}
	}
	if (!ourArgs->noPrint) {
		printf("%c", '\n');
	}
	return 0;
}

int stopAnalysis(void)
{
	if (ourArgs->debug) {
		fprintf(stderr, "stopAnalysis called\n");
	}
	return 0;
}
