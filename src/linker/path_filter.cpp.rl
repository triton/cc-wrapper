#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>

#include "../path.hpp"
#include "path.hpp"

#include "fmt_sv.hpp"
#include <fmt/format.h>

namespace cc_wrapper {
namespace linker {
namespace path {

// clang-format off
%%{
  machine filter;

  action open { open_flag = arg; }
  action path { path = nonstd::string_view(p, pe-p); fbreak; }
  action ignore { check = false; }

  plugin = ( 'plugin' '-opt'? ) @ignore;
  short_long = '-'? ( 'dynamic-linker' | 'rpath' | plugin );
  long = '-' ( 'library-path' );
  long_combined = ( short_long | long ) %/open '=' any* >path;
  short = [IL] %/open any* >path;
  flag = '-' ( long_combined | short );
  path = [^\-] @path;
  main := flag | path;

  write data;
}%%
// clang-format on

void appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes) {
  int cs;
  bool check;
  nonstd::optional<nonstd::string_view> open_flag;
  for (const auto &arg : old_args) {
    if (open_flag) {
      if (!check || cc_wrapper::path::isValid(arg, pure_prefixes)) {
        new_args.push_back(*open_flag);
        new_args.push_back(arg);
      }
      open_flag = nonstd::nullopt;
      continue;
    }
    const char *p = arg.data();
    const char *pe = p + arg.size();
    const char *eof = pe;
    check = true;
    nonstd::optional<nonstd::string_view> path;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
    fmt::print(stderr, "arg={} open_flag={} path={} check={}\n", arg,
               open_flag.value_or("none"), path.value_or("none"), check);
    if (open_flag)
      continue;
    if (path && check && !cc_wrapper::path::isValid(*path, pure_prefixes))
      continue;
    new_args.push_back(arg);
  }
}

}  // namespace path
}  // namespace linker
}  // namespace cc_wrapper
