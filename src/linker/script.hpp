#pragma once
#include "linker/state.hpp"
#include <nonstd/string_view.hpp>

namespace cc_wrapper {
namespace linker {
namespace script {

namespace detail {

void addFileOrLib(state::Libs &libs, nonstd::string_view name,
                  nonstd::string_view cwd);

}  // namespace detail

bool isScript(int fd);
void parseLibs(state::Libs &libs, int fd, nonstd::string_view cwd);

}  // namespace script
}  // namespace linker
}  // namespace cc_wrapper
