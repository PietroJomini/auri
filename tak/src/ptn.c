#include "ptn.h"

#include "moves.h"
#include "position.h"
#include "slides.h"

int placement2ptn(char *buffer, Placement pl, Position p) {
    int k = 0;

    // piace modifier
    if ((pl.piece & PType) > 1) buffer[k++] = (pl.piece & PType) == Cap ? 'C' : 'W';

    // square
    buffer[k++] = 'a' + pl.at % p.size;
    buffer[k++] = '0' + pl.at / p.size;

    buffer[k] = '\0';
    return k;
}

int slide2ptn(char *buffer, Slide s, Position p) {
    int k = 0;

    // count
    // TODO: if the count is > 9 this will fail
    int count = 0;
    for (int i = 0; i < s.length; i++) count += slide_n(s.stacks, i);
    buffer[k++] = '0' + count;

    // square
    buffer[k++] = 'a' + s.origin % p.size;
    buffer[k++] = '0' + s.origin / p.size;

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