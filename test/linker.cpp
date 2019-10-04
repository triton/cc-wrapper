#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "linker.hpp"

namespace cc_wrapper {
namespace linker {
namespace detail {

TEST_CASE("No dynamic linker", "[filterCompilerFlags]") {
  // clang-format off
  const std::vector<nonstd::string_view> expected_new_args = {
      "-dynamic-linker", "gcc-ld.so",
      "-rpath",          "/lobs",
      "-dynamic-linker", "my-ld.so",
  };
  const std::vector<nonstd::string_view> args = {
      "-rpath",          "/libs",
      "-rpath",          "-rpath",
      "-dynamic-linker", "gcc-ld.so",
      "-g",
      "-rpath",          "/lobs",
      "-dynamic-linker", "my-ld.so",
  };
  // clang-format on
  std::vector<nonstd::string_view> new_args;
  filterCompilerFlags(new_args, args);
  CHECK(expected_new_args == new_args);
}

TEST_CASE("Dynamic linker", "[filterCompilerFlags]") {
  // clang-format off
  const std::vector<nonstd::string_view> expected_new_args = {
      "-dynamic-linker", "internal-ld.so",
      "-rpath",          "/lobs",
      "-dynamic-linker", "my-ld.so",
  };
  const std::vector<nonstd::string_view> args = {
      "-rpath",          "/libs",
      "-rpath",          "-rpath",
      "-dynamic-linker", "gcc-ld.so",
      "-g",
      "-rpath",          "/lobs",
      "-dynamic-linker", "my-ld.so",
  };
  std::vector<nonstd::string_view> new_args = {
      "-dynamic-linker", "internal-ld.so",
  };
  // clang-format on
  filterCompilerFlags(new_args, args);
  CHECK(expected_new_args == new_args);
}

TEST_CASE("No args", "[isDynamicLinking]") {
  const std::vector<nonstd::string_view> args = {};
  CHECK(!isDynamicLinking(args));
}

TEST_CASE("Just libc", "[isDynamicLinking]") {
  const std::vector<nonstd::string_view> args = {"-lc"};
  CHECK(isDynamicLinking(args));
}

TEST_CASE("Static libc", "[isDynamicLinking]") {
  const std::vector<nonstd::string_view> args = {"-static", "-lc"};
  CHECK(!isDynamicLinking(args));
}

TEST_CASE("Dynamic libc", "[isDynamicLinking]") {
  const std::vector<nonstd::string_view> args = {"-static", "-Bdynamic", "-lc"};
  CHECK(isDynamicLinking(args));
}

}  // namespace detail
}  // namespace linker
}  // namespace cc_wrapper
