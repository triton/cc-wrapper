#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

#include <bins.hpp>

namespace cc_wrapper {
namespace linker {

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args);

}  // namespace linker
}  // namespace cc_wrapper
