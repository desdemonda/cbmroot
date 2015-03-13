#ifndef NXYTER_DATADEBUG_H
#define NXYTER_DATADEBUG_H

#include <stdint.h>
#include <iostream>

namespace nxyter {

   class DataDebug
   {
      protected:
         uint32_t fAdc;                        //!< data from ROC_ADC_DATA
         uint32_t fNx;                         //!< data from ROC_NX_DATA
         uint32_t fLtsLow;                     //!< data from ROC_LT_LOW
         uint32_t fLtsHigh;                    //!< data from ROC_LT_HIGH

      public:
         //! Default constructor, initialize all fields.
         explicit DataDebug(uint32_t adc=0, uint32_t nx=0,
               uint32_t ltslow=0, uint32_t ltshigh=0) :
               fAdc(adc), fNx(nx), fLtsLow(ltslow), fLtsHigh(ltshigh)
               {}

         //! Returns raw #ROC_ADC_DATA data.
         uint32_t getRawAdc() const
               { return fAdc; }
         //! Returns raw #ROC_NX_DATA data.
         uint32_t getRawNx() const
               { return fNx; }
         //! Returns raw #ROC_LT_LOW data.
         uint32_t getRawLtsLow() const
               { return fLtsLow; }
         //! Returns raw #ROC_LT_HIGH data.
         uint32_t getRawLtsHigh() const
               { return fLtsHigh; }

         //! Returns full 64bit ROC local timestamp (unit=4ns 250 MHz clock).
         uint64_t getRawLts() const
               { return (uint64_t(fLtsHigh)<<32) + uint64_t(fLtsLow); }

         //! Returns raw message type.
         uint32_t getRawMessageType() const
               { return (fAdc>>13) & 0x7; }

         //! Returns nXYTER ADC value.
         uint32_t getNxAdcValue() const
               { return fAdc & 0xfff; }

         //! Returns nXYTER number.
         uint32_t getNxNumber() const
               { return (fNx>>3) & 0x3; }

         //! Returns raw (gray encoded) nXYTER channel number.
         uint32_t getRawNxChNum() const
               { return (fNx>>8) & 0x7f; }

         //! Returns ungrayed nXYTER channel number.
         uint32_t getNxChNum() const
               { return ungray(getRawNxChNum(), 7); }

         //! Returns raw (gray encoded) nXYTER timestamp (unit=1ns).
         uint32_t getRawNxTs() const
               { return ((fNx & 0x7f000000)>>17) + ((fNx & 0x007f0000)>>16); }

         //! Returns ungrayed nXYTER timestamp.
         uint32_t getNxTs() const
               { return ungray(getRawNxTs()^(0x3fff), 14); }

         //! Returns nXYTER parity bit as send from chip. \sa getNxParityCheck()
         uint32_t getNxParity() const
               { return fNx & 0x1; }

         uint32_t getNxParityCheck() const;

         void print(std::ostream& os) const;

         static uint32_t ungray(uint32_t val, int n);
   };

}

#endif
