#ifndef CSTRUCTS_TRIE_H
#define CSTRUCTS_TRIE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define TRIE_ALPHA 26

typedef struct TrieNode {
    struct TrieNode *children[TRIE_ALPHA];
    int end;
} TrieNode;

typedef struct {
    TrieNode *root;
    size_t    len;
} Trie;

static inline TrieNode *_trie_new_node(void) {
    TrieNode *n = (TrieNode *)calloc(1, sizeof(TrieNode));
    return n;
}

static inline int trie_init(Trie *t) {
    t->len  = 0;
    t->root = _trie_new_node();
    return t->root ? 0 : -1;
}

static inline int trie_insert(Trie *t, const char *word) {
    TrieNode *cur = t->root;
    for (; *word; word++) {
        int idx = *word - 'a';
        if (idx < 0 || idx >= TRIE_ALPHA) return -1;
        if (!cur->children[idx]) {
            cur->children[idx] = _trie_new_node();
            if (!cur->children[idx]) return -1;
        }
        cur = cur->children[idx];
    }
    if (!cur->end) {
        cur->end = 1;
        t->len++;
    }
    return 0;
}

static inline int trie_search(const Trie *t, const char *word) {
    TrieNode *cur = t->root;
    for (; *word; word++) {
        int idx = *word - 'a';
        if (idx < 0 || idx >= TRIE_ALPHA) return 0;
        if (!cur->children[idx]) return 0;
        cur = cur->children[idx];
    }
    return cur->end;
}

static inline int trie_starts_with(const Trie *t, const char *prefix) {
    TrieNode *cur = t->root;
    for (; *prefix; prefix++) {
        int idx = *prefix - 'a';
        if (idx < 0 || idx >= TRIE_ALPHA) return 0;
        if (!cur->children[idx]) return 0;
        cur = cur->children[idx];
    }
    return 1;
}

static inline int _trie_has_children(const TrieNode *n) {
    for (int i = 0; i < TRIE_ALPHA; i++)
        if (n->children[i]) return 1;
    return 0;
}

static inline int _trie_delete(TrieNode *node, const char *word, size_t depth, int *removed) {
    if (!node) return 0;

    if (*word == '\0') {
        if (!node->end) return 0;
        node->end = 0;
        *removed = 1;
        return !_trie_has_children(node);
    }

    int idx = word[0] - 'a';
    if (idx < 0 || idx >= TRIE_ALPHA || !node->children[idx])
        return 0;

    int should_free = _trie_delete(node->children[idx], word + 1, depth + 1, removed);
    if (should_free) {
        free(node->children[idx]);
        node->children[idx] = NULL;
        return !node->end && !_trie_has_children(node);
    }
    return 0;
}

static inline int trie_delete(Trie *t, const char *word) {
    int removed = 0;
    _trie_delete(t->root, word, 0, &removed);
    if (removed) t->len--;
    return removed ? 0 : -1;
}

static inline void _trie_free_node(TrieNode *n) {
    if (!n) return;
    for (int i = 0; i < TRIE_ALPHA; i++)
        _trie_free_node(n->children[i]);
    free(n);
}

static inline void trie_free(Trie *t) {
    _trie_free_node(t->root);
    t->root = NULL;
    t->len  = 0;
}

#endif
