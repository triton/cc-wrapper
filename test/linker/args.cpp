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

TEST_CASE("isLinking", "[isLinking]") {
  CHECK(!isLinking(std::vector<nonstd::string_view>{}));
  CHECK(!isLinking(std::vector<nonstd::string_view>{"-m"}));
  CHECK(!isLinking(std::vector<nonstd::string_view>{"-m", "elf"}));
  CHECK(!isLinking(std::vector<nonstd::string_view>{"-L/lib"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"-l", "gcc"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"-lgcc"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"-omain"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"-o", "main"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"main.o"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"/lib/crt0.o"}));
  CHECK(isLinking(std::vector<nonstd::string_view>{"-m", "-m", "elf"}));
}

TEST_CASE("filterLinking", "[filterLinking]") {
  std::vector<nonstd::string_view> old_args, new_args, expected_args;
  SECTION("Empty") {}
  SECTION("Keeps args") { expected_args = new_args = {"main.o"}; }
  SECTION("Keeps Flag") { expected_args = old_args = {"-v"}; }
  SECTION("Keeps Parsed Flag") { expected_args = old_args = {"-L/usr/lib"}; }
  SECTION("Keeps Parsed Two Flag") {
    expected_args = old_args = {"-L", "/usr/lib"};
  }
  SECTION("Filters file") { old_args = {"main.o"}; }
  SECTION("Filters Parsed Flag") { old_args = {"-lgcc"}; }
  SECTION("Filters Parsed Two Flag") { old_args = {"-l", "gcc"}; }
  filterLinking(new_args, old_args);
  CHECK(expected_args == new_args);
}

TEST_CASE("Complex Parsing", "[complex]") {
  // clang-format off
  std::vector<nonstd::string_view> no_link_args = {
    "-plugin", "/usr/lib/liblto_plugin.so",
    "-plugin-opt=/usr/lib/lto-wrapper",
    "--build-id",
    "--eh-frame-hdr",
    "-m", "elf_x86_64",
    "--hash-style=gnu",
    "-dynamic-linker", "/lib64/ld-linux-x86-64.so.2",
    "-pie",
    "-L/usr/lib/doh/../gcc",
    "-L", "/usr/lib/gcc/x86_64-linux-gnu",
    "-v",
    "-rpath", "/usr/lib",
    "--push-state",
    "--as-needed",
    "--pop-state",
    "-L/usr/lib/gcc",
    "-soname", "main.so",
    "-Map", "main.map",
    "-Bstatic", "-Bdynamic",
    "-rpath=/usr/doh/../lib",
    "-L",
  };
  std::vector<nonstd::string_view> link_args = {
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
  SECTION("Not linking") {
    CHECK(!isLinking(no_link_args));

    std::vector<nonstd::string_view> new_args;
    filterLinking(new_args, no_link_args);
    CHECK(new_args == no_link_args);
  }
  SECTION("Linking") {
    CHECK(isLinking(link_args));

    std::vector<nonstd::string_view> new_args;
    filterLinking(new_args, link_args);
    CHECK(new_args == no_link_args);

    state::Libs libs;
    parseLibs(libs, link_args);
    CHECK(libs.getRPaths() == state::Libs::Set{"/usr/lib"});
    CHECK(state::Libs::Array{"/usr/lib/gcc", "/usr/lib/gcc/x86_64-linux-gnu"} ==
          libs.getLibPaths());
    CHECK(state::Libs::Queue{"gcc", "gcc_s", "c", "gcc"} == libs.getLibs());
    CHECK(state::Libs::Queue{"/usr/lib/gcc/Scrt1.o", "/usr/lib/gcc/crti.o",
                             "main.o",
                             "/usr/lib/gcc/crtendS.o"} == libs.getFiles());
  }
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
