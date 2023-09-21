#pragma once

#include <stdint.h>

#include "position.h"

typedef struct {
    Piece piece;
    uint8_t at;
} Placement;

// counts the empty spaces in the board
uint8_t placements_count(Position p);

// compute the possible placements in the given position
// `buffer` should be at least `placements_count(p)` long
void placements(Placement *buffer, Position p);

typedef enum { North, East, South, West } Direction;

// get the next square in a given direction
// returns -1 if the next square would be off-board
int8_t walk(uint8_t origin, Direction dir, uint8_t size);

typedef struct {
    uint8_t origin;
    Direction direction;
    uint8_t length;
    uint32_t stacks;
    uint8_t flattens;
} Slide;

// compute all possible slides in the given position
// at a certain square, in the given direction.
// returns the amount of slides found
int slides_atdir(Slide *buffer, Position p, uint8_t origin, Direction dir);

// compute all possible slides in the given position
// at a certain square.
// returns the amount of slides found
int slides_at(Slide *buffer, Position p, uint8_t origin);
