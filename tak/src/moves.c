#include "moves.h"

#include <stdint.h>

#include "notation/ptn.h"
#include "notation/tps.h"
#include "position.h"

int tak_search_placements(tak_placement *buffer, tak_position const *p) {
    int k = 0;

    tak_player stp = p->mc < TAK_SWAP_MOVES ? 1 - p->stp : p->stp;
    uint64_t fmask = p->cbb[0] | p->cbb[1];

    for (int i = 0; i < p->size * p->size; i++) {
        if ((fmask & 1ull << i) == 0) {
            // flats
            if (p->reserves[stp][0])
                buffer[k++] = (tak_placement){.at = i, .piece = stp | TAK_FLAT};

            // walls. can be placed only after the second turn
            if (p->reserves[stp][0] && p->mc >= TAK_SWAP_MOVES)
                buffer[k++] = (tak_placement){.at = i, .piece = stp | TAK_WALL};

            // caps. can be placed only after the second turn
            if (p->reserves[stp][1] && p->mc >= TAK_SWAP_MOVES)
                buffer[k++] = (tak_placement){.at = i, .piece = stp | TAK_CAP};
        }
    }

    return k;
}

tak_position tak_do_placement(tak_position p, tak_placement pl) {
    tak_player color = pl.piece & 1;

    // stack and height
    p.stacks[pl.at] = color;
    p.heights[pl.at] = 1;

    // color bitmaps
    uint64_t sq = 1ull << pl.at;
    p.cbb[color] |= sq;

    // modifiers bitmaps
    if (pl.piece & TAK_WALL) p.walls |= sq;
    if (pl.piece & TAK_CAP) p.caps |= sq;

    // reserves, mc, stp
    p.reserves[color][(pl.piece & TAK_CAP) == TAK_CAP] -= 1;
    p.stp = 1 - p.stp;
    p.mc += 1;

    return p;
}

uint32_t tak_slt_reduce(int n, int h) {
    uint32_t stacks = 0;
    uint8_t stack = 1;

    for (int i = h - 2; i >= 0; i--) {
        if (n & (1 << i)) {
            stacks = (stacks << 4) | stack;
            stack = 1;
        } else stack += 1;
    }

    return stacks << 4 | stack;
}

tak_slideslt tak_slt_load() {
    tak_slideslt lt = {0};
    int k = 0;  // global slide index

    for (int h = 1; h <= 8; h++) {
        // load height index
        lt.indexes[h - 1] = k;

        for (int i = 0; i < 1 << (h - 1); i++) {
            // load slide stacks and heights
            uint32_t stacks = tak_slt_reduce(i, h);
            int length = 1 + __builtin_popcount(i);

            // add the normal slide
            if (length > 1) {
                lt.slides[k] = stacks;
                lt.lengths[k++] = length;
            }

            // add the slide shifted by one cell, so that no
            // stones are dropped in the origin square
            if (length < 8) {
                lt.slides[k] = stacks << 4;
                lt.lengths[k++] = length + 1;
            }
        }
    }

    return lt;
}

// TODO: experiment with pre-loaded walk results as early
//       experiments showed some improvement in performance
int tak_walk(uint8_t origin, tak_direction d, uint8_t size) {
    if (d == TAK_NORTH) return (origin >= size * (size - 1)) ? -1 : origin + size;
    else if (d == TAK_EAST) return ((origin + 1) % size == 0) ? -1 : origin + 1;
    else if (d == TAK_SOUTH) return (origin < size) ? -1 : origin - size;
    else if (d == TAK_WEST) return (origin % size == 0) ? -1 : origin - 1;
    else return -1;
}

// position and are passed by reference to reduce call time
int tak_search_slides_atdir(tak_slide *buffer, tak_position const *p, uint8_t origin,
                            tak_direction d, tak_slideslt const *slt) {
    if (p->heights[origin] == 0) return 0;

    int length = 1, target = tak_walk(origin, d, p->size);
    uint64_t mask = ~(p->caps | p->walls);

    // compute the max length of the slide
    while (target != -1 && mask & 1ull << target) {
        target = tak_walk(target, d, p->size);
        length++;
    }

    // height of the legally movable stack
    int h = (p->heights[origin] < p->size) ? p->heights[origin] : p->size;
    int k = 0, index = slt->indexes[h - 1],
        next_index = slt->indexes[h];  // TODO: handle next_index overflow

    while (slt->lengths[index] <= length && index < next_index) {
        buffer[k++] = (tak_slide){.origin = origin,
                                  .direction = d,
                                  .length = slt->lengths[index],
                                  .stacks = slt->slides[index],
                                  .flattens = 0};
        index += 1;
    }

    // check for flattening possibilities
    if (target != -1 && p->caps & 1ull << origin && p->walls & 1ull << target)
        while (slt->lengths[index] <= length + 1 && index < next_index) {
            if (slt->slides[index] >> 4 * (slt->lengths[index] - 1) == 1)
                buffer[k++] = (tak_slide){.origin = origin,
                                          .direction = d,
                                          .length = slt->lengths[index],
                                          .stacks = slt->slides[index],
                                          .flattens = 1};
            index += 1;
        }

    return k;
}

int tak_search_slides_at(tak_slide *buffer, tak_position const *p, uint8_t origin,
                         tak_slideslt const *slt) {
    int k = 0;
    k += tak_search_slides_atdir(buffer, p, origin, TAK_NORTH, slt);
    k += tak_search_slides_atdir(buffer + k, p, origin, TAK_EAST, slt);
    k += tak_search_slides_atdir(buffer + k, p, origin, TAK_SOUTH, slt);
    k += tak_search_slides_atdir(buffer + k, p, origin, TAK_WEST, slt);
    return k;
}

int tak_search_slides(tak_slide *buffer, tak_position const *p, tak_slideslt const *slt) {
    // this should be in tak_search_slides_atdir, but it's faster here
    if (p->mc < TAK_SWAP_MOVES) return 0;

    int k = 0;
    for (int i = 0; i < p->size * p->size; i++)
        if (p->cbb[p->stp] & (1ull << i))
            k += tak_search_slides_at(buffer + k, p, i, slt);

    return k;
}

tak_position tak_do_slide(tak_position p, tak_slide s) {
    // last cell of the slide
    int target = s.origin + (s.length - 1) * (s.direction == TAK_EAST    ? 1
                                              : s.direction == TAK_SOUTH ? -p.size
                                              : s.direction == TAK_WEST  ? -1
                                                                         : p.size);

    // modifiers
    uint64_t os = 1ull << s.origin, ts = 1ull << target;
    if (p.caps & os) p.caps = (p.caps & ~os) | ts;
    if (p.walls & os) p.walls = (p.walls & ~os) | ts;
    if (s.flattens) p.walls &= ~ts;

    // opposite direction of the slide
    tak_direction od = tak_opposite_d(s.direction);

    int i = 0;
    while (target != s.origin) {
        // get the height of the current drop
        int h = s.stacks >> 4 * (s.length - i - 1) & 0xf;

        // update target stack
        // TODO: here on 8x8 it can overflow, check and avoid
        p.stacks[target] <<= h;
        p.stacks[target] |= p.stacks[s.origin] & ((1ull << h) - 1);

        // update origin stack
        p.stacks[s.origin] >>= h;

        // update heights
        p.heights[target] += h;
        p.heights[s.origin] -= h;

        // update target color
        uint64_t ts = 1ull << target, color = p.stacks[target] & 1;
        p.cbb[color] |= ts;
        p.cbb[1 - color] &= ~ts;

        // walk target
        target = tak_walk(target, od, p.size);
        i++;
    }

    // update origin color
    if (p.heights[s.origin] == 0) {
        p.cbb[0] &= ~os;
        p.cbb[1] &= ~os;
    } else {
        p.cbb[p.stacks[s.origin] & 1] |= os;
        p.cbb[1 - (p.stacks[s.origin] & 1)] &= ~os;
    }

    // mc, stp
    p.stp = 1 - p.stp;
    p.mc += 1;

    return p;
}

uint64_t tak_perft(tak_position p, int depth, tak_slideslt const *slt) {
    if (depth == 0) return 1;
    uint64_t nodes = 0;

    // check game endings
    tak_endstatus status = tak_check_ending(&p);
    if (status.ended) return 1;

    // load moves
    // TODO: while placements are capped at 196 (64*3), what is the max amount of slides?
    tak_placement pb[196];
    tak_slide sb[500];
    int np = tak_search_placements(pb, &p);
    int ns = tak_search_slides(sb, &p, slt);

    // bulk perft
    if (depth == 1) return np + ns;

    // TODO: benchmark a tak_undo_move version
    for (int i = 0; i < np; i++)
        nodes += tak_perft(tak_do_placement(p, pb[i]), depth - 1, slt);
    for (int i = 0; i < ns; i++)
        nodes += tak_perft(tak_do_slide(p, sb[i]), depth - 1, slt);

    return nodes;
}