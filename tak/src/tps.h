/*
tps standard format (https://ustak.org/tak-positional-system-tps/).

[board description] [turn identifier] [move counter]
    - board description: row 1/row 2/.../row n
        - row n: square 1,square 2,...,square 3
        - n empty squares can be in "xn"
        - stones are listed in bottom to top order
        - leading S or C denotes walls of caps
    - turn identifier: 1/2 based on who is to play
    - move counter: #n of the move due to be played

known identifiers:
    ' ': chunks separator
    '/': rows separator
    ',': squares separator
    'x': empty cell
    'S': wall modifier
    'C': capstone modifier
    '1'-'9': digits
*/

#pragma once

#include "position.h"

// Parse a tps string into a new position
Position tps2p(char *tps);

// TODO: what is the max length of a tps string? (required buffer length)
// Encode a position as tps
int p2tps(char *buffer, Position p);