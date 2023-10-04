#define _GNU_SOURCE

#include "ptn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "moves.h"
#include "position.h"
#include "slides.h"

int placement2ptn(char *buffer, Placement pl, uint8_t size) {
    int k = 0;

    // piace modifier
    if ((pl.piece & PType) > 1) buffer[k++] = (pl.piece & PType) == Cap ? 'C' : 'W';

    // square
    buffer[k++] = 'a' + pl.at % size;
    buffer[k++] = '1' + pl.at / size;

    buffer[k] = '\0';
    return k;
}

int slide2ptn(char *buffer, Slide s, uint8_t size) {
    int k = 0;

    // count
    // TODO: if the count is > 9 this will fail
    int count = 0;
    for (int i = 0; i < s.length; i++) count += slide_n(s.stacks, i);
    buffer[k++] = '0' + count;

    // square
    buffer[k++] = 'a' + s.origin % size;
    buffer[k++] = '1' + s.origin / size;

    // direction
    buffer[k++] = s.direction == North   ? '+'
                  : s.direction == East  ? '>'
                  : s.direction == South ? '-'
                  : s.direction == West  ? '<'
                                         : '?';

    // drop counts
    for (int i = 0; i < s.length; i++) buffer[k++] = '0' + slide_n(s.stacks, i);

    // flattens
    if (s.flattens) buffer[k++] = '*';

    buffer[k] = '\0';
    return k;
}

int move2ptn(char *ptn, Move m, uint8_t size) {
    return (m.type == Placement_t) ? placement2ptn(ptn, m.move.placement, size)
                                   : slide2ptn(ptn, m.move.slide, size);
}

Move playtak2move(char *src, Position p) {
    Move m = {0};

    // P (column)(row) [modifier]
    if (*src == 'P') {
        int row;
        char column, modifier = 0;
        sscanf(src, "P %c%d %c", &column, &row, &modifier);
        m.move.placement.at = column - 'A' + p.size * (row - 1);
        m.move.placement.piece = (modifier == 'C'   ? Cap
                                  : modifier == 'W' ? Wall
                                                    : Flat) |
                                 ((p.mc < 2 ? 1 - p.stp : p.stp) ? Black : White);
        m.type = Placement_t;
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
                m.move.slide.stacks |= drop;
                m.move.slide.stacks <<= 4;
                m.move.slide.length += 1;
                dropt += drop;
            }
        }

        m.move.slide.direction = (dcolumn > column)   ? East
                                 : (dcolumn < column) ? West
                                 : (drow > row)       ? North
                                                      : South;
        m.move.slide.origin = column - 'A' + p.size * (row - 1);
        m.type = Slide_t;

        // since the playtak notation only counts the drops, we should add
        // the count of remaining pieces in the origin square
        m.move.slide.stacks |=
            ((p.size < p.heights[m.move.slide.origin]) ? p.size
                                                       : p.heights[m.move.slide.origin]) -
            dropt;
        m.move.slide.length += 1;

        return m;
    }

    // this should never happen
    // TODO: anyway, soon i'll have to handle errors better
    exit(EXIT_FAILURE);
}

Position playtak2position(char *src, uint8_t size) {
    Position p = pempty_s(size);

    // this is needed to allow strtok to modify the pointer to srcd,
    // which would not be possible with src (which is constant)
    char srcd[strlen(src) + 1];
    memcpy(srcd, src, strlen(src) * sizeof(char));
    srcd[strlen(src)] = '\0';

    // TODO: do i need to free() the token?
    char *rest, *token;

    for (token = strtok_r(srcd, ",", &rest); token != NULL;
         token = strtok_r(NULL, ",", &rest)) {
        p = do_move(p, playtak2move(token, p));
    }

    return p;
}