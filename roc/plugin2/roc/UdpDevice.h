/********************************************************************
 * The Data Acquisition Backbone Core (DABC)
 ********************************************************************
 * Copyright (C) 2009-
 * GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
 * Planckstr. 1
 * 64291 Darmstadt
 * Germany
 * Contact:  http://dabc.gsi.de
 ********************************************************************
 * This software can be used under the GPL license agreements as stated
 * in LICENSE.txt file which is part of the distribution.
 ********************************************************************/

#ifndef ROC_UDPDEVICE_H
#define ROC_UDPDEVICE_H

#ifndef DABC_Device
#include "dabc/Device.h"
#endif

#ifndef DABC_SocketThread
#include "dabc/SocketThread.h"
#endif

#include "roc/UdpBoard.h"
#include "roc/UdpTransport.h"

namespace roc {

   class UdpDevice;

   // TODO: in DABC2 control socket should be inherited from dabc::SocketWorker
   //       or one should reorganize event loop to inherit from dabc::SocketIOWorker

   class UdpControlSocket : public dabc::SocketWorker {
      friend class UdpDevice;
      friend class UdpDataSocket;
      protected:

         enum EUdpEvents { evntSendCtrl = evntSocketLast + 1,
                           evntCheckCmd };

         UdpDevice*      fDev;

         dabc::CommandsQueue  fUdpCmds;

         bool            fCtrlRuns;
         UdpMessageFull  fControlSend;
         unsigned        fControlSendSize;
         UdpMessageFull  fControlRecv;

         uint32_t        fPacketCounter;

         double          fTotalTmoutSec;
         bool            fFastMode;
         int             fLoopCnt;

         dabc::TimeStamp fSendLastOper;

         void            checkCommandsQueue();

         void            completeLoop(bool res, int len = 0);

         void            SendDisconnect();

         void            SetLastSendTime();

      public:
         UdpControlSocket(UdpDevice* dev, int fd);
         virtual ~UdpControlSocket();

         virtual double ProcessTimeout(double last_diff);

         virtual void ProcessEvent(const dabc::EventId&);

         virtual int ExecuteCommand(dabc::Command cmd);
   };


   class UdpDevice : public dabc::Device,
                     public roc::UdpBoard {

      enum EUdpDeviceEvents { eventCheckUdpCmds = eventDeviceLast,
                              eventUdpDeviceLast };

      friend class UdpControlSocket;
      friend class UdpDataSocket;

      protected:
         bool              fConnected;

         MessageFormat     fFormat;

         std::string       fRocIp;
         int               fRocCtrlPort;   // ROC port number, used for control communication
         int               fRocDataPort;   // ROC port number, used for data transport

         int               fCtrlPort;  // local port, used for control communication
         UdpControlSocket *fCtrlCh;

         int               fDataPort; // local port number, used for data transport
         int               fDataFD;   // socket descriptor, kept temporary in this variable
         UdpDataSocket    *fDataCh;   // created data channel

         BoardStatistic    brdStat;    // last available statistic block
         bool              isBrdStat;  // is block statistic contains valid data

         bool              displayConsoleOutput_; // display output, coming from ROC

         dabc::Mutex       fSwMutex;
         int               fSwCmdState;

         void processCtrlMessage(UdpMessageFull* pkt, unsigned len);
         void setBoardStat(void* rawdata, bool print);

         virtual int rawPut(uint32_t address, uint32_t value, const void* rawdata = 0, double tmout = 0.);

         bool initUdp();

         virtual int ExecuteCommand(dabc::Command cmd);

         virtual void ShowDebug(int lvl, const char* msg);

         virtual bool submitSwPut(uint32_t address, uint32_t value, double tmout);
         virtual int checkSwPut();
         void completeSwPut(bool res);

      public:

         UdpDevice(const char* name, const char* thrdname, dabc::Command cmd);
         virtual ~UdpDevice();

         bool IsConnected() const { return fConnected; }

         virtual const char* ClassName() const { return typeUdpDevice; }

         virtual const char* RequiredThrdClass() const { return dabc::typeSocketThread; }

         virtual dabc::Transport* CreateTransport(dabc::Command cmd, dabc::Reference port);

         bool InitAsBoard();
         bool CloseAsBoard();

         virtual void setFlushTimeout(double tmout);

         // put/get interface part of roc::Board
         virtual int operGen(OperList& lst, double tmout = 0.);

         // daq interface part of roc::Board
         virtual bool startDaq();
         virtual bool suspendDaq();
         virtual bool stopDaq();
         virtual bool getNextBuffer(void* &buf, unsigned& len, double tmout = 1.);

         virtual MessageFormat getMsgFormat() const { return fFormat; }

         virtual bool enableCalibration(double period = 10., double calibr = 1., int cnt = -1);

         // interface part of roc::UdpBoard
         BoardStatistic* takeStat(double tmout = 0.01, bool print = false);

         static const char* CmdPutSuspendDaq() { return "PutSuspendDaq"; }
         static const char* CmdPutDisconnect() { return "PutDisconnect"; }
   };

   class UdpDeviceRef : public dabc::DeviceRef {
      DABC_REFERENCE(UdpDeviceRef, dabc::DeviceRef, UdpDevice)

   };


}

#endif
