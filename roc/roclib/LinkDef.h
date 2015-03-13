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

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclasses;

#pragma link C++ namespace nxyter;
#pragma link C++ class nxyter::RocNx+;
#pragma link C++ class nxyter::DataDebug+;
#pragma link C++ class nxyter::NxChip+;
#pragma link C++ class nxyter::NxI2c+;
#pragma link C++ class nxyter::NxContext+;
#pragma link C++ class nxyter::MainAdc+;
#pragma link C++ class nxyter::FebBase+;
#pragma link C++ class nxyter::Feb1nxGenB+;
#pragma link C++ class nxyter::Feb1nxGenC+;
#pragma link C++ class nxyter::Feb2nxGas+;
#pragma link C++ class nxyter::Feb4nxBT+;
#pragma link C++ class nxyter::QuickDaq+;
#pragma link C++ class nxyter::QuickDaqStats+;
#pragma link C++ class nxyter::DistFunc+;
#pragma link C++ class nxyter::DistFuncArray+;
#pragma link C++ class nxyter::NxDataSummary+;

//the following pragma's didn't work, unclear why
//#pragma link C++ function operator<<(ostream&, const nxyter::NxContext&);
//#pragma link C++ enum nxyter::nxRegs;

#pragma link C++ namespace roc;
#pragma link C++ class roc::Board+;
#pragma link C++ class roc::Oper+;
#pragma link C++ class std::vector<roc::Oper>+;
#pragma link C++ class roc::OperList+;
#pragma link C++ class roc::Gpio+;
#pragma link C++ class roc::UdpBoard+;
#pragma link C++ class roc::Peripheral+;
#pragma link C++ class roc::I2cDevice+;
#pragma link C++ class roc::Message+;
#pragma link C++ class roc::Iterator+;
#pragma link C++ class roc::Sorter+;

#endif
