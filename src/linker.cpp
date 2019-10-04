#include <nonstd/string_view.hpp>
#include <vector>

#include "config.h"
#include "flags.hpp"
#include "generic.hpp"
#include "linker.hpp"

namespace cc_wrapper {
namespace linker {
namespace detail {

void filterCompilerFlags(std::vector<nonstd::string_view> &new_args,
                         nonstd::span<const nonstd::string_view> args) {
  bool replace_dyld = false;
  for (const auto &arg : new_args) {
    if (arg == "-dynamic-linker")
      replace_dyld = true;
  }
  bool flag_internal = true, skip_next = false;
  for (const auto &arg : args) {
    if (skip_next)
      skip_next = false;
    else if (arg == "-g")
      flag_internal = false;
    else if (flag_internal && arg == "-rpath")
      skip_next = true;
    else if (flag_internal && replace_dyld && arg == "-dynamic-linker")
      skip_next = true;
    else
      new_args.push_back(arg);
  }
}

bool isDynamicLinking(nonstd::span<const nonstd::string_view> args) {
  bool dynamic_context = true;
  for (const auto &arg : args) {
    if (arg == "-static" || arg == "-Bstatic")
      dynamic_context = false;
    else if (arg == "-Bdynamic")
      dynamic_context = true;
    else if (dynamic_context && arg == "-lc")
      return true;
  }
  return false;
}

}  // namespace detail

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args) {
  std::vector<nonstd::string_view> new_args;
  flags::appendFromVar(new_args, VAR_PREFIX "_LDFLAGS_BEFORE");
  flags::appendFromString(new_args, WRAPPER_LDFLAGS_BEFORE);
  detail::filterCompilerFlags(new_args, args);
  flags::appendFromVar(new_args, VAR_PREFIX "_LDFLAGS");
  flags::appendFromString(new_args, WRAPPER_LDFLAGS);
  if (detail::isDynamicLinking(args)) {
    flags::appendFromVar(new_args, VAR_PREFIX "_LDFLAGS_DYNAMIC");
    flags::appendFromString(new_args, WRAPPER_LDFLAGS_DYNAMIC);
  }
  return generic::main(info, new_args);
}

}  // namespace linker
}  // namespace cc_wrapper
