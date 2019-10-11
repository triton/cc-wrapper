#define CATCH_CONFIG_MAIN
#include <array>
#include <catch2/catch.hpp>
#include <fcntl.h>
#include <nonstd/string_view.hpp>
#include <sys/mman.h>

#include "file.hpp"

namespace cc_wrapper {
namespace file {

TEST_CASE("Open file descriptor handles errors", "[open]") {
  CHECK_THROWS_AS(Fd("/no-such-path/cc-wrapper", O_RDONLY), std::system_error);
}

TEST_CASE("Open file descriptor works", "[open]") {
  int fdn;
  {
    Fd fd("/dev/null", O_RDONLY);
    fdn = fd;
    CHECK(fcntl(fd, F_GETFD) != -1);
  }
  CHECK(fcntl(fdn, F_GETFD) == -1);
  CHECK(errno == EBADF);
}

TEST_CASE("Read file descriptor", "[read]") {
  nonstd::string_view data = "hi";
  nonstd::span<const char> expected = data;
  Fd fd(memfd_create("test", 0));
  std::array<char, 4> buf;
  CHECK(fd.read(buf).size() == 0);
  CHECK(write(fd, data.data(), data.size()) == data.size());
  CHECK(fd.read(buf).size() == 0);
  fd.lseek(0, SEEK_SET);
  CHECK(fd.read(buf) == expected);
}

}  // namespace file
}  // namespace cc_wrapper
