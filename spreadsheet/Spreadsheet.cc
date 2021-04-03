#include <cassert>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <variant>
#include <vector>

using namespace std;

template <typename T> static void print(ostream &os, const vector<T> &vec) {
  os << "[ ";
  copy(vec.begin(), vec.end(), ostream_iterator<T>(os, " "));
  os << "]";
}

template <typename T> static void print(const vector<T> &vec) {
  print<T>(cout, vec);
}

// Spreadsheet.
//
// - Data store:
// 2D array of "cells", each cell has an expression.
// An expression is a simple arithmetic formula operates on either numbers or
// reference to other cells.
//
// * Expression will come from a string. Expression will be parsed from it. The
// parsing algorithm will be shunting-yard.
// * Cell holds a parsed expression. Its value will be lazy-evaluated. A cell
// also stores the cells it refers to, which will be updated once we set a new
// expression to it.
// * A spreadsheet stores unique_ptr<Cell>. It is simpler, more efficient, and
// we clearly know that the ownership of each cell is within the spreadsheet.
// The references hold by each cell can be raw pointers, since we know that the
// life-cycle of all the cells are the same.
// * Each cell holds expression as unique_ptr due to the ownership.
//
// - Functionality
// * We can set the expression of a cell, and update it, and evaluate
// expressions.
// * There might be cyclic references that we should be aware of.
// * Once a cell's expression is updated, other cells depend on it should be
// re-evaluated as well.
// * The size of the spreadsheet is fixed.
//
// - Usage:
// Spreadsheet sp(10, 5);
// sp.set("A2", "3.14");
// sp.set("B3", "2 * A2");
// sp.set("D1", "A2 + B3");
// sp.get("D1") // == 3 * 3.14 = 9.52
// sp.set("A2", "1");
// sp.get("D1") // == 3 * 1 = 3
// sp.set("A2", "D1") // throw "cyclic reference"

struct Token {
  typedef double num_type;

  enum class Kind { T_NUM };

  explicit Token(num_type num) : data(num), kind(Kind::T_NUM) {}

  variant<num_type> data;
  Kind kind;
};

ostream &operator<<(ostream &os, const Token::Kind &kind) {
  switch (kind) {
  case Token::Kind::T_NUM:
    os << "T_NUM";
    break;
  default:
    assert(false);
  }
  return os;
}

ostream &operator<<(ostream &os, const Token &token) {
  os << token.kind << "(" << get<typename Token::num_type>(token.data) << ")";

  return os;
}

static vector<Token> tokenize(const string &str) {
  vector<Token> tokens;
  int i = 0;

  while (i < str.size()) {
    if (str[i] == ' ') {
      ++i;
      continue;
    }

    // try to read a number
    if (str[i] >= '0' && str[i] <= '9') {
      string tmp;
      while (i < str.size() &&
             ((str[i] >= '0' && str[i] <= '9') || str[i] == '.')) {
        tmp += str[i];
        i++;
      }
      tokens.emplace_back(stod(tmp));
    } else {
      assert(false);
    }
  }

  return tokens;
}

class Expr {
public:
  virtual ~Expr() {}

  template <typename NumT> static unique_ptr<Expr> parse(const string &str);
};

template <typename T> class NumExpr : public Expr {
  T num;

public:
  NumExpr(T num) : num(num) {}
};

ostream &operator<<(ostream &os, const Expr &expr) {
  os << "Expr: ";
  return os;
}

template <typename NumT> unique_ptr<Expr> Expr::parse(const string &str) {
  vector<Token> tokens = tokenize(str);
  print(tokens);
  cout << endl;

  vector<unique_ptr<Expr>> exprStack;

  // Shunting yard algorithm.
  for (const auto &token : tokens) {
    if (token.kind == Token::Kind::T_NUM)
      exprStack.push_back(make_unique<NumExpr<NumT>>(
          static_cast<NumT>(get<typename Token::num_type>(token.data))));
  }

  if (exprStack.empty())
    return nullptr;

  assert(exprStack.size() == 1);

  return move(exprStack[0]);
}

template <typename CellT> class Spreadsheet;

template <typename NumT, typename ExprT> class Cell {
public:
  typedef NumT value_type;
  static constexpr NumT empty_value = static_cast<NumT>(0);

  value_type eval(const Spreadsheet<Cell> &sp) { return empty_value; }

  void set(const string &str) { expr = ExprT::template parse<value_type>(str); }

  friend ostream &operator<<(ostream &os, const Cell &cell) {
    if (cell.expr == nullptr)
      os << "NIL";
    else
      os << *cell.expr;

    return os;
  }

private:
  unique_ptr<ExprT> expr;
};

template <typename CellT> class Spreadsheet {
public:
  using NumT = typename CellT::value_type;

  Spreadsheet(size_t rows, size_t cols) : rows(rows), cols(cols) {
    assert(cols <= 26 && cols > 0);
    assert(rows <= 999 && rows > 0);

    // A cell should be initialized in the beginning, otherwise, it will be hard
    // to handle empty cases.
    cells.resize(rows);
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        cells[i].push_back(make_unique<CellT>());
  }

  NumT get(const string &loc) {
    auto cell = at(loc);
    if (cell)
      return cell->eval(*this);
    return CellT::empty_value;
  }

  void set(const string &loc, const string &expr) {
    auto cell = at(loc);
    cell->set(expr);
  };

  friend ostream &operator<<(ostream &os, const Spreadsheet &sp) {
    os << "    ";
    for (int i = 0; i < sp.cols; i++)
      os << setw(3) << static_cast<char>('A' + i) << " ";
    os << endl;
    for (int i = 0; i < sp.rows; i++) {
      os << setw(3) << i + 1 << " ";
      for (int j = 0; j < sp.cols; j++) {
        if (sp.cells[i][j])
          os << *sp.cells[i][j];
        else
          os << "NIL";

        os << " ";
      }
      os << endl;
    }

    return os;
  }

private:
  size_t rows, cols;
  vector<vector<unique_ptr<CellT>>> cells;

  pair<int, int> loc2pos(const string &loc) const {
    assert(loc.size() >= 2); // one for column, one for row.

    int col = loc[0] - 'A';
    int row = stoi(loc.substr(1)) - 1;
    return make_pair(row, col);
  }

  auto at(const string &loc) {
    auto pos = loc2pos(loc);
    return cells[pos.first][pos.second].get();
  }
};

/// ---- Tests ----
void test_init_cell_values() {
  cout << "Test initial cell values ... ";
  Spreadsheet<Cell<double, Expr>> sp(10, 5);
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 5; j++) {
      string loc;
      loc.push_back(static_cast<char>(j + 'A'));
      loc += to_string(i + 1);

      assert(sp.get(loc) == 0);
    }
  }
  cout << "PASSED" << endl;
}

void test_set_scalar_values() {
  Spreadsheet<Cell<double, Expr>> sp(1, 1);
  sp.set("A1", "3.14");
}

int main() {
  test_init_cell_values();
  test_set_scalar_values();
  return 0;
}
