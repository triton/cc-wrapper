#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include "config.h"
#include "gcc/harden.hpp"
#include "util.hpp"

#define CC_VAR(name) (VAR_PREFIX "_CC_" name)

namespace cc_wrapper {
namespace gcc {
namespace harden {

bool getEnvVar(nonstd::string_view flag) {
  nonstd::optional<nonstd::string_view> val = util::getenv(flag);
  if (val)
    return *val == "1";
  // We harden by default
  static bool harden = util::getenv(CC_VAR("HARDEN")) != "";
  return harden;
}

Env getEnv(const bins::GccInfo &info) {
  Env env;
  env.position_independent = getEnvVar(CC_VAR("PI"));
  env.no_strict_overflow = getEnvVar(CC_VAR("NO_STRICT_OVERFLOW"));
  env.fortify_source = getEnvVar(CC_VAR("FORTIFY_SOURCE"));
  env.stack_protector = getEnvVar(CC_VAR("STACK_PROTECTOR"));
  // TODO: env.stack_clash_protection
  env.optimize = getEnvVar(CC_VAR("OPTIMIZE"));
  env.lto = info.has_lto ? getEnvVar(CC_VAR("LTO")) : false;
  return env;
}

void appendFlags(std::vector<nonstd::string_view> &args, const Env &env,
                 const args::State &state) {
  if (env.position_independent) {
    args.push_back("-fPIC");
    if (state.linking && !state.produceShlib)
      args.push_back("-pie");
  }
  if (env.no_strict_overflow) {
    args.push_back("-fno-strict-overflow");
  }
  if (env.fortify_source) {
    args.push_back("-D_FORTIFY_SOURCE=2");
  }
  if (env.stack_protector) {
    args.push_back("-fstack-protector-strong");
  }
  // TODO: -fstack-clash-protection
  if (env.optimize) {
    args.push_back("-O2");
  }
  if (env.lto) {
    args.push_back("-flto");
  }
}

}  // namespace harden
}  // namespace gcc
}  // namespace cc_wrapper
