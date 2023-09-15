#include "moves.h"

#include <stdint.h>

#include "position.h"

uint8_t placements_count(Position p) {
    int kinds = (p.reserve[p.stp][0] ? 2 : 0) + (p.reserve[p.stp][1] ? 1 : 0);
    int spaces = p.size * p.size - __builtin_popcount(p.white | p.black);
    return kinds * spaces;
}

// TODO: is it better to allocate the buffer here?
void placements(Placement *buffer, Position p) {
    int color = p.stp ? Black : White;
    int k = 0;

    for (int i = 0; i < p.size * p.size; i++) {
        // check if the cell is empty
        if (((1ull << i) & (p.white | p.black)) == 0) {
            if (p.reserve[p.stp][0]) {
                // flats + walls
                buffer[k++] = (Placement){.at = i, .piece = color | Flat};
                buffer[k++] = (Placement){.at = i, .piece = color | Wall};
            }
            if (p.reserve[p.stp][1]) {
                // caps
                buffer[k++] = (Placement){.at = i, .piece = color | Cap};
            }
        }
    }
}
