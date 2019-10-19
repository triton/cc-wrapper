#include <catch2/catch.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file.hpp"
#include "linker/state.hpp"

namespace cc_wrapper {
namespace linker {
namespace state {

void cleanup() {
  unlink("test-dir/libgcc.a");
  rmdir("test-dir");
  unlink("libgcc.so");
  unlink("libgcc.a");
}

TEST_CASE("Can find a lib in path", "[lookupLib]") {
  cleanup();

  Libs libs;
  CHECK(libs.lookupLib("gcc") == nonstd::nullopt);
  libs.addLibPath("test-dir");
  libs.addLibPath(".");
  CHECK(libs.lookupLib("gcc") == nonstd::nullopt);
  file::Fd("libgcc.so", O_RDWR | O_CREAT);
  file::Fd("libgcc.a", O_RDWR | O_CREAT);
  CHECK(libs.lookupLib("gcc") == "./libgcc.so");
  CHECK(mkdir("test-dir", 0755) == 0);
  file::Fd("test-dir/libgcc.a", O_RDWR | O_CREAT);
  CHECK(libs.lookupLib("gcc") == "test-dir/libgcc.a");

  cleanup();
}

TEST_CASE("Next unresolved", "[nextUnresolved]") {
  cleanup();

  Libs libs;
  libs.addFile("test/file.cpp");
  libs.addLib("gcc");
  file::Fd("libgcc.so", O_RDWR | O_CREAT);
  CHECK(libs.nextUnresolved() == "test/file.cpp");
  CHECK_THROWS(libs.nextUnresolved());
  libs.addLibPath(".");
  CHECK(libs.nextUnresolved() == "./libgcc.so");
  CHECK(libs.nextUnresolved() == nonstd::nullopt);

  cleanup();
}

}  // namespace state
}  // namespace linker
}  // namespace cc_wrapper
