//! \author Michael Krieger

/**
 * Read SPADIC Messages out of Timeslices containing Microslices using the
 * "packed DTM" format.
 */

#ifndef SPADIC_TIMESLICE_READER
#define SPADIC_TIMESLICE_READER

#include "Message.hpp"
#include "Timeslice.hpp"
#include "MicrosliceContents.hpp"
#include <unordered_set>

namespace spadic {

struct TimesliceReader
{
    TimesliceReader();
    ~TimesliceReader();

    void add_timeslice(const fles::Timeslice& ts);
    void add_component(const fles::Timeslice& ts, size_t component);
    std::unordered_set<uint16_t> sources() const;
    std::unique_ptr<spadic::Message> get_message(uint16_t source_addr) const;

private:
    struct TimesliceReader_;
    std::unique_ptr<TimesliceReader_> _t;
};

} // namespace

#endif
