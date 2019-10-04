#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <parallel_hashmap/phmap.h>
#include <vector>

#include "gcc/path.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace gcc {
namespace path {

TEST_CASE("Impure", "[isValid]") {
  util::detail::enforcingPurity() = false;
  CHECK(isValid("./local/file", {}));
  CHECK(isValid("local", {}));
  CHECK(!isValid("/no-such-path", {}));
  CHECK(!isValid("/no-such-path/file", {}));
  CHECK(isValid("/usr/bin/bash", {}));
}

TEST_CASE("Pure", "[isValid]") {
  util::detail::enforcingPurity() = true;
  CHECK(isValid("./local/file", {}));
  CHECK(isValid("local", {}));
  CHECK(!isValid("/no-such-path", {}));
  CHECK(!isValid("/no-such-path/file", {}));
  CHECK(!isValid("/usr/bin/bash", {}));
  const std::vector<nonstd::string_view> prefixes = {"/usr", "/build"};
  CHECK(isValid("/usr/bin/bash", prefixes));
  CHECK(isValid("/build/bash", prefixes));
  CHECK(!isValid("/tmp/bash", prefixes));
}

TEST_CASE("Filter Arguments", "[appendGood]") {
  util::detail::enforcingPurity() = true;
  const std::vector<nonstd::string_view> expected = {
      "-I/usr/include",
      "-I/build/include",
      "-I",
      "/build/include2",
      "-isystem/build/include",
      "-idirafter",
      "/build/include",
      "-z",
      "/usr/zoink",
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
      "-I/usr/include",
      "-I",
      "/build/include2",
      "-I",
      "/usr/include",
      "-isystem/build/include",
      "-isystem/usr/include",
      "-idirafter",
      "/usr/include",
      "-idirafter",
      "/build/include",
      "-B/usr/bin",
      "-L/usr/lib",
      "-B",
      "/usr/bin",
      "-L",
      "/usr/lib",
      "-z",
      "/usr/zoink",
      "-isystemm/usr/include",
  };
  std::vector<nonstd::string_view> output = {
      "-I/usr/include",
  };
  phmap::flat_hash_set<nonstd::string_view> saved_includes;
  appendGood(output, input, prefixes, saved_includes);
  CHECK(expected == output);
  CHECK(expected_saved_includes == saved_includes);
}

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
