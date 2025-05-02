// engine/tests/test_encoder.cpp
#include "encoder.hpp"
#include "position.hpp"
#include <cassert>
#include <iostream>

int main() {
    Position pos;
    pos.loadFEN("startpos");
    auto tensor = BitboardEncoder::encode(pos);

    // Count total bits — should be 32 pieces on startpos
    int sum = 0;
    for (auto b : tensor) sum += b;
    std::cout << "Total pieces encoded: " << sum << "\n";
    assert(sum == 32);

    // Spot‑check: white pawns (channel 0) = 8
    int wp = 0;
    for (int i = 0; i < 64; ++i) wp += tensor[0 * 64 + i];
    std::cout << "White pawns: " << wp << "\n";
    assert(wp == 8);

    std::cout << "Encoder test passed.\n";
    return 0;
}
