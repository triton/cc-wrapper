#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>

#include "bins.hpp"
#include "env.hpp"
#include "fmt_sv.hpp"
#include "gcc.hpp"
#include "generic.hpp"
#include "linker.hpp"
#include "strings.hpp"
#include "util.hpp"

namespace cc_wrapper {

nonstd::string_view getBinName(const char *arg) {
  if (arg == nullptr)
    throw std::invalid_argument("Binary name required");
  return util::basename(arg);
}

int main(int argc, char *argv[]) {
  try {
    auto bin_name = getBinName(argv[0]);
    if (strings::endsWith(bin_name, "cc-wrapper")) {
      argc--;
      argv++;
      bin_name = getBinName(argv[0]);
    }
    argc--;
    argv++;

    const auto &info = bins::getInfoMap().at(bin_name);

    size_t nargs = info.extra_args.size() + argc;
    std::unique_ptr<nonstd::string_view[]> args(new nonstd::string_view[nargs]);
    size_t iargs = 0;
    for (const auto &arg : info.extra_args)
      args[iargs++] = arg;
    for (int i = 0; i < argc; ++i)
      args[iargs++] = argv[i];
    nonstd::span<const nonstd::string_view> args_view(args.get(), nargs);

    if (env::isDebug()) {
      fmt::print(stderr, "cc-wrapper: Original arguments to `{}`:\n", bin_name);
      for (const auto &arg : args_view)
        fmt::print(stderr, "cc-wrapper:   {}\n", arg);
      fmt::print(stderr, "\n");
    }

    switch (info.type) {
    case bins::Type::GCC_COMPILER:
      return gcc::ccMain(info, args_view);
    case bins::Type::GXX_COMPILER:
      return gcc::cxxMain(info, args_view);
    case bins::Type::GCC_WRAPPER:
      return gcc::wrapperMain(info, args_view);
    case bins::Type::LINKER:
      return linker::main(info, args_view);
    case bins::Type::GENERIC:
      return generic::main(info, args_view);
    };
  } catch (const std::out_of_range &) {
    fmt::print(stderr, "cc-wrapper: Invalid binary name\n");
  } catch (const std::exception &e) {
    fmt::print(stderr, "cc-wrapper: {}\n", e.what());
  }
  return 127;
}

}  // namespace cc_wrapper
