#ifndef ROC_DEFINES_NXYTER_H
#define ROC_DEFINES_NXYTER_H

/*!
 * {ROC, read-only, 32 bit} This register return version of nXYTER firmware block
 */
#define ROC_NX_HWV   0x400000    // r


/*!
 * {ROC, write-only, 16 bit} This register determines the delay between the
 * reset of the the ROC and nXYTER timestamp counters and the first raising 
 * edge of a test pulse. The delay is 4 ns per count.
 * Part of \ref roc_iface_testpulser.
 */
#define ROC_NX_TESTPULSE_RESET_DELAY   0x403000    // w
/*!
 * {ROC, write-only, 24 bit} This registers determines the length of time the
 * test pulse is "H" (time between raising and falling edges). The "H" time 
 * is (LENGTH+1)*4 ns.
 *
 * Part of \ref roc_iface_testpulser.
 */
#define ROC_NX_TESTPULSE_LENGTH        0x403004    // w
/*!
 * {ROC, write-only, 24 bit} This registers determines the length of time the
 * test pulse is "L" (time between falling and rising edges). The "L" time 
 * is (LENGTH2+1)*4 ns.
 *
 * Part of \ref roc_iface_testpulser.
 */
#define ROC_NX_TESTPULSE_LENGTH2       0x403008    // w
/*!
 * {ROC, write-only, 16 bit} This register controls the number of edges 
 * to be generated after a start with #ROC_TESTPULSE_START. Note, it is
 * the <em>number of edges</em>, the <em>number of pulses</em> is half
 * this count because test triggers/pulses are only generated on one edge.
 *
 * Best is to only write even numbers into this register. 
 *
 * A setting of 0 will produce a continuous train of test pulses. To stop
 * such a continuous sequence simply start a finite sequence, e.g. with a 
 * length of one.
 * Part of \ref roc_iface_testpulser.
 */
#define ROC_NX_TESTPULSE_NUMBER        0x40300C    // w
/*!
 * {ROC, write-only, function} A write will start the test pulser and produce a pulse train
 * defined by the parameters loaded into #ROC_TESTPULSE_RESET_DELAY, 
 * #ROC_TESTPULSE_LENGTH, and #ROC_TESTPULSE_NUMBER.
 * Part of \ref roc_iface_testpulser.
 */
#define ROC_NX_TESTPULSE_START         0x403010    // w
/*!
 * {ROC, read-only}  On read it returns the 'test pulser active' signal.
*  1 means a test pulse sequence is running,
 * 0 means test pulse generator is not active.
 * Part of \ref roc_iface_testpulser.
 */
#define ROC_NX_TESTPULSE_RUNNING       0x403014    // r


/*!
 * {ROC, read/write, 4 bit} Holds a mask with enable flags for the 4 nXYTER. A
 * 0 mask bit will disable the recognition of <em>data valid</em> from the
 * nXYTER and thus suppress generation of hits. Bit n corresponds to the nXYTER
 * with the \ref glo_nxyter_nxnum of n. Note that the red nXYTER activity LED on
 * the ROC is controlled by the accepted hits and therefore affected by the
 * setting of ROC_NX_ACTIVE.
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_NXACTIVE               0x402500    // r/w
/*!
 * {ROC, read/write, 1 bit} If set to 1 the ROC is configured for a single FEB
 * of .\ref glo_feb_feb4nx
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_FEB4NX                 0x402508    // r/w 
/*!
 * {ROC, read/write, 1 bit} If set to 1, the parity bit of nXYTER hits is 
 * checked. In case of a parity mismatch, the nXYTER hit is discarted and a
 * nXYTER parity system message generated instead. If set to 0, no parity 
 * checking is performed, all nXYTER hits caused by a <em>data valid</em> 
 * will produce a hit message.
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_PARITY_CHECK            0x402504    // r/w

/*!
 * {ROC, write-only, 1 bit} ACTIVE LOW!
 * Part of \ref roc_iface_nxyter_cntl.
 */
#define ROC_NX_I2C_RESET              0x410004    // w   ACTIVE LOW!
/*!
 * {ROC, write-only, 1 bit} ACTIVE LOW!
 * Part of \ref roc_iface_nxyter_cntl.
 */
#define ROC_NX_I2C_REGRESET           0x410008    // w   ACTIVE LOW!

/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_1a           0x404200    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_1b           0x404204    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_1c           0x404208    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_1d           0x40420C    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_2a           0x404210    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_2b           0x404214    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_2c           0x404218    // r
/*!
 * {ROC, read-only, 12 bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_DIRECT_2d           0x40421C    // r

/*!
 * {ROC, write-only, 8 bit} A write into this register causes a SPI write
 * transaction to the main ADC on \ref glo_roc_port 0.
 * The device register given by #ROC_ADC_ADDR is loaded with the data written 
 * into #ROC_ADC_REG.
 *
 * Part of \ref roc_iface_mainadc_cntl.
 */
#define ROC_NX_ADC_REG                0x404000    // w
/*!
 * {ROC, write-only, 8 bit}  This register holds the device register address
 * for the next SPI access to the main ADC on \ref glo_roc_port 0.
 * Part of \ref roc_iface_mainadc_cntl.
 */
#define ROC_NX_ADC_ADDR               0x404004    // w
/*!
 * {ROC, read-only, 8 bit} A read from this register causes a SPI read
 * transaction from the main ADC on \ref glo_roc_port 0.
 * The device register given by #ROC_ADC_ADDR is read and the retrieved
 * data is made avaibale via #ROC_ADC_ANSWER.
 * Part of \ref roc_iface_mainadc_cntl.
 */
#define ROC_NX_ADC_ANSWER             0x404008    // r

/*!
 * {ROC, write-only, 8 bit} Like #ROC_ADC_REG but for \ref glo_roc_port 1.
 *
 * Part of \ref roc_iface_mainadc_cntl.
 */
#define ROC_NX_ADC_REG2               0x404100    // w
/*!
 * {ROC, write-only, 8 bit} Like #ROC_ADC_ADDR but for \ref glo_roc_port 1.
 *
 * Part of \ref roc_iface_mainadc_cntl.
 */
#define ROC_NX_ADC_ADDR2              0x404104    // w
/*!
 * {ROC, read-only, 8 bit} Like #ROC_ADC_ANSWER but for \ref glo_roc_port 1.
 *
 * Part of \ref roc_iface_mainadc_cntl.
 */
#define ROC_NX_ADC_ANSWER2            0x404108    // r

/*!
 * {ROC, read/write, ? bit}
 * Part of \ref roc_iface_mainadc_data.
 * \note For \ref glo_feb_feb4nx the timing is deterimed with this parameter
 *   because the clocks are on port 0 (#CON19).
 */
#define ROC_NX_SR_INIT                 0x402100    // r/w
/*!
 * {ROC, read/write, ? bit}
 * Part of \ref roc_iface_mainadc_data.
 * \note For \ref glo_feb_feb4nx the timing is deterimed with this parameter
 *   because the clocks are on port 0 (#CON19).
 */
#define ROC_NX_BUFG_SELECT             0x402104    // r/w
/*!
 * {ROC, read/write, ? bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_SR_INIT2                0x402200    // r/w
/*!
 * {ROC, read/write, ? bit}
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_BUFG_SELECT2            0x402204    // r/w

/*!
 * {ROC, read/write, ? bit}
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_DELAY_LTS               0x402000    // r/w
/*!
 * {ROC, read/write, 16 bit} Controls the delay of the RESET pulse for the
 * FEB connected to port 0 (#CON19) in units of 250 MHz clock cycles (4ns).
 *
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_DELAY_NX0               0x402010    // r/w
/*!
 * {ROC, read/write, 16 bit} This 16 bit register is implemented but has no
 * hardware function. It does despite its name it does <em>not</em> control 
 * any nXYTER timing ! \sa #ROC_DELAY_NX3.
 */
#define ROC_NX_DELAY_NX1               0x402014    // r/w
/*!
 * {ROC, read/write, 16 bit} Controls the delay of the RESET pulse for the
 * FEB connected to port 1 (#CON20) in units of 250 MHz clock cycles (4ns).
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_DELAY_NX2               0x402018    // r/w
/*!
 * {ROC, read/write, 16 bit} This 16 bit register is implemented but has no
 * hardware function. It does despite its name does not <em>not</em> control
 * any nXYTER timing ! \sa #ROC_DELAY_NX1.
 */
#define ROC_NX_DELAY_NX3               0x40201C    // r/w

/*!
 * {ROC, read/write, 9 bit} Controls delay between ADC data capture (caused by
 *   'frame clock edge'), and the association with the nXYTER data.
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_LATENCY1            0x402300    // r/w
/*!
 * {ROC, read/write, 9 bit} Like #ROC_ADC_LATENCY1 but for 2nd ADC channel.
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_LATENCY2            0x402304   // r/w
/*!
 * {ROC, read/write, 9 bit} Like #ROC_ADC_LATENCY1 but for 3rd ADC channel.
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_LATENCY3            0x402308    // r/w
/*!
 * {ROC, read/write, 9 bit} Like #ROC_ADC_LATENCY1 but for 4th ADC channel.
 * Part of \ref roc_iface_mainadc_data.
 */
#define ROC_NX_ADC_LATENCY4            0x40230C    // r/w

/*!
 * {ROC, read/write, 2 bit} Selects the ADC channel for the nXYTER with 
 * \ref glo_nxyter_nxnum = 0. Possible values are:
 * \li 0 use data from pin A17/B17
 * \li 1 use data from pin A18/B18
 * \li 2 use data from pin A20/B20
 * \li 3 use data from pin A21/B21
 * 
 * Part of \ref roc_iface_mainadc_data.
 * \note \ref glo_nxyter_nxnum = 0 data come from #CON19 if #ROC_FEB4NX = 0 
 * and from #CON20 if #ROC_FEB4NX = 1.
 */
#define ROC_NX_ADC_PORT_SELECT1        0x402400    // r/w
/*!
 * {ROC, read/write, 2 bit} Like #ROC_ADC_PORT_SELECT1 but for nXYTER with 
 * \ref glo_nxyter_nxnum = 1.
 * Part of \ref roc_iface_mainadc_data.
 * \note \ref glo_nxyter_nxnum = 1 data come from #CON19 if #ROC_FEB4NX = 0 
 * and from #CON20 if #ROC_FEB4NX = 1.
 */
#define ROC_NX_ADC_PORT_SELECT2        0x402404    // r/w
/*!
 * {ROC, read/write, 2 bit} Like #ROC_ADC_PORT_SELECT1 but for nXYTER with 
 * \ref glo_nxyter_nxnum = 2.
 * Part of \ref roc_iface_mainadc_data.
 * \note \ref glo_nxyter_nxnum = 2 data always come from #CON20.
 */
#define ROC_NX_ADC_PORT_SELECT3        0x402408    // r/w
/*!
 * {ROC, read/write, 2 bit} Like #ROC_ADC_PORT_SELECT1 but for nXYTER with 
 * \ref glo_nxyter_nxnum = 3.
 * Part of \ref roc_iface_mainadc_data.
 * \note \ref glo_nxyter_nxnum = 3 data always come from #CON20.
 */
#define ROC_NX_ADC_PORT_SELECT4        0x40240C    // r/w

/*!
 * {ROC, write-only, 1 bit} Controls whether the DAQ subsystem is working
 * in normal mode (bit 0 is 0) or in 'debug' mode (bit 0 is 1). In debug
 * mode the hit builder logic between 'wide FIFO' and 'event FIFO' is
 * disabled and data can be read directly from the 'wide FIFO' via the
 * registers #ROC_NX_ADC_DATA, #ROC_NX_NX_DATA, #ROC_NX_LT_LOW, and #ROC_NX_LT_HIGH.
 *
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_DEBUG_MODE              0x402600    // w

/*!
 * {ROC, read-only, 1 bit} A 1 indicates that of the the 3 FIFOs (NX, ADC, LTS)
 * are empty.
 * 
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_FIFO_EMPTY           0x401108    // r  
/*!
 * {ROC, read-only, 1 bit} A 1 indicates that of the the 3 FIFOs (NX, ADC, LTS)
 * are full.
 * 
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_FIFO_FULL            0x40110C    // r   

/*!
 * {ROC, read-only, 32 bit} Part of 'DAQ debug port' and can be used when
 * #ROC_DEBUG_MODE is set to 1 to access th 'wide FIFO'. The 'wide-FIFO' is
 * implemented as three independent FIFO, to keep them consistent all three
 * must be read, #ROC_ADC_DATA, #ROC_NX_DATA, and #ROC_LT_LOW / #ROC_LT_HIGH.
 * The timestamp part is 64 bit and accessed via two registers, #ROC_LT_LOW
 * and #ROC_LT_HIGH. Always read #ROC_LT_LOW before #ROC_LT_HIGH.
 * 
 * Detailed data format description in nxyter::DataDebug.
 *
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_LT_LOW               0x401000    // r   
/*!
 * {ROC, read-only, 32 bit} Part of 'DAQ debug port' and can be used when
 * #ROC_DEBUG_MODE is set to 1 to access th 'wide FIFO'. The 'wide-FIFO' is
 * implemented as three independent FIFO, to keep them consistent all three
 * must be read, #ROC_ADC_DATA, #ROC_NX_DATA, and #ROC_LT_LOW / #ROC_LT_HIGH.
 * The timestamp part is 64 bit and accessed via two registers, #ROC_LT_LOW
 * and #ROC_LT_HIGH. Always read #ROC_LT_LOW before #ROC_LT_HIGH. The read
 * access of #ROC_LT_HIGH will advance the timestamp FIFO.
 *
 * Note: #ROC_LT_HIGH/#ROC_LT_LOW reflect the internal 64 bit ROC local
 *       timestamp counter. Only the lower 44 bit are used in the message
 *       building, the 12 LSB's to decide on the 'last-epoch' bit, and the
 *       bits 43:12 to build the epoch marker (using a 32 bit epoch number).
 * 
 * Detailed data format description in nxyter::DataDebug.
 *
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_LT_HIGH               0x401004    // r   

/*!
 * {ROC, read-only, 16 bit} Part of 'DAQ debug port' and can be used when
 * #ROC_DEBUG_MODE is set to 1 to access th 'wide FIFO'. The 'wide-FIFO' is
 * implemented as three independent FIFO, to keep them consistent all three
 * must be read, #ROC_ADC_DATA, #ROC_NX_DATA, and #ROC_LT_LOW / #ROC_LT_HIGH.
 * 
 * Detailed data format description in nxyter::DataDebug.
 *
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_ADC_DATA              0x401100    // r
/*!
 * {ROC, read-only, 32 bit} Part of 'DAQ debug port' and can be used when
 * #ROC_DEBUG_MODE is set to 1 to access th 'wide FIFO'. The 'wide-FIFO' is
 * implemented as three independent FIFO, to keep them consistent all three
 * must be read, #ROC_ADC_DATA, #ROC_NX_DATA, and #ROC_LT_LOW / #ROC_LT_HIGH.
 * 
 * Detailed data format description in nxyter::DataDebug.
 *
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_NX_DATA               0x401104    // r

#define ADC_PORT_A                  0
#define ADC_PORT_B                  1
#define ADC_PORT_C                  2
#define ADC_PORT_D                  3

#endif
