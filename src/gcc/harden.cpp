#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include <config.h>
#include <gcc/harden.hpp>
#include <util.hpp>

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

Env getEnv() {
  Env env;
  env.position_independent = getEnvVar(CC_VAR("PI"));
  env.optimize = getEnvVar(CC_VAR("OPTIMIZE"));
  return env;
}

void appendFlags(std::vector<nonstd::string_view> &args, const Env &env,
                 const args::State &state) {
  if (env.position_independent) {
    args.push_back("-fPIC");
    if (state.linking && !state.produceShlib)
      args.push_back("-pie");
  }
  if (env.optimize) {
    args.push_back("-O2");
  }
}

}  // namespace harden
}  // namespace gcc
}  // namespace cc_wrapper
