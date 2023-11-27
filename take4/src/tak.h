#pragma once

#include <stdint.h>

// dunno, maybe this is not necessary?
#define u8 uint8_t
#define u32 uint32_t
#define u64 uint64_t

// having both colors be > 0 would make checking by `&` easier,
// but `WHITE = 0` allows to use them as array indexes, and
// the check could be made by `(... & TAK_BLACK)?`. this way there
// is no "null" color in a player-piece union, but this would have
// littler use cases than the array stuff.
typedef enum { WHITE = 0, BLACK = 1 } player;
typedef enum { FLAT = 2, CAP = 4, WALL = 8 } piece;

// amount of moves that fall under the swap rule
#define SWAP_MOVES 2

// amount of pieces per board size
// { .0  = flats, .1 = caps }
extern const u8 RESERVES[6][2];

// base position representation
typedef struct {
    player stp;  // side to play
    u8 size;     // board size
    u8 mc;       // played moves count

    // bitboards
    u64 cbb[2];  // { .0 = white, .1 = black }
    u64 walls;
    u64 caps;

    // stacks
    // stacks are sorted with the highes piece in the lsb
    u64 stacks[64];
    u8 heights[64];

    // remaining pieces
    // { .0 = white, .i = black } { .0 = flats, .1 = caps }
    u8 reserves[2][2];
} position;

// create a new, empty position
position new_position(int size);

// amount of precomputed slides in the lookup table
#define SLT_LENGTH 255

// slides lookup table
// precoputed slides configurations
typedef struct {
    u8 indexes[9];             // indexes of the first slide of height `n`
                               // the ninth element, used as an upper bound, is SLT_LENGTH
    u8 lengths[SLT_LENGTH];    // length in cells of each slide
    u32 slides[SLT_LENGTH];    // drops of each slide
    u8 can_crush[SLT_LENGTH];  // if the slides ends with a single stone drop
} slides_lt;

// reduce the nth slide with h stones
u32 slt_reduce(int n, int h);

// fill the lookup table
slides_lt slt_fill();

// directions
typedef enum { NORTH, EAST, SOUTH, WEST } direction;

// the cardinal opposite of a direction
direction direction_o(direction d);

// get the next square index in a given direction
// returns -1 if the next square would be off-board
int walk(u8 origin, direction d, u8 size);

// base slide representation
typedef struct {
    direction direction;
    u8 origin;
    u8 length;
    u32 stacks; // the first drop is located in the lsb nibble
    u8 flattens;
} slide;

// TODO: max number of slides?
// #define SLIDES_MAX_AMOUNT

// search all possible slides in the given position,
// starting from a given square and sliding in the given direction.
// returns the amount of slides found
int search_slides_atdir(slide *buffer, position const *p, u8 origin, direction d,
                        slides_lt const *slt);

// search all possible slides in the given position.
// returns the amount of slides found
int search_slides(slide *buffer, position const *p, slides_lt const *slt);

// apply a slide to a position
// assumes the slide to be legal
position do_slide(position p, slide s);

// base placement representation
typedef struct {
    piece piece;
    u8 at;
} placement;

// maximum amount of placement in one position
// 64 * 3
#define PLACEMENTS_MAX_AMOUNT 192

// get all possible placements in the given position
// returns the amount of placements found
int search_placements(placement *buffer, position const *p);

// apply a placement to a position
// assumes the placement to be legal
position do_placement(position p, placement pl);

// bulk perft
uint64_t perft(position p, int depth, slides_lt const *slt);

// end causes
typedef enum { ROAD, FLATWIN, TIE } endcause;
typedef struct {
    u8 ended;
    endcause ending;
    player winner;
} endstatus;

// check for roads
u8 search_road(position const *p);
u8 _search_road_r(position const *p, uint8_t origin, uint8_t i, uint64_t *history);

// check for endings
endstatus check_ending(position const *p, int komi);

// utility moves union
typedef enum { SLIDE, PLACEMENT } move_t;
typedef struct {
    move_t t;
    union {
        placement p;
        slide s;
    };
} move;

// move-aggregated functions
// NOTE: this is usually slower than using the dedicated functions,
//       so when performance is involved use them.
// maybe we can opt to the performance problem by having the buffer just be
// a *union, so we can assign the `placement`s and `slide`s individually, and then
// also have a buffer for the type of the move?
int search_moves(move *buffer, position const *p, slides_lt const *slt);
position do_move(position p, move mv);