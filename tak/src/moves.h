#pragma once

#include <stdint.h>

#include "position.h"

// amount of moves for which the swap rule apply
#define TAK_SWAP_MOVES 2

typedef struct {
    tak_piece piece;
    uint8_t at;
} tak_placement;

// get all possible placements in the given position
// returns the amount of placements found
int tak_search_placements(tak_placement *buffer, tak_position const *p);

// apply a placement to a position
// assumes the placement to be legal
tak_position tak_do_placement(tak_position p, tak_placement pl);

// amount of precomputed slides in the lookup table
#define TAK_SLIDESLT_AMOUNT 501

typedef struct {
    uint8_t indexes[8];                    // indexes of the first slide of height n
    uint8_t lengths[TAK_SLIDESLT_AMOUNT];  // length of each slide
    uint32_t slides[TAK_SLIDESLT_AMOUNT];  // drop stacks of each slide
} tak_slideslt;

// reduce the nth slide with h stones
uint32_t tak_slt_reduce(int n, int h);

// load the lookup table for the slides
// TODO: more details in the documentation
tak_slideslt tak_slt_load();

// direction
typedef enum { TAK_NORTH, TAK_EAST, TAK_SOUTH, TAK_WEST } tak_direction;

static inline tak_direction tak_opposite_d(tak_direction d) {
    return d == TAK_EAST    ? TAK_WEST
           : d == TAK_SOUTH ? TAK_NORTH
           : d == TAK_WEST  ? TAK_EAST
                            : TAK_SOUTH;
}

typedef struct {
    tak_direction direction;
    uint8_t origin;
    uint8_t length;
    uint32_t stacks;
    uint8_t flattens;
} tak_slide;

// get the next square in a given direction
// returns -1 if the next square would be off-board
int tak_walk(uint8_t origin, tak_direction d, uint8_t size);

// compute all possible slides in the given position,
// starting from a given square and sliding in the given direction.
// returns the amount of slides found
int tak_search_slides_atdir(tak_slide *buffer, tak_position const *p, uint8_t origin,
                            tak_direction d, tak_slideslt const *slt);

// compute all possible slides in the given position, starting from a given square.
// returns the amount of slides found
int tak_search_slides_at(tak_slide *buffer, tak_position const *p, uint8_t origin,
                         tak_slideslt const *slt);

// compute all possible slides in the given position.
// returns the amount of slides found
int tak_search_slides(tak_slide *buffer, tak_position const *p, tak_slideslt const *slt);

// apply a slide to a position
// assumes the slide to be legal
tak_position tak_do_slide(tak_position p, tak_slide s);

// bulk perft
uint64_t tak_perft(tak_position p, int depth, tak_slideslt const *slt);

// unified moves
// NOTE: this should be slower, so it's better to use it only for notation
// and high-level APIs
typedef enum { TAK_SLIDE, TAK_PLACEMENT } tak_movet;

typedef union {
    tak_placement p;
    tak_slide s;
} tak_moveu;

typedef struct {
    tak_movet t;
    tak_moveu u;
} tak_move;