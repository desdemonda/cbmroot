// C/C++ standard library headers
#include <sstream>
#include <stdint.h>
#include <vector>
#include <ctime>

// FLES control LIB main header
#include "fles/ctrl/control/ControlProtocol.hpp"


// Generic ROC + FLIB registers: GPIO
#include "fles/nxyter_ctrl/defines_gpio.h"

// NX + ROC registers: NX I2C, NX SPI
//#include "roc/roclib/roc/defines_i2c.h"
//#include "fles/nxyter_ctrl/defines_spi.h"

// GET4 v1.x ROC registers: generic ROC, generic OPTICS, GET4
#include "fles/get4_ctrl/defines_roc_get4v1x.h"

// GET4 v1.x internal registers address
#include "fles/get4_ctrl/defines_get4v1x.h"

// GET4 v1.x ROC message tool functions => maybe already loaded from libCbmFlibReader.so?
#include "fles/reader/unpacker/CbmGet4v1xHackDef.h"

// Symbols from ControlClient.hpp are in libcbmnetcntlclientroot.so, loaded in rootlogon.C
// This is to let your IDE know about CbmNet::ControlClient class, and thus make
// autocompletion possible
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "fles/ctrl/control/libclient/ControlClient.hpp"
#endif

using std::ostringstream;

int SendSpi(CbmNet::ControlClient & conn, uint32_t nodeid,
            uint32_t get4idx, uint16_t configBits,
            uint32_t nbData, uint32_t* data)
{
   CbmNet::ListSeq initList;
   int ret_val = 0;

   // Read current chips mask
   uint32_t uMaskOldLsbs;
   uint32_t uMaskOldMsbs;
   conn.Read( nodeid, ROC_GET4_TRANSMIT_MASK_LSBS, uMaskOldLsbs );
   conn.Read( nodeid, ROC_GET4_TRANSMIT_MASK_MSBS, uMaskOldMsbs );
   printf("SendSpi: Restore Get4 chip mask %08X %08X\n", uMaskOldMsbs, uMaskOldLsbs);

   // Send only to chosen get4
   uint32_t uMaskLsbs = 0x00000000;
   uint32_t uMaskMsbs = 0x00000000;
   if( get4idx < 32 )
   {
      uMaskLsbs += 0x1 << get4idx;
      uMaskMsbs = 0x00000000;
   } // if( get4idx < 32 )
      else if( get4idx < 64 )
      {
         uMaskLsbs = 0x00000000;
         uMaskMsbs = 0x1 << (get4idx - 32);
      } // else of if( get4idx < 32 )
      else
      {
         printf("SendSpi: Invalid get4 chip index (%u) or link stopping there\n", get4idx);
         return 0;
      }
   printf("SendSpi: Get4 chip mask (index %u) %08X %08X\n", get4idx, uMaskMsbs, uMaskLsbs);
   initList.AddWrite(ROC_GET4_TRANSMIT_MASK_LSBS, uMaskLsbs);
   initList.AddWrite(ROC_GET4_TRANSMIT_MASK_MSBS, uMaskMsbs);

   //# ROC_GET4_CMD_TO_GET4 => Disable all channels
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK    + 0x00000F );

   // Send GET4 command Config SPI
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SPI_CONF    + (configBits & 0x7FF) );

   // Send SPI START
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SPI_START );

   // Send SPI DATA by blocks of 24 bits
   for( int iData = 0; iData < nbData; iData++ )
      initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SPI_DATA + ((data[iData]) & 0xFFFFFF) );

   // Send SPI STOP
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SPI_STOP );

   //# ROC_GET4_CMD_TO_GET4 => Enable all channels
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK    + 0x000000 );

   // Process the list of commands
   ret_val = conn.DoListSeq(nodeid, initList);

   // restore original chips mask
   printf("SendSpi: Restore Get4 chip mask %08X %08X\n", uMaskOldMsbs, uMaskOldLsbs);
   conn.Write( nodeid, ROC_GET4_TRANSMIT_MASK_LSBS, uMaskOldLsbs );
   conn.Write( nodeid, ROC_GET4_TRANSMIT_MASK_MSBS, uMaskOldMsbs );

   return ret_val;
}

int SetRocDef(CbmNet::ControlClient & conn, uint32_t nodeid)
{
   CbmNet::ListSeq initList;
   // rocutil> ssm 2
   initList.AddWrite(base::GPIO_SYNCM_SCALEDOWN,        2);
   initList.AddWrite(base::GPIO_SYNCM_BAUD_START,       1);
   initList.AddWrite(base::GPIO_SYNCM_BAUD1,            3);
   initList.AddWrite(base::GPIO_SYNCM_BAUD2,            3);

   // rocutil> setsyncs 0,1 loop=1
   initList.AddWrite(base::GPIO_CONFIG,               900);
   initList.AddWrite(base::GPIO_SYNCS0_BAUD_START,      1);
   initList.AddWrite(base::GPIO_SYNCS0_BAUD1,           3);
   initList.AddWrite(base::GPIO_SYNCS0_BAUD2,           3);

   // rocutil> setaux 2,re=1,fe=0
   initList.AddWrite(base::GPIO_CONFIG,           5000900);
   return conn.DoListSeq(nodeid, initList);
}

int Set24bDef(CbmNet::ControlClient & conn, uint32_t nodeid)
{
   CbmNet::ListSeq initList;

//# ROC_GET4_RECEIVE_CLK_CFG => Set link speed to 156.25 MBit/s
   initList.AddWrite(ROC_GET4_RECEIVE_CLK_CFG,           3);

// ROC_EPOCH250_EN => Enable/Disable the production of 250MHz (ROC)
//                    epochs
   initList.AddWrite(ROC_EPOCH250_EN,                    0);

//# ROC_GET4_RECEIVE_MASK_LSBS & ROC_GET4_RECEIVE_MASK_MSBS
//  => Activate only the 4 first chips
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000BA4);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_MSBS, 0x00000000);
///////// For ROC v3 single board setup
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000001);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000002);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000004);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000008);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x0000000F);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000010);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000020);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000040);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000080);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x000000F0);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x0000FFFF);
///////// For ROC v3 symmetric dual board setup
   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x000000FF);
   initList.AddWrite(ROC_GET4_RECEIVE_MASK_MSBS, 0x00000000);


//# ROC_GET4_SAMPLE_FALLING_EDGE_LSBS & ROC_GET4_SAMPLE_FALLING_EDGE_MSBS
//  => Change the edge on which the data from the GET4 are sampled
//  => Can be necessary with some ROC v2 systems 
//   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_LSBS, 0x0000004B);
//   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_MSBS, 0x00000000);
///////// For ROC v3 single board setup
   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_LSBS, 0x00000000);
   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_MSBS, 0x00000000);


//# ROC_GET4_SUPRESS_EPOCHS_LSBS & ROC_GET4_SUPRESS_EPOCHS_MSBS
//  => Enable the suppression of the 156.25 MHz epoch without data messages
//  => Chip granularity setting, maybe interesting to keep at least one per ROC 
   initList.AddWrite(ROC_GET4_SUPRESS_EPOCHS_LSBS, 0x00000000);
   initList.AddWrite(ROC_GET4_SUPRESS_EPOCHS_MSBS, 0x00000000);

//# ROC_GET4_READOUT_MODE => 24 bits mode selection
   initList.AddWrite(ROC_GET4_READOUT_MODE,               0);

//# Define CMD-list 2 (Reset + Start-DAQ)
//# ROC_STOP_DAQ
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x00, ROC_CMD_LST_PUT + ROC_STOP_DAQ);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x04, 0);
//# ROC_GET4_CMD_TO_GET4 => Initialize epoch counter to 0
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x08, ROC_CMD_LST_PUT + ROC_GET4_CMD_TO_GET4);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x0C, GET4V1X_EP_SYNC_INIT + 0x000000 );
//# ROC_GET4_TS156_RESET => Resets timestamp in ROC for 156.25 MHz clock
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x10, ROC_CMD_LST_PUT + ROC_GET4_TS156_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x14, 1);
//# ROC_TS_RESET => Resets timestamp in ROC for 250.00 MHz clock
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x18, ROC_CMD_LST_PUT + ROC_TS_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x1C, 1);
//# ROC_FIFO_RESET (active high?)
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x20, ROC_CMD_LST_PUT + ROC_FIFO_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x24, 1);
//# ROC_FIFO_RESET
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x28, ROC_CMD_LST_PUT + ROC_FIFO_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x2C, 0);
//# ROC_ADDSYSMSG
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x30, ROC_CMD_LST_PUT + ROC_ADDSYSMSG);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x34, 0x00000010);
//# ROC_START_DAQ
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x38, ROC_CMD_LST_PUT + ROC_START_DAQ);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x3C, 0);
//# ROC_CMD_LST_ACTIVE
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x40, ROC_CMD_LST_PUT + ROC_CMD_LST_ACTIVE );
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x44, 0);

//# GET4 configuration
   //# GET4V1X_TDC_CORE_TIMING_CONF => DLL TDC core timing
   //   (bit 4 enable, bits3..0 : phase shift in 400ps steps)
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_TDC_CORE_TIMING_CONF + 0x000012 );

   //# GET4V1X_DLL_DAC_MIN => lower DLL monitoring threshold 450 mV
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_DLL_DAC_MIN          + 0x000100 );

   //# GET4V1X_DLL_DAC_MAX => upper DLL monitoring threshold 1000 mV
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_DLL_DAC_MAX          + 0x000238 );

   //# ROC_GET4_CMD_TO_GET4 => Disable all channels
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK             + 0x00000F );

   //# GET4V1X_SERIAL_FLOW_CONTR => disable serial interface
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SERIAL_FLOW_CONTR    + 0x000001 );

   //# ROC_GET4_CMD_TO_GET4 => Reset Readout and configuration
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_INIT_ROCONF_RST );

   //# ROC_GET4_CMD_TO_GET4 => Enable DLL Auto Reset
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_INIT_DLL_RST_AUTO    +      0x1 );

   //# ROC_GET4_CMD_TO_GET4 => Enable 24b readout mode, disable LostEventError messages
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_RO_CONF_BASIC        +      0x0 );

   //# ROC_GET4_CMD_TO_GET4 => Select Readout 1 diagnostic for channel 1 Rising edge
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_DIAG_SELECT          +     0x23 );

   //# ROC_GET4_CMD_TO_GET4 => Set link speed to 156.25 MBit/s
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_24B_RO_CONF          +      0x7 );

// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to 156.25 MBit/s
   initList.AddWrite(ROC_GET4_RECEIVE_CLK_CFG,           0x3);

   //# ROC_GET4_CMD_TO_GET4 => Re-Initialize the readout unit state machine
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_INIT_RO_INIT );

   //# GET4V1X_SERIAL_FLOW_CONTR => enable serial interface
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SERIAL_FLOW_CONTR    + 0x000000 );

   //# ROC_GET4_CMD_TO_GET4 => Enable all channels
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK             + 0x000000 );

   //# ROC_GET4_CMD_TO_GET4 => SCv3 test: disable bad channels
//   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK             + 0x00000F );

   return conn.DoListSeq(nodeid, initList);
}

int Set32bDef(CbmNet::ControlClient & conn, uint32_t nodeid)
{
   CbmNet::ListSeq initList;


// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to 19.XX MBit/s
//   initList.AddWrite(ROC_GET4_RECEIVE_CLK_CFG,           0);

// ROC_EPOCH250_EN => Enable/Disable the production of 250MHz (ROC)
//                    epochs
   initList.AddWrite(ROC_EPOCH250_EN,                    0);

// ROC_GET4_RECEIVE_MASK_LSBS & ROC_GET4_RECEIVE_MASK_MSBS
//  => Activate only the 4 first chips
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x000002BC);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x000002BC);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_MSBS, 0x00000000);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0xFFFFFFFF);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_MSBS, 0xFFFFFFFF);
///////// For ROC v3 single board setup
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x000000FF);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x0000FF00);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00FF0000);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0xFF000000);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x0000FFFF);
///////// For ROC v3 symmetric dual board setup
   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x000000FF);
//   initList.AddWrite(ROC_GET4_RECEIVE_MASK_LSBS, 0x00000001);
   initList.AddWrite(ROC_GET4_RECEIVE_MASK_MSBS, 0x00000000);

// ROC_GET4_SAMPLE_FALLING_EDGE_LSBS & ROC_GET4_SAMPLE_FALLING_EDGE_MSBS
//  => Change the edge on which the data from the GET4 are sampled
//  => Can be necessary with some ROC v2 systems
//   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_LSBS, 0x00000004);
//   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_LSBS, 0x000000B4);
//   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_MSBS, 0x00000000);
///////// For ROC v3 single board setup
   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_LSBS, 0x00000000);
   initList.AddWrite(ROC_GET4_SAMPLE_FALLING_EDGE_MSBS, 0x00000000);

// ROC_GET4_SUPRESS_EPOCHS_LSBS & ROC_GET4_SUPRESS_EPOCHS_MSBS
//  => Enable the suppression of the 156.25 MHz epoch without data messages
//  => Chip granularity setting, maybe interesting to keep at least one per ROC
   initList.AddWrite(ROC_GET4_SUPRESS_EPOCHS_LSBS, 0x00000000);
   initList.AddWrite(ROC_GET4_SUPRESS_EPOCHS_MSBS, 0x00000000);

//# ROC_GET4_READOUT_MODE => 32 bits mode selection
//   initList.AddWrite(ROC_GET4_READOUT_MODE,               1);

// Define CMD-list 2 (Reset + Start-DAQ)
// ROC_STOP_DAQ
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x00, ROC_CMD_LST_PUT + ROC_STOP_DAQ);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x04, 0);
// ROC_GET4_CMD_TO_GET4 => Initialize epoch counter to 0
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x08, ROC_CMD_LST_PUT + ROC_GET4_CMD_TO_GET4);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x0C, GET4V1X_EP_SYNC_INIT + 0x000000 );
// ROC_GET4_TS156_RESET => Resets timestamp in ROC for 156.25 MHz clock
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x10, ROC_CMD_LST_PUT + ROC_GET4_TS156_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x14, 1);
// ROC_TS_RESET => Resets timestamp in ROC for 250.00 MHz clock
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x18, ROC_CMD_LST_PUT + ROC_TS_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x1C, 1);
// ROC_FIFO_RESET (active high?)
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x20, ROC_CMD_LST_PUT + ROC_FIFO_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x24, 1);
// ROC_FIFO_RESET
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x28, ROC_CMD_LST_PUT + ROC_FIFO_RESET);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x2C, 0);
//# ROC_ADDSYSMSG
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x30, ROC_CMD_LST_PUT + ROC_ADDSYSMSG);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x34, 0x00000010);
//# ROC_START_DAQ
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x38, ROC_CMD_LST_PUT + ROC_START_DAQ);
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x3C, 0);
//# ROC_CMD_LST_ACTIVE
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x40, ROC_CMD_LST_PUT + ROC_CMD_LST_ACTIVE );
   initList.AddWrite(ROC_CMD_LST_MEM + 0x200 + 0x44, 0);

// GET4 configuration
   //# GET4V1X_TDC_CORE_TIMING_CONF => DLL TDC core timing
   //   (bit 4 enable, bits3..0 : phase shift in 400ps steps)
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_TDC_CORE_TIMING_CONF + 0x000012 );

   //# GET4V1X_DLL_DAC_MIN => lower DLL monitoring threshold 450 mV
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_DLL_DAC_MIN          + 0x000100 );

   //# GET4V1X_DLL_DAC_MAX => upper DLL monitoring threshold 1000 mV
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_DLL_DAC_MAX          + 0x000238 );

   // ROC_GET4_CMD_TO_GET4 => Disable all channels
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK          + 0x00000F );

   //# GET4V1X_SERIAL_FLOW_CONTR => disable serial interface  
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SERIAL_FLOW_CONTR    + 0x000001 );

   // ROC_GET4_CMD_TO_GET4 => Reset Readout and configuration
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_INIT_ROCONF_RST );

   // ROC_GET4_CMD_TO_GET4 => Enable DLL Auto Reset
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_INIT_DLL_RST_AUTO +      0x1 );

   // ROC_GET4_CMD_TO_GET4 => Enable 32b readout mode, disable LostEventError messages
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_RO_CONF_BASIC     +      0x1 );

   // ROC_GET4_CMD_TO_GET4 => Select Readout 1 diagnostic for channel 1 Rising edge
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_DIAG_SELECT       +     0x23 );

   // ROC_GET4_CMD_TO_GET4 => Time over Threshold Configuration: 
   // => DI ON, AutoHeal ON, 2 bins resolution (100ps)
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_32B_RO_CONF_TOT   +      0xD );

   // ROC_GET4_CMD_TO_GET4 => Time over Threshold Range: 
   // Bits maximum = 8 bits output (FF) * n bits resolution = FF (1 bin) to 7FF (8 bins)
   // Eventually can be set to higher than payload capacity => overflow (saturation?)
   // => Set here to to 1FF
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_32B_RO_CONF_TOT_MAX +  0x1FF );

   // ROC_GET4_CMD_TO_GET4 => Set link speed to 156.25 MBit/s
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_32B_RO_CONF_LNK_RATE +   0x6 );

// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to 156.25 MBit/s
   initList.AddWrite(ROC_GET4_RECEIVE_CLK_CFG,           0x3);

   // ROC_GET4_CMD_TO_GET4 => Re-Initialize the readout unit state machine
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_INIT_RO_INIT );

   //# GET4V1X_SERIAL_FLOW_CONTR => enable serial interface
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_SERIAL_FLOW_CONTR    + 0x000000 );

   // ROC_GET4_CMD_TO_GET4 => Enable all channels
   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK          + 0x000000 );

//# ROC_GET4_READOUT_MODE => 32 bits mode selection
   initList.AddWrite(ROC_GET4_READOUT_MODE,               1);

   //# ROC_GET4_CMD_TO_GET4 => SCv3 test: disable bad channels
//   initList.AddWrite(ROC_GET4_CMD_TO_GET4, GET4V1X_HIT_MASK             + 0x00000A );


   return conn.DoListSeq(nodeid, initList);
}

/*
 * GET4 v1.x (>1.2) initial configuration
 * int link = FLIB link number
 * int mode = readout mode to configure
 *            * 0 => default 24b mode settings
 *            * 1 => default 32b mode settings
 *            * 2 => tbd, read config from a text/xml file
 */
void config_get4v1x( int link, int mode, const uint32_t kRocId = 0x01, uint32_t uNbMess = 300  )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;

   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;
   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);
   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);
   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )

   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   printf("build time       = %10u \n", ret);
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");
   printf("Setting ROCID to = %d \n", kRocId);
   conn.Write( kNodeId, ROC_ROCID, kRocId );
   SetRocDef( conn, kNodeId );

   switch( mode )
   {
      case 0:
         Set24bDef( conn, kNodeId );
         break;
      case 1:
         Set32bDef( conn, kNodeId );
         break;
      case 2:
         std::cout<<" Configuration mode 2 not implemented yet ! "<<std::endl;
         break;
      default:
         std::cout<<" Unknown configuration mode "<<std::endl;
         break;
   } // switch( mode )


   // StartDAQ => Use the command list 2 in order to also to make a clean start
   conn.Write( kNodeId, ROC_CMD_LST_NR, 2);

   // Check link status
   conn.Read( kNodeId, ROC_OPTICS_LINK_STATUS, ret );
   printf("ROC_OPTICS_LINK_STATUS  = %d\n", ret);

   // Read 300 messages for quick cross check of setting
   ReadMessages( conn, kNodeId, uNbMess );

   // Close connection
   conn.Close();
}

void spiCmd_get4v1x( int link, uint32_t uSpiWord = 0xA5  )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;

   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;
   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);
   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);
   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )

   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");

   printf("Sending SPI command through GET4 => %d \n", uSpiWord);
   SendSpi( conn, kNodeId, 0, 0x429, 1, &uSpiWord);

   // Close connection
   conn.Close();
}

void spiPadi8All_get4v1x( int link, uint32_t uFeeIdx = 0,
                          uint32_t uDacValChip0 = 0x1C0,
                          uint32_t uDacValChip1 = 0x1C0,
                          uint32_t uDacValChip2 = 0x1C0,
                          uint32_t uDacValChip3 = 0x1C0  )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;

   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;
   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);
   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);
   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )

   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");

   const int32_t kuNbPadiPerGet4 = 4;
   uint32_t uSpiWords[kuNbPadiPerGet4];

   // The GET4 connected to the SPI connector is the #4 on each FEE-GET4
   uint32_t uGet4Idx   = 4 + 8*uFeeIdx;

   // # SPI Interface : 16 Bit, 20 MBit/s, read OFF, CPHA 0, CPOL 1
   uint32_t uSpiConfig = 0x429;

   uint32_t uWriteBit  = 0x1<<23;
   uint32_t uMaskAllCh = 0x1<<21;
   uSpiWords[0]=  uWriteBit + uMaskAllCh + ( (uDacValChip0 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskAllCh + ( (uDacValChip1 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskAllCh + ( (uDacValChip2 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskAllCh + ( (uDacValChip3 & 0x3FF) << 8);

   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Close connection
   conn.Close();
}

void spiPadi8_get4v1x( int link, uint32_t uFeeIdx = 0,
        uint32_t uPa0Ch0 = 0x1C0, uint32_t uPa1Ch0 = 0x1C0, uint32_t uPa2Ch0 = 0x1C0, uint32_t uPa3Ch0 = 0x1C0,
        uint32_t uPa0Ch1 = 0x1C0, uint32_t uPa1Ch1 = 0x1C0, uint32_t uPa2Ch1 = 0x1C0, uint32_t uPa3Ch1 = 0x1C0,
        uint32_t uPa0Ch2 = 0x1C0, uint32_t uPa1Ch2 = 0x1C0, uint32_t uPa2Ch2 = 0x1C0, uint32_t uPa3Ch2 = 0x1C0,
        uint32_t uPa0Ch3 = 0x1C0, uint32_t uPa1Ch3 = 0x1C0, uint32_t uPa2Ch3 = 0x1C0, uint32_t uPa3Ch3 = 0x1C0,
        uint32_t uPa0Ch4 = 0x1C0, uint32_t uPa1Ch4 = 0x1C0, uint32_t uPa2Ch4 = 0x1C0, uint32_t uPa3Ch4 = 0x1C0,
        uint32_t uPa0Ch5 = 0x1C0, uint32_t uPa1Ch5 = 0x1C0, uint32_t uPa2Ch5 = 0x1C0, uint32_t uPa3Ch5 = 0x1C0,
        uint32_t uPa0Ch6 = 0x1C0, uint32_t uPa1Ch6 = 0x1C0, uint32_t uPa2Ch6 = 0x1C0, uint32_t uPa3Ch6 = 0x1C0,
        uint32_t uPa0Ch7 = 0x1C0, uint32_t uPa1Ch7 = 0x1C0, uint32_t uPa2Ch7 = 0x1C0, uint32_t uPa3Ch7 = 0x1C0
        )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;
   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;

   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);

   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);

   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )
   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");

   const int32_t kuNbPadiPerGet4 = 4; // 4 chip/board w/ 8 ch each
   uint32_t uSpiWords[kuNbPadiPerGet4];

   // The GET4 connected to the SPI connector is the #4 on each FEE-GET4
   uint32_t uGet4Idx   = 4 + 8*uFeeIdx;

   // # SPI Interface : 16 Bit, 20 MBit/s, read OFF, CPHA 0, CPOL 1
   uint32_t uSpiConfig = 0x429;

   // Enable the writing in PADI SPI interface
   uint32_t uWriteBit  = 0x1<<23;

   // Set channel 0 of all chips
   uint32_t uMaskCh0 = 0x0 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh0 + ( (uPa0Ch0 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh0 + ( (uPa1Ch0 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh0 + ( (uPa2Ch0 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh0 + ( (uPa3Ch0 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 1 of all chips
   uint32_t uMaskCh1 = 0x1 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh1 + ( (uPa0Ch1 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh1 + ( (uPa1Ch1 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh1 + ( (uPa2Ch1 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh1 + ( (uPa3Ch1 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 2 of all chips
   uint32_t uMaskCh2 = 0x2 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh2 + ( (uPa0Ch2 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh2 + ( (uPa1Ch2 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh2 + ( (uPa2Ch2 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh2 + ( (uPa3Ch2 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 3 of all chips
   uint32_t uMaskCh3 = 0x3 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh3 + ( (uPa0Ch3 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh3 + ( (uPa1Ch3 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh3 + ( (uPa2Ch3 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh3 + ( (uPa3Ch3 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 4 of all chips
   uint32_t uMaskCh4 = 0x4 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh4 + ( (uPa0Ch4 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh4 + ( (uPa1Ch4 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh4 + ( (uPa2Ch4 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh4 + ( (uPa3Ch4 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 5 of all chips
   uint32_t uMaskCh5 = 0x5 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh5 + ( (uPa0Ch5 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh5 + ( (uPa1Ch5 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh5 + ( (uPa2Ch5 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh5 + ( (uPa3Ch5 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 6 of all chips
   uint32_t uMaskCh6 = 0x6 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh6 + ( (uPa0Ch6 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh6 + ( (uPa1Ch6 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh6 + ( (uPa2Ch6 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh6 + ( (uPa3Ch6 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 7 of all chips
   uint32_t uMaskCh7 = 0x7 << 18;
   uSpiWords[0]=  uWriteBit + uMaskCh7 + ( (uPa0Ch7 & 0x3FF) << 8);
   uSpiWords[1]=  uWriteBit + uMaskCh7 + ( (uPa1Ch7 & 0x3FF) << 8);
   uSpiWords[2]=  uWriteBit + uMaskCh7 + ( (uPa2Ch7 & 0x3FF) << 8);
   uSpiWords[3]=  uWriteBit + uMaskCh7 + ( (uPa3Ch7 & 0x3FF) << 8);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Close connection
   conn.Close();
}

void spiPadi6All_get4v1x( int link, uint32_t uFeeIdx = 0, uint32_t uPa0Ch0 = 0x110)
{
   spiPadi6_get4v1x( link, uFeeIdx,
         uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0,
         uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0,
         uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0,
         uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0, uPa0Ch0
           );
}

void spiPadi6_get4v1x( int link, uint32_t uFeeIdx = 0,
        uint32_t uPa0Ch0 = 0x110, uint32_t uPa0Ch1 = 0x110, uint32_t uPa0Ch2 = 0x110, uint32_t uPa0Ch3 = 0x110,
        uint32_t uPa1Ch0 = 0x110, uint32_t uPa1Ch1 = 0x110, uint32_t uPa1Ch2 = 0x110, uint32_t uPa1Ch3 = 0x110,
        uint32_t uPa2Ch0 = 0x110, uint32_t uPa2Ch1 = 0x110, uint32_t uPa2Ch2 = 0x110, uint32_t uPa2Ch3 = 0x110,
        uint32_t uPa3Ch0 = 0x110, uint32_t uPa3Ch1 = 0x110, uint32_t uPa3Ch2 = 0x110, uint32_t uPa3Ch3 = 0x110,
        uint32_t uPa4Ch0 = 0x110, uint32_t uPa4Ch1 = 0x110, uint32_t uPa4Ch2 = 0x110, uint32_t uPa4Ch3 = 0x110,
        uint32_t uPa5Ch0 = 0x110, uint32_t uPa5Ch1 = 0x110, uint32_t uPa5Ch2 = 0x110, uint32_t uPa5Ch3 = 0x110,
        uint32_t uPa6Ch0 = 0x110, uint32_t uPa6Ch1 = 0x110, uint32_t uPa6Ch2 = 0x110, uint32_t uPa6Ch3 = 0x110,
        uint32_t uPa7Ch0 = 0x110, uint32_t uPa7Ch1 = 0x110, uint32_t uPa7Ch2 = 0x110, uint32_t uPa7Ch3 = 0x110
        )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;

   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;
   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);
   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);
   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )

   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");

   const int32_t kuNbPadiPerGet4 = 8; // 8 chip/board w/ 4 ch each
   uint32_t uSpiWords[kuNbPadiPerGet4];

   // The GET4 connected to the SPI connector is the #4 on each FEE-GET4
   uint32_t uGet4Idx   = 4 + 8*uFeeIdx;

   // # SPI Interface : 14 Bit, 20 MBit/s, read OFF, CPHA 0, CPOL 1
   uint32_t uSpiConfig = 0x3A9;

   // Enable the writing in PADI SPI interface
   uint32_t uWriteBit  = 0x4 << 20;

   // Set channel 0 of all chips
   uint32_t uMaskCh0 = 0x0 << 20;
   uSpiWords[0]=  uWriteBit + uMaskCh0 + ( (uPa0Ch0 & 0x3FF) << 10);
   printf("test     = %8x \n", uSpiWords[0]);
   uSpiWords[1]=  uWriteBit + uMaskCh0 + ( (uPa1Ch0 & 0x3FF) << 10);
   uSpiWords[2]=  uWriteBit + uMaskCh0 + ( (uPa2Ch0 & 0x3FF) << 10);
   uSpiWords[3]=  uWriteBit + uMaskCh0 + ( (uPa3Ch0 & 0x3FF) << 10);
   uSpiWords[4]=  uWriteBit + uMaskCh0 + ( (uPa4Ch0 & 0x3FF) << 10);
   uSpiWords[5]=  uWriteBit + uMaskCh0 + ( (uPa5Ch0 & 0x3FF) << 10);
   uSpiWords[6]=  uWriteBit + uMaskCh0 + ( (uPa6Ch0 & 0x3FF) << 10);
   uSpiWords[7]=  uWriteBit + uMaskCh0 + ( (uPa7Ch0 & 0x3FF) << 10);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 1 of all chips
   uint32_t uMaskCh1 = 0x1 << 20;
   uSpiWords[0]=  uWriteBit + uMaskCh1 + ( (uPa0Ch1 & 0x3FF) << 10);
   uSpiWords[1]=  uWriteBit + uMaskCh1 + ( (uPa1Ch1 & 0x3FF) << 10);
   uSpiWords[2]=  uWriteBit + uMaskCh1 + ( (uPa2Ch1 & 0x3FF) << 10);
   uSpiWords[3]=  uWriteBit + uMaskCh1 + ( (uPa3Ch1 & 0x3FF) << 10);
   uSpiWords[4]=  uWriteBit + uMaskCh1 + ( (uPa4Ch1 & 0x3FF) << 10);
   uSpiWords[5]=  uWriteBit + uMaskCh1 + ( (uPa5Ch1 & 0x3FF) << 10);
   uSpiWords[6]=  uWriteBit + uMaskCh1 + ( (uPa6Ch1 & 0x3FF) << 10);
   uSpiWords[7]=  uWriteBit + uMaskCh1 + ( (uPa7Ch1 & 0x3FF) << 10);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 2 of all chips
   uint32_t uMaskCh2 = 0x2 << 20;
   uSpiWords[0]=  uWriteBit + uMaskCh2 + ( (uPa0Ch2 & 0x3FF) << 10);
   uSpiWords[1]=  uWriteBit + uMaskCh2 + ( (uPa1Ch2 & 0x3FF) << 10);
   uSpiWords[2]=  uWriteBit + uMaskCh2 + ( (uPa2Ch2 & 0x3FF) << 10);
   uSpiWords[3]=  uWriteBit + uMaskCh2 + ( (uPa3Ch2 & 0x3FF) << 10);
   uSpiWords[4]=  uWriteBit + uMaskCh2 + ( (uPa4Ch2 & 0x3FF) << 10);
   uSpiWords[5]=  uWriteBit + uMaskCh2 + ( (uPa5Ch2 & 0x3FF) << 10);
   uSpiWords[6]=  uWriteBit + uMaskCh2 + ( (uPa6Ch2 & 0x3FF) << 10);
   uSpiWords[7]=  uWriteBit + uMaskCh2 + ( (uPa7Ch2 & 0x3FF) << 10);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Set channel 3 of all chips
   uint32_t uMaskCh3 = 0x3 << 20;
   uSpiWords[0]=  uWriteBit + uMaskCh3 + ( (uPa0Ch3 & 0x3FF) << 10);
   uSpiWords[1]=  uWriteBit + uMaskCh3 + ( (uPa1Ch3 & 0x3FF) << 10);
   uSpiWords[2]=  uWriteBit + uMaskCh3 + ( (uPa2Ch3 & 0x3FF) << 10);
   uSpiWords[3]=  uWriteBit + uMaskCh3 + ( (uPa3Ch3 & 0x3FF) << 10);
   uSpiWords[4]=  uWriteBit + uMaskCh3 + ( (uPa4Ch3 & 0x3FF) << 10);
   uSpiWords[5]=  uWriteBit + uMaskCh3 + ( (uPa5Ch3 & 0x3FF) << 10);
   uSpiWords[6]=  uWriteBit + uMaskCh3 + ( (uPa6Ch3 & 0x3FF) << 10);
   uSpiWords[7]=  uWriteBit + uMaskCh3 + ( (uPa7Ch3 & 0x3FF) << 10);
   SendSpi( conn, kNodeId, uGet4Idx, uSpiConfig, kuNbPadiPerGet4, uSpiWords);

   // Close connection
   conn.Close();
}

void dump_get4v1x( int link, int mode, uint32_t uNbMess = 300  )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;

   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;
   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);
   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);
   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )

   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");

   ReadMessages( conn, kNodeId, uNbMess);

   // Close connection
   conn.Close();
}


/*
 * GET4 v1.x (>1.2) + ROC + FLIB: direct readout of ROC messages
 * from ROC buffer through the registers interface
 * Mostly copied/adapted from the RocGui function DataWidget::getData()
 */
int ReadMessages(CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t nbMess = 1)
{

   // Register operations list
   CbmNet::ListSeq messList;
   // rocutil> ssm 2
   messList.AddRead(ROC_ROCID);
   messList.AddRead(ROC_BURST1);
   messList.AddRead(ROC_BURST2);
   messList.AddRead(ROC_BURST3);

   // Message printout variables
   get4v1x::Message msg1, msg2;
   msg1.setRocNumber(0);
   msg2.setRocNumber(0);

   std::ostringstream ostr;

   uint64_t value1, value2, value3, message1, message2;
   uint64_t rocid;
   uint64_t data;

   int nSuccTot = 0;
   for( int iMess = 0; iMess <= nbMess/2; iMess++ )
   {
      int nSucc = conn.DoListSeq(nodeid, messList);

      if( nSucc != messList.Size() ) {
         printf("Error in ReadMessages: errors during r/w SysCore registers. Number of successfully executed commands is %d out of %d\n",
               nSucc, messList.Size() );
         return -1;
      } // if( nSucc != list.Size() )

      rocid = messList[0].value;
      rocid = (rocid&0x000000000000FFFF)<<48;


      value1 = messList[1].value;
      value2 = messList[2].value;
      value3 = messList[3].value;

      message1 = rocid | value1<<16 | (value2&0xffff0000)>>16;
      message2 = rocid | (value2&0xffff)<<32 | value3;

      data =  rocid
            + ( ( (message1      ) & 0xFF) << 40)
            + ( ( (message1 >>  8) & 0xFF) << 32)
            + ( ( (message1 >> 16) & 0xFF) << 24)
            + ( ( (message1 >> 24) & 0xFF) << 16)
            + ( ( (message1 >> 32) & 0xFF) <<  8)
            + ( ( (message1 >> 40) & 0xFF));
      msg1.setData(data);

      data =  rocid
            + ( ( (message2      ) & 0xFF) << 40)
            + ( ( (message2 >>  8) & 0xFF) << 32)
            + ( ( (message2 >> 16) & 0xFF) << 24)
            + ( ( (message2 >> 24) & 0xFF) << 16)
            + ( ( (message2 >> 32) & 0xFF) <<  8)
            + ( ( (message2 >> 40) & 0xFF)); 
      msg2.setData(data);

      msg1.printDataCout( get4v1x::msg_print_Hex | get4v1x::msg_print_Prefix | get4v1x::msg_print_Data );
      msg2.printDataCout( get4v1x::msg_print_Hex | get4v1x::msg_print_Prefix | get4v1x::msg_print_Data );

/*
      printf("Message on ROC %04X %02X:%02X:%02X:%02X %02X:%02X:%02X:%02X %02X:%02X:%02X:%02X\n",
             nodeid,
             ((messList[3].value) >> 24)& 0xFF, (messList[3].value >> 16) & 0xFF,
             ((messList[3].value) >>  8)& 0xFF, (messList[3].value      ) & 0xFF,
             ((messList[2].value) >> 24)& 0xFF, (messList[2].value >> 16) & 0xFF,
             ((messList[2].value) >>  8)& 0xFF, (messList[2].value      ) & 0xFF,
             ((messList[1].value) >> 24)& 0xFF, (messList[1].value >> 16) & 0xFF,
             ((messList[1].value) >>  8)& 0xFF, (messList[1].value      ) & 0xFF);
*/
      nSuccTot += nSucc;
   } // for( int iMess = 0; iMess < ; iMess++ )

   return nSuccTot;
}

void lnktest_get4v1x( int link, int mode, uint32_t uNbMess = 30  )
{
   // Custom settings:
   int FlibLink = link;
   const uint32_t kNodeId = 0;

   CbmNet::ControlClient conn;
   ostringstream dpath;

   dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
   conn.Connect(dpath.str());

   // Check board and firmware info
   uint32_t ret;
   conn.Read( kNodeId, ROC_TYPE, ret );
   printf("Firmware type    = FE %5d TS %5d\n", (ret>>16)& 0xFFFF,  (ret)& 0xFFFF);
   conn.Read( kNodeId, ROC_HWV, ret );
   printf("Firmware Version = %10d \n", ret);
   conn.Read( kNodeId, ROC_FPGA_TYPE, ret );
   printf("FPGA type        = %10d \n", ret);
   conn.Read( kNodeId, ROC_SVN_REVISION, ret );
   printf("svn revision     = %10d \n", ret);
   if(0 == ret )
   {
      printf("Invalid svn revision, link or ROC is probably inactive, stopping there\n");
      return;
   } // if(0 == ret )

   conn.Read( kNodeId, ROC_BUILD_TIME, ret );
   time_t rawtime = ret;
   struct tm * timeinfo;
   char buffer [20];
   timeinfo = localtime( &rawtime);
   strftime( buffer,20,"%F %T",timeinfo);
   printf("build time       = %s \n", buffer);
   printf("\n");

   printf("\nTry speed     =  19.XX MBit/s \n");
// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to  19.XX MBit/s
   conn.Write(kNodeId, ROC_GET4_RECEIVE_CLK_CFG,           0);
   // StartDAQ => Use the command list 2 in order to also to make a clean start
   conn.Write( kNodeId, ROC_CMD_LST_NR, 2);
   ReadMessages( conn, kNodeId, uNbMess);

   printf("\nTry speed     =  39.XX MBit/s \n");
// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to  39.XX MBit/s
   conn.Write(kNodeId, ROC_GET4_RECEIVE_CLK_CFG,           1);
   // StartDAQ => Use the command list 2 in order to also to make a clean start
   conn.Write( kNodeId, ROC_CMD_LST_NR, 2);
   ReadMessages( conn, kNodeId, uNbMess);

   printf("\nTry speed     =  78.XX MBit/s \n");
// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to  78.XX MBit/s
   conn.Write(kNodeId, ROC_GET4_RECEIVE_CLK_CFG,           2);
   // StartDAQ => Use the command list 2 in order to also to make a clean start
   conn.Write( kNodeId, ROC_CMD_LST_NR, 2);
   ReadMessages( conn, kNodeId, uNbMess);

   printf("\nTry speed     = 156.XX MBit/s \n");
// ROC_GET4_RECEIVE_CLK_CFG => Set link speed to 156.XX MBit/s
   conn.Write(kNodeId, ROC_GET4_RECEIVE_CLK_CFG,           3);
   // StartDAQ => Use the command list 2 in order to also to make a clean start
   conn.Write( kNodeId, ROC_CMD_LST_NR, 2);
   ReadMessages( conn, kNodeId, uNbMess);

   // Close connection
   conn.Close();
}




