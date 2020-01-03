#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <string>
#include <vector>

#include <config.h>
#include <env.hpp>
#include <flags.hpp>
#include <fmt_sv.hpp>
#include <gcc.hpp>
#include <gcc/args.hpp>
#include <gcc/harden.hpp>
#include <gcc/path.hpp>
#include <generic.hpp>
#include <strings.hpp>
#include <util.hpp>

namespace cc_wrapper {
namespace gcc {

static int ccMainInternal(const bins::GccInfo &info,
                          nonstd::span<const nonstd::string_view> args,
                          bool cxx) {
  const auto state = args::parseState(args, cxx);
  const auto harden_env = harden::getEnv();

  // Generate an unsanitized args list
  std::vector<nonstd::string_view> new_args;
  // It's critical that c++ flags happen before c flags as they
  // often have include paths that need to be ordered before c ones
  if (state.cxx) {
    flags::appendFromString(new_args, WRAPPER_CXXFLAGS_BEFORE);
    flags::appendFromVar(new_args, VAR_PREFIX "_CXXFLAGS_BEFORE");
  }
  flags::appendFromString(new_args, WRAPPER_CFLAGS_BEFORE);
  flags::appendFromVar(new_args, VAR_PREFIX "_CFLAGS_BEFORE");
  for (const auto &arg : args)
    if (harden::isValidFlag(arg, harden_env))
      new_args.push_back(arg);
  if (state.stdinc) {
    if (state.cxx && state.stdincxx) {
      flags::appendFromString(new_args, WRAPPER_STDINCXX);
      flags::appendFromVar(new_args, VAR_PREFIX "_STDINCXX");
    }
    flags::appendFromString(new_args, WRAPPER_STDINC);
    flags::appendFromVar(new_args, VAR_PREFIX "_STDINC");
  }
  if (state.cxx) {
    flags::appendFromString(new_args, WRAPPER_CXXFLAGS);
    flags::appendFromVar(new_args, VAR_PREFIX "_CXXFLAGS");
  }
  flags::appendFromString(new_args, WRAPPER_CFLAGS);
  flags::appendFromVar(new_args, VAR_PREFIX "_CFLAGS");
  if (state.linking) {
    if (state.cxx) {
      flags::appendFromString(new_args, WRAPPER_CXXFLAGS_LINK);
      flags::appendFromVar(new_args, VAR_PREFIX "_CXXFLAGS_LINK");
    }
    flags::appendFromString(new_args, WRAPPER_CFLAGS_LINK);
    flags::appendFromVar(new_args, VAR_PREFIX "_CFLAGS_LINK");
  }

  // Sanitize the final argument list for paths
  std::vector<nonstd::string_view> final_args;
  if (state.linking)
    final_args.push_back("-Wl,-g");
  if (env::isEnforcingPurity())
    final_args.push_back("-nostdinc");
  final_args.push_back("-B" TOOLDIR);
  harden::appendFlags(final_args, harden_env, state);
  std::vector<nonstd::string_view> saved_includes;
  path::appendGood(final_args, new_args, env::purePrefixes(), saved_includes);
#ifdef BUILD_DIR_ENV_VAR
  {
    auto val = util::getenv(BUILD_DIR_ENV_VAR);
    if (val)
      saved_includes.push_back(*val);
  }
#endif
  std::vector<std::string> generated_flags;
  if (info.prefix_map_flag)
    generated_flags =
        path::prefixMapFlags(*info.prefix_map_flag, saved_includes);
  for (const auto &flag : generated_flags)
    final_args.push_back(flag);

  return generic::main(info, final_args);
}

int ccMain(const bins::Info &info,
           nonstd::span<const nonstd::string_view> args) {
  return ccMainInternal(static_cast<const bins::GccInfo &>(info), args, false);
}

int cxxMain(const bins::Info &info,
            nonstd::span<const nonstd::string_view> args) {
  return ccMainInternal(static_cast<const bins::GccInfo &>(info), args, true);
}

int wrapperMain(const bins::Info &info,
                nonstd::span<const nonstd::string_view> args) {
  std::vector<nonstd::string_view> final_args;
  final_args.push_back("-B" TOOLDIR);
  for (const auto arg : args)
    final_args.push_back(arg);
  return generic::main(info, final_args);
}

}  // namespace gcc
}  // namespace cc_wrapper
