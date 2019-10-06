#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "linker/args.hpp"

namespace cc_wrapper {
namespace linker {
namespace args {

TEST_CASE("hasDynamicLinker", "[hasDynamicLinker]") {
  CHECK(!hasDynamicLinker(std::vector<nonstd::string_view>{}));
  CHECK(!hasDynamicLinker(std::vector<nonstd::string_view>{""}));
  CHECK(!hasDynamicLinker(
      std::vector<nonstd::string_view>{"-rpath", "/usr/lib"}));
  CHECK(!hasDynamicLinker(
      std::vector<nonstd::string_view>{"-dynamic-linkerr=/usr/lib"}));
  CHECK(!hasDynamicLinker(
      std::vector<nonstd::string_view>{"-dynamic-linkerr", "/usr/lib"}));
  CHECK(hasDynamicLinker(
      std::vector<nonstd::string_view>{"-z", "-dynamic-linker", "ld.so"}));
  CHECK(hasDynamicLinker(
      std::vector<nonstd::string_view>{"--dynamic-linker", "ld.so"}));
  CHECK(hasDynamicLinker(
      std::vector<nonstd::string_view>{"-dynamic-linker=ld.so"}));
  CHECK(hasDynamicLinker(
      std::vector<nonstd::string_view>{"-dynamic-linker=ld.so"}));
}

TEST_CASE("isDynamicLinking", "[isDynamicLinking]") {
  CHECK(!isDynamicLinking(std::vector<nonstd::string_view>{}));
  CHECK(!isDynamicLinking(std::vector<nonstd::string_view>{""}));
  CHECK(!isDynamicLinking(std::vector<nonstd::string_view>{"-static"}));
  CHECK(!isDynamicLinking(std::vector<nonstd::string_view>{"-sharedd"}));
  CHECK(
      isDynamicLinking(std::vector<nonstd::string_view>{"-shared", "-static"}));
  CHECK(isDynamicLinking(
      std::vector<nonstd::string_view>{"-static", "-Bshared"}));
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
