#include <cassert>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  assert(argc >= 2);

  char c;
  string s;
  string p(argv[1]);
  int i = 0;

  cout << "\nThe pattern is: " << p << "\n\n";
  cout << "Press a key and <ENTER>: ";

  while (cin.get(c)) {
    if (c == '\n') {
      cout << "Press a key and <ENTER>: ";
      continue;
    }

    s.push_back(c);

    // -- Online detection algorithm.

    if (c == p[i]) i++;
    if (i == p.size()) {
      cout << "Yay!" << endl;
      break;
    }

    cout << "Sequence: " << s << endl;
  }
}
