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
#include "roc/ReadoutApplication.h"

#include <time.h>

#include "dabc/Parameter.h"
#include "dabc/Command.h"
#include "dabc/timing.h"
#include "dabc/CommandsSet.h"
#include "dabc/Device.h"
#include "dabc/Port.h"
#include "dabc/Module.h"
#include "dabc/ConnectionRequest.h"
#include "dabc/Url.h"

#include "mbs/MbsTypeDefs.h"
#include "mbs/Factory.h"

#include "roc/defines_roc.h"
#include "roc/UdpBoard.h"
#include "roc/UdpDevice.h"
#include "roc/Commands.h"
#include "roc/OperList.h"
#include "roc/CombinerModule.h"

const char* nameRocComb = "RocComb";
const char* nameRocCalibr = "RocCalibr";
const char* nameSusiboComb = "SusiboComb";
const char* nameMbsComb = "MbsComb";
const char* nameSuperComb = "SuperComb";

roc::ReadoutApplication::ReadoutApplication() :
   dabc::Application(roc::xmlReadoutAppClass),
   fRocBrds()
{
   CreatePar(roc::xmlIgnoreMissingEpoch).DfltBool(false);

   CreatePar(roc::xmlSyncNumber).DfltInt(0);

   CreatePar(roc::xmlSyncScaleDown).DfltInt(-1);

   CreatePar(roc::xmlNumRocs).DfltInt(3);

   for (int nr = 0; nr < NumRocs(); nr++) {
      CreatePar(FORMAT(("%s%d", xmlRocIp, nr)));
      CreatePar(FORMAT(("%s%d", xmlRocFebs, nr)));
   }
   CreatePar(roc::xmlMeasureADC).DfltBool(false);
   CreatePar(roc::xmlUseDLM).DfltBool(true);

   CreatePar(roc::xmlEpicsStreamNode).DfltStr("");

   CreatePar(roc::xmlNumMbs).DfltInt(0);
   for (int n = 0; n < NumMbs(); n++) {
      CreatePar(FORMAT(("%s%d", roc::xmlMbsAddr, n))).DfltStr("");
      CreatePar(FORMAT(("%s%d", roc::xmlSyncSubeventId, n))).DfltInt(0);
   }

   CreatePar(roc::xmlNumSusibo).DfltInt(0);
   for (int n = 0; n < NumSusibo(); n++)
      CreatePar(FORMAT(("%s%d", roc::xmlSusiboAddr, n))).DfltInt(0);

   CreatePar(roc::xmlNumSlaves).DfltInt(0);
   for (int n = 0; n < NumSlaves(); n++) {
      CreatePar(FORMAT(("%s%d", roc::xmlSlaveAddr, n))).DfltStr("");
      CreatePar(FORMAT(("Slave%s%d", roc::xmlSyncSubeventId, n))).DfltInt(0);
    }

   CreatePar("EventsInBuffer").DfltInt(1);
   CreatePar("HitDelay").DfltInt(200);
   CreatePar("PollingTimeout").DfltDouble(0.01);

   CreatePar(dabc::xmlBufferSize).DfltInt(8192);
   CreatePar(dabc::xmlNumBuffers).DfltInt(100);
   CreatePar(dabc::xmlFlushTimeout).DfltDouble(1.);
   CreatePar(dabc::xmlFlushTimeout).DfltDouble(1.);

   CreatePar(mbs::xmlServerKind).DfltStr(mbs::ServerKindToStr(mbs::StreamServer));
   CreatePar(mbs::xmlCombineCompleteOnly).DfltBool(false);

   CreatePar(xmlRawFile).DfltStr("");
   CreatePar(mbs::xmlSizeLimit).DfltInt(0);

   CreatePar(roc::xmlSpillRoc).DfltInt(-1);
   CreatePar(roc::xmlSpillAux).DfltInt(-1);
   CreatePar(roc::xmlCalibrationPeriod).DfltDouble(-1.);
   CreatePar(roc::xmlCalibrationLength).DfltDouble(0.5);

   CreatePar(roc::xmlGet4ResetPeriod).DfltDouble(-1.);
   CreatePar(roc::xmlGet4ResetLimit).DfltDouble(-1.);

   fMasterNode = Cfg("IsMaster").AsBool(true);

   if (fMasterNode) {
      CreatePar("FilePath").DfltStr("./");
      CreatePar("RunPrefix").DfltStr("");

      CreatePar("RunNumber").DfltStr("-1"); // avoid DIM problems
      CreatePar("RunInfo", "info").SetStr("NoRun");

      CreateCmdDef("StartRun");
      CreateCmdDef("StopRun");
      CreateCmdDef("SetPrefix").AddArg("Prefix", "string", true, "te_");
      CreateCmdDef("SetRunNumber").AddArg("RunNumber", "string", true, "1");
      // JAM: currently, dim-epics interface does not support strings > 40 chars.
      // thus we cannot use parameters wrapped in xml formatting as in java gui
      // beamtime workaround: define special commands without parameters
      CreateCmdDef("SetPrefixTest");
      CreateCmdDef("SetPrefixBeam");
      CreateCmdDef("SetPrefixCosmics");
      CreateCmdDef("IncRunNumber");
      CreateCmdDef("DecRunNumber");
      CreateCmdDef("ResetRunNumber");
   }

   fSuperCombiner = false;
   fDoMeasureADC = false;
   fCheckSlavesConn = false;
   fFirstSlavePort = -1;

   DOUT3(("!!!! Readout application %s created !!!!", GetName()));

   if (NumSlaves()>0)
//      RegisterForParameterEvent(std::string(nameSuperComb)+"/Input*/"+dabc::ConnectionObject::ObjectName());
        RegisterForParameterEvent(dabc::ConnectionObject::ObjectName());
//        RegisterForParameterEvent(std::string(nameSuperComb)+"*"+dabc::ConnectionObject::ObjectName());
}

roc::ReadoutApplication::~ReadoutApplication()
{
   DOUT3(("!!!! Readout application %s destructor !!!!", GetName()));
}

std::string roc::ReadoutApplication::DataServerKind() const
{
   return Par(mbs::xmlServerKind).AsStdStr();
}

std::string roc::ReadoutApplication::RocAddr(int nreadout) const
{
   if ((nreadout < 0) || (nreadout >= NumRocs()))
      return "";
   return Par(FORMAT(("%s%d", xmlRocIp, nreadout))).AsStdStr();
}

int roc::ReadoutApplication::SusiboAddr(int index) const
{
   if ((index < 0) || (index >= NumSusibo()))
      return 0;
   return Par(FORMAT(("%s%d", roc::xmlSusiboAddr, index))).AsInt();
}

std::string roc::ReadoutApplication::SlaveAddr(int index) const
{
   if ((index < 0) || (index >= NumSlaves()))
      return "";
   return Par(FORMAT(("%s%d", xmlSlaveAddr, index))).AsStdStr();
}

std::string roc::ReadoutApplication::MbsAddr(int index) const
{
   if ((index < 0) || (index >= NumMbs()))
      return "";
   return Par(FORMAT(("%s%d", roc::xmlMbsAddr, index))).AsStdStr();
}

bool roc::ReadoutApplication::CreateCombiner(const char* modulename,
      bool bnetmode, const char* inppoolname, const char* outpoolname,
      unsigned buffersize, int numoutputs, bool skiperrordata,
      roc::BoardsVector& brds)
{
   int numoptic(0);
   for (unsigned n = 0; n < brds.size(); n++)
      if (roc::BoardConnector::CheckOptic(brds[n].rocname.c_str()) != 0)
         numoptic++;

   dabc::CmdCreateModule cmd1("roc::CombinerModule", modulename, "RocCombThrd");
   cmd1.SetBool(roc::xmlBnetMode, bnetmode);
   cmd1.SetStr(dabc::xmlInputPoolName, inppoolname);
   cmd1.SetStr(dabc::xmlOutputPoolName, outpoolname);
   cmd1.SetInt(dabc::xmlBufferSize, buffersize);
   cmd1.SetInt(dabc::xmlNumInputs, brds.size());
   cmd1.SetInt(dabc::xmlNumOutputs, numoutputs);
   cmd1.SetBool(roc::xmlSkipErrorData, skiperrordata);

   bool res = dabc::mgr.Execute(cmd1);
   DOUT3(("Create ROC combiner module = %s", DBOOL(res)));
   if (!res)
      return false;

   std::string devname, devtype, thrdname, opticdevname, opticthrdname;

   if (numoptic > 0) {
      // create optic device first

      opticdevname = "AbbDev";
      opticthrdname = "AbbDevThrd";

      dabc::CmdCreateDevice cmd2(roc::typeAbbDevice, opticdevname.c_str(),
            opticthrdname.c_str());
//      cmd2.SetBool(roc::xmlUseDLM, true);

      if (!dabc::mgr.Execute(cmd2)) {
         EOUT(("Cannot create AbbDevice"));
         return false;
      }

      brds.addDLMDev(opticdevname);

      if (numoptic > 1) {
         DOUT0(("Create optical splitter module"));

         dabc::CmdCreateModule cmd3("roc::SplitterModule", "Splitter",
               "RocCombThrd");
         cmd3.SetInt(dabc::xmlBufferSize, buffersize);
         cmd3.SetStr(dabc::xmlPoolName, inppoolname);
         cmd3.SetInt(dabc::xmlNumOutputs, numoptic);
         if (!dabc::mgr.Execute(cmd3)) {
            EOUT(("Cannot create optic Splitter module"));
            return false;
         }

         dabc::CmdCreateTransport cmd4("Splitter/Input", opticdevname.c_str(),
               opticthrdname.c_str());
         cmd4.SetStr(roc::xmlBoardAddr, "none");

         if (!dabc::mgr.Execute(cmd4)) {
            EOUT(("Cannot connect splitter module to ABB device"));
            return false;
         }
      }
   }

   int splitterid(0);

   for (unsigned n = 0; n < brds.size(); n++) {

      DOUT3(("Start configure ROC%u", n));

      std::string rocaddr = brds[n].rocname;
      std::string portname = dabc::format("%s/Input%u", modulename, n);

      bool isoptic = roc::BoardConnector::CheckOptic(rocaddr.c_str()) != 0;

      int infoid = -1;

      if (isoptic) {

         devname = opticdevname;

         dabc::Command cmd5;

         if (numoptic == 1) {
            cmd5 = dabc::CmdCreateTransport(portname.c_str(),
                  opticdevname.c_str(), opticthrdname.c_str());
            cmd5.SetStr(roc::xmlBoardAddr, rocaddr);
         } else {
            cmd5 = dabc::CmdConnectPorts(
                  FORMAT(("Splitter/Output%d", splitterid)), portname.c_str());
            infoid = splitterid;
            splitterid++;
         }

         if (!dabc::mgr.Execute(cmd5)) {
            EOUT(("Cannot provide connection for ROC %s", rocaddr.c_str()));
            return false;
         }
      } else {

         devname = dabc::format("Roc%udev", n);
         thrdname = "RocReadoutThrd";
         //thrdname = dabc::format("RocDev%uThrd", n);

         brds.addDLMDev(devname);

         dabc::CmdCreateDevice cmd6(roc::typeUdpDevice, devname.c_str(),
               thrdname.c_str());
         cmd6.SetStr(roc::xmlBoardAddr, rocaddr);
         cmd6.SetStr(roc::xmlRole, roc::roleToString(roc::roleDAQ));

         if (!dabc::mgr.Execute(cmd6)) {
            EOUT(("Cannot create device %s for roc %s", devname.c_str(), rocaddr.c_str()));
            return false;
         }

         dabc::CmdCreateTransport cmd7(portname.c_str(), devname.c_str(),
               thrdname.c_str());
         cmd7.SetStr(roc::xmlBoardAddr, rocaddr);

         if (!dabc::mgr.Execute(cmd7)) {
            EOUT(("Cannot connect readout module to device %s", devname.c_str()));
            return false;
         }
      }

      roc::CmdGetBoardPtr cmd8;
      cmd8.SetStr(roc::xmlBoardAddr, rocaddr);
      cmd8.SetReceiver(devname.c_str());
      roc::Board* brd = 0;
      if (dabc::mgr.Execute(cmd8))
         brd = (roc::Board*) cmd8.GetPtr(roc::CmdGetBoardPtr::Board());

      if (brd == 0) {
         EOUT(("Cannot get board pointer from device %s", devname.c_str()));
         return false;
      }

      brds.setBoard(n, brd, devname);

      if (infoid >= 0) {
         dabc::Command cmd9("AddExtraBoard");
         cmd9.SetStr(roc::xmlBoardAddr, rocaddr);
         cmd9.SetReceiver(opticdevname.c_str());
         if (!dabc::mgr.Execute(cmd9)) {
            EOUT(("Cannot assign explicit ROC path %s to the AbbDevice", rocaddr.c_str()));
            return false;
         }

         // add information to splitter
         dabc::Command cmd10("AddROC");
         cmd10.SetInt("ROCID", brd->rocNumber());
         cmd10.SetReceiver("Splitter");
         if (!dabc::mgr.Execute(cmd10)) {
            EOUT(("Cannot ADD ROCID %d to splitter", brd->rocNumber()));
            return false;
         }
      }

      dabc::Command cmd11("ConfigureInput");

      cmd11.SetInt("Input", n);
      cmd11.SetBool("IsUdp", brd->getTransportKind() == roc::kind_UDP);
      cmd11.SetInt("ROCID", brd->rocNumber());
      cmd11.SetInt("Format", brd->getMsgFormat());
      cmd11.SetReceiver(modulename);
      if (!dabc::mgr.Execute(cmd11)) {
         EOUT(("Cannot configure input %u of combiner module!!!", n));
         return false;
      }

      DOUT2(("Did configure input%d as ROC%u addr:%s", n, brd->rocNumber(), rocaddr.c_str()));
   }

   return true;
}

bool roc::ReadoutApplication::CreateRocCombiner()
{
   fRocBrds.returnBoards();
   for (int n = 0; n < NumRocs(); n++)
      fRocBrds.addRoc(RocAddr(n),
            Par(FORMAT(("%s%d", xmlRocFebs, n))).AsStdStr());

   bool res = CreateCombiner(nameRocComb, // module name
         false, // bnet mode
         roc::xmlRocPool, // inp pool name
         roc::xmlRocPool, // out pool name
         Par(dabc::xmlBufferSize).AsInt(16 * 1024), // buffer size
         2, // num output
         fSuperCombiner, // skip error data
         fRocBrds);

   if (res && fDoMeasureADC)
      for (unsigned n = 0; n < fRocBrds.size(); n++)
         for (int nfeb = 0; nfeb < 2; nfeb++)
            for (int nadc = 0; nadc < fRocBrds.numAdc(n, nfeb); nadc++) {
               std::string parname = dabc::format("ROC%u_FEB%d_ADC%d",
                     fRocBrds[n].brd->rocNumber(), nfeb, nadc);
               CreatePar(parname);
            }

   return res;
}

bool roc::ReadoutApplication::CreateAppModules()
{
   fCalibrState = calNONE;
   fFileOutPort.clear();

   DOUT2(("CreateAppModules starts..."));
   bool res = false;

   dabc::lgr()->SetLogLimit(10000000);

   unsigned bufsize = Par(dabc::xmlBufferSize).AsInt(8192);

   // special case - try to make bigger buffer when simple write mode is used
   if (Par(roc::xmlSyncScaleDown).AsInt()==77)
      bufsize+=100;

   DOUT1(("Create ROC POOL %d x %d", bufsize, Par(dabc::xmlNumBuffers).AsInt(100)));

   dabc::mgr.CreateMemoryPool(roc::xmlRocPool,
         bufsize, Par(dabc::xmlNumBuffers).AsInt(100));

   bool isepics = EpicsStreamNode().length() > 0;

   fDoMeasureADC = (NumRocs() > 0) && Par(roc::xmlMeasureADC).AsBool(false);

   int num_super_inputs = NumMbs() + NumSlaves() + (NumRocs() > 0 ? 1 : 0)
         + (NumSusibo() > 0 ? 1 : 0) + (isepics ? 1 : 0);
   fSuperCombiner = num_super_inputs > 1;
   if (NumMbs() > 0)
      fSuperCombiner = true; // one need supercombiner to readout MBS

   std::string server_port;

//   if (dabc::mgr.NumNodes()>1)
//      dabc::mgr.CreateDevice(dabc::typeSocketDevice, "ConnDev");

   if (NumRocs() > 0) {
      if (!CreateRocCombiner())
         return false;

      server_port = dabc::format("%s/Output0", nameRocComb);

      // SwitchCalibrationMode(false);
   }

   if (NumMbs() > 2) {
      EOUT(("For a moment maximum two MBS input are supported"));
      exit(345);
   }

   if (NumSusibo() > 0) {
      dabc::CmdCreateModule cmd1("mbs::CombinerModule", nameSusiboComb,
            "SusiboCombThrd");
      cmd1.SetStr(dabc::xmlPoolName, roc::xmlRocPool);
      cmd1.SetInt(dabc::xmlNumInputs, NumSusibo());
      cmd1.SetBool(mbs::xmlNormalOutput, true);
      cmd1.SetBool(mbs::xmlFileOutput, true);
      cmd1.SetBool(mbs::xmlServerOutput, true);
      cmd1.SetBool(mbs::xmlCombineCompleteOnly, false);
      cmd1.SetInt(mbs::xmlEvidMask, 0xffffff); // only 24 bit used in sync counter
      cmd1.SetStr(mbs::xmlCombinerRatesPrefix, "Spadic");

      res = dabc::mgr.Execute(cmd1);
      DOUT3(("Create susibo combiner module = %s", DBOOL(res)));
      if (!res)
         return false;

      for (int n = 0; n < NumSusibo(); n++) {
         dabc::CmdCreateTransport cmd2(
               FORMAT(("%s/Input%d", nameSusiboComb, n)),
               "spadic::SusiboInput");

         cmd2.SetStr("SusiboAddr-", FORMAT(("SusiboInput%d", n)));
         cmd2.SetInt("SusiboID-", SusiboAddr(n));
         cmd2.SetInt("EventsInBuffer", Par("EventsInBuffer").AsInt(1));
         cmd2.SetInt("HitDelay", Par("HitDelay").AsInt(200));
         cmd2.SetDouble("PollingTimeout", Par("PollingTimeout").AsDouble(0.01));

         cmd2.SetInt("SusiboMbsFormat", true);

         res = dabc::mgr.Execute(cmd2);

         if (!res)
            return false;
      }

      server_port = dabc::format("%s/%s", nameSusiboComb,
            mbs::portServerOutput);
   }

   if (DoSuperCombiner()) {
      dabc::CmdCreateModule cmd4("mbs::CombinerModule", nameSuperComb,
            "SuperCombThrd");
      cmd4.SetStr(dabc::xmlPoolName, roc::xmlRocPool);
      cmd4.SetInt(dabc::xmlNumInputs, num_super_inputs);
      cmd4.SetBool(mbs::xmlNormalOutput, false);
      cmd4.SetBool(mbs::xmlFileOutput, true);
      cmd4.SetBool(mbs::xmlServerOutput, DataServerKind().length() > 0);
      cmd4.SetBool(mbs::xmlCombineCompleteOnly,
            Par(mbs::xmlCombineCompleteOnly).AsBool(false));
      cmd4.SetStr(mbs::xmlCombinerRatesPrefix, "Super");

      cmd4.SetInt(mbs::xmlEvidMask, 0xffffff); // only 24 bit used in sync counter

      res = dabc::mgr.Execute(cmd4);
      DOUT3(("Create Mbs combiner module = %s", DBOOL(res)));
      if (!res) return false;

      int ninp = 0;

      for (int nmbs = 0; nmbs < NumMbs(); nmbs++) {
         dabc::CmdCreateTransport cmd5(
               FORMAT(("%s/Input%d", nameSuperComb, ninp)),
               mbs::typeClientTransport, "MbsReadoutThrd");
         cmd5.SetStr(mbs::xmlServerName, MbsAddr(nmbs));
         cmd5.SetStr(mbs::xmlServerKind,
               mbs::ServerKindToStr(mbs::TransportServer));
         res = dabc::mgr.Execute(cmd5);
         DOUT1(("Created MBS client transport for node %s : result is %s", MbsAddr(nmbs).c_str(), DBOOL(res)));
         if (!res)
            return false;

         dabc::Command cmd6("ConfigureInput");
         cmd6.SetInt("Port", ninp);
         cmd6.SetBool("RealMbs", true);
         cmd6.SetBool("RealEvntNum", false);
         cmd6.SetUInt("EvntSrcFullId",
               Par(FORMAT(("%s%d", roc::xmlSyncSubeventId, nmbs))).AsInt(0));
         cmd6.SetUInt("EvntSrcShift", 0);
         cmd6.SetStr("RateName", "MbsData");

         cmd6.SetReceiver(nameSuperComb);
         res = dabc::mgr.Execute(cmd6);

         DOUT1(("Configure special MBS case as supercombiner input %d : result is %s", ninp, DBOOL(res)));
         if (!res)
            return false;

         ninp++;
      }

      for (int nslave = 0; nslave < NumSlaves(); nslave++) {

         if (!ConnectSlave(nslave, ninp)) return false;

         if (fFirstSlavePort<0) fFirstSlavePort = ninp;

         ninp++;
      }

      if (NumRocs() > 0) {
         dabc::Command cmd7("ConfigureInput");
         cmd7.SetInt("Port", ninp);
         cmd7.SetBool("RealMbs", false);
         cmd7.SetBool("RealEvntNum", true);
         cmd7.SetReceiver(nameSuperComb);
         res = dabc::mgr.Execute(cmd7);
         DOUT1(("Configure ROC as supercombiner input %d : result is %s", ninp, DBOOL(res)));
         if (!res)
            return false;

         dabc::mgr.Connect(dabc::format("%s/Output0", nameRocComb),
               dabc::format("%s/Input%d", nameSuperComb, ninp));

         ninp++;
      }

      if (NumSusibo() > 0) {
         dabc::mgr.Connect(
               dabc::format("%s/%s", nameSusiboComb, mbs::portOutput),
               dabc::format("%s/Input%d", nameSuperComb, ninp));

         ninp++;
      }

      if (isepics) {
         dabc::CmdCreateTransport cmd8(
               FORMAT(("%s/Input%d", nameSuperComb, ninp)),
               mbs::typeClientTransport, "MbsReadoutThrd");
         cmd8.SetStr(mbs::xmlServerName, EpicsStreamNode());
         cmd8.SetStr(mbs::xmlServerKind,
               mbs::ServerKindToStr(mbs::StreamServer));
         res = dabc::mgr.Execute(cmd8);
         DOUT1(("Created EPICS client transport result is %s", DBOOL(res)));
         if (!res)
            return false;

         dabc::Command cmd9("ConfigureInput");
         cmd9.SetInt("Port", ninp);
         cmd9.SetBool("RealMbs", false);
         cmd9.SetBool("RealEvntNum", false);
         cmd9.SetBool("NoEvntNum", true);

         cmd9.SetReceiver(nameSuperComb);
         res = dabc::mgr.Execute(cmd9);
         DOUT1(("Configure special EPICS case as supercombiner input %d : result is %s", ninp, DBOOL(res)));
         if (!res) return false;

         ninp++;
      }

      server_port = dabc::format("%s/%s", nameSuperComb, mbs::portServerOutput);
   }

   if (DoSuperCombiner()) {
      fFileOutPort = dabc::format("%s/%s", nameSuperComb, mbs::portFileOutput);
   } else if ((NumRocs() > 0)) {
      fFileOutPort = dabc::format("%s/Output1", nameRocComb);
   } else if (NumSusibo() > 0) {
      fFileOutPort = dabc::format("%s/Output1", nameSusiboComb);
   }

   if (!OutputFileName().empty() && !fFileOutPort.empty()) {
      if (!StartFile(OutputFileName())) return false;
   }

   if (!DataServerKind().empty() && !server_port.empty()) {

      ///// connect module to mbs server:

      dabc::CmdCreateTransport cmd13(server_port.c_str(),
            mbs::typeServerTransport, "MbsServerThrd");

      cmd13.SetStr(mbs::xmlServerKind, DataServerKind());

      // no need to set extra parameters - they will be taken from application !!!
//      cmd->SetInt(dabc::xmlBufferSize, GetParInt(dabc::xmlBufferSize, 8192));

      res = dabc::mgr.Execute(cmd13);
      DOUT2(("Connected module %s output0 to Mbs server = %s", server_port.c_str(), DBOOL(res)));
      if (!res) return false;
   }

   return true;
}

bool roc::ReadoutApplication::StartFile(const std::string& filename)
{
   if (fFileOutPort.empty()) {
      EOUT(("Cannot create file output - no appropriate port specified"));
      return false;
   }

   dabc::CmdCreateTransport cmd11(fFileOutPort, mbs::typeLmdOutput);

   cmd11.SetStr(mbs::xmlFileName, filename);

   if (fMasterNode) {
      cmd11.SetStr("InfoPar", "RunInfo");
   } else {
      CreatePar("FileInfo","info").SetSynchron(true, 2., false).SetStr("nofile");
      cmd11.SetStr("InfoPar", "FileInfo"); // inform transport which parameter should be used for information
   }

   bool res = dabc::mgr.Execute(cmd11);
   DOUT1(("Create lmd file %s for %s, res = %s", filename.c_str(), fFileOutPort.c_str(), DBOOL(res)));
   return res;
}

bool roc::ReadoutApplication::StopFile()
{
   if (fFileOutPort.find(nameSuperComb)!=0) {
      EOUT(("File port connected not to the super combiner!!!"));
      dabc::PortRef port = dabc::mgr.FindPort(fFileOutPort);
      if (!port.null()) port()->Disconnect();
   }

   return dabc::mgr.FindModule(nameSuperComb).Execute(mbs::comStopFile);

// TODO: replace later by the new code

//   dabc::PortRef port = dabc::mgr.FindPort(fFileOutPort);
//   return port.Disconnect();
}

bool roc::ReadoutApplication::ConnectSlave(int nslave, int ninp)
{
   dabc::CmdCreateTransport cmd21(
         FORMAT(("%s/Input%d", nameSuperComb, ninp)),
         mbs::typeClientTransport, "SlaveReadoutThrd");
   cmd21.SetStr(mbs::xmlServerName, SlaveAddr(nslave));
   cmd21.SetStr(mbs::xmlServerKind,
         mbs::ServerKindToStr(mbs::TransportServer));
   cmd21.SetDouble(dabc::xmlConnTimeout, 5.);
   bool res = dabc::mgr.Execute(cmd21);

   DOUT1(("Created client transport for slave %s : result is %s",
           SlaveAddr(nslave).c_str(), DBOOL(res)));
   if (!res) return false;

   int slavesubevid = Par(FORMAT(("Slave%s%d", roc::xmlSyncSubeventId, nslave))).AsInt(0);

   if (slavesubevid==0) {
      DOUT1(("Configure slave %d input as normal MBS input", nslave));
   } else {
      DOUT1(("Configure slave %d input as ubnormal MBS input with event number in subeventid 0x%x ", nslave, slavesubevid));

      dabc::Command cmd22("ConfigureInput");
      cmd22.SetInt("Port", ninp);
      cmd22.SetBool("RealMbs", false);
      cmd22.SetBool("RealEvntNum", false);
      cmd22.SetUInt("EvntSrcFullId", slavesubevid);
      cmd22.SetUInt("EvntSrcShift", 0);
      cmd22.SetStr("RateName", "SlaveData");

      cmd22.SetReceiver(nameSuperComb);
      res = dabc::mgr.Execute(cmd22);
      DOUT1(("Configure client for slave %d as supercombiner input %d : result is %s",
              nslave, ninp, DBOOL(res)));
      if (!res) return false;
   }
   return true;
}


bool roc::ReadoutApplication::AfterAppModulesStopped()
{
   if (fCalibrState == calON)
      SwitchCalibrationMode(false);
   return true;
}

bool roc::ReadoutApplication::BeforeAppModulesDestroyed()
{
   fRocBrds.returnBoards();

   return dabc::Application::BeforeAppModulesDestroyed();
}

bool roc::ReadoutApplication::IsModulesRunning()
{
   if ((NumRocs() > 0) && !dabc::mgr.FindModule(nameRocComb).IsRunning())
      return false;

   if ((NumSusibo() > 0) && !dabc::mgr.FindModule(nameSusiboComb).IsRunning())
      return false;

   if (DoSuperCombiner() && !dabc::mgr.FindModule(nameSuperComb).IsRunning())
      return false;

   return true;
}

int roc::ReadoutApplication::ExecuteCommand(dabc::Command cmd)
{
   if (cmd.IsName(roc::CmdCalibration::CmdName())) {
      bool flag = cmd.GetBool(roc::CmdCalibration::FlagName(), true);
      return cmd_bool(SwitchCalibrationMode(flag));
   } else if (cmd.IsName("StartRun")) {
      std::string fname = Par("FilePath").AsStdStr();
      fname+=Par("RunPrefix").AsStdStr();
      int number = Par("RunNumber").AsInt(-1);
      if (number >= 0)
         fname += dabc::format("run%d.lmd", number);
      if (StartFile(fname)) {
         Par("RunInfo").SetStr(std::string("Start file ")+fname);
         return dabc::cmd_true;
      }
      Par("RunInfo").SetStr(std::string("Fail open file ")+fname);
      return dabc::cmd_false;
   } else if (cmd.IsName("StopRun")) {

      bool res = StopFile();
      int number = Par("RunNumber").AsInt(-1);
      if (number >= 0)
         Par("RunNumber").SetInt(number + 1);
      Par("RunInfo").SetStr("NoRun");
      return cmd_bool(res);
   } else if (cmd.IsName("SetPrefix")) {
      // todo: get par from string
      std::string prename = Cfg("Prefix", cmd).AsStdStr();
      Par("RunPrefix").SetStr(prename);
      DOUT1(("Setting Prefix to %s", prename.c_str()));
      return dabc::cmd_true;
   } else if (cmd.IsName("SetRunNumber")) {
      int number = Cfg("RunNumber", cmd).AsInt();
      Par("RunNumber").SetInt(number);
      DOUT1(("Setting Run number to %d", number));
      return dabc::cmd_true;
   } else if (cmd.IsName("SetPrefixTest")) {
      std::string prename = "Te_";
      Par("RunPrefix").SetStr(prename);
      DOUT1(("Setting Prefix to %s", prename.c_str()));
      return dabc::cmd_true;
   } else if (cmd.IsName("SetPrefixBeam")) {
      std::string prename = "Be_";
      Par("RunPrefix").SetStr(prename);
      DOUT1(("Setting Prefix to %s", prename.c_str()));
      return dabc::cmd_true;
   } else if (cmd.IsName("SetPrefixCosmics")) {
      std::string prename = "Co_";
      Par("RunPrefix").SetStr(prename);
      DOUT1(("Setting Prefix to %s", prename.c_str()));
      return dabc::cmd_true;
   } else if (cmd.IsName("IncRunNumber")) {
      int number = Par("RunNumber").AsInt(-1);
      if (number >= 0)
         Par("RunNumber").SetInt(number + 1);
      return dabc::cmd_true;
   } else if (cmd.IsName("DecRunNumber")) {
      int number = Par("RunNumber").AsInt(-1);
      if (number >= 0) {
         number -= 1;
         if (number < 0)
            number = 0;
         Par("RunNumber").SetInt(number);
         return dabc::cmd_true;
      }
   } else if (cmd.IsName("ResetRunNumber")) {
       Par("RunNumber").SetInt(0);
       return dabc::cmd_true;
   } else if (cmd.IsName("ResetAllGet4")) {
      fRocBrds.ResetAllGet4();
      return dabc::cmd_true;
   }

   return dabc::Application::ExecuteCommand(cmd);
}

bool roc::ReadoutApplication::SwitchCalibrationMode(bool on)
{
   if ((fCalibrState == calON) && on)
      return true;

   if ((fCalibrState == calOFF) && !on)
      return true;

   DOUT0(("SwitchCalibration %s %5.2f", DBOOL(on), dabc::Now().AsDouble()));

   if (on && fDoMeasureADC) {
      roc::MessagesVector* vect = fRocBrds.readoutExtraMessages();

      if (vect != 0) {

         for (unsigned n = 0; n < vect->size(); n++)
            if (vect->at(n).isSysMsg()
                  && vect->at(n).getSysMesType() == roc::SYSMSG_ADC) {
               uint32_t val = vect->at(n).getSysMesData();

               std::string parname = dabc::format("ROC%u_FEB%d_ADC%d",
                     vect->at(n).getRocNumber(), val >> 31, (val >> 24) & 0x7f);

               Par(parname).SetInt(val & 0xffffff);

//               dabc::RateParameter* rate = dynamic_cast<dabc::RateParameter*> (FindPar(parname.c_str()));
//               if (rate) rate->ChangeRate(val & 0xffffff);
            }

         roc::CmdMessagesVector cmd(vect);
         cmd.SetReceiver(nameRocComb);
         dabc::mgr.Submit(cmd);
      }
   }

   fRocBrds.autoped_switch(on);

   //  DLM 10/11 not implemented for calibration switch on/off
   // fRocBrds.issueDLM(on ? 10 : 11);

   fCalibrState = on ? calON : calOFF;

   return true;
}

void roc::ReadoutApplication::ProcessParameterEvent(const dabc::ParameterEvent& evnt)
{
    // here one should analyze

   DOUT0(("Get change event for connection %s value %s", evnt.ParName().c_str(), evnt.ParValue().c_str()));

   if (evnt.ParValue() == dabc::ConnectionObject::GetStateName(dabc::ConnectionObject::sBroken)) {
      DOUT0(("Activate reconnection timeout for slaves"));
      fCheckSlavesConn = true;
      ActivateTimeout(3);
   }
}

double roc::ReadoutApplication::ProcessTimeout(double last_diff)
{
   double res = dabc::Application::ProcessTimeout(last_diff);

   // retry at least every
   if (fCheckSlavesConn) {

      DOUT0(("!!! Check reconnection for slaves !!!"));

      dabc::ModuleRef m = dabc::mgr.FindModule(nameSuperComb);
      if (m.null()) { EOUT(("Did not found super combiner !!!")); return res; }

      bool tryagain = false;

      for (int nslave = 0; nslave < NumSlaves(); nslave++) {

         if (m.IsInputConnected(nslave)) continue;

         if (!ConnectSlave(nslave, fFirstSlavePort+nslave)) tryagain = true;
      }

      if (tryagain) {
         if ((res<0) || (res>3)) res = 3;
      } else
         fCheckSlavesConn = false;
   }

   return res;
}

