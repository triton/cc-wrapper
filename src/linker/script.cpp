#include <array>
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

#include <file.hpp>
#include <linker/script.hpp>
#include <path.hpp>
#include <strings.hpp>

namespace cc_wrapper {
namespace linker {
namespace script {

namespace detail {

void addFileOrLib(state::Libs &libs, nonstd::string_view name,
                  nonstd::string_view cwd) {
  if (name.empty())
    return;
  if (name.substr(0, 2) == "-l")
    libs.addLib(name.substr(2));
  else if (path::isAbsolute(name))
    libs.addFile(name);
  else
    libs.addFile(strings::cat(cwd, "/", name));
}

}  // namespace detail

bool isScript(int fd) {
  constexpr nonstd::span<const char> hdr = nonstd::string_view("/*");
  std::array<char, hdr.size()> buf;
  auto data = file::read(fd, buf);
  file::lseek(fd, -data.size(), SEEK_CUR);
  return data == hdr;
}

}  // namespace script
}  // namespace linker
}  // namespace cc_wrapper
