#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap_fwd_decl.h>
#include <vector>

namespace cc_wrapper {
namespace gcc {
namespace path {

void appendGood(std::vector<nonstd::string_view> &new_args,
                nonstd::span<const nonstd::string_view> old_args,
                nonstd::span<const nonstd::string_view> pure_prefixes,
                phmap::flat_hash_set<nonstd::string_view> &saved_includes);

}  // namespace path
}  // namespace gcc
}  // namespace cc_wrapper
