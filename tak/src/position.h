#pragma once
#include <stdint.h>

typedef enum {
    // piece type
    Flat = 1,
    Cap = 2,
    Wall = 3,

    // color of the piece
    White = 4,
    Black = 8
} Piece;

typedef enum { PType = 0b11, PColor = 0b1100 } PieceMask;

// amount of pieces per board size
extern const uint8_t SETUP[6][2];

typedef struct {
    uint8_t size;  // board size
    uint8_t stp;   // side to play
    uint8_t mc;    // moves counter

    // bitboards
    uint64_t black;
    uint64_t white;
    uint64_t caps;
    uint64_t walls;

    // stacks
    uint64_t stacks[64];  // stacks are ordered with the higest piece in the less
                          // significant bit
    uint8_t heights[64];

    // remaining pieces
    // { .0 = white, .1 = black } { .0 = flats, .1 = caps }
    uint8_t reserve[2][2];
} Position;

// create a new, empty position
Position pempty();

// bitboard of a square
inline uint64_t SQ(int index) { return 1ULL << index; }

// mirror the bits of a bitboard
// eg. ...0110101 => 1010110...
uint64_t mirror(uint64_t bb);

// rotate chunks of n bits
// eg n=3 011 010 101 => 101 010 011
uint64_t rotate(uint64_t bb, int n);