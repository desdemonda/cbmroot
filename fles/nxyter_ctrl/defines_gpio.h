#ifndef BASE_DEFINES_GPIO_H
#define BASE_DEFINES_GPIO_H

namespace base {

   enum GpioRegisters {

      /*!
       * {ROC, read/write, 32 bit}
       * Part of \ref roc_iface_syncslave and \ref roc_iface_aux.
       */
       GPIO_CONFIG =            0x010000,    // r/w


       /*!
        * {ROC, read/write, 8 bit} This 8 bit register is implemented but has no
        * hardware function. It does despite its name <em>not</em> control any
        * sync master transmitter timing, since it is only used for receivers.
        */
       GPIO_SYNCM_BAUD_START =          0x010100,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Controls for sync master how many 250 MHz clock
        * cycles the serial transmitter waits after an odd numbered bit.
        * The register contains cycles minus 1, so 4 cycles --> register setting 3
        *
        * Part of \ref roc_iface_syncmaster.
        */
       GPIO_SYNCM_BAUD1      =       0x010104,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Controls for sync master how many 250 MHz clock
        * cycles the serial transmitter waits after an even numbered bit.
        * The register contains cycles minus 1, so 4 cycles --> register setting 3
        *
        * Part of \ref roc_iface_syncmaster.
        */
       GPIO_SYNCM_BAUD2      =       0x010108,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Determines the rate of sending sync messages out of
        * the <em>sync message sender</em> GPIO port. The scale down is settable in
        * powers of two, a value of 0 causes a sync message for each epoch, a value
        * of 1,2,3,... for each 2nd, 4th, 8th,... epoch. Even though the register
        * is 8 bit, only the values 0 to 23 are valid.
        *
        * Part of \ref roc_iface_syncmaster.
        */
       GPIO_SYNCM_SCALEDOWN   =       0x01010C,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Controls how many 250 MHz clock cycles the serial
        * receiver waits after the first falling edge (of the start bit) to sample
        * in the middle of the bit cell. Use max(2, 250MHz/(baudrate/2)).
        * The register contains cycles minus 1, so 4 cycles --> register setting 3
        *
        * Part of \ref roc_iface_syncslave.
        */
       GPIO_SYNCS0_BAUD_START  =       0x010200,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Controls for sync slave channel 0 how many 250 MHz
        * clock cycles the serial receiver waits after an odd numbered bit.
        * Use 250MHz/baudrate.
        * The register contains cycles minus 1, so 4 cycles --> register setting 3
        *
        * Part of \ref roc_iface_syncslave.
        */
       GPIO_SYNCS0_BAUD1       =       0x010204,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Controls for sync slave channel 0 how many 250 MHz
        * clock cycles the serial receiver waits after an even numbered bit.
        * Use 250MHz/baudrate.
        * The register contains cycles minus 1, so 4 cycles --> register setting 3
        *
        * Part of \ref roc_iface_syncslave.
        */
       GPIO_SYNCS0_BAUD2    =          0x010208,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Like #ROC_SYNC2_BAUD_START for sync slave channel 1.
        * Part of \ref roc_iface_syncslave.
        */
       GPIO_SYNCS1_BAUD_START =        0x010300,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Like #ROC_SYNC2_BAUD1 for sync slave channel 1.
        * Part of \ref roc_iface_syncslave.
        */
       GPIO_SYNCS1_BAUD1  =           0x010304,    // r/w


       /*!
        * {ROC, read/write, 8 bit} Like #ROC_SYNC2_BAUD2 for sync slave channel 1.
        * Part of \ref roc_iface_syncslave.
        */
       GPIO_SYNCS1_BAUD2  =           0x010308    // r/w
   };


   enum GpioChannels {
       SYNC_M = 1,
       SYNC_S0 = 2,
       SYNC_S1 = 3,
       AUX0    = 4,
       AUX1    = 5,
       AUX2    = 6,
       AUX3    = 7,
       NumGpio = 7
   };

}

#endif
