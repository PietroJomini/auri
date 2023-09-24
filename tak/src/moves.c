#include "moves.h"

#include <stdint.h>

#include "position.h"
#include "slides.h"

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
    uint64_t bb = p.stp ? p.black : p.white;
    int n = 0;
    for (int i = 0; i < p.size * p.size; i++)
        if ((1ull << i) & bb) n += slides_at(buffer + n, p, i);
    return n;
}