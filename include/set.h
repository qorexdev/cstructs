/**
 * set.h — Open-addressing hash set (string elements) for C
 * Part of cstructs: https://github.com/qorexdev/cstructs
 */

#ifndef CSTRUCTS_SET_H
#define CSTRUCTS_SET_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#  define _cs_set_strdup _strdup
#else
#  define _cs_set_strdup strdup
#endif

#define SET_INIT_CAP    16
#define SET_LOAD_FACTOR 0.70

typedef struct {
    char **buckets;
    int   *occupied;
    size_t cap;
    size_t len;
} Set;

static inline unsigned long _set_hash(const char *s) {
    unsigned long h = 5381;
    while (*s) h = h * 33 ^ (unsigned char)*s++;
    return h;
}

static inline int set_init(Set *s) {
    s->len      = 0;
    s->cap      = SET_INIT_CAP;
    s->buckets  = (char **)calloc(SET_INIT_CAP, sizeof(char *));
    s->occupied = (int *)calloc(SET_INIT_CAP, sizeof(int));
    if (!s->buckets || !s->occupied) {
        free(s->buckets);
        free(s->occupied);
        return -1;
    }
    return 0;
}

static inline void set_free(Set *s) {
    for (size_t i = 0; i < s->cap; i++)
        if (s->occupied[i]) free(s->buckets[i]);
    free(s->buckets);
    free(s->occupied);
    s->buckets  = NULL;
    s->occupied = NULL;
    s->len = s->cap = 0;
}

static inline int _set_insert_raw(char **buckets, int *occupied,
                                    size_t cap, const char *elem) {
    size_t idx = _set_hash(elem) % cap;
    while (occupied[idx]) {
        if (strcmp(buckets[idx], elem) == 0) return 0;
        idx = (idx + 1) % cap;
    }
    buckets[idx] = _cs_set_strdup(elem);
    if (!buckets[idx]) return -1;
    occupied[idx] = 1;
    return 1;
}

static inline int _set_grow(Set *s) {
    size_t new_cap  = s->cap * 2;
    char **new_buck = (char **)calloc(new_cap, sizeof(char *));
    int   *new_occ  = (int *)calloc(new_cap, sizeof(int));
    if (!new_buck || !new_occ) {
        free(new_buck);
        free(new_occ);
        return -1;
    }
    for (size_t i = 0; i < s->cap; i++)
        if (s->occupied[i])
            _set_insert_raw(new_buck, new_occ, new_cap, s->buckets[i]);
    for (size_t i = 0; i < s->cap; i++)
        if (s->occupied[i]) free(s->buckets[i]);
    free(s->buckets);
    free(s->occupied);
    s->buckets  = new_buck;
    s->occupied = new_occ;
    s->cap      = new_cap;
    return 0;
}

/* Add element to set. Returns 0 on success, -1 on OOM. */
static inline int set_add(Set *s, const char *elem) {
    if ((double)(s->len + 1) / (double)s->cap > SET_LOAD_FACTOR)
        if (_set_grow(s) != 0) return -1;
    int r = _set_insert_raw(s->buckets, s->occupied, s->cap, elem);
    if (r == 1) s->len++;
    return r < 0 ? -1 : 0;
}

/* Returns 1 if element exists, 0 otherwise. */
static inline int set_has(const Set *s, const char *elem) {
    size_t idx = _set_hash(elem) % s->cap;
    for (size_t i = 0; i < s->cap; i++) {
        size_t probe = (idx + i) % s->cap;
        if (!s->occupied[probe]) return 0;
        if (strcmp(s->buckets[probe], elem) == 0) return 1;
    }
    return 0;
}

/* Remove element. Returns 0 if removed, -1 if not found. */
static inline int set_del(Set *s, const char *elem) {
    size_t idx = _set_hash(elem) % s->cap;
    for (size_t i = 0; i < s->cap; i++) {
        size_t probe = (idx + i) % s->cap;
        if (!s->occupied[probe]) return -1;
        if (strcmp(s->buckets[probe], elem) == 0) {
            free(s->buckets[probe]);
            s->buckets[probe]  = NULL;
            s->occupied[probe] = 0;
            s->len--;
            return 0;
        }
    }
    return -1;
}

/* Iterate: for (size_t i = 0; i < s->cap; i++) {
 *   if (s->occupied[i]) { ... s->buckets[i] }
 * } */

#endif /* CSTRUCTS_SET_H */
