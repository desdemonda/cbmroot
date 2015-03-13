#ifndef GET4_V1_X_DEF_H
#define GET4_V1_X_DEF_H

// ROOT headers
//#include "Rtypes.h"

namespace get4v1x {

   enum Get4Message32bTypes {
      GET4_32B_EPOCH = 0,
      GET4_32B_SLCM  = 1,
      GET4_32B_ERROR = 2,
      GET4_32B_DATA  = 3
   };

   enum Get4Message32bErrors {
      GET4_V1X_ERR_READ_INIT   = 0x00,
      GET4_V1X_ERR_SYNC        = 0x01,
      GET4_V1X_ERR_EP_CNT_SYNC = 0x02,
      GET4_V1X_ERR_EP          = 0x03,
      GET4_V1X_ERR_FIFO_WRITE  = 0x04,
      GET4_V1X_ERR_LOST_EVT    = 0x05,
      GET4_V1X_ERR_CHAN_STATE  = 0x06,
      GET4_V1X_ERR_TOK_RING_ST = 0x07,
      GET4_V1X_ERR_TOKEN       = 0x08,
      GET4_V1X_ERR_READOUT_ERR = 0x09,
      GET4_V1X_ERR_SPI         = 0x0A,
      GET4_V1X_ERR_DLL_LOCK    = 0x0B,
      GET4_V1X_ERR_DLL_RESET   = 0x0C,
      GET4_V1X_ERR_TOT_OVERWRT = 0x11,
      GET4_V1X_ERR_TOT_RANGE   = 0x12,
      GET4_V1X_ERR_EVT_DISCARD = 0x13,
      GET4_V1X_ERR_UNKNOWN     = 0x7F
   };
   // Hardware max Number
   const uint32_t kuMaxRoc        =  15;
   const uint32_t kuMaxGet4       = 256; // For ROC v3
   const uint32_t kuMaxGet4PerRoc = 128; // For ROC v3
   const uint32_t kuChanPerGet4   =   4;

   // Size of one clock cycle (=1 coarse bin)
   const double   kdClockCycleSize  = 6400.0; //[ps]
   // TODO:For now make 100ps default, maybe need later an option for it
   const double   kdTotBinSize      = 100.0; //ps

   const uint32_t kuFineTime    = 0x0000007F; // Fine Counter value
   const uint32_t kuFtShift     =          0; // Fine Counter offset
   const uint32_t kuCoarseTime  = 0x0007FF80; // Coarse Counter value
   const uint32_t kuCtShift     =          7; // Coarse Counter offset

   const uint32_t kuCoarseCounterSize  = ( (kuCoarseTime>>kuCtShift)+1 );
   const uint32_t kuCoarseOverflowTest = kuCoarseCounterSize / 2 ; // Limit for overflow check

   // Nominal bin size of NL are neglected
   const double   kdBinSize     = kdClockCycleSize / static_cast<double>(kuFineTime + 1);
   // Epoch Size in bins
   const uint32_t kuEpochInBins = kuFineTime + kuCoarseTime + 1;
   // Epoch Size in ps
   // alternatively: (kiCoarseTime>>kiCtShift + 1)*kdClockCycleSize
   const double   kdEpochInPs   = kuEpochInBins*kdBinSize;

   // Size of the epoch counters in ROC messages
   const uint64_t kulMainEpochCycleSz = 0xFFFFFFFFuLL; // for ROC  250.00 MHz clock
   const uint64_t kulGet4EpochCycleSz = 0x00FFFFFFuLL; // for GET4 156.25 MHz clock in 32b mode (Hack)
   const uint64_t kul24bGet4EpochCycleSz = 0xFFFFFFFFuLL; // for GET4 156.25 MHz clock in 24b mode (ROC check)

   // Size of the nominal SYNC cycles
   // (aka nb of epoch between 2 epochs with SYNC flag when no problem)
   const uint32_t kuSyncCycleSzMain =  40;
   const uint32_t kuSyncCycleSzGet4 =  25;
}

#endif // GET4_V1_X_DEF_H
