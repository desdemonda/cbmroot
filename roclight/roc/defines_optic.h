#ifndef ROC_DEFINES_OPTIC_H
#define ROC_DEFINES_OPTIC_H

#define ROC_OPTICS_HWV     0x200000        // r

// true if there are radiation mitigation techniques in use
#define ROC_OPTICS_RADTOL     0x200004        // r

// enable/disable data generator
#define ROC_OPTICS_DATAGEN 0x200300        // W

// start daq, enables sending of data from ROC via optic
#define ROC_OPTICS_START_DAQ      0x200100       // w

// stop daq, disables sending of data from ROC via optic
#define ROC_OPTICS_STOP_DAQ       0x200104        // w

#define ROC_OPTICS_DLMGEN         0x201000        // w
#define ROC_OPTICS_DLM_HISTORY    0x201004        // r


#endif
