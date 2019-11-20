#include <gcc/args.hpp>

namespace cc_wrapper {
namespace gcc {
namespace args {

// clang-format off
%%{
  machine parse;

  cxx = 'c++' %{ state.cxx = true; };
  c = 'c' %{ state.cxx = false; };
  xopts = cxx | c;

  action xopts_next {
    if (++it >= args.end())
      fbreak;
    p = it->data();
    pe = p + it->size();
    eof = pe;
    fhold;
    fgoto xopts;
  }

  noLink = ( '-' . ('c'|'E'|'S'|'M'|'MM') ) %{ state.linking = false; };
  produceShlib = '-shared' %{ state.produceShlib = true; };
  stdinc = '-nostdinc' %{ state.stdinc = false; };
  stdincxx = '-nostdincxx' %{ state.stdincxx = false; };
  flagOnly = [^\-] @{ flagOnly = false; };
  parse = noLink | produceShlib | stdinc | stdincxx | flagOnly;
  lang = '-x' %/xopts_next xopts;

  main := lang | parse;

  write data;
}%%
// clang-format on

State parseState(nonstd::span<const nonstd::string_view> args, bool cxx) {
  int cs;
  bool flagOnly = true;
  State state;
  state.linking = true;
  state.produceShlib = false;
  state.stdinc = true;
  state.stdincxx = true;
  state.cxx = cxx;
  for (auto it = args.begin(); it < args.end(); ++it) {
    const char *p = it->data();
    const char *pe = p + it->size();
    const char *eof = pe;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
  }
  // If we have no file arguments we won't call the linker
  if (flagOnly)
    state.linking = false;
  return state;
}

}  // namespace args
}  // namespace gcc
}  // namespace cc_wrapper
