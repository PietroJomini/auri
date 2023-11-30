#pragma once

// well, semms like i need a prng
// xoshiro256** should be enough, with a period of 2^256-1
// probably for the zobrist hashing LCG would suffice, as long as i check the values,
// but for monte-carlo i'll need something stronger

// https://en.wikipedia.org/wiki/Xorshift#xoshiro256**

#include <stdint.h>

typedef struct {
    uint64_t s[4];
} xoshiro_state;

uint64_t xoshiro_rotate(uint64_t x, int k);
uint64_t xoshiro(xoshiro_state *state);