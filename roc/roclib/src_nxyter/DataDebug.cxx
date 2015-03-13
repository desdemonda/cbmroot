#include "boost/format.hpp"

#include "nxyter/DataDebug.h"

using boost::format;

/*!
 * \class nxyter::DataDebug
 * \brief Class to hold message data retrieved through the 'debug' interface.
 *
 * The 'debug' port provided via registers #ROC_ADC_DATA, #ROC_NX_DATA and
 * #ROC_LT_LOW / #ROC_LT_HIGH gives direct access to the 'wide FIFO' in the
 * ROC logic. 
 *
 * For nXYTER message the 'wide-fifo' holds the raw data from ADC and nXYTER
 * which contains information not available in the final message. For non-nXYTER
 * messages final 48 bit message is stored in the 16 bit 'ADC' and
 * 32 bit 'NX' part of the FIFO.
 *
 * The data format is:
 * <dl>
 * <dt>#ROC_ADC_DATA</dt>
 * <dd>
 *   <ul>
 *     <li> bit 15:12 indicate the message type
 *       <ul>
 *         <li>'000': nXYTER message</li>
 *         <li>'011': Sync message</li>
 *         <li>'100': aux message</li>
 *         <li>'111': GPIO parity message</li>
 *       </ul>
 *     </li>
 *     <li>bit 11:0 hold ADC data for nXYTER message, and the upper 12 bits of
 *         of a non-nXYTER message</li>
 *   </ul>
 * </dd>
 * <dt>#ROC_NX_DATA</dt>
 * <dd>Holds the raw nXYTER data as they come from the chip in case of nXYTER 
 *     messages, only difference is that the \ref glo_nxyter_nxnum is packed 
 *     into bit 4:3. Note that bit 7,15,23 and 31 hold the 4 phases of the
 *     'valid bit', which should ne 0,0,0, and 1 respectively for a valid hit.
 *     In case of non-nXYTER messages this register holds the 32 LSB of the
 *     respective message.
 * </dd>
 * <dt>#ROC_LT_HIGH/ #ROC_LT_LOW</dt>
 * <dd>Holds the local timestamp counter of the ROC latched at the time of
 *     data capture. This counter runs at 250 MHz clock, the LSB thus
 *     corresponds to 4 ns.
 * </dd>
 * </dl>
 */

//----------------------------------------------------------------------------
//! Print message to stream \a os.
/*!
 * A nXYTER message is printed in the format
\verbatim
H: a:2817 nx:1000:1a:1d:00:00  0,  0, 9027 lts:0:00000024:8ee 00149742   +3,5,1
H: a:2789 nx:1000:1a:1c:00:11  2,  0, 9026 lts:0:00000024:8ee 00149742   +3,5,2
H: a:2801 nx:1000:1a:1d:00:18  3,  0, 9027 lts:0:00000024:8ee 00149742   +3,5,1
        ^    ^    ^          ^ ^   ^     ^     ^              ^           ^ ^ ^
        |    |    |          | |   |     |     |              |           | | |
        |    |    nx rawdata | |   |     |     ROC local      timestamp   delta 
        |    |    bytes, bit | |   |     |     timestamp      in decimal  time
        |    |    6:0 only   | |   |     |                                stamp
        |    |               | |   |     nx Timestamp (ungrayed)          see  
        |    nx raw data     | |   nx channel number                      below
        |    valid bits      | nx number (ungrayed)                           
        |                    |                                                 
        nx ADC value         Indicator for nx Parity Error ("P")               
\endverbatim 
 *
 * The difference between the ROC local time stamp and the nXYTER timestamp
 * is represented in the format "+a,b,c" with
 * \li first part representing the 'read-out clock difference, measured in
 *     31.25 MHz clock cycles (1 count = 32 ns). This part is signed and can
 *     vary between -7 and +505.
 * \li second part is the 250 MHz subslot used in the ROC to capture the data.
 *     (1 count = 4 ns).
 * \li third part is the 'fine time' and reflect the 2 LSB of the nXYTER
 *     timestamp (1 count = 1 ns).
 *
 * A non-nXYTER message is currently simply dumped in the format
\verbatim
M:  typ:3 dat:0000:50000080 lts:0:00000020:09f 00131231
M:  typ:3 dat:0000:50000100 lts:0:00000040:09f 00262303
M:  typ:3 dat:0000:50000180 lts:0:00000060:09f 00393375
        ^     ^    ^            ^          
        |     |    |            |
        |     |    |            ROC local Timestamp as for nXYTER messages
        |     |    bits 31:0 of message
        |     bits 44:32 of message
        raw message type: 3=sync; 4=aux; 7=system
 
\endverbatim 
 */


void nxyter::DataDebug::print(std::ostream& os) const
{
  static format fmt_01x("%01x");
  static format fmt_02x("%02x");
  static format fmt_03x("%03x");
  static format fmt_04x("%04x");
  static format fmt_08x("%08x");
  static format fmt_1d("%1d");
  static format fmt_2d("%2d");
  static format fmt_3d("%3d");
  static format fmt_4d("%4d");
  static format fmt_5d("%5d");
  static format fmt_08d("%08d");
  static format fmt_p4d("%+4d");

  uint32_t msgtyp   = getRawMessageType();

  uint32_t nxb3_flg = (fNx>>31) & 0x01;
  uint32_t nxb3_val = (fNx>>24) & 0x7f;
  uint32_t nxb2_flg = (fNx>>23) & 0x01;
  uint32_t nxb2_val = (fNx>>16) & 0x7f;
  uint32_t nxb1_flg = (fNx>>15) & 0x01;
  uint32_t nxb1_val = (fNx>>8 ) & 0x7f;
  uint32_t nxb0_flg = (fNx>>7 ) & 0x01;
  uint32_t nxb0_val = (fNx    ) & 0x7f;
  uint64_t ltsred   = getRawLts() % 100000000;
  // tsdiff is the signed difference between the nXYTER timestamp
  // and the corresponding 14 bits of the ROC local time stamp.
  // The wrapping is done such that value between -7 and + 505 cycles
  // at the 32 MHz clock level are produced.
  int      tsdiff   = int((fLtsLow&0xfff)<<2) - int(getNxTs());
  if (tsdiff < -224) tsdiff += 16384;        // wrap correction, see above

  if (msgtyp == 0) {
    os << "H:";
    os << " a:"  << fmt_4d % getNxAdcValue();
    os << " nx:";
    os << fmt_1d % nxb3_flg;
    os << fmt_1d % nxb2_flg;
    os << fmt_1d % nxb1_flg;
    os << fmt_1d % nxb0_flg;
    os << ":" << fmt_02x % nxb3_val;
    os << ":" << fmt_02x % nxb2_val;
    os << ":" << fmt_02x % nxb1_val;
    os << ":" << fmt_02x % nxb0_val;
    os << ( (getNxParity()!=getNxParityCheck()) ? "P" : " " );
    os << " "     << fmt_1d   % getNxNumber();
    os << ","     << fmt_3d   % getNxChNum();
    os << ","     << fmt_5d   % getNxTs();
    os << " lts:" << fmt_01x  % ((getRawLts()>>44) & 0xf);
    os << ":"     << fmt_08x  % ((getRawLts()>>12) & 0xffffffff);
    os << ":"     << fmt_03x  % (getRawLts() & 0xfff);
    os << " "     << fmt_08d  % ltsred;
    os << " "     << fmt_p4d  % (tsdiff>>5);       // 32 MHz clock slot
    os << ","     << fmt_1d   % ((tsdiff>>2)&0x7); // 256 Mhz sub-slot
    os << ","     << fmt_1d   % (tsdiff&0x3);      // 1 Ghz sub-slot
    os << std::endl;
  } else {
    os << "M: ";
    os << " typ:" << fmt_1d   % msgtyp;
    os << " dat:" << fmt_04x  % (fAdc & 0x1fff);
    os << ":"     << fmt_08x  % fNx;
    os << " lts:" << fmt_01x  % ((getRawLts()>>44) & 0xf);
    os << ":"     << fmt_08x  % ((getRawLts()>>12) & 0xffffffff);
    os << ":"     << fmt_03x  % (getRawLts() & 0xfff);
    os << " "     << fmt_08d  % ltsred;
    os << std::endl;
  }
}

//----------------------------------------------------------------------------
//! Returns nXYTER parity re-calculated from data.
/*!
 * Calculates the parity from data, XOR'ing Bits 30:24, 21:16, 14:8 and 2:0.
 *
 * \sa getNxParity()
 */

uint32_t nxyter::DataDebug::getNxParityCheck() const
{
  // this isn't meant to be efficient....
  uint32_t res = 1;
  for (int i=1; i<=30; i++) {
    if (i>2  && i<8 ) continue;
    if (i==15) continue;
    if (i>21 && i<24) continue;
    res ^= (fNx>>i) & 0x1;
  }
  return res;
}

//----------------------------------------------------------------------------
//! Returns the first \a n bits of \a val after Gray decoding

uint32_t nxyter::DataDebug::ungray(uint32_t val, int n)
{
  uint32_t b=0;
  uint32_t o=0;
  
  for (int i=n; i>0; i--){
    b = b ^ ((val & (1<<(i-1)))>>(i-1));
    o = o | (b << (i-1));
  }
  
  return o;
}
