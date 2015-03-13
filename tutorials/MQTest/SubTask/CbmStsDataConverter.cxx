/* 
 * File:   CbmStsDataConverter.cxx
 * Author: winckler
 * 
 * Created on July 30, 2014, 11:44 AM
 */

#include "CbmStsDataConverter.h"

CbmStsDataConverter::CbmStsDataConverter()
: CbmDigiConverter(),
  fStsDigiPayloadSize(sizeof(ULong64_t)+sizeof(UInt_t)+sizeof(UShort_t)+sizeof(Int_t)+sizeof(Int_t))
{
}

CbmStsDataConverter::~CbmStsDataConverter() 
{
}


void CbmStsDataConverter::FillCbmRootTSlice(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t offset=0;
    uint32_t iDigi=0;
    while(offset<ContentSize)
    {
        //CbmStsDigi StsDigi=*(static_cast<CbmStsDigi*>(ConvertFlesPtrToDigi(&offset,FlesTimeSliceContent)));
        std::shared_ptr<CbmDigi> spDigi=ConvertFlesPtrToDigi(&offset,FlesTimeSliceContent);
        std::shared_ptr<CbmStsDigi> spStsDigi=dynamic_pointer_cast<CbmStsDigi>(spDigi);
        
        CbmRootTimeSlice->InsertData(spStsDigi.get());
        
        if(fPrint && iDigi==fDigiToPrint)
            PrintInfo(MSdesc, spStsDigi.get(),  iDigi);

        iDigi++;
    }
}



vector<CbmStsDigi>  CbmStsDataConverter::GetDigiVector(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t offset=0;
    uint32_t iDigi=0;
    vector<CbmStsDigi> StsData;
    while(offset<ContentSize)
    {
        std::shared_ptr<CbmDigi> spDigi=ConvertFlesPtrToDigi(&offset,FlesTimeSliceContent);
        std::shared_ptr<CbmStsDigi> spStsDigi=dynamic_pointer_cast<CbmStsDigi>(spDigi);
        
        CbmStsDigi StsDigi=*(spStsDigi.get());
        StsData.push_back(StsDigi);

        if(fPrint && iDigi==fDigiToPrint)
            PrintInfo(MSdesc, spStsDigi.get(),  iDigi);
        
        iDigi++;
    }
    return StsData;
}



CbmMicroSlice CbmStsDataConverter::DigiVectToMSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmStsDigi> vStsDigi)
{

    CbmMicroSlice MicroSlice;
    vector<uint8_t> MicroSliceContent;
    //fles::MicrosliceDescriptor desc_sts = *MSdesc;

    // loop over sts data
    for(unsigned int iDigi = 0; iDigi < vStsDigi.size(); iDigi++) 
    {
        
        // split individual data member of CbmStsDigi into pieces of N bytes and store them into vector
        vector<uint8_t> StsData_TimeStamp=SplitData<ULong64_t>(vStsDigi[iDigi].GetTime());
        vector<uint8_t> StsData_Address=SplitData<UInt_t>(vStsDigi[iDigi].GetAddress());
        vector<uint8_t> StsData_Charge=SplitData<UShort_t>(vStsDigi[iDigi].GetCharge());
        vector<uint8_t> StsData_SectorNr=SplitData<Int_t>(vStsDigi[iDigi].GetSectorNr());
        vector<uint8_t> StsData_SystemId= SplitData<Int_t>(vStsDigi[iDigi].GetSystemId());
        
        // print 
        if(fPrint && iDigi==fDigiToPrint)
            PrintInfo(MSdesc, &(vStsDigi[iDigi]),  iDigi);
        // fill Micro slice content 
        // Note:  "start time" and "duration" data members 
        //are implicitely given by the microslice index idx -> interval[idx-1,idx] mus
       
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_TimeStamp.begin(), StsData_TimeStamp.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_Address.begin(), StsData_Address.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_Charge.begin(), StsData_Charge.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_SectorNr.begin(), StsData_SectorNr.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_SystemId.begin(), StsData_SystemId.end());
    }
    
    if(MicroSliceContent.size()!=MSdesc->size)
    {
        MQLOG(ERROR)<<"MicroSlice content does not match with expected CbmRoot Time slice content";
        MSdesc->size = MicroSliceContent.size();
    }

    MicroSlice.SetHeader(*MSdesc);
    MicroSlice.SetData(MicroSliceContent);

    return MicroSlice;
}
    
std::shared_ptr<CbmDigi> CbmStsDataConverter::ConvertFlesPtrToDigi(uint32_t *offset, const uint8_t* FlesTimeSliceContent)
{
    uint32_t start=0;
    std::vector<uint8_t> vTimestamp_sts;     // ULong64_t
    std::vector<uint8_t> vAdress_sts;        // UInt_t
    std::vector<uint8_t> vCharge_sts;        // UShort_t
    std::vector<uint8_t> vSectorNr_sts;      // Int_t
    std::vector<uint8_t> vSystemId_sts;      // Int_t

    // time stamp
    start=*offset;
    *offset+=sizeof(ULong64_t);
    for (uint32_t k = start; k < *offset; ++k)
        vTimestamp_sts.push_back(*(FlesTimeSliceContent+k));

    // adress
    start=*offset;
    *offset+=sizeof(UInt_t);
    for (uint32_t k = start; k < *offset; ++k)
        vAdress_sts.push_back(*(FlesTimeSliceContent+k));

    // charge
    start=*offset;
    *offset+=sizeof(UShort_t);
    for (uint32_t k = start; k < *offset; ++k)
        vCharge_sts.push_back(*(FlesTimeSliceContent+k));

    // sectorNr
    start=*offset;
    *offset+=sizeof(Int_t);
    for (uint32_t k = start; k < *offset; ++k)
        vSectorNr_sts.push_back(*(FlesTimeSliceContent+k));

    // systemId
    start=*offset;
    *offset+=sizeof(Int_t);
    for (uint32_t k = start; k < *offset; ++k)
        vSystemId_sts.push_back(*(FlesTimeSliceContent+k));
    
    
    ULong64_t Digi_Timestamp=CombineData<ULong64_t>(vTimestamp_sts);
    UInt_t Digi_Adress=CombineData<UInt_t>(vAdress_sts);
    UShort_t Digi_Charge=CombineData<UShort_t>(vCharge_sts);
    Int_t Digi_SectorNr=CombineData<Int_t>(vSectorNr_sts);
    Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_sts);
    
    std::shared_ptr<CbmDigi> spDigi(std::make_shared<CbmStsDigi>(Digi_Adress, Digi_Timestamp, Digi_Charge, Digi_SectorNr));
    return spDigi;
}


void CbmStsDataConverter::PrintDigiContent(const CbmDigi* digi)
{
    const CbmStsDigi* StsDigi=static_cast<const CbmStsDigi*>(digi);
    ULong64_t Digi_Timestamp=(ULong64_t)StsDigi->GetTime();
    UInt_t Digi_Adress=(UInt_t)StsDigi->GetAddress();
    UShort_t Digi_Charge=(UShort_t)StsDigi->GetCharge();
    Int_t Digi_SectorNr=(Int_t)StsDigi->GetSectorNr();
    Int_t Digi_SystemId=(Int_t)StsDigi->GetSystemId();
    MQLOG(INFO) << "* SystemId = "  << Digi_SystemId;
    MQLOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
    MQLOG(INFO) << "* Address = "   << Digi_Adress;
    MQLOG(INFO) << "* Charge = "    << Digi_Charge;
    MQLOG(INFO) << "* SectorNr = "  << Digi_SectorNr;
}
