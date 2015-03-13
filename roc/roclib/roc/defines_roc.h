#ifndef ROC_DEFINES_ROC_H
#define ROC_DEFINES_ROC_H


#define KNUT_VERSION             0x02000100    // All parts share first 4 digits

#define ROC_SUCCESS                       0    // Success
#define ROC_READBACK_ERROR                1    // Readback failed (different value)
#define ROC_ADDRESS_ERROR                 2    // Wrong / unexisting address
#define ROC_VALUE_ERROR                   3    // Wrong value
#define ROC_PERMISSION_ERROR              4    // Permission denied (to low level addresses)
#define ROC_ANSWER_DELAYED                5    // The called function needs longer to run. //DEPRECATED
#define ROC_NETWORK_TRANSMISSION_ERROR    6    // Some slow control udp-data packets got lost.
#define ROC_I2C_ERROR                     7    // No Response (No Ack) Error on I2C bus.


/************************************
 * address space
 ***********************************/

//myOPB (ROC hardware)
//#define myOPB_LOW                0x00000000    // LOW-Marker of myOPB-Range   //DEPRECATED
//#define myOPB_HIGH               0x00ffffff    // HIGH-Marker of myOPB-Range  //DEPRECATED


/*!
 * {ROC, read-only} Holds the version number of the front-end and 
 * back-end type of the ROC
 * Frontend:
 *      1 - NX
 *      2 - FEET
 * Backend:
 *      1 - Optics
 *      2 - Ethernet FX20
 *      3 - Ethernet FX40
 */
#define ROC_TYPE                  0x000000   // r    
/*!
 * {ROC, read-only} Holds the version number of the FPGA firmware
 */
#define ROC_HWV                   0x000004   // r    

/*!
 * {ROC, read-only, 8 bit} FPGA type (1: Virtex-4 fx20, 2: Virtex-4 fx40, 3: Virtex-4 fx60, 4: Spartan 6)
 */
#define ROC_FPGA_TYPE             0x000008   // r    

/*!
 * {ROC, read-write} Unique ROC id, in Optic it is real register
 * in Ethernet - emulated.
 */
#define ROC_ROCID                 0x000010   // r/w

/*!
 * {ROC, read-only, 16 bit} subversion revision number of the firmware
 */
#define ROC_SVN_REVISION          0x000014   // r    

/*!
 * {ROC, read-only, 32 bit} date and time the firmware was build (no of sec since 1970-01-01 0:00)
 */
#define ROC_BUILD_TIME            0x000018   // r    

/*!
 * {ROC, function} A write access will cause the FPGA to reconfigure and, if
 * available, the PPC to reboot. This is a cold restart with immediate effect,
 * the write into this location will thus not be acknowledged.
 */
#define ROC_SYSTEM_RESET           0x000100    // w   

#define ROC_ETH_HWV                0x100000   // r    Hardware-Version
#define ROC_ETH_SWV                0x100004   // r    Software-Version

#define ROC_ADDSYSMSG              0x000200   // w    Write Custom System-Message

#define ROC_CMD_LST_NR             0x020000        // w
#define ROC_CMD_LST_ACTIVE         0x020004        // w
#define ROC_CMD_LST_MEM            0x021000        // w

// different constants for commands list handling

#define ROC_CMD_LST_SIZE           0x000800        // size of memory for commands lists
#define ROC_CMD_LST_NUMBER         0x000008        // number of commands lists
#define ROC_CMD_LST_PUT          0x02000000        // PUT command in commands list
#define ROC_CMD_LST_ADDRMASK     0x00ffffff        // address mask for operations lists


// /*!
// * {ROC, read/write, 3 bit} This register holds the ROC number inserted into
// * all data messages. 
// * Part of \ref roc_iface_daq.
// */
//#define ROC_NUMBER                  0x00040    // r/w //DEPRECATED

/*!
 *{ROC, write-only, 1 bit} ACTIVE HIGH since V2.0.0.2! - Resets the nXYTER-TS
 *
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_TS_RESET             0x400010    // w   
/*!
 * {ROC, function} A write access will clear the message FIFO, thus discard all
 * data held in the buffers on the FPGA. It will insert a 'FIFO RESET' system
 * message into the data stream. 
 * Part of \ref roc_iface_daq.
*/
#define ROC_NX_FIFO_RESET          0x400014    // w   
/*!
 * {ROC, write-only, 1 bit} ACTIVE HIGH! - Starts nXYTER-Receiver-Init
 *
 * Part of \ref roc_iface_nxyter_data.
 */
#define ROC_NX_INIT                 0x400018    // w  

/*!
 * {ROC, read-only, ? bit}
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_BURST1                  0x400100    // r
/*!
 * {ROC, read-only, ? bit}
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_BURST2                  0x400104    // r
/*!
 * {ROC, read-only, ? bit}
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_BURST3                  0x400108    // r

/*!
 * {ROC, read-only, 2 bit}
 * Part of \ref roc_iface_daq.
 */
#define ROC_NX_THROTTLE            0x401200    // r


//ROC/FEB Parameters

/*!
 * Symbolic name for \ref glo_roc_port number 0
 */
#define CON19                       0
/*!
 * Symbolic name for \ref glo_roc_port number 1
 */
#define CON20                       1

#endif

