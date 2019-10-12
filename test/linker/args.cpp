#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <nonstd/string_view.hpp>
#include <string>
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
  CHECK(isDynamicLinking(std::vector<nonstd::string_view>{}));
  CHECK(isDynamicLinking(std::vector<nonstd::string_view>{""}));
  CHECK(!isDynamicLinking(std::vector<nonstd::string_view>{"-static"}));
  CHECK(isDynamicLinking(std::vector<nonstd::string_view>{"-staticc"}));
  CHECK(isDynamicLinking(std::vector<nonstd::string_view>{"-sharedd"}));
  CHECK(isDynamicLinking(
      std::vector<nonstd::string_view>{"-static", "-shared", "-static"}));
}

TEST_CASE("parseLibs", "[parseLibs]") {
  // clang-format off
  std::vector<nonstd::string_view> args = {
    "-plugin", "/usr/lib/liblto_plugin.so",
    "-plugin-opt=/usr/lib/lto-wrapper",
    "--build-id",
    "--eh-frame-hdr",
    "-m", "elf_x86_64",
    "--hash-style=gnu",
    "-dynamic-linker", "/lib64/ld-linux-x86-64.so.2",
    "-pie",
    "-o", "main",
    "/usr/lib/gcc/Scrt1.o",
    "/usr/lib/gcc/crti.o",
    "-L/usr/lib/doh/../gcc",
    "-L", "/usr/lib/gcc/x86_64-linux-gnu",
    "-v",
    "main.o",
    "-rpath", "/usr/lib",
    "-lgcc",
    "--push-state",
    "--as-needed",
    "-lgcc_s",
    "--pop-state",
    "-l", "c",
    "-L/usr/lib/gcc",
    "-soname", "main.so",
    "-Map", "main.map",
    "-Bstatic", "-lwat", "-Bdynamic",
    "-lgcc",
    "/usr/lib/gcc/crtendS.o",
    "-rpath=/usr/doh/../lib",
    "-L",
  };
  // clang-format on
  state::Libs libs;
  parseLibs(libs, args);
  CHECK(libs.getRPaths() == state::Libs::Set{"/usr/lib"});
  CHECK(state::Libs::Array{"/usr/lib/gcc", "/usr/lib/gcc/x86_64-linux-gnu"} ==
        libs.getLibPaths());
  CHECK(state::Libs::Queue{"gcc", "gcc_s", "c", "gcc"} == libs.getLibs());
  CHECK(state::Libs::Queue{"/usr/lib/gcc/Scrt1.o", "/usr/lib/gcc/crti.o",
                           "main.o",
                           "/usr/lib/gcc/crtendS.o"} == libs.getFiles());
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
