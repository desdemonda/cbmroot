//! \author Michael Krieger
//! C++ API for the SPADIC Message library.

#ifndef SPADIC_MESSAGE_WRAP_H
#define SPADIC_MESSAGE_WRAP_H

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace spadic {

enum class stop_t : uint8_t {
#define STOP(K, V) K = V,
#include "constants/stop_types.inc"
#undef STOP
};
enum class info_t : uint8_t {
#define INFO(K, V) K = V,
#include "constants/info_types.inc"
#undef INFO
};
enum class hit_t : uint8_t {
#define HIT(K, V) K = V,
#include "constants/hit_types.inc"
#undef HIT
};

struct MessageError : std::runtime_error {
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 7)
    // C++11: inheriting constructor supported by GCC 4.8+
    using std::runtime_error::runtime_error;
#else
    MessageError(const char *text) : std::runtime_error::runtime_error(text) {}
#endif
};

struct Message {
#define PV(F) virtual F const = 0;
    PV (bool is_valid())
    PV (bool is_hit())
    PV (bool is_hit_aborted())
    PV (bool is_buffer_overflow())
    PV (bool is_epoch_marker())
    PV (bool is_epoch_out_of_sync())
    PV (bool is_info())

    PV (uint8_t group_id())
    PV (uint8_t channel_id())
    PV (uint16_t timestamp())
    PV (const std::vector<int16_t>& samples())
    PV (hit_t hit_type())
    PV (stop_t stop_type())
    PV (uint8_t buffer_overflow_count())
    PV (uint16_t epoch_count())
    PV (info_t info_type())
#undef PV

    virtual ~Message() {};
};

struct MessageReader {
    MessageReader();
    MessageReader(MessageReader&& rhs);
    ~MessageReader();

    void reset();
    void add_buffer(const uint16_t *buf, size_t len);

    std::unique_ptr<Message> get_message() const;

private:
    struct MessageReader_;
    std::unique_ptr<MessageReader_> r;
};

} // namespace

#endif
