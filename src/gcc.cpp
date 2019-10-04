#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include "config.h"
#include "flags.hpp"
#include "gcc.hpp"
#include "gcc/args.hpp"
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

  // Generate an unsanitized args list
  std::vector<nonstd::string_view> new_args;
  if (state.linking)
    new_args.push_back("-Wl,-g");
  if (util::isEnforcingPurity())
    new_args.push_back("-nostdinc");
  const bool position_independent = doHardenFlag(CC_VAR("PI"));
  if (position_independent) {
    new_args.push_back("-fPIC");
    if (state.linking && !state.produceShlib)
      new_args.push_back("-pie");
  }
  const bool no_strict_overflow = doHardenFlag(CC_VAR("NO_STRICT_OVERFLOW"));
  if (no_strict_overflow) {
    new_args.push_back("-fno-strict-overflow");
  }
  const bool fortify_source = doHardenFlag(CC_VAR("FORTIFY_SOURCE"));
  if (fortify_source) {
    new_args.push_back("-D_FORTIFY_SOURCE=2");
  }
  const bool stack_protector = doHardenFlag(CC_VAR("STACK_PROTECTOR"));
  if (stack_protector) {
    new_args.push_back("-fstack-protector-strong");
  }
  // TODO: -fstack-clash-protection
  const bool optimize = doHardenFlag(CC_VAR("OPTIMIZE"));
  if (optimize) {
    new_args.push_back("-O2");
  }
  const bool lto = doHardenFlag(CC_VAR("LTO"));
  if (lto) {
    new_args.push_back("-flto");
  }
  for (const auto &arg : args) {
    new_args.push_back(arg);
  }
  new_args.push_back("-B" TOOLDIR);
  if (state.stdinc) {
    flags::appendFromString(new_args, WRAPPER_CFLAGS);
    if (cxx && state.stdincxx)
      flags::appendFromString(new_args, WRAPPER_CXXFLAGS);
  }
  if (state.linking) {
    flags::appendFromString(new_args, WRAPPER_CFLAGS_LINK);
    if (cxx)
      flags::appendFromString(new_args, WRAPPER_CXXFLAGS_LINK);
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
