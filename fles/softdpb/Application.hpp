// Copyright 2012-2013 Jan de Cuveland <cmail@cuveland.de>
#pragma once

#include "Parameters.hpp"
#include "MicrosliceFilter.hpp"
#include "TimesliceSource.hpp"
#include "TimesliceOutputArchive.hpp"
#include <memory>
#include <vector>

/// %Application base class.
class Application
{
public:
    explicit Application(Parameters const& par);

    Application(const Application&) = delete;
    void operator=(const Application&) = delete;

    ~Application();

    void run();

private:
    Parameters const& _par;

    std::unique_ptr<fles::TimesliceSource> _source;
    std::vector<std::unique_ptr<MicrosliceFilter>> _filters;
    std::unique_ptr<fles::TimesliceOutputArchive> _output;

    uint64_t _count = 0;
};
