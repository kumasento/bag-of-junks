#include <cmath>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

template <typename T>
class Skiplist {
 public:
  struct Node {
    T val;
    Node *prev, *next;
    Node *up, *down;

    Node() : prev(nullptr), next(nullptr), up(nullptr), down(nullptr) {}
    Node(T val)
        : val(val), prev(nullptr), next(nullptr), up(nullptr), down(nullptr) {}
  };

  Skiplist() {
    size = 0;

    head = new Node();
    Node *tail = new Node();
    connH(head, tail);

    cout << "HEAD: " << head << endl;
    cout << "TAIL: " << tail << endl;
  }

  void connH(Node *prev, Node *next) {
    assert(prev != nullptr);
    assert(next != nullptr);

    prev->next = next;
    next->prev = prev;
  }

  void connV(Node *up, Node *down) {
    assert(up != nullptr);
    assert(down != nullptr);

    up->down = down;
    down->up = up;
  }

  bool isTail(Node *node) const {
    assert(node != nullptr);
    return node->next == nullptr;
  }

  int maxLevel() const {
    return size == 0
               ? 1
               : static_cast<int>(ceil(log2(static_cast<double>(size)))) + 1;
  }

  int getRandomTopLevel() const {
    return static_cast<int>(
        floor(static_cast<double>(rand()) / RAND_MAX * (maxLevel() + 1)));
  }

  void insertAfter(Node *node, Node *curr) {
    assert(node != nullptr);
    assert(node->next != nullptr);

    Node *next = node->next;
    connH(node, curr);
    connH(curr, next);
  }

  void insert(T val) {
    cout << ">>> Inserting value " << val << " into the list ..." << endl;
    cout << "List size: " << size << endl;

    cout << "Current max level: " << maxLevel() << endl;
    list<Node *> prevNodes;
    Node *node = lookup(val, prevNodes);
    Node *next = node->next;

    cout << "Prev nodes: " << prevNodes.size() << " of them" << endl;

    Node *curr = new Node(val);
    insertAfter(node, curr);

    int currMaxLevel = maxLevel();
    size++;
    int nextMaxLevel = maxLevel();
    if (nextMaxLevel > currMaxLevel) {
      Node *newHead = new Node();
      Node *newTail = new Node();
      connH(newHead, newTail);
      connV(newHead, head);
      head = newHead;

      prevNodes.push_front(head);
    }

    if (currMaxLevel > 1) {
      currMaxLevel = nextMaxLevel;
      int newTopLevel = getRandomTopLevel();
      assert(newTopLevel <= currMaxLevel);
      cout << "Random top level: " << newTopLevel << endl;

      cout << "Current max level: " << currMaxLevel << endl;
      assert(prevNodes.size() == currMaxLevel - 1);

      Node *vertCurr = curr;
      auto currPrevNode = prev(prevNodes.end());

      for (int i = 0; i < newTopLevel - 1; i++) {
        cout << "Insert at level #" << currMaxLevel - i - 1 << " ..." << endl;
        Node *newNode = new Node(val);
        insertAfter(*currPrevNode, newNode);
        connV(newNode, vertCurr);

        vertCurr = newNode;
        currPrevNode = prev(currPrevNode);
      }
    }
  }

  Node *lookup(T val) const {
    list<Node *> prevNodes;
    return lookup(val, prevNodes);
  }

  Node *lookup(T val, list<Node *> &prevNodes) const {
    Node *curr = head;

    // Searching at the current level.
    while (curr->val != val) {
      while (!isTail(curr) && curr->val < val) curr = curr->next;

      if (curr->prev != nullptr) curr = curr->prev;
      if (curr->down == nullptr) return curr;
      prevNodes.push_back(curr);
      curr = curr->down;
    }
    return nullptr;
  }

  void remove(T val) {}

  friend ostream &operator<<(ostream &os, const Skiplist &skiplist) {
    Node *curr = skiplist.head;
    Node *currHead = skiplist.head;

    int i = 0;
    while (currHead != nullptr) {
      cout << "Level #" << i << ": HEAD -> ";
      curr = currHead->next;
      while (curr->next != nullptr) {
        cout << curr->val << " -> ";
        curr = curr->next;
      }
      cout << "NIL" << endl;

      currHead = currHead->down;
      i++;
    }

    return os;
  }

 private:
  Node *head;

  int size;
};

int main() {
  Skiplist<int> list;
  list.insert(30);
  cout << list << endl;
  cout << "Finding value 30: " << (list.lookup(30)->val == 30 ? "YES" : "NO")
       << endl;
  list.insert(40);
  cout << list << endl;
  list.insert(50);
  cout << list << endl;
  list.insert(45);
  cout << list << endl;
  list.insert(40);
  cout << list << endl;
  list.insert(40);
  cout << list << endl;
  list.insert(80);
  cout << list << endl;
  list.insert(80);
  cout << list << endl;
  list.insert(80);
  cout << list << endl;
  list.insert(100);
  cout << list << endl;
  list.insert(120);
  cout << list << endl;
  list.insert(150);
  cout << list << endl;

  return 0;
}
