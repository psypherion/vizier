#pragma once
#include <cstdint>

struct Move {
    uint8_t from;    // 0–63
    uint8_t to;      // 0–63
    uint8_t promo;   // 0 = none, 1=Knight,2=Bishop,3=Rook,4=Queen
    Move(uint8_t f, uint8_t t, uint8_t p = 0) 
      : from(f), to(t), promo(p) {}
};
