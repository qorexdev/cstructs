#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/cstructs.h"

#define N_SMALL   10000
#define N_MEDIUM  100000
#define N_LARGE   1000000

static double elapsed_ms(clock_t start, clock_t end) {
    return (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
}

static void bench_vec(int n) {
    Vec v;
    vec_init(&v, sizeof(int));

    clock_t t0 = clock();
    for (int i = 0; i < n; i++) vec_push(&v, &i);
    clock_t t1 = clock();

    volatile int tmp = 0;
    for (int i = 0; i < n; i++) tmp = *(int *)vec_at(&v, i);
    clock_t t2 = clock();
    (void)tmp;

    for (int i = 0; i < n; i++) vec_pop(&v, NULL);
    clock_t t3 = clock();

    printf("  Vec %-8d  push: %8.2f ms  access: %8.2f ms  pop: %8.2f ms\n",
           n, elapsed_ms(t0, t1), elapsed_ms(t1, t2), elapsed_ms(t2, t3));
    vec_free(&v);
}

static void bench_map(int n) {
    Map m;
    map_init(&m);

    char **keys = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        keys[i] = malloc(16);
        snprintf(keys[i], 16, "k%d", i);
    }

    int val = 42;
    clock_t t0 = clock();
    for (int i = 0; i < n; i++) map_set(&m, keys[i], &val);
    clock_t t1 = clock();

    for (int i = 0; i < n; i++) map_get(&m, keys[i]);
    clock_t t2 = clock();

    for (int i = 0; i < n; i++) map_del(&m, keys[i]);
    clock_t t3 = clock();

    printf("  Map %-8d  set:  %8.2f ms  get:    %8.2f ms  del: %8.2f ms\n",
           n, elapsed_ms(t0, t1), elapsed_ms(t1, t2), elapsed_ms(t2, t3));

    for (int i = 0; i < n; i++) free(keys[i]);
    free(keys);
    map_free(&m);
}

static void bench_list(int n) {
    List l;
    list_init(&l, sizeof(int));

    clock_t t0 = clock();
    for (int i = 0; i < n; i++) list_push_back(&l, &i);
    clock_t t1 = clock();

    for (int i = 0; i < n; i++) list_push_front(&l, &i);
    clock_t t2 = clock();

    int out;
    for (int i = 0; i < n; i++) list_pop_front(&l, &out);
    clock_t t3 = clock();

    for (int i = 0; i < n; i++) list_pop_back(&l, &out);
    clock_t t4 = clock();

    printf("  List %-7d  push_back: %6.2f ms  push_front: %6.2f ms  pop_front: %6.2f ms  pop_back: %6.2f ms\n",
           n, elapsed_ms(t0, t1), elapsed_ms(t1, t2), elapsed_ms(t2, t3), elapsed_ms(t3, t4));
    list_free(&l);
}

static void bench_queue(int n) {
    Queue q;
    queue_init(&q, sizeof(int));

    clock_t t0 = clock();
    for (int i = 0; i < n; i++) queue_push(&q, &i);
    clock_t t1 = clock();

    int out;
    for (int i = 0; i < n; i++) queue_pop(&q, &out);
    clock_t t2 = clock();

    printf("  Queue %-6d  push: %8.2f ms  pop:    %8.2f ms\n",
           n, elapsed_ms(t0, t1), elapsed_ms(t1, t2));
    queue_free(&q);
}

static void bench_set(int n) {
    Set s;
    set_init(&s);

    char **keys = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        keys[i] = malloc(16);
        snprintf(keys[i], 16, "s%d", i);
    }

    clock_t t0 = clock();
    for (int i = 0; i < n; i++) set_add(&s, keys[i]);
    clock_t t1 = clock();

    for (int i = 0; i < n; i++) set_has(&s, keys[i]);
    clock_t t2 = clock();

    for (int i = 0; i < n; i++) set_del(&s, keys[i]);
    clock_t t3 = clock();

    printf("  Set %-8d  add:  %8.2f ms  has:    %8.2f ms  del: %8.2f ms\n",
           n, elapsed_ms(t0, t1), elapsed_ms(t1, t2), elapsed_ms(t2, t3));

    for (int i = 0; i < n; i++) free(keys[i]);
    free(keys);
    set_free(&s);
}

int main(void) {
    int sizes[] = {N_SMALL, N_MEDIUM, N_LARGE};
    int nsizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("cstructs benchmark (%d runs)\n\n", nsizes);

    puts("--- Vec (dynamic array) ---");
    for (int i = 0; i < nsizes; i++) bench_vec(sizes[i]);

    puts("\n--- Map (hash map) ---");
    for (int i = 0; i < nsizes; i++) bench_map(sizes[i]);

    puts("\n--- List (doubly-linked) ---");
    for (int i = 0; i < nsizes; i++) bench_list(sizes[i]);

    puts("\n--- Queue (circular buffer) ---");
    for (int i = 0; i < nsizes; i++) bench_queue(sizes[i]);

    puts("\n--- Set (hash set) ---");
    for (int i = 0; i < nsizes; i++) bench_set(sizes[i]);

    puts("\ndone.");
    return 0;
}
