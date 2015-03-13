/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/* 
 * File:   CbmDataConverterTask.h
 * Author: winckler
 *
 * Created on July 18, 2014, 7:23 PM
 */

#ifndef CBMDATACONVERTERTASK_H
#define	CBMDATACONVERTERTASK_H

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "FairTask.h"

#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"

#include "CbmUnpacker.h"
#include "CbmMicroSlice.h"

#include "CbmDetectorList.h"
#include "CbmStsDigi.h"
#include "CbmMuchDigi.h"
#include "CbmDigiConverter.h"
#include "CbmMuchDataConverter.h"
#include "CbmStsDataConverter.h"

#include "FairMQLogger.h"

class CbmDataConverterTask : public FairTask
{
public:
    CbmDataConverterTask();
    virtual ~CbmDataConverterTask();
    //virtual void Exec(Option_t* opt);
    //virtual InitStatus Init();
    
    CbmMicroSlice BuildMicroSlice(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void PrintCbmMicroSlice(const CbmMicroSlice* MSlice, uint32_t DigiToPrint=0);
    
    /// stuff to convert cbm root timeslices to fles microslices
    CbmMicroSlice BuildMicroSlice(DetectorId iDet, CbmTimeSlice* CbmTSlice);
    
    /// stuff to convert fles microslices to cbm root timeslices
    void CbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    
    /// stuff to handle cbm root timeslices
    void InitCbmTS(Double_t start=0., Double_t duration=1000.);
    void InitCbmTSOutputFile(const char* filename);
    void FillCbmTSTree();
    void SetCbmTSInterval(Double_t start, Double_t duration);
    void WriteTreeToFile();
    
    void SetPrintOption(uint32_t DigiToPrint=0, bool print=true); //temporary
    
protected:
    
    TFile* fOutFile;
    TTree* fTree;
    CbmTimeSlice* fCBMTimeSlice;
    CbmStsDataConverter* fStsConverter;
    CbmMuchDataConverter* fMuchConverter;
    /// Start time of current time slice [ns]
    Double_t fCurrentStartTime;
    /// Duration of time slice [ns] 
    Double_t fDuration;

    bool fPrint; //temporary
    uint32_t fDigiToPrint; //temporary

    size_t fStsDigiPayloadSize;
    size_t fMuchDigiPayloadSize;

private:

    CbmDataConverterTask(const CbmDataConverterTask&);    
    CbmDataConverterTask operator=(const CbmDataConverterTask&);    
};

#endif	/* CBMDATACONVERTERTASK_H */

