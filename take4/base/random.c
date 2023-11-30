#include "random.h"

uint64_t xoshiro_rotate(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

uint64_t xoshiro(xoshiro_state *state) {
    uint64_t *s = state->s;
    uint64_t const result = xoshiro_rotate(s[1] * 5, 7);
    uint64_t const t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = xoshiro_rotate(s[3], 45);

    return result;
}