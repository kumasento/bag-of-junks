#include <iostream>

// Won't work if func accepts references.
// template <typename T1, typename T2>
// void wrapper(T1 e1, T2 e2) {
//   func(e1, e2);
// }

// Passing RValue will fail
// wrapper(42, 3.14f);
//
// Can work with pass-by-reference func though.
//
// template <typename T1, typename T2>
// void wrapper(T1 &e1, T2 &e2) {
//   func(e1, e2);
// }

/// Reference collapsing.
// & -> &
// && -> &
// && & -> &
// & && -> &
// && && -> &&
template <typename T>
void baz(T t) {
  // k will be of reference-to-reference if T is a reference itself.
  // What if T is int&& since rvalue reference is introduced in C++11?
  T& k = t;
}

/// Special deduction rule for rvalue reference.
// t is not an rvalue reference.
// - If t is an lvalue of type U, T will be deduced to U&
// - If t is an rvalue of type U, T will be U;
template <typename T>
void func(T&& t) {}

template <typename T>
T&& forward(typename std::remove_reference<T>::type& t) noexcept {
  return static_cast<T&&>(t);
}

template <typename T>
T&& forward(typename std::remove_reference<T>::type&& t) noexcept {
  return static_cast<T&&>(t);
}

// Then we can try to rewrite the wrapper being forwarded to func.
template <typename T1, typename T2>
void wrapper(T1&& e1, T2&& e2) {
  func(forward<T1>(e1), forward<T2>(e2));
}

int main(int argc, char* argv[]) {
  int ii = 4;
  baz<int&>(ii);

  return 0;
}
