// Copyright 2014 Jan de Cuveland <cmail@cuveland.de>
#pragma once

#include "MicrosliceDescriptor.hpp"
#include <tuple>
#include <vector>

class MicrosliceFilter
{
public:
    typedef std::tuple<fles::MicrosliceDescriptor, std::vector<uint8_t>> MicrosliceTuple;

    virtual MicrosliceTuple filter(const fles::MicrosliceDescriptor& descriptor,
                                   const uint8_t* content) = 0;

    virtual ~MicrosliceFilter() { };
};
