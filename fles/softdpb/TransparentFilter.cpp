// Copyright 2014 Jan de Cuveland <cmail@cuveland.de>

#include "TransparentFilter.hpp"

MicrosliceFilter::MicrosliceTuple
TransparentFilter::filter(const fles::MicrosliceDescriptor& descriptor,
                          const uint8_t* content)
{
    fles::MicrosliceDescriptor new_descriptor = descriptor;
    std::vector<uint8_t> new_content{content, content + descriptor.size};
    MicrosliceTuple new_microslice{new_descriptor, new_content};
    
    return new_microslice;
}
