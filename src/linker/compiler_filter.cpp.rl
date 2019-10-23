#include <linker/compiler.hpp>

namespace cc_wrapper {
namespace linker {
namespace compiler {

// clang-format off
%%{
  machine filter;

  action skip_one { skip_one = true; }
  action skip_two { skip_two = true; fbreak; }
  dynamic_linker = 'dynamic-linker' @{ dynamic_linker = true; };
  skippable = '-'? ( 'rpath' | dynamic_linker ) %/skip_two '=' any* >skip_one;
  main := '-' ( skippable | 'g' %/{ compiler_flags = false; skip_one = true; });

  write data;
}%%
// clang-format on

void filterFlags(std::vector<nonstd::string_view> &new_args,
                 nonstd::span<const nonstd::string_view> args,
                 bool remove_dynamic_linker) {
  int cs;
  bool compiler_flags = true, skip_two = false;
  for (const auto &arg : args) {
    if (!compiler_flags) {
      new_args.push_back(arg);
      continue;
    }
    if (skip_two) {
      skip_two = false;
      continue;
    }
    const char *p = arg.data();
    const char *pe = p + arg.size();
    const char *eof = pe;
    bool skip_one = false, dynamic_linker = false;
    skip_two = false;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
    if (dynamic_linker && !remove_dynamic_linker) {
      skip_one = false;
      skip_two = false;
    }
    if (skip_one || skip_two)
      continue;
    new_args.push_back(arg);
  }
}

}  // namespace compiler
}  // namespace linker
}  // namespace cc_wrapper
