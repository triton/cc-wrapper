#include <linker/args.hpp>

namespace cc_wrapper {
namespace linker {
namespace args {

// clang-format off
%%{
  machine filter;

  # We only need to take care of flags that take arguments
  ls_nolink = 'rpath' | 'dynamic-linker' | ( 'plugin' '-opt'? );
  l_nolink = 'library-path';
  s_nolink = [LhImzTe] | 'soname' | 'Map';
  flag_nolink = ( '-'? ls_nolink ) | ( '-' l_nolink ) | s_nolink;
  nolink = '-' flag_nolink %/{ take_two = true; };
  flag_link = [lo];
  link = ( '-' flag_link %/{ ++it; continue; } | [^\-] ) @{ continue; };
  main := link | nolink;

  write data;
}%%
// clang-format on

void filterLinking(std::vector<nonstd::string_view> &new_args,
                   nonstd::span<const nonstd::string_view> old_args) {
  int cs;
  for (auto it = old_args.begin(); it < old_args.end(); ++it) {
    const char *p = it->data();
    const char *pe = p + it->size();
    const char *eof = pe;
    bool take_two = false;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
    if (take_two) {
      new_args.push_back(*it);
      if (++it >= old_args.end())
        break;
    }
    new_args.push_back(*it);
  }
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
