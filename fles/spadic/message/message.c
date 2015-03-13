/** \author Michael Krieger */

#include <stdlib.h>
#include "message.h"

#define MAX_RAW_COUNT 20 /* number of words needed to hold 32 samples */

struct message {
    uint8_t group_id;
    uint8_t channel_id;
    uint16_t timestamp;
    int16_t *samples;
    uint8_t num_samples;
    uint8_t hit_type;
    uint8_t stop_type;
    uint8_t buffer_overflow_count;
    uint16_t epoch_count;
    uint8_t info_type;

    uint8_t valid;

    uint16_t *raw_buf;
    uint8_t raw_count;
};

static void message_fill(Message *m, uint16_t w);
static void fill_wSOM(Message *m, uint16_t w);
static void fill_wTSW(Message *m, uint16_t w);
static void fill_wRDA(Message *m, uint16_t w);
static void fill_wCON(Message *m, uint16_t w);
static void fill_wEOM(Message *m, uint16_t w);
static void fill_wBOM(Message *m, uint16_t w);
static void fill_wEPM(Message *m, uint16_t w);
static void fill_wEXD(Message *m, uint16_t w);
static void fill_wINF(Message *m, uint16_t w);
static void fill_raw(Message *m);
static size_t raw_count(size_t num_samples);
static void unpack_raw(const uint16_t *raw,
                       int16_t *samples, size_t num_samples);

/* word types/preambles */
typedef struct wordtype {
    uint16_t value;
    uint16_t mask;
    uint8_t valid;
    void (*fill)(Message *m, uint16_t w);
} Wordtype;

static const Wordtype *word_get_type(uint16_t w);
static int word_is_type(uint16_t w, const Wordtype *t);
static int word_is_ignore(uint16_t w);
static int word_is_start(uint16_t w);
static int word_is_end(uint16_t w);
static uint8_t word_get_info_type(uint16_t w);

static const Wordtype wSOM = {0x8000, 0xF000, 1<<0, fill_wSOM};
static const Wordtype wTSW = {0x9000, 0xF000, 1<<1, fill_wTSW};
static const Wordtype wRDA = {0xA000, 0xF000, 1<<2, fill_wRDA};
static const Wordtype wCON = {0x0000, 0x8000,    0, fill_wCON};
static const Wordtype wEOM = {0xB000, 0xF000, 1<<3, fill_wEOM};
static const Wordtype wBOM = {0xC000, 0xF000, 1<<4, fill_wBOM};
static const Wordtype wEPM = {0xD000, 0xF000, 1<<5, fill_wEPM};
static const Wordtype wEXD = {0xE000, 0xF000,    0, fill_wEXD};
static const Wordtype wINF = {0xF000, 0xF000, 1<<6, fill_wINF};

/*==== create, fill, destroy message objects ======================*/

Message *message_new(void)
{
    Message *m;
    if (m = malloc(sizeof *m)) {
        message_init(m);
    }
    return m;
}

size_t message_size(void)
{
    return sizeof(Message);
}

void message_init(Message *m)
{
    m->samples = NULL;
    m->raw_buf = NULL;
    m->raw_count = 0;
    m->valid = 0;
}

void message_reset(Message *m)
{
    free(m->samples);
    free(m->raw_buf);
    message_init(m);
}

void message_delete(Message *m)
{
    free(m->samples);
    free(m->raw_buf);
    free(m);
}

size_t message_read_from_buffer(Message *m, const uint16_t *buf, size_t len)
{
    uint16_t w;
    size_t n = 0;
    while (n < len) {
        w = buf[n++];
        if (word_is_ignore(w)) { continue; }
        if (word_is_start(w)) { message_reset(m); }
        message_fill(m, w);
        if (word_is_end(w)) { break; }
    }
    return n;
}

void message_fill(Message *m, uint16_t w)
{
    const Wordtype *t = word_get_type(w);
    if (t) {
        (t->fill)(m, w);
        m->valid |= t->valid;
    }
}

/*-----------------------------------------------------------------*/

/*
 * These functions are called from message_fill() as the (*fill) member of
 * each Wordtype.
 */

void fill_wSOM(Message *m, uint16_t w)
{
    m->group_id = (w & 0x0FF0) >> 4;
    m->channel_id = (w & 0x000F);
}

void fill_wTSW(Message *m, uint16_t w)
{
    m->timestamp = (w & 0x0FFF);
}

void fill_wRDA(Message *m, uint16_t w)
{
    /* m->raw_buf should normally be NULL, but could already have been
       allocated if this is an additional RDA word somehow (falsely)
       inserted into the data stream */
    m->raw_buf = realloc(m->raw_buf, MAX_RAW_COUNT * sizeof *m->raw_buf);
    if (!m->raw_buf) { return; }
    m->raw_count = 0;
    m->raw_buf[m->raw_count++] = w & 0x0FFF;
}

void fill_wCON(Message *m, uint16_t w)
{
    if (!m->raw_buf) { return; }
    if (m->raw_count >= MAX_RAW_COUNT) { return; }
    m->raw_buf[m->raw_count++] = w & 0x7FFF;
}

void fill_wEOM(Message *m, uint16_t w)
{
    m->num_samples = (w & 0x0FC0) >> 6;
    m->hit_type = (w & 0x0030) >> 4;
    m->stop_type = (w & 0x0007);
}

void fill_wBOM(Message *m, uint16_t w)
{
    m->buffer_overflow_count = (w & 0x00FF);
}

void fill_wEPM(Message *m, uint16_t w)
{
    m->epoch_count = (w & 0x0FFF);
}

void fill_wEXD(Message *m, uint16_t w)
{ /* not implemented in SPADIC 1.0 */
}

void fill_wINF(Message *m, uint16_t w)
{
    uint8_t t = word_get_info_type(w);
    m->info_type = t;
    if (t == iDIS || t == iNGT || t == iNBE || t == iMSB) {
        m->channel_id = (w & 0x00F0) >> 4;
    } else if (t == iSYN) {
        m->epoch_count = (w & 0x00FF);
    }
}

/*-----------------------------------------------------------------*/

void fill_raw(Message *m)
{
    if (!m->raw_buf) { return; }
    /* if the number of samples is not available or is too high for the
       given number of raw data words, we discard the raw data */
    if (!(m->valid & wEOM.valid)) { goto del_raw; }
    if (m->raw_count < raw_count(m->num_samples)) { goto del_raw; }

    int16_t *s;
    if (!(s = malloc(m->num_samples * sizeof *s))) { return; }
    unpack_raw(m->raw_buf, s, m->num_samples);
    m->samples = s;

del_raw:
    free(m->raw_buf);
    m->raw_buf = NULL;
}

size_t raw_count(size_t num_samples)
{
    if (num_samples == 0) { return 0; }
    size_t bits = 9 * num_samples + 3;
    size_t n = bits/15;
    if (15*n < bits) { n++; }
    return n; /* ceil(bits/15) */
}

void unpack_raw(const uint16_t *raw, int16_t *samples, size_t num_samples)
{
    const uint16_t mask = 0x1FF;
    uint16_t v;
    uint32_t r = 0;
    size_t n = 0;

    int pos;
    for (pos = -12; ; pos += 15) {
        for (; pos >= 0; pos -= 9) {
            v = (r >> pos) & mask;
            samples[n++] = (v >= 0x100 ? v-0x200 : v); /* 2's complement */
            if (n >= num_samples) { return; }
            r &= ~(mask << pos);
        }
        r <<= 15;
        r += *raw++;
    }
}

/*-----------------------------------------------------------------*/

const Wordtype *word_get_type(uint16_t w)
{
    static const Wordtype *t[9] = {&wSOM, &wTSW, &wRDA, &wEOM, &wBOM,
                                   &wEPM, &wEXD, &wINF, &wCON};
    int i;
    for (i=0; i<9; i++) {
        if (word_is_type(w, t[i])) { return t[i]; }
    }
    return NULL;
}

int word_is_type(uint16_t w, const Wordtype *t)
{
    return (w & t->mask) == t->value;
}

int word_is_ignore(uint16_t w)
{
    if (!word_is_type(w, &wINF)) { return 0; }
    else { return word_get_info_type(w) == iNOP; }
}

int word_is_start(uint16_t w)
{
    if (word_is_type(w, &wSOM)) {
        return 1;
    } else if (!word_is_type(w, &wINF)) {
        return 0;
    } else {
        return (word_get_info_type(w) == iNGT ||
                word_get_info_type(w) == iNRT ||
                word_get_info_type(w) == iNBE);
    }
}

int word_is_end(uint16_t w)
{
    return (word_is_type(w, &wEOM) ||
            word_is_type(w, &wBOM) ||
            word_is_type(w, &wEPM) ||
            word_is_type(w, &wINF));
}

uint8_t word_get_info_type(uint16_t w)
{
    return (w & 0x0F00) >> 8;
}

/*==== message status query & data access =========================*/

int message_is_complete(const Message *m)
{
    return ((m->valid & wEOM.valid) ||
            (m->valid & wBOM.valid) ||
            (m->valid & wEPM.valid) ||
            (m->valid & wINF.valid));
}

int message_is_valid(const Message *m)
{
    return (message_is_hit(m) ||
            message_is_hit_aborted(m) ||
            message_is_buffer_overflow(m) ||
            message_is_epoch_marker(m) ||
            message_is_epoch_out_of_sync(m) ||
            message_is_info(m));
}

int message_is_hit(const Message *m)
{
    return m->valid == (wSOM.valid | wTSW.valid |
                        wRDA.valid | wEOM.valid);
}

int message_is_hit_aborted(const Message *m)
{
    return (m->valid & wINF.valid) &&
           (m->info_type == iDIS || m->info_type == iMSB);
}

int message_is_buffer_overflow(const Message *m)
{
    return m->valid == (wSOM.valid | wTSW.valid | wBOM.valid);
}

int message_is_epoch_marker(const Message *m)
{
    return m->valid == (wSOM.valid | wEPM.valid);
}

int message_is_epoch_out_of_sync(const Message *m)
{
    return (m->valid == (wSOM.valid | wINF.valid)) &&
           (m->info_type == iSYN);
}

int message_is_info(const Message *m)
{
    return (m->valid == wINF.valid) &&
           (m->info_type == iNGT ||
            m->info_type == iNRT ||
            m->info_type == iNBE);
}

/*-----------------------------------------------------------------*/

uint8_t message_get_group_id(const Message *m)
{
    return m->group_id;
}

uint8_t message_get_channel_id(const Message *m)
{
    return m->channel_id;
}

uint16_t message_get_timestamp(const Message *m)
{
    return m->timestamp;
}

int16_t *message_get_samples(const Message *m)
{
    if (!m->samples) { fill_raw((Message *)m); } /* strip const */
    return m->samples;
}

uint8_t message_get_num_samples(const Message *m)
{
    return m->num_samples;
}

uint8_t message_get_hit_type(const Message *m)
{
    return m->hit_type;
}

uint8_t message_get_stop_type(const Message *m)
{
    return m->stop_type;
}

uint8_t message_get_buffer_overflow_count(const Message *m)
{
    return m->buffer_overflow_count;
}

uint16_t message_get_epoch_count(const Message *m)
{
    return m->epoch_count;
}

uint8_t message_get_info_type(const Message *m)
{
    return m->info_type;
}

