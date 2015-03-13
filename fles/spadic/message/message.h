/**
 * \file
 * \author Michael Krieger
 *
 * This module provides the low-level functionality for decoding the
 * SPADIC 1.0 message format and accessing the message contents.
 *
 * All functions that receive a pointer to a ::Message object assume that
 * it has been properly allocated and initialized (e.g. by message_new()).
 * One exception is message_init(), for which the message object need (and
 * must) not be initialized.
 */

#ifndef SPADIC_MESSAGE_H
#define SPADIC_MESSAGE_H

#include <stddef.h>
#include <stdint.h>

/**
 * Stop types as returned by message_get_stop_type().
 */
enum stop_types {
#define STOP(K, V) s##K = V,
#include "constants/stop_types.inc"
#undef STOP
};

/**
 * Info types as returned by message_get_info_type().
 */
enum info_types {
#define INFO(K, V) i##K = V,
#include "constants/info_types.inc"
#undef INFO
};

/**
 * Hit types as returned by message_get_hit_type().
 */
enum hit_types {
#define HIT(K, V) h##K = V,
#include "constants/hit_types.inc"
#undef HIT
};

typedef struct message Message;
/**<
 * Data structure representing SPADIC 1.0 messages.
 *
 * Pointers to such objects can be obtained by using message_new() or
 * message_reader_get_message() (preferred).
 *
 * They must be deallocated by using message_delete().
 */

/**@{
 * \name Create, destroy and fill message objects
 */
Message *message_new(void);
/**<
 * Allocate and initialize a new message object.
 * \return Pointer to created message object, `NULL` if unsuccessful.
 */
size_t message_size(void);
/**<
 * \return The allocation size for one message object.
 * \note You only need this if you want to allocate memory for message
 * objects yourself. If you use message_new(), you don't need this.
 */
void message_init(Message *m);
/**<
 * Initialize a newly allocated message object.
 *
 * This function must be used exactly once for each message object.
 *
 * \note You only need this if you want to allocate memory for message
 * objects yourself. If you use message_new(), you don't need this.
 */
void message_reset(Message *m);
/**<
 * Reset a message object to its initial state.
 *
 * This function can be used any number of times with a single message
 * object (in contrast to message_init()).
 *
 * Typical use case is to recycle a single message object across multiple
 * calls to message_read_from_buffer() as an alternative to allocating a
 * new message object each time (if the older messages are not needed any
 * longer).
 */
void message_delete(Message *m);
/**<
 * Clean up and deallocate a message object.
 *
 * \note You must also use this if you have allocated the message object
 * yourself.
 */
size_t message_read_from_buffer(Message *m, const uint16_t *buf, size_t len);
/**<
 * Read up to `len` words from `buf` and fill message `m`.
 *
 * `buf` must be non-`NULL`.
 *
 * \return The number `n` of consumed words (at most `len`).
 *
 * Words from the buffer are consumed until either
 * - an end-of-message word is encountered (`n` &le; `len`), or
 * - the end of the buffer is reached (`n` = `len`).
 *
 * If `n` = `len`, the two cases can be distinguished by using
 * message_is_complete().
 *
 * If `len` is zero, nothing is done.
 *
 * The contents of the consumed words are copied into the message object
 * pointed to by `m`. If (and only if) a start-of-message word is
 * encountered, the message object will be reset. This means
 * - all words before the last start-of-message word are effectively
 *   ignored, and
 * - a partially filled message can be reused and possibly completed by
 *   reading from another buffer containing the remaining words.
 */
int message_is_complete(const Message *m);
/**<
 * \return Non-zero if `m` is a complete message.
 *
 * A message is considered "complete" if an end-of-message word has been
 * encountered.
 *
 * Use this function to determine whether a given message object can be
 * further filled by repeated calls of message_read_from_buffer() when
 * `n` = `len` has been returned. If message_read_from_buffer() returns
 * `n` < `len`, the message is complete by definition.
 *
 * Note that this is different from message_is_valid(): a message can be
 * complete and not valid, but a valid message is always complete.
 */
/**@}*/

/**@{
 * \name Query message status and type
 */
int message_is_valid(const Message *m);
/**<
 * \return Non-zero if `m` is a valid message of any type.
 *
 * Valid messages are of one of the following types:
 * - hit message, normal or aborted
 *   (message_is_hit(), message_is_hit_aborted())
 * - buffer overflow message (message_is_buffer_overflow())
 * - epoch marker, normal or "out of sync"
 *   (message_is_epoch_marker(), message_is_epoch_out_of_sync())
 * - info message (message_is_info())
 *
 * A "valid" message is always "complete" (message_is_complete()).  If a
 * message is complete, but not valid, there are either words missing that
 * are required for a particular message type or there are additional
 * words making the message type ambiguous.
 */
int message_is_hit(const Message *m);
/**<
 * \return Non-zero if `m` is a regular hit message.
 *
 * Indicates that the following data is available:
 * - group ID (message_get_group_id())
 * - channel ID (message_get_channel_id())
 * - timestamp (message_get_timestamp())
 * - number of samples (message_get_num_samples())
 * - hit type (message_get_hit_type())
 * - stop type (message_get_stop_type())
 *
 * Does not guarantee that the actual samples (message_get_samples()) are
 * available (although they normally should be), this function only checks
 * the metadata listed above.
 */
int message_is_hit_aborted(const Message *m);
/**<
 * \return Non-zero if `m` is an aborted hit message.
 *
 * Indicates that the following data is available:
 * - channel ID (message_get_channel_id())
 * - info type (message_get_info_type()), can be #iDIS or #iMSB
 */
int message_is_buffer_overflow(const Message *m);
/**<
 * \return Non-zero if `m` is a buffer overflow message.
 *
 * Indicates that the following data is available:
 * - group ID (message_get_group_id())
 * - channel ID (message_get_channel_id())
 * - timestamp (message_get_timestamp())
 * - number of lost hits (message_get_buffer_overflow_count())
 */
int message_is_epoch_marker(const Message *m);
/**<
 * \return Non-zero if `m` is an epoch marker.
 *
 * Indicates that the following data is available:
 * - group ID (message_get_group_id())
 * - epoch count (message_get_epoch_count())
 */
int message_is_epoch_out_of_sync(const Message *m);
/**<
 * \return Non-zero if `m` is an "out of sync" epoch marker.
 *
 * Indicates that the following data is available:
 * - group ID (message_get_group_id())
 * - least significant 8 bits of epoch count (message_get_epoch_count())
 * - info type (can only be #iSYN, so no need to check it)
 */
int message_is_info(const Message *m);
/**<
 * \return Non-zero if `m` is an info message
 *
 * Indicates that the following data is available:
 * - info type (message_get_info_type()), can be #iNGT, #iNRT, or #iNBE
 * - channel ID (message_get_channel_id()), if the info type is #iNGT or #iNBE
 */
/**@}*/

/**@{
 * \name Access message data
 */
uint8_t message_get_group_id(const Message *m);
/**< \return The group ID, if available, undefined otherwise. */
uint8_t message_get_channel_id(const Message *m);
/**< \return The channel ID, if available, undefined otherwise. */
uint16_t message_get_timestamp(const Message *m);
/**< \return The timestamp, if available, undefined otherwise. */
int16_t *message_get_samples(const Message *m);
/**<
 * \return Pointer to a memory location containing the samples, if
 * available, `NULL` otherwise.
 *
 * The number of available samples must be determined using
 * message_get_num_samples(). The memory containing the samples will be
 * released when `m` is destroyed by message_delete().
 */
uint8_t message_get_num_samples(const Message *m);
/**< \return The number of samples, if available, undefined otherwise. */
uint8_t message_get_hit_type(const Message *m);
/**< \return One of the #hit_types, if available, undefined otherwise. */
uint8_t message_get_stop_type(const Message *m);
/**< \return One of the #stop_types, if available, undefined otherwise. */
uint8_t message_get_buffer_overflow_count(const Message *m);
/**< \return The buffer overflow count, if available, undefined otherwise. */
uint16_t message_get_epoch_count(const Message *m);
/**< \return The epoch count, if available, undefined otherwise. */
uint8_t message_get_info_type(const Message *m);
/**< \return One of the #info_types, if available, undefined otherwise. */
/**@}*/

#endif
