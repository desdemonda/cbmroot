/* 
 * File:   CbmMicroSliceMergerTask.tpl
 * Author: winckler
 *
 * Created on May 22, 2014, 9:08 PM
 */


template <typename TPayloadIn, typename TPayloadOut>
CbmMicroSliceMergerTask<TPayloadIn, TPayloadOut>::CbmMicroSliceMergerTask() :
        fMaxMicroSliceNumber(0),
        fTimeSliceIndex(0),
        fMaxComponentNumber(0),
        fMicroSliceIndex(0),
        fComponentIndex(0),
        fTSReady(false),
        fMSIndexSync(false),
        fDataConverterTask(new CbmDataConverterTask()),
        findex_sts(0),
        findex_much(0),
        fMSCounter_sts(0),
        fMSCounter_much(0),
        fDigiVector()
{
    
}




template <typename TPayloadIn, typename TPayloadOut> 
CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::~CbmMicroSliceMergerTask()
{
    if(fDataConverterTask)
    {
        delete fDataConverterTask;
        fDataConverterTask=NULL;
    }
}




template <typename TPayloadIn, typename TPayloadOut> 
InitStatus CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::Init()
{
        
        // temporary : component are input link and not detector id, but for the moment 
        // we use it here as component = detector id
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=0 -> empty
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=1 -> empty
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=2 -> sts
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=3 -> empty
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=4 -> empty
        
        
        
        // detector list in CbmDetectorList.h
        //kREF,0      // Reference plane  
        //kMVD,1      // Micro-Vertex Detector
        //kSTS,2      // Silicon Tracking System
        //kRICH,3     // Ring-Imaging Cherenkov Detector
        //kMUCH,4     // Muon detetcion system
        //kTRD,5      // Transition Radiation Detector
        //kTOF,      // Time-of-flight Detector
        //kECAL,     // EM-Calorimeter
        //kPSD,      // Projectile spectator detector
        //kSTT,      // Straw Tube Tracker (obsolete)
        //kFHODO,    // Fibre Hodoskope (for beam tests)
        //kTutDet,   // Dummy for tutorials
        //kNOFDETS // Number of elements (e.g. for loops)
        
        return kSUCCESS;
}


template <typename TPayloadIn, typename TPayloadOut> 
void InitFlesTimeSlice(uint64_t num_microslices, uint64_t index = UINT64_MAX)
{
    
}

template <typename TPayloadIn, typename TPayloadOut> 
void CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::Exec(Option_t* opt)
{ 
        
        fTSReady=false;
        //prepare boost input archive
        std::string msgStr( static_cast<char*>(fPayload->GetData()), fPayload->GetSize() );
        std::istringstream ibuffer(msgStr);
        TPayloadIn InputArchive(ibuffer);
        try
        {
             InputArchive >> fDigiVector;// get input Archive
        }
        catch (boost::archive::archive_exception e)
        {
             MQLOG(ERROR) << e.what();
        }

        
        MQLOG(INFO) << "---------------------------------------";
        
        fles::MicrosliceDescriptor desc;
        std::vector<uint8_t> MSliceData;
        
        uint64_t RelativeIndex=0;
        uint16_t eqid=0;// component (input link)
        uint16_t sysid=100;
        uint32_t ContentSize=0;
        // Merge Microslices into timeslices
        for (unsigned int i = 0; i < fDigiVector.size(); ++i) 
        {            
            CbmMicroSlice MSlice=fDigiVector[i];
            desc=MSlice.GetHeader();
            sysid=desc.sys_id;
            if(sysid==kSTS)
            {
                findex_sts=desc.idx;
                RelativeIndex=fMSCounter_sts;
                fMSCounter_sts++;
            }
            if(sysid==kMUCH)
            {
                findex_much=desc.idx;
                RelativeIndex=fMSCounter_much;
                fMSCounter_much++;
            }

            
            eqid=desc.eq_id;
            ContentSize=desc.size;
            MSliceData=MSlice.GetData();
            
            fFlesTimeSlices.append_microslice(eqid, RelativeIndex, desc, MSliceData.data());
            
            //MQLOG(INFO) << "MSliceData.size() = "<<MSliceData.size();
            //std::cout<<std::endl;
        
            /// temporary : to check whether we get data 
            bool printdata=true;
            if(printdata)
            {
                
                switch (sysid) 
                {

                    case kSTS:
                    {
                        MQLOG(INFO) << "Detector ID  = STS";
                        MQLOG(INFO) << "Micro Slice Index = "<<findex_sts;
                        MQLOG(INFO) << "Input link = "<<eqid;
                        MQLOG(INFO) << "Content size = "<<ContentSize;
                      break;
                    }
                    case kMUCH:
                    {
                        MQLOG(INFO) << "Detector ID  = MUCH";
                        MQLOG(INFO) << "Micro Slice Index_much = "<<findex_much;
                        MQLOG(INFO) << "Input link = "<<eqid;
                        MQLOG(INFO) << "Content size = "<<ContentSize;
                      break;
                    }
                    default:
                      break;
                }
                //MQLOG(INFO) << "Micro Slice Index = "<<index<<std::endl;
                
                std::cout<<std::endl;
                fDataConverterTask->PrintCbmMicroSlice(&MSlice,0);

            }

        }
        
        
            

        if(findex_sts == findex_much)
            fMSIndexSync=true;
        else
            fMSIndexSync=false;
        
        if(fMSCounter_sts==fMaxMicroSliceNumber  && fMSCounter_much==fMaxMicroSliceNumber)
        {
            fTSReady=true;
            //std::cout<< "++++++++++ READY +++++++++++++++" <<std::endl;
        }
        else 
            fTSReady=false;
        
        
        if(fTSReady)
        {
            MQLOG(INFO) <<"Send time slice to sink";
            //prepare boost output archive
            std::ostringstream obuffer;
            TPayloadOut OutputArchive(obuffer); 
            OutputArchive << fFlesTimeSlices;
            int outputSize=obuffer.str().length();
            fPayload->Rebuild(outputSize);
            std::memcpy(fPayload->GetData(), obuffer.str().c_str(), outputSize);
        }
    
        
}





template <typename TPayloadIn, typename TPayloadOut> 
bool CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::MsgReadyToSend()
{
    return fTSReady;
}



















