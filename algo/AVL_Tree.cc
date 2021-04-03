#include <iostream>
#include <memory>
#include <vector>

using namespace std;

struct BSTNode {
  int key;
  BSTNode *left, *right;
  BSTNode *p;

  // NIL node.
  BSTNode() : key(INT_MIN), left(nullptr), right(nullptr), p(nullptr) {}
  BSTNode(int key) : key(key), left(nullptr), right(nullptr), p(nullptr) {}
  BSTNode(int key, BSTNode *p)
      : key(key), left(nullptr), right(nullptr), p(p) {}
};

class BST {
  BSTNode *root;

public:
  BST() : root{nullptr} {}

  void Insert(int key) { Insert(new BSTNode(key)); }
  void Insert(BSTNode *z) {
    BSTNode *y = nullptr;
    BSTNode *x = root;
    while (x != nullptr) {
      y = x;
      if (z->key < x->key)
        x = x->left;
      else
        x = x->right;
    }

    z->p = y;
    if (y == nullptr)
      root = z;
    else if (z->key < y->key)
      y->left = z;
    else
      y->right = z;
  }

  void Transplant(BSTNode *u, BSTNode *v) {
    if (u->p == nullptr)
      root = v;
    else if (u == u->p->left)
      u->p->left = v;
    else
      u->p->right = v;

    if (v != nullptr)
      v->p = u->p;
  }

  void Remove(int key) {}

  BSTNode *Predecessor(BSTNode *x) {
    if (x->left != nullptr)
      return TreeMaximum(x->left);

    BSTNode *y = x->p;
    while (y != nullptr && y->left == x) {
      x = y;
      y = y->p;
    }
    return y;
  }

  BSTNode *Successor(BSTNode *x) {
    if (x->right != nullptr)
      return TreeMinimum(x->right);

    BSTNode *y = x->p;
    while (y != nullptr && y->right == x) {
      x = y;
      y = y->p;
    }

    return y;
  }

  bool Lookup(int key) const {
    auto curr = root;
    while (curr != nullptr) {
      if (curr->key == key)
        return true;
      if (curr->key < key)
        curr = curr->right;
      else
        curr = curr->left;
    }
    return false;
  }

  BSTNode *TreeMinimum() { return TreeMinimum(root); }
  BSTNode *TreeMinimum(BSTNode *node) {
    if (node == nullptr)
      return nullptr;
    auto curr = node;
    while (curr->left != nullptr)
      curr = curr->left;
    return curr;
  }

  BSTNode *TreeMaximum() { return TreeMaximum(root); }
  BSTNode *TreeMaximum(BSTNode *node) {
    if (node == nullptr)
      return nullptr;

    auto curr = node;
    while (curr->right != nullptr)
      curr = curr->right;
    return curr;
  }
};

int main() {
  vector<int> keys{10, 5, 20, 56, 9, 34};

  BST bst;

  for (const auto &key : keys) {
    bst.Insert(key);
    assert(bst.Lookup(key));
  }

  sort(keys.begin(), keys.end());

  for (const auto &key : keys) {
    assert(bst.TreeMinimum()->key == key);
    bst.Remove(key);
  }
}
