#include "movegen.hpp"
#include <cassert>

static constexpr int rookDirs[4]   = {+1, -1, +8, -8};
static constexpr int bishopDirs[4] = {+9, -9, +7, -7};
static constexpr int knightJumps[8]= {+17,+15,+10,+6,-6,-10,-15,-17};
static constexpr int kingMoves[8]  = {+1, -1, +8, -8, +9, -9, +7, -7};

inline bool onBoard(int sq, int delta) {
    int f = sq % 8, r = sq / 8;
    int nf = (sq + delta) % 8, nr = (sq + delta) / 8;
    return (0 <= sq + delta && sq + delta < 64) 
        && (abs(f - nf) <= 2) && (abs(r - nr) <= 2);
}

std::vector<Move> MoveGen::generate(const Position& pos) {
    std::vector<Move> moves;
    bool us = pos.whiteToMove;
    uint64_t ownOcc = us ? pos.occupancy[WHITE] : pos.occupancy[BLACK];
    uint64_t oppOcc = us ? pos.occupancy[BLACK] : pos.occupancy[WHITE];
    uint64_t allOcc = ownOcc | oppOcc;

    int pawnIdx = us ? 0 : 6;
    int knightIdx = us ? 1 : 7;
    int bishopIdx = us ? 2 : 8;
    int rookIdx = us ? 3 : 9;
    int queenIdx = us ? 4 : 10;
    int kingIdx = us ? 5 : 11;

    int dir = us ? +8 : -8;
    int startRank = us ? 1 : 6;
    int promoRank = us ? 6 : 1;
    int epRank = us ? 4 : 3;

    // Pawn moves
    uint64_t pawns = pos.bitboards[pawnIdx];
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;
        int to = sq + dir;
        if (to >= 0 && to < 64 && !(allOcc & (1ULL << to))) {
            if (sq / 8 == promoRank) {
                moves.emplace_back(sq, to, PROMOTION_Q);
                moves.emplace_back(sq, to, PROMOTION_R);
                moves.emplace_back(sq, to, PROMOTION_B);
                moves.emplace_back(sq, to, PROMOTION_N);
            } else {
                moves.emplace_back(sq, to);
                if (sq / 8 == startRank) {
                    int to2 = to + dir;
                    if (!(allOcc & (1ULL << to2)))
                        moves.emplace_back(sq, to2, DOUBLE_PAWN_PUSH);
                }
            }
        }
        // Captures
        for (int d : {dir + 1, dir - 1}) {
            int cap = sq + d;
            if (cap >= 0 && cap < 64 && (oppOcc & (1ULL << cap))) {
                if (sq / 8 == promoRank) {
                    moves.emplace_back(sq, cap, PROMO_CAPTURE_Q);
                    moves.emplace_back(sq, cap, PROMO_CAPTURE_R);
                    moves.emplace_back(sq, cap, PROMO_CAPTURE_B);
                    moves.emplace_back(sq, cap, PROMO_CAPTURE_N);
                } else {
                    moves.emplace_back(sq, cap, CAPTURE);
                }
            }
        }
        // En passant
        if (pos.enPassantSquare != -1) {
            for (int d : {dir + 1, dir - 1}) {
                int cap = sq + d;
                if (cap == pos.enPassantSquare) {
                    moves.emplace_back(sq, cap, EN_PASSANT);
                }
            }
        }
    }

    // Knight moves
    uint64_t knights = pos.bitboards[knightIdx];
    while (knights) {
        int sq = __builtin_ctzll(knights);
        knights &= knights - 1;
        for (int delta : knightJumps) {
            if (!onBoard(sq, delta)) continue;
            int to = sq + delta;
            if (!(ownOcc & (1ULL << to))) {
                MoveFlag flag = (oppOcc & (1ULL << to)) ? CAPTURE : QUIET;
                moves.emplace_back(sq, to, flag);
            }
        }
    }

    // Sliding pieces
    auto slide = [&](uint64_t pieces, const int* dirs, int ndir) {
        while (pieces) {
            int sq = __builtin_ctzll(pieces);
            pieces &= pieces - 1;
            for (int i = 0; i < ndir; ++i) {
                int d = dirs[i], t = sq + d;
                while (t >= 0 && t < 64 && onBoard(t - d, d)) {
                    if (ownOcc & (1ULL << t)) break;
                    MoveFlag flag = (oppOcc & (1ULL << t)) ? CAPTURE : QUIET;
                    moves.emplace_back(sq, t, flag);
                    if (oppOcc & (1ULL << t)) break;
                    t += d;
                }
            }
        }
    };
    slide(pos.bitboards[bishopIdx], bishopDirs, 4);
    slide(pos.bitboards[rookIdx], rookDirs, 4);
    slide(pos.bitboards[queenIdx], bishopDirs, 4);
    slide(pos.bitboards[queenIdx], rookDirs, 4);

    // King moves
    uint64_t kings = pos.bitboards[kingIdx];
    int sq = __builtin_ctzll(kings);
    for (int d : kingMoves) {
        if (!onBoard(sq, d)) continue;
        int to = sq + d;
        if (!(ownOcc & (1ULL << to))) {
            MoveFlag flag = (oppOcc & (1ULL << to)) ? CAPTURE : QUIET;
            moves.emplace_back(sq, to, flag);
        }
    }

    // Castling
    if (us) {
        if (pos.canCastleK && !(allOcc & ((1ULL << 5) | (1ULL << 6)))) {
            moves.emplace_back(4, 6, KING_CASTLE);
        }
        if (pos.canCastleQ && !(allOcc & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3)))) {
            moves.emplace_back(4, 2, QUEEN_CASTLE);
        }
    } else {
        if (pos.canCastlek && !(allOcc & ((1ULL << 61) | (1ULL << 62)))) {
            moves.emplace_back(60, 62, KING_CASTLE);
        }
        if (pos.canCastleq && !(allOcc & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59)))) {
            moves.emplace_back(60, 58, QUEEN_CASTLE);
        }
    }

    return moves;
}
