#include "path.hpp"
#include "env.hpp"
#include "strings.hpp"

namespace cc_wrapper {
namespace path {

bool isValid(nonstd::string_view path,
             nonstd::span<const nonstd::string_view> pure_prefixes) {
  if (path.empty() || path[0] != '/')
    return true;
  if (strings::startsWith(path, "/no-such-path"))
    return false;
  if (!env::isEnforcingPurity())
    return true;
  for (const auto &prefix : pure_prefixes)
    if (strings::startsWith(path, prefix))
      return true;
  return false;
}

}  // namespace path
}  // namespace cc_wrapper
