#include <array>
#include <vector>

#include "file.hpp"
#include "linker/script.hpp"

namespace cc_wrapper {
namespace linker {
namespace script {

enum class Level {
  Unknown,
  Group,
};

// clang-format off
%%{
  machine libs;

  action level_up {
    if (name == "GROUP" || name == "INPUT") {
      in_input++;
      level.push_back(Level::Group);
    } else {
      level.push_back(Level::Unknown);
    }
    name.clear();
  }

  action level_down {
    if (in_input > 0)
      detail::addFileOrLib(libs, name, cwd);
    name.clear();
    if (level.back() == Level::Group)
      in_input--;
    level.pop_back();
  }

  action name_next {
    if (in_input > 0)
      detail::addFileOrLib(libs, name, cwd);
    name.clear();
  }

  comment = '/*' ( any* - '*/' ) '*/';
  sep = '(' @level_up |  ')' @level_down | space;
  token = sep+ ^sep* >name_next ${ name += *p; };
  main := ( comment | token )*;

  write data;
}%%
// clang-format on

void parseLibs(state::Libs &libs, int fd, nonstd::string_view cwd) {
  int cs;
  // clang-format off
  %% write init;
  // clang-format on
  std::array<char, 4096> buf;
  std::string name;
  size_t in_input = false;
  std::vector<Level> level;
  while (true) {
    auto data = file::read(fd, buf);
    const char *p = data.data();
    const char *pe = p + data.size();
    const char *eof = nullptr;
    if (data.empty())
      eof = pe;
    // clang-format off
    %% write exec;
    // clang-format on
    if (data.empty())
      return;
  }
}

}  // namespace script
}  // namespace linker
}  // namespace cc_wrapper
