#include "rot.h"

// is it faster to just check a pre-made lookup table?
// also, check for more bitwise black magic for the non-power of two sizes
// TODO: also 2, maybe inlining all of them?
uint8_t mirror_h3(uint8_t n) { return n + ((1 - (n % 3)) << 1); }
uint8_t mirror_h4(uint8_t n) { return n ^ 3; }  // n + ((2 - (n % 4)) << 1) - 1
uint8_t mirror_h5(uint8_t n) { return n + ((2 - (n % 5)) << 1); }
uint8_t mirror_h6(uint8_t n) { return n + ((3 - (n % 6)) << 1) - 1; }
uint8_t mirror_h7(uint8_t n) { return n + ((3 - (n % 7)) << 1); }
uint8_t mirror_h8(uint8_t n) { return n ^ 7; }  // n + ((4 - (n % 8)) << 1) - 1

uint8_t mirror_v3(uint8_t n) { return n + 6 * (1 - n / 3); }
uint8_t mirror_v4(uint8_t n) { return n ^ 12; }
uint8_t mirror_v5(uint8_t n) { return n + 10 * (2 - n / 5); }
uint8_t mirror_v6(uint8_t n) { return n + 12 * (2 - n / 6) + 6; }
uint8_t mirror_v7(uint8_t n) { return n + 14 * (3 - n / 7); }
uint8_t mirror_v8(uint8_t n) { return n ^ 56; }

// i could check if n is in the diagonal, but i guess and to
// assignements just to avoid a plus and a multiplication is not
// really worth
// TODO: chessprogramming gives an absurd looking algorithm
//       for the 8x8, i should understand what it does and if
//       it's usable for the other sizes, or at least for 4x4
//          sq' = ((sq >> 3) | (sq << 3)) & 63;
//          sq' = (sq * 0x20800000) >>> 26;
uint8_t mirror_d3(uint8_t n) { return (n % 3) * 3 + n / 3; }
uint8_t mirror_d4(uint8_t n) { return (n % 4) * 4 + n / 4; }
uint8_t mirror_d5(uint8_t n) { return (n % 5) * 5 + n / 5; }
uint8_t mirror_d6(uint8_t n) { return (n % 6) * 6 + n / 6; }
uint8_t mirror_d7(uint8_t n) { return (n % 7) * 7 + n / 7; }
uint8_t mirror_d8(uint8_t n) { return ((n >> 3) | (n << 3)) & 63; }

uint8_t mirror_a3(uint8_t n) { return mirror_d3(rotate_3(n)); }
uint8_t mirror_a4(uint8_t n) { return mirror_d4(rotate_4(n)); }
uint8_t mirror_a5(uint8_t n) { return mirror_d5(rotate_5(n)); }
uint8_t mirror_a6(uint8_t n) { return mirror_d6(rotate_6(n)); }
uint8_t mirror_a7(uint8_t n) { return mirror_d7(rotate_7(n)); }
uint8_t mirror_a8(uint8_t n) { return mirror_d8(rotate_8(n)); }

uint8_t rotate_3(uint8_t n) { return 8 - n; }
uint8_t rotate_4(uint8_t n) { return 15 ^ n; }  // 15 - n
uint8_t rotate_5(uint8_t n) { return 24 - n; }
uint8_t rotate_6(uint8_t n) { return 35 - n; }
uint8_t rotate_7(uint8_t n) { return 48 - n; }
uint8_t rotate_8(uint8_t n) { return 63 ^ n; }  // 63 - n

// can be done by both
//  - diag + h + 180 = -90 + 180
//  - antidag + h
// TODO: check which is faster
uint8_t rotate_c3(uint8_t n) { return rotate_3(rotate_a3(n)); }
uint8_t rotate_c4(uint8_t n) { return rotate_4(rotate_a4(n)); }
uint8_t rotate_c5(uint8_t n) { return rotate_5(rotate_a5(n)); }
uint8_t rotate_c6(uint8_t n) { return rotate_6(rotate_a6(n)); }
uint8_t rotate_c7(uint8_t n) { return rotate_7(rotate_a7(n)); }
uint8_t rotate_c8(uint8_t n) { return rotate_8(rotate_a8(n)); }

uint8_t rotate_a3(uint8_t n) { return mirror_h3(mirror_d3(n)); }
uint8_t rotate_a4(uint8_t n) { return mirror_h4(mirror_d4(n)); }
uint8_t rotate_a5(uint8_t n) { return mirror_h5(mirror_d5(n)); }
uint8_t rotate_a6(uint8_t n) { return mirror_h6(mirror_d6(n)); }
uint8_t rotate_a7(uint8_t n) { return mirror_h7(mirror_d7(n)); }
uint8_t rotate_a8(uint8_t n) { return mirror_h8(mirror_d8(n)); }

uint8_t mirror_h(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return mirror_h3(n);
        case 4: return mirror_h4(n);
        case 5: return mirror_h5(n);
        case 6: return mirror_h6(n);
        case 7: return mirror_h7(n);
        case 8: return mirror_h8(n);
    }

    return 0;  // should never reach
}

uint8_t mirror_v(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return mirror_v3(n);
        case 4: return mirror_v4(n);
        case 5: return mirror_v5(n);
        case 6: return mirror_v6(n);
        case 7: return mirror_v7(n);
        case 8: return mirror_v8(n);
    }

    return 0;  // should never reach
}

uint8_t mirror_d(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return mirror_d3(n);
        case 4: return mirror_d4(n);
        case 5: return mirror_d5(n);
        case 6: return mirror_d6(n);
        case 7: return mirror_d7(n);
        case 8: return mirror_d8(n);
    }

    return 0;  // should never reach
}

uint8_t mirror_a(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return mirror_a3(n);
        case 4: return mirror_a4(n);
        case 5: return mirror_a5(n);
        case 6: return mirror_a6(n);
        case 7: return mirror_a7(n);
        case 8: return mirror_a8(n);
    }

    return 0;  // should never reach
}

uint8_t rotate(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return rotate_3(n);
        case 4: return rotate_4(n);
        case 5: return rotate_5(n);
        case 6: return rotate_6(n);
        case 7: return rotate_7(n);
        case 8: return rotate_8(n);
    }

    return 0;  // should never reach
}

uint8_t rotate_c(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return rotate_c3(n);
        case 4: return rotate_c4(n);
        case 5: return rotate_c5(n);
        case 6: return rotate_c6(n);
        case 7: return rotate_c7(n);
        case 8: return rotate_c8(n);
    }

    return 0;  // should never reach
}

uint8_t rotate_a(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return rotate_a3(n);
        case 4: return rotate_a4(n);
        case 5: return rotate_a5(n);
        case 6: return rotate_a6(n);
        case 7: return rotate_a7(n);
        case 8: return rotate_a8(n);
    }

    return 0;  // should never reach
}