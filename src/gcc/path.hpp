#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <string>
#include <vector>

namespace cc_wrapper {
namespace gcc {
namespace path {

void appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes,
                std::vector<nonstd::string_view> &saved_includes);

std::vector<std::string>
prefixMapFlags(nonstd::string_view flag,
               nonstd::span<const nonstd::string_view> dirs);

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
