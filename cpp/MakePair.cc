template <typename U, typename V>
class pair {
 public:
  pair() : u(), v() {}
  pair(U u, V v) : u(u), v(v) {}

 private:
  U u;
  V v;
};

template <typename U, typename V>
auto make_pair(U u, V v) {
  return pair<U, V>(u, v);
}
