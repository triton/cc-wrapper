#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

#include <bins.hpp>

namespace cc_wrapper {
namespace gcc {


int ccMain(const bins::Info &info,
           nonstd::span<const nonstd::string_view> args);
int cxxMain(const bins::Info &info,
            nonstd::span<const nonstd::string_view> args);
int wrapperMain(const bins::Info &info,
                nonstd::span<const nonstd::string_view> args);

}  // namespace gcc
}  // namespace cc_wrapper
