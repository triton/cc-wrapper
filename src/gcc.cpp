#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>
#include <vector>

#include "config.h"
#include "env.hpp"
#include "flags.hpp"
#include "gcc.hpp"
#include "gcc/args.hpp"
#include "gcc/harden.hpp"
#include "gcc/path.hpp"
#include "generic.hpp"
#include "strings.hpp"
#include "util.hpp"

#define CC_VAR(name) (VAR_PREFIX "_CC_" name)

namespace cc_wrapper {
namespace gcc {

namespace detail {

bool &isHardening() {
  static bool harden = util::getenv(CC_VAR("HARDEN")) != "";
  return harden;
}

bool doHardenFlag(nonstd::string_view flag) {
  nonstd::optional<nonstd::string_view> val = util::getenv(flag);
  if (val)
    return *val == "1";
  return isHardening();
}

}  // namespace detail

static int ccMainInternal(const bins::Info &info,
                          nonstd::span<const nonstd::string_view> args,
                          bool cxx) {
  const auto state = args::parseState(args);
  const auto harden_env = harden::getEnv();

  // Generate an unsanitized args list
  std::vector<nonstd::string_view> new_args;
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

  // Sanitize the final argument list for paths
  std::vector<nonstd::string_view> final_args;
  if (state.linking)
    final_args.push_back("-Wl,-g");
  if (env::isEnforcingPurity())
    final_args.push_back("-nostdinc");
  harden::appendFlags(final_args, harden_env, state);
  phmap::flat_hash_set<nonstd::string_view> saved_includes;
  path::appendGood(final_args, new_args, env::purePrefixes(), saved_includes);
#ifdef BUILD_DIR_ENV_VAR
  {
    auto val = util::getenv(BUILD_DIR_ENV_VAR);
    if (val)
      saved_includes.insert(*val);
  }
#endif
  std::vector<std::string> generated_flags;
  if (info.prefix_map_flag)
    for (const auto &include : saved_includes)
      generated_flags.push_back(strings::cat("-f", *info.prefix_map_flag, "=",
                                             include, "=/no-such-path"));
  for (const auto &flag : generated_flags)
    final_args.push_back(flag);

  return generic::main(info, final_args);
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
