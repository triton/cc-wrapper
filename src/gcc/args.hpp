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
  bool cxx;
};

State parseState(nonstd::span<const nonstd::string_view> args, bool cxx);

}  // namespace args
}  // namespace gcc
}  // namespace cc_wrapper
