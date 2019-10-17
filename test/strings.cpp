#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <nonstd/string_view.hpp>
#include <string>
#include <utility>

#include "strings.hpp"

constexpr nonstd::string_view sv1 = "hi";
constexpr nonstd::string_view sv2 = "world";

namespace cc_wrapper {
namespace strings {
namespace detail {

TEST_CASE("Views sizes", "[sizeViews]") {
  CHECK(sizeViews() == 0);
  CHECK(sizeViews(sv1) == 2);
  CHECK(sizeViews(sv1, sv2) == 7);
}

TEST_CASE("Views append", "[appendViews]") {
  std::string s;
  appendViews(s);
  CHECK(s == "");
  appendViews(s, sv1);
  CHECK(s == "hi");
  s.clear();
  appendViewsReserve(s, sv1, sv2);
  CHECK(s == "hiworld");
}

}  // namespace detail

TEST_CASE("Strings apppend", "[append]") {
  std::string s = "this";
  append(s);
  CHECK(s == "this");
  append(s, "hello", sv2);
  CHECK(s == "thishelloworld");
}

TEST_CASE("Strings cat", "[cat]") {
  std::string s = "that";
  const std::string s2 = cat(std::move(s), sv1);
  CHECK(s2 == "thathi");
  CHECK(cat(s2, std::string(sv2), "gone") == "thathiworldgone");
}

TEST_CASE("Strings prefix", "[startsWith]") {
  CHECK(startsWith("abcd", "abc"));
  CHECK(startsWith("abc", "abc"));
  CHECK(!startsWith("ab", "abc"));
}

TEST_CASE("Split empty", "[split]") {
  nonstd::string_view s = "";
  CHECK(split(s, ' ') == "");
  CHECK(s.empty());
}

TEST_CASE("Split nosep", "[split]") {
  nonstd::string_view s = "abcd";
  CHECK(split(s, ' ') == "abcd");
  CHECK(s.empty());
}

TEST_CASE("Split repeated", "[split]") {
  nonstd::string_view s = "abcd  efgh";
  CHECK(split(s, ' ') == "abcd");
  CHECK(s == " efgh");
  CHECK(split(s, ' ') == "");
  CHECK(s == "efgh");
  CHECK(split(s, ' ') == "efgh");
  CHECK(s.empty());
}

TEST_CASE("Ends With", "[endsWith]") {
  CHECK(!endsWith("", "abc"));
  CHECK(!endsWith("ab", "abc"));
  CHECK(endsWith("abc", "abc"));
  CHECK(!endsWith("abcc", "abc"));
  CHECK(endsWith("aabc", "abc"));
}

}  // namespace strings
}  // namespace cc_wrapper
