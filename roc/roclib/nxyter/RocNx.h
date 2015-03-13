
#ifndef ROCNX_H
#define ROCNX_H

#include <vector>

#include "roc/Peripheral.h"
#include "nxyter/DataDebug.h"

namespace nxyter {


   class RocNx : public roc::Peripheral {
      public:

         explicit RocNx(roc::Board* board);
         virtual ~RocNx();

         int setToDefault();

         int resetRocNxTs();

         int fireTestPulse(uint32_t delay, uint32_t period, int32_t width,
                           uint32_t number);
         int fireTestPulse(uint32_t period, int32_t width,
                           uint32_t number);

         int setNxActive(int nx0, int nx1, int nx2, int nx3);
         int setNxActive(uint32_t mask);
         int getNxActive(uint32_t& mask);

         int setLTSDelay(uint32_t val);

         int setParityCheck(uint32_t val);
         int setDebugMode(uint32_t val);

         int getFifoFull(uint32_t& val);
         int getFifoEmpty(uint32_t& val);

         int getDataDebug(std::vector<nxyter::DataDebug>& datvec, int nmsg);

         static void addAddrMap(roc::Board* board);

   };

}

#endif
