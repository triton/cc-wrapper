#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>

#include "../path.hpp"
#include "path.hpp"

namespace cc_wrapper {
namespace gcc {
namespace path {

// clang-format off
%%{
  machine filter;

  action open { open_flag = arg; }
  action path { path = nonstd::string_view(p, pe-p); fbreak; }

  includes = ( 'isystem' | 'idirafter' | 'I' ) @{ is_include = true; };
  libs = [LB];
  main := '-' ( includes | libs ) %/open any* >path;

  write data;
}%%
// clang-format on

void appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes,
                phmap::flat_hash_set<nonstd::string_view> &saved_includes) {
  int cs;
  nonstd::optional<nonstd::string_view> open_flag;
  bool is_include;
  for (const auto &arg : old_args) {
    if (open_flag) {
      if (cc_wrapper::path::isPure(arg, pure_prefixes)) {
        new_args.push_back(*open_flag);
        new_args.push_back(arg);
        if (is_include)
          saved_includes.insert(arg);
      }
      open_flag = nonstd::nullopt;
      continue;
    }

    const char *p = arg.data();
    const char *pe = p + arg.size();
    const char *eof = pe;
    nonstd::optional<nonstd::string_view> path;
    is_include = false;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
    if (open_flag)
      continue;
    if (path && !cc_wrapper::path::isPure(*path, pure_prefixes))
      continue;
    new_args.push_back(arg);
    if (path && is_include)
      saved_includes.insert(*path);
  }
}

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
