#include "linker/harden.hpp"

namespace cc_wrapper {
namespace linker {
namespace harden {

void filterFlags(std::vector<nonstd::string_view> &new_args,
                 nonstd::span<const nonstd::string_view> old_args,
                 const Env &env) {
  (void)env;
  for (const auto &arg : old_args)
    new_args.push_back(arg);
}

}  // namespace harden
}  // namespace linker
}  // namespace cc_wrapper
