#pragma once

#include "position.hpp"
#include <vector>

enum MoveFlag {
    QUIET = 0,
    DOUBLE_PAWN_PUSH,
    KING_CASTLE,
    QUEEN_CASTLE,
    CAPTURE,
    EN_PASSANT,
    PROMOTION_N,
    PROMOTION_B,
    PROMOTION_R,
    PROMOTION_Q,
    PROMO_CAPTURE_N,
    PROMO_CAPTURE_B,
    PROMO_CAPTURE_R,
    PROMO_CAPTURE_Q
};

struct Move {
    int from;
    int to;
    MoveFlag flag;

    Move(int f, int t, MoveFlag fl = QUIET) : from(f), to(t), flag(fl) {}
};

class MoveGen {
public:
    static std::vector<Move> generate(const Position& pos);
};
