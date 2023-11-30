#include "tak.h"

const uint8_t RESERVES[6][2] = {{10, 0}, {15, 0}, {21, 1}, {30, 1}, {40, 2}, {50, 2}};

position new_position(int size) {
    return (position){
        .size = size,
        .stp = WHITE,
        .reserves = {{RESERVES[size - 3][0], RESERVES[size - 3][1]},
                     {RESERVES[size - 3][0], RESERVES[size - 3][1]}},
    };
}

zobrist_data zobrist_fill() {
    xoshiro_state state = {.s = {3, 1, 4, 1}};  // TODO: find good values to init this
    zobrist_data d;

    d.btp = xoshiro(&state);
    for (int i = 0; i < ZOBRIST_STACK_HEIGHT_THRESH * 64; i++)
        d.stacks[i] = xoshiro(&state);
    for (int i = 0; i < 64; i++) {
        d.walls[i] = xoshiro(&state);
        d.caps[i] = xoshiro(&state);
    }

    return d;
}

void zobrist(position *p, zobrist_data *d) {
    p->hash = 0;

    // btp
    if (p->stp == 1) p->hash ^= d->btp;

    u64 sq = 1;
    for (int i = 0; i < p->size * p->size; i++) {
        // stacks
        for (int h = 0; h < ZOBRIST_STACK_HEIGHT_THRESH && h < p->heights[i]; h++)
            p->hash ^= d->stacks[h + i * ZOBRIST_STACK_HEIGHT_THRESH];

        // walls, caps
        if (p->walls & sq) p->hash ^= d->walls[i];
        if (p->caps & sq) p->hash ^= d->caps[i];
        sq <<= 1;
    }
}

u32 slt_reduce(int n, int h) {
    u32 stacks = 0;
    u8 stack = 1;

    for (int i = 0; i < h - 1; i++) {
        if (n & (1 << i)) {
            stacks = (stacks << 4) | stack;
            stack = 1;
        } else stack += 1;
    }

    return stacks << 4 | stack;
}

slides_lt slt_fill() {
    slides_lt lt;
    int k = 0;

    for (int h = 1; h <= 8; h++) {
        lt.indexes[h - 1] = k;

        for (int i = 0; i < 1 << (h - 1); i++) {
            lt.slides[k] = slt_reduce(i, h);
            lt.can_crush[k] = (lt.slides[k] & 0xf) == 1;
            lt.lengths[k++] = 1 + __builtin_popcount(i);
        }
    }

    // the ninth element serves as a fixed global upper bound
    // and to avoid overflows (or an if) in search_slides_atdir
    lt.indexes[8] = SLT_LENGTH;

    return lt;
}

direction direction_o(direction d) {
    return d == EAST ? WEST : d == SOUTH ? NORTH : d == WEST ? EAST : SOUTH;
}

// TODO: experiment with pre-loaded walk results as early
//       takes showed some improvement in performance
int walk(u8 origin, direction d, u8 size) {
    if (d == NORTH) return (origin >= size * (size - 1)) ? -1 : origin + size;
    else if (d == EAST) return ((origin + 1) % size == 0) ? -1 : origin + 1;
    else if (d == SOUTH) return (origin < size) ? -1 : origin - size;
    else if (d == WEST) return (origin % size == 0) ? -1 : origin - 1;
    else return -1;
}

// position and slt are passed by reference to reduce call time
int search_slides_atdir(slide *buffer, position const *p, u8 origin, direction d,
                        slides_lt const *slt) {
    // find the amount of free cells available for the slide
    int length = 0, last = walk(origin, d, p->size);
    u64 mask = ~(p->caps | p->walls);
    while (last != -1 && mask & 1ull << last) {
        last = walk(last, d, p->size);
        length++;
    }

    // height of the legally movable (sub)stack
    int h = (p->heights[origin] < p->size) ? p->heights[origin] : p->size;

    // check if flattening is a possibility
    int can_crush = last != -1 && p->caps & 1ull << origin && p->walls & 1ull << last;

    // filter available slide configurations
    int k = 0, index, upbound = slt->indexes[h], flattens;
    for (index = 0; index < upbound; index++) {
        // this is ugly, but at least it doesn't check the flattening
        // conditions every iteration
        if ((flattens = slt->lengths[index] <= length) ||
            (can_crush && slt->lengths[index] == length + 1 && slt->can_crush[index]))
            buffer[k++] = (slide){
                .origin = origin,
                .direction = d,
                .length = slt->lengths[index],
                .stacks = slt->slides[index],
                .flattens = 1 - flattens,
            };
    }

    return k;
}

int search_slides(slide *buffer, position const *p, slides_lt const *slt) {
    // this should be in search_slides_atdir, but it's faster here.
    if (p->mc < SWAP_MOVES) return 0;

    int k = 0;
    for (int i = 0; i < p->size * p->size; i++)
        // checking in the color bitboard ensures also the heights
        // of the square to be greater than 0
        if (p->cbb[p->stp] & 1ull << i) {
            k += search_slides_atdir(buffer + k, p, i, NORTH, slt);
            k += search_slides_atdir(buffer + k, p, i, EAST, slt);
            k += search_slides_atdir(buffer + k, p, i, SOUTH, slt);
            k += search_slides_atdir(buffer + k, p, i, WEST, slt);
        }

    return k;
}

position do_slide(position p, slide s, zobrist_data *zd) {
    // last cell of the slide
    int target = s.origin + s.length * (s.direction == EAST    ? 1
                                        : s.direction == SOUTH ? -p.size
                                        : s.direction == WEST  ? -1
                                                               : p.size);

    // modifiers
    u64 os = 1ull << s.origin, ts = 1ull << target;
    if (p.caps & os) p.caps = (p.caps & ~os) | ts;
    if (p.walls & os) p.walls = (p.walls & ~os) | ts;
    if (s.flattens) p.walls &= ~ts;

    int i = 0, color, ch = 0;
    direction od = direction_o(s.direction);
    while (target != s.origin) {
        // get the height of the current drop
        int h = s.stacks >> 4 * (s.length - i - 1) & 0xf;
        ch += h;

        // update hash (for the added elements on the target)
        for (int i = p.heights[target]; i < p.heights[target] + h; i++)
            p.hash ^= zd->stacks[ZOBRIST_STACK_HEIGHT_THRESH * target + i];

        // update target
        // TODO: here on 8x8 it can overflow, check and avoid. __int128?
        p.stacks[target] <<= h;
        p.stacks[target] |= p.stacks[s.origin] & ((1ull << h) - 1);
        p.heights[target] += h;

        // update origin
        p.stacks[s.origin] >>= h;
        p.heights[s.origin] -= h;

        // update target color
        ts = 1ull << target;
        color = p.stacks[target] & 1;
        p.cbb[color] |= ts;
        p.cbb[1 - color] &= ~ts;

        // walk target
        target = walk(target, od, p.size);
        i++;
    }

    // update hash (for the removed elements on the origin)
    for (int i = p.heights[s.origin] + ch; i >= p.heights[s.origin]; i--)
        p.hash ^= zd->stacks[ZOBRIST_STACK_HEIGHT_THRESH * s.origin + i];

    // pdate origin color
    if (p.heights[s.origin] == 0) {
        p.cbb[0] &= ~os;
        p.cbb[1] &= ~os;
    } else {
        color = p.stacks[s.origin] & 1;
        p.cbb[color] |= os;
        p.cbb[1 - color] &= ~os;
    }

    // mc, stp
    p.stp = 1 - p.stp;
    p.mc += 1;

    return p;
}

int search_placements(placement *buffer, position const *p) {
    int k = 0;

    player stp = p->mc < SWAP_MOVES ? 1 - p->stp : p->stp;
    uint64_t fmask = p->cbb[0] | p->cbb[1];

    for (int i = 0; i < p->size * p->size; i++) {
        if ((fmask & 1ull << i) == 0) {
            // flats
            if (p->reserves[stp][0])
                buffer[k++] = (placement){.at = i, .piece = stp | FLAT};

            // walls. can be placed only after the second turn
            if (p->reserves[stp][0] && p->mc >= SWAP_MOVES)
                buffer[k++] = (placement){.at = i, .piece = stp | WALL};

            // caps. can be placed only after the second turn
            if (p->reserves[stp][1] && p->mc >= SWAP_MOVES)
                buffer[k++] = (placement){.at = i, .piece = stp | CAP};
        }
    }

    return k;
}

position do_placement(position p, placement pl, zobrist_data *zd) {
    player color = pl.piece & 1;

    // stack and height
    p.stacks[pl.at] = color;
    p.heights[pl.at] = 1;

    // color bitmaps
    uint64_t sq = 1ull << pl.at;
    p.cbb[color] |= sq;

    // modifiers bitmaps
    if (pl.piece & WALL) p.walls |= sq;
    if (pl.piece & CAP) p.caps |= sq;

    // reserves, mc, stp
    p.reserves[color][(pl.piece & CAP) == CAP] -= 1;
    p.stp = 1 - p.stp;
    p.mc += 1;

    // hash
    p.hash ^= zd->stacks[ZOBRIST_STACK_HEIGHT_THRESH * pl.at];

    return p;
}

uint64_t perft(position p, int depth, slides_lt const *slt, zobrist_data *zd) {
    if (depth == 0) return 1;
    uint64_t nodes = 0;

    // check game endings
    endstatus status = check_ending(&p, 0);
    if (status.ended) return 1;

    // load moves
    // TODO: while placements are capped at 196 (64*3), what is the max amount of slides?
    placement pb[PLACEMENTS_MAX_AMOUNT];
    slide sb[500];
    int np = search_placements(pb, &p);
    int ns = search_slides(sb, &p, slt);

    // bulk perft
    if (depth == 1) return np + ns;

    for (int i = 0; i < np; i++)
        nodes += perft(do_placement(p, pb[i], zd), depth - 1, slt, zd);
    for (int i = 0; i < ns; i++)
        nodes += perft(do_slide(p, sb[i], zd), depth - 1, slt, zd);

    return nodes;
}

u8 _search_road_r(position const *p, uint8_t origin, uint8_t i, uint64_t *history) {
    // check and set history
    if (*history & (1ull << i)) return 0;
    *history |= 1ull << i;

    // check if the current index links a road
    if ((origin / p->size == 0 && i / p->size == p->size - 1) ||
        (origin % p->size == 0 && i % p->size == p->size - 1))
        return 1;

    // spread neighbours
    int next_sq, color_check;
    for (direction d = NORTH; d <= WEST; d++) {
        next_sq = walk(i, d, p->size);
        color_check = p->cbb[p->stp] & ~p->walls & 1ull << next_sq;
        if (next_sq != -1 && color_check && _search_road_r(p, origin, next_sq, history))
            return 1;
    }

    return 0;
}

u8 search_road(position const *p) {
    int color_check;

    // vertical roads
    uint64_t history = 0;
    for (int i = 0; i < p->size; i++) {
        color_check = p->cbb[p->stp] & ~p->walls & 1ull << i;
        if (color_check && _search_road_r(p, i, i, &history)) return 1;
    }

    // horizontal roads
    history = 0;
    for (int i = 0; i < p->size; i++) {
        color_check = p->cbb[p->stp] & ~p->walls & 1ull << i * p->size;
        if (color_check && _search_road_r(p, i * p->size, i * p->size, &history))
            return 1;
    }

    return 0;
}

endstatus check_ending(position const *p, int komi) {
    // road
    if (search_road(p)) return (endstatus){.ended = 1, .ending = ROAD, .winner = p->stp};

    // reserves or filled board
    if ((p->reserves[0][0] == 0 && p->reserves[0][1] == 0) ||
        (p->reserves[1][0] == 0 && p->reserves[1][1] == 0) ||
        (p->cbb[0] | p->cbb[1]) == 0xffffffffffffffff) {
        int popc_w = __builtin_popcount(p->cbb[0] & ~p->caps & ~p->walls);
        int popc_b = __builtin_popcount(p->cbb[1] & ~p->caps & ~p->walls) + komi;
        if (popc_b == popc_w) return (endstatus){.ended = 1, .ending = TIE};
        return (endstatus){
            .ended = 1,
            .ending = FLATWIN,
            .winner = popc_w > popc_b ? WHITE : BLACK,
        };
    }

    return (endstatus){.ended = 0};
}

int search_moves(move *buffer, position const *p, slides_lt const *slt) {
    placement plb[PLACEMENTS_MAX_AMOUNT];
    slide slb[500];  // TODO: SLIDES_MAX_AMOUNT

    // load moves
    int plc = search_placements(plb, p);
    int slc = search_slides(slb, p, slt);

    // move moves into buffer
    for (int k = 0; k < plc; k++) buffer[k] = (move){.t = PLACEMENT, .p = plb[k]};
    for (int k = 0; k < slc; k++) buffer[k + plc] = (move){.t = SLIDE, .s = slb[k]};

    return plc + slc;
}

position do_move(position p, move mv, zobrist_data *zd) {
    switch (mv.t) {
        case PLACEMENT: return do_placement(p, mv.p, zd);
        case SLIDE: return do_slide(p, mv.s, zd);
    }

    return p;
}