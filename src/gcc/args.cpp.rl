#include "gcc/args.hpp"

namespace cc_wrapper {
namespace gcc {
namespace args {

// clang-format off
%%{
  machine parse;

  noLink = ( '-' . ('c'|'E'|'S'|'M'|'MM') ) %{ state.linking = false; };
  produceShlib = '-shared' %{ state.produceShlib = true; };
  stdinc = '-nostdinc' %{ state.stdinc = false; };
  stdincxx = '-nostdincxx' %{ state.stdincxx = false; };
  flagOnly = [^\-] @{ flagOnly = false; };
  main := noLink | produceShlib | stdinc | stdincxx | flagOnly;

  write data;
}%%
// clang-format on

State parseState(nonstd::span<const nonstd::string_view> args) {
  int cs;
  bool flagOnly = true;
  State state;
  state.linking = true;
  state.produceShlib = false;
  state.stdinc = true;
  state.stdincxx = true;
  for (const auto &arg : args) {
    const char *p = arg.data();
    const char *pe = p + arg.size();
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

}  // namespace gcc
}  // namespace cc_wrapper
}  // namespace cc_wrapper
