#include <fcntl.h>
#include <system_error>
#include <unistd.h>

#include "file.hpp"

namespace cc_wrapper {
namespace file {

Fd::Fd(const char *path, int flags) : Fd(open(path, flags)) {}

Fd::Fd(int fd) : fd(fd) {
  if (fd < 0)
    throw std::system_error(errno, std::generic_category(), "open");
}

Fd::~Fd() { close(fd); }

nonstd::span<char> read(int fd, nonstd::span<char> buf) {
  ssize_t r = ::read(fd, buf.data(), buf.size());
  if (r < 0)
    throw std::system_error(errno, std::generic_category(), "read");
  return buf.subspan(0, r);
}

void lseek(int fd, off_t offset, int whence) {
  if (::lseek(fd, offset, whence) < 0)
    throw std::system_error(errno, std::generic_category(), "lseek");
}

}  // namespace file
}  // namespace cc_wrapper
