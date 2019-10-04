#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

namespace cc_wrapper {
namespace gcc {
namespace path {

bool isValid(nonstd::string_view path,
             nonstd::span<const nonstd::string_view> pure_prefixes);
void appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes);

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
