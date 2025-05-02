#include "position.hpp"
#include <sstream>
#include <vector>
#include <cassert>
#include <cctype>

static const std::string STARTPOS_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static int fileCharToIndex(char file) {
    return file - 'a';  // 'a'→0, … 'h'→7
}

void Position::loadFEN(const std::string& fen) {
    std::string f = (fen == "startpos") ? STARTPOS_FEN : fen;
    std::istringstream iss(f);

    std::string board, stm, cast, ep;
    iss >> board >> stm >> cast >> ep;
    // ignore half/fullmove clocks for now

    // Clear bitboards
    bitboards.fill(0);
    occupancy[WHITE] = occupancy[BLACK] = allOccupancy = 0;

    // Parse side to move
    whiteToMove = (stm == "w");

    // Parse castling rights
    canCastleK = cast.find('K') != std::string::npos;
    canCastleQ = cast.find('Q') != std::string::npos;
    canCastlek = cast.find('k') != std::string::npos;
    canCastleq = cast.find('q') != std::string::npos;

    // Parse en passant square
    if (ep == "-") {
        enPassantSquare = -1;
    } else {
        int file = fileCharToIndex(ep[0]);
        int rank = ep[1] - '1';
        enPassantSquare = rank * 8 + file;
    }

    // Parse piece placement
    int rank = 7, file = 0;
    for (char c : board) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(c)) {
            file += c - '0';
        } else {
            int pieceIndex = -1;
            switch (c) {
                case 'P': pieceIndex = 0; break;
                case 'N': pieceIndex = 1; break;
                case 'B': pieceIndex = 2; break;
                case 'R': pieceIndex = 3; break;
                case 'Q': pieceIndex = 4; break;
                case 'K': pieceIndex = 5; break;
                case 'p': pieceIndex = 6; break;
                case 'n': pieceIndex = 7; break;
                case 'b': pieceIndex = 8; break;
                case 'r': pieceIndex = 9; break;
                case 'q': pieceIndex = 10; break;
                case 'k': pieceIndex = 11; break;
                default: assert(false && "Unknown piece");
            }
            int sq = rank * 8 + file;
            bitboards[pieceIndex] |= (1ULL << sq);
            file++;
        }
    }

    // Recalculate occupancies
    for (int i = 0; i < 6; ++i) {
        occupancy[WHITE] |= bitboards[i];
        occupancy[BLACK] |= bitboards[i + 6];
    }
    allOccupancy = occupancy[WHITE] | occupancy[BLACK];
}

void Position::printBoard() const {
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            char piece = '.';
            for (int i = 0; i < 12; ++i) {
                if (bitboards[i] & (1ULL << sq)) {
                    static const char sym[12] = {
                        'P','N','B','R','Q','K','p','n','b','r','q','k'
                    };
                    piece = sym[i];
                    break;
                }
            }
            std::cout << piece << ' ';
        }
        std::cout << '\n';
    }
    std::cout << (whiteToMove ? "White" : "Black") << " to move\n";
}

// === [OPTIONAL LATER] ===
// bool Position::squareAttacked(int sq, Color by) const {}
// bool Position::isKingInCheck(Color side) const {}

void Position::makeMove(const Move& move) {
    // Save current state to history
    GameState state = {
        .bitboards = {},
        .occupancy = {occupancy[WHITE], occupancy[BLACK]},
        .allOccupancy = allOccupancy,
        .whiteToMove = whiteToMove,
        .canCastleK = canCastleK,
        .canCastleQ = canCastleQ,
        .canCastlek = canCastlek,
        .canCastleq = canCastleq,
        .enPassantSquare = enPassantSquare,
    };
    std::copy(bitboards.begin(), bitboards.end(), state.bitboards);
    history.push_back(state);

    uint64_t fromBB = 1ULL << move.from;
    uint64_t toBB = 1ULL << move.to;

    // Remove piece from source square
    bitboards[move.piece] &= ~fromBB;

    // Handle capture
    if (move.captured != -1) {
        bitboards[move.captured] &= ~toBB;
    }

    // Handle en passant
    if (move.isEnPassant) {
        int capSq = whiteToMove ? move.to - 8 : move.to + 8;
        bitboards[whiteToMove ? 6 : 0] &= ~(1ULL << capSq); // remove pawn
        occupancy[!whiteToMove] &= ~(1ULL << capSq);
    }

    // Handle promotion
    if (move.promotion != -1) {
        bitboards[move.promotion] |= toBB;
    } else {
        bitboards[move.piece] |= toBB;
    }

    // Castling (king already moved above)
    if (move.isCastling) {
        if (move.to == 6) { // White kingside
            bitboards[3] &= ~(1ULL << 7);
            bitboards[3] |= (1ULL << 5);
        } else if (move.to == 2) { // White queenside
            bitboards[3] &= ~(1ULL << 0);
            bitboards[3] |= (1ULL << 3);
        } else if (move.to == 62) { // Black kingside
            bitboards[9] &= ~(1ULL << 63);
            bitboards[9] |= (1ULL << 61);
        } else if (move.to == 58) { // Black queenside
            bitboards[9] &= ~(1ULL << 56);
            bitboards[9] |= (1ULL << 59);
        }
    }

    // Update castling rights (if rook or king moves)
    if (move.piece == 5) canCastleK = canCastleQ = false;
    if (move.piece == 11) canCastlek = canCastleq = false;
    if (move.from == 0 || move.to == 0) canCastleQ = false;
    if (move.from == 7 || move.to == 7) canCastleK = false;
    if (move.from == 56 || move.to == 56) canCastleq = false;
    if (move.from == 63 || move.to == 63) canCastlek = false;

    // En passant square
    if (move.piece == 0 && move.to - move.from == 16) {
        enPassantSquare = move.from + 8;
    } else if (move.piece == 6 && move.from - move.to == 16) {
        enPassantSquare = move.from - 8;
    } else {
        enPassantSquare = -1;
    }

    // Update occupancies
    occupancy[WHITE] = occupancy[BLACK] = 0;
    for (int i = 0; i < 6; ++i) {
        occupancy[WHITE] |= bitboards[i];
        occupancy[BLACK] |= bitboards[i + 6];
    }
    allOccupancy = occupancy[WHITE] | occupancy[BLACK];

    // Switch side
    whiteToMove = !whiteToMove;
}

void Position::undoMove() {
    assert(!history.empty());
    GameState prev = history.back();
    history.pop_back();

    std::copy(std::begin(prev.bitboards), std::end(prev.bitboards), bitboards.begin());
    occupancy[WHITE] = prev.occupancy[WHITE];
    occupancy[BLACK] = prev.occupancy[BLACK];
    allOccupancy = prev.allOccupancy;
    whiteToMove = prev.whiteToMove;
    canCastleK = prev.canCastleK;
    canCastleQ = prev.canCastleQ;
    canCastlek = prev.canCastlek;
    canCastleq = prev.canCastleq;
    enPassantSquare = prev.enPassantSquare;
}

