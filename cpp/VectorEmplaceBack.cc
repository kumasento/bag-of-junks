#include <iostream>
#include <vector>

class A {
 public:
  A() { std::cout << "A()" << std::endl; }
  A(const A& a) { std::cout << "A(const A&)" << std::endl; }
  A(A&& a) { std::cout << "A(A&&)" << std::endl; }
};

int main(int argc, char* argv[]) {
  std::vector<A> vec;
  vec.push_back(A());
  vec.emplace_back();

  std::cout << "vec size = " << vec.size() << std::endl;

  return 0;
}
