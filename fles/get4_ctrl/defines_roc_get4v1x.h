#ifndef ROC_DEFINES_GET4_V1_X_H
#define ROC_DEFINES_GET4_V1_X_H

/*****************************************************************************/
/****************************** ROC  module **********************************/
/*****************************************************************************/
/*
 * 32 bit, read-only 
 * Holds the version number of the front-end and back-end type of the ROC
 * The higher 16 bit determine the fontend module, the lower 16 bit determine 
 * the transport module. 
 * Frontend:
 *      1 - NX
 *      2 - The FEET readout (old GET4s). Support dropped.
 *      3 - The GET4 v1.x readout.
 * Backend:
 *      1 - Optics
 *      2 - Ethernet FX20
 *      3 - Ethernet FX40
 *      4 - Ethernet FX60
 *      5 - Transport via USB
 */
#define ROC_TYPE                         0x000000   // r    

/*
 * 32 bit, read-only
 * Holds the version number of the FPGA firmware
 */
#define ROC_HWV                          0x000004   // r    

/*
 * 8 bit, read-only 
 * FPGA type
 *      1: Virtex-4 fx20
 *      2: Virtex-4 fx40
 *      3: Virtex-4 fx60
 *      4: Spartan 6
 *      5: SysCoreV3.x 
 */
#define ROC_FPGA_TYPE                    0x000008   // r    

/*
 * 16 bit, read-write
 * Unique ROC id, in Optic it is real register in Ethernet - emulated.
 */
#define ROC_ROCID                        0x000010   // r/w

/*
 * 16 bit, read-only 
 * subversion revision number of the firmware
 */
#define ROC_SVN_REVISION                 0x000014   // r    

/*
 * 32 bit, read-only,
 * date and time the firmware was build (no of sec since 1970-01-01 0:00)
 */
#define ROC_BUILD_TIME                   0x000018   // r    

/*
 * 1 bit, writeonly 
 * A write access will cause the FPGA to reconfigure and, if available, the PPC 
 * to reboot. This is a cold restart with immediate effect, the write into this 
 * location will thus not be acknowledged.
 */
#define ROC_SYSTEM_RESET                  0x000100    // w   

/*
 * 1 bit, writeonly, any value 
 * This will reset the data path logic in the frontend module.
 * Note that there might be some FIFOs in the communication module that are not reset.
 * A clean FIFO reset requires the ROC to be in the state ”DAQ-Stopped”.
 */
#define ROC_FIFO_RESET                    0x000104   

/*
 * 0 bit, writeonly, any value 
 * This will reset the time stamp counter for the 250 MHz in the frontend module.
 */
#define ROC_TS_RESET                      0x000108

/*
 * 32 bit, writeonly 
 * Write Custom System-Message
 */
#define ROC_ADDSYSMSG                     0x000200 

/*
 * 1 bit, read/write.
 * If this bit is set, there are epoch markers derived from the 250 MHz clock inserted in the
 * datastream. 
 * This is important when synchronizing the FE data to data from FE on other controllers/readout.
 * Per default this register is set.
 */
#define ROC_EPOCH250_EN                   0x000204

/*
 * 4 bit, writeonly 
 * The lower 4 bit that are written to this address will be send as a DLM to the optic receiver.
 */
#define ROC_OPTICS_DLMGEN                 0x000208        // w
/*
 * 32 bit, readonly 
 * This register stores the last 8 received DLMs. 
 * The four lowest bits hold the value of the most recently received DLM.
 */
#define ROC_OPTICS_DLM_HISTORY            0x00020C        // r

/*
 * 1 bit, writeonly, any value  
 * start daq, enables sending of data from ROC via optic
 * Before the first data packet a system message (SYS_MSG_DAQ_START) is send out.
 */
#define ROC_START_DAQ                     0x000300

/*
 * 1 bit, writeonly, any value 
 * stop daq, disables sending of data from ROC via optic
 * If sending of an CBMNet data packet currently in process it will not be interrupted. 
 * A system message (SYS_MSG_DAQ_STOP) is send out as the last data packet. If there is
 * still data in the fo of the optical module, this data will reside there. However, 
 * no more data will be read out from the frontend module when DAQ is stopped.
 */
#define ROC_STOP_DAQ                      0x000304

/*
 * 32 bit, readonly.
 * With this commands the 48 bit wide data fifo can be read out over the control path.
 * The control path has only 32 bit wide data words. So for the readout of two 48 bit words
 * there are three read commands specified.
 */
#define ROC_BURST1                        0x000308
#define ROC_BURST2                        0x00030C
#define ROC_BURST3                        0x000310

/*
 * ?? bit, writeonly, any value 
 * enable/disable data generator
 */
#define ROC_DATAGEN                       0x000314  

/*
 * GPIO control = Adresses in range  0x010000 
 * See fles/nxyter_ctrl/defines_gpio.h
 */ 

/*
 * 3 bit, writeonly.
 * This will activate a command list. 
 */
#define ROC_CMD_LST_NR                    0x020000        // w
/*
 * 1 bit, writeonly.
 * This will (de-)activate the command list processing. This is available for internal use, 
 * to deactivate the command list processing at the end of the list.
 * However one can also use it to activate list processing, then the previously processed list
 * is activated again.
 */
#define ROC_CMD_LST_ACTIVE                0x020004        // w
/*
 * 32 bit words, 2k address space, read/write.
 * This address space translates directly to the memory where the command lists are stored. 
 * Only 4-byte aligned addresses (last two bits equal 0) are valid. 
 * Each list can store up to 32 commands (2*4 bytes per command, 256 bytes/list).
 * Valid adresses in range: 0x 02 10 00 - 0x 02 17 FC
 */
#define ROC_CMD_LST_MEM                   0x021000        // w

//------- different constants for commands list handling ----------//
/*
 * Size of memory for commands lists: last valid adress is  
 * ROC_CMD_LST_MEM + ROC_CMD_LST_SIZE - 4
 */
#define ROC_CMD_LST_SIZE                  0x000800        
/*
 * Number of of possible commands lists
 */
#define ROC_CMD_LST_NUMBER                0x000008
/*
 * 
 * PUT command in commands list
 */
#define ROC_CMD_LST_PUT                 0x02000000 
/*
 * address mask for operations lists
 */
#define ROC_CMD_LST_ADDRMASK            0x00ffffff
//-----------------------------------------------------------------//

/*
 * 32 bit, readonly.
 * Hardware-Version
 */
#define ROC_ETH_HWV                       0x100000   // r   
/*
 * 32 bit, readonly.
 * Software-Version
 */
#define ROC_ETH_SWV                       0x100004   // r  

/*****************************************************************************/
/****************************** OPTIC module *********************************/
/*****************************************************************************/

/*
 * 32 bit, readonly.
 * This address returns the hardware version of the optics transport module.
 */
#define ROC_OPTICS_HWV                    0x200000        // r

/*
 * 1 bit, readonly.
 * This address returns 1 if radiation mitigation techniques are present in 
 * the module and 0 elsewise.
 */
// true if there are radiation mitigation techniques in use
#define ROC_OPTICS_RADTOL                 0x200004        // r

/*
 * ?? bit, readonly.
 * Status of optical link.
 */
#define ROC_OPTICS_LINK_STATUS            0x201008

/*****************************************************************************/
/****************************** GET4 module **********************************/
/*****************************************************************************/

/*
 * 32 bit, readonly.
 * This address returns the hardware version of the GET4 frontend module.
 */
#define ROC_GET4_HWV                      0x500000

/*
 * 1 bit, readonly.
 * This address returns “1” if radiation mitigation techniques are present
 * in the module and “0” elsewise.
 */
#define ROC_GET4_RADTOL                   0x500004

/*
 * 8 bit, readonly.
 * This address returns the number of ports available in the firmware for
 * connecting GET4 chips.
 */
#define ROC_GET4_NR_OF_GET4S              0x500008

/*
 * 0 bit, writeonly.
 * This will reset the control logic in the GET4 frontend module.
 *
 */
#define ROC_GET4_RESET                    0x500010

/*
 * 0 bit, writeonly.
 * This will reset the time stamp counter for the 156.25 MHz in the GET4 frontend module.
 */
#define ROC_GET4_TS156_RESET              0x500014

/*
 * 2 bit, read/write.
 * The 2 bit register is used to configure the incoming data rate.
 * This should match the configuration of the GET4s (see GET4 documentation).
 * The following values are available:
 * "11" => 156.25 MHz
 * "10" =>  78.13 MHz
 * "01" =>  39.06 MHz
 * "00" =>  19.53 MHz (default)
 */
#define ROC_GET4_RECEIVE_CLK_CFG          0x500100

/*
 * 32 bit, read/write.
 * For each GET4 chip one can specify if the serial data will be sampled on the rising edge
 * or on the falling edge of the serial clock.
 * The default is sampling on the rising edge.
 * Two registers in order to support up to 64 chips.
 */
#define ROC_GET4_SAMPLE_FALLING_EDGE_LSBS 0x500200
#define ROC_GET4_SAMPLE_FALLING_EDGE_MSBS 0x500204

/*
 * 32 bit, read/write.
 * For each channel one can specify whether 156MHz epoch markers should be withhold if no hit was
 * detected during the corresponding epoch.
 * Suppressing markers of empty epochs might drastically reduce the data volume in low rate
 * experiments, however full epoch information might be useful for debugging and consistency
 * checks. By default epoch marker suppression is turned off on all channels.
 * Two registers in order to support up to 64 chips.
 */
#define ROC_GET4_SUPRESS_EPOCHS_LSBS      0x500208
#define ROC_GET4_SUPRESS_EPOCHS_MSBS      0x50020C

/*
 * 32 bit, read/write.
 * The 32 bit register is used to unmask incoming data from the GET4 chips.
 * If this register holds the value 0x 0000 0001, only data from the first GET4 will be sampled.
 * A value of 0x FFFF FFFF will sample on every data port.
 * One should only activate ports with a connected GET4 chip since unconnected LVDS lines tend to
 * toggle unpredictably.
 * The default is 0x 0000 0000, all ports are disabled.
 * Two registers in order to support up to 64 chips.
 */
#define ROC_GET4_RECEIVE_MASK_LSBS        0x501000
#define ROC_GET4_RECEIVE_MASK_MSBS        0x501004

/*
 * 32 bit, read/write.
 * The 32 bit register is used to unmask outgoing control messages to the GET4 chips.
 * If this register holds the value 0x 0000 0001, the commands send to ROC GET4 CMD TO GET4 will
 * only be passed to GET4 number 1.
 * A value of 0x FFFF FFFF can be used to address to every connected GET4 simultaneously, this is
 * also the default.
 * Two registers in order to support up to 64 chips.
 */
#define ROC_GET4_TRANSMIT_MASK_LSBS       0x501010
#define ROC_GET4_TRANSMIT_MASK_MSBS       0x501014

/*
 * 32 bit, read/write.
 * The 32 bit register is used to unmask the outputs of the data generator.
 * If this register holds the value 0x 0000 0001, data will be available on date generator output 0.
 * A value of 0x FFFF FFFF can be used to generate data on all 24 outputs.
 * If ROC GET4 DATAGEN EN is set, these outputs will be connected to the data receivers instead of
 * data from the GET4s.
 */
#define ROC_GET4_DATAGEN_MASK             0x501100

/*
 * 32 bit, read/write.
 * The 32 bit register can be used to shape the data that is generated by the data generator.
 * When the data generator is enabled, the register is shifted left and the MSB is used as LSB (circular
 * shift register). So every 32 serial data clock cycles the pattern is repeated. The MSB is connected
 * to the (unmasked) outputs of the data generator.
 */
#define ROC_GET4_DATAGEN_INIT             0x501104

/*
 * 1 bit, writeonly.
 * This enables/disables the data generator.
 * When the data generator is enabled, ROC GET4 DATAGEN INIT and ROC GET4 DATAGEN MASK cannot be updated
 * but the old values are used until the data generator is disabled.
 */
#define ROC_GET4_DATAGEN_EN               0x501108

/*
 * 32 bit, writeonly.
 * The GET4 chip has a 32 bit wide control interface.
 * The 32 bit that are written to this address will be passed to the GET4s enabled by the
 * ROC_GET4_TRANSMIT_MASK register.
 */
#define ROC_GET4_CMD_TO_GET4              0x510000

/*
 * 1 bit, read/write.
 * This selects the GET4 readout mode:
 *  - 0 selects the original 24 bits mode.
 *  - 1 selects the new 32 bits mode (hack or true)
 */
#define ROC_GET4_READOUT_MODE             0x5F0000

#endif // ROC_DEFINES_GET4_V1_X_H
