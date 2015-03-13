//============================================================================
/*! \file NxContext.h
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#ifndef NXYTER_NXCONTEXT_H
#define NXYTER_NXCONTEXT_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>

namespace nxyter {

   // Note: the following defs are in nxyter namespace because rootcint
   //       does apparently not support static const defs in class scope.
   //
   static const int kDoMask = 1<<0; //!< handle mask part 
   static const int kDoCore = 1<<1; //!< handle core part 
   static const int kDoTrim = 1<<2; //!< handle trim part 
   static const int kDoAll  = kDoMask|kDoCore|kDoTrim; //<! handle all

   class NxContext {
      public:

      static const int dimRegMain =  46;    //!< to cover all I2C registers
      static const int dimRegTrim = 129;    //!< 128+1 locations of shift reg

      protected:
        uint8_t   fRegMain[dimRegMain];     //!< holds data for normal registers
        uint8_t   fRegTrim[dimRegTrim];         //!< holds register 42 data

      public:

         NxContext();
         virtual ~NxContext();

         void setRegister(int reg, uint8_t val);
         uint8_t getRegister(int reg) const;
         void setTrimRegister(int channel, uint8_t val);
         uint8_t getTrimRegister(int channel) const;

         void setChannelMaskBit(int channel, bool isoff);
         void setChannelMaskBit(int chbeg, int chend, bool isoff, 
                                int chstep=1);
         bool getChannelMaskBit(int channel) const;

         void setThresholdTrim(int channel, uint8_t val);
         void setThresholdTrim(int chbeg, int chend, uint8_t val,
                               int chstep=1);
         uint8_t getThresholdTrim(int channel) const;

         void setPowerOffMaskBit(int channel, bool isoff);
         void setPowerOffMaskBit(int chbeg, int chend, bool isoff,
                                 int chstep=1);
         bool getPowerOffMaskBit(int channel) const;

         void setToDefault(bool ispos=false, 
                           int maskon=128, int poweron=128);

         bool getConfigurationBit(int n);
         void setConfigurationBit(int n, bool on);

         bool equal(const NxContext& rhs, int domask=kDoAll) const;

         void print(std::ostream& os, int domask=kDoAll) const;

         //! Returns pointer to normal register data array 
         uint8_t* dataRegisterMain()
           { return fRegMain; }
         //! Returns pointer to trim-daq register (register 42) data array
         uint8_t* dataRegisterTrim()
           { return fRegTrim; }


         bool fillCmdFile(FILE* f, int nx);

         //! Compare equal operator. \sa equal()
         bool operator==(const NxContext& rhs) const
           { return equal(rhs); }
         //! Compare non-equal operator. \sa equal()
         bool operator!=(const NxContext& rhs) const
           { return !equal(rhs); }
   };

}

//! ostream insertion for nxyter::NxContext
/*!
 * Just calls nxyter::NxContext::print()
 * \relates nxyter::NxContext
 */
inline std::ostream& operator<<(std::ostream& os, const nxyter::NxContext& obj)
  { obj.print(os); return os; }

#endif
