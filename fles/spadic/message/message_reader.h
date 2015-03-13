/**
 * \file
 * \author Michael Krieger
 *
 * This module provides another level of abstraction for reading SPADIC
 * messages from input buffers. It saves the user from using the
 * message_read_from_buffer()/::message_is_complete() functions
 * directly and having to manually keeping track of the reading position
 * and state.
 *
 * All functions receiving a pointer to a ::MessageReader object assume
 * that it has been properly allocated and initialized (by
 * message_reader_new()).
 */

#ifndef SPADIC_MESSAGE_READER_H
#define SPADIC_MESSAGE_READER_H

#include <stddef.h>
#include <stdint.h>

typedef struct message Message;

typedef struct message_reader MessageReader;
/**<
 * Context for reading SPADIC messages from buffers.
 *
 * Saves the state of partially read messages across buffer boundaries.
 */

MessageReader *message_reader_new(void);
/**<
 * Allocate and initialize a new message reader.
 * \return Pointer to created message reader, `NULL` if unsuccessful.
 */
void message_reader_delete(MessageReader *r);
/**<
 * Clean up and deallocate a message reader.
 *
 * All messages that have not yet been retrieved using
 * message_reader_get_message() will be lost.
 */
void message_reader_reset(MessageReader *r);
/**<
 * Reset a message reader to its initial state.
 *
 * All messages that have not yet been retrieved using
 * message_reader_get_message() will be lost.
 */
int message_reader_add_buffer(MessageReader *r, const uint16_t *buf, size_t len);
/**<
 * Read all messages from a buffer with `len` words.
 * \return Zero if successful, non-zero otherwise.
 *
 * The values of `buf` and `len` are not checked in this function.  If
 * `buf` is `NULL`, the behaviour of the reader is undefined. For the
 * reader to do something *useful*, `len` should be positive.
 *
 * When this function has successfully returned, all words from the buffer
 * have been consumed and it is no longer needed by the MessageReader. All
 * complete messages that were contained in the buffer can be retrieved
 * using message_reader_get_message(). Incomplete messages (contained
 * partially at the end of the buffer) are saved and can be completed by
 * adding another buffer.
 *
 * In case of failure, the state of the message reader is unmodified, so
 * that another attempt can be made without receiving duplicate messages.
 */
Message *message_reader_get_message(MessageReader *r);
/**<
 * Retrieve the next message.
 *
 * \return Pointer to a message object, if available, `NULL` otherwise.
 *
 * The returned messages are always complete (message_is_complete()).
 */

#endif
