#include <catch2/catch.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>
#include <type_traits>

#include <graph.hpp>
#include <strings.hpp>

struct Expensive {
  int i;

  Expensive(int i) : i(i) {}
  Expensive(const Expensive &e) : i(e.i + 10) {}
  Expensive &operator=(const Expensive &e) {
    i = e.i + 10;
    return *this;
  }
  Expensive(Expensive &&e) : i(e.i + 1) {}
  Expensive &operator=(Expensive &&e) {
    i = e.i + 1;
    return *this;
  }
};

namespace std {

template <>
struct equal_to<Expensive> {
  inline std::size_t operator()(const Expensive &a, const Expensive &b) {
    return a.i == b.i;
  }
};

template <>
struct hash<Expensive> {
  inline std::size_t operator()(const Expensive &e) {
    return std::hash<int>{}(e.i);
  }
};

}  // namespace std

namespace cc_wrapper {
namespace graph {

namespace detail {

TEST_CASE("Types", "[types]") {
  static_assert(std::is_trivially_copyable<nonstd::string_view>::value,
                "String view isn't trivially copyable");
  static_assert(std::is_same<KeyRef<nonstd::string_view>::type,
                             nonstd::string_view>::value,
                "KeyRef wraps string_view");
  static_assert(std::is_same<KeyRef<Expensive>::type,
                             std::reference_wrapper<const Expensive>>::value,
                "KeyRef doesn't wrap Expensive");
}

}  // namespace detail

TEST_CASE("Deref hashing works as expected", "[hashing]") {
  nonstd::string_view a = "a";
  auto s = strings::toString(a);
  nonstd::string_view b = s;
  CHECK(a.data() != b.data());
  phmap::flat_hash_map<std::reference_wrapper<const nonstd::string_view>, int>
      set;
  set.insert({std::cref(a), 1});
  CHECK(set.count(a) == 1);
  CHECK(set.count(b) == 1);
}

TEST_CASE("Graph doesn't make extra copies", "[insert]") {
  Graph<Expensive> g;
  g.insert(1);
  auto l = g.toposort();
  CHECK(l.size() == 1);
  CHECK(l[0].get().i == 2);
}

TEST_CASE("Graph copies once", "[insert]") {
  Graph<Expensive> g;
  const Expensive e(1);
  g.insert(e);
  auto l = g.toposort();
  CHECK(l.size() == 1);
  CHECK(l[0].get().i == 11);
}

TEST_CASE("Toposort", "[toposort]") {
  Graph<nonstd::string_view> g;
  g.insert("a");
  g.insert("b");
  g.insert("c");
  g.connect("a", "b");
  g.connectAndInsert("a", "d");
  g.connectAndInsert("e", "f");
  g.connectAndInsert("d", "g");
  g.connectAndInsert("b", "g");
  g.connect("a", "b");
  g.insert("a");

  phmap::flat_hash_map<nonstd::string_view, size_t> order;
  size_t i = 0;
  for (const auto &node : g.toposort()) {
    CHECK(order.count(node) == 0);
    order[node] = i++;
  }
  CHECK(order.size() == 7);
  CHECK(order["a"] > order["b"]);
  CHECK(order["a"] > order["d"]);
  CHECK(order["a"] > order["g"]);
  CHECK(order["a"] > order["g"]);
  CHECK(order["b"] > order["g"]);
  CHECK(order["d"] > order["g"]);
  CHECK(order["e"] > order["f"]);
}

TEST_CASE("Toposort cycle", "[toposort]") {
  Graph<nonstd::string_view> g;
  g.connectAndInsert("a", "b");
  g.connectAndInsert("b", "c");
  CHECK(g.toposort().size() == 3);
  g.connectAndInsert("c", "a");
  CHECK_THROWS(g.toposort());
}

}  // namespace graph
}  // namespace cc_wrapper
