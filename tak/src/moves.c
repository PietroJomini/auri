#include "moves.h"

#include <stdint.h>

#include "position.h"
#include "slides.h"

int placements(Placement *buffer, Position p) {
    // swap rule
    int stp = p.mc < 2 ? 1 - p.stp : p.stp;
    int color = p.stp ? Black : White;

    int k = 0;
    for (int i = 0; i < p.size * p.size; i++) {
        // check if the cell is empty
        if (((1ull << i) & (p.white | p.black)) == 0) {
            if (p.reserve[stp][0]) {
                // flats
                buffer[k++] = (Placement){.at = i, .piece = color | Flat};

                // walls. can be placed only after the second turn
                if (p.mc >= 2) buffer[k++] = (Placement){.at = i, .piece = color | Wall};
            }
            if (p.mc >= 2 && p.reserve[stp][1]) {
                // caps. can be placed only after the second turn
                buffer[k++] = (Placement){.at = i, .piece = color | Cap};
            }
        }
    }

    return k;
}

Position do_placement(Position p, Placement pl) {
    // update stack
    // is `p.stacks[pl.at] = (pl.piece & Color) >> 2;` faster?
    if ((pl.piece & PColor) == Black) p.stacks[pl.at] = 1;

    // update height
    p.heights[pl.at] = 1;

    // update color bitmaps
    uint64_t sq = 1ull << pl.at;
    uint64_t *bb = (pl.piece & Black) ? &p.black : &p.white;
    *bb |= sq;

    // update modifiers bitmaps
    if ((pl.piece & PType) == Wall) p.walls |= sq;
    if ((pl.piece & PType) == Cap) p.caps |= sq;

    // update reserve
    p.reserve[p.stp][(pl.piece & PType) == Cap] -= 1;

    // update move count
    p.mc++;
    p.stp = 1 - p.stp;

    return p;
}

int8_t walk(uint8_t origin, Direction dir, uint8_t size) {
    if (dir == North) return (origin >= size * (size - 1)) ? -1 : origin + size;
    else if (dir == East) return ((origin + 1) % size == 0) ? -1 : origin + 1;
    else if (dir == South) return (origin < size) ? -1 : origin - size;
    else if (dir == West) return (origin % size == 0) ? -1 : origin - 1;
    else return -1;
}

int slides_atdir(Slide *buffer, Position p, uint8_t origin, Direction dir) {
    if (p.heights[origin] == 0) return 0;

    int length = 1, target = walk(origin, dir, p.size);
    uint64_t mask = ~(p.caps | p.walls);

    // compute the max length of the slide
    while (target != -1 && mask & (1ull << (target))) {
        target = walk(target, dir, p.size);
        length++;
    }

    // height of the legally movable stack
    int h = (p.heights[origin] < p.size) ? p.heights[origin] : p.size;
    int index = slides_index(h), k = 0, next_index = slides_index(h + 1);

    while (SLIDES_LENGHT[index] <= length && index < next_index) {
        buffer[k++] = (Slide){.origin = origin,
                              .direction = dir,
                              .length = SLIDES_LENGHT[index],
                              .stacks = SLIDES[index],
                              .flattens = 0};
        index += 1;
    }

    // check for flattening possibilities
    if (target != -1 && (p.caps & (1ull << origin)) && ((p.walls & (1ull << target))))
        while (SLIDES_LENGHT[index] <= length + 1 && index < next_index) {
            if (slide_n(SLIDES[index], SLIDES_LENGHT[index] - 1) == 1)
                buffer[k++] = (Slide){.origin = origin,
                                      .direction = dir,
                                      .length = SLIDES_LENGHT[index],
                                      .stacks = SLIDES[index],
                                      .flattens = 1};

            index += 1;
        }

    return k;
}

int slides_at(Slide *buffer, Position p, uint8_t origin) {
    int n = 0;
    n += slides_atdir(buffer, p, origin, North);
    n += slides_atdir(buffer + n, p, origin, East);
    n += slides_atdir(buffer + n, p, origin, South);
    n += slides_atdir(buffer + n, p, origin, West);
    return n;
}

int slides(Slide *buffer, Position p) {
    // the first two moves can only be placements.
    // TODO: should this be in slides_atdir()?
    if (p.mc < 2) return 0;

    uint64_t bb = p.stp ? p.black : p.white;
    int n = 0;
    for (int i = 0; i < p.size * p.size; i++)
        if ((1ull << i) & bb) n += slides_at(buffer + n, p, i);
    return n;
}

Position do_slide(Position p, Slide s) {
    // last cell of the slide
    int target = s.origin + (s.length - 1) * (s.direction == East    ? 1
                                              : s.direction == South ? -p.size
                                              : s.direction == West  ? -1
                                                                     : p.size);

    // update modifiers
    uint64_t os = 1ull << s.origin, ts = 1ull << target;
    if (p.caps & os) p.caps = (p.caps & ~os) | ts;
    if (p.walls & os) p.walls = (p.walls & ~os) | ts;
    if (s.flattens) p.walls &= ~ts;

    // compute the direction opposite to the slide
    Direction od = s.direction == East    ? West
                   : s.direction == South ? North
                   : s.direction == West  ? East
                                          : South;

    int i = 0;
    while (target != s.origin) {
        int h = slide_n(s.stacks, s.length - i - 1);

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
        uint64_t ts = 1ull << target;
        uint64_t *cbb = p.stacks[target] & 1 ? &p.black : &p.white;
        uint64_t *obb = p.stacks[target] & 1 ? &p.white : &p.black;
        *cbb |= ts;
        *obb &= ~ts;

        // walk target
        target = walk(target, od, p.size);
        i++;
    }

    // update the origin color
    if (p.heights[s.origin] == 0) {
        p.white &= ~os;
        p.black &= ~os;
    } else {
        uint64_t *cbb = p.stacks[s.origin] & 1 ? &p.black : &p.white;
        uint64_t *obb = p.stacks[s.origin] & 1 ? &p.white : &p.black;
        *cbb |= os;
        *obb &= ~os;
    }

    // update move count
    p.mc++;
    p.stp = 1 - p.stp;

    return p;
}

Position do_move(Position p, Move m) {
    return (m.type == Slide_t) ? do_slide(p, m.move.slide)
                               : do_placement(p, m.move.placement);
}

uint64_t perft(Position p, int depth) {
    if (depth == 0) return 1;
    uint64_t nodes = 0;

    // ended game check
    // TODO: maybe this should be in the moves generation?
    EndStatus status = check_ending(p);
    if (status.ended) return 1;

    // load moves
    // TODO: while placements are capped at 196 (64*3), what is the max amount of slides?
    Placement pb[196];
    Slide sb[500];

    int np = placements(pb, p);
    int ns = slides(sb, p);

    // bulk perft
    if (depth == 1) return np + ns;

    for (int i = 0; i < np; i++) nodes += perft(do_placement(p, pb[i]), depth - 1);
    for (int i = 0; i < ns; i++) nodes += perft(do_slide(p, sb[i]), depth - 1);

    return nodes;
}