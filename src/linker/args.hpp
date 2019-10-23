#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include <linker/state.hpp>

namespace cc_wrapper {
namespace linker {
namespace args {

void parseLibs(state::Libs &libs, nonstd::span<const nonstd::string_view> args);
bool isLinking(nonstd::span<const nonstd::string_view> args);
void filterLinking(std::vector<nonstd::string_view> &new_args,
                   nonstd::span<const nonstd::string_view> old_args);
bool hasDynamicLinker(nonstd::span<const nonstd::string_view> args);
bool isDynamicLinking(nonstd::span<const nonstd::string_view> args);

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
