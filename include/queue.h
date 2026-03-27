/**
 * queue.h — FIFO queue backed by a circular buffer for C
 * Part of cstructs: https://github.com/qorexdev/cstructs
 */

#ifndef CSTRUCTS_QUEUE_H
#define CSTRUCTS_QUEUE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void   *data;
    size_t  head;   /* next dequeue position */
    size_t  tail;   /* next enqueue position */
    size_t  len;
    size_t  cap;
    size_t  elem_size;
} Queue;

static inline int queue_init(Queue *q, size_t elem_size) {
    q->elem_size = elem_size;
    q->cap       = 8;
    q->len = q->head = q->tail = 0;
    q->data = malloc(elem_size * q->cap);
    return q->data ? 0 : -1;
}

static inline void queue_free(Queue *q) {
    free(q->data);
    q->data = NULL;
    q->len = q->cap = q->head = q->tail = 0;
}

static inline int _queue_grow(Queue *q) {
    size_t new_cap = q->cap * 2;
    void  *buf     = malloc(q->elem_size * new_cap);
    if (!buf) return -1;
    /* linearise: copy head..end, then start..tail */
    size_t first = q->cap - q->head;
    if (first > q->len) first = q->len;
    memcpy(buf, (char *)q->data + q->head * q->elem_size,
           first * q->elem_size);
    if (q->len > first)
        memcpy((char *)buf + first * q->elem_size,
               q->data, (q->len - first) * q->elem_size);
    free(q->data);
    q->data = buf;
    q->head = 0;
    q->tail = q->len;
    q->cap  = new_cap;
    return 0;
}

/* Enqueue copy of `elem`. Returns 0 on success. */
static inline int queue_push(Queue *q, const void *elem) {
    if (q->len == q->cap && _queue_grow(q) != 0) return -1;
    memcpy((char *)q->data + q->tail * q->elem_size, elem, q->elem_size);
    q->tail = (q->tail + 1) % q->cap;
    q->len++;
    return 0;
}

/* Dequeue into `out` (may be NULL). Returns 0 on success. */
static inline int queue_pop(Queue *q, void *out) {
    if (q->len == 0) return -1;
    if (out) memcpy(out, (char *)q->data + q->head * q->elem_size, q->elem_size);
    q->head = (q->head + 1) % q->cap;
    q->len--;
    return 0;
}

/* Peek at front without removing. Returns pointer or NULL if empty. */
static inline void *queue_front(const Queue *q) {
    if (q->len == 0) return NULL;
    return (char *)q->data + q->head * q->elem_size;
}

#endif /* CSTRUCTS_QUEUE_H */
