#include <env.hpp>
#include <path.hpp>
#include <strings.hpp>

namespace cc_wrapper {
namespace path {

PurityError::PurityError(nonstd::string_view path)
    : std::runtime_error(
          strings::cat("impure path cannot be skipped: ", path)) {}

bool isPure(nonstd::string_view path,
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

bool isAbsolute(nonstd::string_view path) {
  return !path.empty() && path[0] == '/';
}

std::string canonicalize(nonstd::string_view path) {
  auto ret = path.to_string();
  canonicalizeInPlace(ret);
  return ret;
}

bool startsWith(nonstd::string_view path, nonstd::string_view prefix) {
  if (!strings::startsWith(path, prefix))
    return false;
  if (prefix.size() == 1 || path.size() == prefix.size())
    return true;
  return path[prefix.size()] == '/';
}

}  // namespace path
}  // namespace cc_wrapper
