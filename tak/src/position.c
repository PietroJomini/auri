#include "position.h"

#include <limits.h>
#include <stdint.h>

#include "moves.h"

const uint8_t TAK_RESERVES[6][2] = {{10, 0}, {15, 0}, {21, 1}, {30, 1}, {40, 2}, {50, 2}};

uint8_t _tak_search_road_r(tak_position const *p, uint8_t origin, uint8_t i,
                           uint64_t *history) {
    // check and set history
    if (*history & (1ull << i)) return 0;
    *history |= 1ull << i;

    // check if the current index links a road
    if ((origin / p->size == 0 && i / p->size == p->size - 1) ||
        (origin % p->size == 0 && i % p->size == p->size - 1))
        return 1;

    // spread neighbours
    int next_sq, color_check;
    for (tak_direction d = TAK_NORTH; d <= TAK_WEST; d++) {
        next_sq = tak_walk(i, d, p->size);
        color_check = p->cbb[p->stp] & ~p->walls & 1ull << next_sq;
        if (next_sq != -1 && color_check &&
            _tak_search_road_r(p, origin, next_sq, history))
            return 1;
    }

    return 0;
}

uint8_t tak_search_road(tak_position const *p) {
    int color_check;

    // vertical roads
    uint64_t history = 0;
    for (int i = 0; i < p->size; i++) {
        color_check = p->cbb[p->stp] & ~p->walls & 1ull << i;
        if (color_check && _tak_search_road_r(p, i, i, &history)) return 1;
    }

    // horizontal roads
    history = 0;
    for (int i = 0; i < p->size; i++) {
        color_check = p->cbb[p->stp] & ~p->walls & 1ull << i * p->size;
        if (color_check && _tak_search_road_r(p, i * p->size, i * p->size, &history))
            return 1;
    }

    return 0;
}

tak_endstatus tak_check_ending(tak_position const *p) {
    // road endings
    if (tak_search_road(p))
        return (tak_endstatus){.ended = 1, .ending = TAK_ROAD, .winner = p->stp};

    // reserves endings
    if ((p->reserves[0][0] == 0 && p->reserves[0][1] == 0) ||
        (p->reserves[1][0] == 0 && p->reserves[1][1] == 0)) {
        int popc_w = __builtin_popcount(p->cbb[0] & ~p->caps & ~p->walls);
        int popc_b = __builtin_popcount(p->cbb[1] & ~p->caps & ~p->walls);
        if (popc_b == popc_w) return (tak_endstatus){.ended = 1, .ending = TAK_TIE};
        return (tak_endstatus){.ended = 1,
                               .ending = TAK_FLATWIN,
                               .winner = popc_w > popc_b ? TAK_WHITE : TAK_BLACK};
    }

    if ((p->cbb[0] | p->cbb[1]) == 0xffffffffffffffff)
        return (tak_endstatus){.ended = 1, .ending = TAK_TIE};

    return (tak_endstatus){.ended = 0};
}