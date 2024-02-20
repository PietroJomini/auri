/*
- [x] board representation
- [x] moves generation
- [x] zobrist hash
- [x] transposition table

guard macros
- TAKLIB_INCLUDE: include guard
- TAKLIB_IMPLEMENTATION: implementation guard
- TAKLIB_SWAP <n>: amount of plies for which the swap rule apply.
                   setting this to `0` will "disallow" the swap rule, defaults to `2`
- TAKLIB_NOHASH: don't update the hash when `tak_do`ing moves
*/

#ifndef TAKLIB_INCLUDE
#define TAKLIB_INCLUDE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// integers types
typedef uint8_t tak_u8;
typedef uint32_t tak_u32;
typedef uint64_t tak_u64;

// type-safe, no double-evaluation min (requires GCC, i think)
// no return at the end, otherwise compiler will be mad about void types
// TODO: possible speed issues with the added declarations?
#define tak_min(a, b)           \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b;      \
    })

// random state for the xoshiro prng
typedef struct {
    tak_u64 s[4];
} xoshiro_state;

// well, seems like i need a prng
// xoshiro256** should be enough, with a period of 2^256-1
// probably for the zobrist hashing LCG would suffice, as long as i check the values,
// but for monte-carlo i'll need something stronger
// https://en.wikipedia.org/wiki/Xorshift#xoshiro256**
tak_u64 xoshiro(xoshiro_state* state);
tak_u64 xoshiro_rotate(tak_u64 x, int k);

// number of possible rotations of a board, including the "original"
#define TAK_ROTATIONS_AMOUNT 8

// mirror horizontally a index
//  a b c    c b a
//  d e f -> f e d
//  g h i    i h g
tak_u8 tak_mirror_h(uint8_t n, uint8_t s);

// mirror vertically a index
//  a b c    g h i
//  d e f -> d e f
//  g h i    a b c
tak_u8 tak_mirror_v(uint8_t n, uint8_t s);

// mirror a index along the diagonal
//  a b c    a d g
//  d e f -> b e h
//  g h i    c f i
tak_u8 tak_mirror_d(uint8_t n, uint8_t s);

// mirror a index along the antidiagonal
//  a b c    i f c
//  d e f -> h e b
//  g h i    g d a
tak_u8 tak_mirror_a(uint8_t n, uint8_t s);

// rotate a index by 180 degrees
//  a b c    i h g
//  d e f -> f e d
//  g h i    c b a
tak_u8 tak_rotate(uint8_t n, uint8_t s);

// rotate a index by 90 degrees clockwise
//  a b c    g d a
//  d e f -> h e b
//  g h i    i f c
tak_u8 tak_rotate_c(uint8_t n, uint8_t s);

// rotate a index by 90 degrees anticlockwise
//  a b c    c f i
//  d e f -> b e h
//  g h i    a d g
tak_u8 tak_rotate_a(uint8_t n, uint8_t s);

// having both colors be > 0 would make checking by `&` easier,
// but `TAK_WHITE = 0` allows to use them as array indexes, and
// the check can be made by `(... & TAK_BLACK)?`
// `tak_player` and `tak_piece` can be combined flag-like
typedef enum { TAK_WHITE = 0, TAK_BLACK = 1 } tak_player;
typedef enum { TAK_FLAT = 2, TAK_CAP = 4, TAK_WALL = 8 } tak_piece;

// amount of pieces per board size
// { .0  = flats, .1 = caps }
extern const tak_u8 TAK_RESERVES[6][2];

// position representation
typedef struct {
    tak_u8 stp;   // side to play. wheter i make it a `tak_player` or a `tak_u8` i have to
                  // cast it somewhere, so i guess anyway it's fine
    tak_u8 size;  // board size
    tak_u8 ssize;  // squared size (ie. number of squares on the board). maybe having
                   // this here ia a bit overkill, but why not
    tak_u8 mc;     // plies count. could be `u32`, but is a game with more than
                   // 256 moves realistically possible?

    // bitboards
    tak_u64 cbb[2];  // { .0 = white, .1 = black }
    tak_u64 walls;
    tak_u64 caps;

    // zobrist hash
    // TODO: somehow if i place this declaration "lower" in the struct updating
    // the fift element in `tak_zobrist_stack` messes up the walls bitboard (?) (or just
    // segfaults????) wtf? it somehow also seems that having the hash in the struct fixes
    // a perft test??? this plus the -On breaking stuff seems like a unique big problem
    // that needs quite the gcc dive
    tak_u64 hash[TAK_ROTATIONS_AMOUNT];

    // stacks
    // stacks are sorted with the highes piece in the lsb
    tak_u64 stacks[64];
    tak_u8 heights[64];

    // remaining pieces
    // { .0 = white, .i = black } { .0 = flats, .1 = caps }
    tak_u8 reserves[2][2];
} tak_position;

// create a new, empty position
extern inline tak_position tak_newposition(const tak_u8 size);

// if i generate a key for each piece in each position in a stack
// i'd have to generate 64*64 (maybe 64*128 in the future) numbers,
// which sounds overkill.
// but probably i can (safely?) ignore all pieces above a certain
// threshold, to be adjusted in the future.
#define TAK_ZOBRIST_SHT 15

// does u128 improves much?
typedef struct {
    tak_u64 btp;  // black to play;
    // in 64 chunks of STACK_HEIGHT_THRESH elements
    tak_u64 stacks[TAK_ZOBRIST_SHT * 64];
    tak_u64 walls[64];
    tak_u64 caps[64];
} tak_zobrist_data;

// fill needed zobrist data
tak_zobrist_data tak_zd_fill();

// update hashes with btp
void tak_zobrist_btp(tak_position* const p, const tak_zobrist_data* const d);

// update hashes with a stack item
void tak_zobrist_stack(tak_position* const p, const tak_zobrist_data* const d,
                       tak_u8 index, tak_u8 h);

// update hashes with a wall
void tak_zobrist_wall(tak_position* const p, const tak_zobrist_data* const d,
                      tak_u8 index);

// update hashes with a cap
void tak_zobrist_cap(tak_position* const p, const tak_zobrist_data* const d,
                     tak_u8 index);

// hash a position from zero
void tak_zobrist(tak_position* const p, const tak_zobrist_data* const d);

// amount of precomputed spreads in the lookup table
#define TAK_SLT_LENGTH 255

// spreads lookup table
// precomputed spreads configurations
typedef struct {
    tak_u8 indexes[9];  // indexes of the first spread of height `n`
                        // the ninth element, used as an upper bound, is SLT_LENGTH
    tak_u8 lengths[TAK_SLT_LENGTH];    // length in cells of each spread
    tak_u32 spreads[TAK_SLT_LENGTH];   // drops of each spread
    tak_u8 can_crush[TAK_SLT_LENGTH];  // if each spread ends with a single stone drop
                                       // (ie. can crush a wall if the stack is cabbed by
                                       // a capstone)
} tak_slt;

// reduce the `n`th spread starting with a stack of `h` stones
tak_u32 tak_slt_reduce(const int n, const int h);

// fill a new spreads lookup table
tak_slt tak_slt_fill();

// cardinal directions
typedef enum { TAK_NORTH, TAK_EAST, TAK_SOUTH, TAK_WEST } tak_dir;

// get the cardinal opposite of a direction
extern inline tak_dir tak_dir_mirror(const tak_dir d);

// get the next square index in a given direction
// returns -1 if the next square would be off-board
int tak_walk(const tak_u8 origin, const tak_dir d, const tak_u8 size);

// move type
typedef enum { TAK_SPREAD, TAK_PLACEMENT } tak_move_t;

// maximum number of moves in size 8
#define TAK_MAX_MOVES (TAK_MAX_SPREADS + TAK_MAX_PLACEMENTS)
#define TAK_MAX_SPREADS 32704  // https://github.com/PietroJomini/auri/issues/1
#define TAK_MAX_PLACEMENTS \
    192  // 64 * 3. if the swap rule is in place it's actually 62*3, but this way it's
         // more generic

// amount of plies for which the swap rule is applied
#ifdef TAKLIB_SWAP
#define TAK_SWAPN TAKLIB_SWAP
#else
#define TAK_SWAPN 2
#endif

// aggregated move container
// previously this was two separated structs, but while this makes each
// move hold a bit more space, i doubt that it has a speed deficit.
// also, this could be packed in a pretty tight bitfield, and the the values
// would be retrieved via a series of masks, but i doubt the amount of complexity
// it adds would pay off. anyway, maybe try it out?
typedef struct {
    tak_move_t type;  // kind of move
    tak_u8 at;        // square where the piece is placed or
                      // from where the spread originates
    union {
        // placement-specific data
        tak_piece piece;  // placed piece

        // spread-specific data
        // `.length` and `.stacks` could be replaced by an index pointing at the lookup
        // table, but maybe having to bring the lookup table everywere isn't a good idea?
        struct {
            tak_dir dir;      // direction o the spread
            tak_u8 length;    // length of the spread
            tak_u32 stacks;   // drop substacks, with the first located at the lsb nibble
            tak_u8 flattens;  // whether the spread flattens a wall
        };
    };
} tak_move;

// search all legal placements and add them to `buffer`.
// `buffer` should have capacity of at leaADDETTI/E BANCHI SERVITI CASSA/ SCAFFALIst
// `TAK_MAX_PLACEMENTS`. returns the amount of placements found
int tak_search_placements(tak_move* const buffer, const tak_position* const p);

// apply a placement to a position.
// attention: assumes the move to be a legal placement,
// no check sare performed! (we got no time for that shit).
tak_position tak_do_placement(tak_position p, const tak_move pl,
                              const tak_zobrist_data* const zd);

// search all legal spreads starting from a given square
// and oriented in the given direction
int tak_search_spreads_atdir(tak_move* const buffer, const tak_position* const p,
                             const tak_u8 at, const tak_dir d, const tak_slt* const slt);

// search all legal spreads and add them to `buffer`.
// `buffer` should have capacity of at least `TAK_MAX_SPREADS`.
// returns the amount of spreads found
int tak_search_spreads(tak_move* const buffer, const tak_position* const p,
                       const tak_slt* const slt);

// apply a spread to a position.
// attention: assumes the move to be a legal spread,
// no checks are performed!
tak_position tak_do_spread(tak_position p, const tak_move sp,
                           const tak_zobrist_data* const zd);

// search all legal moves and add them to `buffer`
// `buffer` should have capaciy of at least `TAK_MAX_MOVES`.
// returns the amoutnof moves found
int tak_search(tak_move* const buffer, const tak_position* const p,
               const tak_slt* const slt);

// apply a move to a position.
// attention: assumes the move to be a legal spread,
// no checks are performed!
tak_position tak_do(tak_position p, const tak_move m, const tak_zobrist_data* const zd);

// check if the position has a road of the given color
tak_u8 tak_search_road(const tak_position* const p, const tak_player color);
tak_u8 _tak_search_road_r(const tak_position* const p, const tak_player color,
                          const tak_u8 vert, const tak_u8 index, tak_u64* const history);

// end causes
typedef enum { TAK_ROAD, TAK_FLATWIN, TAK_TIE } tak_endcause;
typedef struct {
    tak_u8 ended;
    tak_endcause ending;
    tak_player winner;
} tak_endstatus;

// check if the given position represent an ended game
tak_endstatus tak_check_ending(const tak_position* const p, const int komi);

// bulk perft
// maybe this is too "extra" of a feature to include it here, but it's
// quite handy, so here it is
tak_u64 tak_perft(tak_position p, int depth, const tak_slt* const slt,
                  const tak_zobrist_data* const zd);

// the poorest^tm hashmap ever
// to solve the rotated hashes, each position is stored under the
// "original" hash at index 0, and when searching if a position is already
// stored we need to search each rotated hash
typedef struct {
    // using directly the positions insthead of having a separate hmap_items
    // makes things easier, but negate the option of using separate chaining.
    // i would argue that's fine, since double hasing and open addressing are
    // better anyway
    // maybe it's also a good idea to test having each bucket preallocate
    // a small array (eg. 5 positions) and, when full, override choosing
    // based on some tought parameters (eg. oldest, or less read?) like stockfish
    tak_position** items;  // test also with an array of elements all at 0,
                           // not of pointers. maybe it's slower to create
                           // but faster to insert and search?

    // metrics
    tak_u64 capacity;   // amount of allocated *items
    tak_u64 size;       // n of elements
    tak_u64 overrides;  // n of overridden elements
    tak_u64 jumps;      // n of jumps needed to reach items
} tak_tt;

// create a new transposition table with `capacity` allocated items
tak_tt* tak_tt_new(const tak_u64 capacity);

// free allocated items in a tt
void tak_tt_free(tak_tt* const tt);

// add a position to the tt
void tak_tt_set(tak_tt* const tt, const tak_position p);

// retrieve a position from the tt
// this does not check for all rotations, only the given hash
tak_position* tak_tt_get(const tak_tt* const tt, const tak_u64 hash);

// check if a position is already in the tt, testing all rotations
// returns the position if found, `NULL` if not
tak_position* tak_tt_search(const tak_tt* const tt, const tak_position* const p);

#endif  // TAKLIB_INCLUDE

// implementations
#ifdef TAKLIB_IMPLEMENTATION
// NOLINTBEGIN(misc-definitions-in-headers)

// TODO: should i at least _understand_ what's happening here?
tak_u64 xoshiro_rotate(tak_u64 x, int k) { return (x << k) | (x >> (64 - k)); }
tak_u64 xoshiro(xoshiro_state* state) {
    tak_u64* s = state->s;
    tak_u64 const result = xoshiro_rotate(s[1] * 5, 7);
    tak_u64 const t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = xoshiro_rotate(s[3], 45);

    return result;
}

// is it faster to just check a pre-made lookup table?
// also, check for more bitwise black magic for the non-power of two sizes
// TODO: also 2, maybe inlining all of them?
tak_u8 tak_mirror_h(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return n + ((1 - (n % 3)) << 1);
        case 4: return n ^ 3;  // n + ((2 - (n % 4)) << 1) - 1
        case 5: return n + ((2 - (n % 5)) << 1);
        case 6: return n + ((3 - (n % 6)) << 1) - 1;
        case 7: return n + ((3 - (n % 7)) << 1);
        case 8: return n ^ 7;  // n + ((4 - (n % 8)) << 1) - 1
    }

    return 0;  // should never reach
}

tak_u8 tak_mirror_v(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return n + 6 * (1 - n / 3);
        case 4: return n ^ 12;
        case 5: return n + 10 * (2 - n / 5);
        case 6: return n + 12 * (2 - n / 6) + 6;
        case 7: return n + 14 * (3 - n / 7);
        case 8: return n ^ 56;
    }

    return 0;  // should never reach
}

// i could check if n is in the diagonal, but i guess and to
// assignements just to avoid a plus and a multiplication is not
// really worth
// TODO: chessprogramming gives an absurd looking algorithm
//       for the 8x8, i should understand what it does and if
//       it's usable for the other sizes, or at least for 4x4
//          sq' = ((sq >> 3) | (sq << 3)) & 63;
//          sq' = (sq * 0x20800000) >>> 26;
tak_u8 tak_mirror_d(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return (n % 3) * 3 + n / 3;
        case 4: return (n % 4) * 4 + n / 4;
        case 5: return (n % 5) * 5 + n / 5;
        case 6: return (n % 6) * 6 + n / 6;
        case 7: return (n % 7) * 7 + n / 7;
        case 8: return ((n >> 3) | (n << 3)) & 63;
    }

    return 0;  // should never reach
}

tak_u8 tak_rotate(uint8_t n, uint8_t s) {
    switch (s) {
        case 3: return 8 - n;
        case 4: return 15 ^ n;  // 15 - n
        case 5: return 24 - n;
        case 6: return 35 - n;
        case 7: return 48 - n;
        case 8: return 63 ^ n;  // 63 - n
    }

    return 0;  // should never reach
}

// can be done by both
//  - diag + h + 180 = -90 + 180
//  - antidag + h
// TODO: check which is faster
tak_u8 tak_rotate_c(uint8_t n, uint8_t s) { return tak_rotate(tak_rotate_a(n, s), s); }
tak_u8 tak_rotate_a(uint8_t n, uint8_t s) { return tak_mirror_h(tak_mirror_d(n, s), s); }
tak_u8 tak_mirror_a(uint8_t n, uint8_t s) { return tak_mirror_d(tak_rotate(n, s), s); }

const uint8_t TAK_RESERVES[6][2] = {{10, 0}, {15, 0}, {21, 1}, {30, 1}, {40, 2}, {50, 2}};

inline tak_position tak_newposition(const tak_u8 size) {
    return (tak_position){
        .stp = TAK_WHITE,
        .size = size,
        .ssize = (tak_u8)(size * size),  // TODO: why do i need a cast here? clang wtf!?
        .reserves = {{TAK_RESERVES[size - 3][0], TAK_RESERVES[size - 3][1]},
                     {TAK_RESERVES[size - 3][0], TAK_RESERVES[size - 3][1]}},
    };
}

tak_zobrist_data tak_zd_fill() {
    xoshiro_state state = {.s = {3, 1, 4, 1}};  // TODO: find good values to init this
    tak_zobrist_data d;

    d.btp = xoshiro(&state);
    for (int i = 0; i < TAK_ZOBRIST_SHT * 64; i++) d.stacks[i] = xoshiro(&state);
    for (int i = 0; i < 64; i++) {
        d.walls[i] = xoshiro(&state);
        d.caps[i] = xoshiro(&state);
    }

    return d;
}

void tak_zobrist_btp(tak_position* const p, const tak_zobrist_data* const d) {
    p->hash[0] ^= d->btp;
    p->hash[1] ^= d->btp;
    p->hash[2] ^= d->btp;
    p->hash[3] ^= d->btp;
    p->hash[4] ^= d->btp;
    p->hash[5] ^= d->btp;
    p->hash[6] ^= d->btp;
    p->hash[7] ^= d->btp;
}

void tak_zobrist_stack(tak_position* const p, const tak_zobrist_data* const d,
                       tak_u8 index, tak_u8 h) {
    p->hash[0] ^= d->stacks[h + index * TAK_ZOBRIST_SHT];
    p->hash[1] ^= d->stacks[h + tak_mirror_h(index, p->size) * TAK_ZOBRIST_SHT];
    p->hash[2] ^= d->stacks[h + tak_mirror_v(index, p->size) * TAK_ZOBRIST_SHT];
    p->hash[3] ^= d->stacks[h + tak_mirror_d(index, p->size) * TAK_ZOBRIST_SHT];
    p->hash[4] ^= d->stacks[h + tak_mirror_a(index, p->size) * TAK_ZOBRIST_SHT];
    p->hash[5] ^= d->stacks[h + tak_rotate(index, p->size) * TAK_ZOBRIST_SHT];
    p->hash[6] ^= d->stacks[h + tak_rotate_c(index, p->size) * TAK_ZOBRIST_SHT];
    p->hash[7] ^= d->stacks[h + tak_rotate_a(index, p->size) * TAK_ZOBRIST_SHT];
}

void tak_zobrist_wall(tak_position* const p, const tak_zobrist_data* const d,
                      tak_u8 index) {
    p->hash[0] ^= d->walls[index];
    p->hash[1] ^= d->walls[tak_mirror_h(index, p->size)];
    p->hash[2] ^= d->walls[tak_mirror_v(index, p->size)];
    p->hash[3] ^= d->walls[tak_mirror_d(index, p->size)];
    p->hash[4] ^= d->walls[tak_mirror_a(index, p->size)];
    p->hash[5] ^= d->walls[tak_rotate(index, p->size)];
    p->hash[6] ^= d->walls[tak_rotate_c(index, p->size)];
    p->hash[7] ^= d->walls[tak_rotate_a(index, p->size)];
}

void tak_zobrist_cap(tak_position* const p, const tak_zobrist_data* const d,
                     tak_u8 index) {
    p->hash[0] ^= d->caps[index];
    p->hash[1] ^= d->caps[tak_mirror_h(index, p->size)];
    p->hash[2] ^= d->caps[tak_mirror_v(index, p->size)];
    p->hash[3] ^= d->caps[tak_mirror_d(index, p->size)];
    p->hash[4] ^= d->caps[tak_mirror_a(index, p->size)];
    p->hash[5] ^= d->caps[tak_rotate(index, p->size)];
    p->hash[6] ^= d->caps[tak_rotate_c(index, p->size)];
    p->hash[7] ^= d->caps[tak_rotate_a(index, p->size)];
}

void tak_zobrist(tak_position* const p, const tak_zobrist_data* const d) {
    for (int i = 0; i < TAK_ROTATIONS_AMOUNT; i++) p->hash[i] = 0;

    // btp
    if (p->stp == 1) tak_zobrist_btp(p, d);

    tak_u64 sq = 1;
    for (int i = 0; i < p->size * p->size; i++) {
        // stacks
        for (int h = 0; h < TAK_ZOBRIST_SHT && h < p->heights[i]; h++)
            tak_zobrist_stack(p, d, i, h);

        // walls, caps
        if (p->walls & sq) tak_zobrist_wall(p, d, i);
        if (p->caps & sq) tak_zobrist_cap(p, d, i);
        sq <<= 1;
    }
}

tak_u32 tak_slt_reduce(const int n, const int h) {
    tak_u32 stacks = 0;
    tak_u8 stack = 1;

    for (int i = 0; i < h - 1; i++) {
        if (n & 1 << i) {
            stacks = stacks << 4 | stack;
            stack = 1;
        } else stack += 1;
    }

    return stacks << 4 | stack;
}

tak_slt tak_slt_fill() {
    tak_slt lt;
    int k = 0;

    for (int h = 1; h <= 8; h++) {
        lt.indexes[h - 1] = k;

        // reduce and add each spread with the given stack height
        for (int i = 0; i < 1 << (h - 1); i++) {
            lt.spreads[k] = tak_slt_reduce(i, h);
            lt.can_crush[k] = (lt.spreads[k] & 0xf) == 1;
            lt.lengths[k++] = 1 + __builtin_popcount(i);
        }
    }

    // the ninth element serves as a fixed global upper bound
    // and to avoid overflows (or an if) in search_slides_atdir
    lt.indexes[8] = TAK_SLT_LENGTH;

    return lt;
}

tak_dir tak_dir_mirror(const tak_dir d) {
    return d == TAK_EAST    ? TAK_WEST
           : d == TAK_SOUTH ? TAK_NORTH
           : d == TAK_WEST  ? TAK_EAST
                            : TAK_SOUTH;
}

// TODO: experiment with pre-loaded walk results as early
//       takes showed some improvement in performance
int tak_walk(const tak_u8 origin, const tak_dir d, const tak_u8 size) {
    if (d == TAK_NORTH) return (origin >= size * (size - 1)) ? -1 : origin + size;
    else if (d == TAK_EAST) return ((origin + 1) % size == 0) ? -1 : origin + 1;
    else if (d == TAK_SOUTH) return (origin < size) ? -1 : origin - size;
    else if (d == TAK_WEST) return (origin % size == 0) ? -1 : origin - 1;
    else return -1;
}

int tak_search_placements(tak_move* const buffer, const tak_position* const p) {
    int k = 0;
    tak_u8 stp = p->mc < TAK_SWAPN ? 1 - p->stp : p->stp;

    for (int i = 0; i < p->ssize; i++) {
        if (p->heights[i] == 0) {
            // flats
            if (p->reserves[stp][0])
                buffer[k++] = (tak_move){.type = TAK_PLACEMENT,
                                         .at = (tak_u8)i,
                                         .piece = (tak_piece)(stp | TAK_FLAT)};

            if (p->mc >= TAK_SWAPN) {
                // walls. can be placed only when not in swap range
                if (p->reserves[stp][0])
                    buffer[k++] = (tak_move){.type = TAK_PLACEMENT,
                                             .at = (tak_u8)i,
                                             .piece = (tak_piece)(stp | TAK_WALL)};

                // caps. can be placed only when not in swap range
                if (p->reserves[stp][1])
                    buffer[k++] = (tak_move){.type = TAK_PLACEMENT,
                                             .at = (tak_u8)i,
                                             .piece = (tak_piece)(stp | TAK_CAP)};
            }
        }
    }

    return k;
}

tak_position tak_do_placement(tak_position p, const tak_move pl,
                              const tak_zobrist_data* const zd) {
    tak_player color = (tak_player)(pl.piece & 1);

    // stack and height
    p.stacks[pl.at] = color;
    p.heights[pl.at] = 1;

    // color bitmaps
    tak_u64 sq = 1ull << pl.at;
    p.cbb[color] |= sq;

    // modifiers bitmap
    if (pl.piece & TAK_WALL) p.walls |= sq;
    if (pl.piece & TAK_CAP) p.caps |= sq;

    // reserves, mc, stp
    p.reserves[color][pl.piece & TAK_CAP ? 1 : 0] -= 1;
    p.stp = 1 - p.stp;
    p.mc += 1;

#ifndef TAKLIB_NOHASH
    // hash
    tak_zobrist_btp(&p, zd);
    tak_zobrist_stack(&p, zd, pl.at, 0);
#endif

    return p;
}

int tak_search_spreads_atdir(tak_move* const buffer, const tak_position* const p,
                             const tak_u8 at, const tak_dir d, const tak_slt* const slt) {
    // find the amount of free cells available for the slide
    int length = 0, last = tak_walk(at, d, p->size);
    tak_u64 mask = ~(p->caps | p->walls);
    while (last != -1 && mask & 1ull << last) {
        last = tak_walk(last, d, p->size);
        length += 1;
    }

    // height of the legally movable (sub)stack
    int h = tak_min(p->heights[at], p->size);

    // check if crushing is a possibility
    int can_crush = last != -1 && p->caps & 1ull << at && p->walls & 1ull << last;

    // fiter available spread configurations
    int k = 0, flattens;
    for (int index = 0; index < slt->indexes[h]; index++) {
        // this is ugly, but at least it doesn't check the flattening
        // conditions every iteration
        if ((flattens = slt->lengths[index] <= length) ||
            (can_crush && slt->lengths[index] == length + 1 && slt->can_crush[index]))
            buffer[k++] = (tak_move){
                .type = TAK_SPREAD,
                .at = at,
                .dir = d,
                .length = slt->lengths[index],
                .stacks = slt->spreads[index],
                .flattens = (tak_u8)(1 - flattens),
            };
    }

    return k;
}

int tak_search_spreads(tak_move* const buffer, const tak_position* const p,
                       const tak_slt* const slt) {
    // this should be in `tak_search_spreads_atdir`, but it it's faster here
    // TODO: also, i can #ifdef this (and a couple checks in `tak_search_placements`),
    //       but i don't know if it's worth
    if (p->mc < TAK_SWAPN) return 0;

    int k = 0;
    for (int i = 0; i < p->ssize; i++) {
        // checking in the color bitboard ensures also the heights
        // of the stacks to be greater than 0
        if (p->cbb[p->stp] & 1ull << i) {
            k += tak_search_spreads_atdir(buffer + k, p, i, TAK_NORTH, slt);
            k += tak_search_spreads_atdir(buffer + k, p, i, TAK_EAST, slt);
            k += tak_search_spreads_atdir(buffer + k, p, i, TAK_SOUTH, slt);
            k += tak_search_spreads_atdir(buffer + k, p, i, TAK_WEST, slt);
        }
    }

    return k;
}

tak_position tak_do_spread(tak_position p, const tak_move sp,
                           const tak_zobrist_data* const zd) {
    // last cell of the slide
    int target = sp.at + sp.length * (sp.dir == TAK_EAST    ? 1
                                      : sp.dir == TAK_SOUTH ? -p.size
                                      : sp.dir == TAK_WEST  ? -1
                                                            : p.size);

    // modifiers
    // TODO: it seems that when optimizing with -On this breaks? wtf =(
    tak_u64 os = 1ull << sp.at, ts = 1ull << target, nos = ~os;
    if (p.caps & os) p.caps = (p.caps & nos) | ts;
    if (p.walls & os) p.walls = (p.walls & nos) | ts;
    if (sp.flattens) p.walls &= ~ts;

    int i = 0, color, ch = 0;
    tak_dir od = tak_dir_mirror(sp.dir);
    while (target != sp.at) {
        // get the height of the current drop
        int h = sp.stacks >> 4 * (sp.length - i - 1) & 0xf;
        ch += h;

#ifndef TAKLIB_NOHASH
        // update hash (for the added elements on the target)
        for (int i = p.heights[target]; i < p.heights[target] + h; i++)
            tak_zobrist_stack(&p, zd, target, i);
#endif

        // update target
        // TODO: here on 7x7 and 8x8 it can overflow, check and avoid.
        // TODO: also, test if using __int128 is realistically good?
        p.stacks[target] <<= h;
        p.stacks[target] |= p.stacks[sp.at] & ((1ull << h) - 1);
        p.heights[target] += h;

        // update origin
        p.stacks[sp.at] >>= h;
        p.heights[sp.at] -= h;

        // update target color
        ts = 1ull << target;
        color = p.stacks[target] & 1;
        p.cbb[color] |= ts;
        p.cbb[1 - color] &= ~ts;

        // walk target
        target = tak_walk(target, od, p.size);
        i++;
    }

#ifndef TAKLIB_NOHASH
    // update hash (for the removed elements on the origin)
    for (int i = p.heights[sp.at] + ch; i >= p.heights[sp.at]; i--)
        tak_zobrist_stack(&p, zd, sp.at, i);
    tak_zobrist_btp(&p, zd);
#endif

    // update origin color
    if (p.heights[sp.at] == 0) {
        p.cbb[0] &= nos;
        p.cbb[1] &= nos;
    } else {
        color = p.stacks[sp.at] & 1;
        p.cbb[color] |= os;
        p.cbb[1 - color] &= nos;
    }

    // mc, stp
    p.stp = 1 - p.stp;
    p.mc += 1;

    return p;
}

int tak_search(tak_move* const buffer, const tak_position* const p,
               const tak_slt* const slt) {
    int a = tak_search_placements(buffer, p);
    return a + tak_search_spreads(buffer + a, p, slt);
}

tak_position tak_do(tak_position p, const tak_move m, const tak_zobrist_data* const zd) {
    switch (m.type) {
        case TAK_PLACEMENT: return tak_do_placement(p, m, zd);
        case TAK_SPREAD: return tak_do_spread(p, m, zd);
    }

    // should never reach
    return p;
}

tak_u8 tak_search_road(const tak_position* const p, const tak_player color) {
    tak_u64 mask = p->cbb[color] & ~p->walls, history;

    // vertical roads
    history = 0;
    for (int i = 0; i < p->size; i++)
        if (mask & 1ull << i && _tak_search_road_r(p, color, 1, i, &history)) return 1;

    // horizontal roads
    history = 0;
    for (int i = 0, ai = 0; i < p->size; i++, ai = i * p->size)
        if (mask & 1ull << ai && _tak_search_road_r(p, color, 0, ai, &history)) return 1;

    return 0;
}

// this is where the main road-searching-stuff happens,
// basically a simple depth-first search.
// `vert` should be `1` if the road is supposed to be vertical, `0` if horizontal
tak_u8 _tak_search_road_r(const tak_position* const p, const tak_player color,
                          const tak_u8 vert, const tak_u8 index, tak_u64* const history) {
    // check and set history
    if (*history & 1ull << index) return 0;
    *history |= 1ull << index;

    // check if the current index links a road
    if ((vert && index / p->size == p->size - 1) ||
        (!vert && index % p->size == p->size - 1))
        return 1;

    // spread neighbours
    int neighbour;
    tak_u64 mask = p->cbb[color] & ~p->walls;
    for (int d = TAK_NORTH; d <= TAK_WEST; d++) {
        neighbour = tak_walk(index, (tak_dir)d, p->size);
        if (neighbour != -1 && mask & 1ull << neighbour &&
            _tak_search_road_r(p, color, vert, neighbour, history))
            return 1;
    }

    return 0;
}

tak_endstatus tak_check_ending(const tak_position* const p, const int komi) {
    // roads
    // if a road was linked in the last move it should have been played
    // by the opponent of the current stp, so we look for him first to
    // handle dragon clause
    if (tak_search_road(p, (tak_player)(1 - p->stp)))
        return (tak_endstatus){
            .ended = 1,
            .ending = TAK_ROAD,
            .winner = (tak_player)(1 - p->stp),
        };
    if (tak_search_road(p, (tak_player)(p->stp)))
        return (tak_endstatus){
            .ended = 1,
            .ending = TAK_ROAD,
            .winner = (tak_player)(p->stp),
        };

    // reserves or filled board
    if ((p->reserves[0][0] == 0 && p->reserves[0][1] == 0) ||
        (p->reserves[1][0] == 0 && p->reserves[1][1] == 0) ||
        // should i have a series of constants for full boards?
        (p->cbb[0] | p->cbb[1]) == (1ull << p->ssize) - 1) {
        tak_u64 mask = ~p->walls;
        int popc_w = __builtin_popcount(p->cbb[0] & mask);
        int popc_b = __builtin_popcount(p->cbb[1] & mask) + komi;
        if (popc_b == popc_w) return (tak_endstatus){.ended = 1, .ending = TAK_TIE};
        return (tak_endstatus){
            .ended = 1,
            .ending = TAK_FLATWIN,
            .winner = popc_w > popc_b ? TAK_WHITE : TAK_BLACK,
        };
    }

    return (tak_endstatus){.ended = 0};
}

tak_u64 tak_perft(tak_position p, int depth, const tak_slt* const slt,
                  const tak_zobrist_data* const zd) {
    // game ended (or somehow reached depth 0)
    if (depth <= 0 || tak_check_ending(&p, 0).ended) return 1;
    tak_u64 nodes = 0;

    // load moves
    tak_move buffer[TAK_MAX_MOVES];
    int n = tak_search(buffer, &p, slt);

    if (depth == 1) return n;
    for (int i = 0; i < n; i++)
        nodes += tak_perft(tak_do(p, buffer[i], zd), depth - 1, slt, zd);
    return nodes;
}

tak_tt* tak_tt_new(const tak_u64 capacity) {
    tak_tt* tt = (tak_tt*)malloc(sizeof(tak_tt));
    tt->items = (tak_position**)calloc(capacity, sizeof(tak_position*));
    tt->capacity = capacity;
    return tt;
}

// free allocated items in a tt
void tak_tt_free(tak_tt* const tt) {
    for (tak_u64 i = 0; i < tt->capacity; i++)
        if (tt->items[i]) free(tt->items[i]);
    free(tt->items);
    free(tt);
}

// add a position to the tt
// NOTE: this doesn't check if the position is already in the tt, meaning that
// there can be the possibility for duplicated. TODO: should i prevent this? maybe run
// some metric test
void tak_tt_set(tak_tt* const tt, const tak_position p) {
    tak_u64 index = p.hash[0] % tt->capacity;
    tak_position* address = tt->items[index];

    // maybe i can also test starting to override
    // at certain fill rate, like 80%
    tak_u8 override = tt->size == tt->capacity;

    // if the "original" address is already filled,
    // find the right address with linear probing
    tak_u64 probe = 0;
    while (address != NULL && !override && probe < tt->capacity) {
        index = (index + probe) % tt->capacity;
        address = tt->items[index];

        // maybe in the future test also double hasing and
        // other probing strategies.
        probe += 1;
    }

    // it would be faster to just pass the pointer and save
    // the original one, but if it's get modifier (unlikely, with how
    // my do_move works, but still) the key mapping would broke.
    // if i see that in implementations this would be unlikely, i can
    // still change this
    tak_position* i = (tak_position*)malloc(sizeof(tak_position));
    memcpy(i, &p, sizeof(tak_position));
    tt->items[index] = i;

    // update metrics
    tt->jumps += probe;
    if (override || probe == tt->capacity) tt->overrides++;
    else tt->size++;
}

// retrieve a position from the tt
// this does not check for all rotations, only the given hash
tak_position* tak_tt_get(const tak_tt* const tt, const tak_u64 hash) {
    tak_u64 index = hash % tt->capacity;
    tak_position* address = tt->items[index];

    // find the right address
    tak_u64 probe = 0;
    while ((address == NULL || address->hash[0] != hash) && probe < tt->capacity) {
        index = (index + probe) % tt->capacity;
        address = tt->items[index];
        probe += 1;
    }

    // doing a "full circle" around the table means no matching position found
    return (probe < tt->capacity) ? address : NULL;
}

// check if a position is already in the tt
// returns the position if found, `NULL` if not
tak_position* tak_tt_search(const tak_tt* const tt, const tak_position* const p) {
    tak_position* s;

    // this way if there are more than one "rotated" board in the
    // tt this will only spot the forst one, but the whole scenario
    // shouldn't be possible if before setting a position we check if
    // it's already there
    // also, this way when the tt is full we run a lot more probing cycles
    // than necessary, maybe find a way to optimize? (buckets and no probing?)
    // also, having a single probing cicle with all the comparison in one place should
    // fasten things with a ~full tt, maybe?
    for (int i = 0; i < TAK_ROTATIONS_AMOUNT; i++) {
        s = tak_tt_get(tt, p->hash[i]);
        if (s != NULL) return s;
    }

    return NULL;
}

// NOLINTEND(misc-definitions-in-headers)
#endif  // TAKLIB_IMPLEMENTATION