#include <cassert>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

using namespace std;

/// Plain old data structure
struct Token {
  enum Type { NUMBER, BINARY_OP };

  union {
    double number;
    char op;
  };
  Type type;
};

ostream &operator<<(ostream &os, const Token &token) {
  if (token.type == Token::NUMBER)
    return os << "NUM(" << token.number << ")";

  return os << "BINARY_OP(" << token.op << ")";
}

class Lexer {
public:
  static vector<Token> tokenize(string s) {
    vector<Token> tokens;

    int i = 0;
    while (i < s.size()) {
      // whitespaces
      if (s[i] == ' ' || s[i] == '\n') {
        i++;
        continue;
      }

      Token token;

      // number (don't allow minus numbers)
      if (s[i] >= '0' && s[i] <= '9') {
        int j = i + 1;
        while (j < s.size() && s[j] >= '0' && s[j] <= '9')
          j++;

        token.type = Token::NUMBER;
        token.number = stod(s.substr(i, j - i));
        // Move forward to j.
        i = j;
      } else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
        token.type = Token::BINARY_OP;
        token.op = s[i];
        i++;
      }

      tokens.push_back(token);
    }

    return tokens;
  }
};

/// AST
class Expr {
public:
  virtual ~Expr() = default;

  virtual double eval() const = 0;
};

class NumExpr : public Expr {
public:
  explicit NumExpr(double num) : num(num){};

  double getNum() const { return num; }

  double eval() const { return num; }

private:
  double num;
};

/// Plain-old data structure.
struct BinaryOp {
  enum Kind { ADD, SUB, MUL, DIV };
  enum Assoc { L, R };

  Kind kind;
  int priority;
  Assoc assoc;

  BinaryOp() {}
  BinaryOp(Kind kind, int priority, Assoc assoc)
      : kind(kind), priority(priority), assoc(assoc) {}
};

class BinaryOpExpr : public Expr {
public:
  BinaryOpExpr(BinaryOp op) : op(op), lhs(nullptr), rhs(nullptr) {}

  void setOperands(unique_ptr<Expr> &lhs, unique_ptr<Expr> &rhs) {
    this->lhs = move(lhs);
    this->rhs = move(rhs);
  }

  BinaryOp getOpKind() const { return op; }

  double eval() const {
    if (op.kind == BinaryOp::ADD)
      return lhs->eval() + rhs->eval();
    if (op.kind == BinaryOp::SUB)
      return lhs->eval() - rhs->eval();
    if (op.kind == BinaryOp::MUL)
      return lhs->eval() * rhs->eval();
    if (op.kind == BinaryOp::DIV)
      return lhs->eval() / rhs->eval();

    assert(false);
  }

private:
  BinaryOp op;
  unique_ptr<Expr> lhs, rhs;
};

class Parser {
public:
  static unique_ptr<Expr> parse(string input) {
    vector<Token> tokens = Lexer::tokenize(input);

    stack<unique_ptr<Expr>> exprs;
    stack<unique_ptr<BinaryOpExpr>> ops;

    for (int i = 0; i < tokens.size(); i++) {
      Token token = tokens[i];
      if (token.type == Token::NUMBER) {
        exprs.push(move(make_unique<NumExpr>(token.number)));
      } else {
        BinaryOp opKind;
        switch (token.op) {
        case '+':
          opKind = BinaryOp(BinaryOp::ADD, 0, BinaryOp::L);
          break;
        case '-':
          opKind = BinaryOp(BinaryOp::SUB, 0, BinaryOp::L);
          break;
        case '*':
          opKind = BinaryOp(BinaryOp::MUL, 1, BinaryOp::L);
          break;
        case '/':
          opKind = BinaryOp(BinaryOp::DIV, 1, BinaryOp::L);
          break;
        default:
          assert(false);
          break;
        }

        unique_ptr<BinaryOpExpr> currOp = make_unique<BinaryOpExpr>(opKind);

        while (!ops.empty()) {
          if (ops.top()->getOpKind().priority <= opKind.priority)
            break;

          unique_ptr<BinaryOpExpr> topOp = move(ops.top());
          ops.pop();

          assert(exprs.size() >= 2);
          unique_ptr<Expr> rhs = move(exprs.top());
          exprs.pop();
          unique_ptr<Expr> lhs = move(exprs.top());
          exprs.pop();

          topOp->setOperands(lhs, rhs);
          exprs.push(move(topOp));
        }

        ops.push(move(currOp));
      }
    }

    while (!ops.empty()) {
      unique_ptr<BinaryOpExpr> topOp = move(ops.top());
      ops.pop();

      assert(exprs.size() >= 2);
      unique_ptr<Expr> rhs = move(exprs.top());
      exprs.pop();

      unique_ptr<Expr> lhs = move(exprs.top());
      exprs.pop();
      assert(rhs != nullptr && lhs != nullptr);

      topOp->setOperands(lhs, rhs);
      exprs.push(move(topOp));
    }

    auto result = move(exprs.top());
    exprs.pop();
    return move(result);
  }
};

class Evaluator {
public:
  static double eval(const unique_ptr<Expr> &expr) { return expr->eval(); }
};

int main(int argc, char *argv[]) {
  char opNames[4] = {'+', '-', '*', '/'};

  while (cin) {
    string cmd;
    getline(cin, cmd);

    if (cmd.empty())
      continue;

    if (cmd == "EVAL") {
      string input;
      getline(cin, input);
      cout << Evaluator::eval(Parser::parse(input)) << endl;
    } else if (cmd == "TEST") {
      int a, b, c, d;
      cin >> a >> b >> c >> d;
      cin.get();

      cout << "24 Point for input: " << a << " " << b << " " << c << " " << d
           << "\n";

      bool hasAns = false;
      for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
          for (int k = 0; k < 4; k++) {
            string expr = to_string(a) + " " + opNames[i] + " " + to_string(b) +
                          " " + opNames[j] + " " + to_string(c) + " " +
                          opNames[k] + " " + to_string(d);
            if (Evaluator::eval(Parser::parse(expr)) == 24) {
              hasAns = true;
              cout << expr << endl;
            }
          }

      if (!hasAns)
        cout << "No Answer!\n";
    } else {
      assert(false);
    }
  }

  return 0;
}
