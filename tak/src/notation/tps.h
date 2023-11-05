#pragma once

#include <stdint.h>

#include "../position.h"

// on 8x8, arranging the flats as 96 walls and 4 flats:
//  7 row separators
//  56 cell separators
//  4 flats color indicators
//  96 wall modifiers
//  4 cap modifiers
//  1 std
//  3 mc
//  2 spaces
// => 173 + \0
#define TAK_MAX_TPS 174

// tps lexicon
// specifies all the token needed to parse and encode a tps string
typedef struct {
    char row_s;      // separate two rows
    char cell_s;     // separate two cells
    char section_s;  // separate two sections
    char wall;       // wall modifier
    char cap;        // cap modifier
    char jump;       // empty cell
    char white;      // player 1
    char black;      // player 2
} tak_tps_lexicon;

// standard commonly used lexicon
extern const tak_tps_lexicon TAK_TPS_STD;

// get the board size from a tps string
int tak_tps_gets(char *tps, tak_tps_lexicon lexicon);

// since tps start describing rows from the highest,
// to correctly index the cell we have to mirror it vertically
int tak_mirror_index(int index, int size);

// parse tps string into new position
tak_position tak_tps2p(char *tps, tak_tps_lexicon lexicon);

// encode a position as tps and load the result in `buffer`.
// buffer is expected to have capacity >= TAK_MAX_TPS_LENGTH.
// returns the length of the tps string
int tak_p2tps(char *buffer, tak_position p, tak_tps_lexicon lexicon);
