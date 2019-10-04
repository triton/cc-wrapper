#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

#include "bins.hpp"

namespace cc_wrapper {
namespace linker {
namespace detail {

void filterCompilerFlags(std::vector<nonstd::string_view> &new_args,
                         nonstd::span<const nonstd::string_view> args);
bool isDynamicLinking(nonstd::span<const nonstd::string_view> args);

}  // namespace detail

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args);

}  // namespace linker
}  // namespace cc_wrapper
