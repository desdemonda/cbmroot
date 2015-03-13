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

#include "roc/ReadoutModule.h"

#include <stdlib.h>

#include "roc/Board.h"

#include "dabc/Port.h"

roc::ReadoutModule::ReadoutModule(const char* name, dabc::Command cmd) :
   dabc::ModuleAsync(name, cmd),
   fDataCond(),
   fCurrBuf(),
   fNextBuf()
{
   int numoutputs = Cfg(dabc::xmlNumOutputs, cmd).AsInt(1);
   std::string poolname = Cfg("PoolName", cmd).AsStdStr(roc::xmlRocPool);

   DOUT2(("new roc::ReadoutModule %s", GetName()));

   CreatePoolHandle(poolname.c_str());

   CreateInput("Input", Pool(), 10);
//   CreateRateParameter("InputRate", false, 3., "Input")->SetDebugOutput(true);

   for(int n=0; n<numoutputs; n++)
      CreateOutput(FORMAT(("Output%d", n)), Pool(), 10);
}

roc::ReadoutModule::~ReadoutModule()
{
}

void roc::ReadoutModule::AfterModuleStop()
{
   fCurrBuf.Release();

   dabc::Buffer buf;
   {
      dabc::LockGuard lock(fDataCond.CondMutex());
      buf << fNextBuf;
   }
   buf.Release();
}

void roc::ReadoutModule::ProcessInputEvent(dabc::Port* inport)
{
   if (!inport->CanRecv()) return;

   // one may avoid locking while here is basic type check is done
   // if mutex is locked and user just reading last byte, it will
   // fire new event after buffer set to 0
   if (!fNextBuf.null()) return;

   dabc::Buffer buf = inport->Recv();

//   DOUT1(("Read buffer %p from input", buf));

   dabc::LockGuard lock(fDataCond.CondMutex());
   if (!fNextBuf.null()) {
      EOUT(("Something completely wrong"));
      exit(111);
   }
   fNextBuf << buf;
   if (!fNextBuf.null() && fDataCond._Waiting()) fDataCond._DoFire();
}

void roc::ReadoutModule::ProcessOutputEvent(dabc::Port* inport)
{
}

bool roc::ReadoutModule::getNextBuffer(void* &buf, unsigned& len, double tmout)
{
   if (!fCurrBuf.null())  fCurrBuf.Release();

   Input()->FireInput();

   // lock only when new complete buffer is required
   {
      dabc::LockGuard lock(fDataCond.CondMutex());
      if (fNextBuf.null()) fDataCond._DoWait(tmout);
      fCurrBuf << fNextBuf;
   }

   if (fCurrBuf.null()) return false;

   buf = fCurrBuf.SegmentPtr();
   len = fCurrBuf.SegmentSize();

   return true;
}
