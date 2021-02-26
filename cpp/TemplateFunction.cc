template <typename T>
T foo(T x) {
  return x++;
}

// Failed to compile
template <>
long foo(double x) {
  return x / 2;
}
