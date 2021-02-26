#include <iostream>

void whatami(int x) { std::cout << x << " is int" << std::endl; }
void whatami(long x) { std::cout << x << " is long" << std::endl; }

template <typename T>
void whatami(T *x) {
  std::cout << x << " is pointer" << std::endl;
}

template <typename T>
void whatami(T &&x) {
  std::cout << "something weird" << std::endl;
}

class C {};

void whatami(...) { std::cout << "I'm a variadic template" << std::endl; }

int main(int argc, char *argv[]) {
  whatami(5);
  whatami(5L);
  whatami(5.0);

  int i = 1;
  whatami(&i);

  C c;
  whatami(c);

  whatami();
  whatami(1, 2, 3, 4);

  return 0;
}
