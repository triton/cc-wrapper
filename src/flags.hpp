#pragma once
#include <nonstd/string_view.hpp>
#include <vector>

namespace cc_wrapper {
namespace flags {

void appendFromString(std::vector<nonstd::string_view> &args,
                      nonstd::string_view flags);

}  // namespace flags
}  // namespace cc_wrapper
