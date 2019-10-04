#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

namespace cc_wrapper {
namespace gcc {
namespace args {

struct State {
  bool linking;
  bool produceShlib;
  bool stdinc;
  bool stdincxx;
};

State parseState(nonstd::span<const nonstd::string_view> args);

}  // namespace args
}  // namespace gcc
}  // namespace cc_wrapper
