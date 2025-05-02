#include "perft.hpp"
#include "position.hpp"
#include <cassert>
#include <iostream>

int main() {
    Position pos;
    pos.loadFEN("startpos");
    assert(perft(pos, 1) == 20);
    std::cout<<"perft(1)=20 ✔\n";
    assert(perft(pos, 2) == 400);
    std::cout<<"perft(2)=400 ✔\n";
    // Depth 3 or higher may be slow
    std::cout<<"Perft basic tests passed.\n";
    return 0;
}
