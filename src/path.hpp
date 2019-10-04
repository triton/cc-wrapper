#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

namespace cc_wrapper {
namespace path {

bool isValid(nonstd::string_view path,
             nonstd::span<const nonstd::string_view> pure_prefixes);

}  // namespace path
}  // namespace cc_wrapper
