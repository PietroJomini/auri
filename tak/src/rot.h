#pragma once

#include <stdint.h>

// mirror horizontally a index
//  a b c    c b a
//  d e f -> f e d
//  g h i    i h g
uint8_t mirror_h3(uint8_t n);
uint8_t mirror_h4(uint8_t n);
uint8_t mirror_h5(uint8_t n);
uint8_t mirror_h6(uint8_t n);
uint8_t mirror_h7(uint8_t n);
uint8_t mirror_h8(uint8_t n);

// mirror vertically a index
//  a b c    g h i
//  d e f -> d e f
//  g h i    a b c
uint8_t mirror_v3(uint8_t n);
uint8_t mirror_v4(uint8_t n);
uint8_t mirror_v5(uint8_t n);
uint8_t mirror_v6(uint8_t n);
uint8_t mirror_v7(uint8_t n);
uint8_t mirror_v8(uint8_t n);

// mirror a index along the diagonal
//  a b c    a d g
//  d e f -> b e h
//  g h i    c f i
uint8_t mirror_d3(uint8_t n);
uint8_t mirror_d4(uint8_t n);
uint8_t mirror_d5(uint8_t n);
uint8_t mirror_d6(uint8_t n);
uint8_t mirror_d7(uint8_t n);
uint8_t mirror_d8(uint8_t n);

// mirror a index along the antidiagonal
//  a b c    i f c
//  d e f -> h e b
//  g h i    g d a
uint8_t mirror_a3(uint8_t n);
uint8_t mirror_a4(uint8_t n);
uint8_t mirror_a5(uint8_t n);
uint8_t mirror_a6(uint8_t n);
uint8_t mirror_a7(uint8_t n);
uint8_t mirror_a8(uint8_t n);

// rotate a index by 180 degrees
//  a b c    i h g
//  d e f -> f e d
//  g h i    c b a
uint8_t rotate_3(uint8_t n);
uint8_t rotate_4(uint8_t n);
uint8_t rotate_5(uint8_t n);
uint8_t rotate_6(uint8_t n);
uint8_t rotate_7(uint8_t n);
uint8_t rotate_8(uint8_t n);

// rotate a index by 90 degrees clockwise
//  a b c    g d a
//  d e f -> h e b
//  g h i    i f c
uint8_t rotate_c3(uint8_t n);
uint8_t rotate_c4(uint8_t n);
uint8_t rotate_c5(uint8_t n);
uint8_t rotate_c6(uint8_t n);
uint8_t rotate_c7(uint8_t n);
uint8_t rotate_c8(uint8_t n);

// rotate a index by 90 degrees anticlockwise
//  a b c    c f i
//  d e f -> b e h
//  g h i    a d g
uint8_t rotate_a3(uint8_t n);
uint8_t rotate_a4(uint8_t n);
uint8_t rotate_a5(uint8_t n);
uint8_t rotate_a6(uint8_t n);
uint8_t rotate_a7(uint8_t n);
uint8_t rotate_a8(uint8_t n);

// generic size API
uint8_t mirror_h(uint8_t n, uint8_t s);
uint8_t mirror_v(uint8_t n, uint8_t s);
uint8_t mirror_d(uint8_t n, uint8_t s);
uint8_t mirror_a(uint8_t n, uint8_t s);
uint8_t rotate(uint8_t n, uint8_t s);
uint8_t rotate_c(uint8_t n, uint8_t s);
uint8_t rotate_a(uint8_t n, uint8_t s);

// number of possible rotations of a board, including the "original"
#define N_ROT 8