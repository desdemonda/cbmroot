// Copyright 2012-2013 Jan de Cuveland <cmail@cuveland.de>
#pragma once

#include <stdexcept>
#include <cstdint>
#include <string>

/// Run parameter exception class.
class ParametersException : public std::runtime_error
{
public:
    explicit ParametersException(const std::string& what_arg = "")
        : std::runtime_error(what_arg)
    {
    }
};

/// Global run parameter class.
class Parameters
{
public:
  Parameters(int argc, char* argv[]) 
    : _shm_identifier("")
    , _input_archive("")
    , _output_archive("")
  { parse_options(argc, argv); }

    Parameters(const Parameters&) = delete;
    void operator=(const Parameters&) = delete;

    int32_t client_index() const { return _client_index; }

    std::string shm_identifier() const { return _shm_identifier; }

    std::string input_archive() const { return _input_archive; }

    std::string output_archive() const { return _output_archive; }

    bool analyze() const { return _analyze; }
    bool dump() const { return _dump; }
    bool showExceptions() const { return _showExceptions; }
    
    uint32_t skipTimeslices() const {return _skipTSs;}
    int32_t timesliceLimit() const {return _limitTS;}
    uint32_t MCSize() const {return _MCSize;}
    uint32_t overlapSize() const {return _overlapSize;}
private:
    void parse_options(int argc, char* argv[]);

    int32_t _client_index = -1;
    
    std::string _shm_identifier;
    std::string _input_archive;
    std::string _output_archive;
    bool _analyze = false;
    bool _dump = false;
    bool _showExceptions = false;
    
    uint32_t _skipTSs = 0;
    int32_t _limitTS = -1;

    uint32_t _MCSize = 1250;
    uint32_t _overlapSize = 2;
};
