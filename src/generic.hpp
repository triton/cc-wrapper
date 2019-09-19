#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

#include "bins.hpp"

namespace cc_wrapper {
namespace generic {

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args);

}  // namespace generic
}  // namespace cc_wrapper
