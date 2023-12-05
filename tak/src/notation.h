#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tak.h"

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
#define TPS_MAX_LENGTH 174

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
} tps_lexicon;

// standard commonly used lexicon
extern const tps_lexicon TPS_STD;

// get the board size from a tps string
int tps_getsize(char *tps, tps_lexicon lexicon);

// since tps start describing rows from the highest,
// to correctly index the cell we have to mirror it vertically
int mirror_index(int index, int size);

// parse tps string into new position
position tps2position(char *tps, tps_lexicon lexicon, zobrist_data *zd);

// encode a position as tps and load the result in `buffer`.
// buffer is expected to have capacity >= TPS_MAX_LENGTH.
// returns the length of the tps string
int position2tps(char *buffer, position p, tps_lexicon lexicon);

// tps lexicon
// specifies all the token needed to parse and encode a tps string
typedef struct {
    char wall;           // wall modifier
    char cap;            // cap modifier
    char columns[8];     // column names
    char rows[8];        // row names
    char directions[4];  // direction indicators
    char flattens;       // flattening slide indicator
} ptn_lexicon;

// standard commonly used lexicon
extern const ptn_lexicon PTN_STD;

// the longest ptn slide is 7a1>1111111*
#define MAX_PTN_MOVE_S 13
#define MAX_PTN_MOVE 13

// square + modifier
#define MAX_PTN_MOVE_P 3

// convert a plecement to ptn
// return the length of the string
int placement2ptn(char *buffer, placement p, u8 size, ptn_lexicon lexicon);

// convert slide to ptn representation in the format
//      (count)(square)(direction)(drop counts)[flattens]
// return the length of the string
int slide2ptn(char *buffer, slide s, u8 size, ptn_lexicon lexicon);

// convert move to ptn representation
// return the length of the string
int move2ptn(char *buffer, move m, u8 size, ptn_lexicon lexicon);

// parse a ptn string into a move
// the input is expected to be correct
move ptn2move(char *ptn, u8 size, player stp, ptn_lexicon lexicon);

// apply a list of ptn moves to a position
position ptn2position(position p, char **ptn, int n, ptn_lexicon lexicon,
                      zobrist_data *zd);