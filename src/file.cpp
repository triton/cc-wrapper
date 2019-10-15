#include <fcntl.h>
#include <sys/stat.h>
#include <system_error>
#include <unistd.h>

#include "file.hpp"
#include "path.hpp"
#include "strings.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace file {

bool exists(const char *path) {
  struct stat buf;
  int ret = ::stat(path, &buf);
  if (ret == 0)
    return true;
  if (errno == ENOENT)
    return false;
  throw std::system_error(errno, std::generic_category(), "stat");
}

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

nonstd::optional<std::string> readlinkCanonicalized(const char *path) {
  auto ret = readlink(path);
  if (ret) {
    if (ret->empty())
      *ret = ".";
    else if ((*ret)[0] != '/')
      *ret = strings::cat(util::dirname(path), "/", *ret);
    path::canonicalizeInPlace(*ret);
  }
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

void write(int fd, nonstd::span<const char> data) {
  ssize_t r = ::write(fd, data.data(), data.size());
  if (r < 0)
    throw std::system_error(errno, std::generic_category(), "write");
  if (static_cast<size_t>(r) != data.size())
    throw std::runtime_error("write truncated");
}

void lseek(int fd, off_t offset, int whence) {
  if (::lseek(fd, offset, whence) < 0)
    throw std::system_error(errno, std::generic_category(), "lseek");
}

}  // namespace file
}  // namespace cc_wrapper
