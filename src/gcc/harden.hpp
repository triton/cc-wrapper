#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include "gcc/args.hpp"

namespace cc_wrapper {
namespace gcc {
namespace harden {

struct Env {
  bool position_independent;
  bool no_strict_overflow;
  bool fortify_source;
  bool stack_protector;
  // bool stack_clash_protection;
  bool optimize;
  bool lto;
};

Env getEnv();
void appendFlags(std::vector<nonstd::string_view> &args, const Env &env,
                 const args::State &state);
bool isValidFlag(nonstd::string_view arg, const Env &env);

}  // namespace harden
}  // namespace gcc
}  // namespace cc_wrapper
