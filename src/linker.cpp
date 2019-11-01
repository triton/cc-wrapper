#include <fcntl.h>
#include <nonstd/string_view.hpp>
#include <string>
#include <vector>

#include <config.h>
#include <env.hpp>
#include <file.hpp>
#include <flags.hpp>
#include <generic.hpp>
#include <linker.hpp>
#include <linker/args.hpp>
#include <linker/compiler.hpp>
#include <linker/harden.hpp>
#include <linker/path.hpp>
#include <linker/script.hpp>
#include <linker/state.hpp>
#include <path.hpp>
#include <util.hpp>

namespace cc_wrapper {
namespace linker {

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args) {
  std::vector<nonstd::string_view> combined_args;
  flags::appendFromString(combined_args, WRAPPER_LDFLAGS_BEFORE);
  flags::appendFromVar(combined_args, VAR_PREFIX "_LDFLAGS_BEFORE");

  std::vector<nonstd::string_view> initial_args;
  compiler::filterFlags(initial_args, args,
                        args::hasDynamicLinker(combined_args));
  const auto harden_env = harden::getEnv();
  harden::appendFlags(combined_args, harden_env);
  harden::filterFlags(combined_args, initial_args, harden_env);

  flags::appendFromString(combined_args, WRAPPER_LDFLAGS);
  flags::appendFromVar(combined_args, VAR_PREFIX "_LDFLAGS");
  const bool dynamic = args::isDynamicLinking(initial_args);
  if (dynamic) {
    flags::appendFromString(combined_args, WRAPPER_LDFLAGS_DYNAMIC);
    flags::appendFromVar(combined_args, VAR_PREFIX "_LDFLAGS_DYNAMIC");
  }

  std::vector<nonstd::string_view> filtered_args;
  path::appendGood(filtered_args, combined_args, env::purePrefixes());

  std::vector<nonstd::string_view> final_args;
  state::Libs libs;
  if (dynamic && harden_env.add_rpath) {
    args::parseLibs(libs, filtered_args);
#ifdef BUILD_DIR_ENV_VAR
    auto build_dir = util::getenv(BUILD_DIR_ENV_VAR);
#endif
    for (const auto &rpath : libs.resolveRequiredRPaths()) {
#ifdef BUILD_DIR_ENV_VAR
      if (build_dir && !build_dir->empty() &&
          cc_wrapper::path::startsWith(rpath, *build_dir))
        continue;
#endif
      final_args.push_back("-rpath");
      final_args.push_back(rpath);
    }
  }
  if (!args::isLinking(args))
    args::filterLinking(final_args, filtered_args);
  else
    for (auto arg : filtered_args)
      final_args.push_back(arg);
  return generic::main(info, final_args);
}

}  // namespace linker
}  // namespace cc_wrapper
