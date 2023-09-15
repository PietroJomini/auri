#pragma once

#include <stdint.h>

#include "position.h"

typedef struct {
    Piece piece;
    uint64_t at;
} Placement;

// counts the empty spaces in the board
uint8_t placements_count(Position p);

// compute the possible placements in the given position
// `buffer` should be at least `placements_count(p)` long
void placements(Placement *buffer, Position p);