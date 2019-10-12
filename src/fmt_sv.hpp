#pragma once
#include <fmt/core.h>
#include <fmt/format.h>
#include <nonstd/string_view.hpp>

namespace fmt {

template <typename Char>
struct formatter<nonstd::basic_string_view<Char>, Char>
    : formatter<basic_string_view<Char>, Char> {
  template <typename FormatContext>
  auto format(nonstd::basic_string_view<Char> val, FormatContext &ctx)
      -> decltype(ctx.out()) {
    return formatter<basic_string_view<Char>, Char>::format(
        {val.data(), val.size()}, ctx);
  }
};

}  // namespace fmt
