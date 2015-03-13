/* 
 * File:   CbmMicroSliceMergerTask.h
 * Author: winckler
 *
 * Created on May 22, 2014, 9:00 PM
 */

#ifndef CBMMICROSLICEMERGERTASK_H
#define	CBMMICROSLICEMERGERTASK_H

#include <iostream>
#include <sstream>

#ifndef __CINT__ // Boost serialization 
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif //__CINT__

#include "CbmDetectorList.h"
#include "CbmDataConverterTask.h"
#include "CbmUnpacker.h"

#include "FairMQProcessorTask.h"
#include "FairMQLogger.h"

#include "StorableTimeslice.hpp"
#include "CbmMicroSlice.h"

template <typename TPayloadIn, typename TPayloadOut>
class CbmMicroSliceMergerTask : public FairMQProcessorTask
{
public:
    CbmMicroSliceMergerTask();
    virtual ~CbmMicroSliceMergerTask();
    
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt = "0");
    
    bool MsgReadyToSend();
    
    void SetMicroSliceNumber(uint64_t MaxMicroSliceNumber)
        {fMaxMicroSliceNumber=MaxMicroSliceNumber;}
    
    void SetComponentNumber(uint64_t MaxComponentNumber)
        {fMaxComponentNumber=MaxComponentNumber;}
    
    void SetTimeSliceIndex(uint64_t TimeSliceIndex)
        {fTimeSliceIndex=TimeSliceIndex;}
    
    
    uint64_t GetMaxMicroSliceNumber()
        {return fMaxMicroSliceNumber;}
    
    uint64_t GetMaxComponentNumber()
        {return fMaxComponentNumber;}
    
    uint64_t GetTimeSliceIndex()
        {return fTimeSliceIndex;}
    
    unsigned int GetMicroSliceIndex()
        {return fMicroSliceIndex;}
    
    unsigned int GetComponentIndex()
        {return fComponentIndex;}
    
    
    void InitFlesTimeSlice(uint64_t num_microslices, uint64_t index = UINT64_MAX);
    
    
    
    // boost serialize function
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fDigiVector;
        ar & fFlesTimeSlices;
    }
    
    
private:

    uint64_t fMaxMicroSliceNumber;      // max number of microslice per timeslices
    uint64_t fTimeSliceIndex;           // index of the timeslice
    uint64_t fMaxComponentNumber;
    unsigned int fMicroSliceIndex;
    unsigned int fComponentIndex;
    bool fTSReady;
    bool fMSIndexSync;
    CbmDataConverterTask* fDataConverterTask;
    uint64_t findex_sts;
    uint64_t findex_much;
    uint64_t fMSCounter_sts;
    uint64_t fMSCounter_much;
    std::vector<CbmMicroSlice> fDigiVector;
    #ifndef __CINT__ 
    friend class boost::serialization::access;
    fles::StorableTimeslice fFlesTimeSlices{1, 1};
    #endif
};


#include "CbmMicroSliceMergerTask.tpl"

#endif	/* CBMMICROSLICEMERGERTASK_H */

