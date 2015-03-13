//============================================================================
/*! \file NxChip.h
 *  \author W.F.J.Mueller/GSI, minor adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_NXCHIP_H
#define NXYTER_NXCHIP_H

#include "roc/Peripheral.h"
#include "nxyter/NxI2c.h"

namespace nxyter {

   class NxChip : public roc::Peripheral {
      protected:
         nxyter::NxI2c  fI2c;               //!< I2C interface of Nx
         int            fNxNumber;          //!< the Nx number
  
      public:
         NxChip(roc::Board* board, int port, int addr, int nxnum);
         virtual ~NxChip();
         
         void setNxNumber(int nxnum);

         //! Returns the \ref glo_nxyter_nxnum. \sa setNxNumber()
         int  getNxNumber() { return fNxNumber; }

         int activate();
         int deactivate();

         bool isactive();

         int setChannelDelay(uint32_t val);
         int getChannelDelay(uint32_t& val);

         //! Gives access to the NxI2c object
         nxyter::NxI2c& i2c() 
           { return fI2c; }
   };

}

#endif
