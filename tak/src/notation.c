#include "notation.h"

#include "tak.h"

const tps_lexicon TPS_STD = {
    .row_s = '/',
    .cell_s = ',',
    .section_s = ' ',
    .wall = 'S',
    .cap = 'C',
    .jump = 'x',
    .white = '1',
    .black = '2',
};

int tps_getsize(char *tps, tps_lexicon lexicon) {
    int size = 1;
    while (*tps != lexicon.section_s) size += lexicon.row_s == *tps++;
    return size;
}

int mirror_index(int index, int size) {
    return index % size + (size - index / size - 1) * size;
}

// TODO: benchmark against a strtok_r version
position tps2position(char *tps, tps_lexicon lexicon, zobrist_data *zd) {
    position p = new_position(tps_getsize(tps, lexicon));

    // main section
    player color = WHITE;
    int index = 0, mindex = mirror_index(0, p.size), jumping = 0;
    u64 square = 0;

    for (; *tps != lexicon.section_s; tps += sizeof(char)) {
        // handle empty cell
        if (*tps == lexicon.jump) jumping = 1;

        // handle multiple empty cells
        else if (jumping && *tps >= '0' && *tps <= '9') jumping = *tps - '0';

        // handle color indicator (hence stone)
        // we don't handle previous jump here, since there shouldn't be the need to
        // as after a jump indicator there should be only a number (handled above)
        // or a separator
        else if (*tps == lexicon.white || *tps == lexicon.black) {
            color = *tps == lexicon.white ? WHITE : BLACK;
            mindex = mirror_index(index, p.size);

            // update stacks
            p.stacks[mindex] = (p.stacks[mindex] << 1) | color;
            p.heights[mindex] += 1;

            // update color bitboards
            square = 1ull << mindex;
            p.cbb[color] |= square;
            p.cbb[1 - color] &= ~square;

            // update flats count
            p.reserves[color][0] -= 1;
        }

        // with separators and modifiers we need to handle
        // unhandled jumps as well
        else {
            // resolve jumps
            if (jumping) {
                index += jumping - 1;
                jumping = 0;
            }

            // resolve modifiers
            // here the mindex should already be set, since all modifiers
            // should be after a color indicator
            if (*tps == lexicon.wall) p.walls |= 1ull << mindex;
            else if (*tps == lexicon.cap) {
                p.caps |= 1ull << mindex;

                // update stones count
                p.reserves[color][1] -= 1;
                p.reserves[color][0] += 1;
            }

            // update index when we find a separator
            else if (*tps == lexicon.cell_s || *tps == lexicon.row_s)
                index += 1;
        }
    }

    // turn identifier
    while (*tps == lexicon.section_s) tps++;
    p.stp = strtol(tps, &tps, 10) - 1;

    // size to play section
    while (*tps == lexicon.section_s) tps++;
    p.mc = strtol(tps, &tps, 10) - 1;
    p.mc = p.mc * 2 + p.stp;

    // hash
    zobrist(&p, zd);

    return p;
}

int position2tps(char *buffer, position p, tps_lexicon lexicon) {
    int k = 0;

    // main section
    int jumping = 0;

    for (int row = p.size - 1; row >= 0; row--) {
        for (int column = 0; column < p.size; column++) {
            int index = row * p.size + column;

            if (p.heights[index] == 0) jumping += 1;
            else {
                // handle previous jumps
                if (jumping) {
                    // add cell separator if needed
                    if (k > 0 && buffer[k - 1] != lexicon.row_s) {
                        buffer[k++] = lexicon.cell_s;
                    }

                    buffer[k++] = lexicon.jump;

                    // add jumps count if needed
                    if (jumping > 1) buffer[k++] = '0' + jumping;
                    jumping = 0;
                }

                // cell separator
                if (column > 0) buffer[k++] = lexicon.cell_s;

                // append stack
                uint64_t sq;  // this could be a int, but later it's used as a uint64
                for (int h = p.heights[index] - 1; h >= 0; h--) {
                    sq = p.stacks[index] & 1ull << h;
                    buffer[k++] = sq ? lexicon.black : lexicon.white;
                }

                // modifiers
                sq = 1ull << index;
                if (sq & p.walls) buffer[k++] = lexicon.wall;
                if (sq & p.caps) buffer[k++] = lexicon.cap;
            }
        }

        // handle remaining jumps
        if (jumping) {
            // if we jump for less than the full row, we can be sure that there
            // were cells previously, so we need the separator
            if (jumping < p.size) buffer[k++] = ',';

            buffer[k++] = lexicon.jump;
            if (jumping > 1) buffer[k++] = '0' + jumping;
            jumping = 0;
        }

        // row separator
        if (row > 0) buffer[k++] = lexicon.row_s;
    }

    // move count sections
    int rv = sprintf(buffer + k, "%c%d%c%d", lexicon.section_s, p.stp + 1,
                     lexicon.section_s, p.mc / 2 + 1);
    return rv < 0 ? rv : k + rv;
}

const ptn_lexicon PTN_STD = {
    .wall = 'S',
    .cap = 'C',
    .rows = {'1', '2', '3', '4', '5', '6', '7', '8'},
    .columns = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    .directions = {'+', '>', '-', '<'},  // north, east, south, west
    .flattens = '*',
};

int placement2ptn(char *buffer, placement p, u8 size, ptn_lexicon lexicon) {
    int k = 0;

    // piece modifiers
    if (p.piece & WALL) buffer[k++] = lexicon.wall;
    if (p.piece & CAP) buffer[k++] = lexicon.cap;

    // square
    buffer[k++] = lexicon.columns[p.at % size];
    buffer[k++] = lexicon.rows[p.at / size];

    buffer[k++] = '\0';
    return k;
}

int slide2ptn(char *buffer, slide s, u8 size, ptn_lexicon lexicon) {
    int count = 0, k = 0;
    for (int i = 0; i < s.length; i++) count += (s.stacks >> 4 * i) & 0xf;

    // count (can't be more than 8)
    if (count > 1) buffer[k++] = '0' + count;

    // square, direction
    buffer[k++] = lexicon.columns[s.origin % size];
    buffer[k++] = lexicon.rows[s.origin / size];
    buffer[k++] = lexicon.directions[s.direction];

    // drops (each can't be more than 8)
    if (s.length > 1)
        for (int i = 0; i < s.length; i++) buffer[k++] = '0' + (s.stacks >> 4 * i & 0xf);

    // flattens
    if (s.flattens) buffer[k++] = lexicon.flattens;

    buffer[k++] = '\0';
    return k;
}

int move2ptn(char *buffer, move m, u8 size, ptn_lexicon lexicon) {
    switch (m.t) {
        case PLACEMENT: return placement2ptn(buffer, m.p, size, lexicon);
        case SLIDE: return slide2ptn(buffer, m.s, size, lexicon);
    }

    return 0;
}

move ptn2move(char *ptn, u8 size, player stp, ptn_lexicon lexicon) {
    if (strlen(ptn) == 2 || *ptn == lexicon.cap || *ptn == lexicon.wall) {
        // placement
        placement p = {0};

        // piece
        if (*ptn == lexicon.cap) {
            p.piece = CAP | stp;
            ptn++;
        } else if (*ptn == lexicon.wall) {
            p.piece = WALL | stp;
            ptn++;
        } else p.piece = FLAT | stp;

        // square
        int row = 0, column = 0;
        while (column < 8 && lexicon.columns[column] != *ptn) column++;
        while (row < 8 && lexicon.rows[row] != *(ptn + 1)) row++;
        p.at = column + row * size;

        return (move){.p = p, .t = PLACEMENT};
    } else {  // slide
        slide s = {.length = 0, .stacks = 0};

        // count
        int count = 1;
        if (*ptn >= '1' && *ptn <= '8') count = *ptn++ - '0';

        // square, direction
        int row = 0, column = 0, d = 0;
        while (column < 8 && lexicon.columns[column] != *ptn) column++;
        while (row < 8 && lexicon.rows[row] != *(ptn + 1)) row++;
        while (d < 4 && lexicon.directions[d] != *(ptn + 2)) d++;
        s.origin = column + row * size;
        s.direction = d;
        ptn += 3;

        // drops
        int sum = 0, drop;
        while (*ptn >= '1' && *ptn <= '8') {
            drop = *ptn - '0';
            s.stacks = (s.stacks << 4) | drop;
            s.length += 1;
            sum += drop;
            ptn++;
        }

        // if no drops, drop all the count on the first cell
        if (sum == 0) {
            s.stacks = count;
            s.length = 1;
        }

        // flattens
        if (*ptn == lexicon.flattens) s.flattens = 1;

        return (move){.s = s, .t = SLIDE};
    }
}

position ptn2position(position p, char **ptn, int n, ptn_lexicon lexicon,
                      zobrist_data *zd) {
    for (int i = 0; i < n; i++)
        p = do_move(p, ptn2move(ptn[i], p.size, p.stp, lexicon), zd);
    return p;
}