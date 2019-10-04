#pragma once
#include <nonstd/string_view.hpp>
#include <vector>

namespace cc_wrapper {
namespace env {

namespace detail {

bool &enforcingPurity();
bool &debug();

}  // namespace detail

bool isDebug();
bool isEnforcingPurity();

std::vector<nonstd::string_view> purePrefixes();

}  // namespace env
}  // namespace cc_wrapper
