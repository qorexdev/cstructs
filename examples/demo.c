#include <stdio.h>
#include "../include/cstructs.h"

/* ── Vec demo ──────────────────────────────────────────────────── */
static void demo_vec(void) {
    puts("=== Vec ===");
    Vec v;
    vec_init(&v, sizeof(int));

    for (int i = 0; i < 10; i++) vec_push(&v, &i);

    printf("len=%zu  cap=%zu\n", v.len, v.cap);
    for (size_t i = 0; i < v.len; i++)
        printf("%d ", *(int *)vec_at(&v, i));
    putchar('\n');

    int popped;
    vec_pop(&v, &popped);
    printf("popped: %d  new len: %zu\n", popped, v.len);

    vec_free(&v);
}

/* ── Map demo ──────────────────────────────────────────────────── */
static void demo_map(void) {
    puts("\n=== Map ===");
    Map m;
    map_init(&m);

    int a = 42, b = 100;
    map_set(&m, "answer",  &a);
    map_set(&m, "perfect", &b);

    printf("answer  = %d\n", *(int *)map_get(&m, "answer"));
    printf("perfect = %d\n", *(int *)map_get(&m, "perfect"));
    printf("missing = %p\n", map_get(&m, "missing"));

    map_del(&m, "answer");
    printf("after del, has 'answer': %d\n", map_has(&m, "answer"));

    map_free(&m);
}

/* ── List demo ─────────────────────────────────────────────────── */
static void demo_list(void) {
    puts("\n=== List ===");
    List l;
    list_init(&l, sizeof(int));

    for (int i = 1; i <= 5; i++) list_push_back(&l, &i);
    int zero = 0;
    list_push_front(&l, &zero);

    printf("len=%zu: ", l.len);
    LIST_FOREACH(node, &l) printf("%d ", *(int *)_list_node_data(node));
    putchar('\n');

    int out;
    list_pop_front(&l, &out);
    printf("pop_front: %d\n", out);
    list_pop_back(&l, &out);
    printf("pop_back:  %d\n", out);

    list_free(&l);
}

/* ── Queue demo ────────────────────────────────────────────────── */
static void demo_queue(void) {
    puts("\n=== Queue ===");
    Queue q;
    queue_init(&q, sizeof(int));

    for (int i = 10; i <= 50; i += 10) queue_push(&q, &i);

    printf("front=%d  len=%zu\n", *(int *)queue_front(&q), q.len);
    int x;
    while (queue_pop(&q, &x) == 0) printf("dequeued: %d\n", x);

    queue_free(&q);
}

int main(void) {
    demo_vec();
    demo_map();
    demo_list();
    demo_queue();
    return 0;
}
