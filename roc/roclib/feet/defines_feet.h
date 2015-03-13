#ifndef ROC_DEFINES_FEET_H
#define ROC_DEFINES_FEET_H

#define ROC_FEET_HWV                 0x500000   // r,   32 bit

#define ROC_FEET_RADTOL              0x500004   // r,    1 bit

#define ROC_FEET_NR_OF_FEETS         0x500008   // r,    8 bit

#define ROC_FEET_RESET               0x500010   // w,    0 bit

#define ROC_FEET_FIFO_RESET          0x500014   // w,    1 bit

#define ROC_FEET_SAMPLE_FALLING_EDGE 0x500200   // r/w,  1 bit

#define ROC_FEET_EPOCH250_EN			 0x500204   // r/w,  1 bit

#define ROC_FEET_RECEIVE_MASK        0x501000   // r/w, 32 bit

#define ROC_FEET_TRANSMIT_MASK       0x501004   // r/w, 32 bit

#define ROC_FEET_DATAGEN_MASK        0x501008   // r/w, 32 bit

#define ROC_FEET_DATAGEN_INIT        0x50100C   // r/w, 32 bit

#define ROC_FEET_DATAGEN_EN          0x501010   // w,    1 bit

#define ROC_FEET_CMD_TO_FEET         0x510000   // w,    8 bit

#endif
