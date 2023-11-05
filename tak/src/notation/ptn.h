#pragma once

#include <stdint.h>

#include "../moves.h"

// tps lexicon
// specifies all the token needed to parse and encode a tps string
typedef struct {
    char wall;           // wall modifier
    char cap;            // cap modifier
    char columns[8];     // column names
    char rows[8];        // row names
    char directions[4];  // direction indicators
    char flattens;       // flattening slide indicator
} tak_ptn_lexicon;

// standard commonly used lexicon
extern const tak_ptn_lexicon TAK_PTN_STD;

// the longest ptn slide is 7a1>1111111*
#define TAK_MAX_PTN_MOVE_S 13
#define TAK_MAX_PTN_MOVE 13

// square + modifier
#define TAK_MAX_PTN_MOVE_P 3

// convert a plecement to ptn
// returne the length of the string
int tak_placement2ptn(char *buffer, tak_placement p, uint8_t size, tak_ptn_lexicon lexicon);

// convert slide to ptn representation in the format
//      (count)(square)(direction)(drop counts)[flattens]
// returns the length of the string
int tak_slide2ptn(char *buffer, tak_slide s, uint8_t size, tak_ptn_lexicon lexicon);