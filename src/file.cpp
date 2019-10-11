#include <fcntl.h>
#include <sys/stat.h>
#include <system_error>
#include <unistd.h>

#include "file.hpp"

namespace cc_wrapper {
namespace file {

nonstd::optional<std::string> readlink(const char *path) {
  struct stat buf;
  if (::lstat(path, &buf) < 0)
    throw std::system_error(errno, std::generic_category(), "lstat");
  if ((buf.st_mode & S_IFMT) != S_IFLNK)
    return nonstd::nullopt;
  std::string ret(buf.st_size, '\0');
  ssize_t r = ::readlink(path, &ret[0], ret.size());
  if (r < 0)
    throw std::system_error(errno, std::generic_category(), "readlink");
  if (static_cast<size_t>(r) != ret.size())
    throw std::runtime_error("Bad readlink data");
  return ret;
}

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
