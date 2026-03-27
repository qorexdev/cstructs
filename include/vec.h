/**
 * vec.h — Dynamic array (vector) for C
 * Part of cstructs: https://github.com/qorexdev/cstructs
 *
 * Usage:
 *   #define CSTRUCTS_IMPL
 *   #include "vec.h"
 */

#ifndef CSTRUCTS_VEC_H
#define CSTRUCTS_VEC_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define VEC_INIT_CAP 8

typedef struct {
    void   *data;
    size_t  len;
    size_t  cap;
    size_t  elem_size;
} Vec;

/* Initialize a vector for elements of `elem_size` bytes. */
static inline int vec_init(Vec *v, size_t elem_size) {
    v->elem_size = elem_size;
    v->len       = 0;
    v->cap       = VEC_INIT_CAP;
    v->data      = malloc(elem_size * VEC_INIT_CAP);
    return v->data ? 0 : -1;
}

/* Free internal storage. Call when done. */
static inline void vec_free(Vec *v) {
    free(v->data);
    v->data = NULL;
    v->len = v->cap = 0;
}

/* Return pointer to element at index i (no bounds check). */
static inline void *vec_at(const Vec *v, size_t i) {
    return (char *)v->data + i * v->elem_size;
}

/* Push a copy of `elem` to the back. Returns 0 on success, -1 on OOM. */
static inline int vec_push(Vec *v, const void *elem) {
    if (v->len == v->cap) {
        size_t new_cap = v->cap * 2;
        void  *buf     = realloc(v->data, new_cap * v->elem_size);
        if (!buf) return -1;
        v->data = buf;
        v->cap  = new_cap;
    }
    memcpy(vec_at(v, v->len++), elem, v->elem_size);
    return 0;
}

/* Pop the last element into `out` (may be NULL). Returns 0 on success. */
static inline int vec_pop(Vec *v, void *out) {
    if (v->len == 0) return -1;
    if (out) memcpy(out, vec_at(v, v->len - 1), v->elem_size);
    v->len--;
    return 0;
}

/* Remove element at index i (order-preserving). */
static inline int vec_remove(Vec *v, size_t i) {
    if (i >= v->len) return -1;
    size_t tail = (v->len - 1 - i) * v->elem_size;
    if (tail > 0)
        memmove(vec_at(v, i), vec_at(v, i + 1), tail);
    v->len--;
    return 0;
}

/* Shrink allocation to exactly fit current length. */
static inline int vec_shrink(Vec *v) {
    if (v->len == 0) return 0;
    void *buf = realloc(v->data, v->len * v->elem_size);
    if (!buf) return -1;
    v->data = buf;
    v->cap  = v->len;
    return 0;
}

/* Iterate: for (size_t i = 0; i < v.len; i++) { T *p = vec_at(&v, i); } */

#endif /* CSTRUCTS_VEC_H */
