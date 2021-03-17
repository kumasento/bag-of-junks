#include <iostream>

using namespace std;

enum Color { w, r, b, o, g, y };
enum Layer { U, L, F, R, B, D, M, E, S };

ostream& operator<<(ostream& os, const Layer& layer) {
  switch (layer) {
    case Layer::U:
      cout << 'U';
      break;
    case Layer::L:
      cout << 'L';
      break;
    case Layer::F:
      cout << 'F';
      break;
    case Layer::R:
      cout << 'R';
      break;
    case Layer::B:
      cout << 'B';
      break;
    case Layer::D:
      cout << 'D';
      break;
    case Layer::M:
      cout << 'M';
      break;
    case Layer::E:
      cout << 'E';
      break;
    case Layer::S:
      cout << 'S';
      break;
  }
  return os;
}

struct Move {
  Layer layer;
  bool isClockwise;

  Move(Layer layer, bool isClockwise)
      : layer(layer), isClockwise(isClockwise) {}
};

ostream& operator<<(ostream& os, const Move& move) {
  os << move.layer << (move.isClockwise ? " " : "'");
  return os;
}

class Cube {
 public:
  Cube() {
    for (int i = 0; i < 6; i++)
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++) faces[i][r][c] = Color(i);
  }

  friend ostream& operator<<(ostream& os, const Cube& cube);

  bool finished() const {
    for (int i = 0; i < 6; i++)
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
          if (faces[i][r][c] != faces[i][0][0]) return false;
    return true;
  }

  Layer up(Layer layer) {
    switch (layer) {
      case Layer::F:
        return Layer::U;
      case Layer::F:
        return Layer::U;
      case Layer::F:
        return Layer::U;
      case Layer::F:
        return Layer::U;
      case Layer::F:
        return Layer::U;
    }
  }

  void moveFace(Move m) {
    if (m.layer <= Layer::D) {
      Color topLeft = faces[m.layer][0][0];
      faces[m.layer][0][0] = faces[m.layer][2][0];
      faces[m.layer][2][0] = faces[m.layer][2][2];
      faces[m.layer][2][2] = faces[m.layer][0][2];
      faces[m.layer][0][2] = topLeft;

      Color midTop = faces[m.layer][0][1];
      faces[m.layer][0][1] = faces[m.layer][1][2];
      faces[m.layer][1][2] = faces[m.layer][2][1];
      faces[m.layer][2][1] = faces[m.layer][1][0];
      faces[m.layer][1][0] = faces[m.layer][0][1];

      Color vec[3] = {}
    }
  }

  bool move(Move m) { return finished(); }

 private:
  // U, L, F, R, B, D.
  Color faces[6][3][3];
};

ostream& operator<<(ostream& os, const Color color) {
  switch (color) {
    case Color::w:
      os << 'W';
      break;
    case Color::r:
      os << 'R';
      break;
    case Color::b:
      os << 'B';
      break;
    case Color::o:
      os << 'O';
      break;
    case Color::g:
      os << 'G';
      break;
    case Color::y:
      os << 'Y';
      break;
  }
  return os;
}

ostream& operator<<(ostream& os, const Cube& cube) {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 12; col++) {
      if (col < 3 || col >= 6)
        cout << "  ";
      else
        cout << cube.faces[0][row][col - 3] << ' ';
    }
    cout << endl;
  }

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 12; col++) {
      cout << cube.faces[1 + col / 3][row][col % 3] << ' ';
    }
    cout << endl;
  }

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 12; col++) {
      if (col < 3 || col >= 6)
        cout << "  ";
      else
        cout << cube.faces[5][row][col - 3] << ' ';
    }
    cout << endl;
  }
  return os;
}

int main(int argc, char* argv[]) {
  Cube cube;
  cout << cube << endl;
  cout << Move(F, true) << endl;
  cout << Move(F, false) << endl;
  cout << cube.finished() << endl;

  return 0;
}
