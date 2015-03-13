#ifndef ROC_UDPBOARD_H
#define ROC_UDPBOARD_H

#include "roc/Board.h"
#include "roc/defines_udp.h"

namespace roc {

   extern const char* xmlTransferWindow;

   class UdpBoard : public roc::Board {

      protected:

         int        fRunningUpload;
         uint32_t   fUploadChecksum;

         UdpBoard();
         virtual ~UdpBoard();

         int parseBitfileHeader(char* pBuffer, unsigned int nLen);
         uint8_t calcBinXOR(const char* filename);

         virtual int rawPut(uint32_t address, uint32_t value, const void* rawdata = 0, double tmout = 0.) = 0;
         bool uploadDataToRoc(char* buf, unsigned len);

         virtual bool submitSwPut(uint32_t address, uint32_t value, double tmout) = 0;
         virtual int checkSwPut() = 0;

      public:


         virtual int getTransportKind() const { return kind_UDP; }
         
         virtual int invokeDLM(unsigned num, double tmout = 0.);


         //! Returns power pc software version
         virtual uint32_t getPowerPCVersion();


         virtual uint32_t getRocBackendVersion();

         //! BURST
         /*!
          * \param val Is Burst on? (1 is yes, 0 is no)
          *
          * Activates/Deactivates the burst.
          * Burst means using OCM-Bus for intra ROC communication.
          * This is currently NOT supported, due to wrong timing calculations by Xilinx EDK 8.2i and ISE 8.2i.
          */
         void BURST(uint32_t val);

         //! Returns UDP data buffer thresholds in ROC.
         /*!
          * For details of semantics of the data buffer thresholds see
          * setRocLowHighWater(). Only applicable
          * when UDP transport is used, will return zero's and a false
          * return status otherwise.
          * \param lowWater low water threshold (in KB).
          * \param highWater high water threshold (in KB).
          * \returns \c false if ROC access failed or not using UDP transport,
          *    \c true otherwise.
          * \sa setRocLowHighWater()
          */
         bool getRocLowHighWater(int& lowWater, int& highWater);

         //! Sets UDP data buffer thresholds in ROC.
         /*!
          * When UDP transport is used, the ROC buffers data in a large memory
          * pool more than 100 MByte capacity. For maximal throughput a large
          * buffer space is best, especially for operation on syncrotron beams.
          * In test and laboratory setups a controlled maximal latency is more
          * important than top throughput, in this case the buffer pool should
          * be reduced. This is implemented by a <em>'high water'</em>
          * threshold, at which the ROC suspends transmitting data, and a
          * <em>'low water'</em> threshold at which is resumes sending data.
          *
          * \param lowWater low water threshold (in KB).
          * \param highWater high water threshold (in KB).
          * \returns \c false if ROC access failed or not using UDP transport,
          *    \c true otherwise.
          * \sa getRocLowHighWater()
          */
         bool setRocLowHighWater(int lowWater, int highWater);

         //! Returns UDP data buffer flush timer in ROC.
         /*!
          * For details of semantics of the data buffer flush timer see
          * setRocBufferFlushTimer(). Only applicable when UDP transport is
          * used, will return zero's and a \c false return status otherwise.
          * \param bufferFlushTimer buffer flush time (in msec).
          * \returns \c false if ROC access failed or not using UDP transport,
          *    \c true otherwise.
          * \sa setRocBufferFlushTimer()
          */
         bool getRocBufferFlushTimer(uint32_t& bufferFlushTimer);

         //! Set UDP data buffer flush timer in ROC.
         /*!
          * When UDP transport is used, the ROC tries optimize throughput by
          * sending UDP messages of a size corresponding the maximal Ethernet
          * packet size. To limit data latency in situations with a low data
          * rate a <em>flush timer</em> ensures that UDP message is send after
          * a maximal waiting time. For good responsiveness in interactive
          * test of laboratory setups use flush times of 100 msec or below,
          * and set also the buffering limits, see setRocLowHighWater().
          *
          * \param bufferFlushTimer buffer flush time (in msec).
          * \returns \c false if ROC access failed or not using UDP transport,
          *    \c true otherwise.
          * \sa getRocBufferFlushTimer()
          */
         bool setRocBufferFlushTimer(uint32_t bufferFlushTimer);

         //! takeStat
         /*!
         * Returns statistic block over roc::Board.
         * If \a tmout bigger than 0, first request to board will be produced,
         * otherwise last available statistic block will be delivered
         * If \a print is enabled, statistic block will be printed on display
         */

         virtual BoardStatistic* takeStat(double tmout = 0.01, bool print = false) = 0;


         //! setConsoleOutput
         /*!
         * Enables/disables console output of the ROC   fRole(role),
         *
         * /param terminal defines if output on RS232 console will be provided
         * /param network defines if output will be send over network to user terminal
         */
         virtual void setConsoleOutput(bool terminal = false, bool network = false);

         /** Send console command
          * Via this function you can send remote console commands.
          * It works like telnet.
          * It is developed mainly for debugging reasons. */
         virtual bool sendConsoleCommand(const char* cmd);

         //! switchToConsole
         /*!
          * Changes the ROC input to RS232.
          * Network control is instantly lost and has to be reactivated via RS232.
          */
         void switchToConsole();

         /** Start uploading of bit file to ROC
          * Uploads bitfile to specified position (0 or 1)
          * Returns -1 if error detected or time is seconds how long operation will take
          * Result can be polled with checkUpload(). */
         int uploadBitfile(const char* filename, int position);

         /** Start uploading of file to the SD of the ROC
          * Returns -1 if error detected or time is seconds how long operation will take
          * Result can be polled with checkUpload(). */
         int uploadSDfile(const char* filename, const char* sdfilename = 0);

         /** Check is upload is completed
          * 0 - not running, 1 - running, 2 - done, -1 - fails */
         int checkUpload();

         /** Save ROC configuration on SD card
          * If name not specified, default config file name ROC1.cfg is used */
         virtual bool saveConfig(const char* filename = 0);

         /** Load ROC configuration from SD card
          * If name not specified, default config file name ROC1.cfg is used */
         virtual bool loadConfig(const char* filename = 0);
   };

}

#endif
