#pragma once

#include <stdint.h>

#include "position.h"

typedef struct {
    Piece piece;
    uint8_t at;
} Placement;

// compute the possible placements in the given position.
// returns the amount of placements found
int placements(Placement *buffer, Position p);

// apply a Placement to a Position.
// it presupposes that the placement is legal
Position do_placement(Position p, Placement pl);

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

// compute all possible slides in the five position.
// returns the amount of slides found
int slides(Slide *buffer, Position p);

// apply a Slide to a Position
// presuppones that the slide is legal
Position do_slide(Position p, Slide s);

// bulk perft
// https://www.chessprogramming.org/Perft
uint64_t perft(Position p, int depth);