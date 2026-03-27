/**
 * map.h — Open-addressing hash map (string keys, void* values) for C
 * Part of cstructs: https://github.com/qorexdev/cstructs
 */

#ifndef CSTRUCTS_MAP_H
#define CSTRUCTS_MAP_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* MSVC deprecates POSIX strdup in favour of _strdup */
#ifdef _MSC_VER
#  define _cs_strdup _strdup
#else
#  define _cs_strdup strdup
#endif

#define MAP_INIT_CAP    16
#define MAP_LOAD_FACTOR 0.70

typedef struct MapEntry {
    char  *key;
    void  *value;
    int    occupied;
} MapEntry;

typedef struct {
    MapEntry *buckets;
    size_t    cap;
    size_t    len;
} Map;

static inline unsigned long _map_hash(const char *s) {
    unsigned long h = 5381;
    while (*s) h = h * 33 ^ (unsigned char)*s++;
    return h;
}

static inline int map_init(Map *m) {
    m->len     = 0;
    m->cap     = MAP_INIT_CAP;
    m->buckets = (MapEntry *)calloc(MAP_INIT_CAP, sizeof(MapEntry));
    return m->buckets ? 0 : -1;
}

static inline void map_free(Map *m) {
    for (size_t i = 0; i < m->cap; i++)
        if (m->buckets[i].occupied) free(m->buckets[i].key);
    free(m->buckets);
    m->buckets = NULL;
    m->len = m->cap = 0;
}

static inline int _map_insert_raw(MapEntry *buckets, size_t cap,
                                   const char *key, void *value) {
    size_t idx = _map_hash(key) % cap;
    while (buckets[idx].occupied) {
        if (strcmp(buckets[idx].key, key) == 0) {
            buckets[idx].value = value;
            return 0;
        }
        idx = (idx + 1) % cap;
    }
    buckets[idx].key = _cs_strdup(key);
    if (!buckets[idx].key) return -1;
    buckets[idx].value    = value;
    buckets[idx].occupied = 1;
    return 1; /* new entry */
}

static inline int _map_grow(Map *m) {
    size_t    new_cap     = m->cap * 2;
    MapEntry *new_buckets = (MapEntry *)calloc(new_cap, sizeof(MapEntry));
    if (!new_buckets) return -1;
    for (size_t i = 0; i < m->cap; i++) {
        if (m->buckets[i].occupied)
            _map_insert_raw(new_buckets, new_cap,
                            m->buckets[i].key, m->buckets[i].value);
    }
    /* free old keys (already copied by strdup in _map_insert_raw) */
    for (size_t i = 0; i < m->cap; i++)
        if (m->buckets[i].occupied) free(m->buckets[i].key);
    free(m->buckets);
    m->buckets = new_buckets;
    m->cap     = new_cap;
    return 0;
}

/* Insert or update key→value. Returns 0 on success, -1 on OOM. */
static inline int map_set(Map *m, const char *key, void *value) {
    if ((double)(m->len + 1) / (double)m->cap > MAP_LOAD_FACTOR)
        if (_map_grow(m) != 0) return -1;
    int r = _map_insert_raw(m->buckets, m->cap, key, value);
    if (r == 1) m->len++;
    return r < 0 ? -1 : 0;
}

/* Get value for key, or NULL if not found. */
static inline void *map_get(const Map *m, const char *key) {
    size_t idx = _map_hash(key) % m->cap;
    for (size_t i = 0; i < m->cap; i++) {
        size_t probe = (idx + i) % m->cap;
        if (!m->buckets[probe].occupied) return NULL;
        if (strcmp(m->buckets[probe].key, key) == 0)
            return m->buckets[probe].value;
    }
    return NULL;
}

/* Returns 1 if key exists, 0 otherwise. */
static inline int map_has(const Map *m, const char *key) {
    return map_get(m, key) != NULL;
}

/* Delete key. Returns 0 if removed, -1 if not found. */
static inline int map_del(Map *m, const char *key) {
    size_t idx = _map_hash(key) % m->cap;
    for (size_t i = 0; i < m->cap; i++) {
        size_t probe = (idx + i) % m->cap;
        if (!m->buckets[probe].occupied) return -1;
        if (strcmp(m->buckets[probe].key, key) == 0) {
            free(m->buckets[probe].key);
            m->buckets[probe].occupied = 0;
            m->buckets[probe].key      = NULL;
            m->len--;
            return 0;
        }
    }
    return -1;
}

/* Iterate: for (size_t i = 0; i < m->cap; i++) {
 *   if (m->buckets[i].occupied) { ... m->buckets[i].key / .value }
 * } */

#endif /* CSTRUCTS_MAP_H */
