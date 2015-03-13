/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/* 
 * File:   CbmMQFileSink.tpl
 * Author: winckler
 */

template <typename TPayloadIn>
CbmMQFileSink<TPayloadIn>::CbmMQFileSink() : fDataConverterTask(new CbmDataConverterTask()), 
        fCurrentStartTime (0.),
        fDuration (1000.)
{
        fDataConverterTask->InitCbmTS(fCurrentStartTime,fDuration);
    
}


template <typename TPayloadIn>
CbmMQFileSink<TPayloadIn>::~CbmMQFileSink()
{
    fDataConverterTask->WriteTreeToFile();
    if(fDataConverterTask)
    {
        delete fDataConverterTask;
        fDataConverterTask=NULL;
    }
}

template <typename TPayloadIn>
void CbmMQFileSink<TPayloadIn>::InitOutputFile(TString defaultId)
{
    char outname[256];
    sprintf(outname, "cbmTimeSlicefilesink%s.root", defaultId.Data());
    fDataConverterTask->InitCbmTSOutputFile(outname);
}



template <typename TPayloadIn>
void CbmMQFileSink<TPayloadIn>::Run()
{
        MQLOG(INFO) << ">>>>>>> Run <<<<<<<";
        bool printinfo=true;
        fDataConverterTask->SetPrintOption(0,printinfo);

        boost::thread rateLogger(boost::bind(&FairMQDevice::LogSocketRates, this));
        int receivedMsgs = 0;
        int bytes_received = 0;

        while ( fState == RUNNING ) 
        {
            FairMQMessage* msg = fTransportFactory->CreateMessage();
            bytes_received = fPayloadInputs->at(0)->Receive(msg);
            
            if (bytes_received > 0) 
            {
                receivedMsgs++;
                std::string msgStr( static_cast<char*>(msg->GetData()), msg->GetSize() );
                std::istringstream ibuffer(msgStr);
                TPayloadIn InputArchive(ibuffer);

                try
                {
                    InputArchive >> fFlesTimeSlices;
                }
                catch (boost::archive::archive_exception& e)
                {
                    MQLOG(ERROR) << e.what();
                }

                MQLOG(INFO) << "Message received";
                
                
                /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // In order to fill the root cbm TimeSlice we first get the max number of mircoslice among all components
                // (= total duration of the current fles TimeSlice) 
                // and we also get the smallest MS index (=start time of the current fles Timeslice)
                
                uint64_t InputLinkNumber=fFlesTimeSlices.num_components();
                uint64_t MSlicesNumber_MAX=0;
                uint64_t MSliceIndex_MIN=UINTMAX_MAX;
                
                for(unsigned int comp_j = 0; comp_j < InputLinkNumber; ++comp_j)
                {
                    // get max number of fles microslices in the current fles timeslice
                    uint64_t MSnum_MAX=fFlesTimeSlices.num_microslices(comp_j);
                    if(MSnum_MAX>MSlicesNumber_MAX) 
                            MSlicesNumber_MAX=MSnum_MAX;
                    
                    // get smallest microslice index in current time slice 
                    // if components are non empty
                    
                    uint64_t MSIndex_MIN=UINTMAX_MAX;
                    if(fFlesTimeSlices.descriptor(comp_j,0).size>0)
                        MSIndex_MIN=fFlesTimeSlices.descriptor(comp_j,0).idx;
                    
                    
                    if(MSIndex_MIN<MSliceIndex_MIN)
                        MSliceIndex_MIN=MSIndex_MIN;
                    
                    
                }
                
                
                /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /// initialize time interval of the cbmroot timeslice obtained from the fles header 
                /// assume duration of 1 mus
                fCurrentStartTime=1000.0*(Double_t)MSliceIndex_MIN;
                fDataConverterTask->SetCbmTSInterval(fCurrentStartTime, fDuration);
                
                /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /// loop over all fles MS index (time) of the current fles timeslice
                /// and fill cbmroot timeslice in tree
                for(uint64_t MS_i = 0; MS_i < MSlicesNumber_MAX; ++MS_i)
                {
                    
                    if(printinfo)
                    {
                        MQLOG(INFO) << "-------------------------------------------";
                        MQLOG(INFO) <<MS_i<< "th MS in current fles TS ";
                        MQLOG(INFO) << "Current Time Slice interval = ["<<fCurrentStartTime
                                <<";"<< fCurrentStartTime+fDuration<<"]";
                        MQLOG(INFO) << "InputLinkNumber = "<<InputLinkNumber;
                    }
                    
                    fCurrentStartTime+=fDuration;// not used but in the initialization or in print
                    
                    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    /// loop over all component in fles time slice in the current time interval
                    for(uint64_t comp_j = 0; comp_j < InputLinkNumber; ++comp_j)
                    {
                        uint64_t MSlicesNumber_j=fFlesTimeSlices.num_microslices(comp_j);
                        
                        if(MS_i<MSlicesNumber_j)
                        {
                            const fles::MicrosliceDescriptor MSdesc=fFlesTimeSlices.descriptor(comp_j,MS_i);
                            
                            const uint8_t* ptr_FlesTimeSliceContent;
                            ptr_FlesTimeSliceContent=fFlesTimeSlices.content(comp_j,MS_i);
                            fDataConverterTask->CbmTSFiller(&MSdesc,ptr_FlesTimeSliceContent);
                            
                        }

                    } // end loop on MS component
                    
                    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    /// fill tree and update currentStartTime
                    fDataConverterTask->FillCbmTSTree();
                    
                } // end loop on MS index
                bytes_received = 0;
            } //end of if (bytes_received) 
            delete msg;
            
               
        } // end while loop on state machine
        
        //fDataConverterTask->WriteTreeToFile();
        cout << "I've received " << receivedMsgs << " messages!" << endl;
        boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
        
        try 
        {
            rateLogger.interrupt();
            rateLogger.join();
        } 
        catch(boost::thread_resource_error& e) 
        {
            MQLOG(ERROR) << e.what();
        }
        
        FairMQDevice::Shutdown();
    
}




