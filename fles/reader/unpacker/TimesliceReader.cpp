//! \author Michael Krieger

#include "TimesliceReader.hpp"
#include <unordered_map>

namespace spadic {

struct TimesliceReader::TimesliceReader_ {
    std::unordered_map<uint16_t, spadic::MessageReader> _readers;
    //< one MessageReader per CBMnet source address

    TimesliceReader_() : _readers{} {}

    void add_mc(const flib_dpb::MicrosliceContents& mc)
    {
        for (auto& dtm : mc.dtms()) {
            // TODO distinguish same source address from different components
            auto& reader = _readers[dtm.addr];
            reader.add_buffer(dtm.data, dtm.size);
        }
    }
};

TimesliceReader::TimesliceReader() : _t {new TimesliceReader_} {}
TimesliceReader::~TimesliceReader() {}

void TimesliceReader::add_timeslice(const fles::Timeslice& ts)
{
    for (size_t c {0}; c < ts.num_components(); c++) {
        add_component(ts, c);
    }
}

void TimesliceReader::add_component(const fles::Timeslice& ts, size_t component)
{
    for (size_t m {0}; m < ts.num_microslices(component); m++) {
        auto& desc = ts.descriptor(component, m);
        auto *content = ts.content(component, m);

        // interpret raw bytes as 16-bit unsigned integers, assuming the
        // native byte order is correct
        auto p = reinterpret_cast<const uint16_t *>(content);
        auto s = desc.size * sizeof(*content) / sizeof(*p);
        _t->add_mc({p, s});
    }
}

std::unordered_set<uint16_t> TimesliceReader::sources() const
{
    std::unordered_set<uint16_t> result;
    for (auto& item : _t->_readers) {
        auto addr = item.first;
        result.insert(addr);
    }
    return result;
}

std::unique_ptr<spadic::Message>
TimesliceReader::get_message(uint16_t source_addr) const
{
    return _t->_readers[source_addr].get_message();
}

} // namespace
