#ifndef ROC_GPIO_H
#define ROC_GPIO_H

#include "roc/Peripheral.h"

namespace roc {

   class Gpio : public roc::Peripheral {
      protected:
      public:
         enum { NumGpio = 7 };

         static const char* name(int n);

         Gpio(roc::Board* board);
         virtual ~Gpio();

         int setSyncBaud(int gpio_nr, uint32_t ratediv);
         int setSyncBaud(int gpio_nr, uint32_t offset, 
                         uint32_t even, uint32_t odd);

         int getSyncBaud(int gpio_nr, uint32_t& ratediv);
         int getSyncBaud(int gpio_nr, uint32_t& offset, 
                         uint32_t& even, uint32_t& odd);

         int setSyncScale(uint32_t val);
         int getSyncScale(uint32_t& val);

         int getConfig(uint32_t& mask);
         int setConfig(uint32_t mask);
         int setConfig(int gpio_nr, bool riseedge, bool falledge,
                       bool throttled, bool extrafunc, 
                       bool altin);
         
         static bool packConfig(uint32_t& mask, int gpio_nr, 
                                bool riseedge, bool falledge, 
                                bool throttled, bool extrafunc, 
                                bool altin);
         static bool unpackConfig(uint32_t mask, int gpio_nr, 
                                  bool& riseedge, bool& falledge,
                                  bool& throttled, bool& extrafunc, 
                                  bool& altin);

         static void addAddrMap(roc::Board* board);
   };

}

#endif
