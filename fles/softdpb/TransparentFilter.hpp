// Copyright 2014 Jan de Cuveland <cmail@cuveland.de>
#pragma once

#include "MicrosliceFilter.hpp"

class TransparentFilter : public MicrosliceFilter
{
public:
    virtual MicrosliceTuple filter(const fles::MicrosliceDescriptor& descriptor,
                                   const uint8_t* content);
};
