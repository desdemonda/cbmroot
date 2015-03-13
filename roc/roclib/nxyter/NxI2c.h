//============================================================================
/*! \file NxI2c.h
 *  \author W.F.J.Mueller/GSI, based on earlier work of Norbert Abel/KIP
 */
//============================================================================

#ifndef NXYTER_NXI2C_H
#define NXYTER_NXI2C_H

#include "roc/I2cDevice.h"
#include "nxyter/NxContext.h"

namespace nxyter {

   //! Names for nXYTER I2C Registers
   // they are directly in nxyter name space for ease of usage !!
   enum nxRegs {
      kNxRegMask00  =  0,
      kNxRegMask01  =  1,
      kNxRegMask02  =  2,
      kNxRegMask03  =  3,
      kNxRegMask04  =  4,
      kNxRegMask05  =  5,
      kNxRegMask06  =  6,
      kNxRegMask07  =  7,
      kNxRegMask08  =  8,
      kNxRegMask09  =  9,
      kNxRegMask10  = 10,
      kNxRegMask11  = 11,
      kNxRegMask12  = 12,
      kNxRegMask13  = 13,
      kNxRegMask14  = 14,
      kNxRegMask15  = 15,
      kNxRegVcg     = 16,
      kNxRegIcgfoll = 17,
      kNxRegVth     = 18,
      kNxRegVbfb    = 19,
      kNxRegVbiasF  = 20,
      kNxRegVbiasS  = 21,
      kNxRegVbiasS2 = 22,
      kNxRegVcm     = 23,
      kNxRegcal     = 24,
      kNxRegiCOMP   = 25,
      kNxRegiDUR    = 26,
      kNxRegiINV    = 27,
      kNxRegiPDH    = 28,
      kNxRegiTWC    = 29,
      kNxRegSpare30 = 30,
      kNxRegSpare31 = 31,
      kNxRegConfig0 = 32,
      kNxRegConfig1 = 33,
      kNxRegOverflowLSB   = 34,
      kNxRegOverflowMSB   = 35,
      kNxRegMissTokenLSB  = 36,
      kNxRegMissTokenMSB  = 37,
      kNxRegdelayTestPuls = 38,
      kNxRegdelayTestTrig = 39,
      kNxRegSpare40 = 40,
      kNxRegSpare41 = 41,
      kNxRegTrimDAQPower  = 42,
      kNxRegdelayClock1   = 43,
      kNxRegdelayClock2   = 44,
      kNxRegdelayClock3   = 45
   };

   static const uint8_t kNxC0TestPulsEnable    = 1<<0;
   static const uint8_t kNxC0TestPulsSync      = 1<<1;
   static const uint8_t kNxC0TestPulsPolarity  = 1<<2;
   static const uint8_t kNxC0TestTrigEnable    = 1<<3;
   static const uint8_t kNxC0TestTrigSync      = 1<<4;
   static const uint8_t kNxC0Disable32MHz      = 1<<5;
   static const uint8_t kNxC0Disable128MHz     = 1<<6;
   static const uint8_t kNxC0TsLsbClockSelect  = 1<<7;
   static const uint8_t kNxC1CalibSelectMask   = 0x3;
   static const uint8_t kNxC1FrontEndPolarity  = 1<<2;
   static const uint8_t kNxC1ReadClockSelect   = 1<<3;

   class NxI2c : public roc::I2cDevice {
      public:

         explicit NxI2c(roc::Board* board, int port=0, uint8_t addr=0);
         virtual ~NxI2c();

         int setContext(NxContext& cntx, int domask=nxyter::kDoAll,
                        bool veri=true);
         int getContext(NxContext& cntx, int domask=nxyter::kDoAll);

         int getCounters(uint16_t& overflow, uint16_t& tokenmiss);
         int setTestModes(bool testpuls, bool testtrig, int calselect); 

         int probe();
         int setToDefault(bool ispos=false, 
                          int maskon=128, int poweron=128);
         void printRegisters(std::ostream& os, int domask=nxyter::kDoAll);

         int setRegMask(const uint8_t *val);
         int setRegCore(const uint8_t *val);
         int setRegTrim(const uint8_t *val);
         int getRegMask(uint8_t *val);
         int getRegCore(uint8_t *val);
         int getRegTrim(uint8_t *val);

         static const char* registerName(int reg);
         static const char* configurationBitName(int bit);

         static uint8_t delayToSetting(uint8_t delay);
         static uint8_t settingToDelay(uint8_t val);

   };

}

//! ostream insertion for nxyter::NxI2c
/*!
 * Just calls nxyter::NxI2c::printRegisters()
 * \relates nxyter::NxI2c
 */
inline std::ostream& operator<<(std::ostream& os, nxyter::NxI2c& obj)
  { obj.printRegisters(os); return os; }

#endif
