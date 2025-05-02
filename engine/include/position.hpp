#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <iostream>

enum Color { WHITE = 0, BLACK = 1 };

class Position {
public:
    std::array<uint64_t, 12> bitboards{};  // 6 piece types × 2 colors
    uint64_t occupancy[2] = {0, 0};         // Per color occupancy
    uint64_t allOccupancy = 0ULL;          // Combined

    bool whiteToMove = true;
    bool canCastleK = false, canCastleQ = false;
    bool canCastlek = false, canCastleq = false;

    int enPassantSquare = -1;              // Square index (0-63), -1 if none

    void loadFEN(const std::string& fen);
    void printBoard() const;

    // Helpers
    inline bool squareAttacked(int sq, Color by) const;
    inline bool isKingInCheck(Color side) const;
};


struct Move {
    int from;
    int to;
    int piece;       // 0–11 for P/N/B/R/Q/K/p/n/b/r/q/k
    int captured;    // same encoding, or -1 if none
    int promotion;   // same encoding, or -1 if not a promo
    bool isEnPassant = false;
    bool isCastling = false;
};


void makeMove(const Move& move);
void undoMove(); // we’ll need a history stack

struct GameState {
    uint64_t bitboards[12];
    uint64_t occupancy[2];
    uint64_t allOccupancy;
    bool whiteToMove;
    bool canCastleK, canCastleQ, canCastlek, canCastleq;
    int enPassantSquare;
};
std::vector<GameState> history;
