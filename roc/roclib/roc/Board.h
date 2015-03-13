#ifndef ROC_BOARD_H
#define ROC_BOARD_H

#include <stdint.h>

#include <iostream>
#include <string>
#include <map>
#include <list>

#include "roc/Message.h"

#include "roc/OperList.h"


namespace roc {

  //! Represents the role the client of a board can have
  enum ClientRole { roleNone,               //!< no defined role
                    roleObserver,           //!< pure observer, no changes
                    roleControl,            //!< allowed to change state, no DAQ
                    roleDAQ                 //!< full control including DAQ
   };

   // FIXME: why is MBS stuff here ???
   // Answer (S.Linev): - just to have in common place constants, which define mbs event types,
   //                     which is than used in DABC and further application, where MBS files should be read

   //! value of iProcId if mbs subevent.
   /*!
    * Data, taken from ROC by DABC, packed into mbs events and stored in lmd file.
    * MBS event consists from one or several subevents.
    * Each subevent has three fields - iProcId (16bit), iSubcrate (8bit) and iControl (8bit)
    * iProcId used to code data origin - see comment for  /ref ERocMbsTypes
    * iSubcrate contains roc number (rocid)
    * iControl stores data format see /ref MessageFormat
    */
   enum ERocMbsTypes {
      proc_RocEvent     =  1,   //!< complete event from one roc board (iSubcrate = rocid)
      proc_ErrEvent     =  2,   //!< one or several events with corrupted data inside (iSubcrate = rocid)
      proc_MergedEvent  =  3,   //!< sorted and synchronized data from several rocs (iSubcrate = upper rocid bits)
      proc_RawData      =  4,   //!< unsorted uncorrelated data from one ROC, no SYNC markers required (mainly for FEET case)
      proc_Triglog      =  5,   //!< subevent produced by MBS directly with sync number and trigger module scalers
      proc_TRD_MADC     =  6,   //!< subevent produced by MBS directly with CERN-Nov10 data
      proc_TRD_Spadic   =  7,   //!< collection of data from susibo board
      proc_CERN_Oct11   =  8,   //!< id for CERN beamtime in October 11
      proc_SlaveMbs     =  9,   //!< subevent produce by slave MBS system, which emulates number of triglog module
      proc_EPICS        = 10,   //!< subevent produced by dabc EPICS plugin (ezca)
      proc_COSY_Nov11   = 11    //!< id for COSY beamtime in November 11
   };

   // symbolic names for ROC-related configuration parameters
   extern const char* xmlNumRocs;
   extern const char* xmlRocPool;
   extern const char* xmlTransportWindow;
   extern const char* xmlTransportKind;
   extern const char* xmlBoardAddr;
   extern const char* xmlRole;
   extern const char* xmlRocNumber;
   extern const char* xmlLowWater;
   extern const char* xmlHighWater;
   extern const char* xmlMsgFormat;
   extern const char* xmlFlushTime;

   extern const char* typeUdpDevice;
   extern const char* typeAbbDevice;

   extern const char* roleToString(ClientRole role);
   extern ClientRole defineClientRole(const char* name, 
                                      ClientRole dflt = roleObserver);

   enum ERocBufferTypes {
      rbt_RawRocData     = 234
   };
   
   enum ETransportKinds {
      kind_UDP = 0,
      kind_ABB = 1,
      kind_File = 2
   };

   enum EFrontendKinds {
      kind_nXYTER = 1,
      kind_FEET = 2
   };

   enum EBackendKinds {
      kind_Optic = 1,
      kind_FX20  = 2,
      kind_FX40  = 3,
      kind_FX60  = 4
   };


   class BoardConnector;

   class Board {

      friend class BoardConnector;

      protected:
         ClientRole      fRole;              //!< client role (access rights)
         uint32_t        fRocNumber;         //!< number of connected ROC
         double          fDefaultTimeout;    //!< default timeout
         int             fVerbosity;         //!< Debug verbosity level
         int             fOperTrace;         //!< Oper trace level
         BoardConnector* fConnector;

         std::map<std::string,uint32_t> fMapName2Addr;
         std::map<uint32_t,std::string> fMapAddr2Name;
         typedef std::map<std::string,uint32_t>::iterator mapn2a_iter_t;
         typedef std::map<uint32_t,std::string>::iterator mapa2n_iter_t;

         Board();
         virtual ~Board();

         void setRole(ClientRole r) { fRole = r; }

         void traceOper(OperList* lst,  int rc);

         virtual void ShowDebug(int lvl, const char* msg);

         /** reads roc number and displays basic version values
          * return ROC hardware version or 0 in case of error */
         uint32_t initBoard();

      public:

         //! Return codes for get, put and oper calls
         /*!
          * For a detailed documentation of get/put/oper return code see
          * \ref roc_board_opererr.
          */
         enum operErr {
           kOperSuccess    = 0,             //!< success
           kOperAddrErr    = 2,             //!< invalid address
           kOperValueErr   = 3,             //!< value not allowed in context
           kOperStateErr   = 4,             //!< operation not allowed in state
           kOperNetworkErr = 6,             //!< network communication error
           kHostError      = 8,             //!< PC software stack errpr
           kOperVerifyErr  = 20,            //!< readback after write mismatch
           kOperBusErr     = 21             //!< secondary bus error
         };      

         ClientRole getRole() const { return fRole; }

         void Debug(int lvl, const char *fmt, ...);

         bool findRocAddressByName(const char* name, uint32_t& addr);
         bool findRocNameByAddress(uint32_t addr, const char*& name);
         void printRocAddressMap(std::ostream& os, bool byname=false);

         void addRocAddrMapping(const char* name, uint32_t addr);

         void fillAddrNames(std::list<std::string>& lst);

         //! Set debug output verbosity level. \sa getVerbosity(), Debug()
         void setVerbosity(int lvl = 0) 
            { fVerbosity = lvl; }
         //! Returns debug output verbosity level. \sa setVerbosity(), Debug()
         int getVerbosity() 
            { return fVerbosity; }

         //! Set get/put/operGen trace level.
         /*!
          * Controls whether the full context of get(), put() and operGen() 
          * requests is printed to stdout. The \a lvl determines whether all,
          * none or only failed requests are traced:
          * \li 0 no trace at all
          * \li 1 trace if network error (default setting)
          * \li 2 trace if any error
          * \li 3 trace all
          *
          * The output format is like (-> indicates get, <- indicates put):
\verbatim
get   (3,0x00000580 ROC_GPIO_CONFIG            ) -> 0x00000000          0 ok
put   (3,0x00000580 ROC_GPIO_CONFIG            ) <- 0x00000100        256 ok
op[00](3,0x00000514 ROC_SYNC2_BAUD_START       ) <- 0x00000002          2
op[01](3,0x00000518 ROC_SYNC2_BAUD1            ) <- 0x00000004          4
op[02](3,0x0000051c ROC_SYNC2_BAUD2            ) <- 0x00000004          4 ok
put   (3,0x00000500 ROC_SYNC1_M_SCALEDOWN      ) <- 0x00000004          4 ok
op[00](3,0x00000504 ROC_SYNC1_BAUD_START       ) <- 0x00000002          2
op[01](3,0x00000508 ROC_SYNC1_BAUD1            ) <- 0x00000004          4
op[02](3,0x0000050c ROC_SYNC1_BAUD2            ) <- 0x00000004          4 ok
op[00](3,0x00000054 ROC_TESTPULSE_LENGTH       ) <- 0x0000c34f      49999
op[01](3,0x00000058 ROC_TESTPULSE_NUMBER       ) <- 0x00000000          0
op[02](3,0x0000005c ROC_TESTPULSE_START        ) <- 0x00000001          1 ok
op[00](3,0x00010100 ROC_I2C1_SWITCH            ) <- 0x00000000          0
op[01](3,0x0001000c ROC_I2C1_SLAVEADDR         ) <- 0x00000008          8
op[02](3,0x00010010 ROC_I2C1_REGISTER          ) <- 0x00000015         21
op[03](3,0x00010000 ROC_I2C1_DATA              ) -> 0x0000000f         15
op[04](3,0x00010020 ROC_I2C1_ERROR             ) -> 0x00000000          0 ok
\endverbatim
         *  \sa getOperTrace()
         */
         void setOperTrace(int lvl = 1) 
            { fOperTrace = lvl; }
         //! Returns debug output verbosity level. \sa setOperTrace()
         int getOperTrace() 
            { return fOperTrace; }

         //! Set default get/put/oper timeout.
         /*!
          * The value set with this method is used whenever no explicit
          *       timeout is specified in a get(), put() or openGen() call.
          * \param tmout network communication timeout in sec.
          * \sa getDefaultTimeout()
          */
         void setDefaultTimeout(double tmout = 2.) 
                 { fDefaultTimeout = tmout>0 ? tmout : 1.; }

         //! Returns default get/put/oper timeout. \sa setDefaultTimeout()
         double getDefaultTimeout() const { return fDefaultTimeout; }

         //! Sets flush timeout of data transport
         /*!
          * Local transport accumulates data in buffer and until buffer is not filled,
          * data is not delivered further. Flush timeout defines time interval,
          * when any amount of data will be flushed.
          * \param tmout timeout value in sec.
          */
         virtual void setFlushTimeout(double tmout) {}

         //! General operate, arbitrary list of get/put accesses to ROC control space.
         /*!
          * This function allow to execute a list of gets and puts from and to the
          * ROC control address space. It is guaranteed that the execution is an
          * atomic action, no control space access from other sources will be
          * handled concurrently.
          * \param isput array of flags, if \c true a put is done, otherwise a get.
          * \param addr array of ROC control space addresses
          * \param value array of data values. If the corresponding entry in \a isput
          *         is true the array element is read and written into the ROC address,
          *         otherwise the array element is written by data retrieved from the
          *         ROC address.
          * \param num number of operates to do, size of arrays \a isput, \a addr,
          *         and \a value
          * \param tmout maximal network response time
          * \returns see \ref roc_board_opererr
          *
          * \sa operPG(), operPP(), operPPP(), operPPPP()
          */

         virtual int operGen(OperList& lst, double tmout = 0.) = 0;

         virtual int operGen(bool* isput,
                             uint32_t* addr,
                             uint32_t* value,
                             int num,
                             double tmout = 0.);

         int put(uint32_t addr, uint32_t value, double tmout = 0.);
         int get(uint32_t addr, uint32_t& value, double tmout = 0.);

         int operPP(uint32_t addr0, uint32_t  val0p,
                    uint32_t addr1, uint32_t  val1p,
                    double tmout = 0.);
         int operPG(uint32_t addr0, uint32_t  val0p,
                    uint32_t addr1, uint32_t& val1g,
                    double tmout = 0.);
         int operPPP(uint32_t addr0, uint32_t  val0p,
                     uint32_t addr1, uint32_t  val1p,
                     uint32_t addr2, uint32_t  val2p,
                     double tmout = 0.);
         int operGGG(uint32_t addr0, uint32_t&  val0p,
                     uint32_t addr1, uint32_t&  val1p,
                     uint32_t addr2, uint32_t&  val2p,
                     double tmout = 0.);
         int operPPPP(uint32_t addr0, uint32_t  val0p,
                      uint32_t addr1, uint32_t  val1p,
                      uint32_t addr2, uint32_t  val2p,
                      uint32_t addr3, uint32_t  val3p,
                      double tmout = 0.);

         // --------------------------------------------------------------------
         // methods for controlling ROC and components
         // --------------------------------------------------------------------

         static const char* versionToString(uint32_t ver);

         static uint32_t getRoclibVersion();
         uint32_t getRocFpgaType();
         uint32_t getRocSvnRev();
         uint32_t getRocBuildTime();

         uint32_t getRocHardwareVersion();
         uint32_t getRocHardwareType();

         //! Return frontend kinds - kind_nXYTER (1) or kind_FEET (2)
         uint32_t getRocFrontendKind() { return getRocHardwareType() >> 16; }

         //! Return backend kinds - kind_Optic (1), kind_FX20 (2), kind_FX40 (3) or kind_FX60 (4)
         uint32_t getRocBackendKind() { return getRocHardwareType() & 0xffff; }

         //! Returns backend (Optic or UDP) version
         virtual uint32_t getRocBackendVersion() { return 0; }


         //! Returns locally stored ROC number
         uint32_t rocNumber() const { return fRocNumber; }

         //! Returns number from ROC directly, store fRocNumber variable
         uint32_t getRocNumber();
         void setRocNumber(uint32_t);

         int uploadCommandsList(unsigned num, const OperList& lst, double tmout = 0.);

         int downloadCommandsList(unsigned num, OperList& lst, double tmout = 0.);

         int invokeCommandsList(unsigned num, double tmout = 0.);

         int uploadStartDaqCmdList(bool reset_frontend = false, bool reset_fifo = true);

         int uploadStopDaqCmdList();

         virtual int invokeDLM(unsigned num, double tmout = 0.) { return 0; }

         void clearRocFifo();
         void restartRoc();

         int getRocThrottleState(uint32_t& val);

         // DAQ functions

         //! Activate DAQ.
         /*!
         * This method will initialize the data transport chain which brings
         * message data from the ROC. The details depend in the underlying
         * transport. By the daq starting commands list 0 is always executed.
         * This commands list can be initialized by \sa uploadStartDaqCmdList()
         * By default configuration (after \sa setToDefault()) frontend is initialized
         * (in case of nXYTER, timestamp logic initialization) and message
         * FIFO will be cleared.
         *
         * \sa suspendDaq(), stopDaq(), getNextBuffer()
         */
         virtual bool startDaq() = 0;

         //! suspendDaq
         /*!
         * Sends to ROC ROC_SUSPEND_DAQ command. ROC will produce stop message
         * and will deliver it sometime to host PC. User can continue to take data until
         * this message. Normally no more messages will be delivered after stop daq message.
         */
         virtual bool suspendDaq() = 0;

         //! stopDaq
         /*!
         * Complimentary function for startDaq.
         * Disables any new data requests, all coming data will be discarded
         */
         virtual bool stopDaq() = 0;

         //! getNextBuffer
         /*!
         * Returns pointer on the buffer which contains received messages.
         * \param len returns length of buffer in bytes.
         * Buffer is allocated by board itself, buffer content remains valid until next
         * call of getNextBuffer() or when daq is stopped.
         * Format of messages can be defined by \ref getMsgFormat()
         * To extract single messages from the buffer, \ref roc::Iterator class should be used
         * \sa \ref roc::Iterator getMsgFormat()
         */
         virtual bool getNextBuffer(void* &buf, unsigned& len, double tmout = 1.) = 0;

         //! getMsgFormat
         /*!
          * Returns message format like formatEth2, formatOptic2 and so on
          */
         virtual MessageFormat getMsgFormat() const = 0;

         //! getTransportKind
         /*!
          * Returns kind of transport - roc::kind_UDP or roc::kind_ABB
          */
         virtual int getTransportKind() const = 0;
         
         //! isFile
         /*!
          * Returns true if board is just envelope around file reading -
          * most operation like put/get are nit working and should not be tried
          */
         bool isFile() const { return getTransportKind() == roc::kind_File; }

         //! Sets to default values common ROC registers
         /*!
          * For a moment, initializes start/stop commands lists
          */
         virtual int setToDefault();

         //! enableCalibration
         /*!
         * Preliminary functionality for nXYTER calibration
         * Reconfigures nXYTER in test-trigger mode and fires several test pulses
         * \param period can has following values:
         *     0  - make calibration once
         *     >0 - perform calibration periodically
         *     <0 - disable calibration
         */
         virtual bool enableCalibration(double period = 10., double calibr = 0.1, int cnt = -1) { return false; }

         virtual bool disableCalibration() { return enableCalibration(-1., -1., 0); }

         //! Retrieve error code from get/put/oper return code.
         /*!
          * Extracts the error code, in general one of the values specified in
          * the board::operErr enum, from a return code \a rc of a board access
          * method.
          * \sa \ref roc_board_opererr.
          */
         static int operErrCode(int rc)
           { return rc & 0xff; }
         //! Retrieve index from get/put/oper return code
         /*!
          * Extracts the index value from a return code \a rc of a board access.
          * method. The index allows to reconstruct at which point of an
          * access sequence the error occurred.
          * \sa \ref roc_board_opererr.
          */
         static int operErrIndex(int rc)
           { return (rc>>8) & 0xffffff; }

         //! Build get/put/oper return code from error code and index value.
         /*!
          * Build a return code for a board access method from an error
          * code \a code, in general one of the values specified in the
          * board::operErr enum, and an index value \a index.
          * \param code error code. Only the lower 8 bits are used, to
          *   one can use a full return code, the index part will be discarded.
          * \param index index or stage number at the point of failure
          * \sa \ref roc_board_opererr.
          * \sa operErrBuildInc()
          */
         static int operErrBuild(int code, int index)
           { return (code & 0xff) | ((index & 0xffffff)<<8); }

         //! Add to index value of get/put/oper return code.
         /*!
          * \param rc return code from board access method
          * \param inc value to be added to the index field
          * \returns the return code \a rc with the index incremented by \a inc
          * \sa \ref roc_board_opererr.
          * \sa operErrBuild()
          */
         static int operErrBuildInc(int rc, int inc)
           { return operErrBuild(rc,  operErrIndex(rc) + inc); }

         static std::string operErrToString(int rc);

         static const char* operErrCodeName(int rc);

         static Board* Connect(const char* name, ClientRole role = roleDAQ);
         static bool Close(Board* brd);
   };


   class BoardConnector {

      private:
         static BoardConnector* gArr[10];

      public:
         BoardConnector();
         virtual ~BoardConnector();

         static int NumConn() { return 10; }
         static BoardConnector* Conn(int n) { return gArr[n]; }

         //! Checks if name has specified prefix
         /*!
          * Can be used to resolve address name like udp:\\rocname or file:\\filename.
          */
         static const char* CheckNamePrefix(const char* rocname, const char* prefix);
         
         static const char* CheckOptic(const char* name);
         static const char* CheckUdp(const char* name);
         static const char* CheckFile(const char* name);

         virtual Board* DoConnect(const char* name, ClientRole role = roleDAQ) = 0;
         virtual bool DoClose(Board* brd) = 0;
   };

}

#endif
