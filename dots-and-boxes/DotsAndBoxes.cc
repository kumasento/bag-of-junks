#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

#define N 2
#define M 3

enum Pos { LEFT, UP, RIGHT, DOWN };

struct Line {
  int bx, by, id;

  Line(int bx, int by, int id) : bx(bx), by(by), id(id) {}
};

class Board {
  int boxes[N][M][5];  // { pos[4], occupied }
  int scores[2];

 public:
  Board() {
    scores[0] = scores[1] = 0;
    for (int i = 0; i < N; i++)
      for (int j = 0; j < M; j++)
        for (int k = 0; k < 5; k++) boxes[i][j][k] = -1;
  }

  /// Just consider a single box.
  void mark(int x, int y, int lineId, int player) {
    assert(player == 0 || player == 1);

    boxes[x][y][lineId] = player;
    if (complete(x, y)) {
      cout << ">> Player #" << player << " completes box (" << x << ", " << y
           << ")!" << endl;
      scores[player]++;
      cout << "Scores: " << scores[0] << " vs " << scores[1] << endl;
      boxes[x][y][4] = player;
    }
  }

  bool hasOpposite(int x, int y, int lineId) {
    return !((lineId == LEFT && y == 0) || (lineId == UP && x == 0) ||
             (lineId == RIGHT && y == M - 1) || (lineId == DOWN && x == N - 1));
  }

  Line opposite(int x, int y, int lineId) {
    assert(hasOpposite(x, y, lineId));

    if (lineId == LEFT) return Line(x, y - 1, RIGHT);
    if (lineId == UP) return Line(x - 1, y, DOWN);
    if (lineId == RIGHT) return Line(x, y + 1, LEFT);
    if (lineId == DOWN) return Line(x + 1, y, UP);

    assert(false);
  }

  /// `player` takes the line in box (x, y) of its lineId.
  void take(int x, int y, int lineId, int player) {
    cout << ">> Player #" << player << " is taking box (" << x << ", " << y
         << ")'s line " << lineId << " ..." << endl;

    assert(x < N && x >= 0);
    assert(y < M && y >= 0);
    assert(player >= 0 && player <= 1);
    assert(boxes[x][y][lineId] == -1);  // unoccupied.
    assert(boxes[x][y][4] == -1);       // unoccupied.

    mark(x, y, lineId, player);
    if (hasOpposite(x, y, lineId)) {
      Line oppo = opposite(x, y, lineId);
      mark(oppo.bx, oppo.by, oppo.id, player);
    }

    if (gameOver()) {
      cout << endl;
      cout << ">>>>     GAME OVER!     <<<<" << endl;
      cout << endl;
      cout << "Scores: " << scores[0] << " vs " << scores[1] << endl;
    }
  }

  bool complete(int x, int y) const {
    assert(x >= 0 && x < N);
    assert(y >= 0 && y < M);

    for (int i = 0; i < 4; i++)
      if (boxes[x][y][i] == -1) return false;

    return true;
  }

  bool gameOver() const {
    for (int i = 0; i < N; i++)
      for (int j = 0; j < M; j++)
        if (!complete(i, j)) return false;
    return true;
  }

  friend ostream &operator<<(ostream &os, const Board &board);
};

ostream &operator<<(ostream &os, const Board &board) {
  for (int i = 0; i <= N; i++) {
    for (int j = 0; j < M; j++) {
      cout << "* ";
      if ((i == N && board.boxes[i - 1][j][DOWN] != -1) ||
          (i < N && board.boxes[i][j][UP] != -1))
        cout << "- ";
      else
        cout << "  ";
    }
    cout << "*";
    cout << endl;
    if (i != N) {
      if (board.boxes[i][0][LEFT] != -1)
        cout << "| ";
      else
        cout << "  ";

      for (int j = 0; j < M; j++) {
        if (board.boxes[i][j][4] != -1)
          cout << board.boxes[i][j][4];
        else
          cout << " ";
        cout << " ";
        if (board.boxes[i][j][RIGHT] != -1)
          cout << "| ";
        else
          cout << "  ";
      }
      cout << endl;
    }
  }

  return os;
}

int main(int argc, char *argv[]) {
  Board board;
  cout << board << endl;

  board.take(0, 0, LEFT, 0);
  cout << board << endl;

  board.take(0, 0, UP, 1);
  cout << board << endl;

  board.take(0, 0, DOWN, 0);
  cout << board << endl;

  board.take(0, 1, UP, 1);
  cout << board << endl;
  board.take(0, 1, RIGHT, 0);
  cout << board << endl;
  board.take(0, 1, DOWN, 1);
  cout << board << endl;
  board.take(0, 0, RIGHT, 0);
  cout << board << endl;

  return 0;
}
