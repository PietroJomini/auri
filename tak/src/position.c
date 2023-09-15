#include "position.h"

// amount of pieces per board size
const uint8_t SETUP[6][2] = {{10, 0}, {15, 0}, {21, 1}, {30, 1}, {40, 2}, {50, 2}};

Position pempty() {
    return (Position){.size = 0,
                      .stp = 0,
                      .mc = 0,
                      .white = 0,
                      .black = 0,
                      .walls = 0,
                      .caps = 0,
                      .reserve = {{0, 0}, {0, 0}}};
}

// adaptation http://graphics.stanford.edu/~seander/bithacks.html
// if this ends up becoming too slow we can use a byte-level lookup table
// to swap bytes internally and then swap endiannes with assembly,
// but this should suffice
uint64_t mirror(uint64_t bb) {
    bb = ((bb & 0x5555555555555555) << 1) | ((bb & 0xAAAAAAAAAAAAAAAA) >> 1);
    bb = ((bb & 0x3333333333333333) << 2) | ((bb & 0xCCCCCCCCCCCCCCCC) >> 2);
    bb = ((bb & 0x0F0F0F0F0F0F0F0F) << 4) | ((bb & 0xF0F0F0F0F0F0F0F0) >> 4);
    bb = ((bb & 0x00FF00FF00FF00FF) << 8) | ((bb & 0xFF00FF00FF00FF00) >> 8);
    bb = ((bb & 0x0000FFFF0000FFFF) << 16) | ((bb & 0xFFFF0000FFFF0000) >> 16);
    return (bb << 32) | (bb >> 32);
}

// removes chunks from the "end" (msb) of the input bb and
// appends them to the "beginning" (lsb) of the reversed bb.
// if 64 is not a multiple of n, the smaller chunk that remains
// at the end will then be the most significant one in the
// reversed output.
// TODO: doesn't it leave some errors with the remaining smaller chunk
// at the end? it won't matter usually, but it's safer to test.
// TODO: i can also mirror each n-sized chunk and then mirror the
//       entire stuff, but i don't think it's worth? benchmark...
//          YZ ABC DEF GHI =>
//          ZY CBA FED IHG =>
//          GHI DEF ABC YZ
uint64_t rotate(uint64_t bb, int n) {
    uint64_t rb = 0;
    uint8_t mask = (2 << (n - 1)) - 1;
    for (int i = 0; i < 64 / n; i++) {
        rb |= (bb & mask) << (64 - n * (i + 1));
        bb >>= n;
    }
    return rb;
}