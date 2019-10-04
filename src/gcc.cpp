#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include "config.h"
#include "flags.hpp"
#include "gcc.hpp"
#include "gcc/args.hpp"
#include "gcc/harden.hpp"
#include "generic.hpp"
#include "util.hpp"

#define CC_VAR(name) (VAR_PREFIX "_CC_" name)

namespace cc_wrapper {
namespace gcc {

bool doHardenFlag(const char *flag) {
  nonstd::optional<nonstd::string_view> val = util::getenv(flag);
  if (val)
    return *val == "1";
  // We harden by default
  static bool harden = util::getenv(CC_VAR("HARDEN")) != "";
  return harden;
}

static int ccMainInternal(const bins::Info &info,
                          nonstd::span<const nonstd::string_view> args,
                          bool cxx) {
  const auto state = args::parseState(args);
  const auto harden_env = harden::getEnv();

  // Generate an unsanitized args list
  std::vector<nonstd::string_view> new_args;
  if (state.linking)
    new_args.push_back("-Wl,-g");
  if (util::isEnforcingPurity())
    new_args.push_back("-nostdinc");
  harden::appendFlags(new_args, harden_env, state);
  for (const auto &arg : args)
    if (harden::isValidFlag(arg, harden_env))
      new_args.push_back(arg);
  new_args.push_back("-B" TOOLDIR);
  flags::appendFromVar(new_args, VAR_PREFIX "_CFLAGS");
  if (cxx)
    flags::appendFromVar(new_args, VAR_PREFIX "_CXXFLAGS");
  if (state.stdinc) {
    flags::appendFromString(new_args, WRAPPER_CFLAGS);
    if (cxx && state.stdincxx)
      flags::appendFromString(new_args, WRAPPER_CXXFLAGS);
  }
  if (state.linking) {
    flags::appendFromVar(new_args, VAR_PREFIX "_CFLAGS_LINK");
    flags::appendFromString(new_args, WRAPPER_CFLAGS_LINK);
    if (cxx) {
      flags::appendFromVar(new_args, VAR_PREFIX "_CXXFLAGS_LINK");
      flags::appendFromString(new_args, WRAPPER_CXXFLAGS_LINK);
    }
  }

  return generic::main(info, new_args);
}

int ccMain(const bins::Info &info,
           nonstd::span<const nonstd::string_view> args) {
  return ccMainInternal(info, args, false);
}

int cxxMain(const bins::Info &info,
            nonstd::span<const nonstd::string_view> args) {
  return ccMainInternal(info, args, true);
}

}  // namespace gcc
}  // namespace cc_wrapper
