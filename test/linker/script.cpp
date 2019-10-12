#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <fcntl.h>
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <sys/mman.h>

#include "file.hpp"
#include "linker/script.hpp"

namespace cc_wrapper {
namespace linker {
namespace script {

TEST_CASE("Can determine if linker script", "[isScript]") {
  file::Fd fd(memfd_create("test", 0));
  bool is_script;
  nonstd::span<const char> data;
  SECTION("Empty file") { is_script = false; }
  SECTION("Is script") {
    data = nonstd::string_view("/* GNU Linker");
    is_script = true;
  }
  SECTION("ELF lib") {
    constexpr char d[] = {0x7f, 0x45, 0x4c, 0x46};
    data = d;
    is_script = false;
  }
  fd.write(data);
  fd.lseek(0, SEEK_SET);
  CHECK(isScript(fd) == is_script);
  std::array<char, 32> buf;
  CHECK(fd.read(buf) == data);
}

TEST_CASE("Can parse libc linker script", "[parseLibs]") {
  file::Fd fd(SRCDIR "/test/linker/script.d/libc.so", O_RDONLY);
  state::Libs libs;
  parseLibs(libs, fd, "test/linker/script.d");
  std::array<char, 1> buf;
  CHECK(fd.read(buf).empty());
  CHECK(state::Libs::Queue{"/nix/store/glibc-2.23/lib/libc.so.6",
                           "/nix/store/glibc-2.23/lib/libc_nonshared.a",
                           "/nix/store/glibc-2.23/lib/ld-linux-x86-64.so.2"} ==
        libs.getFiles());
}

TEST_CASE("Can parse libgcc linker script", "[parseLibs]") {
  file::Fd fd(SRCDIR "/test/linker/script.d/libgcc_s.so", O_RDONLY);
  state::Libs libs;
  parseLibs(libs, fd, "test/linker/script.d");
  std::array<char, 1> buf;
  CHECK(fd.read(buf).empty());
  CHECK(state::Libs::Queue{"test/linker/script.d/libgcc_s.so.1"} ==
        libs.getFiles());
  CHECK(state::Libs::Queue{"gcc"} == libs.getLibs());
}

}  // namespace script
}  // namespace linker
}  // namespace cc_wrapper
