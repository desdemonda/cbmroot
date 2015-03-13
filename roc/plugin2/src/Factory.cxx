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

#include "roc/Factory.h"

#include "roc/CombinerModule.h"
#include "roc/ReadoutModule.h"
#include "roc/ReadoutApplication.h"
#include "roc/UdpDevice.h"
#include "roc/NxCalibrModule.h"

#include "dabc/Command.h"
#include "dabc/logging.h"

#include "mbs/LmdInput.h"
#include "roc/FileInput.h"

roc::Factory rocfactory("roc");

roc::Factory::Factory(const char* name) :
   dabc::Factory(name),
   roc::BoardConnector(),
   fDevs(),
   fFileCount(0),
   fMgrCreated(false)
{
}

dabc::Application* roc::Factory::CreateApplication(const char* classname, dabc::Command cmd)
{
   if (strcmp(classname, xmlReadoutAppClass)==0)
      return new roc::ReadoutApplication();

   return dabc::Factory::CreateApplication(classname, cmd);
}

dabc::Module* roc::Factory::CreateModule(const char* classname, const char* modulename, dabc::Command cmd)
{
   DOUT2(("roc::Factory::CreateModule called for class:%s, module:%s", classname, modulename));

   if (strcmp(classname, "roc::CombinerModule")==0) {
      DOUT2(("roc::Factory::CreateModule - Created roc::CombinerModule %s ", modulename));
      return new roc::CombinerModule(modulename, cmd);
   } else
   if (strcmp(classname, "roc::ReadoutModule")==0) {
      DOUT2(("roc::Factory::CreateModule - Created roc::ReadoutModule module %s ", modulename));
      return new roc::ReadoutModule(modulename, cmd);
   } else
   if (strcmp(classname, "roc::NxCalibrModule")==0) {
      DOUT2(("roc::Factory::CreateModule - Created roc::NxCalibrModule module %s ", modulename));
      return new roc::NxCalibrModule(modulename, cmd);
   }

   return 0;
}

dabc::Device* roc::Factory::CreateDevice(const char* classname, const char* devname, dabc::Command cmd)
{
   const char* thrdname = cmd.Field("Thread").AsStr();

   if (strcmp(classname, roc::typeUdpDevice)==0) {
      DOUT2(("roc::Factory::CreateDevice - Creating ROC UdpDevice %s ...", devname));

      roc::UdpDevice* dev = new roc::UdpDevice(devname, thrdname, cmd);

      if (!dev->IsConnected()) {
         dabc::Object::Destroy(dev);
         return 0;
      }

      return dev;
   }

   return 0;
}

roc::Board* roc::Factory::DoConnect(const char* name, roc::ClientRole role)
{
   dabc::SetDebugLevel(0);
   dabc::lgr()->SetLogLimit(1000000);
   dabc::lgr()->SetDebugMask(dabc::lgr()->GetDebugMask() & ~dabc::Logger::lTStamp);

   if (dabc::mgr.null()) {
      if (!dabc::Factory::CreateManager("dabc")) return 0;
      fMgrCreated = true;
   }

   const char* subname = CheckFile(name);
   if (subname!=0) {

      char sbuf[1024];
      strcpy(sbuf, subname);

      int selectroc = -1;

      char* pos = strstr(sbuf,"&roc=");
      if (pos!=0) {
         selectroc = atoi(pos+5);
         *pos = 0;
      }

      DOUT0(("Creating LMD-file input %s selectroc = %d", sbuf, selectroc));

      mbs::LmdInput* inp = new mbs::LmdInput(sbuf, 0x100000);

      if (!inp->Init()) {
         EOUT(("Cannot create LMD input %s", sbuf));
         delete inp;
         return 0;
      }

      fFileCount++;

      return new roc::FileInput(inp, selectroc);
   }

   subname = CheckUdp(name);
   if (subname==0) return 0; 

   static int brdcnt = 0;

   std::string devname = dabc::format("/roc/Udp%d", brdcnt++);
   if (!dabc::mgr.FindDevice(devname).null()) {
      EOUT(("Device with name %s already exists!!!", devname.c_str()));
      return 0;
   }

   DOUT3(("Create device"));

   dabc::CmdCreateDevice cmd(typeUdpDevice, devname.c_str());
   cmd.SetStr(roc::xmlBoardAddr, subname);
   cmd.SetStr(roc::xmlRole, roleToString(role));
   if (!dabc::mgr.Execute(cmd)) return 0;

   UdpDeviceRef dev = dabc::mgr.FindDevice(devname);

   DOUT3(("Create device dev = %p", dev()));

   if (dev.null()) {
      EOUT(("UDP Device for board %s cannot be created!!!", subname));
      return 0;
   }

   if (!dev()->InitAsBoard()) {
      dev.Destroy();
      return 0;
   }

   roc::Board* brd = static_cast<roc::Board*> (dev());

   fDevs.Add(dev);

   return brd;

}

bool roc::Factory::DoClose(roc::Board* brd)
{
   roc::FileInput* inp = dynamic_cast<roc::FileInput*> (brd);
   if (inp!=0) {
      fFileCount--;
      delete inp;

      if ((fFileCount==0) && (fDevs.GetSize()==0) && fMgrCreated) {
         DOUT3(("Destroying manager from roc::Factory"));

         dabc::mgr()->HaltManager();
         dabc::mgr.Destroy();
         fMgrCreated = false;
      }

      return true;
   }

   if (dabc::mgr.null()) return false;

   for (unsigned n=0;n<fDevs.GetSize();n++) {
      UdpDevice* dev = (UdpDevice*) fDevs.GetObject(n);

      if (static_cast<roc::Board*>(dev) == brd) {

         dev->CloseAsBoard();
         dev->Execute(dabc::Command(roc::UdpDevice::CmdPutDisconnect()).SetTimeout(4.));
         fDevs.TakeRef(n).Destroy();

         if ((fFileCount==0) && (fDevs.GetSize()==0) && fMgrCreated) {
            DOUT3(("Destroying manager from roc::Factory"));

            dabc::mgr()->HaltManager();
            dabc::mgr.Destroy();
            fMgrCreated = false;
         }

         return true;
      }
   }


   EOUT(("Board::Close FAIL - brd %p not registered. Check if board closed second time", brd));
   return false;
}
