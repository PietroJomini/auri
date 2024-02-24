/*
this uses the same implementation guard as `tak.h`, so:
- TAKLIB_INCLUDE_PTN: include guard
- TAKLIB_IMPLEMENTATION: implementation guard
*/

#ifndef TAKLIB_INCLUDE_PTN
#define TAKLIB_INCLUDE_PTN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TAKLIB_IMPLEMENTATION
#undef TAKLIB_IMPLEMENTATION
#include "tak.h"
#define TAKLIB_IMPLEMENTATION
#else
#include "tak.h"
#endif

// on 8x8, given the standard 50 stones each, we can arrange the
// pieces as 60 walls and 4 caps, with the remaining 40 stones flat unde
// the stacks. probably this position is not reachable, but it should be
// a sane upper limit
//  7 row separators
//  56 cell separators
//  60 wall modifiers
//  4 cap modifiers
//  104 stone color flags
//  1 std
//  3 mc - could bu hihger, but up to 999 should be pretty reasonable
//  2 spaces
// => 237 + \0
#define TPS_MAX_LENGTH 238

// tps lexicon
// specifies all the token needed to parse and encode a tps string
typedef struct {
    char row;      // separate two rows
    char cell;     // separate two cells
    char section;  // separate two sections
    char wall;     // wall modifier
    char cap;      // cap modifier
    char jump;     // empty cell
    char white;    // player 1
    char black;    // player 2
} tps_lexicon;

// standard commonly used tps lexicon
extern const tps_lexicon TPS_STD;

// get the board size from a tps string
int tps_getsize(const char *tps, const tps_lexicon lexicon);

// since tps start describing rows from the highest,
// to correctly index the cell we have to mirror it vertically
// TODO: how should i prefix this function? i don't want to put it in `tak.h`,
// but it's functionality is of higher order than just as a `tps_*` namespace...
int tak_mirror_index(const int index, const int size);

// parse tps string into new position
// TODO: refactor to allow passing `const char* const buffer`
tak_position tps_parse(char *tps, const tps_lexicon lexicon,
                       const tak_zobrist_data *const zd);

// encode a position as tps and load the result in `buffer`.
// buffer is expected to have capacity >= TPS_MAX_LENGTH.
// returns the length of the encoded string
int tps_encode(char *const buffer, const tak_position p, const tps_lexicon lexicon);

// ptn lexicon
// specifies all the token needed to parse and encode a ptn string
typedef struct {
    char wall;           // wall modifier
    char cap;            // cap modifier
    char columns[8];     // column names
    char rows[8];        // row names
    char directions[4];  // direction indicators
    char flattens;       // flattening slide indicator
} ptn_lexicon;

// the longest ptn slide is 7a1>1111111*
#define PTN_MAX_MOVE_LENGTH 13

// parse a ptn string into a move
// the input is expected to be correct
// TODO: I should modify it to allow passing a const string
tak_move ptn_parse(char *ptn, const tak_u8 size, const tak_player stp,
                   const ptn_lexicon lexicon);

// convert a move to ptn representation
// return the length of the string
int ptn_encode(char *const buffer, const tak_move m, const tak_u8 size,
               const ptn_lexicon lexicon);

// apply a list of ptn moves to a position
tak_position ptn_apply(tak_position p, char **const ptn, const int n,
                       const ptn_lexicon lexicon, const tak_zobrist_data *const zd);

// standard commonly used lexicon
extern const ptn_lexicon PTN_STD;

#endif  // TAKLIB_INCLUDE_PTN

// implementations
#ifdef TAKLIB_IMPLEMENTATION
// NOLINTBEGIN(misc-definitions-in-headers)

const tps_lexicon TPS_STD = {
    .row = '/',
    .cell = ',',
    .section = ' ',
    .wall = 'S',
    .cap = 'C',
    .jump = 'x',
    .white = '1',
    .black = '2',
};

int tps_getsize(const char *tps, const tps_lexicon lexicon) {
    int size = 1;
    while (*tps != lexicon.section) size += lexicon.row == *tps++;
    return size;
}

// TODO: replace with a tak.h rotation
int tak_mirror_index(const int index, const int size) {
    return index % size + (size - index / size - 1) * size;
}

// TODO: benchmark against a strtok_r version
tak_position tps_parse(char *tps, const tps_lexicon lexicon,
                       const tak_zobrist_data *const zd) {
    tak_position p = tak_newposition(tps_getsize(tps, lexicon));

    // main section
    tak_player color = TAK_WHITE;
    int index = 0, mindex = tak_mirror_index(0, p.size), jumping = 0;
    tak_u64 square = 0;

    for (; *tps != lexicon.section; tps += sizeof(char)) {
        // handle empty cell
        if (*tps == lexicon.jump) jumping = 1;

        // handle multiple empty cells
        else if (jumping && *tps >= '0' && *tps <= '9') jumping = *tps - '0';

        // handle color indicator (hence stone)
        // we don't handle previous jump here, since there shouldn't be the need to
        // as after a jump indicator there should be only a number (handled above)
        // or a separator
        else if (*tps == lexicon.white || *tps == lexicon.black) {
            color = *tps == lexicon.white ? TAK_WHITE : TAK_BLACK;
            mindex = tak_mirror_index(index, p.size);

            // update stacks
            p.stacks[mindex] = (p.stacks[mindex] << 1) | color;
            p.heights[mindex] += 1;

            // update color bitboards
            square = 1ull << mindex;
            p.cbb[color] |= square;
            p.cbb[1 - color] &= ~square;

            // update flats count
            p.reserves[color][0] -= 1;
        }

        // with separators and modifiers we need to handle
        // unhandled jumps as well
        else {
            // resolve jumps
            if (jumping) {
                index += jumping - 1;
                jumping = 0;
            }

            // resolve modifiers
            // here the mindex should already be set, since all modifiers
            // should be after a color indicator
            if (*tps == lexicon.wall) p.walls |= 1ull << mindex;
            else if (*tps == lexicon.cap) {
                p.caps |= 1ull << mindex;

                // update stones count
                p.reserves[color][1] -= 1;
                p.reserves[color][0] += 1;
            }

            // update index when we find a separator
            else if (*tps == lexicon.cell || *tps == lexicon.row)
                index += 1;
        }
    }

    // turn identifier
    while (*tps == lexicon.section) tps++;
    p.stp = strtol(tps, &tps, 10) - 1;

    // size to play section
    while (*tps == lexicon.section) tps++;
    p.mc = strtol(tps, &tps, 10) - 1;
    p.mc = p.mc * 2 + p.stp;

    // hash
    tak_zobrist(&p, zd);

    return p;
}

int tps_encode(char *const buffer, const tak_position p, const tps_lexicon lexicon) {
    int k = 0;

    // main section
    int jumping = 0;

    for (int row = p.size - 1; row >= 0; row--) {
        for (int column = 0; column < p.size; column++) {
            int index = row * p.size + column;

            if (p.heights[index] == 0) jumping += 1;
            else {
                // handle previous jumps
                if (jumping) {
                    // add cell separator if needed
                    if (k > 0 && buffer[k - 1] != lexicon.row) {
                        buffer[k++] = lexicon.cell;
                    }

                    buffer[k++] = lexicon.jump;

                    // add jumps count if needed
                    if (jumping > 1) buffer[k++] = '0' + jumping;
                    jumping = 0;
                }

                // cell separator
                if (column > 0) buffer[k++] = lexicon.cell;

                // append stack
                uint64_t sq;  // this could be a int, but later it's used as a uint64
                for (int h = p.heights[index] - 1; h >= 0; h--) {
                    sq = p.stacks[index] & 1ull << h;
                    buffer[k++] = sq ? lexicon.black : lexicon.white;
                }

                // modifiers
                sq = 1ull << index;
                if (sq & p.walls) buffer[k++] = lexicon.wall;
                if (sq & p.caps) buffer[k++] = lexicon.cap;
            }
        }

        // handle remaining jumps
        if (jumping) {
            // if we jump for less than the full row, we can be sure that there
            // were cells previously, so we need the separator
            if (jumping < p.size) buffer[k++] = ',';

            buffer[k++] = lexicon.jump;
            if (jumping > 1) buffer[k++] = '0' + jumping;
            jumping = 0;
        }

        // row separator
        if (row > 0) buffer[k++] = lexicon.row;
    }

    // move count sections
    int rv = sprintf(buffer + k, "%c%d%c%d", lexicon.section, p.stp + 1, lexicon.section,
                     p.mc / 2 + 1);
    return rv < 0 ? rv : k + rv;
}

const ptn_lexicon PTN_STD = {
    .wall = 'S',
    .cap = 'C',
    .columns = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    .rows = {'1', '2', '3', '4', '5', '6', '7', '8'},
    .directions = {'+', '>', '-', '<'},  // north, east, south, west
    .flattens = '*',
};

tak_move ptn_parse(char *ptn, const tak_u8 size, const tak_player stp,
                   const ptn_lexicon lexicon) {
    // not setting it to `0` was brutally destroying parsing with gcc optimization
    // so yeah, in the future better remember to 0 stuff
    tak_move m = {0};

    if (strlen(ptn) == 2 || *ptn == lexicon.cap || *ptn == lexicon.wall) {
        m.type = TAK_PLACEMENT;

        // piece
        if (*ptn == lexicon.cap) {
            m.piece = (tak_piece)(TAK_CAP | stp);
            ptn++;
        } else if (*ptn == lexicon.wall) {
            m.piece = (tak_piece)(TAK_WALL | stp);
            ptn++;
        } else m.piece = (tak_piece)(TAK_FLAT | stp);

        // square
        int row = 0, column = 0;
        while (column < 8 && lexicon.columns[column] != *ptn) column++;
        while (row < 8 && lexicon.rows[row] != *(ptn + 1)) row++;
        m.at = column + row * size;

        return m;
    } else {
        // spread
        m.type = TAK_SPREAD;

        // count
        int count = 1;
        if (*ptn >= '1' && *ptn <= '8') count = *ptn++ - '0';

        // square, direction
        int row = 0, column = 0, d = 0;
        while (column < 8 && lexicon.columns[column] != *ptn) column++;
        while (row < 8 && lexicon.rows[row] != *(ptn + 1)) row++;
        while (d < 4 && lexicon.directions[d] != *(ptn + 2)) d++;
        m.at = column + row * size;
        m.dir = (tak_dir)d;
        ptn += 3;

        // drops
        int sum = 0, drop;
        while (*ptn >= '1' && *ptn <= '8') {
            drop = *ptn - '0';
            m.stacks = m.stacks | drop << 4 * m.length;
            m.length += 1;
            sum += drop;
            ptn++;
        }

        // if no drops, drop all the count on the first cell
        if (sum == 0) {
            m.stacks = count;
            m.length = 1;
        }

        // flattens
        if (*ptn == lexicon.flattens) m.flattens = 1;

        return m;
    }
}

int ptn_encode(char *const buffer, const tak_move m, const tak_u8 size,
               const ptn_lexicon lexicon) {
    int k = 0;

    switch (m.type) {
        case TAK_PLACEMENT:
            // piece modifiers
            if (m.piece & TAK_WALL) buffer[k++] = lexicon.wall;
            if (m.piece & TAK_CAP) buffer[k++] = lexicon.cap;

            // square
            buffer[k++] = lexicon.columns[m.at % size];
            buffer[k++] = lexicon.rows[m.at / size];

            buffer[k++] = '\0';
            return k;

        case TAK_SPREAD:
            int count = 0;
            for (int i = 0; i < m.length; i++) count += (m.stacks >> 4 * i) & 0xf;

            // count (can't be more than 8)
            if (count > 1) buffer[k++] = '0' + count;

            // square, direction
            buffer[k++] = lexicon.columns[m.at % size];
            buffer[k++] = lexicon.rows[m.at / size];
            buffer[k++] = lexicon.directions[m.dir];

            // drops (each can't be more than 8)
            if (m.length > 1)
                for (int i = 0; i < m.length; i++)
                    buffer[k++] = '0' + (m.stacks >> 4 * i & 0xf);

            // flattens
            if (m.flattens) buffer[k++] = lexicon.flattens;

            buffer[k++] = '\0';
            return k;
    }

    // sohuld never reach
    return 0;
}

tak_position ptn_apply(tak_position p, char **const ptn, const int n,
                       const ptn_lexicon lexicon, const tak_zobrist_data *const zd) {
    for (int i = 0; i < n; i++) {
        tak_player stp = p.mc < TAK_SWAPN ? 1 - p.stp : p.stp;
        tak_move move = ptn_parse(ptn[i], p.size, stp, lexicon);
        p = tak_do(p, move, zd);
    }

    return p;
}

// NOLINTEND(misc-definitions-in-headers)
#endif  // TAKLIB_IMPLEMENTATION