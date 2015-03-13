/* 
 * File:   CbmMuchDataConverter.cxx
 * Author: winckler
 * 
 * Created on July 30, 2014, 11:44 AM
 */

#include "CbmMuchDataConverter.h"

CbmMuchDataConverter::CbmMuchDataConverter()
: CbmDigiConverter(),
  fMuchDigiPayloadSize(sizeof(Int_t)+sizeof(Int_t)+sizeof(Int_t)+sizeof(Int_t))
{
}


CbmMuchDataConverter::~CbmMuchDataConverter() 
{
    
}


void CbmMuchDataConverter::FillCbmRootTSlice(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t offset=0;
    uint32_t iDigi=0;
    while(offset<ContentSize)
    {
        std::shared_ptr<CbmDigi> spDigi=ConvertFlesPtrToDigi(&offset,FlesTimeSliceContent);
        std::shared_ptr<CbmMuchDigi> spMuchDigi=dynamic_pointer_cast<CbmMuchDigi>(spDigi);
        
        CbmRootTimeSlice->InsertData(spMuchDigi.get());
        
        if(fPrint && iDigi==fDigiToPrint)
            PrintInfo(MSdesc, spMuchDigi.get(),  iDigi);
        
        iDigi++;
    }
}



vector<CbmMuchDigi> CbmMuchDataConverter::GetDigiVector(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t offset=0;
    uint32_t iDigi=0;
    vector<CbmMuchDigi> MuchData;
    while(offset<ContentSize)
    {
        std::shared_ptr<CbmDigi> spDigi=ConvertFlesPtrToDigi(&offset,FlesTimeSliceContent);
        std::shared_ptr<CbmMuchDigi> spMuchDigi=dynamic_pointer_cast<CbmMuchDigi>(spDigi);
        
        CbmMuchDigi MuchDigi(spMuchDigi.get());
        
        MuchData.push_back(MuchDigi);
        
        if(fPrint && iDigi==fDigiToPrint)
            PrintInfo(MSdesc, spMuchDigi.get(),  iDigi);
        iDigi++;
        
    }
    return MuchData;
}



CbmMicroSlice CbmMuchDataConverter::DigiVectToMSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmMuchDigi> vMuchDigi)
{

    CbmMicroSlice MicroSlice;
    vector<uint8_t> MicroSliceContent;

    // loop over Much data
    for(unsigned int iDigi = 0; iDigi < vMuchDigi.size(); iDigi++) 
    {
        // split individual data member of CbmMuchDigi into pieces of N bytes and store them into vector
        vector<uint8_t> MuchData_TimeStamp=SplitData<Int_t>(vMuchDigi[iDigi].GetTime());
        vector<uint8_t> MuchData_Address=SplitData<Int_t>(vMuchDigi[iDigi].GetAddress());
        vector<uint8_t> MuchData_Charge=SplitData<Int_t>(vMuchDigi[iDigi].GetAdc());
        vector<uint8_t> MuchData_SystemId= SplitData<Int_t>(vMuchDigi[iDigi].GetSystemId());
        
        // print 
        if(fPrint && iDigi==fDigiToPrint)
            PrintInfo(MSdesc, &(vMuchDigi[iDigi]),  iDigi);
        
        // fill Micro slice content 
        // Note:  "start time" and "duration" data members 
        //are implicitely given by the microslice index idx -> interval[idx-1,idx] mus
       
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_TimeStamp.begin(), MuchData_TimeStamp.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_Address.begin(), MuchData_Address.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_Charge.begin(), MuchData_Charge.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_SystemId.begin(), MuchData_SystemId.end());
    }
    
    if(MicroSliceContent.size()!=MSdesc->size)
    {
        MQLOG(ERROR)<<"MicroSlice content "<< MicroSliceContent.size() 
        << " does not match with expected CbmRoot Time slice content : "<<MSdesc->size;
        MSdesc->size = MicroSliceContent.size();
    }

    MicroSlice.SetHeader(*MSdesc);
    MicroSlice.SetData(MicroSliceContent);

    return MicroSlice;
}




std::shared_ptr<CbmDigi> CbmMuchDataConverter::ConvertFlesPtrToDigi(uint32_t *offset, const uint8_t* FlesTimeSliceContent)
{    
    uint32_t start=0;
    std::vector<uint8_t> vTimestamp_much;     // ULong64_t
    std::vector<uint8_t> vAdress_much;        // UInt_t
    std::vector<uint8_t> vCharge_much;        // UShort_t
    std::vector<uint8_t> vSystemId_much;      // Int_t

    /// get data of first digi of current microslice

    // time stamp
    start=*offset;
    *offset+=sizeof(Int_t);
    for (uint32_t k = start; k < *offset; ++k)
        vTimestamp_much.push_back(*(FlesTimeSliceContent+k));

    // adress
    start=*offset;
    *offset+=sizeof(Int_t);
    for (uint32_t k = start; k < *offset; ++k)
        vAdress_much.push_back(*(FlesTimeSliceContent+k));

    // charge
    start=*offset;
    *offset+=sizeof(Int_t);
    for (uint32_t k = start; k < *offset; ++k)
        vCharge_much.push_back(*(FlesTimeSliceContent+k));

    // systemId
    start=*offset;
    *offset+=sizeof(Int_t);
    for (uint32_t k = start; k < *offset; ++k)
        vSystemId_much.push_back(*(FlesTimeSliceContent+k));

    /// reconvert and fill digi in time slices
    Int_t Digi_Timestamp=CombineData<Int_t>(vTimestamp_much);
    Int_t Digi_Adress=CombineData<Int_t>(vAdress_much);
    Int_t Digi_Charge=CombineData<Int_t>(vCharge_much);
    Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_much);
    
    std::shared_ptr<CbmDigi> spDigi(std::make_shared<CbmMuchDigi>(Digi_Adress, Digi_Charge,  Digi_Timestamp));
    return spDigi;
}


void CbmMuchDataConverter::PrintDigiContent(const CbmDigi* digi)
{
    const CbmMuchDigi* MuchDigi=static_cast<const CbmMuchDigi*>(digi);
    Int_t Digi_Timestamp=(Int_t)MuchDigi->GetTime();
    Int_t Digi_Adress=(Int_t)MuchDigi->GetAddress();
    Int_t Digi_Charge=(Int_t)MuchDigi->GetAdc();
    Int_t Digi_SystemId=(Int_t)MuchDigi->GetSystemId();
    MQLOG(INFO) << "* System Id = "  << Digi_SystemId;
    MQLOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
    MQLOG(INFO) << "* Address = "   << Digi_Adress;
    MQLOG(INFO) << "* Charge = "    << Digi_Charge;
}



