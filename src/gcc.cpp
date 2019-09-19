#include "gcc.hpp"
#include "generic.hpp"

namespace cc_wrapper {
namespace gcc {

int ccMain(const bins::Info &info,
           nonstd::span<const nonstd::string_view> args) {
  return generic::main(info, args);
}

int cxxMain(const bins::Info &info,
            nonstd::span<const nonstd::string_view> args) {
  return generic::main(info, args);
}

}  // namespace gcc
}  // namespace cc_wrapper
