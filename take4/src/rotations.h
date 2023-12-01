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
