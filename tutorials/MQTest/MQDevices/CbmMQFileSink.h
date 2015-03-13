/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/* 
 * File:   CbmMQFileSink.h
 * Author: winckler
 */

#ifndef FAIRMQFILESINK_H_
#define FAIRMQFILESINK_H_

#include <iostream>

#include "Rtypes.h"
#include "TString.h"
#include <chrono>


#include "FairMQDevice.h"
#include "StorableTimeslice.hpp"
#include "CbmDataConverterTask.h"

#ifndef __CINT__
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#endif //__CINT__

#include "FairMQLogger.h"


template <typename TPayloadIn>
class CbmMQFileSink: public FairMQDevice
{
  public:
    CbmMQFileSink();
    virtual ~CbmMQFileSink();
    virtual void InitOutputFile(TString defaultId = "100");
    
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fFlesTimeSlices;
    }
    
  protected:
    virtual void Run();
    
    CbmDataConverterTask* fDataConverterTask;
  private:
    
    // Start time of current time slice [ns]
    Double_t fCurrentStartTime;
    
    // Duration of time slice [ns] 
    Double_t fDuration;
    
    #ifndef __CINT__ // for BOOST serialization
    friend class boost::serialization::access;
    fles::StorableTimeslice fFlesTimeSlices{1,1};
    #endif // for BOOST serialization

    CbmMQFileSink(const CbmMQFileSink&);
    CbmMQFileSink operator=(const CbmMQFileSink&);
};


////////// Template implementation of Run() in CbmMQFileSink.tpl :
#include "CbmMQFileSink.tpl"

#endif /* FAIRMQFILESINK_H_ */
