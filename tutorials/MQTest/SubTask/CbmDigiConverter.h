/* 
 * File:   CbmDigiConverter.h
 * Author: winckler
 *
 * Created on July 30, 2014, 6:36 PM
 */

#ifndef CBMDIGICONVERTER_H
#define	CBMDIGICONVERTER_H


#include <iostream>
#include <memory>

#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"

#include "CbmUnpacker.h"
#include "CbmMicroSlice.h"

#include "CbmDetectorList.h"

#include "FairMQLogger.h"


class CbmDigiConverter 
{
public:
    CbmDigiConverter();
    virtual ~CbmDigiConverter();
    
    void SetPrintOption(uint32_t DigiToPrint, bool print);
    void PrintFlesMicroSliceHeader(const fles::MicrosliceDescriptor* MSdesc);
    void PrintInfo(const fles::MicrosliceDescriptor* MSdesc, const CbmDigi* Digi, uint32_t iDigi);
    virtual void PrintDigiContent(const CbmDigi* Digi)=0;
protected:
    virtual std::shared_ptr<CbmDigi> ConvertFlesPtrToDigi(uint32_t *offset, const uint8_t* FlesTimeSliceContent)=0;
    bool fPrint;
    uint32_t fDigiToPrint;
};

#endif	/* CBMDIGICONVERTER_H */

