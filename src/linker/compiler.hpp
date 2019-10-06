#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

namespace cc_wrapper {
namespace linker {
namespace compiler {

void filterFlags(std::vector<nonstd::string_view> &new_args,
                 nonstd::span<const nonstd::string_view> args,
                 bool remove_dynamic_linker);

}  // namespace compiler
}  // namespace linker
}  // namespace cc_wrapper
