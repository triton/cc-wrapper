#include <cstring>
#include <nonstd/span.hpp>
#include <vector>

#include <path.hpp>

namespace cc_wrapper {
namespace path {

// clang-format off
%%{
  machine canonicalize;

  action remove_cur {
    memset(cur, '\0', 1+p-cur);
  }

  action remove_up {
    if (abs || !prev.empty()) {
      memset(cur, '\0', 1+p-cur);
    }
    if (!prev.empty()) {
      memset(prev.back().data(), '\0', prev.back().size());
      prev.pop_back();
    }
  }

  action add_prev {
    prev.push_back({cur, static_cast<size_t>(1+p-cur)});
  }

  sep = '/';
  curdir = ( '.' %/remove_cur )? sep @remove_cur;
  updir = '..' %/remove_up sep @remove_up;
  other = ( ( ^sep+ sep ) - ( curdir | updir ) ) @add_prev;
  base = ( curdir | updir | other ) >{ cur = p; };
  abs = sep @{ abs = true; };
  main := ( abs | (base - abs) ) base*;

  write data;
}%%
// clang-format on

void canonicalizeInPlace(std::string &path) {
  int cs;
  char *p = &path[0];
  char *pe = p + path.size();
  char *eof = pe;
  bool abs = false;
  std::vector<nonstd::span<char>> prev;
  char *cur = p;
  // clang-format off
  %% write init;
  %% write exec;
  // clang-format on
  size_t final_i = 0;

  for (const char c : path)
    if (c != '\0')
      path[final_i++] = c;
  path.resize(final_i);

  if (path.empty())
    path = ".";
  else if (path.size() > 1 && path.back() == '/')
    path.resize(path.size() - 1);
}

}  // namespace path
}  // namespace cc_wrapper
