#pragma once
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

namespace cc_wrapper {
namespace linker {
namespace harden {

struct Env {
  bool new_dtags;
  bool no_exec_stack;
  bool relro;
  bool bind_now;
};

Env getEnv();
void appendFlags(std::vector<nonstd::string_view> &args, const Env &env);
void filterFlags(std::vector<nonstd::string_view> &new_args,
                 nonstd::span<const nonstd::string_view> old_args,
                 const Env &env);

}  // namespace harden
}  // namespace linker
}  // namespace cc_wrapper
