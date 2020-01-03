#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include <bins.hpp>

namespace cc_wrapper {
namespace gcc {

std::vector<nonstd::string_view>
rewriteArgs(nonstd::span<const nonstd::string_view> args, bool cxx,
            std::vector<nonstd::string_view> &saved_includes);

int ccMain(const bins::Info &info,
           nonstd::span<const nonstd::string_view> args);
int cxxMain(const bins::Info &info,
            nonstd::span<const nonstd::string_view> args);
int wrapperMain(const bins::Info &info,
                nonstd::span<const nonstd::string_view> args);

}  // namespace gcc
}  // namespace cc_wrapper
