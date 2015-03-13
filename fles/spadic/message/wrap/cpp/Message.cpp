//! \author Michael Krieger
#include "Message.hpp"

extern "C" {
#include "message.h"
#include "message_reader.h"
}

namespace spadic {

//---- Message implementation ---------------------------------------

struct Message_ : Message {

#define IS(NAME) bool is_##NAME() const { return message_is_##NAME(m); }
    IS (valid)
    IS (hit)
    IS (hit_aborted)
    IS (buffer_overflow)
    IS (epoch_marker)
    IS (epoch_out_of_sync)
    IS (info)
#undef IS

#define GET(NAME) NAME() const { return message_get_##NAME(m); }
    uint8_t GET (group_id)
    uint8_t GET (channel_id)
    uint16_t GET (timestamp)
    uint8_t GET (buffer_overflow_count)
    uint16_t GET (epoch_count)
#undef GET

#define GET_TYPE(NAME) NAME##_t NAME##_type() const\
        { return static_cast<NAME##_t>(message_get_##NAME##_type(m)); };
    GET_TYPE (hit)
    GET_TYPE (stop)
    GET_TYPE (info)
#undef GET_TYPE

    const std::vector<int16_t>& samples() const { return _samples; }

    Message_(struct message *m);
    ~Message_();

private:
    struct message *m;
    std::vector<int16_t> _samples;
    void init_samples();
};

Message_::Message_(struct message *m) : m(m)
{
    if (is_hit()) {
        init_samples();
    }
}

Message_::~Message_()
{
    message_delete(m);
}

void Message_::init_samples()
{
    const int16_t *s;
    size_t n;
    if ((s = message_get_samples(m)) &&
        (n = message_get_num_samples(m))) {
        _samples.assign(s, s+n);
    }
}

//---- MessageReader implementation (nested pimpl...) ----------------

struct MessageReader::MessageReader_ {
    struct message_reader *r;
    MessageReader_() : r {message_reader_new()}
        { if (!r) throw MessageError("Could not create MessageReader."); };
    ~MessageReader_() { message_reader_delete(r); };
};

MessageReader::MessageReader() : r {new MessageReader_} {}
MessageReader::MessageReader(MessageReader&& rhs) : r {std::move(rhs.r)} {}
MessageReader::~MessageReader() {}

void MessageReader::reset()
{
    message_reader_reset(r->r);
}

void MessageReader::add_buffer(const uint16_t *buf, size_t len)
{
    if (!(message_reader_add_buffer(r->r, buf, len) == 0)) {
        throw MessageError("Could not add buffer.");
    }
}

std::unique_ptr<Message> MessageReader::get_message() const
{
    Message_ *M;
    struct message *m;
    if ((m = message_reader_get_message(r->r))) {
        M = new Message_(m);
    } else {
        M = nullptr;
    }
    return std::unique_ptr<Message> {M};
}

} // namespace
