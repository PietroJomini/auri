#include "tt.h"

#include <string.h>

#include "rot.h"
#include "tak.h"

hmap *hmap_new(u64 capacity) {
    hmap *hm = malloc(sizeof(hmap));
    hm->items = calloc(capacity, sizeof(position *));
    hm->capacity = capacity;
    return hm;
}

void hmap_free(hmap *hm) {
    for (u64 i = 0; i < hm->capacity; i++)
        if (hm->items[i]) free(hm->items[i]);
    free(hm->items);
    free(hm);
}

void hmap_set(hmap *hm, position p) {
    u64 index = p.hash[0] % hm->capacity;
    position *address = hm->items[index];

    // find the right address with linear probing
    // TODO: somehow (in testing, random positions, capacity 100
    // and 100 elements) sometimes (2) the probing don't find an ampty
    // spot? but if i add 5 more elemets it fills the empty buckets
    u64 probe = 0,
        override = hm->size == hm->capacity;  // maybe i can also test starting to
                                              // override at a certain fill rate? 80%?
    while (address != NULL && !override && probe < hm->capacity) {
        index = (index + probe) % hm->capacity;
        address = hm->items[index];

        // maybe in the future test also double hasing and
        // other probing strategies.
        probe += 1;
    }

    // it would be faster to just pass the pointer and save
    // the original one, but if it's get modifier (unlikely, with how
    // my do_move works, but still) the key mapping would broke.
    // if i see that in implementations this would be unlikely, i can
    // still change this
    position *i = malloc(sizeof(position));
    memcpy(i, &p, sizeof(position));
    hm->items[index] = i;

    // update metrics
    hm->jumps += probe;
    if (override || probe == hm->capacity) hm->overrides++;
    else hm->size++;
}

position *hmap_get(hmap *hm, u64 hash) {
    u64 index = hash % hm->capacity;
    position *address = hm->items[index];

    // find the right address with linear probing
    u64 probe = 0;
    while (address == NULL && address->hash[0] != hash && probe < hm->capacity) {
        index = (index + probe) % hm->capacity;
        address = hm->items[index];
        probe += 1;
    }

    return (probe < hm->capacity) ? address : NULL;
}

position *hmap_search(hmap *hm, position *p) {
    position *s;

    // this way if there are more than one "rotated" board in the
    // tt this will only spot the forst one, but the whole scenario
    // shouldn't be possible if before setting a position we check if
    // it's already there
    // also, this way when the tt is full we run a lot more probing cycles
    // than necessary, maybe find a way to optimize? (buckets and no probing?)
    for (int i = 0; i < N_ROT; i++) {
        s = hmap_get(hm, p->hash[i]);
        if (s != NULL) return s;
    }

    return NULL;
}
