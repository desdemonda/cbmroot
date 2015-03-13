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

#define USE_NOPER

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <vector>

#include "boost/format.hpp"

#include "roc/Board.h"
#include "roc/defines_roc.h"
#include "roc/defines_optic.h"
#include "roc/defines_udp.h"
#include "feet/defines_feet.h"


const char* roc::xmlNumRocs         = "NumRocs";
const char* roc::xmlRocPool         = "RocPool";
const char* roc::xmlTransportWindow = "TransportWindow";
const char* roc::xmlTransportKind   = "TransportKind";
const char* roc::xmlBoardAddr       = "BoardAddr";
const char* roc::xmlRole            = "Role";
const char* roc::xmlRocNumber       = "RocNumber";
const char* roc::xmlLowWater        = "LowWater";
const char* roc::xmlHighWater       = "HighWater";
const char* roc::xmlMsgFormat       = "MsgFormat";
const char* roc::xmlFlushTime       = "FlushTime";

const char* roc::typeUdpDevice      = "roc::UdpDevice";
const char* roc::typeAbbDevice      = "roc::AbbDevice";


/*!
 * \page roc_board_opererr roc::Board get/put/oper return codes
 *
 * The methods accessing the ROC control space, like
 * \li get, put, and oper... in roc::Board
 * \li many set and get methods in classes derived from roc::Peripheral
 *
 * return an \c int return code. It holds 0 in case the operation succeeded
 * and is non-zero when it failed. In case of failure
 * \li the lower 8 bits contain an error code as defined in
 *     roc::Board::operErr and is detailed below
 * \li the upper 24 bits contain an index or step number. This allows to
 *     reconstruct the point of failure for list or array operations like
 *     roc::Board::operGen.
 *
 * Use the methods
 * \li roc::Board::operErrCode() to retrieve the error code
 * \li roc::Board::operErrIndex() to retrieve the index value
 * \li roc::Board::operErrBuild() to build a return code
 * \li roc::Board::operErrBuildInc() to modify index part of a return code
 *
 * The error codes are
 * \li \e kOperSuccess: success
 * \li \e kOperAddrErr: invalid address, there is no ROC resource under this
 *                       address.
 * \li \e kOperValueErr: invalid value, the value to be written is not allowed
 *                       in the current state of the ROC or ROC peripheral.
 *                       Only returned for write type operations.
 * \li \e kOperStateErr: invalid state, the operation is not allowed in the
 *                       current state of the ROC or ROC peripheral.
 * \li \e kOperNetworkErr: network communication error.
 * \li \e kOperVerifyErr: verify mismatch, a readback after a write operations
 *                        yielded a value different from the written one.
 *                        Only returned by dedicated  write type operations,
 *                        the method name often ends in ...Verify.
 * \li \e kOperBusErr: secondary bus error. This is returned when a field bus
 *                      operation, like on a I2C bus, failed.
 */

//----------------------------------------------------------------------------
//! Convert role state to a string representation

const char* roc::roleToString(roc::ClientRole role)
{
   switch (role) {
      case roleNone: return 0;
      case roleObserver: return "Observer";
      case roleControl: return "Control";
      case roleDAQ: return "DAQ";
   }
   return 0;
}

//----------------------------------------------------------------------------
//! Setup the role of the client application

roc::ClientRole roc::defineClientRole(const char* name, roc::ClientRole dflt)
{
   if ((name==0) || (strlen(name)==0)) return dflt;
   if (strcmp(name,"Observer")==0) return roleObserver;
   if (strcmp(name,"Control")==0) return roleControl;
   if (strcmp(name,"DAQ")==0) return roleDAQ;
   return dflt;
}

/*!
 * \class roc::Board
 * \brief This class represents a single ROC.
 *
 * \todo add class docu
 */

//----------------------------------------------------------------------------

roc::Board::Board() :
   fRole(roleNone),
   fRocNumber(0),
   fDefaultTimeout(2.),
   fVerbosity(0),
   fOperTrace(1),
   fConnector(0)
{

  // current ROC address def's from defines.h

  addRocAddrMapping("ROC_TYPE", ROC_TYPE);
  addRocAddrMapping("ROC_HWV", ROC_HWV);
  addRocAddrMapping("ROC_ROCID", ROC_ROCID);
  addRocAddrMapping("ROC_ADDSYSMSG", ROC_ADDSYSMSG);
  addRocAddrMapping("ROC_SYSTEM_RESET", ROC_SYSTEM_RESET);
}

//----------------------------------------------------------------------------

roc::Board::~Board()
{
}

uint32_t roc::Board::initBoard()
{
   OperList lst;
   lst.addGet(ROC_ROCID);
   lst.addGet(ROC_HWV);
   lst.addGet(ROC_TYPE);
   if (operGen(lst)!=0) return 0;

   fRocNumber          = lst.oper(0).value;
   uint32_t roc_hw_ver = lst.oper(1).value;
   uint32_t roc_typ    = lst.oper(2).value;

   char sbuf[1000];

   if(roc_hw_ver < 0x02000000) {
      sprintf(sbuf, "The ROC you want to access has hardware version 0x%08x", roc_hw_ver);
      ShowDebug(-1, sbuf);
      sprintf(sbuf, "Please update your hardware to major version 2.0 == 0x%08x", 0x02000000);
      ShowDebug(-1, sbuf);
   }

   const char* fe_typ = "<unknown>";
   if ((roc_typ >> 16) == kind_nXYTER) fe_typ = "nXYTER";
   if ((roc_typ >> 16) == kind_FEET) fe_typ = "FEET";

   const char* be_typ = "<unknown>";
   if ((roc_typ & 0xffff) == kind_Optic) be_typ = "Optic";
   if ((roc_typ & 0xffff) == kind_FX20) be_typ = "Eth-FX20";
   if ((roc_typ & 0xffff) == kind_FX40) be_typ = "Eth-FX40";
   if ((roc_typ & 0xffff) == kind_FX60) be_typ = "Eth-FX60";

   sprintf(sbuf, "ROC%u ROC HW version: %s; ROC type front:%s back:%s",
          fRocNumber, versionToString(roc_hw_ver), fe_typ, be_typ);
   ShowDebug(0, sbuf);

   return roc_hw_ver;
}

//----------------------------------------------------------------------------
//! Updates ROC number in ROC

void roc::Board::setRocNumber(uint32_t num)
{
   put(ROC_ROCID, num);
}

//----------------------------------------------------------------------------
//! Returns ROC number as stored in ROC

uint32_t roc::Board::getRocNumber()
{
   uint32_t num = 0;
   if (get(ROC_ROCID, num)==0)
      fRocNumber = num;
   return num;
}

//----------------------------------------------------------------------------
//! Returns the FPGA type the firmeware was syntesized for

uint32_t roc::Board::getRocFpgaType()
{
   uint32_t num = 0;
	get(ROC_FPGA_TYPE, num);
   return num;
}

//----------------------------------------------------------------------------
//! Returns subversion revision number of the ROC firmware source code

uint32_t roc::Board::getRocSvnRev()
{
   uint32_t num = 0;
   get(ROC_SVN_REVISION, num);
   return num;
}

//----------------------------------------------------------------------------
//! Returns the time the firmware was synthesized.
/*!
 *  The time is given in numbers of seconds since 1970-01-01 0:00 .
 */

uint32_t roc::Board::getRocBuildTime()
{
   uint32_t num = 0;
   get(ROC_BUILD_TIME, num);
   return num;
}

//----------------------------------------------------------------------------
//! Add an commands list entry .
/*!
 * This method sets commands list, which can be executed by
 * command activateCommandsList() or by issuing of DLM
 */

int roc::Board::uploadCommandsList(unsigned num, const OperList& lst, double tmout)
{
   if (num >= ROC_CMD_LST_NUMBER) {
      Debug(-1, "Wrong number %u of commands list", num);
      return ROC_ADDRESS_ERROR;
   }

   OperList setlst;

   unsigned addr = ROC_CMD_LST_MEM + num * ROC_CMD_LST_SIZE / ROC_CMD_LST_NUMBER;

   for (int n=0;n<lst.number();n++) {
      if (!lst.isput(n)) {
         Debug(-1, "Only PUT operations are supported by commands list");
         return ROC_VALUE_ERROR;
      }

      if (addr >= ROC_CMD_LST_MEM + ROC_CMD_LST_SIZE) {
         Debug(-1, "Commands list to long %d for slot %u", lst.number(), num);
         return ROC_VALUE_ERROR;
      }

      setlst.addPut(addr, ROC_CMD_LST_PUT | (lst.oper(n).addr & ROC_CMD_LST_ADDRMASK));
      addr+=4;
      setlst.addPut(addr, lst.oper(n).value);
      addr+=4;
   }

   // check if last list command not specified, add it

   if ((lst.number()>0) && (lst.oper(lst.number()-1).addr != ROC_CMD_LST_ACTIVE)) {

      if (addr >= ROC_CMD_LST_MEM + ROC_CMD_LST_SIZE) {
         Debug(-1, "Commands list to long %d for slot %u", lst.number(), num);
         return ROC_VALUE_ERROR;
      }

      setlst.addPut(addr, ROC_CMD_LST_PUT | ROC_CMD_LST_ACTIVE);
      addr+=4;
      setlst.addPut(addr, 0);
      addr+=4;
   }

   return operGen(setlst, tmout);
}

//----------------------------------------------------------------------------
//! Download commands list from the ROC.
/*!
 * Method reads specified commands list from the board.
 */

int roc::Board::downloadCommandsList(unsigned num, OperList& lst, double tmout)
{
   lst.clear();

   if (num >= ROC_CMD_LST_NUMBER) {
      Debug(-1, "Wrong number %u of commands list", num);
      return ROC_ADDRESS_ERROR;
   }

   OperList getlst;

   unsigned addr = ROC_CMD_LST_MEM + num * ROC_CMD_LST_SIZE / ROC_CMD_LST_NUMBER;

   unsigned lastaddr = addr + ROC_CMD_LST_SIZE / ROC_CMD_LST_NUMBER;

   while (addr<lastaddr) {
      getlst.addGet(addr);
      addr+=4;
   }

   int res = operGen(getlst, tmout);

   if (res!=0) return res;

   for (int n=0;n<getlst.number();n+=2) {
      uint32_t addr = getlst.oper(n).value;
      uint32_t value = getlst.oper(n+1).value;

      if ((addr & ROC_CMD_LST_PUT) == 0) {
         Debug(-1, "Commands list %u has non-Put operation", num);
         return ROC_VALUE_ERROR;
      }

      addr = addr & ROC_CMD_LST_ADDRMASK;
      if (addr==ROC_CMD_LST_ACTIVE) break;
      lst.addPut(addr, value);
   }

   return 0;
}


//----------------------------------------------------------------------------
//! Produce and upload start daq commands list.
/*!
 * This method produces commands list, which is executed
 * on the board when startDaq method is called. Actually, it is list number 0
 * \param reset_frontend defines if front-end specific reset commands will
 * be put into commands list. For nXYTER frontend, ROC_NX_TS_RESET will be done
 * \param reset_fifo defines if message FIFO will be cleared
 */

int roc::Board::uploadStartDaqCmdList(bool reset_frontend, bool reset_fifo)
{
   OperList lst;
   uint32_t hw_kind = getRocFrontendKind();

   if (hw_kind==kind_nXYTER) {

      if (reset_frontend && (getTransportKind() == kind_UDP)) {
         lst.addPut(ROC_NX_TS_RESET, 1);
         lst.addPut(ROC_NX_TS_RESET, 0);
      }
      if (reset_fifo) {
         lst.addPut(ROC_NX_FIFO_RESET, 1);
         lst.addPut(ROC_NX_FIFO_RESET, 0);
      }

   } else 

   if (hw_kind==kind_FEET) {

   	if (reset_frontend) {
           lst.addPut(ROC_FEET_RESET, 1);
	}
   	if (reset_fifo) {
	   lst.addPut(ROC_FEET_FIFO_RESET, 1);
	   lst.addPut(ROC_FEET_FIFO_RESET, 0);
	}
	lst.addPut(ROC_FEET_CMD_TO_FEET, 0x28);
	lst.addPut(ROC_FEET_CMD_TO_FEET, 0x2f);
   }

   if (getTransportKind() == kind_UDP)
      lst.addPut(ROC_ETH_START_DAQ, 1);
   else
      lst.addPut(ROC_OPTICS_START_DAQ, 1);

   return uploadCommandsList(0, lst, 3.);
}

//----------------------------------------------------------------------------
//! Produce and upload stop daq commands list.
/*!
 * For a moment, list contains only stopdaq command
 */

int roc::Board::uploadStopDaqCmdList()
{
   OperList lst;

   if (getTransportKind() == kind_UDP)
      lst.addPut(ROC_ETH_STOP_DAQ, 1);
   else
      lst.addPut(ROC_OPTICS_STOP_DAQ, 1);

   return uploadCommandsList(1, lst, 3.);
}


//! Sets to default values common ROC registers
/*!
 * For a moment, initializes start/stop commands lists
 */

int roc::Board::setToDefault()
{
   int res = uploadStartDaqCmdList();
   if (res==0) res = uploadStopDaqCmdList();
   return res;
}

//----------------------------------------------------------------------------
//! Invoke command list.
/*!
 * Invokes specified commands list and returns immediately.
 * Means, commands list may still be executed when new operation is submitted
 */

int roc::Board::invokeCommandsList(unsigned num, double tmout)
{
   return put(ROC_CMD_LST_NR, num, tmout);
}

//----------------------------------------------------------------------------
//! Add an entry to the ROC control space address mapping table.
/*!
 * This method is protected and used in the contructors to initialize
 * the table.
 * \sa findRocNameByAddress(), findRocAddressByName(), printRocAddressMap()
 */

void roc::Board::addRocAddrMapping(const char* name, uint32_t addr)
{
  std::string sname(name);
  fMapName2Addr[sname] = addr;
  fMapAddr2Name[addr]  = sname;
}

//----------------------------------------------------------------------------
//! Retrieve a ROC control space address by its symbolic name.
/*!
 * Locates in the ROC control space address mapping table the entry for
 * the name \a name and returns the associated address in \a addr.
 * \param name name to be looked up
 * \param addr address to be returned
 * \returns \c true if \a name was found, \c false otherwise
 * \sa findRocNameByAddress(), addRocAddrMapping(), printRocAddressMap()
 */

bool roc::Board::findRocAddressByName(const char* name, uint32_t& addr)
{
  mapn2a_iter_t it = fMapName2Addr.find(std::string(name));
  if (it != fMapName2Addr.end()) {
    addr = (*it).second;
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
//! Retrieve symbolic name of a ROC control space address.
/*!
 * Locates in the ROC control space address mapping table the entry for
 * the address \a addr and returns the associated sympolic name in \a name.
 * \param addr address to be looked up
 * \param name name to be returned
 * \returns \c true if \a addr was found, \c false otherwise
 * \sa findRocAddressByName(), addRocAddrMapping(), printRocAddressMap()
 */

bool roc::Board::findRocNameByAddress(uint32_t addr, const char*& name)
{
  mapa2n_iter_t it = fMapAddr2Name.find(addr);
  if (it != fMapAddr2Name.end()) {
    name = (*it).second.c_str();            // ok because entries never deleted
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------

void roc::Board::printRocAddressMap(std::ostream& os, bool byname)
{
  static boost::format fmt("0x%|08x| : %s");
  if (byname) {
    for (mapn2a_iter_t it=fMapName2Addr.begin(); 
                       it!=fMapName2Addr.end(); it++) {
      std::cout << fmt % (*it).second % (*it).first << std::endl;
    }

  } else {
    for (mapa2n_iter_t it=fMapAddr2Name.begin(); 
                       it!=fMapAddr2Name.end(); it++) {
      std::cout << fmt % (*it).first % (*it).second << std::endl;
    }
  }
  
}

//----------------------------------------------------------------------------
void roc::Board::fillAddrNames(std::list<std::string>& lst)
{
   for (mapn2a_iter_t it=fMapName2Addr.begin(); it!=fMapName2Addr.end(); it++)
      lst.push_back(it->first);
}

//----------------------------------------------------------------------------

roc::Board* roc::Board::Connect(const char* name, ClientRole role)
{
   for (int n=0; n<BoardConnector::NumConn(); n++)
      if (BoardConnector::Conn(n)) {
         roc::Board* brd = BoardConnector::Conn(n)->DoConnect(name, role);
         if (brd!=0) {
            brd->fConnector = BoardConnector::Conn(n);
            return brd;
         }
      }

   return 0;
}

//----------------------------------------------------------------------------

bool roc::Board::Close(Board* brd)
{
   if (brd==0) return false;

   BoardConnector* conn = brd->fConnector;

   if (conn==0) return false;

   return conn->DoClose(brd);
}

//----------------------------------------------------------------------------
//! Print a formatted debug message.
/*!
 * If the debug level \a lvl is less or equal to the verbosity level
 * setup with setVerbosity() a message will be printed with ShowDebug().
 * The message text is defined by the \c printf style format \a fmt and
 * variable length argument list.
 * \param lvl  debug level
 * \param fmt  format a la \c printf
 * \par Example:
 * \code
 *    Debug(-1, "Error reading Register %d on bus %d", reg, num);
 * \endcode
 */

void roc::Board::Debug(int lvl, const char *fmt, ...)
{
   if (!fmt || (lvl>fVerbosity)) return;

   va_list args;
   va_start(args, fmt);

   int  result(128), length(128);
   char *buffer = 0;
   while (result==length) {
      if (buffer) delete [] buffer;
      length *= 2;
      buffer = new char [length + 1];
      memset(buffer, 0, length + 1);
      result = vsnprintf(buffer, length, fmt, args);
      if (result<0) break;
   }

   ShowDebug(lvl, buffer);
   delete [] buffer;
   va_end(args);
}

//----------------------------------------------------------------------------
//! Print a debug message to \c stdout.
/*!
 * If the debug level \a lvl is less or equal to the verbosity level
 * setup with setVerbosity() the message text \a msg is printed to \c stdout.
 * \param lvl  debug level
 * \param msg  text message
 * \sa Debug()
 */

void roc::Board::ShowDebug(int lvl, const char* msg)
{
   if (lvl<=fVerbosity) printf("%s\n", msg);
}

//----------------------------------------------------------------------------
//! Generate output for get/put/operGen traces.
/*!
 * \param isput 'isput' array 
 * \param addr  'addr' array (or pointer to address in case of get/put)
 * \param value  'value' array (or pointer to value in case of get/put)
 * \param num request list length
 * \param rc return code
 */

void roc::Board::traceOper(OperList* lst, int rc)
{
   if (lst==0) return;
   if (fOperTrace == 0) return;
   if (fOperTrace == 1 && rc != kOperNetworkErr) return;
   if (fOperTrace == 2 && rc == 0) return;

   for (int i=0; i<lst->number(); i++) {
      const char* aname = "???";
      findRocNameByAddress(lst->oper(i).addr, aname);

      if (lst->number() == 1) {
         Debug(0, "%3s   (%d,%08x %-27s) %s 0x%08x %10d %s",
                  (lst->oper(i).isput ? "put" : "get"),
                  fRocNumber, lst->oper(i).addr, aname,
                  (lst->oper(i).isput) ? "<-" : "->",
                  lst->oper(i).value, lst->oper(i).value,
                  (rc == 0) ? "ok" : "FAIL");

      } else {
         const char* stat = "";
         if (i == lst->number()-1) stat = (rc == 0) ? "ok" : "FAIL";
         Debug(0, "op[%02d](%d,%08x %-27s) %s 0x%08x %10d %s",
               i, fRocNumber, lst->oper(i).addr, aname,
               (lst->oper(i).isput ? "<-" : "->"),
               lst->oper(i).value, lst->oper(i).value, stat);
      }

      if (rc != 0) {
         std::string str = operErrToString(rc);
         Debug(0, "   %s", str.c_str());
      }
   }
}

int roc::Board::operGen(bool* isput,
                        uint32_t* addr,
                        uint32_t* value,
                        int num,
                        double tmout)
{
   OperList lst(num, isput, addr, value);

   int res = operGen(lst, tmout);

   for (int n=0;n<num;n++)
      if (!isput[n]) value[n] = lst.oper(n).value;

   return res;
}


int roc::Board::put(uint32_t addr, uint32_t value, double tmout)
{
   bool isput = true;

   return operGen(&isput, &addr, &value, 1, tmout);
}

int roc::Board::get(uint32_t addr, uint32_t& value, double tmout)
{
   bool isput = false;

   return operGen(&isput, &addr, &value, 1, tmout);
}


//----------------------------------------------------------------------------
//! Put-Put accesses sequence to ROC control space.
/*!
 * This is a specialization of operGen(), check the documentation of this
 * method for details.
 *
 * \param addr0 address for 1st put
 * \param val0p value to write in 1st put
 * \param addr1 address for 2nd put
 * \param val1p value to write in 2nd put
 * \param tmout maximal network response time
 * \returns see \ref roc_board_opererr
 */

int roc::Board::operPP(uint32_t addr0, uint32_t  val0p,
                       uint32_t addr1, uint32_t  val1p,
                       double tmout)
{
  bool isput[2];
  uint32_t addr[2];
  uint32_t data[2];

  isput[0] = true;  addr[0] = addr0;    data[0] = val0p;
  isput[1] = true;  addr[1] = addr1;    data[1] = val1p;

  int rc = operGen(isput, addr, data, 2, tmout);
  return rc;
}

//----------------------------------------------------------------------------
//! Put-Get accesses sequence to ROC control space.
/*!
 * This is a specialization of operGen(), check the documentation of this
 * method for details.
 *
 * \param addr0 address for 1st put
 * \param val0p value to write in 1st put
 * \param addr1 address for 1st get
 * \param val1g value returned in 1st get
 * \param tmout maximal network response time
 * \returns see \ref roc_board_opererr
 */

int roc::Board::operPG(uint32_t addr0, uint32_t  val0p,
                       uint32_t addr1, uint32_t& val1g,
                       double tmout)
{
  bool isput[2];
  uint32_t addr[2];
  uint32_t data[2];

  isput[0] = true;  addr[0] = addr0;    data[0] = val0p;
  isput[1] = false; addr[1] = addr1;

  int rc = operGen(isput, addr, data, 2, tmout);
  val1g = data[1];
  return rc;
}

//----------------------------------------------------------------------------
//! Put-Put-Put accesses sequence to ROC control space.
/*!
 * This is a specialization of operGen(), check the documentation of this
 * method for details.
 *
 * \param addr0 address for 1st put
 * \param val0p value to write in 1st put
 * \param addr1 address for 2nd put
 * \param val1p value to write in 2nd put
 * \param addr2 address for 3rd put
 * \param val2p value to write in 3rd put
 * \param tmout maximal network response time
 * \returns see \ref roc_board_opererr
 */

int roc::Board::operPPP(uint32_t addr0, uint32_t  val0p,
                        uint32_t addr1, uint32_t  val1p,
                        uint32_t addr2, uint32_t  val2p,
                        double tmout)
{
  bool isput[3];
  uint32_t addr[3];
  uint32_t data[3];

  isput[0] = true;  addr[0] = addr0;    data[0] = val0p;
  isput[1] = true;  addr[1] = addr1;    data[1] = val1p;
  isput[2] = true;  addr[2] = addr2;    data[2] = val2p;

  int rc = operGen(isput, addr, data, 3, tmout);
  return rc;
}

//----------------------------------------------------------------------------
//! Get-Get-Get accesses sequence to ROC control space.
/*!
 * This is a specialization of operGen(), check the documentation of this
 * method for details.
 *
 * \param addr0 address for 1st get
 * \param val0p value to write in 1st get
 * \param addr1 address for 2nd get
 * \param val1p value to write in 2nd get
 * \param addr2 address for 3rd get
 * \param val2p value to write in 3rd get
 * \param tmout maximal network response time
 * \returns see \ref roc_board_opererr
 */

int roc::Board::operGGG(uint32_t addr0, uint32_t&  val0p,
                        uint32_t addr1, uint32_t&  val1p,
                        uint32_t addr2, uint32_t&  val2p,
                        double tmout)
{
  bool isput[3];
  uint32_t addr[3];
  uint32_t data[3];

  isput[0] = false;  addr[0] = addr0;
  isput[1] = false;  addr[1] = addr1;
  isput[2] = false;  addr[2] = addr2;

  int rc = operGen(isput, addr, data, 3, tmout);

  val0p = data[0];
  val1p = data[1];
  val2p = data[2];

  return rc;
}

//----------------------------------------------------------------------------
//! Put-Put-Put-Put accesses sequence to ROC control space.
/*!
 * This is a specialization of operGen(), check the documentation of this
 * method for details.
 *
 * \param addr0 address for 1st put
 * \param val0p value to write in 1st put
 * \param addr1 address for 2nd put
 * \param val1p value to write in 2nd put
 * \param addr2 address for 3rd put
 * \param val2p value to write in 3rd put
 * \param addr3 address for 4th put
 * \param val3p value to write in 4th put
 * \param tmout maximal network response time
 * \returns see \ref roc_board_opererr
 */

int roc::Board::operPPPP(uint32_t addr0, uint32_t  val0p,
                         uint32_t addr1, uint32_t  val1p,
                         uint32_t addr2, uint32_t  val2p,
                         uint32_t addr3, uint32_t  val3p,
                         double tmout)
{
  bool isput[4];
  uint32_t addr[4];
  uint32_t data[4];

  isput[0] = true;  addr[0] = addr0;    data[0] = val0p;
  isput[1] = true;  addr[1] = addr1;    data[1] = val1p;
  isput[2] = true;  addr[2] = addr2;    data[2] = val2p;
  isput[3] = true;  addr[3] = addr3;    data[3] = val3p;

  int rc = operGen(isput, addr, data, 4, tmout);
  return rc;
}


//----------------------------------------------------------------------------
//! Returns a string representation of a version number

const char* roc::Board::versionToString(uint32_t ver)
{
   static char sbuf[100];
   snprintf(sbuf, sizeof(sbuf), "%u.%u.%u.%u",
           (ver >> 24),
           (ver >> 16) & 0xff,
           (ver >> 8) & 0xff,
           ver & 0xff);
   return sbuf;
}

//----------------------------------------------------------------------------
//! Returns software version of client software

uint32_t roc::Board::getRoclibVersion()
{
   return KNUT_VERSION;
}

//----------------------------------------------------------------------------
//! Returns hardware version of ROC (FPGA firmware version)

uint32_t roc::Board::getRocHardwareVersion()
{
   uint32_t val = 0;
   get(ROC_HWV, val);
   return val;
}

//----------------------------------------------------------------------------
//! Returns hardware type (frontend/backend) of ROC (FPGA firmware kind)
/**
 * Frontend:
 *      1 - NX
 *      2 - FEET
 * Backend:
 *      1 - Optics
 *      2 - Ethernet FX20
 *      3 - Ethernet FX40
 */

uint32_t roc::Board::getRocHardwareType()
{
   uint32_t val = 0;
   get(ROC_TYPE, val);
   return val;
}

//----------------------------------------------------------------------------
//! Clears the FIFO in ROC (discard all old data)

void roc::Board::clearRocFifo()
{
   int rc = operPP(ROC_NX_FIFO_RESET, 1,
                   ROC_NX_FIFO_RESET, 0);
}

//----------------------------------------------------------------------------
//! Full ROC restart (FPGA and PPC, if used)
/*!
 * This methods sends a message to the ROC which causes a <b>full reset</b>
 * of the ROC board, the FPGA is reconfigured, and if used, the PPC is
 * rebooted. Due to this hard reset the connection between the Board object
 * and the ROC will abort, the Board object will essentially be a zombie
 * after restartRoc() is called.
 *
 * This method can come in handy when the ROC is in an inaccessible place
 * and partially hung.
 *
 * \post ROC restarting, Board object defunct.
 */

void roc::Board::restartRoc()
{
   put(ROC_SYSTEM_RESET, 1);
}

//----------------------------------------------------------------------------
//! Returns the status of the throttle system.
/*!
 * \param val bit mask with throttle system state
 *   \li bit 0 set indicates that throttling is active, thus data is currently
 *       been discarded.
 *   \li bit 1 set indicates that the throttle request flip-flop is set and that
 *       a throttle request is emitted from GPIO pin 0.
 *
 * \returns see \ref roc_board_opererr
 */

int roc::Board::getRocThrottleState(uint32_t& val)
{
   return get(ROC_NX_THROTTLE, val);
}

//----------------------------------------------------------------------------
//! Returns descriptive text for a get/put/oper error code.
/*!
 * \param rc return code, the index part will be ignored
 * \returns pointer to statically allocated string with descriptive text
 */

const char* roc::Board::operErrCodeName(int rc)
{
  switch(operErrCode(rc)) {
  case kOperSuccess:    return "success";
  case kOperAddrErr:    return "invalid address";
  case kOperValueErr:   return "value not allowed";
  case kOperStateErr:   return "operation not allowed";
  case kOperNetworkErr: return "communication error";
  case kOperVerifyErr:  return "readback verify error";
  case kOperBusErr:     return "secondary bus error";
  }
  return "";
}

//----------------------------------------------------------------------------
//! Returns a string with a human readable form of a get/put/oper return code.
/*!
 * Return codes of many roc or device handling methods return either a
 * negative number to indicate an illegal argument combination or state or
 * a positive number to indicate a control space access errors (see
 * \ref roc_board_opererr). This method converts the return code into a
 * human readable form. For negative values it simply returns "rc = nnn",
 * while for positive values it extracts the index and error code part of 
 * the return code \a rc and returns a string of the form
\verbatim
rc =    7,20 -> readback verify error
        |  |    |
        |  |    string representation of error code
        |  error code
        index part
\endverbatim
 */

std::string roc::Board::operErrToString(int rc)
{
  char buf[32];
  if (rc <0) {
    snprintf(buf, sizeof(buf), "rc = %d", rc);
    return std::string(buf);
  }
  
  snprintf(buf, sizeof(buf), "rc = %4d,%2d -> ",
           operErrIndex(rc), operErrCode(rc));
  std::string str(buf);
  return str+operErrCodeName(rc);
}

//----------------------------------------------------------------------------

roc::BoardConnector* roc::BoardConnector::gArr[10] = { 0, 0, 0, 0, 0,
                                                       0, 0, 0, 0, 0 };

//----------------------------------------------------------------------------

roc::BoardConnector::BoardConnector()
{
   for (int n=0;n<10;n++)
     if (gArr[n]==0) {
        gArr[n] = this;
        break;
     }
}

//----------------------------------------------------------------------------

roc::BoardConnector::~BoardConnector()
{
   for (int n=0;n<10;n++)
     if (gArr[n]==this) gArr[n] = 0;
}

//----------------------------------------------------------------------------

const char* roc::BoardConnector::CheckNamePrefix(const char* rocname, const char* prefix)
{
   if ((rocname==0) || (prefix==0) || (strlen(rocname)<strlen(prefix))) return 0;

   if (strstr(rocname,"://")==0) return 0;

   if (strncmp(rocname, prefix, strlen(prefix))==0) return rocname+strlen(prefix);

   return 0;
}

//----------------------------------------------------------------------------

//! Check if specified address is address of board, connected via optic
/*!
 * First checks that it has preffix optic://, second that name starts from abb
 * Also checks, that no udp:// prefix is specified
 * Returns connection name without prefix or 0 
 */

const char* roc::BoardConnector::CheckOptic(const char* name)
{
   if (CheckNamePrefix(name,"udp://")!=0) return 0;
   const char* res = CheckNamePrefix(name, "optic://");
   if (res==0) res = name;
   if (strlen(res)<3) return 0;
   if (strncmp(res,"abb",3)!=0) return 0;
   return res;
}

//----------------------------------------------------------------------------

//! Check if specified address is address of board, connected via UDP
/*!
 * First checks that it is not optic address
 * Also checks, that no udp:// prefix is specified 
 * Returns connection name without prefix or 0 
 */

const char* roc::BoardConnector::CheckUdp(const char* name)
{
   if (CheckFile(name)) return 0;
   if (CheckOptic(name)) return 0;
   const char* res = CheckNamePrefix(name, "udp://");
   if (res) return res;
   return (strstr(name,"://")==0) ? name : 0;
}

//----------------------------------------------------------------------------

//! Check if specified address is filename
/*!
 * First checks that it is not optic address
 * Also checks, that no udp:// prefix is specified 
 * Returns connection name without prefix or 0 
 */

const char* roc::BoardConnector::CheckFile(const char* name)
{
   const char* res = CheckNamePrefix(name, "file://");
   if (res!=0) return res;
   if (strstr(name, ".lmd")!=0) return name;
   return 0;
}

