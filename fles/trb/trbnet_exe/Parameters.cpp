// Copyright 2012-2013 Jan de Cuveland <cmail@cuveland.de>

#include "Parameters.hpp"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

void Parameters::parse_options(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()("version,V", "print version string")(
        "help,h", "produce help message")(
        "client-index,c", po::value<int32_t>(&_client_index),
        "index of this executable in the list of processor tasks")(
        "analyze-pattern,a", po::value<bool>(&_analyze)->implicit_value(true),
        "enable/disable pattern check")(
        "dump-timslices,d", po::value<bool>(&_dump)->implicit_value(true),
        "enable/disable timeslice dump")(
        "show-exceptions,e", po::value<bool>(&_showExceptions)->implicit_value(true),
        "enable/disable exception print out")(
        "skip-timeslices,f", po::value<uint32_t>(&_skipTSs),
        "number of first timeslice to be imported. default: 0")(
        "number-timeslices,n", po::value<int32_t>(&_limitTS),
        "number of timeslices to be imported. -1 for unlimited. default: -1")(
        "mc-size,m", po::value<uint32_t>(&_MCSize),
        "applies only to offline unpacking as a time base. mc-size as set in flib.conf when archive was captured. default: 1250")(
        "overlap-size,l", po::value<uint32_t>(&_overlapSize),
        "number of mircoslices per timeslice overlapping")(
        "shm-identifier,s", po::value<std::string>(&_shm_identifier),
        "shared memory identifier used for receiving timeslices")(
        "input-archive,i", po::value<std::string>(&_input_archive),
        "name of an input file archive to read")(
        "output-archive,o", po::value<std::string>(&_output_archive),
        "name of an output file archive to write");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (vm.count("version")) {
        std::cout << "tsclient, version 0.0" << std::endl;
        exit(EXIT_SUCCESS);
    }

    int input_sources = vm.count("shm-identifier") + vm.count("input-archive");
    if (input_sources == 0)
        throw ParametersException("no input source specified");
    if (input_sources > 1)
        throw ParametersException("more than one input source specified");
    if (_limitTS == 0) 
        throw ParametersException("number-timeslices, n must not be zero. use negative value to disable limit and strictly positive value to limit number of timeslices");
}
