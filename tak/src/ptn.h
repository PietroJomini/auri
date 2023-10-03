// https://ustak.org/portable-tak-notation/

#pragma once

#include <stdint.h>
#include "moves.h"
#include "position.h"

// the longest ptn slide is 7a1>1111111*
#define MAX_PTN_MOVE_S 13
#define MAX_PTN_MOVE MAX_PTN_MOVE_S

// square + modifier
#define MAX_PTN_MOVE_P 3

// convert placement to ptn representation.
// returns the lenght of the string
int placement2ptn(char *buffer, Placement pl, uint8_t size);

// convert slide to ptn representation in the format
//      (count)(square)(direciton)(drop counts)[flattens]
// returns the length of the string
int slide2ptn(char *buffer, Slide s, uint8_t size);

// generic combination of `placement2ptn` and `slide2ptn`
int move2ptn(char *ptn, Move m, uint8_t size);

// parse a playtak-formatted move
//      (P|M) (data)     P = placement, M = slide
//       P => (data) := (square)
//       M => (data) := (origin) (end) (drop counts)
// NOTE: this doesn't check the move legality
Move playtak2move(char *src, Position p);

// load playtak move list
Position playtak2position(char *src, uint8_t size);