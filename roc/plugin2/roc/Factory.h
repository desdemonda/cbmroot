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
#ifndef ROC_FACTORY_H
#define ROC_FACTORY_H

#ifndef DABC_Factory
#include "dabc/Factory.h"
#endif

#ifndef DABC_ReferencesVector
#include "dabc/ReferencesVector.h"
#endif


#ifndef ROC_Board
#include "roc/Board.h"
#endif

namespace roc {

   class UdpDevice;

   class Factory: public dabc::Factory,
                  public roc::BoardConnector {
      protected:

         dabc::ReferencesVector fDevs;
         int  fFileCount;  // number of created files handles
         bool fMgrCreated; // true when manager was created automatically

      public:

         Factory(const char* name);

         virtual dabc::Application* CreateApplication(const char* classname, dabc::Command cmd);

         virtual dabc::Module* CreateModule(const char* classname, const char* modulename, dabc::Command cmd);

         virtual dabc::Device* CreateDevice(const char* classname, const char* devname, dabc::Command cmd);

         virtual roc::Board* DoConnect(const char* name, roc::ClientRole role = roc::roleDAQ);

         virtual bool DoClose(roc::Board* brd);

   };

}

#endif

