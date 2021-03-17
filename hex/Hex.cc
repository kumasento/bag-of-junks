#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

// Hex game:
// - board of 11 x 11.
// - each slot is a hexagon
// - two players
// - win: one player forms a path between its two ends.

// Coordinate:
using Pos = std::pair<int, int>;

// Piece:
enum class Piece { NONE = 0, RED, BLUE };

// Board:
// We use an xyz-coordinate system. Given an 11 x 11 board, there will be a
// [11][11][11] array to hold its pieces.
// e.g., board[x][y][z] is adjacent to board[x+1][y][z], board[x-1][y][z], y+-1,
// z+-1.

// Can two coordinates refer to the same thing?
//          (2,0,0)
//         /       \
//       (1,0,0) - (1,1,0)
//      /       \  /     \
//  (0,0,0) - (0,1,0) - (0,2,0)
//
//              ^
//              |
//   This node can be (0,1,0) or (1,0,1)

// How many pieces will there be on the board? Still 11x11.
//
//    x x x x
//   x x x x
//  x x x x

class Board {
  Piece pieces[11][11];

 public:
  Board() { reset(); }

  void reset() {
    for (int i = 0; i < 11; i++)
      for (int j = 0; j < 11; j++) {
        pieces[i][j] = Piece::NONE;
      }
  }

  std::vector<Pos> neighbours(Pos pos) const;

  Piece& at(Pos pos);

  /// Find pieces on the starting edge (x = 0 for BLUE, y = 0 for RED).
  std::vector<Piece> startPieces(Piece piece) const;

  static bool inBound(Pos pos);

  friend std::ostream& operator<<(std::ostream& os, const Board& board);
};

Piece& Board::at(Pos pos) { return pieces[pos.first][pos.second]; }

std::vector<Pos> Board::neighbours(Pos pos) const {
  std::vector<Pos> poses;

  if (pos.first >= 1 && pos.second <= 9)
    poses.push_back(Pos(pos.first - 1, pos.second + 1));
  if (pos.first <= 9 && pos.second >= 1)
    poses.push_back(Pos(pos.first + 1, pos.second - 1));
  if (pos.first >= 1) poses.push_back(Pos(pos.first - 1, pos.second));
  if (pos.first <= 9) poses.push_back(Pos(pos.first + 1, pos.second));
  if (pos.second >= 1) poses.push_back(Pos(pos.first, pos.second - 1));
  if (pos.second <= 9) poses.push_back(Pos(pos.first, pos.second + 1));

  return poses;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
  std::cout << std::endl;
  for (int i = 0; i < 11; i++) {
    std::cout << std::setw(3) << i + 1 << "  ";

    for (int j = 0; j < i; j++) std::cout.put(' ');
    for (int j = 0; j < 11; j++) {
      std::cout.put(static_cast<int>(board.pieces[i][j]) + '*');
      std::cout.put(' ');
    }
    std::cout << std::endl;
  }
  return os;
}

bool Board::inBound(Pos pos) {
  return pos.first >= 0 && pos.first < 11 && pos.second >= 0 && pos.second < 11;
}

// Game:
// - place(pos, player)
// - gameover()
// - isWinning(player)

class HexGame {
 public:
  void place(Pos pos, Piece piece);

  bool gameOver() const { return isGameOver; }

  bool isWinning(Piece piece);

  Board& getBoard() { return board; }

  void reset() {
    board.reset();
    isGameOver = false;
  }

 private:
  Board board;

  bool isGameOver = false;
};

void HexGame::place(Pos pos, Piece piece) {
  std::cout << "Player " << (piece == Piece::RED ? "RED " : "BLUE")
            << " to place at (" << pos.first << ", " << pos.second << ")"
            << std::endl;
  assert(!isGameOver);
  assert(Board::inBound(pos));
  assert(board.at(pos) == Piece::NONE);

  board.at(pos) = piece;

  if (isWinning(piece)) {
    std::cout << "Player " << (piece == Piece::RED ? "RED " : "BLUE")
              << " wins the game!" << std::endl;
    isGameOver = true;
  }
}

bool HexGame::isWinning(Piece piece) {
  assert(piece != Piece::NONE);

  std::queue<Pos> worklist;
  std::set<Pos> visited;

  if (piece == Piece::RED) {
    for (int x = 0; x < 11; x++)
      if (board.at(Pos{x, 0}) == piece) worklist.push(Pos{x, 0});
  } else {
    for (int y = 0; y < 11; y++)
      if (board.at(Pos{0, y}) == piece) worklist.push(Pos{0, y});
  }

  while (!worklist.empty()) {
    auto front = worklist.front();
    worklist.pop();

    if ((piece == Piece::RED && front.second == 10) ||
        (piece == Piece::BLUE && front.first == 10))
      return true;

    visited.insert(front);

    for (auto pos : board.neighbours(front))
      if (board.at(pos) == piece && !visited.count(pos)) worklist.push(pos);
  }

  return false;
}

int main() {
  std::cout << "Welcome to the Hex Game!" << std::endl;

  Board board;
  std::cout << board << std::endl;

  // corners
  assert(board.neighbours(Pos(0, 0)).size() == 2);
  assert(board.neighbours(Pos(10, 0)).size() == 3);
  assert(board.neighbours(Pos(0, 10)).size() == 3);
  assert(board.neighbours(Pos(10, 10)).size() == 2);
  // edges
  assert(board.neighbours(Pos(0, 5)).size() == 4);
  assert(board.neighbours(Pos(5, 0)).size() == 4);
  assert(board.neighbours(Pos(10, 5)).size() == 4);
  assert(board.neighbours(Pos(5, 10)).size() == 4);
  // internals
  assert(board.neighbours(Pos(5, 5)).size() == 6);

  for (auto pos : board.neighbours(Pos(5, 5))) board.at(pos) = Piece::RED;
  for (auto pos : board.neighbours(Pos(0, 0))) board.at(pos) = Piece::BLUE;
  for (auto pos : board.neighbours(Pos(7, 10))) board.at(pos) = Piece::RED;
  std::cout << board << std::endl;

  board.reset();

  HexGame game;
  for (int i = 0; i < 11; i++) game.place(Pos(0, i), Piece::RED);
  std::cout << game.getBoard() << std::endl;

  assert(game.isWinning(Piece::RED));

  game.reset();
  for (int i = 0; i < 11; i++) game.place(Pos(i, 0), Piece::BLUE);
  std::cout << game.getBoard() << std::endl;
  assert(game.isWinning(Piece::BLUE));

  game.reset();

  std::default_random_engine dre;
  std::uniform_int_distribution<int> dist(0, 10);

  int step = 0;
  while (!game.gameOver()) {
    Piece piece = static_cast<Piece>(static_cast<int>(Piece::RED) + (step % 2));

    Pos pos{dist(dre), dist(dre)};
    while (game.getBoard().at(pos) != Piece::NONE) pos = {dist(dre), dist(dre)};

    game.place(pos, piece);

    step++;
  }
  std::cout << game.getBoard() << std::endl;
}
