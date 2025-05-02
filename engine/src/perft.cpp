#include "perft.hpp"
#include "movegen.hpp"

uint64_t perft(Position& pos, int depth) {
    if (depth == 0) return 1;
    uint64_t nodes = 0;
    auto moves = MoveGen::generate(pos);
    for (auto m : moves) {
        Position next = pos;            // copy
        // apply move (very naive):
        // remove any piece on 'to', move piece bit in bitboards[fromChannel]
        for (int ch = 0; ch < 12; ++ch) {
            uint64_t fromMask = 1ULL << m.from;
            if (next.bitboards[ch] & fromMask) {
                next.bitboards[ch] &= ~fromMask;
                next.bitboards[ch] |= (1ULL << m.to);
                break;
            }
        }
        next.whiteToMove = !pos.whiteToMove;
        nodes += perft(next, depth-1);
    }
    return nodes;
}
