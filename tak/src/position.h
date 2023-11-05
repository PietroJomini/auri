#pragma once

#include <stdint.h>

// having both colors be > 0 would make checking by & easier,
// but TAL_WHITE = 0 allows to use them as array indexes, and
// the check could be made by (... & TAK_BLACK)?. this way there
// is no "null" color in a player-piece union, but this would have
// littler use cases than the array stuff.
typedef enum { TAK_WHITE = 0, TAK_BLACK = 1 } tak_player;
typedef enum { TAK_FLAT = 2, TAK_CAP = 4, TAK_WALL = 8 } tak_piece;

// amount of pieces per board size
extern const uint8_t TAK_RESERVES[6][2];

typedef struct {
    uint8_t size;    // board size
    tak_player stp;  // side to play
    uint8_t mc;      // played moves count

    // bitboards
    uint64_t cbb[2];  // { .0 = white, .1 = black }
    uint64_t walls;
    uint64_t caps;

    // stacks
    // stacks are sorted with the highes piece in the lsb
    uint64_t stacks[64];
    uint8_t heights[64];

    // remaining pieces
    // { .0 = white, .i = black } { .0 = flats, .1 = caps }
    uint8_t reserves[2][2];
} tak_position;

// create a new, empty position
static inline tak_position tak_new_position(uint8_t size) {
    return (tak_position){
        .size = size,
        .stp = TAK_WHITE,
        .reserves = {{TAK_RESERVES[size - 3][0], TAK_RESERVES[size - 3][1]},
                     {TAK_RESERVES[size - 3][0], TAK_RESERVES[size - 3][1]}},
    };
}

// end causes
typedef enum { TAK_ROAD, TAK_FLATWIN, TAK_TIE } tak_endcause;
typedef struct {
    uint8_t ended;
    tak_endcause ending;
    tak_player winner;
} tak_endstatus;

// check for roads
uint8_t tak_search_road(tak_position const *p);
uint8_t _tak_search_road_r(tak_position const *p, uint8_t origin, uint8_t i,
                           uint64_t *history);

// check for endings
tak_endstatus tak_check_ending(tak_position const *p);