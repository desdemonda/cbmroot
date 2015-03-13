#include "roc/UdpBoard.h"

#include <fstream>
#include <arpa/inet.h>
#include <string.h>

#include "roc/defines_roc.h"
#include "roc/defines_udp.h"

roc::UdpBoard::UdpBoard() :
   roc::Board()
{

   fRunningUpload = -1;
   fUploadChecksum = 0;

  // current ROC address def's from udpdefines.h

  addRocAddrMapping("ROC_ETH_HWV", ROC_ETH_HWV);
  addRocAddrMapping("ROC_ETH_SWV", ROC_ETH_SWV);
  addRocAddrMapping("ROC_ETH_OCM", ROC_ETH_OCM);
  addRocAddrMapping("ROC_ETH_CFG_READ", ROC_ETH_CFG_READ);
  addRocAddrMapping("ROC_ETH_CFG_WRITE", ROC_ETH_CFG_WRITE);
  addRocAddrMapping("ROC_ETH_FLASH_KIBFILE_FROM_DDR", ROC_ETH_FLASH_KIBFILE_FROM_DDR);
  addRocAddrMapping("ROC_ETH_SWITCHCONSOLE", ROC_ETH_SWITCHCONSOLE);
  addRocAddrMapping("ROC_ETH_START_DAQ", ROC_ETH_START_DAQ);
  addRocAddrMapping("ROC_ETH_STOP_DAQ", ROC_ETH_STOP_DAQ);
  addRocAddrMapping("ROC_ETH_IP_ADDRESS", ROC_ETH_IP_ADDRESS);
  addRocAddrMapping("ROC_ETH_NETMASK", ROC_ETH_NETMASK);
  addRocAddrMapping("ROC_ETH_MAC_ADDRESS_UPPER", ROC_ETH_MAC_ADDRESS_UPPER);
  addRocAddrMapping("ROC_ETH_MAC_ADDRESS_LOWER", ROC_ETH_MAC_ADDRESS_LOWER);
  addRocAddrMapping("ROC_ETH_MASTER_LOGIN", ROC_ETH_MASTER_LOGIN);
  addRocAddrMapping("ROC_ETH_MASTER_DATAPORT", ROC_ETH_MASTER_DATAPORT);
  addRocAddrMapping("ROC_ETH_MASTER_CTRLPORT", ROC_ETH_MASTER_CTRLPORT);
  addRocAddrMapping("ROC_ETH_BUFFER_FLUSH_TIMER", ROC_ETH_BUFFER_FLUSH_TIMER);
  addRocAddrMapping("ROC_ETH_MASTER_LOGOUT", ROC_ETH_MASTER_LOGOUT);
  addRocAddrMapping("ROC_ETH_MASTER_IP", ROC_ETH_MASTER_IP);
  addRocAddrMapping("ROC_ETH_RESTART_NETWORK", ROC_ETH_RESTART_NETWORK);
  addRocAddrMapping("ROC_ETH_CONSOLE_OUTPUT", ROC_ETH_CONSOLE_OUTPUT);
  addRocAddrMapping("ROC_ETH_LOST_ETHER_FRAMES", ROC_ETH_LOST_ETHER_FRAMES);
  addRocAddrMapping("ROC_ETH_UKNOWN_ETHER_FRAMES", ROC_ETH_UKNOWN_ETHER_FRAMES);
  addRocAddrMapping("ROC_ETH_SUSPEND_DAQ", ROC_ETH_SUSPEND_DAQ);
  addRocAddrMapping("ROC_ETH_FLASH_CLEAR_FILEBUFFER", ROC_ETH_FLASH_CLEAR_FILEBUFFER);
  addRocAddrMapping("ROC_ETH_CHECK_BITFILEBUFFER", ROC_ETH_CHECK_BITFILEBUFFER);
  addRocAddrMapping("ROC_ETH_CHECK_BITFILEFLASH0", ROC_ETH_CHECK_BITFILEFLASH0);
  addRocAddrMapping("ROC_ETH_CHECK_BITFILEFLASH1", ROC_ETH_CHECK_BITFILEFLASH1);
  addRocAddrMapping("ROC_ETH_CHECK_FILEBUFFER", ROC_ETH_CHECK_FILEBUFFER);
  addRocAddrMapping("ROC_ETH_TEMAC_PRINT", ROC_ETH_TEMAC_PRINT);
  addRocAddrMapping("ROC_ETH_TEMAC_REG0", ROC_ETH_TEMAC_REG0);
  addRocAddrMapping("ROC_ETH_OVERWRITE_SD_FILE", ROC_ETH_OVERWRITE_SD_FILE);
  addRocAddrMapping("ROC_ETH_CONSOLE_CMD", ROC_ETH_CONSOLE_CMD);
  addRocAddrMapping("ROC_ETH_CTRLPORT", ROC_ETH_CTRLPORT);
  addRocAddrMapping("ROC_ETH_DATAPORT", ROC_ETH_DATAPORT);
  addRocAddrMapping("ROC_ETH_BURST_LOOPCNT", ROC_ETH_BURST_LOOPCNT);
  addRocAddrMapping("ROC_ETH_OCM_LOOPCNT", ROC_ETH_OCM_LOOPCNT);
  addRocAddrMapping("ROC_ETH_STATBLOCK", ROC_ETH_STATBLOCK);
  addRocAddrMapping("ROC_ETH_DEBUGMSG", ROC_ETH_DEBUGMSG);
  addRocAddrMapping("ROC_ETH_HIGHWATER", ROC_ETH_HIGHWATER);
  addRocAddrMapping("ROC_ETH_LOWWATER", ROC_ETH_LOWWATER);
  addRocAddrMapping("ROC_ETH_NUMBUFALLOC", ROC_ETH_NUMBUFALLOC);
}

roc::UdpBoard::~UdpBoard()
{
}

const char* roc::xmlTransferWindow = "RocTransferWindow";


//----------------------------------------------------------------------------

uint32_t roc::UdpBoard::getRocBackendVersion()
{
   uint32_t val;
   get(ROC_ETH_HWV, val);
   return val;
}

//----------------------------------------------------------------------------

uint32_t roc::UdpBoard::getPowerPCVersion()
{
   uint32_t val;
   get(ROC_ETH_SWV, val);
   return val;
}


//----------------------------------------------------------------------------

int roc::UdpBoard::invokeDLM(unsigned num, double tmout)
{
   return put(ROC_ETH_DLM, num, tmout);
}

//----------------------------------------------------------------------------

void roc::UdpBoard::BURST(uint32_t val)
{
   put(ROC_ETH_OCM, val);
}

//----------------------------------------------------------------------------

bool roc::UdpBoard::getRocLowHighWater(int& lowWater, int& highWater)
{
   uint32_t low, high;
   if (get(ROC_ETH_LOWWATER, low) != 0) return false;
   if (get(ROC_ETH_HIGHWATER, high) != 0) return false;

   double k = 1024. / (UDP_PAYLOAD_OFFSET + MAX_UDP_PAYLOAD);

   lowWater = int(low / k);
   highWater = int(high / k);

   if (low/k - lowWater >= 0.5) lowWater++;
   if (high/k - highWater >= 0.5) highWater++;

   return true;
}

//----------------------------------------------------------------------------

bool roc::UdpBoard::setRocLowHighWater(int lowWater, int highWater)
{
   double k = 1024. / (UDP_PAYLOAD_OFFSET + MAX_UDP_PAYLOAD);

   uint32_t low = uint32_t(lowWater * k);
   uint32_t high = uint32_t(highWater * k);

   if (lowWater*k - low >= 0.5) low++;
   if (highWater*k - high >= 0.5) high++;

   if ((low < 10) || (high < low + 10)) {
      Debug(-1, "Wrong values for high/low water markers");
      return false;
   }

   // to avoid error messages, just move high limit before setting new low limit
   uint32_t curr_high;
   if (get(ROC_ETH_HIGHWATER, curr_high) != 0) return false;
   if (low > curr_high)
      if (put(ROC_ETH_HIGHWATER, low+1) != 0) return false;

   if (put(ROC_ETH_LOWWATER, low) !=0) return false;
   if (put(ROC_ETH_HIGHWATER, high) != 0) return false;

   return true;
}

//----------------------------------------------------------------------------

bool roc::UdpBoard::getRocBufferFlushTimer(uint32_t& bufferFlushTimer)
{
  if (get(ROC_ETH_BUFFER_FLUSH_TIMER, bufferFlushTimer) != 0) return false;
  return true;
}

//----------------------------------------------------------------------------

bool roc::UdpBoard::setRocBufferFlushTimer(uint32_t bufferFlushTimer)
{
  if (put(ROC_ETH_BUFFER_FLUSH_TIMER, bufferFlushTimer) != 0) return false;
  return true;
}

//----------------------------------------------------------------------------

void roc::UdpBoard::setConsoleOutput(bool terminal, bool network)
{
   put(ROC_ETH_CONSOLE_OUTPUT, (terminal ? 1 : 0) | (network ? 2 : 0));
}

//----------------------------------------------------------------------------

void roc::UdpBoard::switchToConsole()
{
   put(ROC_ETH_SWITCHCONSOLE, 1);
}

//----------------------------------------------------------------------------

int roc::UdpBoard::parseBitfileHeader(char* pBuffer, unsigned int nLen)
{
   char* binBuffer;

   if (nLen < 16) {
      Debug(-1, "parseBitfileHeader: Invalid filesize");
      return -1;
   }

   if (*((unsigned short*) pBuffer) == 0x0900 && *((unsigned int*) (pBuffer + 2)) == 0xF00FF00F && *((unsigned int*) (pBuffer + 6)) == 0xF00FF00F) {
      Debug(1, "parseBitfileHeader: Bitfile found, skipping header...");
      if (*((unsigned int*) (pBuffer + 10)) != 0x61010000)
      {
         Debug(-1, "parseBitfileHeader: Corrupt file");
         return -1;
      }

      unsigned int tmpLen = ntohs(*((unsigned short*) (pBuffer + 14)));
      unsigned int tmpPos = 13;

      for (int i = 0;i < 4;i++)
      {
         if (nLen < tmpPos + 5 + tmpLen)
         {
            Debug(-1, "parseBitfileHeader: Invalid filesize");
            return -1;
         }

         tmpPos += 3 + tmpLen;
         if (*((unsigned char*) (pBuffer + tmpPos)) != 0x62 + i) {
            Debug(-1, "parseBitfileHeader: Corrupt Bitfile");
            return -1;
         }
         tmpLen = ntohs(*((unsigned short*) (pBuffer + tmpPos + 1)));
      }

      binBuffer = pBuffer + tmpPos + 5;
   } else {
      Debug(-1, "parseBitfileHeader: No Bitfile header found");
      binBuffer = pBuffer;
   }

   if (*((unsigned int*) (binBuffer)) != 0xFFFFFFFF || *((unsigned int*) (binBuffer + 4)) != 0x665599AA) {
      Debug(-1, "parseBitfileHeader: Corrupt Binfile");
      return -1;
   }
   Debug(1, "parseBitfileHeader: Binfile checked");

   return binBuffer - pBuffer -1;  //-1 bugfix stefan Mueller-Klieser
}

//----------------------------------------------------------------------------

uint8_t roc::UdpBoard::calcBinXOR(const char* filename)
{
   char* bitfileBuffer = new char[SC_BITFILE_BUFFER_SIZE]; //bitfile is a binfile + header
   unsigned int size=0;
   int i = 0;

   Debug(0, "Loading bitfile %s", filename);

   std::ifstream bitfile;
   bitfile.open(filename, std::ios_base::in);
   if(!bitfile) {
      Debug(-1, "calcBinXOR: Cannot open file: %s", filename);
      return 0;
   }

   uint8_t XORCheckSum = 0;

   while(!bitfile.eof())
   {
      bitfile.read(bitfileBuffer + size++, 1);
   }
   size--;//eof is +1

   int nBinPos = parseBitfileHeader(bitfileBuffer, size);
   if (nBinPos == -1)
      Debug(-1, "calcBinXOR: File does not seem to be a bit or bin file");

   Debug(0, "Bitfilesize: %u + 512 bytes for the KIB header minus %d bytes for removed bitfile header", size, nBinPos);

   int bytes = 0;
   for(i = 0; i < 6;i++)//(int)(size - nBinPos); i++)
   {
      XORCheckSum ^= (uint8_t)bitfileBuffer[nBinPos + i];
      bytes++;
   }
   Debug(0, "check started at %d", nBinPos);
   Debug(0, "XOR is %u bytes checkes %d", XORCheckSum, bytes);

   delete [] bitfileBuffer;

   return XORCheckSum;
}

//----------------------------------------------------------------------------

bool roc::UdpBoard::sendConsoleCommand(const char* cmd)
{
   unsigned length = cmd ? strlen(cmd) + 1 : 0;

   if ((length<2) || (length > sizeof(UdpMessageFull) - sizeof(UdpMessage))) return false;

   return rawPut(ROC_ETH_CONSOLE_CMD, length, cmd) == 0;
}

bool roc::UdpBoard::saveConfig(const char* filename)
{
   uint32_t len = filename ? strlen(filename) + 1 : 0;

   Debug(0, "Save config file %s on ROC",filename ? filename : "");

   return rawPut(ROC_ETH_CFG_WRITE, len, filename, 10.) == 0;
}

bool roc::UdpBoard::loadConfig(const char* filename)
{
   uint32_t len = filename ? strlen(filename) + 1 : 0;

   Debug(0, "Load config file %s on ROC",filename ? filename : "");

   return rawPut(ROC_ETH_CFG_READ, len, filename, 10.) == 0;
}

//----------------------------------------------------------------------------

bool roc::UdpBoard::uploadDataToRoc(char* buf, unsigned datalen)
{
   put(ROC_ETH_FLASH_CLEAR_FILEBUFFER, 1);

   uint32_t maxsendsize = sizeof(UdpMessageFull) - sizeof(UdpMessage);

   unsigned pos = 0;

   while (pos < datalen) {
      uint32_t sendsize = datalen - pos;
      if (sendsize > maxsendsize) sendsize = maxsendsize;

      if (rawPut(ROC_ETH_FLASH_DATA, sendsize, buf + pos)!=0) {
         Debug(-1, "uploadDataToRoc:: packet error with addr %u", pos);
         return false;
      }

      pos += sendsize;
   }
   return true;
}

//----------------------------------------------------------------------------

int roc::UdpBoard::uploadSDfile(const char* filename, const char* sdfilename)
{
   if (sdfilename == 0) sdfilename = filename;

   if ((filename==0) || (strlen(filename)==0)) return -1;

   std::ifstream bfile;
   bfile.open(filename, std::ios_base::in);

   if(!bfile) {
      Debug(-1, "uploadSDfile: Cannot open file: %s", filename);
      return -1;
   }

   bfile.seekg(0, std::ios::end);
   unsigned bufSize = bfile.tellg();
   if (bufSize == 0) {
      Debug(-1, "uploadSDfile: File is empty: %s", filename);
      return -1;
   }

   bfile.seekg(0, std::ios::beg);
   char* buffer = new char [bufSize + 1024];
   if (buffer == 0) {
      Debug(-1, "uploadSDfile: Memory allocation error!");
      return -1;
   }
   memset(buffer, 0, bufSize + 1024);
   bfile.read(buffer + 1024, bufSize);
   bfile.close();

   *((uint32_t*) buffer) = htonl(bufSize);

   memcpy(buffer + 4, sdfilename, strlen(sdfilename) + 1);

   bufSize+=1024;

   uint8_t XORCheckSum = 0;

   for(unsigned i = 0; i < bufSize; i++)
      XORCheckSum ^= buffer[i];

   if (!uploadDataToRoc(buffer, bufSize)) {
      Debug(-1, "uploadSDfile: Fail to upload file %s to the ROC\n", filename);
      delete [] buffer;
      return -1;
   }

   uint32_t ROCchecksum(0);
   get(ROC_ETH_CHECK_FILEBUFFER, ROCchecksum);

   delete[] buffer;

   if (ROCchecksum != XORCheckSum) {
      Debug(-1, "uploadSDfile: File XOR checks sum %u differ from uploaded %u",
             XORCheckSum, ROCchecksum);
      return -1;
   }

   // one need about 1.5 sec to write single 4K block on SD-card :((
   double waittime = ((bufSize - 1024) / 4096 + 1.) * 1.5;

   Debug(0, "Start %s file writing, please wait ~%2.0f sec", sdfilename, waittime);

   if (!submitSwPut(ROC_ETH_OVERWRITE_SD_FILE, 1, waittime + 30.)) return -1;

   fRunningUpload = 3;
   fUploadChecksum = 0;

   return int(waittime + 40.);

   /*
   // take 30 sec more for any unexpectable delays
   int res = put(ROC_OVERWRITE_SD_FILE, 1, waittime + 30.);
   if (res==0)
      Debug(0, "File %s was overwritten on SD card", sdfilename);
   else
      Debug(-1, "uploadSDfile: Fail to overwrite file %s on SD card", sdfilename);

   return res == 0;

   */
}

//----------------------------------------------------------------------------

int roc::UdpBoard::uploadBitfile(const char* filename, int position)
{
   Debug(0, "Start upload of file %s, position: %d ...\n", filename, position);

   //bitfile is a binfile + header
   if(position > 1) {
      Debug(-1, "uploadBitfile: Only fileposition 0 and 1 is supported, sorry!");
      return -1;
   }

   std::ifstream bitfile;
   bitfile.open(filename, std::ios_base::in);

   if(!bitfile) {
      Debug(-1, "uploadBitfile: Cannot open file: %s", filename);
      return -1;
   }

   ISEBinfileHeader hdr;
   memset(&hdr, 0, sizeof(hdr));
   strcpy((char*)hdr.ident, "XBF");
   hdr.headerSize = htonl(sizeof(hdr));
   memcpy(hdr.bitfileName, filename, 64);
   hdr.timestamp = 0;
   hdr.XORCheckSum = 0;

   bitfile.seekg(0, std::ios::end);
   unsigned bitfileSize  = bitfile.tellg();
   bitfile.seekg(0, std::ios::beg);
   char* bitfileBuffer = new char [bitfileSize];
   if (bitfileBuffer == 0) {
      Debug(-1, "uploadBitfile: Memory allocation error!");
      return -1;
   }
   memset(bitfileBuffer, 0, bitfileSize);
   bitfile.read(bitfileBuffer, bitfileSize);
   bitfile.close();
   //cout << "Bitfile loaded, size is " << bitfileSize << " bytes." << endl;

   int nBinPos = parseBitfileHeader(bitfileBuffer, bitfileSize);
   if (nBinPos == -1) {
      Debug(-1, "uploadBitfile: File does not seem to be a bit or bin file");
      delete [] bitfileBuffer;
      return -1;
   }

   //cout << "Bitfilesize: " << bitfileSize <<  " + 512 bytes for the KIB header minus " << nBinPos << " bytes for removed bitfile header" << endl;
   unsigned binfileSize = bitfileSize - nBinPos;
   Debug(0, "Binfile loaded, size is %u bytes.", binfileSize);

   for(unsigned i = 0; i < binfileSize; i++)
      hdr.XORCheckSum ^= bitfileBuffer[nBinPos + i];
   //cout << "XOR 8bit over bin file is " << (int)hdr.XORCheckSum << endl;

   hdr.binfileSize = htonl(binfileSize);

   uint32_t uploadBufferSize = sizeof(hdr) + binfileSize;

   char* buffer = new char[uploadBufferSize];
   if (buffer == 0) {
      delete [] bitfileBuffer;
      return -1;
   }
   memset(buffer, 0, uploadBufferSize);

   memcpy(buffer, &hdr, sizeof(hdr));
   memcpy(buffer + sizeof(hdr), bitfileBuffer + nBinPos, binfileSize);

   uint8_t XORbitfileCheckSum = 0;
   for(unsigned n = 0; n < uploadBufferSize; n++)
      XORbitfileCheckSum ^= buffer[n];

   Debug(0, "XOR 8bit over Kib file is %u", XORbitfileCheckSum);

   Debug(0, "Now uploading Bitfile to ROC.");

   ISEBinfileHeader* testhdr = (ISEBinfileHeader*) buffer;

   uint32_t testlen = ntohl(testhdr->binfileSize) + ntohl(testhdr->headerSize);
   if (testlen != uploadBufferSize)
      Debug(-1, "uploadBitfile: Size mismatch %u %u", testlen, uploadBufferSize);

   if (!uploadDataToRoc(buffer, uploadBufferSize)) {
      Debug(-1, "uploadBitfile: Fail to upload file to the ROC");
      delete [] bitfileBuffer;
      delete [] buffer;
      return -1;
   }

   uint32_t ROCchecksum(0);

   get(ROC_ETH_CHECK_BITFILEBUFFER, ROCchecksum);

   if(ROCchecksum != XORbitfileCheckSum) {
      Debug(-1, "uploadBitfile: checksum error between PowerPC and file !!!");
      delete [] bitfileBuffer;
      delete [] buffer;
      return -1;
   }

   int upload_time = 170 * bitfileSize / 905408; // empiric value

   Debug(0, "Start flashing - wait ~ %d s.", upload_time);

   if (!submitSwPut(ROC_ETH_FLASH_KIBFILE_FROM_DDR, position, upload_time + 30)) return -1;

   fRunningUpload = position+1;
   fUploadChecksum = XORbitfileCheckSum;

   return upload_time + 40;


/*
   ROCchecksum = ~XORbitfileCheckSum;

   if (put(ROC_FLASH_KIBFILE_FROM_DDR, position, 300.)==0) {
      if(position == 0)
         get(ROC_CHECK_BITFILEFLASH0, ROCchecksum);
      else
         get(ROC_CHECK_BITFILEFLASH1, ROCchecksum);
   }

   delete [] bitfileBuffer;
   delete [] buffer;

   bool res(false);

   if(ROCchecksum != XORbitfileCheckSum) {
      Debug(-1, "uploadBitfile: --- Flashing finished with error! ---");
      res = false;
   } else {
      Debug(0, "--- Flashing finished successfully! ---");
      res = true;
   }

   return res;

*/
}

int roc::UdpBoard::checkUpload()
{
   if (fRunningUpload==0) return 0;

   int res = checkSwPut();
   if (res==1) return 1;

   uint32_t ROCchecksum(0);

   if (res==2)
      switch (fRunningUpload) {
         case 1:
            get(ROC_ETH_CHECK_BITFILEFLASH0, ROCchecksum);
            if (ROCchecksum != fUploadChecksum) res = -1;
            break;
         case 2:
            get(ROC_ETH_CHECK_BITFILEFLASH1, ROCchecksum);
            if (ROCchecksum != fUploadChecksum) res = -1;
            break;
         case 3: break;
      }

   fRunningUpload = 0;
   fUploadChecksum = 0;

   return res;
}

