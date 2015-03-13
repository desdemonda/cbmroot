/* 
 * File:   CbmUnpacker.h
 * Author: winckler
 *
 * Created on July 15, 2014, 4:11 PM
 */

#ifndef CBMUNPACKER_H
#define	CBMUNPACKER_H

#include <assert.h>
#include <vector>

    // split data member of type U1 in N pieces of 1 byte
    template<typename U1>
    std::vector<uint8_t> SplitData(U1 DataToSplit)
    {
        U1 Input=DataToSplit;
        std::size_t InputTypeSize=sizeof(U1);
        uint8_t *Data_ptr = (uint8_t*)&Input;
        
        std::vector<uint8_t> output;
        for(unsigned int i=0;i<InputTypeSize;i++)
        {
            output.push_back(Data_ptr[i]);
        }
    
        return output;
    }
    
    // reconstruct data
    template<typename U2>
    U2 CombineData(std::vector<uint8_t> input)
    {
        
        assert( sizeof(U2)==input.size());
        
        uint64_t combinedData=0;
        for(unsigned int i=0;i<input.size();i++)
        {
            uint64_t temp=0;
            temp=(uint64_t)input[i];
            if(i>0)
                temp<<=8*i;
            combinedData|=temp;
            
            
        }
        
        U2 output=(U2)combinedData;
        return output;
    }

#endif	/* CBMUNPACKER_H */

