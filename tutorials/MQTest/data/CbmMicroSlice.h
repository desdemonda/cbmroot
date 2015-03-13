/* 
 * File:   CbmMicroSlice.h
 * Author: winckler
 *
 * Created on May 22, 2014, 1:40 PM
 */

#ifndef CBMMICROSLICE_H
#define	CBMMICROSLICE_H

#include <vector>
#include"MicrosliceDescriptor.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>


class CbmMicroSlice 
{
public:
    CbmMicroSlice();
    virtual ~CbmMicroSlice();
    
    void SetHeader(fles::MicrosliceDescriptor header) {fMicroSliceHeader=header;}
    void SetData(std::vector<uint8_t> Data) {fMicroSliceData=Data;}
    fles::MicrosliceDescriptor GetHeader() const {return fMicroSliceHeader;}
    std::vector<uint8_t> GetData() const {return fMicroSliceData;}
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fMicroSliceHeader;
        ar & fMicroSliceData;
    }
    
private:

    friend class boost::serialization::access;
    
    fles::MicrosliceDescriptor fMicroSliceHeader;
    std::vector<uint8_t> fMicroSliceData;
};

#endif	/* CBMMICROSLICE_H */

