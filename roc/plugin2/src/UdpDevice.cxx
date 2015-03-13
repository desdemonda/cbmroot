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

#include "roc/UdpDevice.h"

#include <iostream>
#include <fstream>

#include "dabc/Manager.h"
#include "dabc/Port.h"

#include "roc/defines_roc.h"
#include "roc/defines_udp.h"
#include "roc/Commands.h"
#include "roc/OperList.h"

#include "roc/ReadoutModule.h"
#include "roc/NxCalibrModule.h"


const char* ReadoutModuleName = "RocReadout";
const char* CalibrModuleName = "RocNxCalibr";


roc::UdpControlSocket::UdpControlSocket(UdpDevice* dev, int fd) :
   dabc::SocketWorker(fd),
   fDev(dev),
   fUdpCmds(dabc::CommandsQueue::kindSubmit),
   fCtrlRuns(false),
   fControlSendSize(0),
   fPacketCounter(1),
   fSendLastOper()
{
   // we will react on all input packets
   SetDoingInput(true);

   SetLastSendTime();
}

//----------------------------------------------------------------------------

roc::UdpControlSocket::~UdpControlSocket()
{
   fUdpCmds.Cleanup();

   if (fDev) fDev->fCtrlCh = 0;
   fDev = 0;
}


void roc::UdpControlSocket::SetLastSendTime()
{
   fSendLastOper.GetNow();
}

//----------------------------------------------------------------------------

void roc::UdpControlSocket::ProcessEvent(const dabc::EventId& evnt)
{
   switch (evnt.GetCode()) {

      case evntSendCtrl: {
         DOUT5(("Send requests of size %d tag %d", fControlSendSize, fControlSend.tag));

         DoSendBuffer(&fControlSend, fControlSendSize);
         double tmout = fFastMode ? (fLoopCnt++ < 4 ? 0.01 : fLoopCnt*0.1) : 1.;
         fTotalTmoutSec -= tmout;
         SetLastSendTime();
         ActivateTimeout(tmout);
         break;
      }

      case evntSocketRead: {

         // this is required for DABC 2.0 to let thread again generate socket-read event
         SetDoingInput(true);

         ssize_t len = DoRecvBuffer(&fControlRecv, sizeof(UdpMessageFull));

//         DOUT0(("Get answer of size %d req %d answ %d", len, ntohl(fControlSend.id), ntohl(fControlRecv.id)));

         if (len<=0) return;

         if (fCtrlRuns &&
             (fControlRecv.password == htonl(ROC_PASSWORD)) &&
             (fControlSend.id == fControlRecv.id) &&
             (fControlSend.tag == fControlRecv.tag)) {
                completeLoop(true, len);
         } else
         if (fDev)
            fDev->processCtrlMessage(&fControlRecv, len);
         break;
      }

      case evntCheckCmd: {
         checkCommandsQueue();
         break;
      }

      default:
         dabc::SocketWorker::ProcessEvent(evnt);
         break;
   }
}

//----------------------------------------------------------------------------
void roc::UdpControlSocket::SendDisconnect()
{
   fControlSend.tag = ROC_POKE;
   fControlSend.address = htonl(ROC_ETH_MASTER_LOGOUT);
   fControlSend.value = htonl(1);
   fControlSendSize = sizeof(UdpMessage);

   DoSendBuffer(&fControlSend, fControlSendSize);
}

//----------------------------------------------------------------------------

double roc::UdpControlSocket::ProcessTimeout(double last_diff)
{
   if (!fCtrlRuns) {
      if (fSendLastOper.Expired(2.))
         if (fUdpCmds.Size()==0) {
            Submit(CmdGet(ROC_ROCID, 1.));
            // DOUT0(("Submit keep alive command"));
         }

      return 1.;
   }

   if (fTotalTmoutSec <= 0.) {
      // stop doing;
      completeLoop(false);
      return 1.;
   }

   if (fFastMode || (fTotalTmoutSec <=5.)) {
      FireEvent(evntSendCtrl);
      return 1.;
   }

   fTotalTmoutSec -= 1.;

   return 1.;
}

void roc::UdpControlSocket::completeLoop(bool res, int)
{
   bool finished = true;
   dabc::Command cmd = fUdpCmds.Front();
   if (cmd.null()) {
      EOUT(("Something wrong"));
   } else
   if (res) {
      if (cmd.IsName(CmdGet::CmdName())) {
         cmd.SetUInt(ValuePar, res ? ntohl(fControlRecv.value) : 0);
         cmd.SetUInt(ErrNoPar, res ? ntohl(fControlRecv.address) : roc::Board::kOperNetworkErr);
      } else
      if (cmd.IsName(CmdPut::CmdName()) ||
          cmd.IsName(roc::UdpDevice::CmdPutSuspendDaq()) ||
          cmd.IsName(roc::UdpDevice::CmdPutDisconnect()) ||
          cmd.IsName(CmdDLM::CmdName())) {
         unsigned errno = res ? ntohl(fControlRecv.value) : roc::Board::kOperNetworkErr;
         cmd.SetUInt(ErrNoPar, errno);
         if (cmd.GetBool("SwPut", false))
            fDev->completeSwPut(errno==0);
      } else
      if (cmd.IsName(CmdNOper::CmdName())) {
         uint32_t errcode = res ? ntohl(fControlRecv.value) : roc::Board::kOperNetworkErr;

         OperList* lst = (OperList*) cmd.GetPtr(OperListPar, 0);

         int first = cmd.GetInt("FirstIndx", 0);
         int nsend = lst->number() - first;
         int max = sizeof(fControlSend.rawdata) / 2 / sizeof(uint32_t);
         if (nsend>max) nsend = max;

         if (nsend != (int) ntohl(fControlRecv.address))
            errcode = roc::Board::kOperNetworkErr;
         else {
            uint32_t* src = (uint32_t*) fControlRecv.rawdata;
            for (int n=first;n<first+nsend;n++) {
               uint32_t res_addr = ntohl(*src++);
               if (lst->oper(n).addr != (res_addr & NOPER_ADDR_MASK)) {
                  if (errcode==0) {
                     errcode = roc::Board::operErrBuild(roc::Board::kOperNetworkErr, n);
                     EOUT(("Source %x and target %x address mismatch", lst->oper(n).addr, res_addr));
                  }
               } else {
                  uint32_t res_value = ntohl(*src++);
                  if (!lst->oper(n).isput)
                     lst->oper(n).value = res_value;
                  else
                  if (lst->oper(n).value != res_value) {
                     EOUT(("Initial %x and obtained %x value mismatch", lst->oper(n).value, res_value));
                     errcode = roc::Board::operErrBuild(roc::Board::kOperNetworkErr, n);
                  }
               }
            }
         }

         // if list was too large, send next buffer
         if ((errcode==0) && (first+nsend<lst->number())) {
            finished = false;
            cmd.SetInt("FirstIndx", first+nsend);
            DOUT3(("Very long NOper list, continue with index %d", first+nsend));
         } else
            cmd.SetUInt(ErrNoPar, errcode);

         if (errcode!=0) DOUT0(("NOper err = %d res = %s", errcode, DBOOL(res)));
      }
   }

   if (finished) {
      fUdpCmds.Pop();
      cmd.ReplyBool(res);
   }

   fCtrlRuns = false;

//   ActivateTimeout(-1.);

   checkCommandsQueue();
}

//----------------------------------------------------------------------------

int roc::UdpControlSocket::ExecuteCommand(dabc::Command cmd)
{
   fUdpCmds.Push(cmd);
   checkCommandsQueue();
   return dabc::cmd_postponed;
}

//----------------------------------------------------------------------------

void roc::UdpControlSocket::checkCommandsQueue()
{
   if (fCtrlRuns) return;

   if (fUdpCmds.Size()==0) return;

   dabc::Command cmd = fUdpCmds.Front();

   bool cmdfails = false;
   fTotalTmoutSec = cmd.GetDouble(TmoutPar, 5.0);

   if (cmd.IsName(CmdPut::CmdName()) ||
       cmd.IsName(UdpDevice::CmdPutSuspendDaq()) ||
       cmd.IsName(UdpDevice::CmdPutDisconnect()) ||
       cmd.IsName(CmdDLM::CmdName())) {

       uint32_t addr = cmd.GetUInt(AddrPar, 0);
       uint32_t value = cmd.GetUInt(ValuePar, 0);

       if (cmd.IsName(CmdDLM::CmdName())) {
          value = addr;
          addr = ROC_ETH_DLM;
          DOUT3(("Activate SOFT-DLM %u", value));
       }

       switch (addr) {
          case ROC_ETH_CFG_WRITE:
          case ROC_ETH_CFG_READ:
          case ROC_ETH_OVERWRITE_SD_FILE:
          case ROC_ETH_FLASH_KIBFILE_FROM_DDR:
             if (fTotalTmoutSec < 10) fTotalTmoutSec = 10.;
             break;
       }

       fControlSend.tag = ROC_POKE;
       fControlSend.address = htonl(addr);
       fControlSend.value = htonl(value);

       fControlSendSize = sizeof(UdpMessage);

       void* ptr = cmd.GetPtr(RawDataPar, 0);
       if (ptr) {
          memcpy(fControlSend.rawdata, ptr, value);
          fControlSendSize += value;
       }
   } else
   if (cmd.IsName(CmdGet::CmdName())) {
      fControlSend.tag = ROC_PEEK;
      fControlSend.address = htonl(cmd.GetUInt(AddrPar, 0));
      fControlSendSize = sizeof(UdpMessage);
   } else
   if (cmd.IsName(CmdNOper::CmdName())) {
      fControlSend.tag = ROC_NOPER;
      fControlSend.address = 0;
      fControlSend.value = 0;

      fControlSendSize = sizeof(UdpMessage);

      OperList* lst = (OperList*) cmd.GetPtr(OperListPar, 0);

      if (lst==0) {
         cmdfails = true;
      } else {
         int first = cmd.GetInt("FirstIndx", 0);
         int nsend = lst->number() - first;
         int max = sizeof(fControlSend.rawdata) / 2 / sizeof(uint32_t);
         if (nsend>max) {
            nsend = max;
            fControlSend.value = htonl(1); // LOCK control channel for this node
         }

         fControlSend.address = htonl(nsend);
         fControlSendSize += nsend * 2 * sizeof(uint32_t);
         uint32_t* tgt = (uint32_t*) fControlSend.rawdata;

         for (int n=first;n<first+nsend;n++) {
            uint32_t mask = lst->oper(n).isput ? NOPER_PUT : NOPER_GET;
            *tgt++ = htonl(lst->oper(n).addr | mask);
            *tgt++ = htonl(lst->oper(n).value);
         }

         DOUT3(("Sending NOper of num = %d, size = %d failes %s", nsend, fControlSendSize, DBOOL(cmdfails)));
      }
   } else {
      cmdfails = true;
   }

   if (cmdfails) {
      fUdpCmds.Pop().ReplyFalse();
      FireEvent(evntCheckCmd);
      return;
   }

   fControlSend.password = htonl(ROC_PASSWORD);
   fControlSend.id = htonl(fPacketCounter++);

   // send aligned to 4 bytes packet to the ROC

   while ((fControlSendSize < MAX_UDP_PAYLOAD) &&
          (fControlSendSize + UDP_PAYLOAD_OFFSET) % 4) fControlSendSize++;

   // in fast mode we will try to resend as fast as possible
   fFastMode = (fTotalTmoutSec < 10.);
   fLoopCnt = 0;

   fCtrlRuns = true;

   FireEvent(evntSendCtrl);
}

//----------------------------------------------------------------------------

roc::UdpDevice::UdpDevice(const char* name, const char* thrdname, dabc::Command cmd) :
   dabc::Device(name),
   roc::UdpBoard(),
   fConnected(false),
   fFormat(formatEth1),
   fRocIp(),
   fRocCtrlPort(ROC_DEFAULT_CTRLPORT),
   fRocDataPort(ROC_DEFAULT_DATAPORT),
   fCtrlPort(0),
   fCtrlCh(0),
   fDataPort(0),
   fDataFD(0),
   fDataCh(0),
   isBrdStat(false),
   displayConsoleOutput_(false),
   fSwMutex(),
   fSwCmdState(0)
{
   fRocIp = Cfg(roc::xmlBoardAddr, cmd).AsStdStr();
   if (fRocIp.length()==0) return;
   size_t separ = fRocIp.find(':');
   if ((separ>0) && (separ!=std::string::npos)) {
      fRocCtrlPort = atoi(fRocIp.c_str()+separ+1);
      fRocIp.resize(separ);
      if (fRocCtrlPort==0)
         fRocCtrlPort = ROC_DEFAULT_CTRLPORT;
      else
         fRocDataPort = fRocCtrlPort - 1;
   }

   if (!dabc::mgr()->MakeThreadFor(this, thrdname)) return;

   int nport = 8725;

   int fd = dabc::SocketThread::StartUdp(nport, nport, nport+1000);

   fd = dabc::SocketThread::ConnectUdp(fd, fRocIp.c_str(), fRocCtrlPort);

   if (fd<=0) return;

   fCtrlCh = new UdpControlSocket(this, fd);

   fCtrlCh->AssignToThread(thread());

   DOUT2(("Create control socket %d for port %d connected to host %s", fd, nport, fRocIp.c_str()));

   fCtrlPort = nport;
   fConnected = true;

   std::string srole = Cfg(roc::xmlRole, cmd).AsStdStr(roleToString(roc::roleDAQ));

   ClientRole role = defineClientRole(srole.c_str(), roc::roleDAQ);

   if (role==roleNone) role = roleDAQ;

   setRole(role);

   if (!initUdp()) fConnected = false;
}

//----------------------------------------------------------------------------

roc::UdpDevice::~UdpDevice()
{
   DOUT3(("Destroy roc::UdpDevice %s", GetName()));

   if (fDataFD!=0) {
      close(fDataFD);
      fDataFD = 0;
   }

   if (fDataCh) {
      // EOUT(("Data channel not yet destroyed !!!"));
      fDataCh->fDev = 0;
      // transport should be destroyed by port
      // dabc::Object::Destroy(fDataCh);
      fDataCh = 0;
   }

   if (fCtrlCh) {
      if (fConnected  && (getRole() != roleObserver)) fCtrlCh->SendDisconnect();

      fCtrlCh->fDev = 0;
      dabc::Object::Destroy(fCtrlCh);
      fCtrlCh = 0;
   }

   DOUT3(("Destroy roc::UdpDevice %s done", GetName()));
}

//----------------------------------------------------------------------------

int roc::UdpDevice::ExecuteCommand(dabc::Command cmd)
{
   if (cmd.IsName(CmdGet::CmdName()) ||
       cmd.IsName(CmdPut::CmdName()) ||
       cmd.IsName(CmdNOper::CmdName()) ||
       cmd.IsName(CmdDLM::CmdName())) {
      if (fCtrlCh==0) return dabc::cmd_false;
      fCtrlCh->Submit(cmd);
      return dabc::cmd_postponed;
   } else
   if (cmd.IsName(roc::UdpDevice::CmdPutSuspendDaq())) {
      if ((fCtrlCh==0) || (fDataCh==0)) return dabc::cmd_false;

      if (!fDataCh->prepareForSuspend()) return dabc::cmd_false;

      cmd.SetUInt(AddrPar, ROC_ETH_SUSPEND_DAQ);
      cmd.SetUInt(ValuePar, 1);
      fCtrlCh->Submit(cmd);

      return dabc::cmd_postponed;
   } else
   if (cmd.IsName(roc::UdpDevice::CmdPutDisconnect())) {
      if (fCtrlCh==0) return dabc::cmd_false;

      if (getRole() == roleObserver) return dabc::cmd_true;

      fConnected = false;

      cmd.SetUInt(AddrPar, ROC_ETH_MASTER_LOGOUT);
      cmd.SetUInt(ValuePar, 1);
      fCtrlCh->Submit(cmd);

      return dabc::cmd_postponed;
   } else
   if (cmd.IsName("SetFlushTimeout")) {
      if (fDataCh==0) {
         EOUT(("No data channel - cannot setup flush timeout"));
         return dabc::cmd_false;
      }

      fDataCh->setFlushTimeout(cmd.GetDouble("Timeout", 0.1));

      return dabc::cmd_true;
   } else
   if (cmd.IsName("enableCalibration")) {
      dabc::mgr.FindModule(CalibrModuleName).Submit(cmd);
      return dabc::cmd_postponed;
   } else
   if (cmd.IsName(CmdGetBoardPtr::CmdName())) {
      cmd.SetPtr(CmdGetBoardPtr::Board(), (roc::Board*)this);
      return dabc::cmd_true;
   } else
   if (cmd.IsName(CmdReturnBoardPtr::CmdName())) {
      return dabc::cmd_true;
   }

   return dabc::Device::ExecuteCommand(cmd);
}

//----------------------------------------------------------------------------

bool roc::UdpDevice::initUdp()
{
   DOUT2(("Starting UdpDevice::initialize"));

   if (fCtrlCh==0) return false;

   int logincode = 99;
   if (getRole() == roleDAQ) logincode = 0; else
   if (getRole() == roleControl) logincode = 1;

   if (rawPut(ROC_ETH_MASTER_LOGIN, logincode)!=0) return false;

   DOUT2(("Put login %d OK ",logincode));

   if (getRole() == roleDAQ) {

      int nport = fCtrlPort + 1;

      int fd = dabc::SocketThread::StartUdp(nport, nport, nport+1000);

      fd = dabc::SocketThread::ConnectUdp(fd, fRocIp.c_str(), fRocDataPort);

      if (fd<=0) return false;

      if (put(ROC_ETH_STOP_DAQ, 1)!=0) {
         EOUT(("Cannot stop daq"));
         close(fd);
         return false;
      }

      if (put(ROC_ETH_MASTER_DATAPORT, nport)!=0) {
         EOUT(("Cannot assign to ROC data port number"));
         close(fd);
         return false;
      }

      fDataPort = nport;
      fDataFD = fd;

      DOUT2(("Create data socket %d for port %d connected to host %s", fd, nport, fRocIp.c_str()));
   }

   uint32_t roc_hw_ver = initBoard();

   fFormat = (roc_hw_ver < 0x01090000) ? formatEth1 : formatEth2;

   OperList lst;
   lst.addGet(ROC_ETH_HWV);
   lst.addGet(ROC_ETH_SWV);
   if (operGen(lst)!=0) return false;
   uint32_t eth_hw_ver  = lst.oper(0).value;
   uint32_t eth_sw_ver  = lst.oper(1).value;

   DOUT0(("ROC%u Eth HW version: %s", fRocNumber, versionToString(eth_hw_ver)));

   DOUT0(("ROC%u Eth SW version: %s", fRocNumber, versionToString(eth_sw_ver)));

   if(eth_sw_ver < 0x02000000) {
      EOUT(("The ROC you want to access has software version 0x%08x", eth_sw_ver));
      EOUT(("This C++ access class only supports boards with major version 2.0 == 0x%08x", 0x02000000));
   }

   if ((eth_sw_ver & 0xFFFF0000) != (roc_hw_ver & 0xFFFF0000))
      EOUT(("Mismatch of major number of hardware and software version"));

   return (eth_sw_ver!=0) && (roc_hw_ver!=0);
}

//----------------------------------------------------------------------------

dabc::Transport* roc::UdpDevice::CreateTransport(dabc::Command cmd, dabc::Reference port)
{
   DOUT3(("UdpDevice creates transport for port %p ch %p", port(), fDataCh));

   if (fDataCh!=0) return 0;

   fDataCh = new UdpDataSocket(this, port, fDataFD, fFormat);
   fDataFD = 0;

   return fDataCh;
}

//----------------------------------------------------------------------------

void roc::UdpDevice::setFlushTimeout(double tmout)
{
   dabc::Command cmd("SetFlushTimeout");
   cmd.SetDouble("Timeout", tmout);
   Execute(cmd);
}

//----------------------------------------------------------------------------

int roc::UdpDevice::operGen(OperList& lst, double tmout)
{
   int res = kHostError;

   if (fCtrlCh!=0) {
      if (tmout <= 0.) tmout = getDefaultTimeout();

      CmdNOper cmd(&lst, tmout);
      cmd.SetTimeout(tmout + .1);
      if (fCtrlCh->Execute(cmd))
         res = cmd.GetInt(ErrNoPar, kHostError);
   }

   traceOper(&lst, res);

   return res;
}

//----------------------------------------------------------------------------

void roc::UdpDevice::processCtrlMessage(UdpMessageFull* pkt, unsigned len)
{
   // process PEEK or POKE reply messages from ROC

   if(pkt->tag == ROC_CONSOLE) {
      pkt->address = ntohl(pkt->address);

      switch (pkt->address) {
         case ROC_ETH_STATBLOCK:
            setBoardStat(pkt->rawdata, displayConsoleOutput_);
            break;

         case ROC_ETH_DEBUGMSG:
            if (displayConsoleOutput_)
               DOUT0(("\033[0;%dm Roc:%d %s \033[0m", 36, 0, (const char*) pkt->rawdata));
            break;
         default:
            if (displayConsoleOutput_)
               DOUT0(("Error addr 0x%04x in console message", pkt->address));
      }
   }
}

//----------------------------------------------------------------------------

void roc::UdpDevice::setBoardStat(void* rawdata, bool print)
{
   BoardStatistic* src = (BoardStatistic*) rawdata;

   if (src!=0) {
      brdStat.dataRate = ntohl(src->dataRate);
      brdStat.sendRate = ntohl(src->sendRate);
      brdStat.recvRate = ntohl(src->recvRate);
      brdStat.nopRate = ntohl(src->nopRate);
      brdStat.frameRate = ntohl(src->frameRate);
      brdStat.takePerf = ntohl(src->takePerf);
      brdStat.dispPerf = ntohl(src->dispPerf);
      brdStat.sendPerf = ntohl(src->sendPerf);
      brdStat.daqState = ntohl(src->daqState);
      isBrdStat = true;
   }

   if (print && isBrdStat)
      DOUT0(("\033[0;%dm Roc:%u  Data:%6.3f MB/s Send:%6.3f MB/s Recv:%6.3f MB/s NOPs:%2u Frames:%2u Data:%4.1f%s Disp:%4.1f%s Send:%4.1f%s \033[0m",
         36, 0, brdStat.dataRate*1e-6, brdStat.sendRate*1e-6, brdStat.recvRate*1e-6,
             brdStat.nopRate, brdStat.frameRate,
             brdStat.takePerf*1e-3,"%", brdStat.dispPerf*1e-3,"%", brdStat.sendPerf*1e-3,"%"));
}

//----------------------------------------------------------------------------

roc::BoardStatistic* roc::UdpDevice::takeStat(double tmout, bool print)
{
   void *rawdata = 0;

   if (tmout > 0.) {

      CmdGet cmd(ROC_ETH_STATBLOCK, tmout);
      cmd.SetTimeout(tmout + 1.);

      if (!fCtrlCh->Execute(cmd)) return 0;

      rawdata = fCtrlCh->fControlRecv.rawdata;
   }

   setBoardStat(rawdata, print);

   return isBrdStat ? &brdStat : 0;
}

//----------------------------------------------------------------------------

int roc::UdpDevice::rawPut(uint32_t address, uint32_t value, const void* rawdata, double tmout)
{
   if (fCtrlCh==0) return kHostError;

   if (tmout <= 0.) tmout = getDefaultTimeout();

   CmdPut cmd(address, value, tmout);
   cmd.SetPtr(RawDataPar, (void*) rawdata);

   int res = kOperNetworkErr;

   if (fCtrlCh->Execute(cmd, tmout + 1.))
      res = cmd.GetInt(ErrNoPar, kHostError);

   DOUT2(("Roc:%s rawPut(0x%04x, 0x%04x) res = %d",
          GetName(), address, value, res));

   return res;
}

//----------------------------------------------------------------------------

bool roc::UdpDevice::startDaq()
{
   DOUT2(("Starting DAQ !!!!"));

   dabc::ModuleRef m = dabc::mgr.FindModule(ReadoutModuleName);

   if (m.null()) return false;

   m.Start();

   return true;
}

//----------------------------------------------------------------------------

bool roc::UdpDevice::suspendDaq()
{
   DOUT2(("Suspend DAQ !!!!"));

   if (!Execute(roc::UdpDevice::CmdPutSuspendDaq(), 5.)) return false;

   return true;
}

//----------------------------------------------------------------------------

bool roc::UdpDevice::stopDaq()
{
   DOUT2(("Stop DAQ !!!!"));

   dabc::ModuleRef m = dabc::mgr.FindModule(ReadoutModuleName);

   if (m.null()) return false;

   m.Stop();

   return true;
}

//----------------------------------------------------------------------------

void roc::UdpDevice::ShowDebug(int lvl, const char* msg)
{
   if (lvl<0) EOUT((msg));
         else DOUT0((msg));
}

//----------------------------------------------------------------------------

bool roc::UdpDevice::getNextBuffer(void* &buf, unsigned& len, double tmout)
{
   dabc::ModuleRef m = dabc::mgr.FindModule(ReadoutModuleName);
   if (m.null()) return false;

   return ((ReadoutModule*) m())->getNextBuffer(buf, len, tmout);
}

bool roc::UdpDevice::enableCalibration(double period, double calibr, int cnt)
{
   dabc::ModuleRef m = dabc::mgr.FindModule(CalibrModuleName);

   if ((period<0) && m.null()) return true;

   if (m.null()) {
      DOUT0(("Create calibr module"));
      dabc::mgr.CreateModule("roc::NxCalibrModule", CalibrModuleName, "NxCalibrThrd");
      dabc::mgr.StartModule(CalibrModuleName);
   }

   dabc::Command cmd("enableCalibration");
   if ((period<0.) || (calibr<0.)) cnt = 0;
   cmd.SetDouble("WorkPeriod", period);
   cmd.SetDouble("CalibrPeriod", calibr);
   cmd.SetInt("CalibrLoops", cnt);
   cmd.SetReceiver(CalibrModuleName);

   return dabc::mgr.Execute(cmd);
}


bool roc::UdpDevice::InitAsBoard()
{
   // called from main thread

   if (getRole() == roleDAQ) {

      DOUT3(("Create readout module"));

      dabc::mgr.CreateThread("ReadoutThrd", dabc::typeSocketThread);

      dabc::mgr.CreateMemoryPool(roc::xmlRocPool, 16384, 100, 2);

      dabc::mgr.CreateModule("roc::ReadoutModule", ReadoutModuleName, "ReadoutThrd");

      DOUT3(("Connect readout module"));

      if (!dabc::mgr.CreateTransport(dabc::format("%s/Input",ReadoutModuleName), ItemName(), "ReadoutThrd")) {
         EOUT(("Cannot connect readout module to device %s", ItemName().c_str()));
         dabc::mgr.DeleteModule(ReadoutModuleName);
         return false;
      }
   }

   DOUT3(("InitAsBoard done"));

   return true;
}


bool roc::UdpDevice::CloseAsBoard()
{
   // called from main thread

   DOUT3(("UdpDevice::CloseAsBoard Close readout"));

   dabc::mgr.StopModule(ReadoutModuleName);
   dabc::mgr.DeleteModule(ReadoutModuleName);

   DOUT3(("UdpDevice::CloseAsBoard   Close calibration"));

   disableCalibration();
   dabc::mgr.StopModule(CalibrModuleName);
   dabc::mgr.DeleteModule(CalibrModuleName);

   if (dabc::mgr()) dabc::mgr()->FindThread("ReadoutThrd").Destroy();

   DOUT3(("UdpDevice::CloseAsBoard  done"));

   return true;
}

bool roc::UdpDevice::submitSwPut(uint32_t address, uint32_t value, double tmout)
{
   CmdPut cmd(address, value, tmout);
   cmd.SetBool("SwPut", true);

   fCtrlCh->Submit(cmd);

   dabc::LockGuard lock(fSwMutex);
   if (fSwCmdState!=0) EOUT(("Last command not completed!!!"));

   fSwCmdState = 1;

   return true;
}

int roc::UdpDevice::checkSwPut()
{
   dabc::LockGuard lock(fSwMutex);

   int res = fSwCmdState;
   if (fSwCmdState!=1) fSwCmdState = 0;
   return res;
}

void roc::UdpDevice::completeSwPut(bool res)
{
   dabc::LockGuard lock(fSwMutex);
   fSwCmdState = res ? 2 : -1;
}
