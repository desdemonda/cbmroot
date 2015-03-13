/** \author Michael Krieger */

#include <stdlib.h>
#include "message.h"
#include "message_reader.h"

/*==== message queue implementation ================================*/

struct msg_item;
struct msg_queue;
static void msg_queue_init(struct msg_queue *q);
static void msg_queue_clear(struct msg_queue *q);
static void msg_queue_append(struct msg_queue *q, struct msg_item *t);
static void msg_queue_extend(struct msg_queue *q, struct msg_queue *r);
static struct msg_item *msg_queue_pop(struct msg_queue *q);
static int msg_queue_is_empty(struct msg_queue *q);

/*------------------------------------------------------------------*/

struct msg_item {
    Message *msg;
    struct msg_item *next;
};

struct msg_queue {
    struct msg_item *begin;
    struct msg_item *end;
};

void msg_queue_init(struct msg_queue *q)
{
    q->begin = NULL;
    q->end = NULL;
}

void msg_queue_clear(struct msg_queue *q)
{
    struct msg_item *t;
    while (t = msg_queue_pop(q)) {
        message_delete(t->msg);
        free(t);
    }
}

void msg_queue_append(struct msg_queue *q, struct msg_item *t)
{
    t->next = NULL;
    struct msg_item *end = q->end;
    if (end) {
        end->next = t;
    } else {
        q->begin = t;
    }
    q->end = t;
}

void msg_queue_extend(struct msg_queue *q, struct msg_queue *r)
{
    struct msg_item *end = q->end;
    if (end) {
        end->next = r->begin;
        if (r->end) {
            q->end = r->end;
        }
    } else {
        q->begin = r->begin;
        q->end = r->end;
    }
}

struct msg_item *msg_queue_pop(struct msg_queue *q)
{
    struct msg_item *t = q->begin;
    if (t) {
        q->begin = t->next;
        if (!t->next) {
            q->end = NULL;
        }
    }
    return t;
}

int msg_queue_is_empty(struct msg_queue *q)
{
    return !q->begin;
}

/*==== message_reader implementation ===============================*/

struct message_reader {
    struct msg_queue messages;
    Message *msg;
};

static int reader_init(MessageReader *r);

MessageReader *message_reader_new(void)
{
    MessageReader *r;
    if (!(r = malloc(sizeof *r))) {
        return NULL;
    }
    if (!(reader_init(r) == 0)) {
        message_reader_delete(r);
        return NULL;
    }
    return r;
}

int reader_init(MessageReader *r)
{
    msg_queue_init(&r->messages);
    if (!(r->msg = message_new())) {
        return 1;
    }
    return 0;
}

void message_reader_delete(MessageReader *r)
{
    msg_queue_clear(&r->messages);
    if (r->msg) {
        message_delete(r->msg);
    }
    free(r);
}

void message_reader_reset(MessageReader *r)
{
    msg_queue_clear(&r->messages);
    if (r->msg) {
        message_reset(r->msg);
    }
}

int message_reader_add_buffer(MessageReader *r, const uint16_t *buf, size_t len)
{
    struct msg_queue q;
    msg_queue_init(&q);

    Message *m = r->msg;

    size_t pos = 0;
    while (1) {
        pos += message_read_from_buffer(m, buf+pos, len-pos);
        /* We avoid calling message_is_complete() if pos < len, because it
           is true by definition in this case. pos > len is impossible,
           because message_read_from_buffer() returns at most len-pos. If
           pos == len and message_is_complete(), we finish the current
           iteration. In the next iteration, message_reader_add_buffer()
           will be a no-op and because we created a new message at the end
           of the current iteration, we will then break. */
        if (!(pos < len) && !(message_is_complete(m))) { break; }

        struct msg_item *t;
        if (!(t = malloc(sizeof *t))) { goto abort; }
        t->msg = m;
        msg_queue_append(&q, t);

        if (!(m = message_new())) { goto abort; }
    }

    r->msg = m;

    msg_queue_extend(&r->messages, &q);
    return 0;

abort:
    msg_queue_clear(&q);
    return 1;
}

Message *message_reader_get_message(MessageReader *r)
{
    struct msg_item *t = msg_queue_pop(&r->messages);
    if (!t) { return NULL; }
    Message *m = t->msg;
    free(t);
    return m;
}
