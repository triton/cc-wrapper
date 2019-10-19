#include <catch2/catch.hpp>
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
  std::vector<nonstd::string_view> expected_saved_includes = {
      "/build/include",
      "/build/include2",
      "/build/include",
      "/build/include",
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
  std::vector<nonstd::string_view> saved_includes;
  appendGood(output, input, prefixes, saved_includes);
  CHECK(expected == output);
  CHECK(saved_includes == expected_saved_includes);
}

TEST_CASE("Combine prefix flags", "[prefixMapFlags]") {
  const std::vector<nonstd::string_view> dirs = {
      "/usr///", "/usr/include/", "/include", "relative", ".",
  };
  const std::vector<std::string> expected = {
      "-fmap=/usr=/no-such-path",
      "-fmap=/include=/no-such-path",
  };
  CHECK_THAT(prefixMapFlags("map", dirs),
             Catch::Matchers::UnorderedEquals(expected));
}

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
