#define _GNU_SOURCE

#include "playtak.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tak_move tak_playtak2move(char *src, tak_position p) {
    tak_move m = {0};

    // P (column)(row) [modifier]
    if (*src == 'P') {
        int row;
        char column, modifier = 0;
        sscanf(src, "P %c%d %c", &column, &row, &modifier);
        m.u.p.at = column - 'A' + p.size * (row - 1);
        m.u.p.piece = (modifier == 'C'   ? TAK_CAP
                       : modifier == 'W' ? TAK_WALL
                                         : TAK_FLAT) |
                      ((p.mc < 2 ? 1 - p.stp : p.stp) ? TAK_WHITE : TAK_BLACK);
        m.t = TAK_PLACEMENT;
        return m;
    }

    // M (origin column)(origin row) (destination column)(destination row) (drops)
    if (*src == 'M') {
        int row, drow, drop, dropt = 0;
        char drops[17] = {0}, column, dcolumn;
        sscanf(src, "M %c%d %c%d %[0-9 ]", &column, &row, &dcolumn, &drow, drops);

        for (size_t i = strlen(drops); i > 0; i--) {
            if (drops[i - 1] >= '0' && drops[i] <= '9') {
                drop = drops[i - 1] - '0';
                m.u.s.stacks |= drop;
                m.u.s.stacks <<= 4;
                m.u.s.length += 1;
                dropt += drop;
            }
        }

        m.u.s.direction = (dcolumn > column)   ? TAK_EAST
                          : (dcolumn < column) ? TAK_WEST
                          : (drow > row)       ? TAK_NORTH
                                               : TAK_SOUTH;
        m.u.s.origin = column - 'A' + p.size * (row - 1);
        m.t = TAK_SLIDE;

        // since the playtak notation only counts the drops, we should add
        // the count of remaining pieces in the origin square
        m.u.s.stacks |=
            ((p.size < p.heights[m.u.s.origin]) ? p.size : p.heights[m.u.s.origin]) -
            dropt;

        m.u.s.length += 1;

        return m;
    }

    // this should never happen
    // TODO: anyway, soon i'll have to handle errors better
    exit(EXIT_FAILURE);
}


tak_position tak_playtak2position(char *src, uint8_t size) {
    tak_position p = tak_new_position(size);

    // this is needed to allow strtok to modify the pointer to srcd,
    // which would not be possible with src (which is constant)
    char srcd[strlen(src) + 1];
    memcpy(srcd, src, strlen(src) * sizeof(char));
    srcd[strlen(src)] = '\0';

    // TODO: do i need to free() the token?
    char *rest, *token;

    for (token = strtok_r(srcd, ",", &rest); token != NULL;
         token = strtok_r(NULL, ",", &rest)) {
        tak_move m = tak_playtak2move(token, p);
        if (m.t == TAK_SLIDE) p = tak_do_slide(p, m.u.s);
        else p = tak_do_placement(p, m.u.p);
    }

    return p;
}