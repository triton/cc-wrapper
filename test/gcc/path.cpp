#include <catch2/catch.hpp>
#include <parallel_hashmap/phmap.h>
#include <vector>

#include "env.hpp"
#include "gcc/path.hpp"

namespace cc_wrapper {
namespace gcc {
namespace path {

TEST_CASE("Filter Arguments", "[appendGood]") {
  env::detail::enforcingPurity() = true;
  // clang-format off
  const std::vector<nonstd::string_view> expected = {
      "-I/usr/include",
      "-I/build/include",
      "",
      "-I", "/build/include2",
      "-isystem/build/include",
      "-idirafter", "/build/include",
      "-z", "/usr/zoink",
      "-isystemm/usr/include",
  };
  phmap::flat_hash_set<nonstd::string_view> expected_saved_includes = {
      "/build/include",
      "/build/include2",
      "m/usr/include",
  };
  const std::vector<nonstd::string_view> prefixes = {
      "/build",
  };
  const std::vector<nonstd::string_view> input = {
      "-I/build/include",
      "",
      "-I/usr/include",
      "-I", "/build/include2",
      "-I", "/usr/include",
      "-isystem/build/include",
      "-isystem/usr/include",
      "-idirafter", "/usr/include",
      "-idirafter", "/build/include",
      "-B/usr/bin",
      "-L/usr/lib",
      "-B", "/usr/bin",
      "-L", "/usr/lib",
      "-z", "/usr/zoink",
      "-isystemm/usr/include",
  };
  std::vector<nonstd::string_view> output = {
      "-I/usr/include",
  };
  // clang-format on
  phmap::flat_hash_set<nonstd::string_view> saved_includes;
  appendGood(output, input, prefixes, saved_includes);
  CHECK(expected == output);
  CHECK(expected_saved_includes == saved_includes);
}

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
