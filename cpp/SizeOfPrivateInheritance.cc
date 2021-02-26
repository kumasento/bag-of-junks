#include <iostream>

class Empty {
 public:
  void useful_function();
};

class Derived : private Empty {
  int i;
};

class Composed {
  int i;
  Empty empty;
};

int main(int argc, char *argv[]) {
  std::cout << "Size of Empty: " << sizeof(Empty) << " byte" << std::endl;
  std::cout << "Size of Derived: " << sizeof(Derived) << " byte" << std::endl;
  std::cout << "Size of Composed: " << sizeof(Composed) << " byte" << std::endl;

  return 0;
}
