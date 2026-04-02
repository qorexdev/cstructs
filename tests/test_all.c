#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/cstructs.h"

static void test_vec(void) {
    Vec v;
    assert(vec_init(&v, sizeof(int)) == 0);

    for (int i = 0; i < 20; i++) assert(vec_push(&v, &i) == 0);
    assert(v.len == 20);
    assert(*(int *)vec_at(&v, 0)  == 0);
    assert(*(int *)vec_at(&v, 19) == 19);

    int out;
    assert(vec_pop(&v, &out) == 0 && out == 19);
    assert(v.len == 19);

    assert(vec_remove(&v, 0) == 0);
    assert(*(int *)vec_at(&v, 0) == 1);
    assert(v.len == 18);

    vec_free(&v);
    assert(v.data == NULL);
    puts("[PASS] Vec");
}

static void test_map(void) {
    Map m;
    assert(map_init(&m) == 0);

    int vals[5] = {1, 2, 3, 4, 5};
    char keys[5][16] = {"one","two","three","four","five"};
    for (int i = 0; i < 5; i++) assert(map_set(&m, keys[i], &vals[i]) == 0);

    assert(*(int *)map_get(&m, "three") == 3);
    assert(map_has(&m, "five")  == 1);
    assert(map_has(&m, "six")   == 0);
    assert(map_get(&m, "zero")  == NULL);

    assert(map_del(&m, "two") == 0);
    assert(map_has(&m, "two") == 0);
    assert(m.len == 4);

    int updated = 99;
    map_set(&m, "one", &updated);
    assert(*(int *)map_get(&m, "one") == 99);

    map_free(&m);
    puts("[PASS] Map");
}

static void test_list(void) {
    List l;
    list_init(&l, sizeof(int));

    for (int i = 1; i <= 3; i++) assert(list_push_back(&l, &i) == 0);
    int zero = 0;
    assert(list_push_front(&l, &zero) == 0);
    assert(l.len == 4);

    int out;
    assert(list_pop_front(&l, &out) == 0 && out == 0);
    assert(list_pop_back(&l,  &out) == 0 && out == 3);
    assert(l.len == 2);

    list_free(&l);
    assert(l.head == NULL && l.tail == NULL);
    puts("[PASS] List");
}

static void test_queue(void) {
    Queue q;
    assert(queue_init(&q, sizeof(int)) == 0);

    for (int i = 0; i < 16; i++) assert(queue_push(&q, &i) == 0);
    assert(q.len == 16);
    assert(*(int *)queue_front(&q) == 0);

    int out;
    for (int i = 0; i < 16; i++) {
        assert(queue_pop(&q, &out) == 0);
        assert(out == i);
    }
    assert(q.len == 0);
    assert(queue_pop(&q, NULL) == -1);

    queue_free(&q);
    puts("[PASS] Queue");
}

static void test_set(void) {
    Set s;
    assert(set_init(&s) == 0);

    assert(set_add(&s, "apple")  == 0);
    assert(set_add(&s, "banana") == 0);
    assert(set_add(&s, "cherry") == 0);
    assert(s.len == 3);

    assert(set_add(&s, "apple") == 0);
    assert(s.len == 3);

    assert(set_has(&s, "banana") == 1);
    assert(set_has(&s, "grape")  == 0);

    assert(set_del(&s, "banana") == 0);
    assert(set_has(&s, "banana") == 0);
    assert(s.len == 2);

    assert(set_del(&s, "missing") == -1);

    for (int i = 0; i < 50; i++) {
        char buf[32];
        sprintf(buf, "item_%d", i);
        assert(set_add(&s, buf) == 0);
    }
    assert(s.len == 52);

    set_free(&s);
    assert(s.buckets == NULL);
    puts("[PASS] Set");
}

static void test_trie(void) {
    Trie t;
    assert(trie_init(&t) == 0);

    assert(trie_insert(&t, "apple") == 0);
    assert(trie_insert(&t, "app") == 0);
    assert(trie_insert(&t, "banana") == 0);
    assert(t.len == 3);

    assert(trie_search(&t, "apple") == 1);
    assert(trie_search(&t, "app") == 1);
    assert(trie_search(&t, "ap") == 0);
    assert(trie_search(&t, "banana") == 1);
    assert(trie_search(&t, "ban") == 0);

    assert(trie_starts_with(&t, "app") == 1);
    assert(trie_starts_with(&t, "ban") == 1);
    assert(trie_starts_with(&t, "cat") == 0);
    assert(trie_starts_with(&t, "") == 1);

    // duplicate insert shouldn't change len
    assert(trie_insert(&t, "apple") == 0);
    assert(t.len == 3);

    assert(trie_delete(&t, "apple") == 0);
    assert(trie_search(&t, "apple") == 0);
    assert(trie_search(&t, "app") == 1);
    assert(t.len == 2);

    assert(trie_delete(&t, "missing") == -1);

    assert(trie_delete(&t, "app") == 0);
    assert(trie_starts_with(&t, "app") == 0);
    assert(t.len == 1);

    trie_free(&t);
    assert(t.root == NULL);
    puts("[PASS] Trie");
}

int main(void) {
    puts("Running cstructs tests...");
    test_vec();
    test_map();
    test_list();
    test_queue();
    test_set();
    test_trie();
    puts("\nAll tests passed.");
    return 0;
}
