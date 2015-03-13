/* 
 * File:   CbmMicroSliceMerger.tpl
 * Author: winckler
 *
 * Created on July 15, 2014, 12:33 PM
 */



template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::CbmMicroSliceMerger() : fTSIndex(0), 
        fMaxMicroSliceNumber(0),
        fProcessorTask(new TPolicyTask())
{
}

template <typename TPolicyTask>
CbmMicroSliceMerger<TPolicyTask>::~CbmMicroSliceMerger()
{
    if(fProcessorTask)
    {
        delete fProcessorTask;
        fProcessorTask=NULL;
    }
}

/*
Warning in <TClass::TClass>: no dictionary for class CbmTrd is available
Warning in <TClass::TClass>: no dictionary for class CbmTof is available
Warning in <TClass::TClass>: no dictionary for class CbmPsdv1 is available
Warning in <TClass::TClass>: no dictionary for class CbmPsd is available
Warning in <TClass::TClass>: no dictionary for class CbmUrqmdGenerator is available
Warning in <TClass::TClass>: no dictionary for class CbmFieldPar is availa
 */

template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::Init()
{
    FairMQDevice::Init();
    fProcessorTask->SetTimeSliceIndex(fTSIndex);
    fProcessorTask->SetMicroSliceNumber(fMaxMicroSliceNumber);
    fProcessorTask->InitTask();
}

template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::ReInitMergerTask()
{
    
    if(fProcessorTask)
    {
        delete fProcessorTask;
        fProcessorTask=NULL;
        fProcessorTask =new TPolicyTask();
    }
    else
        fProcessorTask =new TPolicyTask();
    
    fProcessorTask->SetTimeSliceIndex(fTSIndex);
    fProcessorTask->SetMicroSliceNumber(fMaxMicroSliceNumber);
    fProcessorTask->InitTask();
}


template <typename TPolicyTask>
void CbmMicroSliceMerger<TPolicyTask>::Run()
{
    MQLOG(INFO) << ">>>>>>> Run <<<<<<<";

    boost::thread rateLogger(boost::bind(&FairMQDevice::LogSocketRates, this));

    //FairMQPoller* poller = fTransportFactory->CreatePoller(*fPayloadInputs);


    int receivedMsgs = 0;
    int sentMsgs = 0;
    bool ReadyToSend=false;

    int bytes_received = 0;

    //temporary
    int NumTSinSampler=16;
    int NumTStoSend=NumTSinSampler/fMaxMicroSliceNumber;
    
    while (fState == RUNNING ) 
    {    

        FairMQMessage* msg = fTransportFactory->CreateMessage();
        //poller->Poll(100);

        for (int i = 0; i < fNumInputs; i++)
        {
            //if (poller->CheckInput(i))
                bytes_received = fPayloadInputs->at(i)->Receive(msg);

            if (bytes_received>0)
            {
                receivedMsgs++;
                cout << "I've received " << receivedMsgs << " messages!" << endl;
                fProcessorTask->SetPayload(msg);
                fProcessorTask->Exec();

                ReadyToSend=fProcessorTask->MsgReadyToSend();

                if(ReadyToSend)
                {  
                    fPayloadOutputs->at(0)->Send(msg);  
                    sentMsgs++;
                    fTSIndex++;
                    ReInitMergerTask();
                }
                bytes_received = 0;
            }
        }
                    
        delete msg; 
        
        // temporary to break the merger loop when finished
        //if(ReadyToSend && sentMsgs==NumTStoSend) 
          //  break;
    }

    //delete poller;

    cout << "I've received " << receivedMsgs << " Microslices and sent " << sentMsgs << " TimeSlice(s)!" << endl;

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

