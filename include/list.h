/**
 * list.h — Doubly-linked list for C
 * Part of cstructs: https://github.com/qorexdev/cstructs
 */

#ifndef CSTRUCTS_LIST_H
#define CSTRUCTS_LIST_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    /* payload follows in memory (allocated together) */
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t    len;
    size_t    elem_size;
} List;

static inline void list_init(List *l, size_t elem_size) {
    l->head = l->tail = NULL;
    l->len       = 0;
    l->elem_size = elem_size;
}

static inline void *_list_node_data(ListNode *n) {
    return (char *)n + sizeof(ListNode);
}

static inline ListNode *_list_new_node(const List *l, const void *elem) {
    ListNode *n = (ListNode *)malloc(sizeof(ListNode) + l->elem_size);
    if (!n) return NULL;
    n->prev = n->next = NULL;
    memcpy(_list_node_data(n), elem, l->elem_size);
    return n;
}

/* Push copy of `elem` to back. Returns 0 on success. */
static inline int list_push_back(List *l, const void *elem) {
    ListNode *n = _list_new_node(l, elem);
    if (!n) return -1;
    n->prev = l->tail;
    if (l->tail) l->tail->next = n;
    else         l->head = n;
    l->tail = n;
    l->len++;
    return 0;
}

/* Push copy of `elem` to front. Returns 0 on success. */
static inline int list_push_front(List *l, const void *elem) {
    ListNode *n = _list_new_node(l, elem);
    if (!n) return -1;
    n->next = l->head;
    if (l->head) l->head->prev = n;
    else         l->tail = n;
    l->head = n;
    l->len++;
    return 0;
}

/* Pop from back into `out` (may be NULL). Returns 0 on success. */
static inline int list_pop_back(List *l, void *out) {
    if (!l->tail) return -1;
    ListNode *n = l->tail;
    if (out) memcpy(out, _list_node_data(n), l->elem_size);
    l->tail = n->prev;
    if (l->tail) l->tail->next = NULL;
    else         l->head = NULL;
    free(n);
    l->len--;
    return 0;
}

/* Pop from front into `out` (may be NULL). Returns 0 on success. */
static inline int list_pop_front(List *l, void *out) {
    if (!l->head) return -1;
    ListNode *n = l->head;
    if (out) memcpy(out, _list_node_data(n), l->elem_size);
    l->head = n->next;
    if (l->head) l->head->prev = NULL;
    else         l->tail = NULL;
    free(n);
    l->len--;
    return 0;
}

/* Free all nodes. */
static inline void list_free(List *l) {
    ListNode *cur = l->head;
    while (cur) {
        ListNode *next = cur->next;
        free(cur);
        cur = next;
    }
    l->head = l->tail = NULL;
    l->len = 0;
}

/* Macro to iterate forward:
 * LIST_FOREACH(node, &list) { T *p = (T*)_list_node_data(node); } */
#define LIST_FOREACH(node, list_ptr) \
    for (ListNode *node = (list_ptr)->head; node != NULL; node = node->next)

#endif /* CSTRUCTS_LIST_H */
