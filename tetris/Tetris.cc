#include <cassert>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

/**
 * Tetris game simulator.
 *
 * Run ./Tetris <player>
 *
 * It simulates a whole Tetris game. It generates a sequence of tetris blocks,
 * and the player will need to operate on them.
 *
 */

/// Tetriminos.
class Tetriminos {
 public:
  enum Kind { I, O, T, S, Z, J, L };
  /// Not every tetriminos has all these directions.
  enum Direction { UP, DOWN, LEFT, RIGHT };

  using State = std::pair<Kind, Direction>;

  Tetriminos(Kind kind, Direction dir) : kind_(kind), dir_(dir), x_(0), y_(0) {}

  const Kind kind() const { return kind_; }
  const Direction dir() const { return dir_; }

 private:
  Kind kind_;
  Direction dir_;

  /// Coordinates
  // Count from the top left the screen, represent the top-left of the block.
  int x_, y_;
};

std::ostream& operator<<(std::ostream& os, Tetriminos&& t) {
  switch (t.kind()) {
    case Tetriminos::I:
      os << 'I';
      break;
    case Tetriminos::O:
      os << 'O';
      break;
    case Tetriminos::T:
      os << 'T';
      break;
    case Tetriminos::S:
      os << 'S';
      break;
    case Tetriminos::Z:
      os << 'Z';
      break;
    case Tetriminos::J:
      os << 'J';
      break;
    case Tetriminos::L:
      os << 'L';
      break;
    default:
      assert(false);
  }

  os << " ";

  switch (t.dir()) {
    case Tetriminos::UP:
      os << "↑";
      break;
    case Tetriminos::DOWN:
      os << "↓";
      break;
    case Tetriminos::LEFT:
      os << "←";
      break;
    case Tetriminos::RIGHT:
      os << "→";
      break;
    default:
      assert(false);
  }

  return os;
}

/// Player.
class Player;

/// The game.
class Tetris {
 public:
  Tetris() : grid(NUM_ROWS, std::vector<bool>(NUM_COLS, false)) {
    nextTetriminosState = getNextTetriminosState();
  }

  void run() {
    while (true) {
      if (tick % NUM_TICKS_PER_BLOCK_ISSUING) {
        // Issue a new block.
        // The type of the block is chosen randomly.
      }

      tick++;
    }
  }

  // Return the grid value at coord (i, j)
  // May throw exception if the index is out of bound.
  bool at(int i, int j) const { return grid.at(i).at(j); }

  const int num_rows() const { return NUM_ROWS; }
  const int num_cols() const { return NUM_COLS; }

  Tetriminos getNextTetriminos() {
    Tetriminos block(nextTetriminosState.first, nextTetriminosState.second);
    nextTetriminosState = getNextTetriminosState();
    return block;
  }

  Tetriminos::State getNextTetriminosState() const {
    return std::make_pair(
        Tetriminos::Kind(rand() % TETRIMINOS_KINDS.size()),
        Tetriminos::Direction(rand() % TETRIMINOS_DIRECTIONS.size()));
  }

 private:
  /// Config.
  const int NUM_TICKS_PER_BLOCK_ISSUING = 10;
  const int NUM_ROWS = 40;
  const int NUM_COLS = 10;
  const std::vector<Tetriminos::Kind> TETRIMINOS_KINDS{
      Tetriminos::I, Tetriminos::O, Tetriminos::T, Tetriminos::S,
      Tetriminos::Z, Tetriminos::J, Tetriminos::L};
  const std::vector<Tetriminos::Direction> TETRIMINOS_DIRECTIONS{
      Tetriminos::UP, Tetriminos::DOWN, Tetriminos::LEFT, Tetriminos::RIGHT};

  /// Game state
  uint64_t tick;                          /// Current tick.
  std::vector<std::vector<bool>> grid;    /// Occupancy of each grid.
  Tetriminos::State nextTetriminosState;  /// Next block to be emitted.
};

std::ostream& operator<<(std::ostream& os, Tetris& tetris) {
  for (int i = 0; i < tetris.num_rows(); i++) {
    os << std::setw(3) << i + 1;
    for (int j = 0; j < tetris.num_cols(); j++)
      os << (tetris.at(i, j) ? '.' : ' ');
    os << std::endl;
  }
  return os;
}

int main(int argc, char* argv[]) {
  Tetris tetris;

  // std::cout << tetris;
  std::cout << Tetriminos(Tetriminos::L, Tetriminos::UP) << std::endl;

  return 0;
}
