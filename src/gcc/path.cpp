#include <parallel_hashmap/phmap.h>

#include "../path.hpp"
#include "path.hpp"
#include "strings.hpp"

namespace cc_wrapper {
namespace gcc {
namespace path {

std::vector<std::string>
prefixMapFlags(nonstd::string_view flag,
               nonstd::span<const nonstd::string_view> dirs) {
  std::vector<std::string> ret;
  phmap::flat_hash_set<std::string> mapped;
  for (const auto &dir : dirs) {
    if (!cc_wrapper::path::isAbsolute(dir))
      continue;
    auto canonical = cc_wrapper::path::canonicalize(dir);
    bool included = false;
    for (auto it = mapped.begin(); it != mapped.end();) {
      if (cc_wrapper::path::startsWith(canonical, *it)) {
        included = true;
        break;
      }
      if (cc_wrapper::path::startsWith(*it, canonical))
        it = mapped.erase(it);
      else
        ++it;
    }
    if (!included) {
      mapped.insert(std::move(canonical));
    }
  }
  for (const auto &dir : mapped)
    ret.push_back(strings::cat("-f", flag, "=", dir, "=/no-such-path"));
  return ret;
}

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
