#include "position.hpp"
int main() {
  Position p; p.loadFEN("startpos");
  p.printBoard();
  return 0;
}
