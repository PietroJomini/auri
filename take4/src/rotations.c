#include "rotations.h"

// is it faster to just check a pre-made lookup table?
// also, check for more bitwise black magic for the non-power of two sizes
uint8_t mirror_h3(uint8_t n) { return n + ((1 - (n % 3)) << 1); }
uint8_t mirror_h4(uint8_t n) { return n ^ 3; }  // n + ((2 - (n % 4)) << 1) - 1
uint8_t mirror_h5(uint8_t n) { return n + ((2 - (n % 5)) << 1); }
uint8_t mirror_h6(uint8_t n) { return n + ((3 - (n % 6)) << 1) - 1; }
uint8_t mirror_h7(uint8_t n) { return n + ((3 - (n % 7)) << 1); }
uint8_t mirror_h8(uint8_t n) { return n ^ 7; }  // n + ((4 - (n % 8)) << 1) - 1

uint8_t mirror_v3(uint8_t n) { return n + 6 * (1 - n / 3); }
uint8_t mirror_v4(uint8_t n) { return n ^ 12; }
uint8_t mirror_v5(uint8_t n) { return n + 10 * (2 - n / 5); }
uint8_t mirror_v6(uint8_t n) { return n + 12 * (2 - n / 6) + 1; }
uint8_t mirror_v7(uint8_t n) { return n + 14 * (3 - n / 7); }
uint8_t mirror_v8(uint8_t n) { return n ^ 56; }