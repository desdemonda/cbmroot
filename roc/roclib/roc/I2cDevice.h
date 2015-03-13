//============================================================================
/*! \file I2cDevice.h
 *  \author W.F.J.Mueller/GSI, based on earlier work of Norbert Abel/KIP
 */
//============================================================================

#ifndef ROC_I2CDEVICE_H
#define ROC_I2CDEVICE_H

#include "roc/Peripheral.h"

namespace roc {

   class I2cDevice : public roc::Peripheral {
      protected:
         int fPort;                         //!< I2C bus port number
         uint8_t fSlaveAddr;                //!< I2C device slave address
      public:

         I2cDevice();
         I2cDevice(roc::Board* board, int port=0, uint8_t addr=0);
         virtual ~I2cDevice();

         //! Set I2C bus port number on ROC
         void setPortNumber(int port)
            { fPort = port; }

         //! Returns I2C bus port number on ROC
         int getPortNumber() const
            { return fPort; }

         //! Set slave address of device on I2C bus
         void setSlaveAddr(uint8_t addr)
            { fSlaveAddr = addr; }

         //! Return slave address of device on I2C bus
         uint8_t getSlaveAddr() const
            { return fSlaveAddr; }

         int setRegister(uint8_t reg, uint8_t val, bool veri=false);
         int getRegister(uint8_t reg, uint8_t& val);
         int getRegister16(uint8_t reg, uint16_t& val);
         int setRegisterVerify(uint8_t reg, uint8_t valset, uint8_t& valget);

         int setRegisterArray(uint8_t reg, const uint8_t *val, int nreg,
                              bool veri=false);
         int getRegisterArray(uint8_t reg, uint8_t *val, int nreg);

         int setMailboxRegister(uint8_t reg, const uint8_t *val, int nval);
         int getMailboxRegister(uint8_t reg, uint8_t *val, int nval);

         static void addAddrMap(roc::Board* board);
   };

}

#endif
