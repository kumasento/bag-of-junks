#include <iostream>

// Fold expression
// template <typename... T>
// auto sum(const T &... x) {
//   return (x + ...);
// }

template <typename T1>
auto sum(const T1 &x) {
  return x;
}

template <typename T1, typename... T>
auto sum(const T1 &x1, const T &... x) {
  return x1 + sum(x...);
}

template <typename... T>
struct Group;

template <typename T1>
struct Group<T1> {
  T1 t1_;

  Group() = default;
  explicit Group(const T1 &t1) : t1_(t1) {}
  explicit Group(T1 &&t1) : t1_(std::move(t1)) {}
};

template <typename T1, typename... T>
struct Group<T1, T...> : Group<T...> {
  T1 t1_;

  Group() = default;
  explicit Group(const T1 &t1, T &&... t)
      : Group<T...>(std::forward<T>(t)...), t1_(t1) {}
  explicit Group(T1 &&t1, T &&... t)
      : Group<T...>(std::forward<T>(t)...), t1_(std::move(t1)) {}
};

int main(int argc, char *argv[]) {
  std::cout << "sum(1) = " << sum(1) << std::endl;
  std::cout << "sum(1, 5L) = " << sum(1, 5L) << std::endl;
  std::cout << "sum(1, 5L, 7.1) = " << sum(1, 5L, 7.1) << std::endl;

  Group<int> g1(1);
  Group<int, long> g2(1, 6L);
  Group<int, long, double> g3(1, 6L, 1.23);

  return 0;
}
