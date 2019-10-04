#include <unistd.h>

#include "config.h"
#include "flags.hpp"
#include "strings.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace env {

namespace detail {

bool &enforcingPurity() {
  static bool val = util::getenv(VAR_PREFIX "_ENFORCE_PURITY") == "1";
  return val;
}

bool &debug() {
  static bool val = util::getenv(VAR_PREFIX "_DEBUG") == "1";
  return val;
}

}  // namespace detail

bool isDebug() { return detail::debug(); }
bool isEnforcingPurity() { return detail::enforcingPurity(); }

std::vector<nonstd::string_view> purePrefixes() {
  std::vector<nonstd::string_view> pure_prefixes;
  flags::appendFromString(pure_prefixes, PURE_PREFIXES);
  nonstd::string_view pure_vars = PURE_PREFIX_ENV_VARS
#ifdef BUILD_DIR_ENV_VAR
      " " BUILD_DIR_ENV_VAR
#endif
      ;
  while (!pure_vars.empty()) {
    auto var = strings::split(pure_vars, ' ');
    if (!var.empty()) {
      auto val = util::getenv(var);
      if (val)
        pure_prefixes.push_back(*val);
    }
  }
  pure_prefixes.push_back(util::getenv("TMPDIR").value_or("/tmp"));
  return pure_prefixes;
}

}  // namespace env
}  // namespace cc_wrapper
