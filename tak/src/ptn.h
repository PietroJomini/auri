// https://ustak.org/portable-tak-notation/

#pragma once

#include "moves.h"
#include "position.h"

// the longest ptn slide is 7a1>1111111*
#define MAX_PTN_MOVE_S 13
#define MAX_PTN_MOVE MAX_PTN_MOVE_S

// square + modifier
#define MAX_PTN_MOVE_P 3

// convert placement to ptn representation.
// returns the lenght of the string
int placement2ptn(char *buffer, Placement pl, Position p);

// convert slide to ptn representation in the format
//      (count)(square)(direciton)(drop counts)[flattens]
// returns the length of the string
int slide2ptn(char *buffer, Slide s, Position p);