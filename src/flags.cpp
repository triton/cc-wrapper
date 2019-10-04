#include "flags.hpp"

namespace cc_wrapper {
namespace flags {

void appendFromString(std::vector<nonstd::string_view> &args,
                      nonstd::string_view flags) {
  size_t pos = -1;
  do {
    size_t lpos = pos + 1;
    pos = flags.find(' ', lpos);
    nonstd::string_view flag = flags.substr(lpos, pos - lpos);
    if (!flag.empty())
      args.push_back(flag);
  } while (pos != nonstd::string_view::npos);
}

}  // namespace flags
}  // namespace cc_wrapper
