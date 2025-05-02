#pragma once
#include <array>
#include "position.hpp"

class BitboardEncoder {
public:
    // Returns a flat 12×64 array: channel-major (piece_type × 64 squares)
    static std::array<uint8_t, 12 * 64> encode(const Position& pos);
};
