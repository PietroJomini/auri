#include "ptn.h"

const tak_ptn_lexicon TAK_PTN_STD = {
    .wall = 'S',
    .cap = 'C',
    .columns = {'1', '2', '3', '4', '5', '6', '7', '8'},
    .rows = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    .directions = {'+', '>', '-', '<'},  // north, east, south, west
    .flattens = '*',
};

int tak_placement2ptn(char *buffer, tak_placement p, uint8_t size,
                      tak_ptn_lexicon lexicon) {
    int k = 0;

    // piece modifiers
    if (p.piece & TAK_WALL) buffer[k++] = lexicon.wall;
    if (p.piece & TAK_CAP) buffer[k++] = lexicon.cap;

    // square
    buffer[k++] = lexicon.rows[p.at % size];
    buffer[k++] = lexicon.columns[p.at / size];

    buffer[k++] = '\0';
    return k;
}

// TODO: ptn should not include the stones left on the
int tak_slide2ptn(char *buffer, tak_slide s, uint8_t size, tak_ptn_lexicon lexicon) {
    int k = 0;

    // count
    // TODO: if the count is > 9 this will fail
    int count = 0;
    for (int i = 0; i < s.length; i++) count += (s.stacks >> 4 * i) & 0xf;
    buffer[k++] = '0' + count;

    // square, direction
    buffer[k++] = lexicon.rows[s.origin % size];
    buffer[k++] = lexicon.columns[s.origin / size];
    buffer[k++] = lexicon.directions[s.direction];

    // drop count
    // TODO: this shouldn't include stones dropped on the origin square
    for (int i = 0; i < s.length; i++) buffer[k++] = '0' + (s.stacks >> 4 * i & 0xf);

    // flattens
    if (s.flattens) buffer[k++] = lexicon.flattens;

    buffer[k++] = '\0';
    return k;
}