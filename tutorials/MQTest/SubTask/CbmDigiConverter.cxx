/* 
 * File:   CbmDigiConverter.cxx
 * Author: winckler
 * 
 * Created on July 30, 2014, 6:36 PM
 */

#include "CbmDigiConverter.h"

CbmDigiConverter::CbmDigiConverter() : fPrint(false), fDigiToPrint(0)
{
}

CbmDigiConverter::~CbmDigiConverter() 
{
}


void CbmDigiConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
}

void CbmDigiConverter::PrintFlesMicroSliceHeader(const fles::MicrosliceDescriptor* MSdesc)
{
    
    uint8_t MSHeaderID=MSdesc->hdr_id;          ///< Header format identifier (0xDD)
    uint8_t MSHeaderVer=MSdesc->hdr_ver;        ///< Header format version (0x01)
    uint16_t MSComponent=MSdesc->eq_id;         ///< Equipment identifier
    //uint16_t MSFlags=MSdesc->flags;           ///< Status and error flags
    uint8_t MSDetectorId= MSdesc->sys_id;       ///< Subsystem identifier
    uint8_t MSSysVer=MSdesc->sys_ver;           ///< Subsystem format version
    uint64_t MSliceIndex=MSdesc->idx;           ///< Microslice index
    //uint32_t MSCRC=MSdesc->crc;               ///< CRC32 checksum
    uint32_t MSContentSize=MSdesc->size;        ///< Content size (bytes)
    uint64_t MSoffset=MSdesc->offset;           ///< Offset in event buffer (bytes)
    
    MQLOG(INFO) << "* Header format identifier (0xDD) = "   << (uint16_t)MSHeaderID;
    MQLOG(INFO) << "* Header format version (0x01) = "   << (uint16_t)MSHeaderVer;
    MQLOG(INFO) << "* Subsystem format version = "   << (uint16_t)MSSysVer;
    MQLOG(INFO) << "* Detector ID = "   << (uint16_t)MSDetectorId;
    MQLOG(INFO) << "* Component ID = "   << MSComponent;
    MQLOG(INFO) << "* Micro Slice index = "   << MSliceIndex;
    MQLOG(INFO) << "* Micro Slice Content size (bytes) = "   << MSContentSize;
    MQLOG(INFO) << "* Offset in event buffer (bytes) = "   << MSoffset;
    
}


void CbmDigiConverter::PrintInfo(const fles::MicrosliceDescriptor* MSdesc, const CbmDigi* Digi, uint32_t iDigi)
{
    Int_t iDet = Digi->GetSystemId();
    std::string strHeader;
    std::string strIdigi;
    switch (iDet) 
    {
        case kSTS:
        {
            strHeader="*********STS*********";
            strIdigi="th STS digi :";
            break;
        }
        case kMUCH:
        {
            strHeader="********MUCH*********";
            strIdigi="th MUCH digi :";
            break;
        }
        default:
        {
          strHeader="********UNKNOWN**********";
          strIdigi="th unknown detector digi :";
          break;
        }
    }
    
    MQLOG(INFO) << strHeader;
    MQLOG(INFO) <<"* --Header of current Micro Slice : ";
    PrintFlesMicroSliceHeader(MSdesc);
    MQLOG(INFO) << "* * * * * * * * * ";
    MQLOG(INFO) << "* --Content of the "<< iDigi <<strIdigi;
    PrintDigiContent(Digi);
    MQLOG(INFO) << "*********************";
}
