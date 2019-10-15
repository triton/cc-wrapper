#include "linker/args.hpp"

namespace cc_wrapper {
namespace linker {
namespace args {

// clang-format off
%%{
  machine dyld;

  shared = 'shared' %/{ return true; };
  static = 'static' %/{ is_shared = false; };
  main := '-' ( shared | static );

  write data;
}%%
// clang-format on

bool isDynamicLinking(nonstd::span<const nonstd::string_view> args) {
  int cs;
  bool is_shared = true;
  for (const auto &arg : args) {
    const char *p = arg.data();
    const char *pe = p + arg.size();
    const char *eof = pe;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
  }
  return is_shared;
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
