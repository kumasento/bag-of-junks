#include <iostream>

class A {
 public:
  virtual void foo() { std::cout << "A::foo()" << std::endl; }
};

class B : public A {
 public:
  virtual void foo() { std::cout << "B::foo()" << std::endl; }
};

int main(int argc, char* argv[]) {
  A* b = new B();
  b->foo();

  return 0;
}
