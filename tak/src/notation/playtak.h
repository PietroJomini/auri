#pragma once

#include "../position.h"
#include "../moves.h"

// parse a playtak-formatted move
//      (P|M) (data)     P = placement, M = slide
//       P => (data) := (square)
//       M => (data) := (origin) (end) (drop counts)
// NOTE: this doesn't check the move legality
tak_move tak_playtak2move(char *src, tak_position p);

// load playtak move list
tak_position tak_playtak2position(char *src, uint8_t size);