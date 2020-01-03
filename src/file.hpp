#pragma once
#include <nonstd/optional.hpp>
#include <nonstd/span.hpp>
#include <string>
#include <sys/types.h>

namespace cc_wrapper {
namespace file {

struct stat stat(const char *path);
bool exists(const char *path);
void rename(const char *oldpath, const char *newpath);
nonstd::optional<std::string> readlink(const char *path);
nonstd::optional<std::string> readlinkCanonicalized(const char *path);

nonstd::span<char> read(int fd, nonstd::span<char> buf);
void write(int fd, nonstd::span<const char> data);
void lseek(int fd, off_t offset, int whence);

class Fd {
public:
  Fd(const char *path, int flags);
  Fd(const char *path, int flags, mode_t mode);
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
  inline void write(nonstd::span<const char> data) { file::write(fd, data); }
  inline void lseek(off_t offset, int whence) {
    file::lseek(fd, offset, whence);
  }

private:
  int fd;
};

}  // namespace file
}  // namespace cc_wrapper
