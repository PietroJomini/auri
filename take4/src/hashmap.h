#pragma once

#include <stdlib.h>

#include "../base/random.h"
#include "tak.h"

// the poorest^tm hashmap ever
typedef struct {
    // using directly the positions insthead of having a separate hmap_items
    // makes things easier, but negate the option of using separate chaining.
    // i would argue that's fine, since double hasing and open addressing are
    // better anyway
    // maybe it's also a good idea to test having each bucket preallocate
    // a small array (eg. 5 positions) and, when full, override choosing
    // based on some tought parameters (eg. oldest, or less read?) like stockfish
    position **items;  // test also with an array of elements all at 0,
                       // not of pointers. maybe it's slower to create
                       // but faster to insert and search?

    // metrics
    u64 capacity;   // amount of allocated *items
    u64 size;       // n of elements
    u64 overrides;  // n of overridden elements
    u64 jumps;      // n of jumps needed to reach items
} hmap;

hmap *hmap_new(u64 capacity);
void hmap_free(hmap *hm);

// handle stuff
void hmap_set(hmap *hm, position p);
position *hmap_get(hmap *hm, u64 hash);