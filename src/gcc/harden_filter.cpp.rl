#include "gcc/harden.hpp"

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
  nso = flag_prefix 'strict-overflow'
      %{ if (env.no_strict_overflow) return false; };
  action fsa { if (env.fortify_source) return false; }
  fs = /-[UD]_FORTIFY_SOURCE/ %fsa | /-D_FORTIFY_SOURCE=/ @fsa;
  sp = flag_prefix 'stack-protector'
      @{if (env.stack_protector) return false; };
  opt = '-O' @{ if (env.optimize) return false; };
  lto = flag_prefix 'lto' %{ if (env.lto) return false; };
  native = '-m' ( 'arch' | 'tune' ) '=native' %{ return false; };
  main := pi | nso | fs | sp | opt | lto | native;

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

}  // namespace cc_wrapper
}  // namespace gcc
}  // namespace cc_wrapper
