# cstructs

Portable, **header-only** data structures for C99+.
Drop one header into your project — no build system required.

Vec · Map · List · Queue · Set

| Header | Structure | Notes |
|---|---|---|
| `vec.h` | Dynamic array | Amortised O(1) push/pop |
| `map.h` | Hash map (string → void*) | Open addressing, auto-resize |
| `list.h` | Doubly-linked list | O(1) front/back ops |
| `queue.h` | FIFO queue | Circular buffer, auto-resize |
| `set.h` | Hash set (strings) | Open addressing, auto-resize |

---

## Quick start

Copy the headers you need from `include/` into your project and include them:

```c
#include "cstructs.h"   /* or individual headers */

int main(void) {
    /* Dynamic array of ints */
    Vec v;
    vec_init(&v, sizeof(int));
    int x = 42;
    vec_push(&v, &x);
    printf("%d\n", *(int *)vec_at(&v, 0));
    vec_free(&v);

    /* Hash map */
    Map m;
    map_init(&m);
    int score = 100;
    map_set(&m, "player1", &score);
    printf("%d\n", *(int *)map_get(&m, "player1"));
    map_free(&m);

    return 0;
}
```

Compile with any C99+ compiler:

```bash
cc -std=c99 -Wall -Iinclude main.c -o main
```

---

## Usage examples

### Vec

```c
#include "vec.h"
#include <stdio.h>

int main(void) {
    Vec v;
    vec_init(&v, sizeof(int));
    int vals[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) vec_push(&v, &vals[i]);
    for (size_t i = 0; i < v.len; i++)
        printf("%d ", *(int *)vec_at(&v, i));
    vec_free(&v);
}
```

### Map

```c
#include "map.h"
#include <stdio.h>

int main(void) {
    Map m;
    map_init(&m);
    int age = 25;
    map_set(&m, "alice", &age);
    if (map_has(&m, "alice"))
        printf("alice: %d\n", *(int *)map_get(&m, "alice"));
    map_del(&m, "alice");
    map_free(&m);
}
```

### List

```c
#include "list.h"
#include <stdio.h>

int main(void) {
    List l;
    list_init(&l, sizeof(double));
    double a = 1.5, b = 2.5, c = 3.5;
    list_push_back(&l, &a);
    list_push_back(&l, &b);
    list_push_front(&l, &c);
    double out;
    while (list_pop_front(&l, &out) == 0)
        printf("%.1f ", out);
    list_free(&l);
}
```

### Queue

```c
#include "queue.h"
#include <stdio.h>

int main(void) {
    Queue q;
    queue_init(&q, sizeof(int));
    for (int i = 1; i <= 5; i++) queue_push(&q, &i);
    printf("front: %d\n", *(int *)queue_front(&q));
    int out;
    while (queue_pop(&q, &out) == 0)
        printf("%d ", out);
    queue_free(&q);
}
```

### Set

```c
#include "set.h"
#include <stdio.h>

int main(void) {
    Set s;
    set_init(&s);
    set_add(&s, "apple");
    set_add(&s, "banana");
    set_add(&s, "apple");
    printf("has apple: %d\n", set_has(&s, "apple"));
    set_del(&s, "banana");
    printf("has banana: %d\n", set_has(&s, "banana"));
    set_free(&s);
}
```

---

## Building examples & tests

```bash
cmake -B build
cmake --build build
./build/demo        # run example
ctest --test-dir build --output-on-failure
```

---

## API reference

### Vec — Dynamic array

```c
int   vec_init(Vec *v, size_t elem_size);   // 0 on success
void  vec_free(Vec *v);
int   vec_push(Vec *v, const void *elem);   // 0 on success
int   vec_pop (Vec *v, void *out);          // 0 on success
void *vec_at  (const Vec *v, size_t i);     // pointer to element
int   vec_remove(Vec *v, size_t i);         // order-preserving removal
int   vec_shrink(Vec *v);                   // trim capacity to length
```

### Map — Hash map (string keys)

```c
int   map_init(Map *m);
void  map_free(Map *m);
int   map_set (Map *m, const char *key, void *value);
void *map_get (const Map *m, const char *key);   // NULL if missing
int   map_has (const Map *m, const char *key);
int   map_del (Map *m, const char *key);
```

### List — Doubly-linked list

```c
void list_init      (List *l, size_t elem_size);
void list_free      (List *l);
int  list_push_back (List *l, const void *elem);
int  list_push_front(List *l, const void *elem);
int  list_pop_back  (List *l, void *out);
int  list_pop_front (List *l, void *out);
/* iterate: LIST_FOREACH(node, &list) { void *p = _list_node_data(node); } */
```

### Queue — FIFO queue

```c
int   queue_init (Queue *q, size_t elem_size);
void  queue_free (Queue *q);
int   queue_push (Queue *q, const void *elem);
int   queue_pop  (Queue *q, void *out);
void *queue_front(const Queue *q);   // peek without removing
```

### Set — Hash set (string elements)

```c
int  set_init(Set *s);
void set_free(Set *s);
int  set_add (Set *s, const char *elem);
int  set_has (const Set *s, const char *elem);
int  set_del (Set *s, const char *elem);
```

---

## Benchmarks

Build and run `bench` to measure throughput on your machine:

```bash
cmake -B build
cmake --build build
./build/bench
```

Sample output (1M operations, MSVC Release, Windows 11):

| Structure | Insert | Lookup/Access | Delete |
|---|---|---|---|
| Vec | 5 ms | 2 ms | 2 ms |
| Map | 1535 ms | 174 ms | 101 ms |
| List | 82 ms | — | 83 ms |
| Queue | 8 ms | — | 4 ms |
| Set | 1154 ms | 130 ms | 98 ms |

Vec and Queue are backed by contiguous memory so they're the fastest.
Map/Set use open addressing with string keys — the cost comes from hashing and key comparison.

---

## Requirements

- C99 or later
- Standard library: `stdlib.h`, `string.h`, `stddef.h`

---

## License

MIT — see [LICENSE](LICENSE).

---

<div align="center">
  <sub>developed by <strong>qorex</strong> &nbsp;
    <a href="https://github.com/qorexdev">
      <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="currentColor" style="vertical-align:middle;opacity:0.6">
        <path d="M12 0C5.37 0 0 5.37 0 12c0 5.3 3.438 9.8 8.205 11.385.6.113.82-.258.82-.577 0-.285-.01-1.04-.015-2.04-3.338.724-4.042-1.61-4.042-1.61-.546-1.385-1.335-1.755-1.335-1.755-1.087-.744.084-.729.084-.729 1.205.084 1.838 1.236 1.838 1.236 1.07 1.835 2.809 1.305 3.495.998.108-.776.417-1.305.76-1.605-2.665-.3-5.466-1.332-5.466-5.93 0-1.31.465-2.38 1.235-3.22-.135-.303-.54-1.523.105-3.176 0 0 1.005-.322 3.3 1.23.96-.267 1.98-.399 3-.405 1.02.006 2.04.138 3 .405 2.28-1.552 3.285-1.23 3.285-1.23.645 1.653.24 2.873.12 3.176.765.84 1.23 1.91 1.23 3.22 0 4.61-2.805 5.625-5.475 5.92.42.36.81 1.096.81 2.22 0 1.606-.015 2.896-.015 3.286 0 .315.21.69.825.57C20.565 21.795 24 17.295 24 12c0-6.63-5.37-12-12-12"/>
      </svg>
    </a>
    &nbsp;
    <a href="https://t.me/qorexdev">
      <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="currentColor" style="vertical-align:middle;opacity:0.6">
        <path d="M11.944 0A12 12 0 0 0 0 12a12 12 0 0 0 12 12 12 12 0 0 0 12-12A12 12 0 0 0 12 0a12 12 0 0 0-.056 0zm4.962 7.224c.1-.002.321.023.465.14a.506.506 0 0 1 .171.325c.016.093.036.306.02.472-.18 1.898-.962 6.502-1.36 8.627-.168.9-.499 1.201-.82 1.23-.696.065-1.225-.46-1.9-.902-1.056-.693-1.653-1.124-2.678-1.8-1.185-.78-.417-1.21.258-1.91.177-.184 3.247-2.977 3.307-3.23.007-.032.014-.15-.056-.212s-.174-.041-.249-.024c-.106.024-1.793 1.14-5.061 3.345-.48.33-.913.49-1.302.48-.428-.008-1.252-.241-1.865-.44-.752-.245-1.349-.374-1.297-.789.027-.216.325-.437.893-.663 3.498-1.524 5.83-2.529 6.998-3.014 3.332-1.386 4.025-1.627 4.476-1.635z"/>
      </svg>
    </a>
  </sub>
</div>
