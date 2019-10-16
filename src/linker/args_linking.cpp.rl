#include "linker/args.hpp"

namespace cc_wrapper {
namespace linker {
namespace args {

// clang-format off
%%{
  machine linking;

  # We only need to take care of flags that take arguments
  ls = 'rpath' | 'dynamic-linker' | ( 'plugin' '-opt'? );
  l = 'library-path';
  s = [LhImzTe] | 'soname' | 'Map';
  flag = '-' ( ( '-'? ls ) | ( '-' l ) | s ) %/{ it++; };
  input = [^\-] | '-' [ol];
  main := input @{ return true; } | flag;

  write data;
}%%
// clang-format on

bool isLinking(nonstd::span<const nonstd::string_view> args) {
  int cs;
  for (auto it = args.begin(); it < args.end(); ++it) {
    const char *p = it->data();
    const char *pe = p + it->size();
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
