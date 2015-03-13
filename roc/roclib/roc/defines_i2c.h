#ifndef ROC_DEFINES_I2C_H
#define ROC_DEFINES_I2C_H

/*!
 * {ROC, read/write, 8/16 bit} I2C bus data port. A write into this register
 * or a read from this register will initiate a initiate an I2C bus write
 * or read cycle, respectively. The addressing is determined by the
 * contents of the #ROC_I2C1_SWITCH, #ROC_I2C1_SLAVEADDR, and
 * #ROC_I2C1_REGISTER registers. Any error conditions during the
 * I2C bus cycle are registered in #ROC_I2C1_ERROR. The setting of
 * #ROC_NX_I2C_READ16 determines whether 8 or 16 bit are read. Write
 * cycles are always 8 bit.
 * 
 * Part of \ref roc_iface_i2cbus.
 */
#define ROC_NX_I2C_DATA               0x410000    // r/w
/*!
 * {ROC, read/write, 7 bit} Holds the I2C slave address for the next 
 * I2C bus access.
 *
 * Part of \ref roc_iface_i2cbus.
 */
#define ROC_NX_I2C_SLAVEADDR          0x41000C    // r/w
/*!
 * {ROC, write-only, 8 bit} Holds the device register address for the
 * next I2C bus access. The register is write-only. The address stays
 * after an I2C bus cycle, it is <em>not</em> auto-incremented.
 *
 * Part of \ref roc_iface_i2cbus.
 */
#define ROC_NX_I2C_REGISTER           0x410010    // w
/*!
 * {ROC, read-only, 2 bit} Holds error status flags of the last I2C bus access
 * \li bit 0 is set when a bus cycle is aborted due to a timeout, e.g. 
 *    because a slave does excessive cycle stretching. Firmware after 
 *    01070208 has proper pull-ups, so an open I2C bus shouldn't cause 
 *    timeouts anymore.
 * \li bit 1 is set when an I2C acknowledge is missing, e.g. because no
 *    device responds to the slave address.
 *
 * Part of \ref roc_iface_i2cbus.
 */
#define ROC_NX_I2C_ERROR              0x410020    // r
/*!
 * {ROC, write-only, 1 bit} Selects the port to be used for the next I2C
 * bus access.
 *
 * Part of \ref roc_iface_i2cbus.
 */
#define ROC_NX_I2C_SWITCH             0x410100    // w

/*!
 * {ROC, write-only, 1 bit} Determines whether 1 or 2 bytes are read in a 
 * I2C read cycle. When LSB is 0, the default, only a single byte is read.
 * When LSB is 1, the interface expects that 2 bytes are transmitted from
 * the device and a total of 16 bit is returned in ROC_NX_I2C_DATA.
 *
 * Part of \ref roc_iface_i2cbus.
 */
#define ROC_NX_I2C_READ16             0x410104    // w


#endif
