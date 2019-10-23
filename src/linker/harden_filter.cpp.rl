#include <linker/harden.hpp>

namespace cc_wrapper {
namespace linker {
namespace harden {

// clang-format off
%%{
  machine filter;

  now = ( 'lazy' | 'now' ) %/{ if (env.bind_now) skip = true; };
  relro = ( 'no'? 'relro' ) %/{ if (env.relro) skip = true; };
  execstack = ( 'no'? 'execstack' ) %/{ if (env.no_exec_stack) skip = true; };
  z := now | relro | execstack;

  action next_z {
    if (++it < old_args.end()) {
      p = it->data();
      pe = p + it->size();
      eof = pe;
      two_args = true;
      fhold;
      fgoto z;
    } else {
      --it;
    }
  }

  dtags = '-' ( 'disable' | 'enable' ) '-new-dtags' %/{ if (env.new_dtags) skip = true; };
  opts = 'z' %/next_z any* @{ fhold; fnext z; };
  main := '-' ( opts | dtags );

  write data;
}%%

void filterFlags(std::vector<nonstd::string_view> &new_args,
                 nonstd::span<const nonstd::string_view> old_args,
                 const Env &env) {
  int cs;
  for (auto it = old_args.begin(); it < old_args.end(); ++it) {
    const char *p = it->data();
    const char *pe = p + it->size();
    const char *eof = pe;
    bool skip = false, two_args = false;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
    if (skip) {
      continue;
    }
    if (two_args)
      new_args.push_back(*(it - 1));
    new_args.push_back(*it);
  }
}

}  // namespace harden
}  // namespace linker
}  // namespace cc_wrapper
