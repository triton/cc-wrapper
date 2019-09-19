#include "linker.hpp"
#include "generic.hpp"

namespace cc_wrapper {
namespace linker {

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args) {
  return generic::main(info, args);
}

}  // namespace linker
}  // namespace cc_wrapper
