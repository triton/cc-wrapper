#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "linker/compiler.hpp"

namespace cc_wrapper {
namespace linker {
namespace compiler {

SCENARIO("Filter Compiler Flags", "[filterFlags]") {
  // clang-format off
  const std::vector<nonstd::string_view> args = {
    "-rpath",          "/libs1",
    "-rpath=/libs2",
    "-rpathh",         "/libs3",
    "-rpathh=/libs4",
    "-rpath",          "-rpath",
    "-dynamic-linker", "gcc-ld.so",
    "-g",
    "-rpath",          "/lobs",
    "-dynamic-linker", "my-ld.so",
  };
  std::vector<nonstd::string_view> new_args;
  std::vector<nonstd::string_view> expected_new_args;
  // clang-format on
  SECTION("Dont filter linker") {
    // clang-format off
    expected_new_args = {
      "-rpathh",         "/libs3",
      "-rpathh=/libs4",
      "-dynamic-linker", "gcc-ld.so",
      "-rpath",          "/lobs",
      "-dynamic-linker", "my-ld.so",
    };
    // clang-format on
    filterFlags(new_args, args, false);
  }
  SECTION("Filter linker") {
    // clang-format off
    expected_new_args = {
      "-dynamic-linker=internal-ld.so",
      "-rpathh",         "/libs3",
      "-rpathh=/libs4",
      "-rpath",          "/lobs",
      "-dynamic-linker", "my-ld.so",
    };
    // clang-format on
    new_args.push_back("-dynamic-linker=internal-ld.so");
    filterFlags(new_args, args, true);
  }
  CHECK(expected_new_args == new_args);
}

}  // namespace compiler
}  // namespace linker
}  // namespace cc_wrapper
