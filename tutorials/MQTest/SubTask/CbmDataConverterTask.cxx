/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/* 
 * File:   CbmDataConverterTask.cxx
 * Author: winckler
 * 
 * Created on July 18, 2014, 7:23 PM
 */

#include "CbmDataConverterTask.h"


CbmDataConverterTask::CbmDataConverterTask() : FairTask("CbmDataConverterTask"),
        fOutFile(NULL),
        fTree(NULL),
        fCBMTimeSlice(NULL),
        fStsConverter(new CbmStsDataConverter()),
        fMuchConverter(new CbmMuchDataConverter()),
        fCurrentStartTime (0.),
        fDuration (1000.),
        fPrint(false),
        fDigiToPrint(0),
        fStsDigiPayloadSize(fStsConverter->GetDigiPayloadSize()),
        fMuchDigiPayloadSize(fMuchConverter->GetDigiPayloadSize())
{
}


CbmDataConverterTask::~CbmDataConverterTask() 
{
    if(fCBMTimeSlice)
    {
        delete fCBMTimeSlice;
        fCBMTimeSlice=NULL;
    }
    
    if(fMuchConverter)
    {
        delete fMuchConverter;
        fMuchConverter=NULL;
    }
    
    if(fStsConverter)
    {
        delete fStsConverter;
        fStsConverter=NULL;
    }
    
}

/*
void Exec(Option_t* opt)
{

}
// /
InitStatus Init()
{

 return kSUCCESS;
}
// */

void CbmDataConverterTask::InitCbmTS(Double_t start, Double_t duration)
{
    fCurrentStartTime=start;
    fDuration=duration;
    if(!fCBMTimeSlice)
        fCBMTimeSlice = new CbmTimeSlice(fCurrentStartTime,fDuration);
    else
        fCBMTimeSlice->Reset(fCurrentStartTime, fDuration);
}

void CbmDataConverterTask::InitCbmTSOutputFile(const char* filename)
{
    
    fOutFile = new TFile(filename,"recreate");
    if(fCBMTimeSlice)
    {
        fTree = new TTree("CbmMQOut", "Test output");
        fTree->Branch("TimeSlice.","CbmTimeSlice", &fCBMTimeSlice, 64000, 99);
    }
    else
    {
        fCBMTimeSlice = new CbmTimeSlice(fCurrentStartTime,fDuration);
        fTree = new TTree("CbmMQOut", "Test output");
        fTree->Branch("TimeSlice.","CbmTimeSlice", &fCBMTimeSlice, 64000, 99);
    }
    
}

void CbmDataConverterTask::SetCbmTSInterval(Double_t start, Double_t duration)
{
    fCurrentStartTime=start;
    fDuration=duration;
    fCBMTimeSlice->Reset(fCurrentStartTime, fDuration);
    
}

void CbmDataConverterTask::FillCbmTSTree() 
{
  fTree->Fill();
  // --- Reset time slice with new time interval
  fCurrentStartTime += fDuration;
  fCBMTimeSlice->Reset(fCurrentStartTime, fDuration);

}


void CbmDataConverterTask::WriteTreeToFile()
{
    MQLOG(INFO)<<"Save Tree to file";
    fCBMTimeSlice->Reset(0.,0.);
    fTree->Write();
    fOutFile->Close();
}



void CbmDataConverterTask::CbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent)
{
    uint8_t DetectorId=MSdesc->sys_id;
    uint32_t ContentSize=MSdesc->size;
    if(ContentSize>0)
    {
        switch (DetectorId) 
        {

        case kSTS:
            fStsConverter->FillCbmRootTSlice(MSdesc, FlesTimeSliceContent,fCBMTimeSlice);
          break;
        case kMUCH:
            fMuchConverter->FillCbmRootTSlice(MSdesc, FlesTimeSliceContent,fCBMTimeSlice);
          break;
        default:
          break;
        }
    }
}


void CbmDataConverterTask::PrintCbmMicroSlice(const CbmMicroSlice* MSlice, uint32_t DigiToPrint)
{
    SetPrintOption(DigiToPrint);
    const fles::MicrosliceDescriptor desc=MSlice->GetHeader();
    uint8_t DetectorId=MSlice->GetHeader().sys_id;
    uint32_t ContentSize=MSlice->GetHeader().size;
    if(ContentSize>0)
    {
        switch (DetectorId) 
        {

        case kSTS:
            fStsConverter->GetDigiVector(&desc, MSlice->GetData().data());
          break;
        case kMUCH:
            fMuchConverter->GetDigiVector(&desc, MSlice->GetData().data());
          break;
        default:
          break;
        }
    }
}



CbmMicroSlice CbmDataConverterTask::BuildMicroSlice(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent)
{
    CbmMicroSlice MicroSlice;
    vector<uint8_t> MicroSliceContent;
    fles::MicrosliceDescriptor MS_header = *MSdesc;
    uint32_t ContentSize=MS_header.size;
    
    for(uint32_t k=0; k<ContentSize;k++)
    {
        MicroSliceContent.push_back(*(FlesTimeSliceContent+k));
    }
    
    MicroSlice.SetHeader(MS_header);
    MicroSlice.SetData(MicroSliceContent);
    
    return MicroSlice;
}

CbmMicroSlice CbmDataConverterTask::BuildMicroSlice(DetectorId iDet, CbmTimeSlice* CbmTSlice)
{
    CbmMicroSlice MicroSlice;

    if(fPrint)
    {
        
        MQLOG(INFO)<< "Time interval in ns [" 
                    << CbmTSlice->GetStartTime() << ", "
                    << CbmTSlice->GetEndTime() << "]";
    }
    
    // initialize fles MicroSlice header common to all detectors
    fles::MicrosliceDescriptor MSdesc = fles::MicrosliceDescriptor();
    MSdesc.hdr_id = 0xDD;// From Jan's example
    MSdesc.hdr_ver = 0x01;
    
    MSdesc.idx=(uint64_t)(CbmTSlice->GetStartTime()/1000.0);// assume 1mus interval

    switch (iDet) 
    {

    case kSTS:
    {
        if(fPrint)
        {
            MQLOG(INFO)   << "   number of STS points = "
                           << CbmTSlice->GetDataSize(kSTS);
        }

        // define fles MicroSlice header specific to STS detector
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kSTS);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kSTS);// detector id
        MSdesc.size = (uint32_t)(fStsDigiPayloadSize*CbmTSlice->GetDataSize(kSTS));
        MSdesc.offset=0;
        vector<CbmStsDigi> StsData=CbmTSlice->GetStsData();
        MicroSlice=fStsConverter->DigiVectToMSlice(&MSdesc, StsData);

      break;
    }
    case kMUCH:
    {
        bool fakeMuchDigi=true;
        

        // define fles MicroSlice header specific to MUCH detector
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kMUCH);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kMUCH);// detector id
        //MSdesc.size = (uint32_t)(fMuchDigiPayloadSize*CbmTSlice->GetDataSize(kMUCH));
        uint32_t NMuchPoints;
        if(fakeMuchDigi)
            NMuchPoints=2000;
        else
            NMuchPoints=(uint32_t)CbmTSlice->GetDataSize(kMUCH);
        
        
        if(fPrint)
        {
            MQLOG(INFO) << "   number of MUCH points = "
                        << CbmTSlice->GetDataSize(kMUCH);
            if(fakeMuchDigi)
                MQLOG(WARN)   << "   Warning: "<< NMuchPoints <<" dummy MUCH digi created for test";
        }
        
        MSdesc.size = (uint32_t)(fMuchDigiPayloadSize)*NMuchPoints;
        MSdesc.offset=0;
        // loop below temporary
        vector<CbmMuchDigi> MuchData;
        if(fakeMuchDigi)
        {
            for(uint32_t iMuchDigi=0;iMuchDigi<NMuchPoints;iMuchDigi++)
            {
                Int_t Muchaddress=(Int_t)(rand() % 100 + 1);
                Int_t Muchcharge=(Int_t)(rand() % 100 + 1);
                Int_t Muchtime=(Int_t)(CbmTSlice->GetStartTime()+(Double_t)(rand() % 100 + 1));
                CbmMuchDigi muchdigi(Muchaddress, Muchcharge, Muchtime);

                MuchData.push_back(muchdigi);
            }
        }
        else 
        {
            MuchData=CbmTSlice->GetMuchData();
        }
        MicroSlice=fMuchConverter->DigiVectToMSlice(&MSdesc, MuchData);
        break;
    }
    default:
      break;
    }
    

    return MicroSlice;
}


void CbmDataConverterTask::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
    fStsConverter->SetPrintOption(fDigiToPrint,fPrint);
    fMuchConverter->SetPrintOption(fDigiToPrint,fPrint);
}

