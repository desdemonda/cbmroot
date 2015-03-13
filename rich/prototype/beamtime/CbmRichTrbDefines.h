#ifndef CBMRICHTRBDEFINES_H
#define CBMRICHTRBDEFINES_H


#define EVENT_TIME_WINDOW 300.          // Time window for building event in ns
#define NOF_RAW_EVENTS_IN_BUFFER 1000  // Maximum raw events in the buffer

// for nov2014 beamtime - 33 channels per TDC: 0th channel - reference, then 16 pairs (leading/trailing) for one PADIWA channel
#define TRB_TDC3_CHANNELS 33           //TODO check

#define TRB_TDC3_NUMTDC 4              //TODO check (for nov2014 beamtime - 4)

#define TRB_TDC3_NUMBOARDS 17          //TODO check (for nov2014 beamtime - 17)

#define TRB_TDC3_FINEBINS 0x400        //TODO check

#define TRB_TDC3_COARSEUNIT 5          // 5 ns - hopefully right


#endif // CBMRICHTRBDEFINES_H
