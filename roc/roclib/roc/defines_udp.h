#ifndef ROC_DEFINES_UDP_H
#define ROC_DEFINES_UDP_H

#define ROC_PASSWORD               832226211

//ROC software
#define ROC_ETH_OCM                 0x110090   // r/w  activate OPB-2-OCM-Prefetcher



//sdcard
#define ROC_ETH_CFG_READ            0x110060   // w    Beim Schreiben einer 1 werden die Konfigurationsdaten (IP, MAC, Delays) von der SD-Karte gelesen
#define ROC_ETH_CFG_WRITE           0x110064   // w    Beim Schreiben einer 1 werden die Konfigurationsdaten (IP, MAC, Delays) auf der SD-Karte gespeichert
#define ROC_ETH_OVERWRITE_SD_FILE   0x110068 // w
//network
#define ROC_ETH_SWITCHCONSOLE       0x110100 // r/w
#define ROC_ETH_START_DAQ           0x100100 // w
#define ROC_ETH_STOP_DAQ            0x100104 // w
#define ROC_ETH_SUSPEND_DAQ         0x100108 // w go into suspend state
#define ROC_ETH_IP_ADDRESS          0x110000 // r/w
#define ROC_ETH_NETMASK             0x110004
#define ROC_ETH_MAC_ADDRESS_UPPER   0x110008 // r/w upper two bytes
#define ROC_ETH_MAC_ADDRESS_LOWER   0x11000C // r/w lower four bytes
#define ROC_ETH_MASTER_LOGIN        0x110010 // r/w
#define ROC_ETH_MASTER_DATAPORT     0x110014 // r/w  data port of master node
#define ROC_ETH_MASTER_CTRLPORT     0x110018 // r/w
#define ROC_ETH_BUFFER_FLUSH_TIMER  0x11001C // r/w
#define ROC_ETH_MASTER_LOGOUT       0x110020 // w
#define ROC_ETH_MASTER_IP           0x110024 // r
#define ROC_ETH_RESTART_NETWORK     0x110028 // w
#define ROC_ETH_CONSOLE_OUTPUT      0x11002C // w
#define ROC_ETH_LOST_ETHER_FRAMES   0x110030 // r ethernet frames lost
#define ROC_ETH_UKNOWN_ETHER_FRAMES 0x110034 // r number of ethernet frames with tag other than IP and ARP
#define ROC_ETH_TEMAC_PRINT         0x11003C // w - just print some Temac registers and config
#define ROC_ETH_TEMAC_REG0          0x110040 // r/w - set or read Remac config register
#define ROC_ETH_CTRLPORT            0x110044 // r/w - control port number
#define ROC_ETH_DATAPORT            0x110048 // r/w - data port number
#define ROC_ETH_BURST_LOOPCNT       0x11004C // r/w - number of readout in burst loops
#define ROC_ETH_OCM_LOOPCNT         0x110050 // r/w - number of readout in ocm loops
#define ROC_ETH_CONSOLE_CMD         0x11006C // w - executes provided console command
#define ROC_ETH_STATBLOCK           0x110070 // r - block with sc_statistic
#define ROC_ETH_DEBUGMSG            0x110074 // r - last output of sc_printf command
#define ROC_ETH_HIGHWATER           0x110080 // r/w - value of high water marker (in number of UDP buffers, maximum is about 78000 buffers or 112 MB)
#define ROC_ETH_LOWWATER            0x110084 // r/w - value of low water marker (in number of UDP buffers, default is 10% of highwater)
#define ROC_ETH_NUMBUFALLOC         0x110088 // r - number of allocated UDP buffers

//Flash
#define ROC_ETH_FLASH_KIBFILE_FROM_DDR  0x120000 // w position value is parsed right now only 0 is supported
#define ROC_ETH_FLASH_ADDR              0x120004 // w - set current FlashFileBuffer-Address Pointer
#define ROC_ETH_FLASH_DATA              0x120008 // r/w - write rawData to FlashFileBuffer and Increment Pointer / read Data from FlashFileBuffer and Pointer++;
#define ROC_ETH_FLASH_CLEAR_FILEBUFFER  0x12000C // w - clears the filebuffer, sets FlashFileBuffer-Address Pointer to 0
#define ROC_ETH_CHECK_BITFILEBUFFER     0x120010 // r
#define ROC_ETH_CHECK_BITFILEFLASH0     0x120014 // r
#define ROC_ETH_CHECK_BITFILEFLASH1     0x120018 // r
#define ROC_ETH_CHECK_FILEBUFFER        0x120020 // r

#define ROC_ETH_DLM                     0x130000 // w - create a DLM-Message (1=Timestamp-Reset, 4=Custom-Message)



/************************************
 * address space finished
 ***********************************/





// default ports number of ROC itself
#define ROC_DEFAULT_CTRLPORT        13132
#define ROC_DEFAULT_DATAPORT        13131

//ROC TAGS
#define ROC_PEEK                    111
#define ROC_POKE                    112
#define ROC_CONSOLE                 113
#define ROC_NOPER                   114

#define NOPER_PUT                   0x80000000    // to write address, one should set high LSB
#define NOPER_GET                   0x0           // if address not modified, just do get operation
#define NOPER_OPER_MASK             0x80000000
#define NOPER_ADDR_MASK             0x7fffffff


#define UDP_PAYLOAD_OFFSET 42
#define MAX_UDP_PAYLOAD 1472
#define MESSAGES_PER_PACKET 243

//4mb buffer as in syscore for file transfer
#define SC_BITFILE_BUFFER_SIZE 4194304


#include <stdint.h>

#pragma pack(push, 1)

namespace roc {

   struct UdpMessage
   {
      uint8_t  tag;
      uint8_t  pad[3];
      uint32_t password;
      uint32_t id;
      uint32_t address;
      uint32_t value;
   };

   struct UdpMessageFull : public UdpMessage
   {
      uint8_t rawdata[MAX_UDP_PAYLOAD - sizeof(UdpMessage)];
   };

   struct UdpDataRequest
   {
      uint32_t password;
      uint32_t reqpktid;
      uint32_t frontpktid;
      uint32_t tailpktid;
      uint32_t numresend;
   };

   struct UdpDataRequestFull : public UdpDataRequest
   {
      uint32_t resend[(MAX_UDP_PAYLOAD - sizeof(struct UdpDataRequest)) / 4];
   };

   struct UdpDataPacket
   {
      uint32_t pktid;
      uint32_t lastreqid;
      uint32_t nummsg;
   // here all messages should follow
   };

   struct UdpDataPacketFull : public UdpDataPacket
   {
      uint8_t msgs[MAX_UDP_PAYLOAD - sizeof(struct UdpDataPacket)];
   };

   struct BoardStatistic {
      uint32_t   dataRate;   // data taking rate in  B/s
      uint32_t   sendRate;   // network send rate in B/s
      uint32_t   recvRate;   // network recv rate in B/s
      uint32_t   nopRate;    // double-NOP messages 1/s
      uint32_t   frameRate;  // unrecognized frames 1/s
      uint32_t   takePerf;   // relative use of time for data taking (*100000)
      uint32_t   dispPerf;   // relative use of time for packets dispatching (*100000)
      uint32_t   sendPerf;   // relative use of time for data sending (*100000)
      uint32_t   daqState;   // current daq state
   };


   /** ISE Binfile header
    * A bitfile from Xilinx ISE consists of a binfile and a fileheader.
    * We need only a bin file for reprogramming the virtex. So
    * we use this header in front of the bin file
    * to store it on the Actel Flash. Its size is 512 bytes.
    * Behind the Header, the binfile will be written to the flash */

   union ISEBinfileHeader {
      struct {
         uint8_t ident[4];
         uint32_t headerSize;
         uint32_t binfileSize;
         uint8_t XORCheckSum;
         uint8_t bitfileName[65];
         uint32_t timestamp;
      };
      uint8_t bytes[512];
   };

}

#pragma pack(pop)


#endif
