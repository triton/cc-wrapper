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
#include <linker/path.hpp>
#include <linker/script.hpp>
#include <linker/state.hpp>
#include <path.hpp>
#include <util.hpp>

namespace cc_wrapper {
namespace linker {

#ifdef DYNAMIC_LINKER
constexpr bool remove_dynamic_linker = true;
constexpr nonstd::string_view dynamic_linker = DYNAMIC_LINKER;
#else
constexpr bool remove_dynamic_linker = false;
constexpr nonstd::string_view dynamic_linker = "";
#endif

void maybeFilterLinking(std::vector<nonstd::string_view> &new_args,
                   nonstd::span<const nonstd::string_view> old_args, bool is_linking) {
  if (!is_linking)
    args::filterLinking(new_args, old_args);
  else
    for (auto arg : old_args)
      new_args.push_back(arg);
}

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args) {
  std::vector<nonstd::string_view> combined_args, combined_args_begin;
  if (!dynamic_linker.empty() && args::hasDynamicLinker(args)) {
    combined_args_begin.push_back("-dynamic-linker");
    combined_args_begin.push_back(dynamic_linker);
  }
  flags::appendFromString(combined_args_begin, WRAPPER_LDFLAGS_BEFORE);
  flags::appendFromVar(combined_args_begin, VAR_PREFIX "_LDFLAGS_BEFORE");

  std::vector<nonstd::string_view> initial_args;
  compiler::filterFlags(initial_args, args, remove_dynamic_linker);
  for (const auto &arg : initial_args)
    combined_args.push_back(arg);

  flags::appendFromString(combined_args, WRAPPER_LDFLAGS);
  flags::appendFromVar(combined_args, VAR_PREFIX "_LDFLAGS");
  const bool dynamic = args::isDynamicLinking(initial_args);
  if (dynamic) {
    flags::appendFromString(combined_args, WRAPPER_LDFLAGS_DYNAMIC);
    flags::appendFromVar(combined_args, VAR_PREFIX "_LDFLAGS_DYNAMIC");
  }

  std::vector<nonstd::string_view> filtered_args, filtered_args_begin;
  path::appendGood(filtered_args_begin, combined_args_begin, env::purePrefixes());
  path::appendGood(filtered_args, combined_args, env::purePrefixes());

  const bool is_linking = args::isLinking(args);
  std::vector<nonstd::string_view> final_args;
  maybeFilterLinking(final_args, filtered_args_begin, is_linking);
  state::Libs libs;
  const bool add_rpath =
      util::getenv(VAR_PREFIX "_LD_ADD_RPATH").value_or("1") == "1";
  if (dynamic && add_rpath) {
    args::parseLibs(libs, filtered_args_begin);
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
  maybeFilterLinking(final_args, filtered_args, is_linking);
  return generic::main(info, final_args);
}

}  // namespace linker
}  // namespace cc_wrapper
