#include <gcc/harden.hpp>

namespace cc_wrapper {
namespace gcc {
namespace harden {

// clang-format off
%%{
  machine filter;

  action ignore { continue; }
  maybe_no = 'no-'?;
  flag_prefix = '-f' maybe_no;
  pi_suffix = 'PIC' | 'PIE' | 'pic' | 'pie';
  pi = ( flag_prefix pi_suffix  | '-' maybe_no 'pie' )
      %{ if (env.position_independent) return false; };
  opt = '-O'[^3] @{ if (env.optimize) return false; };
  native = '-m' ( 'arch' | 'cpu' | 'tune' ) '=native' %{ return false; };
  main := pi | opt | native;

  write data;
}%%
// clang-format on

bool isValidFlag(nonstd::string_view arg, const Env &env) {
  int cs;
  const char *p = arg.data();
  const char *pe = p + arg.size();
  const char *eof = pe;
  // clang-format off
  %% write init;
  %% write exec;
  // clang-format on
  return true;
}

}  // namespace harden
}  // namespace gcc
}  // namespace cc_wrapper
