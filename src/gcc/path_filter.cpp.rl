#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>

#include "gcc/path.hpp"

namespace cc_wrapper {
namespace gcc {
namespace path {

// clang-format off
%%{
  machine filter;

  includes = 'isystem' | 'idirafter' | 'I';
  libs = [LB];
  main := '-' ( includes | libs ) @{ is_flag = true; fbreak; };

  write data;
}%%
// clang-format on

void
appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes) {
  int cs;
  nonstd::optional<nonstd::string_view> open_flag = nonstd::nullopt;
  for (const auto &arg : old_args) {
    if (open_flag) {
      if (isValid(arg, pure_prefixes)) {
        new_args.push_back(*open_flag);
        new_args.push_back(arg);
      }
      open_flag = nonstd::nullopt;
      continue;
    }

    const char *p = arg.data();
    const char *pe = p + arg.size();
    bool is_flag = false;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
    if (!is_flag) {
      new_args.push_back(arg);
      continue;
    }
    if (p == pe) {
      open_flag = arg;
      continue;
    }
    nonstd::string_view path(p, pe - p);
    if (isValid(path, pure_prefixes)) {
      new_args.push_back(arg);
    }
  }
}

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
