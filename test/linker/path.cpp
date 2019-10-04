#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "env.hpp"
#include "linker/path.hpp"

namespace cc_wrapper {
namespace linker {
namespace path {

TEST_CASE("Filter Arguments", "[appendGood]") {
  env::detail::enforcingPurity() = true;
  // clang-format off
  const std::vector<nonstd::string_view> expected = {
      "-dynamic-linker=/usr/lib/ld.so",
      "",
      "-plugin", "/usr/lib/liblto.so",
      "-plugin-opt", "/usr/lib/liblto.so",
      "-plugin=/usr/lib/liblto.so",
      "-dynamic-linke",
      "-dynamic-linke", "/build/lib/ld.so",
      "-dynamic-linker", "/build/lib/ld.so",
      "-dynamic-linker=/build/lib/ld.so",
      "-L/build/lib",
      "-L=/usr/lib",
      "-L", "/build/lib",
      "main.o",
  };
  const std::vector<nonstd::string_view> prefixes = {
      "/build",
  };
  const std::vector<nonstd::string_view> input = {
      "",
      "-plugin", "/usr/lib/liblto.so",
      "-plugin-opt", "/usr/lib/liblto.so",
      "-plugin=/usr/lib/liblto.so",
      "-dynamic-linker=/usr/lib/ld2.so",
      "-dynamic-linker", "/usr/lib/ld2.so",
      "--dynamic-linker", "/usr/lib/ld2.so",
      "-dynamic-linke", "/usr/lib/ld2.so",
      "-dynamic-linke", "/build/lib/ld.so",
      "-dynamic-linker", "/build/lib/ld.so",
      "-dynamic-linker=/build/lib/ld.so",
      "-L/usr/lib",
      "-L/build/lib",
      "-L=/usr/lib",
      "-L", "/usr/lib",
      "-L", "/build/lib",
      "/usr/lib/crtn.o",
      "main.o",
  };
  std::vector<nonstd::string_view> output = {
      "-dynamic-linker=/usr/lib/ld.so",
  };
  // clang-format on
  appendGood(output, input, prefixes);
  CHECK(expected == output);
}

}  // namespace path
}  // namespace linker
}  // namespace cc_wrapper
