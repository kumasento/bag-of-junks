// Compile with -std=c++14
#include <iostream>
template <typename T>
constexpr T pi =
    T(3.14159265358979323846264338327950288419716939937510582097494459230781L);

int main(int argc, char *argv[]) {
  std::cout << pi<float> << std::endl;
  std::cout << pi<double> << std::endl;

  return 0;
}
