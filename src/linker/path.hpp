#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

namespace cc_wrapper {
namespace linker {
namespace path {

void appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes);

}  // namespace path
}  // namespace linker
}  // namespace cc_wrapper
