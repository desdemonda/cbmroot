
#ifndef CBMTIMESLICELOADER_H
#define	CBMTIMESLICELOADER_H

#if __cplusplus >= 201103L
#include <type_traits>
#include <array>
#endif

#include <iostream> 
#include <boost/timer/timer.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "FairMQSamplerTask.h"
#include "CbmTimeSlice.h"
#include "CbmMicroSlice.h"
#include "CbmDataConverterTask.h"
#include "FairMQLogger.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////


////////// Base template header <TCbmDigi,TPayloadOut>
template <typename TCbmDigi, typename TPayloadOut>
class CbmTimeSliceLoader : public FairMQSamplerTask
{ 
public : 
 
    CbmTimeSliceLoader();
    virtual ~CbmTimeSliceLoader();
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fDigiVector;
    }
    
    
private :
    CbmDataConverterTask* fDataConverterTask;
    CbmTimeSlice* fCBMTimeSlice;
    friend class boost::serialization::access;
    vector<CbmMicroSlice> fDigiVector;
    CbmMicroSlice fMicroSlice;
    DetectorId fDetID;

    CbmTimeSliceLoader(const CbmTimeSliceLoader&);
    CbmTimeSliceLoader operator=(const CbmTimeSliceLoader&);
}; 





////////// Template implementation is in CbmTimeSliceLoader.tpl :
#include "CbmTimeSliceLoader.tpl"

#endif /* CBMTIMESLICELOADER_H */

