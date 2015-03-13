//============================================================================
/*! \file MainAdc.h
 *  \author W.F.J.Mueller/GSI, minor adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_MAINADC_H
#define NXYTER_MAINADC_H

#include <iostream>

#include "roc/Peripheral.h"

namespace nxyter {

   class MainAdc : public roc::Peripheral {
      protected:
         int     fPort;                     //!< the port number
         bool    fMode4nx;                  //!< 4nx mode selector

      public:
         MainAdc();
         MainAdc(roc::Board* board, int port, bool mode4nx=false);
         virtual ~MainAdc();

         int setRegister(uint8_t reg, uint8_t val, bool veri=false);
         int getRegister(uint8_t reg, uint8_t& val);

         int setTestMode(int ch, uint8_t mode);
         int setUserPattern(uint16_t pat);

         int probe();
         int setToDefault();
         void printRegisters(std::ostream& os);

         int setChannelMux(int nxnum, uint32_t val);
         int setChannelLatency(int nxnum, uint32_t val);
         int getChannelLatency(int nxnum, uint32_t& val);

         static void calcClockDelayRegs(uint32_t delay, uint32_t& sr_init, uint32_t& bufg_select);
         static uint32_t findClockDelay(uint32_t sr_init, uint32_t bufg_select);

         int setClockDelay(uint32_t val);
         int setClockDelaySrInit(uint32_t val);
         int getClockDelaySrInit(uint32_t& val);
         int setClockDelayBufg(uint32_t val);
         int getClockDelayBufg(uint32_t& val);

         int getAdcDirect(int num, uint32_t& val);

         static const char* registerName(int reg);

         //! Names for ADC Registers
         enum adcRegs {
           kReg_chip_port_config =  0x00,
           kReg_chip_id          =  0x01,
           kReg_chip_grade       =  0x02,
           kReg_device_index_A   =  0x05,
           kReg_modes            =  0x08,
           kReg_clock            =  0x09,
           kReg_test_io          =  0x0d,
           kReg_output_mode      =  0x14,
           kReg_output_adjust    =  0x15,
           kReg_output_phase     =  0x16,
           kReg_user_patt1_lsb   =  0x19,
           kReg_user_patt1_msb   =  0x1a,
           kReg_user_patt2_lsb   =  0x1b,
           kReg_user_patt2_msb   =  0x1c,
           kReg_serial_control   =  0x21,
           kReg_serial_ch_stat   =  0x22,
           kReg_device_update    =  0xff
         };

   };

}

//! ostream insertion for nxyter::MainAdc
/*!
 * Just calls nxyter::MainAdc::printRegisters()
 * \relates nxyter::MainAdc
 */
inline std::ostream& operator<<(std::ostream& os, nxyter::MainAdc& obj)
  { obj.printRegisters(os); return os; }

#endif
