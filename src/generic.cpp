#include <memory>
#include <nonstd/string_view.hpp>

#include "generic.hpp"
#include "strings.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace generic {

int main(const bins::Info &info, nonstd::span<const nonstd::string_view> args) {
  auto arg0 = strings::cat(util::dirname(info.abs_path), "/", info.name);
  const size_t nargs = 1 + args.size();
  std::unique_ptr<nonstd::string_view[]> final_args(
      new nonstd::string_view[nargs]);
  size_t iarg = 0;
  final_args[iarg++] = arg0;
  for (const auto &arg : args)
    final_args[iarg++] = arg;
  util::exec(info.abs_path, {final_args.get(), nargs});
  return 126;
}

}  // namespace generic
}  // namespace cc_wrapper
