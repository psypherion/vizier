#include "encoder.hpp"


std::array<uint8_t, 12 * 64> BitboardEncoder::encode(const Position& pos) {
    std::array<uint8_t, 12 * 64> out{};
    // For each piece-channel
    for (int ch = 0; ch < 12; ++ch) {
        uint64_t bb = pos.bitboards[ch];
        // For each square index (0 = a1, 63 = h8)
        for (int sq = 0; sq < 64; ++sq) {
            out[ch * 64 + sq] = (bb >> sq) & 1;
        }
    }
    return out;
}
