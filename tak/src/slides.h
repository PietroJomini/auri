#pragma once
#include <stdint.h>

#define SLIDES_MAX_LENGTH 8
#define SLIDES_AMOUNT 503

// length of each slide
extern const uint8_t SLIDES_LENGHT[SLIDES_AMOUNT];

// slides are computed by the script `scripts/slides.py`
extern const uint8_t SLIDES[SLIDES_AMOUNT][SLIDES_MAX_LENGTH];

// return the amount of slides that can be generated for
// a stack of height `h` as `2^(h-1)`
inline uint8_t slides_count(uint8_t h) { return h < 8 ? 1 << (h) : (1 << h) - 1; }

// return the index of the first branch of a given height
// indeces are manually located in the file, or can be computed
// with the python script
extern const uint8_t SLIDES_INDEX[6];
inline uint8_t slides_index(uint8_t h) { return SLIDES_INDEX[h - 3]; }
