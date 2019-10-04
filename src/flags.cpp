#include "flags.hpp"
#include "strings.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace flags {

void appendFromString(std::vector<nonstd::string_view> &args,
                      nonstd::string_view flags) {
  while (!flags.empty()) {
    auto prefix = strings::split(flags, ' ');
    if (!prefix.empty())
      args.push_back(prefix);
  }
}

void appendFromVar(std::vector<nonstd::string_view> &args, const char *var) {
  const auto maybeData = util::getenv(var);
  if (maybeData)
    appendFromString(args, *maybeData);
}

}  // namespace flags
}  // namespace cc_wrapper
