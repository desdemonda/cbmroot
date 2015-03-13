
/* 
 * File:   CbmMicroSliceMerger.h
 * Author: winckler
 *
 * Created on May 22, 2014, 5:06 PM
 */

#ifndef CBMMICROSLICEMERGER_H
#define	CBMMICROSLICEMERGER_H
#include "FairMQDevice.h"
#include "StorableTimeslice.hpp"
#include "CbmMicroSlice.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "FairMQLogger.h"

template <typename TPolicyTask>
class CbmMicroSliceMerger: public FairMQDevice 
{
public:
    
    //using CbmProcessor<TPolicyTask>::fProcessorTask;
    
    
    CbmMicroSliceMerger(); 
    virtual ~CbmMicroSliceMerger();
    void SetTask(TPolicyTask* task);
    
    
    
    void SetMicroSliceNum(uint64_t MSNumber)
    {
        fMaxMicroSliceNumber=MSNumber;
    }
    
    void SetTimeSliceIdx(uint64_t TSIndex)
    {
        fTSIndex=TSIndex;
    }
    

protected:
    
    virtual void Init();
    virtual void Run();
    void ReInitMergerTask();

    uint64_t fTSIndex;
    uint64_t fMaxMicroSliceNumber;
    //bool fMSIndexSync;
    TPolicyTask* fProcessorTask; 
    
    //fles::StorableTimeslice fFlesTimeSlices{1, 1};

private:
    CbmMicroSliceMerger(const CbmMicroSliceMerger&);       
    CbmMicroSliceMerger operator=(const CbmMicroSliceMerger&);       
};

#include "CbmMicroSliceMerger.tpl"

#endif	/* CBMMICROSLICEMERGER_H */

