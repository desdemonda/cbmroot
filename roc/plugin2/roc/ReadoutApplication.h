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
#ifndef ROC_READOUTAPPLICATION_H
#define ROC_READOUTAPPLICATION_H

#include "dabc/Application.h"
#include "dabc/threads.h"
#include "dabc/MemoryPool.h"
#include "dabc/Manager.h"
#include "dabc/Module.h"

#include "roc/Factory.h"
#include "roc/Board.h"
#include "roc/Commands.h"
#include "roc/BoardsVector.h"

// #include "../plugins/ezca/ezca/Definitions.h"
// need this to compile if ezca was not build

namespace roc {

   class ReadoutApplication : public dabc::Application {
      public:
         ReadoutApplication();
         virtual ~ReadoutApplication();

         virtual bool IsModulesRunning();

         /** Return true if super combiner runs on the current node */
         bool DoSuperCombiner() const { return fSuperCombiner; }

         /** Number of ROCs connected to ReadoutModule*/
         int   NumRocs() const { return Par(roc::xmlNumRocs).AsInt(1); }

         /** Address for ROC of index*/
         std::string RocAddr(int index = 0) const;

         /** Number of SPADIC chip to readout */
         int   NumSusibo() const { return Par(roc::xmlNumSusibo).AsInt(0); }

         /** Address for SPADIC chip of index*/
         int SusiboAddr(int index = 0) const;

         /** Number of slave DABC application, used for readout */
         int   NumSlaves() const { return Par(roc::xmlNumSlaves).AsInt(0); }

         /** Address for slave dabc application of index*/
         std::string SlaveAddr(int index = 0) const;

         /** Number of MBS servers to readout */
         int   NumMbs() const { return Par(roc::xmlNumMbs).AsInt(0); }

         /** Address of addition MBS server of index*/
         std::string MbsAddr(int index = 0) const;

         /** Kind of MBS server in text format (Stream, Transport)*/
         std::string DataServerKind() const;

         std::string OutputFileName() const { return Par(xmlRawFile).AsStdStr(); }

         std::string EpicsStreamNode() const { return Par(roc::xmlEpicsStreamNode).AsStdStr(); }

         virtual bool CreateAppModules();

         virtual bool AfterAppModulesStopped();
         
         virtual bool BeforeAppModulesDestroyed();

         virtual int SMCommandTimeout() const { return 20; }

         virtual int ExecuteCommand(dabc::Command cmd);

         static bool CreateCombiner(const char* modulename, 
                                    bool bnetmode,
                                    const char* inppoolname, 
                                    const char* outpoolname, 
                                    unsigned buffersize,
                                    int numoutputs,
                                    bool skiperrordata,
                                    roc::BoardsVector& brds);

      protected:

         roc::BoardsVector         fRocBrds; /** List of boards for put/get operations */

         enum ECalibrationState { calNONE, calOFF, calON };

         bool  fSuperCombiner; /** true if super combiner is necessary */
         bool  fDoMeasureADC; /** if true, measures all values on all febsd */
         bool  fCheckSlavesConn; /** Set to true, when checking for slave connection should be done */
         int   fFirstSlavePort;  /** First port for the slaves connections */
         bool  fMasterNode;   /** Identifies master node where number of additional commands are implemented */
         std::string fFileOutPort; /** port name where file output should be connected */

         ECalibrationState fCalibrState;

         bool SwitchCalibrationMode(bool on);

         bool CreateRocCombiner();

         bool ConnectSlave(int nslave, int ninp);

         virtual void ProcessParameterEvent(const dabc::ParameterEvent& evnt);

         virtual double ProcessTimeout(double last_diff);

         bool StartFile(const std::string& filename);
         bool StopFile();

   };
}

#endif
