#pragma once
#include <nonstd/optional.hpp>
#include <nonstd/span.hpp>
#include <string>

namespace cc_wrapper {
namespace file {

nonstd::optional<std::string> readlink(const char *path);

nonstd::span<char> read(int fd, nonstd::span<char> buf);
void lseek(int fd, off_t offset, int whence);

class Fd {
public:
  Fd(const char *path, int flags);
  Fd(int fd);
  Fd(const Fd &) = delete;
  Fd(Fd &&) = delete;
  Fd &operator=(const Fd &) = delete;
  Fd &operator=(Fd &&) = delete;
  ~Fd();

  inline operator int() { return fd; }

  inline nonstd::span<char> read(nonstd::span<char> buf) {
    return file::read(fd, buf);
  }
  inline void lseek(off_t offset, int whence) {
    file::lseek(fd, offset, whence);
  }

private:
  int fd;
};

}  // namespace file
}  // namespace cc_wrapper
