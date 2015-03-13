//! \author Michael Krieger

#include "MicrosliceContents.hpp"

namespace flib_dpb {

//---- static implementation details ---------------------------------

static const size_t DESC_OFFSET {16 / sizeof(*DTM::data)};
static std::vector<DTM> _get_dtms(const uint16_t *data, size_t size);
static size_t _next_dtm(std::vector<DTM>& dtms, const uint16_t *data);
static std::vector<uint16_t> _pack_dtm(DTM d, uint8_t index);
static size_t _num_padding(size_t length, size_t mult=4);

// extract all DTMs from a raw buffer in "packed DTM" format
std::vector<DTM> _get_dtms(const uint16_t *data, size_t size)
{
    if (size < DESC_OFFSET) { return {}; }

    data += DESC_OFFSET;
    size -= DESC_OFFSET;
    auto end = data + size;

    auto dtms = std::vector<DTM> {};
    while (data < end) {
        data += _next_dtm(dtms, data);
    }
    return dtms;
}

// extract a single DTM from a raw buffer in "packed DTM" format
size_t _next_dtm(std::vector<DTM>& dtms, const uint16_t *data)
{
    auto n = size_t {2};
    auto size = size_t {*data++ & 0xFFu};
    if (size) {
        auto addr = uint16_t {*data++};
        dtms.push_back({addr, data, size}); // `size` must not lie!
        n += size;
    }
    n += _num_padding(n, 4); // skip padding (n -> k*4)
    return n;
}

// create "packed" form of the DTM including header and padding
std::vector<uint16_t> _pack_dtm(DTM d, uint8_t index)
{
    auto result = std::vector<uint16_t> {};
    result.push_back((index << 8) + d.size);
    result.push_back(d.addr);
    result.insert(end(result), d.data, d.data + d.size);
    result.insert(end(result), _num_padding(result.size(), 4), 0x0000);
    return result;
}

// calculate number of padding words
// length + _num_padding(length, mult) will be a multiple of mult
size_t _num_padding(size_t length, size_t mult)
{
    return (~length + 1) % mult;
}

//---- private member functions --------------------------------------

// copy all data into _raw and update DTM::data pointers, if not done before
/*
 * attention: vector (3, 5) -> [5, 5, 5]
 *            vector {3, 5} -> [3, 5]
 */
void MicrosliceContents::_store_raw() const
{
    if (_stored_raw) { return; }

    // save a copy of the DTMs we are already tracking
    auto dtms = std::vector<DTM> {std::move(_dtms)};
    // reset local data
    _dtms.clear();
    _raw = std::vector<uint16_t> (DESC_OFFSET, 0); // do not change to {}
    // rebuild local data
    for (auto d : dtms) {
        _add_dtm(d);
    }

    _stored_raw = true;
}

void MicrosliceContents::_add_dtm(DTM d) const
{
    auto index = static_cast<uint8_t>(_dtms.size() + _start_index);
    auto packed = _pack_dtm(d, index);
#if __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 9
    auto pos_inserted = _raw.insert(end(_raw), begin(packed), end(packed));
#else
    // bug in GCC before version 4.9: insert returns void instead of iterator
    // see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55817
    auto old_size = _raw.size();
    _raw.insert(end(_raw), begin(packed), end(packed));
    auto pos_inserted = begin(_raw) + old_size;
#endif
    auto pos_dtm_data = pos_inserted + 2; // after `nnll aaaa`
    d.data = &(*pos_dtm_data); // convert iterator to raw pointer
    _dtms.push_back(d);
}

//---- public interface ----------------------------------------------

MicrosliceContents::MicrosliceContents(uint8_t start_index)
: _start_index {start_index},
  _stored_raw {false},
  _raw {},
  _dtms {}
{
}

MicrosliceContents::MicrosliceContents(const uint16_t *data, size_t size)
: _start_index {static_cast<uint8_t>(size > DESC_OFFSET ?
                                     *(data + DESC_OFFSET) >> 8 : 0)},
  _stored_raw {false},
  _raw {},
  _dtms {_get_dtms(data, size)}
{
}

const std::vector<DTM>& MicrosliceContents::dtms() const {
    if (_stored_raw) {
        // TODO do this not every time
        _dtms = _get_dtms(_raw.data(), _raw.size());
    }
    return _dtms;
};

void MicrosliceContents::add_dtm(DTM d)
{
    _store_raw();
    _add_dtm(d);
}

const std::vector<uint16_t>& MicrosliceContents::raw() const
{
    _store_raw();
    return _raw;
}

} // namespace
