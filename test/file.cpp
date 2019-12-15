#include <array>
#include <catch2/catch.hpp>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <file.hpp>
#include <util.hpp>

namespace cc_wrapper {
namespace file {

TEST_CASE("Exists", "[exists]") {
  constexpr char text[] = "test-file-text";
  unlink(text);
  CHECK(!exists(text));
  Fd(text, O_RDWR | O_CREAT);
  CHECK(exists(text));
  unlink(text);
}

TEST_CASE("Readlink works", "[readlink]") {
  CHECK_THROWS_AS(readlink("/no-such-path/cc-wrapper"), std::system_error);
  constexpr char dir[] = "test-file-dir";
  constexpr char link[] = "test-file-dir/test-file-link";
  constexpr char link2[] = "test-file-link2";
  constexpr char text[] = "test-file-dir/test-file-text";
  unlink(link);
  unlink(link2);
  unlink(text);
  rmdir(dir);
  CHECK(mkdir(dir, 0755) == 0);
  CHECK(symlink("test-file-text", link) == 0);
  CHECK(symlink("/no-such-path", link2) == 0);
  Fd(text, O_RDWR | O_CREAT);
  CHECK(readlink(text) == nonstd::nullopt);
  CHECK(readlink(link) == "test-file-text");
  CHECK(readlinkCanonicalized(text) == nonstd::nullopt);
  CHECK(readlinkCanonicalized(link) == text);
  CHECK(readlinkCanonicalized(link2) == "/no-such-path");
  unlink(link);
  unlink(link2);
  unlink(text);
  rmdir(dir);
}

TEST_CASE("Open file descriptor handles errors", "[open]") {
  CHECK_THROWS_AS(Fd("/no-such-path/cc-wrapper", O_RDONLY), std::system_error);
}

TEST_CASE("Open file descriptor works", "[open]") {
  int fdn;
  {
    Fd fd("/dev/null", O_RDONLY);
    fdn = fd;
    CHECK(fcntl(fd, F_GETFD) != -1);
    std::array<char, 1> buf;
    CHECK(fd.read(buf).empty());
  }
  CHECK(fcntl(fdn, F_GETFD) == -1);
  CHECK(errno == EBADF);
}

TEST_CASE("File descriptor basic ops", "[readwrite]") {
  nonstd::span<const char> expected = "hi";
  Fd fd(memfd_create("test", 0));
  std::array<char, 4> buf;
  CHECK(fd.read(buf).size() == 0);
  fd.write(expected);
  CHECK(fd.read(buf).size() == 0);
  fd.lseek(0, SEEK_SET);
  CHECK(util::spanEqual(fd.read(buf), expected));
}

}  // namespace file
}  // namespace cc_wrapper
