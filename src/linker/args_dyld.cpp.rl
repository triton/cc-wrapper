#include "linker/args.hpp"

namespace cc_wrapper {
namespace linker {
namespace args {

// clang-format off
%%{
  machine dyld;

  action true { return true; }
  main := '-'? '-dynamic-linker' %/true '=' @true;

  write data;
}%%
// clang-format on

bool hasDynamicLinker(nonstd::span<const nonstd::string_view> args) {
  int cs;
  for (const auto &arg : args) {
    const char *p = arg.data();
    const char *pe = p + arg.size();
    const char *eof = pe;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
  }
  return false;
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
