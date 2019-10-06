#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

namespace cc_wrapper {
namespace linker {
namespace args {

bool hasDynamicLinker(nonstd::span<const nonstd::string_view> args);
bool isDynamicLinking(nonstd::span<const nonstd::string_view> args);

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
