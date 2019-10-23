#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include <config.h>
#include <linker/harden.hpp>
#include <util.hpp>

#define LD_VAR(name) (VAR_PREFIX "_LD_" name)

namespace cc_wrapper {
namespace linker {
namespace harden {

bool getEnvVar(nonstd::string_view flag) {
  nonstd::optional<nonstd::string_view> val = util::getenv(flag);
  if (val)
    return *val == "1";
  // We harden by default
  static bool harden = util::getenv(LD_VAR("HARDEN")) != "";
  return harden;
}

Env getEnv() {
  Env env;
  env.new_dtags = getEnvVar(LD_VAR("NEW_DTAGS"));
  env.no_exec_stack = getEnvVar(LD_VAR("NO_EXEC_STACK"));
  env.relro = getEnvVar(LD_VAR("RELRO"));
  env.bind_now = getEnvVar(LD_VAR("BIND_NOW"));
  env.add_rpath = util::getenv(LD_VAR("ADD_RPATH")).value_or("1") == "1";
  return env;
}

void appendFlags(std::vector<nonstd::string_view> &args, const Env &env) {
  if (env.new_dtags)
    args.push_back("--enable-new-dtags");
  if (env.no_exec_stack) {
    args.push_back("-z");
    args.push_back("noexecstack");
  }
  if (env.relro) {
    args.push_back("-z");
    args.push_back("relro");
  }
  if (env.bind_now) {
    args.push_back("-z");
    args.push_back("now");
  }
}

}  // namespace harden
}  // namespace linker
}  // namespace cc_wrapper
