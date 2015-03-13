#ifndef DEFINES_GET4_V1_X_H
#define DEFINES_GET4_V1_X_H

/*****************************************************************************/
// Address 00000 – Initialisation Unit
/* Sub address 000 – Readout Initialisation => 0000 0000
 * This command sends an initialisation signal to the main readout state machine.
 */
#define GET4V1X_INIT_RO_INIT         0x00000000

/*
 * Sub address 001 – Readout Reset => 0000 0001
 * This command sends a reset signal of 16 clock cycles pulse width (102.4 ns)
 * to the readout system
 */
#define GET4V1X_INIT_RO_RST          0x01000000

/*
 * Sub address 011 – Readout and Configuration Reset => 0000 0011
 * This command sends a reset signal of 16 clock cycles pulse width (102.4 ns)
 * to the readout system and clears all configuration registers
 */
#define GET4V1X_INIT_ROCONF_RST      0x03000000

/*
 * Sub address 100 – DLL Reset => 0000 0100
 * This command sends a reset signal of 5500 clock cycles pulse width (35.2 μs)
 * to the DLL phase comparator.
 * Since version GET4 V1.20 the width of the reset signal is reduced to 160 clock
 * cycles (1.024 μs).
 */
#define GET4V1X_INIT_DLL_RST         0x04000000

/*
 * Sub address 111 – Readout and DLL Reset => 0000 0111
 * This command sends a reset to the readout system and the DLL phase comparator
 * and clears all configuration registers.
 * The pulse width is 16 clock cycles for the readout system and 5500 clock cycles
 * for the DLL phase comparator.
 * Since version GET4 V1.20 the width of the DLL reset signal is reduced to 160
 * clock cycles (1.024 μs).
 */
#define GET4V1X_INIT_RODLL_RST       0x07000000

/*
 * Sub address 101 – DLL Reset Phase => 0000 0101
 * The phase of the DLL reset release can be shifted by a 16 step phase shifter.
 * Depending on PVT variations the step size is in the order of 400 ps.
 * The default value after reset is a DLL reset release on the falling edge of the
 * clock signal.
 */
#define GET4V1X_INIT_DLL_RST_PH      0x05000000

/*
 * Sub address 110 – Auto DLL reset enable => 0000 0110
 * With this command the auto DLL reset mode can be enabled.
 * When the least significant data bit is set the auto DLL reset mode is enabled
 * otherwise it is disabled
 */
#define GET4V1X_INIT_DLL_RST_AUTO    0x06000000

/*****************************************************************************/
// Address 00001 – Epoch Synchronisation
/*
 * Sub address 000 – Epoch Initialisation => 0000 1000
 * If the sub address is 000, this command sends a 24 bit epoch number to the GET4
 * TDC. The epoch number is loaded into the epoch counter on the next external sync
 * signal after this slow control command.
 */
#define GET4V1X_EP_SYNC_INIT         0x08000000

/*
 * Sub address 001 – Epoch Synchronisation => 0000 1001
 * If the sub address is 001, 264 clock cycles after rising clock edge succeeding
 * the falling edge invoking the start bit of this slow control command the timestamp
 * counter is set to zero.
 */
#define GET4V1X_EP_SYNC_RST          0x09000000

/*****************************************************************************/
// Address 00010 – Diagnostics Select
/*
 * The sub address entry is not encoded. => 0001 0000
 * The least significant four bits are written into the diagnostics select register.
 * The Bits 6 and 5 are written into the channel select register that defines which
 * channel is used for the FIFO write and FIFO read diagnostics function.
 * The selected edge is defined by Bit 4. A logic 1 corresponds to the trailing edge,
 * a logic 0 corresponds to the leading edge.
 *  cf GET4 > v1.20 manuals
 */
#define GET4V1X_DIAG_SELECT          0x10000000

/*****************************************************************************/
// Address 00011 – Hit Masking
/*
 * The sub address entry is not encoded. => 0001 1000
 * The least significant four bits are written into the hit mask register.
 * Bit 0 in the hit mask register is corresponding with input HIT1 and Bit 3 with
 * input HIT4. A set bit in the hit mask register inhibits the corresponding input so the
 * input is decoupled from the TDC.
 * After reset all hit masking bits are cleared so all inputs are enabled.
 */
#define GET4V1X_HIT_MASK             0x18000000

/*****************************************************************************/
// Address 00100 – Readout Configuration
/*
 * Sub address 000 – Basic readout configuration => 0010 0000
 * Bit |   3   2 |    1    |   0
 * Use |  unused | dLostEv | En32/24
 * Depending on the En32/24 Flag the 32 bit readout or the 24 bit readout is enabled.
 * If the flag is set (log. 1) the 32 bit readout is enabled.
 * The LostEv-Flag enables the LostEventError generation.
 * After reset all configuration bits are cleared
 */
#define GET4V1X_RO_CONF_BASIC        0x20000000

/*
 * Sub address 001 – Bit error rate testing mode => 0010 0001
 * Bit |   3    |  2 1 0
 * Use | Enable | Bit rate
 * cf GET4 > v1.20 manuals
 */
#define GET4V1X_RO_CONF_BIT_ERR_TEST 0x21000000

/*****************************************************************************/
// Address 00101 – 24 bit Readout Configuration
/*
 * Only subaddress 000 => 0010 1000
 * Three least significant bits encode the readout link speed.
 * cf GET4 > v1.20 manuals
 * Default value is 00 or 000 depending on the version
 */
#define GET4V1X_24B_RO_CONF          0x28000000

/*****************************************************************************/
// Address 00110 – 32 bit Readout Configuration
/*
 * Sub address 000 – Time over Threshold Configuration => 0011 0000
 * Bit | 3  |     2    |   1    0
 * Use | DI | AutoHeal | ToT resolution
 * DI = DiscardImmediate Flag
 * cf GET4 > v1.20 manuals
 */
#define GET4V1X_32B_RO_CONF_TOT      0x30000000

/*
 * Sub address 001 – Time over Threshold Range => 0011 0001
 * cf GET4 > v1.20 manuals
 */
#define GET4V1X_32B_RO_CONF_TOT_MAX  0x31000000

/*
 * Sub address 010 – Bit Rate => 0011 0010
 * Bit | 3  | 2    1    0
 * Use | DE | bit rate
 * DE = DE flag the double serial link readout enable
 * The rates are the same as for the Bit error rate.
 * cf GET4 > v1.20 manuals
 */
#define GET4V1X_32B_RO_CONF_LNK_RATE 0x32000000

/*****************************************************************************/
// Address 00111 – Test Pattern Generator
/*
 * Sub address 000 – Pattern Register Channel 0 => 0011 1000
 * Sub address 001 – Pattern Register Channel 1 => 0011 1001
 * Sub address 002 – Pattern Register Channel 2 => 0011 1010
 * Sub address 003 – Pattern Register Channel 3 => 0011 1011
 * This slow control command writes the 24 bit pattern register of channel n
 */
#define GET4V1X_TEST_PATT_GEN_CH0    0x38000000
#define GET4V1X_TEST_PATT_GEN_CH0    0x39000000
#define GET4V1X_TEST_PATT_GEN_CH0    0x3A000000
#define GET4V1X_TEST_PATT_GEN_CH0    0x3B000000

/*
 * Sub address 100 – Counter Configuration => 0011 1100
 * cf GET4 > v1.20 manuals
 */
#define GET4V1X_TEST_PATT_GEN_CONF   0x3C000000

/*
 * Sub address 101 – Stop Pattern Generator => 0011 1101
 */
#define GET4V1X_TEST_PATT_GEN_ STOP  0x3D000000

/*
 * Sub address 111 – Oscillator Frequency => 0011 1111
 * cf GET4 > v1.20 manuals
 */
#define GET4V1X_TEST_PATT_GEN_OSC_FQ 0x3F000000

/*****************************************************************************/
// Address 01000 – SPI Master Interface
// cf GET4 > v1.20 manuals
/*
 * Sub address 000 – Configuration Register => 0100 0000
 */
#define GET4V1X_SPI_CONF             0x40000000

/*
 * Sub address 001 – SPI start => 0100 0001
 */
#define GET4V1X_SPI_START            0x41000000

/*
 * Sub address 010 – SPI stop => 0100 0010
 */
#define GET4V1X_SPI_STOP             0x42000000

/*
 * Sub address 011 – SPI data => 0100 0011
 */
#define GET4V1X_SPI_DATA             0x43000000

/*****************************************************************************/
// Address 01001 – DAC settings
// Set limits for the DAC used to monitor the DLL lock state
/*
 * Sub address 000 – Lower DLL lock threshold => 0100 1000
 * From Version 1.20 the default value after reset is 0x071.
 */
#define GET4V1X_DLL_DAC_MIN          0x48000000

/*
 * Sub address 001 – Upper DLL lock threshold => 0100 1001
 * From Version 1.20 the default value after reset is 0x238.
 */
#define GET4V1X_DLL_DAC_MAX          0x49000000

/*****************************************************************************/
// Address 01010 – Scaler Readout configuration
// cf GET4 > v1.20 manuals => 0101 0000
#define GET4V1X_SCALER_RO_CONF       0x50000000

/*****************************************************************************/
// Address 01011 – Dead time counter Readout configuration
// cf GET4 > v1.20 manuals => 0101 1000
#define GET4V1X_DEAD_T_CNT_RO_CONF   0x58000000

/*****************************************************************************/
// Address 01100 – Serial Flow Control
// cf GET4 > v1.20 manuals => 0110 0000
#define GET4V1X_SERIAL_FLOW_CONTR    0x60000000

/*****************************************************************************/
// Address 01101 – SEU Counter Readout configuration
// cf GET4 > v1.20 manuals => 0110 1000
#define GET4V1X_SEU_CNT_RO_CONF      0x68000000

/*****************************************************************************/
// Address 01110 – Configuration of TDC Core timing
// cf GET4 > v1.20 manuals => 0111 0000
#define GET4V1X_TDC_CORE_TIMING_CONF 0x70000000

#endif // DEFINES_GET4_V1_X_H
